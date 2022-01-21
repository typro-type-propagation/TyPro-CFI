import glob
import html
import json
import os
import random
import string
import subprocess
import urllib.parse
from collections import defaultdict

from graph_tool.all import *
from typing import Dict, Optional, Set, List, Tuple, Union, Callable, NamedTuple
import copy

TMPDIR = '/tmp/typegraph_tests'
os.makedirs(TMPDIR, exist_ok=True)


def find_tool(name, subdir='bin'):
    basedir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    llvm_dir = os.path.join(basedir, 'llvm-typro')
    build_dirs = list(glob.glob(os.path.join(llvm_dir, 'cmake-build-*'))) + [os.path.join(basedir, 'build'), os.path.join(llvm_dir, 'build')]
    candidates = [os.path.join(dir, subdir, name) for dir in build_dirs if os.path.exists(os.path.join(dir, subdir, name))]
    candidates.sort(key=lambda fname: os.path.getmtime(fname), reverse=True)
    assert len(candidates) > 0, f'Tool "{name}" not found!'
    print(f'{name}: "{candidates[0]}"')
    return candidates[0]


CLANG = find_tool('clang')
TYPEGRAPH_TOOL = find_tool('llvm-typegraph')
SYSROOT = None
CFLAGS = []
CXXFLAGS = []
LDFLAGS = []
RUN_PREFIX = []
ARCH = {
    'name': 'native',
    'fileident': 'ELF 64-bit LSB executable, x86-64',
    'signal': -4,
    'support_icfi': True
}

ARCHS = {
    'aarch64': {
        'name': 'aarch64',
        'flags': ['--target=aarch64-linux-gnu'],
        'target': 'aarch64-linux-gnu',
        'fileident': 'ELF 64-bit LSB executable, ARM aarch64',
        'signal': -5,
        'support_icfi': True
    },
    'mips64el': {
        'name': 'mips64el',
        'flags': ['--target=mips64el-linux-gnuabi64', '-fPIC'],
        'ldflags': ['-Wl,-z,notext'],
        'target': 'mips64el-linux-gnuabi64',
        'fileident': 'ELF 64-bit LSB executable, MIPS, MIPS64 rel2 version 1 (SYSV)',
        'signal': -5,
        'support_icfi': False
    }
}


def set_arch(name: str):
    global SYSROOT, ARCHS, ARCH, CFLAGS, CXXFLAGS, LDFLAGS, RUN_PREFIX
    ARCH = ARCHS[name]
    target = ARCH['target']
    print(f'=== USING TARGET ARCH {name} ({target}) ===')
    basedir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    SYSROOT = os.path.join(basedir, 'sysroots', target)
    assert os.path.exists(SYSROOT)
    # found with: qemu-aarch64 -L ../sysroots/aarch64-linux-gnu/ /tmp/sysroots/aarch64-linux-gnu/usr/bin/cpp -v /dev/null -o /dev/null
    gcc = 10
    CFLAGS = ARCH['flags'] + [
        '--sysroot', SYSROOT,
        '-isystem', f'{SYSROOT}/usr/lib/gcc/{target}/{gcc}/include',
        '-isystem', f'{SYSROOT}/usr/include/{target}',
        '-isystem', f'{SYSROOT}/usr/include',
    ]
    CXXFLAGS = ARCH['flags'] + [
        '--sysroot', SYSROOT,
        '-isystem', f'{SYSROOT}/usr/include/c++/{gcc}',
        '-isystem', f'{SYSROOT}/usr/include/{target}/c++/{gcc}',
        '-isystem', f'{SYSROOT}/usr/lib/gcc/{target}/{gcc}/include',
        '-isystem', f'{SYSROOT}/usr/include/{target}',
        '-isystem', f'{SYSROOT}/usr/include',
    ]
    LDFLAGS = ARCH['flags'] + ARCH.get('ldflags', []) + [
        '--sysroot', SYSROOT,
        '-L', f'{SYSROOT}/usr/lib/{target}', '-L', f'{SYSROOT}/usr/lib/gcc/{target}/{gcc}',
        '-B', f'{SYSROOT}/usr/lib/{target}', '-B', f'{SYSROOT}/usr/lib/gcc/{target}/{gcc}'
    ]
    RUN_PREFIX = [f'qemu-{name}', '-L', SYSROOT]


if 'ARCH' in os.environ and os.environ['ARCH'] != 'native':
    set_arch(os.environ['ARCH'])
# set_arch('aarch64')
# set_arch('mips64el')


def random_string(l: int) -> str:
    return ''.join(random.choice(string.ascii_letters) for _ in range(l))


def dot_escape(s: str) -> str:
    return s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n')


def html_escape(s: str) -> str:
    return html.escape(s)


def escaped_list_with_br(lst: List[str]) -> str:
    current_line_size = 0
    buffer = ''
    for s in lst:
        if buffer:
            if current_line_size + len(s) > 150:
                buffer += ',<BR/>'
                current_line_size = 0
            else:
                buffer += ', '
        buffer += html_escape(s)
        current_line_size += len(s)
    return buffer


class EdgeType:
    SIMPLE_CAST = 0
    POINTS_TO = 1
    STRUCT_MEMBER = 2
    REACHABILITY = 3
    UNION_MEMBER = 4


