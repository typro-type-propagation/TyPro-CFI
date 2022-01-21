#!/usr/bin/env python3
import argparse
import os
import sys
import time
from typing import Set

from typegraph_utils import Typegraph, TYPEGRAPH_TOOL


# OPTIONS:
# --all
# --numbers

class Timer:
    def __init__(self):
        self.start = time.time()

    def report(self, msg):
        t = time.time() - self.start
        print(f'[{t:5.1f}s]  {msg}')


def print_type_relations(graph: Typegraph):
    #t1, t2 = 'void (*)(struct cv *)', 'struct hv *'
    t1, t2 = 'struct _PerlIO *', 'struct sv *'
    #t1, t2 = 'struct _PerlIO *', 'void *'
    print(f'----- Searching: {t1} -> {t2} -----')
    found = 0
    for e in graph.graph.edges():
        if graph.edgeTypes[e] not in (0, 3):
            continue
        v1, v2 = e.source(), e.target()
        if graph.types[v1] == t1 and graph.types[v2] == t2:
            print(f'- FOUND: {v1} [{graph.types[v1]} / {graph.contexts[v1]}] --> {v2} [{graph.types[v2]} / {graph.contexts[v2]}]')
            found += 1
        if graph.types[v1] == t2 and graph.types[v2] == t1:
            print(f'- BACK:  {v1} [{graph.types[v1]} / {graph.contexts[v1]}] --> {v2} [{graph.types[v2]} / {graph.contexts[v2]}]')
    print('-'*60)
    if found == 0:
        sys.exit(1)
    sys.exit(0)


def get_nodes(graph: Typegraph, nodes) -> Set[int]:
    if nodes is None:
        return set()
    if isinstance(nodes, list):
        s = set()
        for item in nodes:
            for x in get_nodes(graph, item):
                s.add(x)
        return s
    nodes = nodes.strip()
    if nodes and os.path.exists(nodes):
        with open(nodes, 'r') as f:
            return get_nodes(graph, f.read())
    s = set()
    for part in nodes.split(','):
        if '/' in part:
            t, c = part.strip().split('/')
            v = graph.type_context_to_vertex[(t.strip(), c.strip())]
            assert v in graph.graph.vertices()
            s.add(v)
        else:
            s.add(int(part))
    return s


