#!/usr/bin/env python3
import argparse
import sys
import time

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


def main():
    parser = argparse.ArgumentParser(description='Show a typegraph')
    parser.add_argument('files', type=str, nargs='+')
    parser.add_argument('--numbers', action='store_true', help='Show graph IDs')
    parser.add_argument('--time', action='store_true', help='Report time')
    parser.add_argument('--output-dot', type=str, default=None)
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
    if args.time: timer.report(f'Callgraph loaded ({graph.callgraph.num_vertices()} vertices, {graph.callgraph.num_edges()} edges)')
    if args.output_dot:
        dot = graph.callgraph_to_dot(include_number=args.numbers)
        with open(args.output_dot, 'w') as f:
            f.write(dot)
        if args.time: timer.report('Wrote dot file')
    graph.callgraph_build_dot(output=output_file, include_number=args.numbers)
    if args.time: timer.report('Dot finished')


if __name__ == '__main__':
    main()