class InterfaceDesc(NamedTuple):
    SymbolName: str
    ContextName: str
    IsFunction: bool
    IsVarArg: bool
    DoNotMinimize: bool
    IsDefined: bool
    Types: List[int]


class CallInfo(NamedTuple):
    V: int
    NumArgs: int
    AllVertices: List[int]
    IsResolvePoint: bool


class Typegraph:
    def __init__(self):
        self.graph = Graph()
        self.types = self.graph.new_vertex_property('string')
        self.contexts = self.graph.new_vertex_property('string')
        self.layers = self.graph.new_vertex_property('int')
        self.additionalNames = self.graph.new_vertex_property('vector<string>')
        self.type_context_to_vertex: Dict[Tuple[str, str], int] = {}
        self.functionUsages = self.graph.new_vertex_property('vector<string>')
        self.usedFunctions: Dict[str, Set[int]] = {}  # function symbol => types this appears as
        self.callsPerVertex = self.graph.new_vertex_property('vector<string>')
        self.calls: Dict[str, CallInfo] = {}
        self.external = self.graph.new_vertex_property('bool')
        self.edgeTypes = self.graph.new_edge_property('int')
        self.edgeOffsets = self.graph.new_edge_property('int')
        self.edgeUnionTypes = self.graph.new_edge_property('string')
        self.interfaces: List[InterfaceDesc] = []
        self.dotVertexAttributes: Dict[int, Dict] = defaultdict(dict)
        self.contextDefiningUnits: Dict[str, str] = {}
        self.dotAdditional: List[str] = []

        self.callgraph = Graph()
        self.context_to_callgraph_vertex: Dict[str, int] = {}
        self.callgraph_contexts = self.callgraph.new_vertex_property('vector<string>')

    def __getitem__(self, item) -> int:
        return self.type_context_to_vertex[item]

    def rename_call(self, old: str, new: str):
        for v in self.graph.vertices():
            if self.contexts[v] == old:
                self.contexts[v] = new
                self.type_context_to_vertex[(self.types[v], new)] = v
            # skip additional names for now
            if self.callsPerVertex[v]:
                self.callsPerVertex[v] = [new if c == old else c for c in self.callsPerVertex[v]]
        if old in self.calls:
            self.calls[new] = self.calls[old]

    def rename_type(self, old: str, new: str):
        for v in self.graph.vertices():
            if self.types[v] == old:
                self.types[v] = new
                self.type_context_to_vertex[new, self.contexts[v]] = v

    def is_reachable_default(self, v_from: int, v_to: int) -> bool:
        discovered = []

        class ReachableVisitor(BFSVisitor):
            def discover_vertex(self, u):
                if u == v_to:
                    discovered.append(u)

        g = GraphView(self.graph, efilt=lambda e: self.edgeTypes[e] == EdgeType.SIMPLE_CAST or self.edgeTypes[e] == EdgeType.REACHABILITY)
        bfs_search(g, v_from, ReachableVisitor())
        return len(discovered) > 0

    def get_call_first(self, filter: Callable[[str], bool] = lambda x: True) -> Optional[str]:
        for c in self.calls:
            if filter(c):
                return c
        return None

    def get_call_first_functions(self, filter: Callable[[str], bool] = lambda x: True) -> List[str]:
        c = self.get_call_first(filter)
        assert c, 'No indirect call found'
        functions = set()
        for V in self.calls[c].AllVertices:
            # print(V, self.functionUsages[V])
            functions.update(self.functionUsages[V])
        return list(sorted(functions))

    def get_interfaces_for_symbol(self, name: str) -> List[InterfaceDesc]:
        return [i for i in self.interfaces if i.SymbolName == name]

    def is_interface_defined(self, name: str) -> bool:
        return any(interface.IsDefined and interface.SymbolName == name for interface in self.interfaces)

    def __is_interesting(self, v):
        return len(self.callsPerVertex[v]) > 0 or len(self.functionUsages[v]) > 0

    def get_interesting_subgraph(self, interesting_cb=None, limit=None, excluded_numbers=None) -> 'Typegraph':
        if not interesting_cb:
            interesting_cb = self.__is_interesting
        is_interesting = self.graph.new_vertex_property('bool')
        dist = self.graph.new_vertex_property("int")
        undirected_view = GraphView(self.graph, directed=False)

        class InterestingVisitor(BFSVisitor):
            def discover_vertex(self, u):
                if limit is None or dist[u] <= limit:
                    if excluded_numbers is None or u not in excluded_numbers:
                        is_interesting[u] = True

            def tree_edge(self, e):
                dist[e.target()] = dist[e.source()] + 1

        explore = []
        for v in self.graph.vertices():
            if interesting_cb(v) and (excluded_numbers is None or v not in excluded_numbers):
                is_interesting[v] = True
                if limit is None or limit > 0:
                    explore.append(v)
        for v in explore:
            dist[v] = 0
            bfs_search(undirected_view, v, InterestingVisitor())
        # stats
        tg = copy.copy(self)
        tg.graph = GraphView(self.graph, vfilt=lambda v: is_interesting[v])
        # TODO update dicts
        return tg

    def to_dot(self, show_calls_and_functions=True, include_number=False, include_additional_names=False, include_additional_types=False) -> str:
        lines = ['digraph pdg {']
        # for v in range(self.graph.num_vertices()):
        for v in self.graph.vertices():
            l = html_escape(self.types[v]) + "<BR />" + html_escape(self.contexts[v])
            if self.layers[v] != 0:
                l += ' L'+str(self.layers[v])
            if include_number:
                l = f'#{v} - {l}'
            if include_additional_names:
                names = self.additionalNames[v]
                for name in names:
                    l += '<BR /><sub>' + name.replace('\t', ' / ') + '</sub>'
            elif include_additional_types:
                seen = {self.types[v]}
                names = self.additionalNames[v]
                for name in names:
                    t = name.split('\t')[0]
                    if t not in seen:
                        l += '<BR /><sub>' + name.replace('\t', ' / ') + '</sub>'
                        seen.add(t)

            attribs = {}
            if show_calls_and_functions:
                funcs = self.functionUsages[v]
                calls = self.callsPerVertex[v]
                external = self.external[v]
                if external:
                    attribs['fillcolor'] = 'lightskyblue1'
                    attribs['style'] = 'filled'
                if funcs:
                    l += '<BR /><i>uses ' + escaped_list_with_br(funcs) + '</i>'
                    attribs['fillcolor'] = 'darkolivegreen1'
                    attribs['style'] = 'filled'
                if calls:
                    l += '<BR /><i>called by ' + escaped_list_with_br(calls) + '</i>'
                    attribs['color'] = 'darkorange3'
                    attribs['penwidth'] = '3'
                if funcs and external:
                    l += '<br /><I>(external)</I>'
            if v in self.dotVertexAttributes:
                attribs.update(self.dotVertexAttributes[v])
            attribs = ' '.join(f'{k}=\"{v}\"' for k, v in attribs.items())
            lines.append(f'v{v} [id=v{v} label=<{l}> {attribs}];')
        for e in self.graph.edges():
            attribs = {'label': ''}
            ety = self.edgeTypes[e]
            if ety == EdgeType.SIMPLE_CAST:
                pass
            elif ety == EdgeType.POINTS_TO:
                attribs['label'] = '*'
                attribs['style'] = 'dashed'
            elif ety == EdgeType.STRUCT_MEMBER:
                attribs['label'] = '.' + str(self.edgeOffsets[e])
                attribs['style'] = 'dotted'
            elif ety == EdgeType.UNION_MEMBER:
                attribs['label'] = '.' + self.edgeUnionTypes[e]
                attribs['style'] = 'dotted'
            elif ety == EdgeType.REACHABILITY:
                attribs['color'] = 'gray'
            else:
                attribs['label'] = f'[type {ety}]'

            attribs = ' '.join(f'{k}=\"{v}\"' for k, v in attribs.items())
            lines.append(f'v{e.source()} -> v{e.target()} [{attribs}];')
        lines += self.dotAdditional
        lines.append('}')
        return '\n'.join(lines)

    def to_dot_paper_style(self) -> str:
        lines = ['digraph pdg {']
        # for v in range(self.graph.num_vertices()):
        for v in self.graph.vertices():
            l = f'#{v}<BR />' + html_escape(self.types[v]) + "<BR />" + html_escape(self.contexts[v])
            if self.layers[v] != 0:
                l += ' L'+str(self.layers[v])

            attribs = {'shape': 'box'}
            funcs = self.functionUsages[v]
            calls = self.callsPerVertex[v]
            if self.contexts[v].startswith('call#') or self.contexts[v] in ('call1', 'call2', 'call3', 'call4'):
                attribs['shape'] = 'Mrecord'
            if funcs:
                for f in funcs:
                    # l += f'<BR /><i>FunctionPointer({v}, {html_escape(f)}, 1, <sub>┴</sub>)</i>'  # number of args is hacked (!)
                    l += f'<BR /><i>FunctionPointer({v}, {html_escape(f)}, 1,f)</i>'  # number of args is hacked (!)
                #attribs['fillcolor'] = 'darkolivegreen1'
                #attribs['style'] = 'filled'
                attribs['color'] = 'darkblue'
                attribs['penwidth'] = '3'
            if calls:
                for c in calls:
                    l += f'<BR /><i>ICall({v}, {c}, 1)</i>'
                attribs['color'] = 'darkorange3'
                attribs['penwidth'] = '3'
            if v in self.dotVertexAttributes:
                attribs.update(self.dotVertexAttributes[v])
            attribs = ' '.join(f'{k}=\"{v}\"' for k, v in attribs.items())
            lines.append(f'v{v} [id=v{v} label=<{l}> {attribs}];')
        for e in self.graph.edges():
            attribs = {'label': ''}
            ety = self.edgeTypes[e]
            if ety == EdgeType.SIMPLE_CAST:
                attribs['label'] = f'Cast({e.source()},{e.target()})'
                pass
            elif ety == EdgeType.POINTS_TO:
                attribs['label'] = f'PointsTo({e.source()},{e.target()})'
                attribs['style'] = 'dashed'
            elif ety == EdgeType.STRUCT_MEMBER:
                attribs['label'] = f'StructMember\\l({e.source()},{e.target()},{self.edgeOffsets[e]})\\l'
                attribs['style'] = 'dotted'
            elif ety == EdgeType.UNION_MEMBER:
                attribs['label'] = f'UnionMember\\l({e.source()},{e.target()},{self.edgeUnionTypes[e]})\\l'
                attribs['style'] = 'dotted'
            elif ety == EdgeType.REACHABILITY:
                attribs['color'] = 'gray'
            else:
                attribs['label'] = f'[type {ety}]'

            attribs = ' '.join(f'{k}={v}' if v.startswith('<') else f'{k}=\"{v}\"' for k, v in attribs.items())
            lines.append(f'v{e.source()} -> v{e.target()} [{attribs}];')
        lines += self.dotAdditional
        lines.append('}')
        return '\n'.join(lines)

    def build_dot(self, output: Optional[str] = None, show_calls_and_functions=True, include_number=False, include_additional_names=False, include_additional_types=False):
        dot = self.to_dot(show_calls_and_functions, include_number, include_additional_names, include_additional_types)
        if output:
            format = output.split('.')[-1]
            subprocess.check_output(['dot', f'-T{format}', f'-o{output}'], input=dot.encode())
        else:
            subprocess.check_output(['dot', '-Txlib'], input=dot.encode())

    def build_dot_paper(self, output: Optional[str] = None):
        dot = self.to_dot_paper_style()
        if output:
            format = output.split('.')[-1]
            subprocess.check_output(['dot', f'-T{format}', f'-o{output}'], input=dot.encode())
        else:
            subprocess.check_output(['dot', '-Txlib'], input=dot.encode())

    def callgraph_to_dot(self, include_number=False) -> str:
        lines = ['digraph pdg {']
        # for v in range(self.graph.num_vertices()):
        for v in self.callgraph.vertices():
            ctx = self.callgraph_contexts[v]
            if len(ctx) > 1:
                l = "<BR />".join(html_escape('- ' + s) for s in ctx)
            else:
                l = html_escape(ctx[0])
            if include_number:
                l = f'#{v}<BR />{l}'
            attribs = {}
            #if v in self.dotVertexAttributes:
            #    attribs.update(self.dotVertexAttributes[v])
            attribs = ' '.join(f'{k}=\"{v}\"' for k, v in attribs.items())
            lines.append(f'v{v} [id=v{v} label=<{l}> {attribs}];')
        for e in self.callgraph.edges():
            attribs = {'label': ''}
            attribs = ' '.join(f'{k}=\"{v}\"' for k, v in attribs.items())
            lines.append(f'v{e.source()} -> v{e.target()} [{attribs}];')
        lines.append('}')
        return '\n'.join(lines)

    def callgraph_build_dot(self, output: Optional[str] = None, include_number=False):
        dot = self.callgraph_to_dot(include_number)
        if output:
            format = output.split('.')[-1]
            subprocess.check_output(['dot', f'-T{format}', f'-o{output}'], input=dot.encode())
        else:
            subprocess.check_output(['dot', '-Txlib'], input=dot.encode())

    def dot_mark_call_context_nodes(self):
        for v in self.graph.vertices():
            if self.contexts[v].startswith('call#') or any('\tcall#' in name for name in self.additionalNames[v]):
                self.dotVertexAttributes[v]['fillcolor'] = 'lightcyan'
                self.dotVertexAttributes[v]['style'] = 'filled'

    @staticmethod
    def load_from_file(fname: str) -> 'Typegraph':
        t = Typegraph()
        vertex_id = {}
        callgraph_vertex_id = {}
        with open(fname, 'r') as f:
            content = f.read()
            for line in content.split('\n'):
                line = line.strip()
                if not line: continue
                if line.startswith('N '):
                    vnum, ty, ctx, ctxIsGlobal, layer = line[2:].split('\t')
                    if vnum not in vertex_id:
                        vertex_id[vnum] = t.graph.add_vertex()
                        v = vertex_id[vnum]
                        t.types[t.graph.vertex(v)] = ty
                        t.contexts[t.graph.vertex(v)] = ctx
                        t.layers[t.graph.vertex(v)] = layer
                        t.external[t.graph.vertex(v)] = len(ctxIsGlobal) > 1 and ctxIsGlobal[1] == '1'
                    else:
                        v = vertex_id[vnum]
                        if t.types[t.graph.vertex(v)]:
                            if t.types[t.graph.vertex(v)] != ty and t.contexts[t.graph.vertex(v)] != ctx and t.layers[t.graph.vertex(v)] != layer:
                                t.additionalNames[t.graph.vertex(v)].append(f'{ty}\t{ctx}\t{layer}')
                        else:
                            t.types[t.graph.vertex(v)] = ty
                            t.contexts[t.graph.vertex(v)] = ctx
                            t.layers[t.graph.vertex(v)] = layer
                    t.type_context_to_vertex[(ty, ctx)] = v

                elif line.startswith('E '):
                    l = line[2:] + "\t" if line[2:].count("\t") < 3 else line[2:]
                    src, dst, edgetype, additional = l.split("\t", 3)
                    if src not in vertex_id:
                        vertex_id[src] = t.graph.add_vertex()
                    v1 = vertex_id[src]
                    if dst not in vertex_id:
                        vertex_id[dst] = t.graph.add_vertex()
                    v2 = vertex_id[dst]
                    e = t.graph.add_edge(v1, v2)
                    t.edgeTypes[e] = int(edgetype)
                    if int(edgetype) == EdgeType.STRUCT_MEMBER:
                        t.edgeOffsets[e] = int(additional)
                    if int(edgetype) == EdgeType.UNION_MEMBER:
                        t.edgeUnionTypes[e] = additional
                elif line.startswith('F'):
                    vnum, context, sym = line[2:].split('\t')
                    if vnum not in vertex_id:
                        vertex_id[vnum] = t.graph.add_vertex()
                    v = vertex_id[vnum]
                    t.functionUsages[v].append(sym)
                    if sym not in t.usedFunctions:
                        t.usedFunctions[sym] = set()
                    t.usedFunctions[sym].add(v)
                elif line.startswith('C '):
                    vnum, call, num_args, additional_vertices = line[2:].split('\t')
                    if vnum not in vertex_id:
                        vertex_id[vnum] = t.graph.add_vertex()
                    v = vertex_id[vnum]
                    all_vertices = []
                    for additional_vertex in additional_vertices.split(','):
                        if additional_vertex not in vertex_id:
                            vertex_id[additional_vertex] = t.graph.add_vertex()
                        all_vertices.append(vertex_id[additional_vertex])
                    assert call not in t.calls, f'Call name "{call}" occured multiple times'
                    t.calls[call] = CallInfo(v, int(num_args[1:]), all_vertices, num_args[0] == '1')
                    for v in all_vertices:
                        t.callsPerVertex[v].append(call)
                elif line.startswith('I '):
                    symname, context, params, typeids = line[2:].split('\t')
                    typeids = [int(v) for v in typeids.split(',')]
                    t.interfaces.append(InterfaceDesc(SymbolName=symname, ContextName=context,
                                                      IsFunction=params[0] == '1', IsVarArg=params[1] == '1', DoNotMinimize=params[2] == '1', IsDefined=params[3]=='1', Types=typeids))
                elif line.startswith('S '):
                    params = line[2:].split('\t')
                    vnum = params[0]
                    if vnum not in callgraph_vertex_id:
                        callgraph_vertex_id[vnum] = t.callgraph.add_vertex()
                    v = callgraph_vertex_id[vnum]
                    t.callgraph_contexts[v] = params[1:]
                    for c in params[1:]:
                        t.context_to_callgraph_vertex[c] = v

                elif line.startswith('T '):
                    src, tgt = line[2:].split('\t')
                    if src not in callgraph_vertex_id:
                        callgraph_vertex_id[src] = t.callgraph.add_vertex()
                    if tgt not in callgraph_vertex_id:
                        callgraph_vertex_id[tgt] = t.callgraph.add_vertex()
                    t.callgraph.add_edge(callgraph_vertex_id[src], callgraph_vertex_id[tgt])

                elif line.startswith('D '):
                    ctx, unit = line[2:].split('\t')
                    t.contextDefiningUnits[ctx] = unit

                else:
                    assert False, 'Invalid line: ' + line
        t.assert_valid()
        return t

    def assert_valid(self):
        seen = set()
        #for v in range(self.graph.num_vertices()):
        for v in self.graph.vertices():
            assert self.types[v], f'No type for vertex {v}!'
            assert self.contexts[v], f'No context for vertex {v}!'
            key = f'{self.types[v]} | {self.contexts[v]} | {self.layers[v]}'
            assert key not in seen, f'Key {key} is duplicated!'
            seen.add(key)

    def get_pointsto(self, v: int) -> Optional[int]:
        for e in self.graph.vertex(v).out_edges():
            if self.edgeTypes[e] == EdgeType.POINTS_TO:
                return e.target()
        return None

    def get_struct_member(self, v: int, offset: int) -> Optional[int]:
        for e in self.graph.vertex(v).out_edges():
            if self.edgeTypes[e] == EdgeType.STRUCT_MEMBER and self.edgeOffsets[e] == offset:
                return e.target()
        return None

    def get_casts(self, v: int) -> List[int]:
        return [e.target() for e in self.graph.vertex(v).out_edges() if self.edgeTypes[e] in (EdgeType.SIMPLE_CAST, EdgeType.REACHABILITY)]