def main():
    parser = argparse.ArgumentParser(description='Show a typegraph')
    parser.add_argument('files', type=str, nargs='+')
    parser.add_argument('--all', action='store_true', help='Show all nodes (not only functions, calls and connected nodes)')
    parser.add_argument('--limit', type=int, default=None, help='A discovery limit for connected nodes')
    parser.add_argument('--numbers', action='store_true', help='Show graph IDs')
    parser.add_argument('--no-additional', action='store_true', help='Do not show additional names (type/context pairs)')
    parser.add_argument('--additional-types', action='store_true', help='Show additional types (but not contexts)')
    parser.add_argument('--time', action='store_true', help='Report time')
    parser.add_argument('--output-dot', type=str, default=None)
    parser.add_argument('--interesting-functions', type=str, default=None)
    parser.add_argument('--interesting-context', type=str, default=None)
    parser.add_argument('--interesting-numbers', type=str, default=None, help='Node numbers that should be initially considered')
    parser.add_argument('--interesting-external', action='store_true', help='All external nodes are interesting')
    parser.add_argument('--interesting-numbers-file', type=str, default=None, help='A file with interesting node numbers to start exploration from')
    parser.add_argument('--excluded-numbers', type=str, default=None, help='List of numbers to now draw (with interesting numbers only)')
    parser.add_argument('--mark-node', default=None, action='append', help='Mark this node (make it easier to find)')
    parser.add_argument('--mark-nodes-light', default=None, help='Mark nodes')
    parser.add_argument('--print-type-relations', default=False, action='store_true', help='Check for type relations and exit')
    args = parser.parse_args()

    input_files = []
    output_file = None
    for f in args.files:
        if f.endswith('.png') or f.endswith('.svg'):
            output_file = f
        else:
            input_files.append(f)
    assert len(input_files) == 1  # for now

    timer = Timer()
    graph = Typegraph.load_from_file(input_files[0])
    if args.time: timer.report(f'Graph loaded ({graph.graph.num_vertices()} vertices, {graph.graph.num_edges()} edges)')
    if args.print_type_relations:
        print_type_relations(graph)
    if not args.all:
        old_graph = graph
        if args.interesting_functions or args.interesting_context:
            functions = args.interesting_functions.split(',') if args.interesting_functions else []
            contexts = args.interesting_context.split(',') if args.interesting_context else []
            external = args.interesting_external
            def is_interesting(v: int) -> bool:
                if external and graph.external[v]:
                    return True
                # any cally in a named function
                calls = graph.callsPerVertex[v]
                for c in calls:
                    if any(c.endswith(' '+f) for f in functions):
                        return True
                # any use of that function
                if any(f in graph.functionUsages[v] for f in functions):
                    return True
                if any(c in graph.contexts[v] for c in contexts):
                    return True
                if contexts:
                    for namecontext in graph.additionalNames[v]:
                        if any(namecontext.endswith('\t'+c) for c in contexts):
                            return True
                return False
            graph = graph.get_interesting_subgraph(interesting_cb=is_interesting, limit=args.limit)
        elif args.interesting_numbers:
            # interesting_numbers = set(int(s.strip()) for s in args.interesting_numbers.split(','))
            # excluded_numbers = set(int(s.strip()) for s in args.excluded_numbers.split(',')) if args.excluded_numbers else None
            interesting_numbers = get_nodes(graph, args.interesting_numbers)
            excluded_numbers = get_nodes(graph, args.excluded_numbers) if args.excluded_numbers else None
            def is_interesting(v: int):
                return v in interesting_numbers
            graph = graph.get_interesting_subgraph(interesting_cb=is_interesting, limit=args.limit, excluded_numbers=excluded_numbers)
        elif args.interesting_external:
            def is_interesting(v: int) -> bool:
                if graph.external[v]:
                    return True
                if len(graph.callsPerVertex[v]) > 0 or len(graph.functionUsages[v]) > 0:
                    return True
                return False
            graph = graph.get_interesting_subgraph(interesting_cb=is_interesting, limit=args.limit)
        elif args.interesting_numbers_file:
            with open(args.interesting_numbers_file, 'r') as f:
                numbers = [s.strip() for s in f.read().split(',')]
                numbers = [int(s) for s in numbers if s]
            def is_interesting(v: int) -> bool:
                return v in numbers
            graph = graph.get_interesting_subgraph(interesting_cb=is_interesting, limit=args.limit)
        else:
            graph = graph.get_interesting_subgraph(limit=args.limit)
        if args.time: timer.report(f'Subgraph computed ({graph.graph.num_vertices()} / {old_graph.graph.num_vertices()} vertices, {graph.graph.num_edges()} / {old_graph.graph.num_edges()} edges)')
    if args.mark_nodes_light:
        for v in get_nodes(graph, args.mark_nodes_light):
            graph.dotVertexAttributes[v] = {
                'fillcolor': 'lightblue',
                'style': 'filled'
            }
    if args.mark_node:
        for v in get_nodes(graph, args.mark_node):
            graph.dotVertexAttributes[v] = {
                'fillcolor': 'goldenrod1',
                'style': 'filled',
                'color': 'goldenrod4',
                'penwidth': '5',
            }
    if args.output_dot:
        dot = graph.to_dot(show_calls_and_functions=True, include_number=args.numbers, include_additional_names=not args.no_additional, include_additional_types=args.additional_types)
        with open(args.output_dot, 'w') as f:
            f.write(dot)
        if args.time: timer.report('Wrote dot file')
    try:
        graph.build_dot(output=output_file, show_calls_and_functions=True, include_number=args.numbers, include_additional_names=not args.no_additional, include_additional_types=args.additional_types)
    except KeyboardInterrupt:
        pass
    if args.time: timer.report('Dot finished')


if __name__ == '__main__':
    main()