class SourceCodeFile:
    def __init__(self, code: str, compiler='C', flags=None):
        self.code = code
        self.compiler = compiler
        self.flags: List[str] = flags or ['-O1']
        self.env: Dict[str, str] = {}

    def get_compiler(self) -> List[str]:
        if self.compiler == 'C': return [CLANG] + CFLAGS
        if self.compiler == 'CXX': return [CLANG + '++'] + CXXFLAGS
        assert False

    def get_extension(self) -> str:
        if self.compiler == 'C': return '.c'
        if self.compiler == 'CXX': return '.cpp'
        assert False

    def set_env(self, key: str, value: str) -> 'SourceCodeFile':
        self.env[key] = value
        return self

    @staticmethod
    def from_file(fname: str) -> 'SourceCodeFile':
        with open(fname, 'r') as f:
            return SourceCodeFile(f.read(), compiler='CXX' if fname.endswith('.cpp') else 'C')

    def preprocessor(self) -> str:
        env = dict(os.environ.items())
        env.update(self.env)
        output = subprocess.check_output([CLANG + '-cpp', '-'] + self.flags, env=env, cwd=TMPDIR, input=self.code.encode('utf-8'))
        return output.decode()


class SourceCode:
    def __init__(self, headers=''):
        self.shared_headers = headers
        self.shared_headers_fname = random_string(20) + '.h'
        self.files: Dict[str, SourceCodeFile] = {}
        self.binary_fname = random_string(20) + '.bin'
        self.linker_flags = ['-O1']
        self.env = {}

    @staticmethod
    def from_string(code: str) -> 'SourceCode':
        sc = SourceCode()
        sc.files[random_string(20)] = SourceCodeFile(code)
        return sc

    @staticmethod
    def from_cxx(code: str) -> 'SourceCode':
        sc = SourceCode()
        sc.files[random_string(20)] = SourceCodeFile(code, compiler='CXX')
        return sc

    @staticmethod
    def from_file(fname: str) -> 'SourceCode':
        sc = SourceCode()
        sc.add_file(SourceCodeFile.from_file(fname))
        return sc

    def add_file(self, file: SourceCodeFile):
        self.files[random_string(20)] = file

    def is_cxx(self) -> bool:
        return any(f.compiler == 'CXX' for f in self.files.values())

    def to_cxx(self) -> 'SourceCode':
        code = SourceCode(self.shared_headers)
        for f in self.files.values():
            code.add_file(SourceCodeFile(f.code, 'CXX', f.flags))
        code.linker_flags = self.linker_flags
        code.env = {k: v for k, v in self.env.items()}
        return code

    def compile(self) -> List[Tuple[str, str]]:
        env = dict(os.environ.items())
        env.update(self.env)
        result = []

        if self.shared_headers:
            with open(TMPDIR + '/' + self.shared_headers_fname, 'w') as f:
                f.write(self.shared_headers)

        for name, file in self.files.items():
            fn = f'{TMPDIR}/{name}.{file.compiler.lower()}'
            with open(fn, 'w') as f:
                if self.shared_headers:
                    f.write(f'#include "{self.shared_headers_fname}"\n')
                f.write(file.code)
            source_env = env
            if file.env:
                source_env = dict(env.items())
                source_env.update(file.env)
            subprocess.check_call(file.get_compiler() + ['-c', fn, '-o', f'{TMPDIR}/{name}.o'] + file.flags, env=source_env, cwd=TMPDIR)
            result.append((f'{TMPDIR}/{name}.o', f'{TMPDIR}/{name}.o.typegraph'))
        return result

    def compile_and_link(self) -> str:
        env = dict(os.environ.items())
        env.update(self.env)
        output = f'{TMPDIR}/{self.binary_fname}'
        files = [f for f, _ in self.compile()]
        cmd = [CLANG + '++' if self.is_cxx() else CLANG] + LDFLAGS
        cmd += ['-o', output] + files + self.linker_flags
        subprocess.check_call(cmd, env=env, cwd=TMPDIR)
        return output

    def get_tmp_files(self) -> List[str]:
        result = [f'{TMPDIR}/{self.binary_fname}']
        if self.shared_headers:
            result.append(TMPDIR + '/' + self.shared_headers_fname)
        for name, file in self.files.items():
            result += [f'{TMPDIR}/{name}.{file.compiler.lower()}', f'{TMPDIR}/{name}.o', f'{TMPDIR}/{name}.o.typegraph']
        return result

    def cleanup(self, with_artifacts=False):
        files = self.get_tmp_files()
        if with_artifacts:
            files += glob.glob(f'{dir}/{self.binary_fname}.calltargets*.json')
            if 'TG_GRAPH_OUTPUT' in self.env:
                files += [self.env['TG_GRAPH_OUTPUT'] + '.before.typegraph', self.env['TG_GRAPH_OUTPUT'] + '.after.typegraph']
        for f in files:
            if os.path.exists(f):
                os.remove(f)

    def load_typegraph_before(self) -> Typegraph:
        return Typegraph.load_from_file(self.env['TG_GRAPH_OUTPUT'] + '.before.typegraph')

    def load_typegraph_after(self) -> Typegraph:
        return Typegraph.load_from_file(self.env['TG_GRAPH_OUTPUT'] + '.after.typegraph')

    def write_for_debug(self, directory):
        os.makedirs(directory, exist_ok=True)
        if self.shared_headers:
            with open(directory + '/' + self.shared_headers_fname, 'w') as f:
                f.write(self.shared_headers)
        for name, file in self.files.items():
            fn = f'{directory}/{name}.{file.compiler.lower()}'
            with open(fn, 'w') as f:
                if self.shared_headers:
                    f.write(f'#include "{self.shared_headers_fname}"\n')
                f.write(file.code)
                print('>', fn)

    def make_shared(self) -> 'SourceCode':
        self.linker_flags.append('-shared')
        for f in self.files.values():
            f.flags.append('-fPIC')
        self.env['TG_DYNLIB_SUPPORT'] = '1'
        self.binary_fname = f'lib{self.binary_fname.replace(".bin", "")}.so'
        return self

    def add_library(self, library: str) -> 'SourceCode':
        libname = os.path.basename(library)
        libdir = os.path.abspath(os.path.dirname(library))
        self.linker_flags += ['-l' + libname[3:].replace('.so', ''), '-L' + libdir, '-Wl,-rpath,' + libdir]
        self.env['TG_DYNLIB_SUPPORT'] = '1'
        return self

    def set_env(self, key: str, value: str) -> 'SourceCode':
        self.env[key] = value
        return self


def compile_to_graph(sourcecode: Union[str, SourceCode], apply_typegraph_tool: bool = False, read_from_ir: bool = False,
                     compute_function_usages=False, check_integrity=False) -> Typegraph:
    if isinstance(sourcecode, str):
        sourcecode = SourceCode.from_string(sourcecode)
    if compute_function_usages or len(sourcecode.files) > 1 or check_integrity:
        apply_typegraph_tool = True
    if check_integrity:
        read_from_ir = True
    output = TMPDIR + '/' + random_string(24) + '.typegraph'
    try:
        outputs = sourcecode.compile()
        if apply_typegraph_tool:
            if read_from_ir:
                input_files = [ir for ir, _ in outputs]
            else:
                input_files = [graph for _, graph in outputs]
            return llvm_typegraph_tool(input_files, compute_function_usages, check_integrity)
        else:
            output = outputs[0][1]
        return Typegraph.load_from_file(output)
    finally:
        for f in sourcecode.get_tmp_files() + [output]:
            if os.path.exists(f):
                os.remove(f)


def llvm_typegraph_tool(input_files: List[str], compute_function_usages=False, check_integrity=False) -> Typegraph:
    output = TMPDIR + '/' + random_string(24) + '.typegraph'
    try:
        cmd = [TYPEGRAPH_TOOL, '-o', output]
        if compute_function_usages:
                cmd.append('--function-usages')
        if check_integrity:
            cmd.append('--check-integrity')
        cmd += input_files
        subprocess.check_call(cmd)
        return Typegraph.load_from_file(output)
    finally:
        if os.path.exists(output):
            os.remove(output)


def compile_link(sourcecode: Union[str, SourceCode], output_graph=False, instrument_collect_calltargets=False, enforce_simple=False, enforce=False) -> str:
    if isinstance(sourcecode, str):
        sourcecode = SourceCode.from_string(sourcecode)
    if output_graph:
        sourcecode.env['TG_GRAPH_OUTPUT'] = TMPDIR + '/' + sourcecode.binary_fname
    if instrument_collect_calltargets:
        sourcecode.env['TG_INSTRUMENT_COLLECTCALLTARGETS'] = '1'
    if enforce_simple:
        sourcecode.env['TG_ENFORCE_SIMPLE'] = '1'
        sourcecode.env['TG_ENFORCE_ARGNUM'] = '1'
    else:
        sourcecode.env['TG_ENFORCE'] = '1' if enforce else '0'
    binary = None
    try:
        binary = sourcecode.compile_and_link()
        return binary
    finally:
        for f in sourcecode.get_tmp_files():
            if os.path.exists(f) and f != binary:
                os.remove(f)


def compile_link_run(sourcecode: Union[str, SourceCode], arguments: List[List[str]], output_graph=False, instrument_collect_calltargets=False, enforce_simple=False, enforce=False) -> \
        Tuple[str, List[bytes]]:
    if isinstance(sourcecode, str):
        sourcecode = SourceCode.from_string(sourcecode)
    if output_graph:
        sourcecode.env['TG_GRAPH_OUTPUT'] = TMPDIR + '/' + sourcecode.binary_fname
    if instrument_collect_calltargets:
        sourcecode.env['TG_INSTRUMENT_COLLECTCALLTARGETS'] = '1'
    if enforce_simple:
        sourcecode.env['TG_ENFORCE_SIMPLE'] = '1'
        sourcecode.env['TG_ENFORCE_ARGNUM'] = '1'
    else:
        sourcecode.env['TG_ENFORCE'] = '1' if enforce else '0'
    binary = compile_link(sourcecode, output_graph, instrument_collect_calltargets, enforce_simple, enforce)
    outputs = []
    for args in arguments:
        outputs.append(subprocess.check_output(RUN_PREFIX + [binary] + args, timeout=10, cwd=TMPDIR))
    return binary, outputs


def filter_typegraph_notion(graph: str) -> str:
    lines = graph.strip().split('\n')
    return '\n'.join(l.strip() for l in lines)


def run_typegraph_tool(graphs: List[str], compute_equivalences=False) -> Typegraph:
    filenames = [TMPDIR + '/' + random_string(16) + '.typegraph' for _ in graphs]
    output = TMPDIR + '/' + random_string(18) + '.typegraph'
    for fname, graph in zip(filenames, graphs):
        with open(fname, 'w') as f:
            f.write(filter_typegraph_notion(graph))
    try:
        cmd = [TYPEGRAPH_TOOL, '-o', output] + filenames
        if compute_equivalences:
            cmd += ['--equivalences']
        subprocess.check_call(cmd)
        return Typegraph.load_from_file(output)
    finally:
        for f in filenames + [output]:
            if os.path.exists(f):
                os.remove(f)


class CallPrecision(NamedTuple):
    allowed_and_called: int
    allowed_but_not_called: int
    called_but_not_allowed: int

    def __add__(self, other: 'CallPrecision') -> 'CallPrecision':
        return CallPrecision(
            self.allowed_and_called + other.allowed_and_called,
            self.allowed_but_not_called + other.allowed_but_not_called,
            self.called_but_not_allowed + other.called_but_not_allowed
        )

    def is_covered(self) -> bool:
        return self.allowed_and_called > 0 or self.called_but_not_allowed > 0

    def sum(self) -> int:
        return self.allowed_and_called + self.allowed_but_not_called + self.called_but_not_allowed


class CallTargets:
    """
    Instrumentation result from one or more instrumented runs
    """

    def __init__(self, fname=None):
        self.binary = ''
        self.cmdline = []
        self.binaries: Set[str] = set()
        self.cmdlines: List[List[str]] = []
        self.num_calls = 0
        self.num_functions = 0
        self.calls: Dict[str, Set[str]] = {}
        self.num_files = 0
        if fname:
            self.load_file(fname)

    def load_file(self, fname: str):
        with open(fname, 'r') as f:
            data = json.loads(f.read())
        self.binary = data['binary']
        if len(self.binaries) > 0 and self.binary not in self.binaries:
            print('[WARNING] Different binaries?')
        self.binaries.add(data['binary'])
        self.cmdline = data['cmdline']
        self.cmdlines.append(self.cmdline)
        if self.num_calls > 0 and self.num_calls != data['num_calls']:
            print('[WARNING] Different number of calls!')
        if self.num_functions > 0 and self.num_functions != data['num_functions']:
            print('[WARNING] Different number of functions!')
        self.num_calls = data['num_calls']
        self.num_functions = data['num_functions']
        if 'calls' in data:
            for k, v in data['calls'].items():
                if k not in self.calls:
                    self.calls[k] = set()
                for func in v:
                    self.calls[k].add(func)
        self.num_files += 1

    @staticmethod
    def from_files(files: List[str]) -> 'CallTargets':
        ct = CallTargets()
        for f in files:
            ct.load_file(f)
        return ct

    @staticmethod
    def from_pattern(pattern: str) -> 'CallTargets':
        return CallTargets.from_files(glob.glob(pattern))

    def assert_matches_graph(self, graph: Typegraph):
        for k, v in self.calls.items():
            assert k in graph.calls, f'Call "{k}" not in graph'
            for f in v:
                assert f in graph.usedFunctions, f'Function "{f}" called but never used'

    def compute_precision_from_dict(self, d: Dict[str, List[str]]) -> Dict[str, CallPrecision]:
        results = {}
        for call, called_functions in self.calls.items():
            if len(called_functions) == 0:
                allowed_functions = set(d.get(call, []))
            else:
                allowed_functions = set(d[call])
            results[call] = CallPrecision(len(called_functions & allowed_functions), len(allowed_functions - called_functions),
                                          len(called_functions - allowed_functions))
            #if len(called_functions) > 0 and len(allowed_functions - called_functions) > 10:
            #    print(f'CALL {len(called_functions)} to {len(allowed_functions - called_functions)} | {call}: Allowed but not called: {allowed_functions - called_functions}. Called: {called_functions}')
        return results

    def print_errors_from_dict(self, d: Dict[str, List[str]]):
        for call, called_functions in self.calls.items():
            errors = called_functions - set(d.get(call, []))
            if len(errors) > 0:
                print('ERRORS:', call, ': called but not in set are ', errors, '. Allowed =', set(d.get(call, [])), 'Called =', called_functions)

    def compute_precision(self, graph: Typegraph) -> Dict[str, CallPrecision]:
        results = {}
        for call, called_functions in self.calls.items():
            v = graph.calls[call].V
            allowed_functions = set(graph.functionUsages[v])
            results[call] = CallPrecision(len(called_functions & allowed_functions), len(allowed_functions - called_functions),
                                          len(called_functions - allowed_functions))
        return results

    def compute_precision_with_dict(self, graph: Typegraph, d: Dict[str, List[str]]) -> Dict[str, CallPrecision]:
        results = {}
        for call, called_functions in self.calls.items():
            v = graph.calls[call].V
            allowed_functions = set(graph.functionUsages[v])
            allowed_functions = allowed_functions & set(d[call])
            results[call] = CallPrecision(len(called_functions & allowed_functions), len(allowed_functions - called_functions),
                                          len(called_functions - allowed_functions))
        return results

    def compute_errors(self, graph: Typegraph) -> Dict[str, Set[str]]:
        """
        :param graph:
        :return: call-name => set of missing functions (forbidden but required ones)
        """
        results = {}
        for call, called_functions in self.calls.items():
            v = graph.calls[call].V
            allowed_functions = set(graph.functionUsages[v])
            error_functions = called_functions - allowed_functions
            if error_functions:
                results[call] = error_functions
        return results

    def rename_functions(self, renamer: Callable[[str], str]):
        for k, v in self.calls.items():
            self.calls[k] = set(renamer(f) for f in v)
