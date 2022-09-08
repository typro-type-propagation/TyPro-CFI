import atexit
import json
import os
import shutil
import subprocess
import sys
import unittest

from typing import Union, List, Tuple

from typegraph_utils import compile_to_graph, Typegraph, SourceCode, compile_link_run, run_typegraph_tool, CallTargets, CallPrecision, SourceCodeFile, \
    InterfaceDesc, llvm_typegraph_tool, compile_link, TMPDIR, ARCH, RUN_PREFIX, MUSL_CLANG

SAMPLE_PREFIX_HEADER = r'''
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define noinline __attribute__ ((noinline))
typedef void (*functype)(int);
typedef int (*functype2)(long);

void noinline f1(int x);
void noinline f2(int x);
int noinline g1(long x);
int noinline g2(long x);
'''
SAMPLE_PREFIX_DEFS = r'''
void noinline f1(int x) {
  printf("f1(%d)\n", x);
}
void noinline f2(int x) {
  printf("f2(%d)\n", x);
}
int noinline g1(long x) {
  printf("g1(%ld)\n", x);
  return 3;
}
int noinline g2(long x) {
  printf("g2(%ld)\n", x);
  return 4;
}
'''
SAMPLE_PREFIX = SAMPLE_PREFIX_HEADER + SAMPLE_PREFIX_DEFS

OLDSTYLE_PREFIX = '''
#include <stdio.h>
#include <stdint.h>

#define noinline __attribute__ ((noinline))
typedef void (*functype)(int);
typedef int (*functype2)(long);

static void f1(int);
static void f2(int);
static int g1(long);
static int g2(long);
static void f1(x) int x; {}
static void f2(x) int x; {}
static int g1(x) long x; { return 0; }
static int g2(x) long x; { return 0; }
'''

SAMPLE_GETONE = '''
    functype noinline get_one(char c) {
        if (c == '1') return f1;
        if (c == '2') return &f2;
        if (c == '3') return (void (*)(int)) g1;
        if (c == '4') return (void (*)(int)) &g2;
        return NULL;
    }
    int main(int argc, const char* argv[]) {
        void (*f)(int);
        f = get_one(argv[1][0]);
        if (f) f(1);
        return 0;
    }
    '''
SAMPLE_GETONE_2 = SAMPLE_GETONE.replace('return NULL;', 'if (c == \'5\') return (void (*)(int)) 0x12345678;\n\t\treturn NULL;')
SAMPLE_GET_MAIN = '''
    int main(int argc, const char* argv[]) {
        void (*f)(int);
        f = get_one(argv[1][0]);
        if (f) f(1);
        return 0;
    }
'''

LIBRARY_HEADER = '''
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define noinline __attribute__ ((noinline))
typedef void (*functype)(int);
typedef int (*functype2)(long);

#ifndef NO_CHECK_FUNCTIONS
#define assert_function_is_id(f) do { if (((uintptr_t) f) >= 0x100000) { fprintf(stderr, "Function is %p\\n", f); abort(); } } while(0)
#define assert_function_is_id2(f) do { if (((long long) f) >= 0x100000) { fprintf(stderr, "Function is %p\\n", f); abort(); } } while(0)
#else
#define assert_function_is_id(f) do {} while(0)
#define assert_function_is_id2(f) do {} while(0)

#endif

// functions defined in the library
void lib11(int x);
void lib12(int x);
int lib21(long x);
int lib22(long x);
void lib3(int x);

// use&return these functions in library
functype library_get1(char c);
functype2 library_get2(char c);

// type conversions
functype2 convert_1to2(functype f);
functype convert_2to1(functype2 f);

// calls defined in the library
void library_call1(functype f);
void library_call2(functype2 f);

// globals
extern int lib_global;
'''
LIBRARY_CODE = LIBRARY_HEADER + '''
// functions defined in the library
void lib11(int x) { printf("lib11 %d\\n", x); }
void lib12(int x) { printf("lib12 %d\\n", x); }
int lib21(long x) { printf("lib21 %ld\\n", x); return 21; }
int lib22(long x) { printf("lib22 %ld\\n", x); return 22; }
void lib3(int x) { printf("lib3 %d\\n", x); }

// use&return these functions in library
functype library_get1(char c) { return c == '1' ? lib11 : lib12; }
functype2 library_get2(char c) { return c == '1' ? lib21 : lib22; }

// type conversions
functype2 convert_1to2(functype f) { return (functype2) f; }
functype convert_2to1(functype2 f) { return (functype) f; }

// calls defined in the library
void library_call1(functype f) { assert_function_is_id(f); if (f) f(1); }
void library_call2(functype2 f) { assert_function_is_id(f); if (f) printf("library_call2 => %d\\n", f(2)); }
void library_call1_10(functype f) { assert_function_is_id(f); if (f) f(10); }
void library_call1_20(functype f) { assert_function_is_id(f); if (f) f(20); }

// globals
int lib_global = 0x1337;
'''
LIBRARY_MAIN = '''
int main(int argc, const char* argv[]) {
    run(argv[1][0]);
    return 0;
}
'''
LIBRARY2_HEADER = LIBRARY_HEADER + '''
// functions defined in the library
void xlib11(int x);
void xlib12(int x);
int xlib21(long x);
int xlib22(long x);

// use&return these functions in library
functype xlibrary_get1(char c);
functype2 xlibrary_get2(char c);

// calls defined in the library
void xlibrary_call1(functype f);
void xlibrary_call2(functype2 f);
void xlibrary_delegate_call1(functype f);
void xlibrary_delegate_call2(functype2 f);
'''
LIBRARY2 = LIBRARY2_HEADER + '''
// functions defined in the library
void xlib11(int x) { printf("xlib11 %d\\n", x); }
void xlib12(int x) { printf("xlib12 %d\\n", x); }
int xlib21(long x) { printf("xlib21 %ld\\n", x); return 41; }
int xlib22(long x) { printf("xlib22 %ld\\n", x); return 42; }

// use&return these functions in library
functype xlibrary_get1(char c) { return c == '1' ? xlib11 : xlib12; }
functype2 xlibrary_get2(char c) { return c == '1' ? xlib21 : xlib22; }

// calls defined in the library
void xlibrary_call1(functype f) { assert_function_is_id(f); if (f) f(7); }
void xlibrary_call2(functype2 f) { assert_function_is_id(f); if (f) printf("xlibrary_call2 => %d\\n", f(8)); }
void xlibrary_delegate_call1(functype f) { assert_function_is_id(f); library_call1(f); }
void xlibrary_delegate_call2(functype2 f) { assert_function_is_id(f); library_call2(f); }
'''


os.environ['TG_CONSIDER_RETURN_TYPE'] = '0'


class TypeGraphTestCases(unittest.TestCase):
    def test_typegraph_import(self):
        graph = Typegraph.load_from_file('samples/sample1.o.typegraph')
        self.assertIsNotNone(graph)
        self.assertGreaterEqual(graph.graph.num_vertices(), 10)
        self.assertGreaterEqual(graph.graph.num_edges(), 10)

    def test_graph_loading_equivalent_node(self):
        graphs = ['''
        N 0\tT1\tC1
        N 0\tT2\tC1
        N 1\tT5\tC2
        E 0\t1\t0
        ''', '''
        N 0\tT5\tC3
        N 1\tT3\tC1
        N 1\tT1\tC1
        E 0\t1\t1
        ''']
        graph = run_typegraph_tool(graphs)
        # graph.build_dot(include_additional_names=True)
        self.assertEqual(graph['T1', 'C1'], graph['T2', 'C1'])
        self.assertEqual(graph['T1', 'C1'], graph['T3', 'C1'])
        self.assertNotEqual(graph['T1', 'C1'], graph['T5', 'C3'])
        self.assertNotEqual(graph['T1', 'C1'], graph['T5', 'C2'])
        self.assertEqual(graph.graph.num_vertices(), 3)
        self.assertTrue(graph.is_reachable_default(graph['T1', 'C1'], graph['T5', 'C2']))


class GetOneTestCases(unittest.TestCase):
    def check_getone(self, g: Typegraph):
        self.assertGreaterEqual(g.graph.num_vertices(), 8)
        self.assertGreaterEqual(g.graph.num_edges(), 4)
        self.assertEqual(len(g.calls), 1)
        self.assertEqual(len(g.usedFunctions), 4)

    def test_getone_simple(self):
        g1 = compile_to_graph(SAMPLE_PREFIX + SAMPLE_GETONE)
        self.check_getone(g1)

    def test_getone_with_tool(self):
        g1 = compile_to_graph(SAMPLE_PREFIX + SAMPLE_GETONE)
        self.check_getone(g1)
        g2 = compile_to_graph(SAMPLE_PREFIX + SAMPLE_GETONE, apply_typegraph_tool=True)
        self.check_getone(g2)
        g3 = compile_to_graph(SAMPLE_PREFIX + SAMPLE_GETONE, apply_typegraph_tool=True, read_from_ir=True)
        self.check_getone(g3)
        self.assertEqual(g1.graph.num_vertices(), g2.graph.num_vertices())
        self.assertEqual(g1.graph.num_vertices(), g3.graph.num_vertices())
        self.assertEqual(g1.graph.num_edges(), g2.graph.num_edges())
        self.assertEqual(g1.graph.num_edges(), g3.graph.num_edges())

    def test_getone_with_computation(self):
        g1 = compile_to_graph(SAMPLE_PREFIX + SAMPLE_GETONE)
        self.check_getone(g1)
        g4 = compile_to_graph(SAMPLE_PREFIX + SAMPLE_GETONE, compute_function_usages=True)
        self.check_getone(g4)
        self.assertEqual(g1.graph.num_vertices(), g4.graph.num_vertices())
        self.assertEqual(g1.graph.num_edges(), g4.graph.num_edges())

    def test_getone_with_computation_cxx(self):
        g4 = compile_to_graph(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE_GETONE), compute_function_usages=True)
        self.check_getone(g4)

    def test_getone_find_functions(self):
        g = compile_to_graph(SAMPLE_PREFIX + SAMPLE_GETONE, compute_function_usages=True)
        self.check_getone(g)
        funcs = g.get_call_first_functions()
        self.assertIn('f1', funcs)
        self.assertIn('f2', funcs)
        self.assertIn('g1', funcs)
        self.assertIn('g2', funcs)

    def test_getone_find_functions_cxx(self):
        g = compile_to_graph(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE_GETONE), compute_function_usages=True)
        self.check_getone(g)
        funcs = g.get_call_first_functions()
        self.assertIn('_Z2f1i', funcs)
        self.assertIn('_Z2f2i', funcs)
        self.assertIn('_Z2g1l', funcs)
        self.assertIn('_Z2g2l', funcs)

    def test_getone_stable_call_names(self):
        g1 = compile_to_graph(SAMPLE_PREFIX + SAMPLE_GETONE, apply_typegraph_tool=False)
        g2 = compile_to_graph(SAMPLE_PREFIX + SAMPLE_GETONE, apply_typegraph_tool=False)
        calls1 = list(sorted(g1.calls.keys()))
        calls2 = list(sorted(g2.calls.keys()))
        self.assertEqual(calls1, calls2)
        self.assertEqual(len(calls1), 1)
        g1 = compile_to_graph(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE_GETONE), apply_typegraph_tool=False)
        g2 = compile_to_graph(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE_GETONE), apply_typegraph_tool=False)
        calls1 = list(sorted(g1.calls.keys()))
        calls2 = list(sorted(g2.calls.keys()))
        self.assertEqual(calls1, calls2)

    def test_getone_run(self):
        binary, outputs = compile_link_run(SAMPLE_PREFIX + SAMPLE_GETONE, [['1'], ['2'], ['3'], ['4']])
        self.assertEqual(outputs[0].decode().strip(), 'f1(1)')
        self.assertEqual(outputs[1].decode().strip(), 'f2(1)')
        self.assertEqual(outputs[2].decode().strip(), 'g1(1)')
        self.assertEqual(outputs[3].decode().strip(), 'g2(1)')
        filetype = subprocess.check_output(['file', binary])
        self.assertIn(ARCH['fileident'], filetype.decode())
        binary, outputs = compile_link_run(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE_GETONE), [['1'], ['2'], ['3'], ['4']])
        self.assertEqual(outputs[0].decode().strip(), 'f1(1)')
        self.assertEqual(outputs[1].decode().strip(), 'f2(1)')
        self.assertEqual(outputs[2].decode().strip(), 'g1(1)')
        self.assertEqual(outputs[3].decode().strip(), 'g2(1)')
        filetype = subprocess.check_output(['file', binary])
        self.assertIn(ARCH['fileident'], filetype.decode())

    def test_getone_enforce_simple(self):
        binary, outputs = compile_link_run(SAMPLE_PREFIX + SAMPLE_GETONE_2, [['1'], ['2'], ['3'], ['4']], enforce_simple=True)
        self.assertEqual(outputs[0].decode().strip(), 'f1(1)')
        self.assertEqual(outputs[1].decode().strip(), 'f2(1)')
        self.assertEqual(outputs[2].decode().strip(), 'g1(1)')
        self.assertEqual(outputs[3].decode().strip(), 'g2(1)')
        rc = subprocess.run(RUN_PREFIX + [binary, '5'], timeout=10)
        os.remove(binary)
        self.assertEqual(rc.returncode, ARCH['signal'])
        binary, outputs = compile_link_run(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE_GETONE_2), [['1'], ['2'], ['3'], ['4']], enforce_simple=True)
        self.assertEqual(outputs[0].decode().strip(), 'f1(1)')
        self.assertEqual(outputs[1].decode().strip(), 'f2(1)')
        self.assertEqual(outputs[2].decode().strip(), 'g1(1)')
        self.assertEqual(outputs[3].decode().strip(), 'g2(1)')
        rc = subprocess.run(RUN_PREFIX + [binary, '5'], timeout=10)
        os.remove(binary)
        self.assertEqual(rc.returncode, ARCH['signal'])

    def test_getone_enforce(self):
        binary, outputs = compile_link_run(SAMPLE_PREFIX + SAMPLE_GETONE_2, [['1'], ['2'], ['3'], ['4']], enforce=True)
        self.assertEqual(outputs[0].decode().strip(), 'f1(1)')
        self.assertEqual(outputs[1].decode().strip(), 'f2(1)')
        self.assertEqual(outputs[2].decode().strip(), 'g1(1)')
        self.assertEqual(outputs[3].decode().strip(), 'g2(1)')
        rc = subprocess.run(RUN_PREFIX + [binary, '5'], timeout=10)
        os.remove(binary)
        self.assertEqual(rc.returncode, ARCH['signal'])
        binary, outputs = compile_link_run(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE_GETONE_2), [['1'], ['2'], ['3'], ['4']], enforce=True)
        self.assertEqual(outputs[0].decode().strip(), 'f1(1)')
        self.assertEqual(outputs[1].decode().strip(), 'f2(1)')
        self.assertEqual(outputs[2].decode().strip(), 'g1(1)')
        self.assertEqual(outputs[3].decode().strip(), 'g2(1)')
        rc = subprocess.run(RUN_PREFIX + [binary, '5'], timeout=10)
        os.remove(binary)
        self.assertEqual(rc.returncode, ARCH['signal'])


class OneCallTestCaseBase(unittest.TestCase):
    CXX_NAMES = {'f1': '_Z2f1i', 'f2': '_Z2f2i', 'g1': '_Z2g1l', 'g2': '_Z2g2l'}

    def _run_test(self, code: Union[str, SourceCode], valid: List[str], cxxnames=None, show_graph=False, **kwargs):
        if not cxxnames:
            cxxnames = self.CXX_NAMES
        g = compile_to_graph(code, compute_function_usages=True, **kwargs)
        if show_graph:
            # g.build_dot(show_calls_and_functions=True)
            g.get_interesting_subgraph().build_dot(show_calls_and_functions=True)
        funcs = g.get_call_first_functions(lambda call: ' in target' in call or '6target' in call)
        for f in ('f1', 'f2', 'g1', 'g2'):
            f2 = cxxnames[f] if isinstance(code, SourceCode) and code.is_cxx() else f
            if f in valid:
                self.assertIn(f2, funcs)
            else:
                self.assertNotIn(f2, funcs)


class OneCallTestCase(OneCallTestCaseBase):
    """
    Test cases with exactly one indirect call, asserting the possible functions for this call
    """
    SAMPLE = ''
    VALID_FUNCTIONS = []

    def test_c(self):
        self._run_test(SAMPLE_PREFIX + self.SAMPLE, self.VALID_FUNCTIONS)

    def test_cxx(self):
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + self.SAMPLE), self.VALID_FUNCTIONS)


class CastsTestCase(OneCallTestCase):
    SAMPLE = '''
    void target(functype f) { f(1); }
    void test() { target((functype) g1); }
    '''
    VALID_FUNCTIONS = ['g1']

    def test_deref_function(self):
        SAMPLE = '''
        static functype x;
        void target(functype f) { f(1); }
        void test1() { x = &f1; }
        void test2() { target(*x); }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self._run_test(code, ['f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_deref_with_struct(self):
        SAMPLE = '''
        struct S { const functype f; };
        static const struct S s[] = {{f1}};
        void target(functype f) { f(1); }
        void test(int x) { target((*s[x].f)); }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self._run_test(code, ['f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])


class SimpleIfTestCase(OneCallTestCase):
    SAMPLE = '''
    functype noinline get_one(char c) {
        if (c == 1) return f1;
        else return f2;
    }
    void noinline target() { get_one(0)(1); }
    '''
    VALID_FUNCTIONS = ['f1', 'f2']


class TakeFunctionRefTestCase(OneCallTestCase):
    SAMPLE = '''
    functype noinline get1() { return f1; }
    functype noinline get2() { return &f2; }
    functype noinline get3() { return (void (*)(int)) g1; }
    functype noinline get4() { return (void (*)(int)) &g2; }
    functype noinline get5() { functype x[2] = {f1, &f2}; return x[0]; }
    functype noinline get6() { struct {functype x; functype y; } S = {f1, &f2}; return S.x; }
    void noinline target() {
        functype f = get1();
        f(1);
    }
    '''
    VALID_FUNCTIONS = ['f1']

    def test_address_of(self):
        code = SAMPLE_PREFIX + self.SAMPLE.replace('get1();', 'get2();')
        self._run_test(code, ['f2'])
        self._run_test(SourceCode.from_cxx(code), ['f2'])

    def test_cast_direct(self):
        code = SAMPLE_PREFIX + self.SAMPLE.replace('get1();', 'get3();')
        self._run_test(code, ['g1'])
        self._run_test(SourceCode.from_cxx(code), ['g1'])

    def test_cast_address_of(self):
        code = SAMPLE_PREFIX + self.SAMPLE.replace('get1();', 'get4();')
        self._run_test(code, ['g2'])
        self._run_test(SourceCode.from_cxx(code), ['g2'])

    def test_initializer_array(self):
        code = SAMPLE_PREFIX + self.SAMPLE.replace('get1();', 'get5();')
        self._run_test(code, ['f1', 'f2'])
        self._run_test(SourceCode.from_cxx(code), ['f1', 'f2'])

    def test_initializer_struct(self):
        code = SAMPLE_PREFIX + self.SAMPLE.replace('get1();', 'get6();')
        self._run_test(code, ['f1', 'f2'])
        self._run_test(SourceCode.from_cxx(code), ['f1', 'f2'])


class VoidPtrTestCases(OneCallTestCaseBase):
    SAMPLE = '''
    functype noinline get_one(char c) {
        void* shit = &g1;
        if (c == '1') return f1;
        return NULL;
    }
    void noinline target() { get_one(0)(1); }
    '''

    def test_null_is_not_voidptr(self):
        g = compile_to_graph(SAMPLE_PREFIX + self.SAMPLE, compute_function_usages=True)
        funcs = g.get_call_first_functions()
        self.assertIn('f1', funcs)
        self.assertNotIn('g1', funcs, 'NULL should not induce a void* cast')

    def test_null_is_not_voidptr_cxx(self):
        g = compile_to_graph(SourceCode.from_cxx(SAMPLE_PREFIX + self.SAMPLE.replace('&g1', '(void*)&g1')), compute_function_usages=True)
        funcs = g.get_call_first_functions()
        self.assertIn('_Z2f1i', funcs)
        self.assertNotIn('_Z2g1l', funcs, 'NULL should not induce a void* cast')

    def test_null_is_not_voidptr_nullptr(self):
        g = compile_to_graph(SourceCode.from_cxx(SAMPLE_PREFIX + self.SAMPLE.replace('&g1', '(void*)&g1').replace('NULL', 'nullptr')),
                             compute_function_usages=True)
        funcs = g.get_call_first_functions()
        self.assertIn('_Z2f1i', funcs)
        self.assertNotIn('_Z2g1l', funcs, 'NULL should not induce a void* cast')

    def test_literal_null(self):
        code = SAMPLE_PREFIX + '''
        functype noinline get_one(char c) {
            void* shit = (void*) &g1;
            if (c == '1') return f1;
            return 0;
        }
        void noinline target() { get_one(0)(1); }
        '''
        g = compile_to_graph(code, compute_function_usages=True)
        funcs = g.get_call_first_functions()
        self.assertIn('f1', funcs)
        self.assertNotIn('g1', funcs, '0 should not induce a void* cast')
        g = compile_to_graph(SourceCode.from_cxx(code), compute_function_usages=True)
        funcs = g.get_call_first_functions()
        self.assertIn('_Z2f1i', funcs)
        self.assertNotIn('_Z2g1l', funcs, '0 should not induce a void* cast')

    def test_nullptr(self):
        code = SAMPLE_PREFIX + '''
        functype noinline get_one(char c) {
            void* shit = (void*) &g1;
            if (c == '1') return f1;
            return nullptr;
        }
        void noinline target() { get_one(0)(1); }
        '''
        g = compile_to_graph(SourceCode.from_cxx(code), compute_function_usages=True)
        funcs = g.get_call_first_functions()
        self.assertIn('_Z2f1i', funcs)
        self.assertNotIn('_Z2g1l', funcs, '0 should not induce a void* cast')

    def test_voidptr_in_summary(self):
        code = SourceCode(SAMPLE_PREFIX_HEADER + ''' char *testX(void *x); ''')
        code.add_file(SourceCodeFile('''
        char *testX(void *x) {
            int *y = (int*) *(long**) x;
            return (char*) &y;
        }
        '''))
        code.add_file(SourceCodeFile(SAMPLE_PREFIX_DEFS + '''
        void target(functype f) { f(1); }
        void test(void* vp) {
            target(*(functype*) testX(vp));
        }
        void test2() {
            functype f = f1;
            test(&f);
        }
        '''))
        self._run_test(code, ['f1'])

    def test_voidptr_as_argument(self):
        SAMPLE = '''
        void target(functype f) { f(1); }
        void test(char* fptr) { target(*(functype*) fptr); }
        void test2() {
            functype f = f1;
            test((char*) &f);
        }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1'])


class GlobalTestCases(OneCallTestCase):
    SAMPLE = '''
    functype gv = NULL;
    void noinline test1() { gv = f1; }
    void noinline target() { gv(1); }
    '''
    VALID_FUNCTIONS = ['f1']

    def test_two_globals(self):
        SAMPLE = '''
        functype gv = NULL;
        functype gv2 = NULL;
        void noinline test1() { gv = f1; }
        void noinline test2() { gv2 = f2; }
        void noinline target() { gv(1); }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_struct_global(self):
        SAMPLE = '''
        typedef struct { char* name; functype f; } S;
        S gv;
        S* gv2;
        void noinline test1() { gv.f = f1; }
        void noinline target() { gv.f(1); }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])
        SAMPLE = SAMPLE.replace('gv.f', 'gv2->f')
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_global_initializers(self):
        SAMPLE = '''
        typedef struct { char* name; functype f; } S;
        functype glob1 = f1;
        functype glob2 = &f2;
        S glob3 = { "test", (functype) &g1 };
        noinline void target(functype f) { f(1); }
        noinline void test() { target(glob1); target(glob2); target(glob3.f); }
        '''
        # graph = compile_to_graph(SAMPLE_PREFIX + SAMPLE, compute_function_usages=True)
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1', 'f2', 'g1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1', 'f2', 'g1'])

    def test_cxx_initializer(self):
        SAMPLE = '''
        struct S {
            functype f;
            S(functype f) : f(f){}
        };
        S s(f1);
        noinline void target(functype f) { f(1); }
        noinline void test() { target(s.f); }
        '''
        # graph = compile_to_graph(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), compute_function_usages=True)
        # graph.build_dot()
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_cxx_class_static_field(self):
        SAMPLE = '''
        struct S {
            static functype f;
            void target() { f(1); }
        };
        functype S::f = f1;
        noinline void test() { S s; s.f = f2; }
        noinline void test2() { S s; s.target(); }
        '''
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1', 'f2'])

    def test_function_array(self):
        SAMPLE = '''
        static functype funcs[3] = {f1, &f2, (functype) g1};
        noinline void target(int i) { funcs[i](1); }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1', 'f2', 'g1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1', 'f2', 'g1'])
        SAMPLE = '''
        static functype funcs[3] = {f1, &f2, (functype) g1};
        noinline void target(int i) { (*(funcs[i]))(1); }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1', 'f2', 'g1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1', 'f2', 'g1'])

    def test_function_struct_array(self):
        SAMPLE = '''
        struct S { functype f; int i; };
        static struct S funcs[3] = {{f1, 0}, {&f2, 1}, {(functype) g1, 2}};
        noinline void target(int i) { funcs[i].f(1); }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1', 'f2', 'g1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1', 'f2', 'g1'])
        SAMPLE = '''
        struct S { void (*const f)(int x); int i; };
        static struct S funcs[3] = {{f1, 0}, {&f2, 1}, {(functype) g1, 2}};
        noinline void target(int i) { funcs[i].f(1); }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1', 'f2', 'g1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1', 'f2', 'g1'])

    def test_struct_initializers(self):
        # parens are bad!
        SAMPLE = '''
        struct S { functype f; int i; };
        static struct S funcs[2] = {{(f1), 1}, { .f = (f2), .i = 2 }};
        noinline void target(int i) { funcs[i].f(1); }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1', 'f2'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1', 'f2'])


class StructPtrTests(OneCallTestCase):
    SAMPLE = '''
    typedef struct { int a; functype f; long c; } S;
    void noinline target(S s) { s.f(1); }
    void noinline test1() { S s; s.f = f1; target(s); }
    '''
    VALID_FUNCTIONS = ['f1']

    def test_struct_ptr_param(self):
        SAMPLE = '''
        typedef struct { int a; functype f; long c; } S;
        void noinline target(S *s) { s->f(1); }
        void noinline test1() { S s; s.f = f1; target(&s); }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])
        # reference instead of ptr
        SAMPLE = SAMPLE.replace('&s', 's').replace('S *s', 'S &s').replace('s->', 's.')
        print(SAMPLE)
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_struct_ptr_cast_param(self):
        SAMPLE = '''
        typedef struct { int a; functype f; long c; } S;
        typedef struct { unsigned b; functype f; unsigned long c; } T;
        void noinline target(T *t) { t->f(1); }
        void noinline test2(S *s) { target((T*) s); }
        void noinline test1() { S s; s.f = f1; test2(&s); }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_struct_ptr_cast_void_param(self):
        SAMPLE = '''
        typedef struct { int a; functype f; long c; } S;
        typedef struct { unsigned b; void *f; unsigned long c; } T;
        void noinline target(S *s) { s->f(1); }
        void noinline test3(T *t) { target((S*) t); }
        void noinline test2(S *s) { test3((T*) s); }
        void noinline test1() { S s; s.f = f1; test2(&s); }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])
        SAMPLE = '''
        typedef struct { int a; functype f; long c; } S;
        typedef struct { unsigned b; void *f; unsigned long c; } T;
        void noinline target(T *t) { ((functype) t->f)(1); }
        void noinline test2(S *s) { target((T*) s); }
        void noinline test1() { S s; s.f = f1; test2(&s); }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_struct_with_two_fp(self):
        SAMPLE = '''
        typedef struct { int a; functype f; functype2 g; } S;
        void noinline target(S *s) { s->f(1); }
        void noinline test2(S *s) { s->f = (functype) s->g; }
        void noinline test1() { S s; s.f = f1; s.g = g1; test2(&s); target(&s); }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1', 'g1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1', 'g1'])
        SAMPLE = SAMPLE.replace('s->f(', 's->g(')
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['g1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['g1'])

    """
    # That's actually undefined behaviour
    def test_union(self):
        SAMPLE = '''
        typedef union {
            functype f;
            functype2 g;
        } FunctionTypes;
        noinline void target() {
            FunctionTypes ft;
            ft.f = f1;
            ft.g = g1;
            ft.f(1);
        }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1', 'g1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1', 'g1'])
    """

    def test_cxx_reference(self):
        SAMPLE = '''
        noinline void target(functype &f) { f(1); }
        noinline void test() { functype f = f1; target(f); }
        '''
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])
        SAMPLE = '''
        noinline void target(const functype &f) { f(1); }
        noinline void test() { functype f = f1; target(f); }
        '''
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_cxx_reference_is_ptr(self):
        SAMPLE = '''
        noinline void test1(functype &f) { f = f1; }
        noinline void test2(functype f) { f = f2; }
        noinline void target() {
          functype f;
          test1(f); test2(f);
          f(1);
        }
        '''
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_const_global_struct(self):
        SAMPLE = '''
        struct S { functype f; functype2 g; };
        const struct S global_s = {f1, g1};
        noinline void target() {
            global_s.f(1);
        }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_const_global_array(self):
        # taken from SPEC's gcc:
        # call#80.0 in actual_hazard_this_instance targets "athlon_ieu_unit_blockage" over global "function_units"
        SAMPLE = '''
        extern const struct function_unit_desc {
          const char *const name;
          int (*const func) (long x);
        } function_units[];

        const struct function_unit_desc function_units[] = {
          {"func1", 0}, {"func2", g1}, {"func3", (functype2) f1}  
        };

        noinline void target(int unit) {
          function_units[unit].func(1);
        }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['g1', 'f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['g1', 'f1'])

    def test_const_global_array_static(self):
        # taken from SPEC's gcc:
        # call#80.0 in actual_hazard_this_instance targets "athlon_ieu_unit_blockage" over global "function_units"
        SAMPLE = '''
        extern const struct function_unit_desc {
          const char *const name;
          int (*const func) (long x);
        } function_units[];

        const struct function_unit_desc function_units[] = {
          {"func1", 0}, {"func2", g1}, {"func3", (functype2) f1}  
        };

        noinline void target(int unit) {
          function_units[unit].func(1);
        }
        '''
        # code = SourceCode.from_string(OLDSTYLE_PREFIX + SAMPLE)
        # compile_to_graph(code).build_dot()
        # code.env['TG_CLANG_MINIMIZE_GRAPH'] = '0'
        # compile_to_graph(code).build_dot()
        self._run_test(OLDSTYLE_PREFIX + SAMPLE, ['g1', 'f1'])

    def test_const_array(self):
        # taken from SPEC's gcc:
        # call#80.0 in actual_hazard_this_instance targets "athlon_ieu_unit_blockage" over global "function_units"
        SAMPLE = '''
        noinline void target(int unit) {
            const struct function_unit_desc {
              const char *const name;
              int (*const func) (long x);
            } function_units[] = {
              {"func1", 0}, {"func2", g1}, {"func3", (functype2) f1}  
            };
            function_units[unit].func(1);
        }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['g1', 'f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['g1', 'f1'])

    def test_nested_array_init(self):
        SAMPLE = '''
        typedef struct S {
            int x;
            functype a;
            functype b;
        } S;
        static S global1[] = {
            {0, f1, (functype) g1}
        };
        struct { int x; S* s; } global2[] = {
            {0, &global1[0]}
        };
        void target() {
            global2[0].s->a(1);
        }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['g1', 'f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['g1', 'f1'])

    def test_use_before_def(self):
        SAMPLE = '''
        typedef struct S S;
        typedef struct S2 S2;
        void set(S *s, functype f);
        void target(S *s);
        void test(S *s) { set(s, f1); target(s); }
        
        struct S { functype f; };
        struct S2 { functype2 f; };
        
        void target(S *s) { s->f(1); }
        void set(S *s, functype f) { s->f = f; }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_const_global_struct_array(self):
        SAMPLE = '''
        struct S { const char* c; functype func; };
        const struct S cmds[] = {
            {"f1", f1},
            {"f2", f2},
            {0}
        };
        void target(functype f) { f(1); }
        void test() {
            for (int i = 0; i < (sizeof(cmds)/sizeof(*cmds) - 1); i++) {
                target(cmds[i].func);
            }
        }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1', 'f2'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1', 'f2'])


class TypeQualifiersTestCase(OneCallTestCaseBase):
    def test_const(self):
        SAMPLE = '''
        noinline void test1_2(const unsigned long *y) {
        }
        noinline void test1_3(unsigned long a) { test1_2(&a); }
        noinline void test1(const unsigned long* x) {
            const unsigned long y = x[0];
            long z = x[1];
        }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX_HEADER + SAMPLE)
        code.env['TG_CLANG_MINIMIZE_GRAPH'] = '0'
        graph = compile_to_graph(code)
        n1 = graph['unsigned long *', 'test1_3']
        n2 = graph['const unsigned long *', 'test1_2']
        assert graph.is_reachable_default(n1, n2)
        assert graph.is_reachable_default(graph['const unsigned long', 'test1'], graph['long', 'test1'])
        code = SourceCode.from_cxx(SAMPLE_PREFIX_HEADER + SAMPLE)
        code.env['TG_CLANG_MINIMIZE_GRAPH'] = '0'
        graph = compile_to_graph(code)
        assert graph.is_reachable_default(graph['unsigned long *', '_Z7test1_3m'], graph['const unsigned long *', '_Z7test1_2PKm'])
        assert graph.is_reachable_default(graph['const unsigned long', '_Z5test1PKm'], graph['long', '_Z5test1PKm'])

    def test_volatile(self):
        SAMPLE = '''
        noinline void test1_2(volatile unsigned long *y) {
        }
        noinline void test1_3(unsigned long a) { test1_2(&a); }
        noinline void test1(volatile unsigned long* x) {
            volatile unsigned long y = x[0];
            long z = x[1];
        }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX_HEADER + SAMPLE)
        code.env['TG_CLANG_MINIMIZE_GRAPH'] = '0'
        graph = compile_to_graph(code)
        n1 = graph['unsigned long *', 'test1_3']
        n2 = graph['volatile unsigned long *', 'test1_2']
        assert graph.is_reachable_default(n1, n2)
        assert graph.is_reachable_default(graph['volatile unsigned long', 'test1'], graph['long', 'test1'])
        code = SourceCode.from_cxx(SAMPLE_PREFIX_HEADER + SAMPLE)
        code.env['TG_CLANG_MINIMIZE_GRAPH'] = '0'
        graph = compile_to_graph(code)
        assert graph.is_reachable_default(graph['unsigned long *', '_Z7test1_3m'], graph['volatile unsigned long *', '_Z7test1_2PVm'])
        assert graph.is_reachable_default(graph['volatile unsigned long', '_Z5test1PVm'], graph['long', '_Z5test1PVm'])

    def test_const_params(self):
        SAMPLE = '''
        noinline void test1(const long x) {}
        noinline void test2(long y) { test1(y); }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX_HEADER + SAMPLE)
        code.env['TG_CLANG_MINIMIZE_GRAPH'] = '0'
        graph = compile_to_graph(code)
        assert graph.is_reachable_default(graph['long', 'test2'], graph['const long', 'test1'])
        graph = compile_to_graph(code.to_cxx())
        assert graph.is_reachable_default(graph['long', '_Z5test2l'], graph['const long', '_Z5test1l'])
        # TODO test the same for methods / virtual methods

    def test_const_function_ptr(self):
        SAMPLE1 = '''
        noinline void target(const functype f);
        noinline void test1() { target(f1); }
        '''
        SAMPLE2 = '''
        noinline void target(const functype f) { f(1); }
        '''
        code = SourceCode(SAMPLE_PREFIX_HEADER)
        code.add_file(SourceCodeFile(SAMPLE1))
        code.add_file(SourceCodeFile(SAMPLE_PREFIX_DEFS + SAMPLE2))
        self._run_test(code, ['f1'])
        self._run_test(code.to_cxx(), ['f1'])

    def test_qualified_struct_pointer(self):
        SAMPLE = '''
        typedef struct S { functype f; long l; } S;
        void noinline target(functype f, long l) { f(1); }
        void test1(const S* s) { target(s->f, s->l);}
        void test2(volatile S* s) { target(s->f, s->l);}
        void test3(volatile const S* s) { target(s->f, s->l);}
        void init1() { S s = {f1, 0}; test1(&s); }
        void init2() { S s = {f2, 0}; test2(&s); }
        void init3() { S s = {(functype) g1, 0}; test3(&s); }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX_HEADER + SAMPLE)
        self._run_test(code, ['f1', 'f2', 'g1'])
        self._run_test(code.to_cxx(), ['f1', 'f2', 'g1'])

    def test_qualified_struct_pointer_field(self):
        SAMPLE = '''
        typedef struct S2 { const functype f; long l; } S2;
        void noinline target(functype f, long l) { f(1); }
        void test4(S2* s) { target(s->f, s->l);}
        void test5(const S2* s) { target(s->f, s->l);}
        void test6(volatile S2* s) { target(s->f, s->l);}
        void init1() { S2 s = {f1, 0}; test4(&s); }
        void init2() { S2 s = {f2, 0}; test5(&s); }
        void init3() { S2 s = {(functype) g1, 0}; test6(&s); }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX_HEADER + SAMPLE)
        self._run_test(code, ['f1', 'f2', 'g1'])
        self._run_test(code.to_cxx(), ['f1', 'f2', 'g1'])

    def test_qualified_subtypes(self):
        SAMPLE = '''
        void capture(int x, ...) {}
        void test_simple(long l, const long l2) { capture(0, l, l2); }
        void test_ptr(char *c1, const char *c2, char * const c3, const char* const c4, long *const l1, const long *l2) {
            capture(0, c1, c2, c3, c4, *l1, *l2);
        }
        typedef const long *const constlongptr;
        void test_ptr_2(constlongptr l) { capture(0, l, *l); }
        // structs
        struct S {
            long l;
            const unsigned long u;
            const long *p1;
            long *const p2;
        };
        void test_struct_1(struct S s) { capture(0, s, s.l, s.u, s.p1, s.p2); }
        void test_struct_2(const struct S s) { capture(0, s, s.l, s.u, s.p1, s.p2); }
        void test_struct_3(const struct S s) { capture(0, *s.p1, *s.p2); }
        // structs + typedef
        typedef struct {
            long l;
            const unsigned long u;
            const long *p1;
            long *const p2;
        } S2;
        typedef struct S S3;
        void test_structdef_1(S2 s) { capture(0, s, s.l, s.u, s.p1, s.p2); }
        void test_structdef_2(const S2 s) { capture(0, s, s.l, s.u, s.p1, s.p2); }
        void test_structdef_3(const S2 s) { capture(0, *s.p1, *s.p2); }
        void test_structdef_4(const S3 s) { capture(0, *s.p1, *s.p2); }
        // struct + ptr
        void test_struct_ptr_1(const struct S *s) { capture(0, s->l); }
        void test_struct_ptr_2(const S2 *s) { capture(0, s->l); }
        void test_struct_ptr_3(struct S *const s) { capture(0, s->u); }
        void test_struct_ptr_4(struct S *const *s) { capture(0, (*s)->u); }
        // struct as variable
        void test_struct_var() { const struct S s; capture(0, s, s.l, s.u, s.p1, s.p2); }
        void test_structdef_var() { const S2 s; capture(0, s, s.l, s.u, s.p1, s.p2); }
        // array types
        void test_array_1(long l[], const unsigned long u[]) { capture(0, l, u); }
        void test_array_2(long l[], const unsigned long u[]) { capture(0, l[1], u[2]); }
        void test_array_3(const struct S s[], int i) { capture(0, s[i].p1); }
        void test_fixed_array_1(long l[3], const unsigned long u[3]) { capture(0, l, u); }
        void test_fixed_array_2(long l[3], const unsigned long u[3]) { capture(0, l[1], u[2]); }
        void test_fixed_array_3(const struct S s[3], int i) { capture(0, s[i].p1); }
        void test_const_array_1(const struct S s[])  { capture(0, s[0].l); }
        void test_const_array_2(const struct S s[3]) { capture(0, s[0].l); }
        void test_const_array_3(struct S const s[])  { capture(0, s[0].l); }
        void test_const_array_4(struct S const s[3]) { capture(0, s[0].l); }
        // test array variables
        void test_const_array_var_1() { const struct S s[3]; capture(0, s[0].l, s); }
        void test_const_array_var_2() { struct S const s[3]; capture(0, s[0].l, s); }
        void test_const_array_var_3(void *p) { const struct S s[] = {{0,0,p,p}}; capture(0, s[0].l, s); }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX_HEADER + SAMPLE)
        code.env['TG_CLANG_MINIMIZE_GRAPH'] = '0'
        graph = compile_to_graph(code)
        # test_simple
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['long', 'test_simple']))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['const long', 'test_simple']))
        # test_ptr
        self.assertIn(graph['char *', 'capture'], graph.get_casts(graph['char *', 'test_ptr']))
        self.assertIn(graph['char *', 'capture'], graph.get_casts(graph['char *const', 'test_ptr']))
        self.assertIn(graph['const char *', 'capture'], graph.get_casts(graph['const char *', 'test_ptr']))
        self.assertIn(graph['const char *', 'capture'], graph.get_casts(graph['const char *const', 'test_ptr']))
        self.assertEqual(graph['const long', 'test_ptr'], graph.get_pointsto(graph['const long *', 'test_ptr']))
        self.assertEqual(graph['long', 'test_ptr'], graph.get_pointsto(graph['long *const', 'test_ptr']))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph.get_pointsto(graph['const long *', 'test_ptr'])))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph.get_pointsto(graph['long *const', 'test_ptr'])))
        # test_ptr_2
        self.assertEqual(graph['const long', 'test_ptr_2'], graph.get_pointsto(graph['const long *const', 'test_ptr_2']))
        self.assertIn(graph['const long *', 'capture'], graph.get_casts(graph['const long *const', 'test_ptr_2']))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['const long', 'test_ptr_2']))
        # test_struct_1
        self.assertEqual(graph['long', 'test_struct_1'], graph.get_struct_member(graph['struct S', 'test_struct_1'], 0))
        self.assertEqual(graph['const unsigned long', 'test_struct_1'], graph.get_struct_member(graph['struct S', 'test_struct_1'], 8))
        self.assertEqual(graph['const long *', 'test_struct_1'], graph.get_struct_member(graph['struct S', 'test_struct_1'], 16))
        self.assertEqual(graph['long *const', 'test_struct_1'], graph.get_struct_member(graph['struct S', 'test_struct_1'], 24))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['long', 'test_struct_1']))
        self.assertIn(graph['unsigned long', 'capture'], graph.get_casts(graph['const unsigned long', 'test_struct_1']))
        self.assertIn(graph['const long *', 'capture'], graph.get_casts(graph['const long *', 'test_struct_1']))
        self.assertIn(graph['long *', 'capture'], graph.get_casts(graph['long *const', 'test_struct_1']))
        # test_struct_2
        self.assertEqual(graph['const long', 'test_struct_2'], graph.get_struct_member(graph['const struct S', 'test_struct_2'], 0))
        self.assertEqual(graph['const unsigned long', 'test_struct_2'], graph.get_struct_member(graph['const struct S', 'test_struct_2'], 8))
        self.assertEqual(graph['const long *const', 'test_struct_2'], graph.get_struct_member(graph['const struct S', 'test_struct_2'], 16))
        self.assertEqual(graph['long *const', 'test_struct_2'], graph.get_struct_member(graph['const struct S', 'test_struct_2'], 24))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['const long', 'test_struct_2']))
        self.assertIn(graph['unsigned long', 'capture'], graph.get_casts(graph['const unsigned long', 'test_struct_2']))
        self.assertIn(graph['const long *', 'capture'], graph.get_casts(graph['const long *const', 'test_struct_2']))
        self.assertIn(graph['long *', 'capture'], graph.get_casts(graph['long *const', 'test_struct_2']))
        # test_struct_3
        self.assertEqual(graph['const long *const', 'test_struct_3'], graph.get_struct_member(graph['const struct S', 'test_struct_3'], 16))
        self.assertEqual(graph['long *const', 'test_struct_3'], graph.get_struct_member(graph['const struct S', 'test_struct_3'], 24))
        self.assertEqual(graph['const long', 'test_struct_3'], graph.get_pointsto(graph['const long *const', 'test_struct_3']))
        self.assertEqual(graph['long', 'test_struct_3'], graph.get_pointsto(graph['long *const', 'test_struct_3']))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['const long', 'test_struct_3']))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['long', 'test_struct_3']))
        # test_structdef_1
        self.assertEqual(graph['long', 'test_structdef_1'], graph.get_struct_member(graph['S2', 'test_structdef_1'], 0))
        self.assertEqual(graph['const unsigned long', 'test_structdef_1'], graph.get_struct_member(graph['S2', 'test_structdef_1'], 8))
        self.assertEqual(graph['const long *', 'test_structdef_1'], graph.get_struct_member(graph['S2', 'test_structdef_1'], 16))
        self.assertEqual(graph['long *const', 'test_structdef_1'], graph.get_struct_member(graph['S2', 'test_structdef_1'], 24))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['long', 'test_structdef_1']))
        self.assertIn(graph['unsigned long', 'capture'], graph.get_casts(graph['const unsigned long', 'test_structdef_1']))
        self.assertIn(graph['const long *', 'capture'], graph.get_casts(graph['const long *', 'test_structdef_1']))
        self.assertIn(graph['long *', 'capture'], graph.get_casts(graph['long *const', 'test_structdef_1']))
        # test_structdef_2
        self.assertEqual(graph['const long', 'test_structdef_2'], graph.get_struct_member(graph['const S2', 'test_structdef_2'], 0))
        self.assertEqual(graph['const unsigned long', 'test_structdef_2'], graph.get_struct_member(graph['const S2', 'test_structdef_2'], 8))
        self.assertEqual(graph['const long *const', 'test_structdef_2'], graph.get_struct_member(graph['const S2', 'test_structdef_2'], 16))
        self.assertEqual(graph['long *const', 'test_structdef_2'], graph.get_struct_member(graph['const S2', 'test_structdef_2'], 24))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['const long', 'test_structdef_2']))
        self.assertIn(graph['unsigned long', 'capture'], graph.get_casts(graph['const unsigned long', 'test_structdef_2']))
        self.assertIn(graph['const long *', 'capture'], graph.get_casts(graph['const long *const', 'test_structdef_2']))
        self.assertIn(graph['long *', 'capture'], graph.get_casts(graph['long *const', 'test_structdef_2']))
        # test_structdef_3
        self.assertEqual(graph['const long *const', 'test_structdef_3'], graph.get_struct_member(graph['const S2', 'test_structdef_3'], 16))
        self.assertEqual(graph['long *const', 'test_structdef_3'], graph.get_struct_member(graph['const S2', 'test_structdef_3'], 24))
        self.assertEqual(graph['const long', 'test_structdef_3'], graph.get_pointsto(graph['const long *const', 'test_structdef_3']))
        self.assertEqual(graph['long', 'test_structdef_3'], graph.get_pointsto(graph['long *const', 'test_structdef_3']))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['const long', 'test_structdef_3']))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['long', 'test_structdef_3']))
        # test_structdef_4
        self.assertEqual(graph['const long *const', 'test_structdef_4'], graph.get_struct_member(graph['const struct S', 'test_structdef_4'], 16))
        self.assertEqual(graph['long *const', 'test_structdef_4'], graph.get_struct_member(graph['const struct S', 'test_structdef_4'], 24))
        self.assertEqual(graph['const long', 'test_structdef_4'], graph.get_pointsto(graph['const long *const', 'test_structdef_4']))
        self.assertEqual(graph['long', 'test_structdef_4'], graph.get_pointsto(graph['long *const', 'test_structdef_4']))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['const long', 'test_structdef_4']))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['long', 'test_structdef_4']))
        # test_struct_ptr1
        self.assertEqual(graph['const struct S', 'test_struct_ptr_1'], graph.get_pointsto(graph['const struct S *', 'test_struct_ptr_1']))
        self.assertEqual(graph['const long', 'test_struct_ptr_1'], graph.get_struct_member(graph['const struct S', 'test_struct_ptr_1'], 0))
        self.assertEqual(graph['const unsigned long', 'test_struct_ptr_1'], graph.get_struct_member(graph['const struct S', 'test_struct_ptr_1'], 8))
        self.assertEqual(graph['const long *const', 'test_struct_ptr_1'], graph.get_struct_member(graph['const struct S', 'test_struct_ptr_1'], 16))
        self.assertEqual(graph['long *const', 'test_struct_ptr_1'], graph.get_struct_member(graph['const struct S', 'test_struct_ptr_1'], 24))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['const long', 'test_struct_ptr_1']))
        # test_struct_ptr2
        self.assertEqual(graph['const S2', 'test_struct_ptr_2'], graph.get_pointsto(graph['const S2 *', 'test_struct_ptr_2']))
        self.assertEqual(graph['const long', 'test_struct_ptr_2'], graph.get_struct_member(graph['const S2', 'test_struct_ptr_2'], 0))
        self.assertEqual(graph['const unsigned long', 'test_struct_ptr_2'], graph.get_struct_member(graph['const S2', 'test_struct_ptr_2'], 8))
        self.assertEqual(graph['const long *const', 'test_struct_ptr_2'], graph.get_struct_member(graph['const S2', 'test_struct_ptr_2'], 16))
        self.assertEqual(graph['long *const', 'test_struct_ptr_2'], graph.get_struct_member(graph['const S2', 'test_struct_ptr_2'], 24))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['const long', 'test_struct_ptr_2']))
        # test_struct_ptr3
        self.assertEqual(graph['struct S', 'test_struct_ptr_3'], graph.get_pointsto(graph['struct S *const', 'test_struct_ptr_3']))
        self.assertEqual(graph['long', 'test_struct_ptr_3'], graph.get_struct_member(graph['struct S', 'test_struct_ptr_3'], 0))
        self.assertEqual(graph['const unsigned long', 'test_struct_ptr_3'], graph.get_struct_member(graph['struct S', 'test_struct_ptr_3'], 8))
        self.assertEqual(graph['const long *', 'test_struct_ptr_3'], graph.get_struct_member(graph['struct S', 'test_struct_ptr_3'], 16))
        self.assertEqual(graph['long *const', 'test_struct_ptr_3'], graph.get_struct_member(graph['struct S', 'test_struct_ptr_3'], 24))
        self.assertIn(graph['unsigned long', 'capture'], graph.get_casts(graph['const unsigned long', 'test_struct_ptr_3']))
        # test_struct_ptr4
        self.assertEqual(graph['struct S *const', 'test_struct_ptr_4'], graph.get_pointsto(graph['struct S *const *', 'test_struct_ptr_4']))
        self.assertEqual(graph['struct S', 'test_struct_ptr_4'], graph.get_pointsto(graph['struct S *const', 'test_struct_ptr_4']))
        # test_array_1
        self.assertEqual(graph['long', 'test_array_1'], graph.get_pointsto(graph['long *', 'test_array_1']))
        self.assertEqual(graph['const unsigned long', 'test_array_1'], graph.get_pointsto(graph['const unsigned long *', 'test_array_1']))
        self.assertIn(graph['long *', 'capture'], graph.get_casts(graph['long *', 'test_array_1']))
        self.assertIn(graph['const unsigned long *', 'capture'], graph.get_casts(graph['const unsigned long *', 'test_array_1']))
        # test_array_2
        self.assertEqual(graph['long', 'test_array_2'], graph.get_pointsto(graph['long *', 'test_array_2']))
        self.assertEqual(graph['const unsigned long', 'test_array_2'], graph.get_pointsto(graph['const unsigned long *', 'test_array_2']))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['long', 'test_array_2']))
        self.assertIn(graph['unsigned long', 'capture'], graph.get_casts(graph['const unsigned long', 'test_array_2']))
        # test_array_3
        self.assertEqual(graph['const struct S', 'test_array_3'], graph.get_pointsto(graph['const struct S *', 'test_array_3']))
        self.assertEqual(graph['const long', 'test_array_3'], graph.get_struct_member(graph['const struct S', 'test_array_3'], 0))
        self.assertEqual(graph['const unsigned long', 'test_array_3'], graph.get_struct_member(graph['const struct S', 'test_array_3'], 8))
        self.assertEqual(graph['const long *const', 'test_array_3'], graph.get_struct_member(graph['const struct S', 'test_array_3'], 16))
        self.assertEqual(graph['long *const', 'test_array_3'], graph.get_struct_member(graph['const struct S', 'test_array_3'], 24))
        self.assertIn(graph['const long *', 'capture'], graph.get_casts(graph['const long *const', 'test_array_3']))
        # test_fixed_array_1
        self.assertEqual(graph['long', 'test_fixed_array_1'], graph.get_pointsto(graph['long *', 'test_fixed_array_1']))
        self.assertEqual(graph['const unsigned long', 'test_fixed_array_1'], graph.get_pointsto(graph['const unsigned long *', 'test_fixed_array_1']))
        self.assertIn(graph['long *', 'capture'], graph.get_casts(graph['long *', 'test_fixed_array_1']))
        self.assertIn(graph['const unsigned long *', 'capture'], graph.get_casts(graph['const unsigned long *', 'test_fixed_array_1']))
        # test_fixed_array_2
        self.assertEqual(graph['long', 'test_fixed_array_2'], graph.get_pointsto(graph['long *', 'test_fixed_array_2']))
        self.assertEqual(graph['const unsigned long', 'test_fixed_array_2'], graph.get_pointsto(graph['const unsigned long *', 'test_fixed_array_2']))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['long', 'test_fixed_array_2']))
        self.assertIn(graph['unsigned long', 'capture'], graph.get_casts(graph['const unsigned long', 'test_fixed_array_2']))
        # test_fixed_array_3
        self.assertEqual(graph['const struct S', 'test_fixed_array_3'], graph.get_pointsto(graph['const struct S *', 'test_fixed_array_3']))
        self.assertEqual(graph['const long', 'test_fixed_array_3'], graph.get_struct_member(graph['const struct S', 'test_fixed_array_3'], 0))
        self.assertEqual(graph['const unsigned long', 'test_fixed_array_3'], graph.get_struct_member(graph['const struct S', 'test_fixed_array_3'], 8))
        self.assertEqual(graph['const long *const', 'test_fixed_array_3'], graph.get_struct_member(graph['const struct S', 'test_fixed_array_3'], 16))
        self.assertEqual(graph['long *const', 'test_fixed_array_3'], graph.get_struct_member(graph['const struct S', 'test_fixed_array_3'], 24))
        self.assertIn(graph['const long *', 'capture'], graph.get_casts(graph['const long *const', 'test_fixed_array_3']))
        # test_const_array_1 .. _4
        for f in ['test_const_array_1', 'test_const_array_2', 'test_const_array_3', 'test_const_array_4']:
            self.assertEqual(graph['const struct S', f], graph.get_pointsto(graph['const struct S *', f]))
            self.assertEqual(graph['const long', f], graph.get_struct_member(graph['const struct S', f], 0))
            self.assertIn(graph['long', 'capture'], graph.get_casts(graph['const long', f]))
        # test_const_array_var_1
        self.assertIn(graph['const struct S *', 'capture'], graph.get_casts(graph['struct S const[3]', 'test_const_array_var_1']))
        self.assertEqual(graph['const struct S', 'test_const_array_var_1'], graph.get_pointsto(graph['struct S const[3]', 'test_const_array_var_1']))
        self.assertEqual(graph['const long', 'test_const_array_var_1'], graph.get_struct_member(graph['const struct S', 'test_const_array_var_1'], 0))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['const long', 'test_const_array_var_1']))
        # test_const_array_var_2
        self.assertIn(graph['const struct S *', 'capture'], graph.get_casts(graph['struct S const[3]', 'test_const_array_var_2']))
        self.assertEqual(graph['const struct S', 'test_const_array_var_2'], graph.get_pointsto(graph['struct S const[3]', 'test_const_array_var_2']))
        self.assertEqual(graph['const long', 'test_const_array_var_2'], graph.get_struct_member(graph['const struct S', 'test_const_array_var_2'], 0))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['const long', 'test_const_array_var_2']))
        # test_const_array_var_3
        self.assertIn(graph['const struct S *', 'capture'], graph.get_casts(graph['struct S const[1]', 'test_const_array_var_3']))
        self.assertEqual(graph['const struct S', 'test_const_array_var_3'], graph.get_pointsto(graph['struct S const[1]', 'test_const_array_var_3']))
        self.assertEqual(graph['const long', 'test_const_array_var_3'], graph.get_struct_member(graph['const struct S', 'test_const_array_var_3'], 0))
        self.assertEqual(graph['const long *const', 'test_const_array_var_3'], graph.get_struct_member(graph['const struct S', 'test_const_array_var_3'], 16))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['const long', 'test_const_array_var_3']))
        #self.assertIn(graph['const long *const', 'test_const_array_var_3'], graph.get_casts(graph['void *', 'test_const_array_var_3']))
        #self.assertIn(graph['long *const', 'test_const_array_var_3'], graph.get_casts(graph['void *', 'test_const_array_var_3']))
        # test_struct_var
        self.assertEqual(graph['const long', 'test_struct_var'], graph.get_struct_member(graph['const struct S', 'test_struct_var'], 0))
        self.assertEqual(graph['const unsigned long', 'test_struct_var'], graph.get_struct_member(graph['const struct S', 'test_struct_var'], 8))
        self.assertEqual(graph['const long *const', 'test_struct_var'], graph.get_struct_member(graph['const struct S', 'test_struct_var'], 16))
        self.assertEqual(graph['long *const', 'test_struct_var'], graph.get_struct_member(graph['const struct S', 'test_struct_var'], 24))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['const long', 'test_struct_var']))
        self.assertIn(graph['unsigned long', 'capture'], graph.get_casts(graph['const unsigned long', 'test_struct_var']))
        self.assertIn(graph['const long *', 'capture'], graph.get_casts(graph['const long *const', 'test_struct_var']))
        self.assertIn(graph['long *', 'capture'], graph.get_casts(graph['long *const', 'test_struct_var']))
        # test_structdef_var
        self.assertEqual(graph['const long', 'test_structdef_var'], graph.get_struct_member(graph['const S2', 'test_structdef_var'], 0))
        self.assertEqual(graph['const unsigned long', 'test_structdef_var'], graph.get_struct_member(graph['const S2', 'test_structdef_var'], 8))
        self.assertEqual(graph['const long *const', 'test_structdef_var'], graph.get_struct_member(graph['const S2', 'test_structdef_var'], 16))
        self.assertEqual(graph['long *const', 'test_structdef_var'], graph.get_struct_member(graph['const S2', 'test_structdef_var'], 24))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['const long', 'test_structdef_var']))
        self.assertIn(graph['unsigned long', 'capture'], graph.get_casts(graph['const unsigned long', 'test_structdef_var']))
        self.assertIn(graph['const long *', 'capture'], graph.get_casts(graph['const long *const', 'test_structdef_var']))
        self.assertIn(graph['long *', 'capture'], graph.get_casts(graph['long *const', 'test_structdef_var']))

    def test_const_var_initializers(self):
        SAMPLE = '''
        void capture(int x, ...) {}
        void capture2(long x) {}
        struct S {
            long l;
            const unsigned long u;
            const long *p1;
            long *const p2;
        };
        struct S4 {
            void (*f)(int);
            void (*g)(long, long);
        };
        typedef void (*funcptr1)(int);
        typedef void (*funcptr2)(long, long);
        static void some_g(long l1, long l2) {}
        
        void test_const_init_1(void *p) { const struct S s[] = {{0,0,p,p}}; capture(0, s[0].l, s); }
        void test_const_init_2(const long *p, char *q) { const struct S s[] = {{0,0,p,(void*) q}}; capture2(s[0].l); }
        void test_const_init_3(void (*f)(int)) { const struct S4 s[] = {{f, (&some_g)}}; capture(0, &s[0], s->g); }
        void test_const_init_4(funcptr1 f) { const struct S4 s[] = {{f, ((funcptr2) f)}}; capture(0, &s); }
        void test_const_init_5(void *p) { const struct Sx { unsigned long * const l } s[] = {{p}}; capture(0, s[0].l); }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX_HEADER + SAMPLE)
        code.env['TG_CLANG_MINIMIZE_GRAPH'] = '0'
        graph = compile_to_graph(code)
        for f in ['test_const_init_1', 'test_const_init_2']:
            self.assertEqual(graph['const long', f], graph.get_struct_member(graph['const struct S', f], 0))
            self.assertEqual(graph['const unsigned long', f], graph.get_struct_member(graph['const struct S', f], 8))
            self.assertEqual(graph['const long *const', f], graph.get_struct_member(graph['const struct S', f], 16))
            self.assertEqual(graph['long *const', f], graph.get_struct_member(graph['const struct S', f], 24))
        # test_const_init_1
        self.assertEqual(graph['const struct S', 'test_const_init_1'], graph.get_pointsto(graph['struct S const[1]', 'test_const_init_1']))
        self.assertIn(graph['long', 'capture'], graph.get_casts(graph['const long', 'test_const_init_1']))
        self.assertIn(graph['const long *const', 'test_const_init_1'], graph.get_casts(graph['void *', 'test_const_init_1']))
        self.assertIn(graph['long *const', 'test_const_init_1'], graph.get_casts(graph['void *', 'test_const_init_1']))
        # test_const_init_2
        self.assertIn(graph['long', 'capture2'], graph.get_casts(graph['const long', 'test_const_init_2']))
        self.assertIn(graph['const long *const', 'test_const_init_2'], graph.get_casts(graph['const long *', 'test_const_init_2']))
        self.assertIn(graph['long *const', 'test_const_init_2'], graph.get_casts(graph['char *', 'test_const_init_2']))
        # test_const_init_3+4
        for f in ['test_const_init_3', 'test_const_init_4']:
            self.assertEqual(graph['const struct S4', f], graph.get_pointsto(graph['struct S4 const[1]', f]))
            self.assertEqual(graph['void (*const)(int)', f], graph.get_struct_member(graph['const struct S4', f], 0))
            self.assertEqual(graph['void (*const)(long, long)', f], graph.get_struct_member(graph['const struct S4', f], 8))
        # test_const_init_3
        self.assertIn(graph['void (*)(long, long)', 'capture'], graph.get_casts(graph['void (*const)(long, long)', 'test_const_init_3']))
        self.assertIn(graph['const struct S4 *', 'capture'], graph.get_casts(graph['const struct S4 *', 'test_const_init_3']))
        self.assertEquals(graph['const struct S4', 'test_const_init_3'], graph.get_pointsto(graph['const struct S4 *', 'test_const_init_3']))
        some_g = list(graph.usedFunctions['some_g'])[0]
        self.assertIn(graph['void (*const)(long, long)', 'test_const_init_3'], graph.get_casts(some_g))
        self.assertIn(graph['void (*const)(int)', 'test_const_init_3'], graph.get_casts(graph['void (*)(int)', 'test_const_init_3']))
        # test_const_init_4
        self.assertIn(graph['struct S4 const (*)[1]', 'capture'], graph.get_casts(graph['struct S4 const (*)[1]', 'test_const_init_4']))
        self.assertEquals(graph['struct S4 const[1]', 'test_const_init_4'], graph.get_pointsto(graph['struct S4 const (*)[1]', 'test_const_init_4']))
        self.assertIn(graph['void (*const)(long, long)', 'test_const_init_4'], graph.get_casts(graph['void (*)(int)', 'test_const_init_4']))
        self.assertIn(graph['void (*const)(int)', 'test_const_init_4'], graph.get_casts(graph['void (*)(int)', 'test_const_init_4']))
        # test_const_init_5
        self.assertEqual(graph['unsigned long *const', 'test_const_init_5'], graph.get_struct_member(graph['const struct Sx', 'test_const_init_5'], 0))
        self.assertEqual(graph['unsigned long', 'test_const_init_5'], graph.get_pointsto(graph['unsigned long *const', 'test_const_init_5']))
        self.assertIn(graph['unsigned long *', 'capture'], graph.get_casts(graph['unsigned long *const', 'test_const_init_5']))
        self.assertIn(graph['unsigned long *const', 'test_const_init_5'], graph.get_casts(graph['void *', 'test_const_init_5']))


class VarArgTestCases(OneCallTestCase):
    SAMPLE = '''
    void target(int a, ...) { functype f; f(1); }
    void test() { target(0, 1, &f1, &g1); }
    '''
    VALID_FUNCTIONS = ['f1']

    def test_simple_vararg(self):
        # vararg function call => context transfer for additional param works?
        SAMPLE = '''
        void test1(unsigned long a, ...) {}
        void test2() {
            long b;
            const char c = 'a';
            unsigned* d;
            unsigned long l = 1;
            test1(l, &b, &c, d);
        }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX_HEADER + SAMPLE)
        code.env['TG_CLANG_MINIMIZE_GRAPH'] = '0'
        graph = compile_to_graph(code)
        assert graph.is_reachable_default(graph['long *', 'test2'], graph['long *', 'test1'])
        assert graph.is_reachable_default(graph['unsigned long', 'test2'], graph['unsigned long', 'test1'])
        assert graph.is_reachable_default(graph['const char *', 'test2'], graph['const char *', 'test1'])
        assert graph.is_reachable_default(graph['unsigned int *', 'test2'], graph['unsigned int *', 'test1'])

        code = SourceCode.from_cxx(SAMPLE_PREFIX_HEADER + SAMPLE)
        code.env['TG_CLANG_MINIMIZE_GRAPH'] = '0'
        graph = compile_to_graph(code)
        assert graph.is_reachable_default(graph['long *', '_Z5test2v'], graph['long *', '_Z5test1mz'])
        assert graph.is_reachable_default(graph['unsigned long', '_Z5test2v'], graph['unsigned long', '_Z5test1mz'])
        assert graph.is_reachable_default(graph['const char *', '_Z5test2v'], graph['const char *', '_Z5test1mz'])
        assert graph.is_reachable_default(graph['unsigned int *', '_Z5test2v'], graph['unsigned int *', '_Z5test1mz'])

    def test_vararg_fp(self):
        # vararg function pointer => cast + context transfer working?
        SAMPLE = '''
        typedef void (*functype3)(int, ...);
        void target(functype3 f) { f(1,2,3); }
        void test() { target((functype3) f1); }
        '''
        self._run_test(SourceCode.from_string(SAMPLE_PREFIX + SAMPLE), ['f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_indirect_resolution_with_vararg(self):
        # indirect resolution with vararg
        SAMPLE = '''
        typedef void (*secondorder_function_1)(int, ...);
        typedef int (*secondorder_function_2)(int, ...);
        void noinline target(int i, ...) { functype f; f(1); }
        int noinline so2(int i, ...) { return 123; }
        void noinline pass_f1_to_so(int x, ...) { secondorder_function_1 p_so1; p_so1(1, f1); }
        void noinline pass_f2_to_so(int x, ...) { secondorder_function_2 p_so2; p_so2(2, f2); }
        void noinline test() { pass_f1_to_so(0, target); pass_f2_to_so(1, so2); }
        '''
        self._run_test(SourceCode.from_string(SAMPLE_PREFIX + SAMPLE), ['f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])


class IndirectFunctionTransferTestCase(OneCallTestCase):
    SAMPLE = '''
    typedef void (*secondorder_function_1)(functype f, int);
    typedef int (*secondorder_function_2)(functype f, int);
    void noinline target(functype f, int i) { f(1); }
    int noinline so2(functype f, int i) { return 123; }
    void noinline pass_f1_to_so(secondorder_function_1 p_so1) { p_so1(f1, 1); }
    void noinline pass_f2_to_so(secondorder_function_2 p_so2) { p_so2(f2, 2); }
    void noinline test() { pass_f1_to_so(target); pass_f2_to_so(so2); }
    '''
    VALID_FUNCTIONS = ['f1']

    def test_func_returning_func(self):
        SAMPLE = '''
        typedef functype (*secondorder_function_2)(int);
        functype noinline get_functions(int i) {
            if (i == 1) return f1;
            else return (functype) g1;
        }
        void noinline target(functype f) { f(1); }
        void noinline test(secondorder_function_2 getter) { target(getter(1)); }
        void noinline test2() { test(&get_functions); }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1', 'g1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1', 'g1'])

    def test_indirect_with_cast_in_other_module(self):
        HDR = SAMPLE_PREFIX_HEADER + '''
        typedef char* (*functype3)(void*);
        noinline char* indirect_transfer(unsigned long *x, const functype3 f);
        noinline char* mycast(void* x);
        '''
        SAMPLE = '''
        char* indirect_transfer(unsigned long *x, const functype3 f) {
            functype3 func = f;
            return func(x);
        }
        char* mycast(void* x) { return (char*) x; }
        '''
        SAMPLE2 = '''
        noinline void target() {
            functype2 f = (functype2) indirect_transfer((unsigned long*) f1, mycast);
            f(1);
        }
        '''
        code = SourceCode.from_string(HDR + SAMPLE)
        code.add_file(SourceCodeFile(HDR + SAMPLE2))
        code.env['TG_CLANG_MINIMIZE_GRAPH'] = '0'
        self._run_test(code, ['f1'])
        self._run_test(code.to_cxx(), ['f1'])

        code.env['TG_CLANG_MINIMIZE_GRAPH'] = '1'
        self._run_test(code, ['f1'])
        self._run_test(code.to_cxx(), ['f1'])

    def test_confuse_equivalence(self):
        COMMON_HEADER = '''
        struct S;
        struct S* join_pointers();
        
        typedef void (*register_type)(functype);
        noinline register_type get_register();
        extern register_type global_reg;
        '''
        SAMPLE0 = SAMPLE_PREFIX_HEADER + COMMON_HEADER + '''
        struct S { functype ptr; };
        static struct S s;
        struct S* join_pointers() {
            return &s;
        }
        '''
        SAMPLE1 = SAMPLE_PREFIX + COMMON_HEADER + '''
        struct S2 { functype ptr; };
        static functype global_f;
        static void register_function(functype f) {
            struct S2* s = (struct S2*) join_pointers();
            global_f = f;
        }
        register_type global_reg = register_function;
        
        void noinline target() { global_f(1); }
        static void noinline register_stuff() {
            register_type reg = get_register();
            reg(f1);
            reg(f2);
        }
        
        int main() {
            register_stuff();
            target();
            return 0;
        }
        '''
        SAMPLE2 = SAMPLE_PREFIX_HEADER + COMMON_HEADER + '''
        noinline register_type get_register() { return global_reg; }
        '''
        code = SourceCode.from_string(SAMPLE0)
        code.add_file(SourceCodeFile(SAMPLE1))
        code.add_file(SourceCodeFile(SAMPLE2))
        binary = compile_link(code, output_graph=True)
        # graph_before = Typegraph.load_from_file(binary + '.before.typegraph')
        # graph_equivalent = Typegraph.load_from_file(binary + '.equivalent.typegraph')
        graph_after = Typegraph.load_from_file(binary + '.after.typegraph')
        # graph_before.get_interesting_subgraph().build_dot(include_number=True, include_additional_names=True)
        # graph_equivalent.get_interesting_subgraph().build_dot(include_number=True, include_additional_names=True)
        # graph_after.get_interesting_subgraph().build_dot(include_number=True, include_additional_names=True)
        funcs = graph_after.get_call_first_functions(lambda call: ' in target' in call or '6target' in call)
        self.assertEqual(['f1', 'f2'], funcs)
        
        # same with dynlink enforcing
        code.env['TG_DYNLIB_SUPPORT'] = '1'
        binary = compile_link(code, output_graph=True, enforce=True)
        graph_after = Typegraph.load_from_file(binary + '.after.typegraph')
        funcs = graph_after.get_call_first_functions(lambda call: ' in target' in call or '6target' in call)
        self.assertEqual(['f1', 'f2'], funcs)

    def test_transfer_with_different_types(self):
        SAMPLE = '''
        typedef void (*higherorder)(char, void*);
        void noinline set_fp(char c, void** x) {
            *x = (void*) (c == '1' ? f1 : f2);
        }
        higherorder noinline get_setter() { return (higherorder)(void*)set_fp; }
        void noinline target(functype f) { f(1); }
        void test(char c) {
            functype f = NULL;
            get_setter()(c, &f);
            target(f);
        }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1', 'f2'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1', 'f2'])

    def test_transfer_with_different_types_v2(self):
        HEADER = '''
        typedef void (*higherorder)(char, void*);
        higherorder noinline get_setter();
        '''
        SAMPLE = '''
        void noinline set_fp(char c, void** x) {
            *x = (c == '1' ? f1 : f2);
        }
        higherorder noinline get_setter() { return (void*)set_fp; }
        '''
        SAMPLE2 = '''
        functype f = NULL;
        void target(char c) {
            get_setter()(c, (void**) &f);
            f(1);
        }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX + HEADER + SAMPLE)
        code.add_file(SourceCodeFile(SAMPLE_PREFIX_HEADER + HEADER + SAMPLE2))
        graph = compile_to_graph(code, compute_function_usages=True)
        funcs = graph.get_call_first_functions(lambda call: '.1 in target' in call)
        funcs.sort()
        print(funcs)
        self.assertEqual(['f1', 'f2'], funcs)




class FunctionNamingTestCases(OneCallTestCase):
    SAMPLE = '''
    static functype transfer_pointers(functype2 g) { return (functype) g; }
    void noinline target(functype f) { f(1); }
    void noinline test() {
        target(f1);
        target(transfer_pointers(g1));
    }
    '''
    VALID_FUNCTIONS = ['f1', 'g1']

    def test_anonymous_namespaces_cxx(self):
        SAMPLE = '''
        namespace {
        functype transfer_pointers(functype2 g) { return (functype) g; }
        }
        void noinline target(functype f) { f(1); }
        void noinline test() {
            target(f1);
            target(transfer_pointers(g1));
        }
        '''
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), self.VALID_FUNCTIONS)

    def test_namespaces(self):
        SAMPLE = '''
        namespace a {
            functype transfer_pointers(functype2 g) { return (functype) g; }
        }
        namespace b {
            functype transfer_pointers(functype2 g) { return nullptr; }
        }
        void noinline target(functype f) { f(1); }
        '''
        V1 = '''
        void noinline test() { target(a::transfer_pointers(g1)); }
        void noinline confusion() { b::transfer_pointers(g2); }
        '''
        V2 = '''
        void noinline test() { target(b::transfer_pointers(g1)); }
        void noinline confusion() { a::transfer_pointers(g2); }
        '''
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE + V1), ['g1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE + V2), [])

    def test_multi_module_static(self):
        SAMPLE_1 = '''
        static noinline functype test(functype2 g) { return (functype) g; }
        '''
        SAMPLE_2 = '''
        static noinline functype test(functype2 g) { return NULL; }
        '''
        CHECKING = '''
        void noinline target(functype f) { f(1); }
        void transfer() { target(test(g1)); }
        '''
        for read_from_ir in (False, True):
            code = SourceCode(SAMPLE_PREFIX_HEADER)
            code.add_file(SourceCodeFile(SAMPLE_1 + CHECKING))
            code.add_file(SourceCodeFile(SAMPLE_2))
            code.add_file(SourceCodeFile(SAMPLE_PREFIX_DEFS))
            self._run_test(code, ['g1'], read_from_ir=read_from_ir)
            for f in code.files.values():
                f.compiler = 'CXX'
            self._run_test(code, ['g1'], read_from_ir=read_from_ir)
            code = SourceCode(SAMPLE_PREFIX_HEADER)
            code.add_file(SourceCodeFile(SAMPLE_1))
            code.add_file(SourceCodeFile(SAMPLE_2 + CHECKING))
            code.add_file(SourceCodeFile(SAMPLE_PREFIX_DEFS))
            self._run_test(code, [], read_from_ir=read_from_ir)
            for f in code.files.values():
                f.compiler = 'CXX'
            self._run_test(code, [], read_from_ir=read_from_ir)

    def test_multi_module_namespace(self):
        SAMPLE_1 = '''
        namespace{ noinline functype test(functype2 g) { return (functype) g; } }
        '''
        SAMPLE_2 = '''
        namespace{ noinline functype test(functype2 g) { return nullptr; } }
        '''
        CHECKING = '''
        void noinline target(functype f) { f(1); }
        void transfer() { target(test(g1)); }
        '''
        for read_from_ir in (False, True):
            code = SourceCode(SAMPLE_PREFIX_HEADER)
            code.add_file(SourceCodeFile(SAMPLE_PREFIX_DEFS, 'CXX'))
            code.add_file(SourceCodeFile(SAMPLE_1 + CHECKING, 'CXX'))
            code.add_file(SourceCodeFile(SAMPLE_2, 'CXX'))
            self._run_test(code, ['g1'], read_from_ir=read_from_ir)
            code = SourceCode(SAMPLE_PREFIX_HEADER)
            code.add_file(SourceCodeFile(SAMPLE_PREFIX_DEFS, 'CXX'))
            code.add_file(SourceCodeFile(SAMPLE_1, 'CXX'))
            code.add_file(SourceCodeFile(SAMPLE_2 + CHECKING, 'CXX'))
            self._run_test(code, [], read_from_ir=read_from_ir)

    def test_functions_with_same_name_used(self):
        SAMPLE1 = '''
        static void f(int x) {}
        noinline functype get_fp1() { return &f; }
        '''
        SAMPLE1_CXX = '''
        namespace { void f(int x) {} }
        noinline functype get_fp1() { return &f; }
        '''
        SAMPLE2 = '''
        static void f(int x) {}
        noinline functype get_fp2() { return &f; }
        '''
        SAMPLE2_CXX = '''
        namespace { void f(int x) {} }
        noinline functype get_fp2() { return &f; }
        '''
        SAMPLE3 = '''
        noinline functype get_fp1();
        noinline functype get_fp2();
        noinline void target(functype f) { f(1); }
        noinline void test() { XYZ }
        noinline void test2() { get_fp1(); get_fp2(); }
        '''
        read_from_ir = True
        for xyz, l in [('target(get_fp1());', 1), ('target(get_fp2());', 1), ('target(get_fp1()); target(get_fp2());', 2)]:
            code = SourceCode(SAMPLE_PREFIX_HEADER)
            code.add_file(SourceCodeFile(SAMPLE1))
            code.add_file(SourceCodeFile(SAMPLE2))
            code.add_file(SourceCodeFile(SAMPLE_PREFIX_DEFS + SAMPLE3.replace('XYZ', xyz)))
            g = compile_to_graph(code, compute_function_usages=True, read_from_ir=read_from_ir)
            funcs = g.get_call_first_functions(lambda call: ' in target' in call or ' in _Z6target' in call)
            # print(xyz, read_from_ir, funcs)
            print(funcs)
            self.assertEqual(len(funcs), l, f'Fail: xyz={xyz} read_from_ir={read_from_ir}')

            # static, but with C++
            for f in code.files.values(): f.compiler = 'CXX'
            g = compile_to_graph(code, compute_function_usages=True, read_from_ir=read_from_ir)
            funcs = g.get_call_first_functions(lambda call: ' in target' in call or ' in _Z6target' in call)
            # print(xyz, read_from_ir, funcs)
            self.assertEqual(len(funcs), l, f'Fail: xyz={xyz} read_from_ir={read_from_ir}')

            # do the same, but with namespaces
            code = SourceCode(SAMPLE_PREFIX_HEADER)
            code.add_file(SourceCodeFile(SAMPLE1_CXX, 'CXX'))
            code.add_file(SourceCodeFile(SAMPLE2_CXX, 'CXX'))
            code.add_file(SourceCodeFile(SAMPLE_PREFIX_DEFS + SAMPLE3.replace('XYZ', xyz), 'CXX'))
            g = compile_to_graph(code, compute_function_usages=True, read_from_ir=read_from_ir)
            funcs = g.get_call_first_functions(lambda call: ' in target' in call or ' in _Z6target' in call)
            # print(xyz, read_from_ir, funcs)
            self.assertEqual(len(funcs), l, f'Fail: xyz={xyz} read_from_ir={read_from_ir}')

    def test_linkage_names(self):
        SAMPLE = '''
        #include <stdio.h>

        #define hidden __attribute__((visibility("hidden"))) extern
        static void visible_locally(long l) { printf("%ld", l); }
        hidden void visible_in_dso(long l) { printf("%ld", l); }
        void visible_outside_dso(long l) { printf("%ld", l); }

        void test(long l) {
            visible_locally(l);
            visible_in_dso(l);
            visible_outside_dso(l);
        }
        '''
        code = SourceCode.from_string(SAMPLE)
        for f in code.files.values():
            f.env['TG_CLANG_MINIMIZE_GRAPH'] = '0'
        graph = compile_to_graph(code, apply_typegraph_tool=False)
        self.assertIn(('long', 'visible_outside_dso'), graph.type_context_to_vertex)
        self.assertIn(('long', 'visible_in_dso'), graph.type_context_to_vertex)
        # local symbols must have random name
        self.assertNotIn(('long', 'visible_locally'), graph.type_context_to_vertex)
        assert any(t == 'long' and 'visible_locally' in c for t, c in graph.type_context_to_vertex)

    def test_linkage_visibility(self):
        SAMPLE = '''
        #include <stdio.h>
        
        #define hidden __attribute__((visibility("hidden"))) extern
        static void visible_locally(long l) { printf("%ld", l); }
        hidden void visible_in_dso(long l) { printf("%ld", l); }
        void visible_outside_dso(long l) { printf("%ld", l); }
        
        void test(long l) {
            visible_locally(l);
            visible_in_dso(l);
            visible_outside_dso(l);
        }
        '''
        code = SourceCode.from_string(SAMPLE).make_shared()
        for f in code.files.values():
            f.env['TG_CLANG_MINIMIZE_GRAPH'] = '1'
        graph = compile_to_graph(code, apply_typegraph_tool=False)
        # visible_locally must not be exported (also not in graph), everything else must be!
        self.assertIn(('long', 'visible_outside_dso'), graph.type_context_to_vertex)
        self.assertIn(('long', 'visible_in_dso'), graph.type_context_to_vertex)
        self.assertNotIn(('long', 'visible_locally'), graph.type_context_to_vertex)
        for t, c in graph.type_context_to_vertex.keys():
            assert 'visible_locally' not in c
        assert graph.is_interface_defined('visible_outside_dso')
        assert graph.is_interface_defined('visible_in_dso')
        assert not graph.is_interface_defined('visible_locally')
        # in the final program, the hidden symbol must not be exported
        binary = compile_link(code, output_graph=True, enforce=True)
        os.remove(binary)
        graph2 = Typegraph.load_from_file(f'{binary}.min.typegraph')
        self.assertIn(('long', 'visible_outside_dso'), graph2.type_context_to_vertex)
        self.assertNotIn(('long', 'visible_in_dso'), graph2.type_context_to_vertex)
        self.assertNotIn(('long', 'visible_locally'), graph2.type_context_to_vertex)
        assert graph2.is_interface_defined('visible_outside_dso')
        assert not graph2.is_interface_defined('visible_in_dso')
        assert not graph2.is_interface_defined('visible_locally')



class GlobalNamingTestCases(OneCallTestCase):
    SAMPLE = '''
    static functype glob = f1;
    void noinline target(functype f) { f(1); }
    void noinline test() { target(glob); }
    '''
    VALID_FUNCTIONS = ['f1']

    def test_multi_module_static(self):
        SAMPLE_1 = '''
        static functype glob = f1;
        noinline void use1() { if (glob != 0) glob(1); glob = 0; }
        '''
        SAMPLE_2 = '''
        static functype glob = f2;
        noinline void use2() { if (glob != 0) glob(1); glob = 0; }
        '''
        CHECKING = '''
        void use2();
        void use1();
        void noinline target(functype f) { f(1); }
        void transfer() { target(glob); }
        int main() { use1(); use2(); transfer(); return 0; }
        '''
        for read_from_ir in (False, True):
            code = SourceCode(SAMPLE_PREFIX_HEADER)
            code.add_file(SourceCodeFile(SAMPLE_1 + CHECKING))
            code.add_file(SourceCodeFile(SAMPLE_2))
            code.add_file(SourceCodeFile(SAMPLE_PREFIX_DEFS))
            self._run_test(code, ['f1'], read_from_ir=read_from_ir)
            for f in code.files.values():
                f.compiler = 'CXX'
            self._run_test(code, ['f1'], read_from_ir=read_from_ir)
            code = SourceCode(SAMPLE_PREFIX_HEADER)
            code.add_file(SourceCodeFile(SAMPLE_1))
            code.add_file(SourceCodeFile(SAMPLE_2 + CHECKING))
            code.add_file(SourceCodeFile(SAMPLE_PREFIX_DEFS))
            self._run_test(code, ['f2'], read_from_ir=read_from_ir)
            for f in code.files.values():
                f.compiler = 'CXX'
            self._run_test(code, ['f2'], read_from_ir=read_from_ir)

    def test_multi_module_namespace(self):
        SAMPLE_1 = '''
        namespace{ functype glob = f1; }
        noinline void use1() { glob(1); }
        '''
        SAMPLE_2 = '''
        namespace{ functype glob = f2; }
        noinline void use2() { glob(1); }
        '''
        CHECKING = '''
        void noinline target(functype f) { f(1); }
        void transfer() { target(glob); }
        '''
        for read_from_ir in (False, True):
            code = SourceCode(SAMPLE_PREFIX_HEADER)
            code.add_file(SourceCodeFile(SAMPLE_PREFIX_DEFS, 'CXX'))
            code.add_file(SourceCodeFile(SAMPLE_1 + CHECKING, 'CXX'))
            code.add_file(SourceCodeFile(SAMPLE_2, 'CXX'))
            self._run_test(code, ['f1'], read_from_ir=read_from_ir)
            code = SourceCode(SAMPLE_PREFIX_HEADER)
            code.add_file(SourceCodeFile(SAMPLE_PREFIX_DEFS, 'CXX'))
            code.add_file(SourceCodeFile(SAMPLE_1, 'CXX'))
            code.add_file(SourceCodeFile(SAMPLE_2 + CHECKING, 'CXX'))
            self._run_test(code, ['f2'], read_from_ir=read_from_ir)

    def test_multi_module_different_namespace(self):
        SAMPLE = '''
        namespace a { functype glob = f1; }
        noinline void use1() { a::glob(1); }
        namespace b { functype glob = f2; }
        noinline void use2() { b::glob(1); }
        void noinline target(functype f) { f(1); }
        void transfer() { target(XYZ::glob); }
        '''
        for read_from_ir in (False, True):
            self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE.replace('XYZ', 'a')), ['f1'], read_from_ir=read_from_ir)
            self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE.replace('XYZ', 'b')), ['f2'], read_from_ir=read_from_ir)

    def test_multi_module_global_sharing(self):
        # a testcase with "extern" and legit information transfer
        SAMPLE_1 = '''
        extern functype glob;
        noinline void test1() { glob = (functype) g1; }
        '''
        SAMPLE_2 = '''
        extern functype glob;
        noinline void test2() { glob = (functype) g2; }
        '''
        CHECKING = '''
        functype glob = f1;
        void noinline target() { glob(1); }
        '''
        for read_from_ir in (False, True):
            code = SourceCode(SAMPLE_PREFIX_HEADER)
            code.add_file(SourceCodeFile(SAMPLE_1))
            code.add_file(SourceCodeFile(SAMPLE_2))
            code.add_file(SourceCodeFile(CHECKING))
            code.add_file(SourceCodeFile(SAMPLE_PREFIX_DEFS))
            self._run_test(code, ['f1', 'g1', 'g2'], read_from_ir=read_from_ir)
            for f in code.files.values():
                f.compiler = 'CXX'
            self._run_test(code, ['f1', 'g1', 'g2'], read_from_ir=read_from_ir)


class LambdaTestCases(unittest.TestCase):
    def test_lambda_to_fp(self):
        SAMPLE = '''
        noinline functype get_pointer() {
            return +[](int x) { x = x-1; };
        }
        noinline void target() { get_pointer()(1); }
        '''
        g = compile_to_graph(SourceCode.from_cxx(SAMPLE_PREFIX_HEADER + SAMPLE), compute_function_usages=True)
        funcs = g.get_call_first_functions()
        self.assertEqual(len(funcs), 1)
        print(funcs)

        g = compile_to_graph(SourceCode.from_cxx(SAMPLE_PREFIX_HEADER + SAMPLE.replace('+', '')), compute_function_usages=True)
        funcs = g.get_call_first_functions()
        self.assertEqual(len(funcs), 1)

    def test_static_method(self):
        SAMPLE = '''
        struct A { static void f(int x) { x = x-1; } };
        noinline functype get_pointer() { return A::f; }
        noinline void target() { get_pointer()(1); }
        '''
        g = compile_to_graph(SourceCode.from_cxx(SAMPLE_PREFIX_HEADER + SAMPLE), compute_function_usages=True)
        funcs = g.get_call_first_functions()
        self.assertEqual(len(funcs), 1)
        self.assertEqual(['_ZN1A1fEi'], funcs)

    def test_inline_class(self):
        SAMPLE = '''
        noinline functype get_pointer() {
            struct A { static void f(int x) { x = x-1; } };
            return A::f;
        }
        noinline void target() { get_pointer()(1); }
        '''
        g = compile_to_graph(SourceCode.from_cxx(SAMPLE_PREFIX_HEADER + SAMPLE), compute_function_usages=True)
        funcs = g.get_call_first_functions()
        self.assertEqual(len(funcs), 1)
        self.assertEqual(['_ZZ11get_pointervEN1A1fEi'], funcs)

    def test_implicit_used(self):
        SAMPLE = '''
        struct A { static void f(int x){}  operator functype() const { return f; } };
        noinline functype get_pointer() { return A(); }
        noinline void target() { get_pointer()(1); }
        '''
        g = compile_to_graph(SourceCode.from_cxx(SAMPLE_PREFIX_HEADER + SAMPLE), compute_function_usages=True)
        funcs = g.get_call_first_functions()
        self.assertEqual(len(funcs), 1)
        self.assertEqual(['_ZN1A1fEi'], funcs)

    def test_lambda_capturing(self):
        SAMPLE = '''
        noinline void target(functype f) { f(1); }
        noinline void test() {
            functype f = nullptr;
            functype2 g = g1;
            auto l = [&f, g]() { target(f); };
            f = f1;
            l();
        }
        '''
        g = compile_to_graph(SourceCode.from_cxx(SAMPLE_PREFIX_HEADER + SAMPLE), compute_function_usages=True)
        # g.get_interesting_subgraph().build_dot()
        funcs = g.get_call_first_functions()
        self.assertEqual(len(funcs), 1)
        self.assertEqual(['_Z2f1i'], funcs)

    def test_lambda_capturing_v2(self):
        SAMPLE = '''
        noinline void target(functype f) { f(1); }
        noinline void test() {
            functype f = nullptr;
            functype f2x = f2;
            auto l = [&f, f2x]() { if (f) target(f); else target(f2x); };
            f = f1;
            l();
        }
        '''
        g = compile_to_graph(SourceCode.from_cxx(SAMPLE_PREFIX_HEADER + SAMPLE), compute_function_usages=True)
        # g.get_interesting_subgraph().build_dot()
        funcs = list(sorted(g.get_call_first_functions()))
        self.assertEqual(len(funcs), 2)
        self.assertEqual(['_Z2f1i', '_Z2f2i'], funcs)

    def test_lambda_capturing_bidirectional(self):
        SAMPLE = '''
        noinline void target(functype f) { f(1); }
        noinline void test() {
            functype f = nullptr;
            auto l = [&f]() { f = f1; };
            l();
            target(f);
        }
        '''
        g = compile_to_graph(SourceCode.from_cxx(SAMPLE_PREFIX_HEADER + SAMPLE), compute_function_usages=True)
        funcs = g.get_call_first_functions()
        self.assertEqual(len(funcs), 1)
        self.assertEqual(['_Z2f1i'], funcs)


class CXXTestCases(OneCallTestCase):
    SAMPLE = '''
    struct A {
        noinline functype2 transfer(functype f) { return (functype2) f; }
    };
    noinline void target() {
        A a;
        auto x = a.transfer(f1);
        x(1);
    } 
    '''
    VALID_FUNCTIONS = ['f1']

    def test_c(self):
        self.assertTrue(True)

    def test_static_call(self):
        SAMPLE = '''
        struct A {
            noinline static functype2 transfer(functype f) { return (functype2) f; }
        };
        noinline void target() {
            A a;
            auto x = a.transfer(f1);
            x(1);
        } 
        '''
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_ptr_call(self):
        SAMPLE = '''
        struct A {
            noinline functype2 transfer(functype f) { return (functype2) f; }
        };
        noinline void target() {
            A *a = new A();
            auto x = a->transfer(f1);
            x(1);
        } 
        '''
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_this_transfer(self):
        SAMPLE = '''
        struct A {
            functype fp;
            noinline void target() { fp(1); }
        };
        noinline void test() {
            A a;
            a.fp = f1;
            a.target();
        } 
        '''
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_constructor_transfer(self):
        SAMPLE = '''
        struct A {
            functype fp;
            noinline A(functype fp) : fp(fp) {}
            noinline void target() { fp(1); }
        };
        noinline void test() {
            A a(f1);
            a.target();
        } 
        '''
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    @unittest.skip('C++ is future work')
    def test_virtual_function(self):
        SAMPLE = '''
        struct A {
            noinline virtual functype2 transfer(functype f) { }
        };
        struct B : public A {
            noinline functype2 transfer(functype f) override { return (functype2) f; }
        };
        noinline void target() {
            A *a = new B();
            auto x = a->transfer(f1);
            x(1);
        } 
        '''
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_templates(self):
        SAMPLE = '''
        template <typename T, typename U>
        void do_a_cast(T t, U* u) {
            *u = (U) t;
        }

        noinline void target() {
            functype f;
            do_a_cast<functype2, functype>(g1, &f);
            f(1);
        }
        noinline void test() {
            functype2 g;
            do_a_cast(f1, &g);
        }
        '''
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['g1'])


class OptimizationTests(OneCallTestCase):
    SAMPLE = '''
    noinline void target(void* f) { ((functype) f)(1); }
    noinline void test() {
        void* x = (void*) (functype) &g1;
        target((void*) (functype2) &f1);
    }
    '''
    VALID_FUNCTIONS = ['f1']

    def test_collapse_contexts(self):
        SAMPLE = '''
        noinline functype get_f1() { return f1; }
        noinline void target(functype f) { f(1); }
        noinline void test() {
            target(get_f1());
            functype f = f2;
        }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1'])

    def test_collapse_functions(self):
        SAMPLE = '''
        noinline functype get_f1() { return f1; }
        noinline void target(functype f) { f(1); }
        noinline void test2(functype f) {}
        noinline void test() {
            test2(get_f1());
            functype f = f2;
            target(f);
        }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f2'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f2'])

    # TODO collapsing global access

    def test_malloc(self):
        SAMPLE = '''
        void test1() {
            functype* f = (functype*) malloc(16);
            *f = f1;
        }
        void target() {
            functype* f = (functype*) malloc(16);
            *f = f2;
            (*f)(1);
        }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f2'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f2'])

    def test_free(self):
        SAMPLE = '''
        void test1(functype *f) {
            *f = f1;
            free(f);
        }
        void target(functype *f) {
            *f = f2;
            (*f)(1);
            free(f);
        }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f2'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f2'])

    def test_memcpy(self):
        SAMPLE = '''
        #include <string.h>
        void test1() {
            functype f = f2;
            functype2 g = g2;
            memcpy(&f, &g, 8);
        }
        void target() {
            functype f = f1;
            functype2 g = g1;
            memcpy(&f, &g, 8);
            f(1);
        }
        '''
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1', 'g1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1', 'g1'])
        SAMPLE = SAMPLE.replace('memcpy', 'memmove')
        self._run_test(SAMPLE_PREFIX + SAMPLE, ['f1', 'g1'])
        self._run_test(SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE), ['f1', 'g1'])

    def test_voidptr_cast_in_interface(self):
        HEADER = '''
        typedef struct { functype f; } S;
        void target(void *p);
        '''
        SAMPLE1 = '''
        void target(void *p) { ((S*) p)->f(1); }
        '''
        SAMPLE2 = '''
        void test() {
            S s; s.f = f1;
            target((void*) &s);
        }
        '''
        code = SourceCode(SAMPLE_PREFIX_HEADER + HEADER)
        code.add_file(SourceCodeFile(SAMPLE1))
        code.add_file(SourceCodeFile(SAMPLE2))
        code.env['TG_CLANG_MINIMIZE_GRAPH'] = '1'
        self._run_test(code, ['f1'])
        self._run_test(code.to_cxx(), ['f1'])

    def test_global_use_in_call(self):
        SAMPLE = '''
        functype global_f = f1;
        void target(functype f) { f(1); }
        void test() {
            target(global_f);
            functype x = f2;
            x(2);
        }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self._run_test(code, ['f1'])
        self._run_test(code.to_cxx(), ['f1'])

    def test_global_use_does_not_carry_over(self):
        SAMPLE = '''
        functype global_f = f1;
        void target(functype f) { f(1); }
        void dump(functype f) {}
        void test() {
            dump(global_f);
            functype x = f2;
            target(x);
        }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self._run_test(code, ['f2'])
        self._run_test(code.to_cxx(), ['f2'])

    def test_builtin(self):
        SAMPLE = '''
        #include <string.h>
        void target(void *p) {
            ((functype) p)(1);
        }
        void test() {
            functype2 g = g1;
            memset(g, 0, 8);
            void* f = f1;
            target(f);
        }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        code.env['TG_CLANG_MINIMIZE_GRAPH'] = '0'
        self._run_test(code, ['f1'])

    def test_pthread(self):
        SAMPLE = '''
        #include <pthread.h>
        void *target(void *f) {
            ((functype) f)(1);
            return NULL;
        }
        void test() {
            void *vp = (void *) f1;
            pthread_t thread;
            pthread_create(&thread, NULL, target, vp);
        }
        void test2(void* (*x)(void*), void* vp) {
            x(vp);
        }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self._run_test(code, ['f1'])
        self._run_test(code, ['f1'], read_from_ir=True)
        self._run_test(code.to_cxx(), ['f1'])


class CompilerOptsTestCases(unittest.TestCase):
    def test_call_combining(self):
        SAMPLE = '''
        typedef struct { functype f; } S;
        void target(S* s, int x) {
            if (x == 0) { s->f(1); } else { s->f(2); }
        }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        compile_to_graph(code, compute_function_usages=True, check_integrity=True)
        for f in code.files.values(): f.flags = ['-O3']
        compile_to_graph(code, compute_function_usages=True, check_integrity=True)
        # TODO assert final calls


class InterfaceDescriptionTestCases(OneCallTestCaseBase):
    def _getTypeNames(self, graph: Typegraph, ifd: InterfaceDesc) -> List[str]:
        return [graph.types[v] if v >= 0 else '-' for v in ifd.Types]

    def test_interface_generation(self):
        for read_from_ir in (False, True):
            graph = compile_to_graph('void* f(int a, long b, const long *c) { return 0; }', apply_typegraph_tool=True, read_from_ir=read_from_ir)
            print(graph.interfaces)
            self.assertEqual(len(graph.interfaces), 1)
            self.assertEqual(graph.interfaces[0].SymbolName, 'f')
            self.assertEqual(graph.interfaces[0].ContextName, 'f')
            self.assertEqual(graph.interfaces[0].IsFunction, True)
            self.assertEqual(graph.interfaces[0].IsVarArg, False)
            self.assertEqual(len(graph.interfaces[0].Types), 4)
            names = self._getTypeNames(graph, graph.interfaces[0])
            self.assertEqual(names, ['void *', '-', 'long', 'const long *'])

    def test_interface_generation_full(self):
        SAMPLE = '''
        extern void* imported_glob;
        void* defined_glob = 0;
        static void* internal_glob = 0;
        static void* internal_function(int a, ...) {
            long dark = 1;
            return 0;
        }
        long* imported_function(void);
        void exported_function(int a, void* b) {
            long *c = imported_function();
            implicit_declaration(1, b);
            internal_function(a);
            defined_glob = 0;
            imported_glob = 0;
        }
        void exported_vararg_1() {}
        void exported_vararg_2(int a, ...) {}
        '''
        for read_from_ir in (False, True):
            code = SourceCode()
            code.add_file(SourceCodeFile(SAMPLE, flags=['-std=c89']))
            graph = compile_to_graph(code, apply_typegraph_tool=True, read_from_ir=read_from_ir)
            graph.interfaces.sort(key=lambda ifd: ifd.SymbolName)
            symbol_names = [ifd.SymbolName for ifd in graph.interfaces]
            self.assertEqual(symbol_names, ['defined_glob', 'exported_function', 'exported_vararg_1', 'exported_vararg_2',
                                            'implicit_declaration', 'implicit_declaration', 'imported_function', 'imported_glob'])
            self.assertEqual(self._getTypeNames(graph, graph.interfaces[0]), ['void *'])
            self.assertEqual(self._getTypeNames(graph, graph.interfaces[1]), ['-', '-', 'void *'])
            self.assertEqual(self._getTypeNames(graph, graph.interfaces[6]), ['long *'])
            self.assertEqual(graph.interfaces[1].IsFunction, True)
            self.assertEqual(graph.interfaces[1].IsVarArg, False)
            self.assertEqual(graph.interfaces[2].IsFunction, True)
            # self.assertEqual(graph.interfaces[2].IsVarArg, True)
            self.assertEqual(graph.interfaces[3].IsFunction, True)
            self.assertEqual(graph.interfaces[3].IsVarArg, True)
            self.assertEqual(graph.interfaces[4].IsFunction, True)
            self.assertEqual(graph.interfaces[4].IsVarArg, False)
            self.assertEqual(graph.interfaces[0].IsFunction, False)
            self.assertEqual(graph.interfaces[7].IsFunction, False)

    def test_interface_generation_full_cxx(self):
        SAMPLE = '''
        extern void* imported_glob;
        void* defined_glob = 0;
        static void* internal_glob = 0;
        static void* internal_function(int a, ...) {
            long dark = 1;
            return 0;
        }
        long* imported_function(void);
        void exported_function(int a, void* b) {
            long *c = imported_function();
            internal_function(a);
            defined_glob = 0;
            imported_glob = 0;
        }
        void exported_vararg_1() {}
        void exported_vararg_2(int a, ...) {}
        '''
        for read_from_ir in (False, True):
            code = SourceCode()
            code.add_file(SourceCodeFile(SAMPLE, compiler='CXX'))
            graph = compile_to_graph(code, apply_typegraph_tool=True, read_from_ir=read_from_ir)
            graph.interfaces.sort(key=lambda ifd: ifd.SymbolName)
            symbol_names = [ifd.SymbolName for ifd in graph.interfaces]
            self.assertEqual(symbol_names, ['_Z17exported_functioniPv', '_Z17exported_vararg_1v', '_Z17exported_vararg_2iz', '_Z17imported_functionv',
                                            'defined_glob', 'imported_glob'])
            self.assertEqual(self._getTypeNames(graph, graph.interfaces[4]), ['void *'])
            self.assertEqual(self._getTypeNames(graph, graph.interfaces[0]), ['-', '-', 'void *'])
            self.assertEqual(self._getTypeNames(graph, graph.interfaces[3]), ['long *'])
            self.assertEqual(graph.interfaces[0].IsFunction, True)
            self.assertEqual(graph.interfaces[0].IsVarArg, False)
            self.assertEqual(graph.interfaces[1].IsFunction, True)
            # self.assertEqual(graph.interfaces[2].IsVarArg, True)
            self.assertEqual(graph.interfaces[2].IsFunction, True)
            self.assertEqual(graph.interfaces[2].IsVarArg, True)
            self.assertEqual(graph.interfaces[4].IsFunction, False)
            self.assertEqual(graph.interfaces[5].IsFunction, False)

    def test_header_blowup(self):
        for read_from_ir in (False, True):
            graph = compile_to_graph('#include <stdio.h>\n#include <stdlib.h>\nvoid* f(int a, long b, const long *c) { return 0; }',
                                     apply_typegraph_tool=True, read_from_ir=read_from_ir)
            self.assertLess(graph.graph.num_vertices(), 10)

    def test_changing_definitions(self):
        SAMPLE1 = '''
        noinline void transfer(unsigned i, const char *p, functype g);
        noinline void test1() { transfer(1, "abc", f1); }
        '''
        SAMPLE2 = '''
        noinline void target(functype2 g) { g(1); }
        noinline void transfer(unsigned i, void *p, functype2 g) { target(g); }
        '''
        code = SourceCode(SAMPLE_PREFIX_HEADER)
        code.add_file(SourceCodeFile(SAMPLE1, flags=['-std=c89', '-O1']))
        code.add_file(SourceCodeFile(SAMPLE_PREFIX_DEFS + SAMPLE2, flags=['-std=c89', '-O1']))
        self._run_test(code, ['f1'])

    def test_changing_definitions_implicit(self):
        SAMPLE1 = '''
        noinline void test1() { transfer(1, "abc", f1); }
        '''
        SAMPLE2 = '''
        noinline void target(functype2 g) { g(1); }
        noinline void transfer(unsigned i, void *p, functype2 g) { target(g); }
        '''
        code = SourceCode(SAMPLE_PREFIX_HEADER)
        code.add_file(SourceCodeFile(SAMPLE1, flags=['-std=c89', '-O1']))
        code.add_file(SourceCodeFile(SAMPLE_PREFIX_DEFS + SAMPLE2, flags=['-std=c89', '-O1']))
        self._run_test(code, ['f1'])

    def test_changing_definitions_vararg(self):
        SAMPLE1 = '''
        noinline void test1() { transfer(1, "abc", f1); }
        '''
        SAMPLE2 = '''
        noinline void target(void(*g)()) { g(1); }
        noinline void transfer(unsigned i, void *p, void(*g)()) { target(g); }
        '''
        code = SourceCode(SAMPLE_PREFIX_HEADER)
        code.add_file(SourceCodeFile(SAMPLE1, flags=['-std=c89', '-O1']))
        code.add_file(SourceCodeFile(SAMPLE_PREFIX_DEFS + SAMPLE2, flags=['-std=c89', '-O1']))
        self._run_test(code, ['f1'])

    def test_interface_flags(self):
        SAMPLE = '''
        void test1(int);
        static void test2() {}
        void test3(int x) { test1(x); test2(); }
        extern int a1;
        int a2;
        static int a3;
        void use_them() { a1 = a2 = a3; }
        '''
        graphs = [
            compile_to_graph(SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)),
            compile_to_graph(SourceCode.from_string(SAMPLE_PREFIX + SAMPLE), apply_typegraph_tool=True),
            compile_to_graph(SourceCode.from_string(SAMPLE_PREFIX + SAMPLE), apply_typegraph_tool=True, read_from_ir=True),
        ]
        for graph in graphs:
            # check function flags
            self.assertEqual(len(graph.get_interfaces_for_symbol("test1")), 1)
            assert graph.get_interfaces_for_symbol("test1")[0].IsFunction is True
            assert graph.get_interfaces_for_symbol("test1")[0].IsVarArg is False
            assert graph.get_interfaces_for_symbol("test1")[0].IsDefined is False
            self.assertEqual(len(graph.get_interfaces_for_symbol("test2")), 0)
            self.assertEqual(len(graph.get_interfaces_for_symbol("test3")), 1)
            assert graph.get_interfaces_for_symbol("test3")[0].IsFunction is True
            assert graph.get_interfaces_for_symbol("test3")[0].IsVarArg is False
            assert graph.get_interfaces_for_symbol("test3")[0].IsDefined is True
            # check variables flags
            self.assertEqual(len(graph.get_interfaces_for_symbol("a1")), 1)
            assert graph.get_interfaces_for_symbol("a1")[0].IsFunction is False
            assert graph.get_interfaces_for_symbol("a1")[0].IsDefined is False
            self.assertEqual(len(graph.get_interfaces_for_symbol("a2")), 1)
            assert graph.get_interfaces_for_symbol("a2")[0].IsFunction is False
            assert graph.get_interfaces_for_symbol("a2")[0].IsDefined is True
            self.assertEqual(len(graph.get_interfaces_for_symbol("a3")), 0)

    def test_context_defining_units(self):
        SAMPLE = '''
        void test1();
        void test2() {
            long l = atol("123");
        }
        void test3();
        void test3(){ long l = 1; }
        static void test4() { long l = 1; }
        void test5(){ long l = 1; }
        void test6(long l);
        static void test8(long *l, int *i) { l = (long *) i; }
        void use_things() { long l = 1; test6(l); test8(&l, 0); }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        graph = compile_to_graph(code)
        # print(list(sorted(graph.contextDefiningUnits.keys())))
        # defined:
        self.assertIn('f1', graph.contextDefiningUnits)
        self.assertIn('f2', graph.contextDefiningUnits)
        self.assertIn('g1', graph.contextDefiningUnits)
        self.assertIn('g2', graph.contextDefiningUnits)
        self.assertIn('test2', graph.contextDefiningUnits)
        self.assertIn('test3', graph.contextDefiningUnits)
        self.assertIn('test5', graph.contextDefiningUnits)
        self.assertIn('use_things', graph.contextDefiningUnits)
        # not defined:
        self.assertNotIn('test1', graph.contextDefiningUnits)  # only declared
        self.assertNotIn('test4', graph.contextDefiningUnits)  # internal
        self.assertNotIn('test6', graph.contextDefiningUnits)  # only declared + used
        self.assertNotIn('atol', graph.contextDefiningUnits)  # only declared + used, stdlib
        self.assertNotIn('test8', graph.contextDefiningUnits)  # static
        # all must be from one unit:
        self.assertEqual(len(set(graph.contextDefiningUnits.values())), 1)

    def test_weak_alias(self):
        SAMPLE = '''
        #define weak_alias(old, new) \
        extern __typeof(old) new __attribute__((__weak__, __alias__(#old)))
        
        long __library_function(void *p) { return (long) p; }
        weak_alias(__library_function, library_function);
        
        void target() {
            long l = library_function(g1);
            ((functype) l)(1);
        }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self._run_test(code, ['g1'])


class LayeringTestCases(OneCallTestCaseBase):
    SAMPLE1 = '''
    noinline void target(functype f) { f(1); }
    noinline void test1() { target(f1); }
    noinline void test2() { target(f2); }
    noinline void test3() { target(0); }
    '''

    def test_simple_layering(self):
        code = SourceCode.from_string(SAMPLE_PREFIX + self.SAMPLE1)
        self._run_test(code, ['f1', 'f2'])

    def test_simple_callgraph(self):
        graphs = [
            compile_to_graph(SourceCode.from_string(SAMPLE_PREFIX + self.SAMPLE1)),
            compile_to_graph(SourceCode.from_string(SAMPLE_PREFIX + self.SAMPLE1), apply_typegraph_tool=True),
            compile_to_graph(SourceCode.from_string(SAMPLE_PREFIX + self.SAMPLE1), apply_typegraph_tool=True, read_from_ir=True),
            compile_to_graph(SourceCode.from_string(SAMPLE_PREFIX + self.SAMPLE1), read_from_ir=True, compute_function_usages=True)
        ]
        for graph in graphs:
            # graph.callgraph_build_dot()
            self.assertEqual(graph.callgraph.num_vertices(), 3)
            self.assertEqual(graph.callgraph.num_edges(), 2)
            v1 = graph.context_to_callgraph_vertex['test1']
            v2 = graph.context_to_callgraph_vertex['test2']
            v3 = graph.context_to_callgraph_vertex['target']
            for src, tgt in graph.callgraph.get_in_edges(v3):
                assert v1 == src or v2 == src

    def test_callgraph_scc_building(self):
        # check if SCCs are correctly computed
        SAMPLE = '''
        noinline void test1(void *p);
        noinline void test2(void *p);
        noinline void test3(void *p);
        noinline void test4(void *p);
        noinline void test5(void *p);
        noinline void test6(void *p);
        
        void test1(void *p) { test2(p); }
        void test2(void *p) { test3(p); }
        void test3(void *p) { test1(p); }
        void test4(void *p) { test1(p); test5(p); }
        void test5(void *p) { test2(p); test4(p); }
        void test6(void *p) { test4(p); test5(p); test6(p); test1(p); }
        '''
        graph = compile_to_graph(SAMPLE_PREFIX + SAMPLE)
        # graph.callgraph_build_dot()
        self.assertEqual(graph.context_to_callgraph_vertex['test1'], graph.context_to_callgraph_vertex['test2'])
        self.assertEqual(graph.context_to_callgraph_vertex['test1'], graph.context_to_callgraph_vertex['test3'])
        self.assertEqual(graph.context_to_callgraph_vertex['test4'], graph.context_to_callgraph_vertex['test5'])
        self.assertEqual(graph.callgraph.num_vertices(), 3)
        self.assertEqual(graph.callgraph.num_edges(), 3)
        assert len(graph.callgraph.get_in_edges(graph.context_to_callgraph_vertex['test6'])) == 0
        assert len(graph.callgraph.get_in_edges(graph.context_to_callgraph_vertex['test4'])) == 1
        assert graph.callgraph.get_in_edges(graph.context_to_callgraph_vertex['test4'])[0][0] == graph.context_to_callgraph_vertex['test6']
        assert len(graph.callgraph.get_in_edges(graph.context_to_callgraph_vertex['test1'])) == 2
        for src, _ in graph.callgraph.get_in_edges(graph.context_to_callgraph_vertex['test1']):
            assert src in (graph.context_to_callgraph_vertex['test4'], graph.context_to_callgraph_vertex['test6'])



class SamplesTestCases(unittest.TestCase):
    def test_gcc_htab(self):
        graph = compile_to_graph(SourceCode.from_file('samples/gcc_htab.c'), compute_function_usages=True, read_from_ir=True)
        # graph.build_dot(include_additional_names=True)
        fset1 = graph.get_call_first_functions(lambda x: '.0 in internal ggc_htab_delete@' in x)
        fset2 = graph.get_call_first_functions(lambda x: '.1 in internal ggc_htab_delete@' in x)
        self.assertEqual(fset1, ['ggc_marked_p', 'type_hash_marked_p'])
        self.assertEqual(fset2, ['type_hash_mark'])

    def test_gcc_htab2(self):
        code = SourceCode.from_file('samples/gcc_htab_tree.c')
        code.add_file(SourceCodeFile.from_file('samples/gcc_htab_ggc_common.c'))
        code.add_file(SourceCodeFile.from_file('samples/gcc_htab_hashtab.c'))
        graph = compile_to_graph(code, compute_function_usages=True, read_from_ir=True)
        '''
        def interesting(v):
            names = [graph.types[v]+' '+graph.contexts[v]]
            names += list(graph.additionalNames[v])
            #return any('ggc_htab_delete' in n for n in names)
            if any('ggc_htab_delete' in n for n in names): return True
            if any(n.endswith('htab_traverse') for n in names): return True
            if 'ggc_htab_delete' in graph.functionUsages[v]: return True
            return False
        g2 = graph.get_interesting_subgraph(interesting)
        print(g2.graph.num_vertices(), g2.graph.num_edges(), graph.graph.num_vertices(), graph.graph.num_edges())
        g2.dot_mark_call_context_nodes()
        graph.dot_mark_call_context_nodes()
        graph.build_dot(output='/tmp/test.svg', include_additional_names=True)
        g2.build_dot(include_additional_names=True)
        '''
        fset1 = graph.get_call_first_functions(lambda x: '.0 in internal ggc_htab_delete@' in x)
        fset2 = graph.get_call_first_functions(lambda x: '.1 in internal ggc_htab_delete@' in x)
        print(fset1, fset2)
        self.assertEqual(fset1, ['ggc_marked_p', 'type_hash_marked_p'])
        self.assertEqual(fset2, ['type_hash_mark'])
        # self.assertEqual(fset1, ['ggc_marked_p', 'type_hash_eq', 'type_hash_hash', 'type_hash_mark', 'type_hash_marked_p'])
        # self.assertEqual(fset2, ['ggc_marked_p', 'type_hash_eq', 'type_hash_hash', 'type_hash_mark', 'type_hash_marked_p'])

    def test_gcc_function_units(self):
        # taken from gcc
        HEADER = '''
        struct rtx_def;
        typedef struct rtx_def *rtx;
        
        extern int actual_hazard_this_instance (int unit, int instance, rtx insn, int clock, int cost);
        
        extern const struct function_unit_desc
        {
          const char *const name;
          const int bitmask;
          const int multiplicity;
          const int simultaneity;
          const int default_cost;
          const int max_issue_delay;
          int (*const ready_cost_function) (rtx a);
          int (*const conflict_cost_function) (rtx a, rtx b);
          const int max_blockage;
          unsigned int (*const blockage_range_function) (rtx a);
          int (*const blockage_function) (rtx a, rtx b);
        } function_units[];
        '''
        FILE1 = '''
        extern unsigned int athlon_ieu_unit_blockage_range (rtx insn);
        unsigned int athlon_ieu_unit_blockage_range (rtx insn) { return 0; }
        
        static int athlon_ieu_unit_blockage (rtx executing_insn, rtx candidate_insn);
        static int athlon_ieu_unit_blockage (rtx executing_insn, rtx candidate_insn) { return 0; }
        
        // athlon_ieu_unit_blockage
        const struct function_unit_desc function_units[] = {
            //{"athlon_ieu", 262144, 3, 0, 0, 15, athlon_ieu_unit_ready_cost, athlon_ieu_unit_conflict_cost, 15, athlon_ieu_unit_blockage_range, athlon_ieu_unit_blockage}
            {"athlon_ieu", 262144, 3, 0, 0, 15, 0, 0, 15, athlon_ieu_unit_blockage_range, athlon_ieu_unit_blockage}
        };
        '''
        FILE2 = '''
        static rtx unit_last_insn[1337];
        
        int actual_hazard_this_instance (int unit, int instance, rtx insn, int clock, int cost) {
            function_units[unit].blockage_function(unit_last_insn[instance], insn);
            return 0;
        }
        '''
        #compile_to_graph(HEADER + FILE1).build_dot()
        #os.environ['TG_CLANG_MINIMIZE_GRAPH'] = '0'
        #compile_to_graph(HEADER + FILE2).get_interesting_subgraph().build_dot(include_number=True)
        code = SourceCode(HEADER)
        code.add_file(SourceCodeFile(FILE1))
        code.add_file(SourceCodeFile(FILE2))
        graph = compile_to_graph(code, compute_function_usages=True, read_from_ir=True)
        #graph.get_interesting_subgraph().build_dot()
        funcs = graph.get_call_first_functions(lambda call: ' in actual_hazard_this_instance' in call)
        print(funcs)
        self.assertIn('athlon_ieu_unit_blockage', funcs)

    def test_gobmk_1(self):
        HEADER = '''
        #define BOARDMAX 10
        #define NULL (0)
        #define WHITE 1
        
        struct pattern;
        struct pattern_db;
        struct dfa_rt;
        typedef void (*matchpat_callback_fn_ptr)(int anchor, int color, struct pattern *, int rotation, void *data);
        
        void matchpat(matchpat_callback_fn_ptr callback, int color, struct pattern_db *pdb, void *callback_data, char goal[BOARDMAX]);
        
        typedef int (*pattern_helper_fn_ptr)(struct pattern *, int rotation, int move, int color);
        typedef int (*autohelper_fn_ptr)(int rotation, int move, int color, int action);
        typedef void (*loop_fn_ptr_t)(matchpat_callback_fn_ptr callback, int color, int anchor, struct pattern_db *pdb, void *callback_data, char goal[BOARDMAX], int anchor_in_goal);
        
        struct pattern {
          const char *name;
          int autohelper_flag;
          pattern_helper_fn_ptr helper;
          autohelper_fn_ptr autohelper;
          float constraint_cost;
        };
        struct pattern_db {
          int fixed_for_size;
          const int fixed_anchor;
          struct pattern *patterns;
        };
        
        extern struct pattern_db pat_db;
        '''
        FILE1 = '''
        static int autohelperpat1141(int trans, int move, int color, int action) {
          return 0;
        }
        static int reinforce_helper(struct pattern *pattern, int trans, int move, int color) {
          return 0;
        }
        static struct pattern pat[] = {
          {"Reinforce1", 0, reinforce_helper, NULL, 0.000000},
          {"DD13", 3, NULL, autohelperpat1141, 1.618000},
          {NULL, 0,NULL, NULL, 0.0}
        };
        struct pattern_db pat_db = {-1, 0, pat};
        '''
        FILE2 = '''
        /*static*/ void shapes_callback(int anchor, int color, struct pattern *pattern, int ll, void *data) {
          if (pattern->helper) {
            int accepted = pattern->helper(pattern, ll, 18, color);
          }
          if (pattern->autohelper_flag)
            pattern->autohelper(ll, 18, color, 1);
        }
        
        void shapes(int color) {
          matchpat(shapes_callback, color, &pat_db, NULL, NULL);
        }
        '''
        FILE3 = '''        
        static void matchpat_loop(matchpat_callback_fn_ptr callback, int color, int anchor, struct pattern_db *pdb, void *callback_data, char goal[BOARDMAX], int anchor_in_goal) {
          // do_matchpat(0, callback, color, pdb->patterns, callback_data, goal);
          callback(0, color, pdb->patterns, 0, callback_data);
        }
        
        void matchpat(matchpat_callback_fn_ptr callback, int color, struct pattern_db *pdb, void *callback_data, char goal[BOARDMAX]) {
          // inlined: matchpat_goal_anchor(callback, color, pdb, callback_data, goal, pdb->fixed_anchor);
          loop_fn_ptr_t loop = matchpat_loop;
          loop(callback, WHITE, WHITE, pdb, callback_data, goal, pdb->fixed_anchor);
        }
        '''
        code = SourceCode(HEADER)
        code.add_file(SourceCodeFile(FILE1))
        code.add_file(SourceCodeFile(FILE2))
        code.add_file(SourceCodeFile(FILE3))
        graph = compile_to_graph(code, compute_function_usages=True, read_from_ir=True)
        # graph.get_interesting_subgraph().build_dot()
        funcs0 = graph.get_call_first_functions(lambda call: '.0 in shapes_callback' in call)
        funcs1 = graph.get_call_first_functions(lambda call: '.1 in shapes_callback' in call)
        print(funcs0, funcs1)
        self.assertIn('reinforce_helper', funcs0)
        self.assertIn('autohelperpat1141', funcs1)

    def test_lua1(self):
        SAMPLE = '''
        typedef struct lua_State lua_State;
        typedef int (*lua_CFunction) (lua_State *L);
        extern void (lua_pushcclosure) (lua_State *L, lua_CFunction fn, int n);
        
        typedef struct luaL_Reg {
          const char *name;
          lua_CFunction func;
        } luaL_Reg;
        
        extern void luaL_openlib (lua_State *L, const char *libname, const luaL_Reg *l, int nup) {
            lua_pushcclosure(L, l->func, nup);
        }
        '''
        graph = compile_to_graph(SAMPLE)
        v = graph.type_context_to_vertex[('const struct luaL_Reg', 'luaL_openlib')]
        v2 = graph.get_struct_member(v, 8)
        v3 = graph.type_context_to_vertex[('int (*)(struct lua_State *)', 'lua_pushcclosure')]
        assert graph.is_reachable_default(v2, v3)


class InstrumentationCollectCallTargetsTestCases(unittest.TestCase):
    def __check_getone_sample(self, code: SourceCode, **kwargs):
        try:
            binary, outputs = compile_link_run(code, [['1'], ['2'], ['3'], ['4']], instrument_collect_calltargets=True, output_graph=True, **kwargs)
            outputs = b''.join(outputs).decode().replace('((remove))', '')
            self.assertEqual(outputs.strip(), 'f1(1)\nf2(1)\ng1(1)\ng2(1)')  # instrumentation does not change output
            ct = CallTargets.from_pattern(f'{binary}.calltargets*.json')
            self.assertEqual(len(ct.calls), 1)
            self.assertGreaterEqual(ct.num_calls, 1)
            self.assertGreaterEqual(ct.num_functions, 4)
            called_functions = list(sorted(list(ct.calls.values())[0]))
            if code.is_cxx():
                self.assertEqual(called_functions, ['_Z2f1i', '_Z2f2i', '_Z2g1l', '_Z2g2l'])
            else:
                self.assertEqual(called_functions, ['f1', 'f2', 'g1', 'g2'])
            g1, g2 = code.load_typegraph_before(), code.load_typegraph_after()
            ct.assert_matches_graph(g1)
            ct.assert_matches_graph(g2)
            for c, p in ct.compute_precision(g2).items():
                print(p)
                self.assertEqual(p.called_but_not_allowed, 0, f'In call "{c}", functions are missing in analysis: {p}')
        finally:
            code.cleanup()

    def test_getone_instrumented(self):
        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE_GETONE)
        self.__check_getone_sample(code)

    def test_getone_with_initializer(self):
        CODE = '''
        __attribute__((constructor))
        void at_init() {}
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE_GETONE + CODE)
        self.__check_getone_sample(code)
        CODE = '''
        __attribute__((constructor))
        void at_init() {
            printf("((remove))");
        }
        __attribute__((destructor))
        void at_fini() {
            printf("((remove))");
        }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE_GETONE + CODE)
        self.__check_getone_sample(code)

    def test_getone_instrumented_cxx(self):
        code = SourceCode.from_cxx(SAMPLE_PREFIX + SAMPLE_GETONE)
        self.__check_getone_sample(code)

    def test_getone_precision(self):
        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE_GETONE)
        try:
            binary, outputs = compile_link_run(code, [['1'], ['2'], ['3']], instrument_collect_calltargets=True, output_graph=True)
            ct = CallTargets.from_pattern(f'{binary}.calltargets*.json')
            g2 = code.load_typegraph_after()
            ct.assert_matches_graph(g2)
            p = ct.compute_precision(g2)
            self.assertEqual(len(p), 1)
            p = list(p.values())[0]
            self.assertEqual(p, CallPrecision(3, 1, 0))
        finally:
            code.cleanup()

    def test_fork(self):
        SAMPLE = '''
        #include <unistd.h>

        void noinline target(functype f) { f(1); }
        
        int main(int argc, const char* argv[]) {
            target(argv[1][0] == '1' ? f1 : f2);
            if (fork() == 0) {
                // child
                target(argv[1][0] == '1' ? (functype) g1 : (functype) g2);
                exit(0);
            } else {
                // parent
                sleep(1);
            }
            return 0;
        }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        try:
            binary, outputs = compile_link_run(code, [['1'], ['2']], instrument_collect_calltargets=True, output_graph=True)
            print(outputs)
            ct = CallTargets.from_pattern(f'{binary}.calltargets*.json')
            g2 = code.load_typegraph_after()
            ct.assert_matches_graph(g2)
            print(ct.num_files, 'files')
            print(ct.calls)
            for f in ['f1', 'f2', 'g1', 'g2']:
                self.assertIn(f, ct.calls['call#80.0 in target'])
        finally:
            code.cleanup()

        if ARCH['name'] != 'native':
            return

        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE.replace('exit(0)', '__asm__("movq $0, %rdi ; movq $60, %rax ; syscall")'))
        try:
            binary, outputs = compile_link_run(code, [['1'], ['2']], instrument_collect_calltargets=True, output_graph=True)
            print(outputs)
            ct = CallTargets.from_pattern(f'{binary}.calltargets*.json')
            g2 = code.load_typegraph_after()
            ct.assert_matches_graph(g2)
            print(ct.num_files, 'files')
            print(ct.calls)
            for f in ['f1', 'f2', 'g1', 'g2']:
                self.assertIn(f, ct.calls['call#80.0 in target'])
        finally:
            code.cleanup()

    def test_dynamic_linking(self):
        # compile library
        code = SourceCode.from_string('#define NO_CHECK_FUNCTIONS\n' + LIBRARY_CODE)
        code.make_shared()
        code.env['TG_DYNLIB_SUPPORT'] = '1'
        library = compile_link(code, instrument_collect_calltargets=True)
        atexit.register(lambda: os.remove(library) if os.path.exists(library) else None)
        # compile program
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        void noinline run(char c) {
            library_get1(c)(1);
            printf("Result = %d\\n", library_get2(c)(2));
            library_call1(f1);
            library_call1(library_get1(c));
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(library).set_env('TG_DYNLIB_SUPPORT', '1')
        binary, outputs = compile_link_run(code, [['1']], instrument_collect_calltargets=True, output_graph=True)
        atexit.register(lambda: os.remove(binary) if os.path.exists(binary) else None)
        ct = CallTargets.from_pattern(f'{binary}.calltargets*.json')
        assert ct.binary.endswith(code.binary_fname)
        self.assertEqual({'lib11'}, ct.calls['call#80.0 in run'])
        self.assertEqual({'lib21'}, ct.calls['call#80.1 in run'])
        self.assertEqual({'f1', 'lib11'}, ct.calls['call#80.0 in library_call1'])
        self.assertEqual(set(), ct.calls['call#80.0 in library_call2'])

    def test_dynamic_linking_output(self):
        code = SourceCode.from_string('#define NO_CHECK_FUNCTIONS\n' + LIBRARY_CODE)
        code.make_shared()
        library = compile_link(code, enforce=True)
        atexit.register(lambda: os.remove(library) if os.path.exists(library) else None)
        # compile program
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        void noinline run(char c) {
            library_get1(c)(1);
            printf("Result = %d\\n", library_get2(c)(2));
            library_call1(f1);
            library_call1(library_get1(c));
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(library)
        try:
            os.environ['TG_CFI_OUTPUT_RT'] = 'auto'
            binary, outputs = compile_link_run(code, [['1']], enforce=True, output_graph=True)
        finally:
            del os.environ['TG_CFI_OUTPUT_RT']
        # check file
        with open(f'{binary}.dynamic-tgcfi0.json', 'r') as f:
            data = json.loads(f.read())
        self.assertEqual(2, len(data['modules']))
        self.assertIn('f1', data['tg_additional_targets']['call#80.0 in library_call1'])
        self.assertIn('lib11', data['tg_additional_targets']['call#80.0 in library_call1'])
        self.assertIn('lib12', data['tg_additional_targets']['call#80.0 in library_call1'])
        self.assertIn('lib11', data['tg_additional_targets']['call#80.0 in run'])
        self.assertIn('lib12', data['tg_additional_targets']['call#80.0 in run'])
        self.assertIn('lib21', data['tg_additional_targets']['call#80.1 in run'])
        self.assertIn('lib22', data['tg_additional_targets']['call#80.1 in run'])


class RelatedWorkInstrumentationsTestCases(unittest.TestCase):
    @staticmethod
    def __code_with_icfi(code: SourceCode) -> SourceCode:
        code.env['TG_ICFI_OUTPUT'] = code.binary_fname + '.json'
        code.linker_flags += ['-fsanitize=cfi-icall', '-fvisibility=default']
        for f in code.files.values():
            f.flags += ['-fsanitize=cfi-icall', '-fvisibility=default']
        return code

    @staticmethod
    def __code_with_ifcc(code: SourceCode) -> SourceCode:
        code.env['TG_IFCC_OUTPUT'] = code.binary_fname + '-ifcc.json'
        return code

    @staticmethod
    def __code_with_tgcfi(code: SourceCode) -> SourceCode:
        code.env['TG_CFI_OUTPUT'] = code.binary_fname + '-tgcfi.json'
        return code

    def test_getone_icfi(self):
        if not ARCH['support_icfi']:
            return
        code = self.__code_with_icfi(SourceCode.from_string(SAMPLE_PREFIX + SAMPLE_GETONE))
        binary = code.compile_and_link()
        with open(binary + '.json', 'r') as f:
            data = json.loads(f.read())
        self.assertEqual(data['icfi_generalize_pointers'], False)
        self.assertEqual(len(data['icfi_targets']), 1)
        self.assertEqual(len(data['icfi_targets_generalized']), 1)
        self.assertEqual(set(data['icfi_targets']['call#80.0 in main']), {'f1', 'f2'})
        self.assertEqual(set(data['icfi_targets_generalized']['call#80.0 in main']), {'f1', 'f2'})

    def test_getone_ifcc(self):
        code = self.__code_with_ifcc(SourceCode.from_string(SAMPLE_PREFIX + SAMPLE_GETONE))
        binary = code.compile_and_link()
        with open(binary + '-ifcc.json', 'r') as f:
            data = json.loads(f.read())
        self.assertEqual(len(data['ifcc_targets']), 1)
        self.assertEqual(len(data['ifcc_targets_vararg']), 1)
        self.assertEqual(set(data['ifcc_targets']['call#80.0 in main']), {'f1', 'f2', 'g1', 'g2'})
        self.assertEqual(set(data['ifcc_targets_vararg']['call#80.0 in main']), {'f1', 'f2', 'g1', 'g2'})

    def test_getone_tgcfi(self):
        code = self.__code_with_tgcfi(SourceCode.from_string(SAMPLE_PREFIX + SAMPLE_GETONE))
        binary = code.compile_and_link()
        with open(binary + '-tgcfi.json', 'r') as f:
            data = json.loads(f.read())
        self.assertEqual(len(data['tg_targets']), 1)
        self.assertEqual(len(data['tg_targets_argnum']), 1)
        self.assertEqual(set(data['tg_targets']['call#80.0 in main']), {'f1', 'f2', 'g1', 'g2'})
        self.assertEqual(set(data['tg_targets_argnum']['call#80.0 in main']), {'f1', 'f2', 'g1', 'g2'})


class EnforcementTestCases(unittest.TestCase):
    def __compare_enforced_unenforced(self, program: Union[SourceCode, str], arguments: List[List[str]], compare_stderr=True):
        if isinstance(program, str):
            program = SourceCode.from_string(program)
        # compile stuff
        program.binary_fname = program.binary_fname.replace('.enf.bin', '.bin').replace('.bin', '.ref.bin')
        binary_ref = compile_link(program, enforce=False)
        atexit.register(lambda: os.remove(binary_ref) if os.path.exists(binary_ref) else None)
        program.binary_fname = program.binary_fname.replace('.ref.bin', '.enf.bin')
        binary_enf = compile_link(program, enforce=True)
        atexit.register(lambda: os.remove(binary_enf) if os.path.exists(binary_enf) else None)
        # run and compare
        for args in arguments:
            result_ref = subprocess.run(RUN_PREFIX + [binary_ref] + args, timeout=3, stdout=subprocess.PIPE, stderr=subprocess.PIPE, input=b'')
            result_enf = subprocess.run(RUN_PREFIX + [binary_enf] + args, timeout=3, stdout=subprocess.PIPE, stderr=subprocess.PIPE, input=b'')
            print('ref', result_ref)
            print('enf', result_enf)
            if result_ref.returncode != result_enf.returncode:
                print(f'Different return codes (expected {result_ref.returncode}, was {result_enf.returncode}) for inputs {args}')
                sys.stdout.flush()
                sys.stdout.write('=== stdout ===\n')
                sys.stdout.write(result_enf.stdout.decode(errors='ignore'))
                sys.stdout.write('\n=== stderr ===\n')
                sys.stdout.write(result_enf.stderr.decode(errors='ignore'))
                self.assertEqual(result_ref.returncode, result_enf.returncode)
            if result_ref.stdout != result_enf.stdout:
                print(f'Different output for inputs {args}')
                sys.stdout.flush()
                sys.stdout.write('=== stdout (expected) ===\n')
                sys.stdout.write(result_ref.stdout.decode(errors='ignore'))
                sys.stdout.write('=== stdout (actual) ===\n')
                sys.stdout.write(result_enf.stdout.decode(errors='ignore'))
                sys.stdout.write('=== stderr (actual) ===\n')
                sys.stdout.write(result_enf.stderr.decode(errors='ignore'))
                self.assertEqual(result_ref.stdout, result_enf.stdout)
            if compare_stderr and result_ref.stderr != result_enf.stderr:
                print(f'Different stderr output for inputs {args}')
                sys.stdout.flush()
                sys.stdout.write('=== stderr (expected) ===\n')
                sys.stdout.write(result_ref.stderr.decode(errors='ignore'))
                sys.stdout.write('=== stderr (actual) ===\n')
                sys.stdout.write(result_enf.stderr.decode(errors='ignore'))
                self.assertEqual(result_ref.stderr, result_enf.stderr)
        return binary_ref, binary_enf

    def test_getone(self):
        self.__compare_enforced_unenforced(SAMPLE_PREFIX + SAMPLE_GETONE, [['1', '2', '3', '4', '5']])
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE_GETONE).to_cxx()
        self.__compare_enforced_unenforced(source, [['1', '2', '3', '4', '5']])

    def test_one_candidate(self):
        SAMPLE = '''
        functype noinline get_one(char c) {
            if (c == '1') return f1;
            return NULL;
        }
        ''' + SAMPLE_GET_MAIN
        args = [['1'], ['2']]
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_one_candidate_different_types(self):
        SAMPLE = '''
        functype noinline get_one(char c) {
            if (c == '1') return (functype) g1;
            return NULL;
        }
        ''' + SAMPLE_GET_MAIN
        args = [['1'], ['2']]
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_one_candidate_different_return_types(self):
        SAMPLE = '''
        typedef short (*functype3)(long);
        functype3 noinline get_one(char c) {
            if (c == '1') return (functype3) g1;
            return NULL;
        }
        int main(int argc, const char* argv[]) {
            functype3 f = get_one(argv[1][0]);
            if (f) printf("%hd\\n", f(1));
            return 0;
        }
        '''
        args = [['1'], ['2']]
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_return_types(self):
        SAMPLE = '''
        long noinline fx1(int x) { return 15; }
        char noinline fx2(int x) { return 27 + x; }
        
        functype2 noinline get_one(char c) {
            if (c == '1') return (functype2) fx1;
            if (c == '2') return (functype2) fx2;
            if (c == '3') return g1;
            return NULL;
        }
        int main(int argc, const char* argv[]) {
            int (*f)(long);
            f = get_one(argv[1][0]);
            printf("=> %d\\n", f(1));
            return 0;
        }
        '''
        args = [['1'], ['2'], ['3']]
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_invalid_pointer(self):
        if ARCH['name'] != 'native':
            return
        SAMPLE = '''
        void nopfunc(int x) { __asm__("popq %rax ; nop ; nop ; nop ; nop ; nop ; pushq %rax"); printf("HI!\\n"); }
        
        functype noinline get_one(char c) {
            if (c < '0') return &f1;
            functype result = &nopfunc;
            result = (functype) ((uintptr_t) result + (c - '0'));
            return result;
        }
        ''' + SAMPLE_GET_MAIN
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        binary_ref, binary_enf = self.__compare_enforced_unenforced(source, [['0']])
        # check that inputs > 1 crash
        result_ref = subprocess.run([binary_ref] + ['2'], timeout=3, stdout=subprocess.PIPE, stderr=subprocess.PIPE, input=b'')
        print(result_ref)
        self.assertEqual(result_ref.returncode, 0)
        result_enf = subprocess.run([binary_enf] + ['2'], timeout=3, stdout=subprocess.PIPE, stderr=subprocess.PIPE, input=b'')
        self.assertEqual(result_enf.returncode, ARCH['signal'])

        binary_ref, binary_enf = self.__compare_enforced_unenforced(source.to_cxx(), [['0']])
        # check that inputs > 1 crash
        result_ref = subprocess.run([binary_ref] + ['2'], timeout=3, stdout=subprocess.PIPE, stderr=subprocess.PIPE, input=b'')
        self.assertEqual(result_ref.returncode, 0)
        result_enf = subprocess.run([binary_enf] + ['2'], timeout=3, stdout=subprocess.PIPE, stderr=subprocess.PIPE, input=b'')
        self.assertEqual(result_enf.returncode, ARCH['signal'])

    def test_global_struct_array(self):
        SAMPLE = '''
        struct S { void (*const f)(int x); int i; };
        static struct S funcs[3] = {{f1, 0}, {&f2, 1}, {(functype) g1, 2}};
        noinline void target(int i) { funcs[i].f(1); }
        int main(int argc, const char* argv[]) {
            target(argv[1][0] - '0');
            return 0;
        }
        '''
        args = [['0'], ['1'], ['2']]
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_global_array(self):
        SAMPLE = '''
        static functype funcs[3] = {f1, &f2, (functype) g1};
        noinline void target(int i) { funcs[i](1); }
        int main(int argc, const char* argv[]) {
            target(argv[1][0] - '0');
            return 0;
        }
        '''
        args = [['0'], ['1'], ['2']]
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_global_var(self):
        SAMPLE = '''
        static functype func = f1;
        int main(int argc, const char* argv[]) {
            if (argv[1][0] == '2') func = f2;
            func(1);
            return 0;
        }
        '''
        args = [['0'], ['2']]
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_getone_debug(self):
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE_GETONE)
        for f in source.files.values():
            f.flags += ['-g']
        source.linker_flags += ['-g']
        self.__compare_enforced_unenforced(source, [['1'], ['2'], ['3'], ['4'], ['5']])
        self.__compare_enforced_unenforced(source.to_cxx(), [['1'], ['2'], ['3'], ['4'], ['5']])

    def test_getone_opt(self):
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE_GETONE)
        for f in source.files.values():
            f.flags = ['-O3', '-flto']
        source.linker_flags += ['-O3', '-flto', '-s']
        self.__compare_enforced_unenforced(source, [['1'], ['2'], ['3'], ['4'], ['5']])
        self.__compare_enforced_unenforced(source.to_cxx(), [['1'], ['2'], ['3'], ['4'], ['5']])

    def test_different_interfaces(self):
        SAMPLE = SAMPLE_PREFIX_HEADER + '''
        void test1(void* p1, void* p2) { printf("%d\\n", p1 < p2); }
        void test2(int* p1, int* p2) { printf("%d <-> %d\\n", *p1, *p2); }
        typedef void (*functype3)(long, long);
        
        functype3 noinline get(char c) {
            return c == '1' ? (functype3) test1 : (functype3) test2;
        }
        
        int main(int argc, const char* argv[]) {
            long a = 0x1122334455667788;
            functype3 f = get(argv[1][0]);
            f((long) &a, 4 + (long) &a);
            return 0;
        }
        '''
        args = [['0'], ['1']]
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_cxx_struct_init(self):
        SAMPLE = '''
        struct S {
            functype f;
            S(functype f) : f(f){}
        };
        S s(f1);
        
        functype noinline get_one(char c) {
            if (c == '1') s.f = f2;
            return s.f;
        }
        ''' + SAMPLE_GET_MAIN
        args = [['0'], ['1']]
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

        SAMPLE = '''
        struct S {
            static functype f;
            void target() { f(1); }
        };
        functype S::f = f1;
        noinline void test() { S s; s.f = f2; }
        noinline int main(int argc, const char* argv[]) {
            if (argv[1][0] == '1') test();
            S s;
            s.target();
            return 0;
        }
        '''
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_const_global_array_static(self):
        SAMPLE = '''
        extern const struct function_unit_desc {
          const char *const name;
          int (*const func) (long x);
        } function_units[];

        const struct function_unit_desc function_units[] = {
          {"func1", 0}, {"func2", g1}, {"func3", (functype2) f1}  
        };

        functype noinline get_one(char c) {
          return (functype) function_units[c - '0'].func;
        }
        ''' + SAMPLE_GET_MAIN
        args = [['0'], ['1'], ['2']]
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_nested_array_init(self):
        SAMPLE = '''
        typedef struct S {
            int x;
            functype a;
            functype b;
        } S;
        static S global1[] = {
            {0, f1, (functype) g1}
        };
        struct { int x; S* s; } global2[] = {
            {0, &global1[0]}
        };
        int main() {
            global2[0].s->a(1);
            global2[0].s->b(1);
            return 0;
        }
        '''
        args = [[]]
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_vararg_from_fixed_type(self):
        SAMPLE = '''
        #include <stdarg.h>
        typedef void (*functype3)(int x, ...);
        typedef void (*functype4)(int x, int y, int z);
        void test1(int x, ...) {
            va_list args; va_start(args, x);
            printf("[test1]");
            for (int i = 0; i < x; i++) printf(" %d", va_arg(args, int));
            va_end(args);
            printf("\\n");
        }
        void test2(int x, ...) {
            va_list args; va_start(args, x);
            printf("[test2]");
            for (int i = 0; i < x; i++) printf(" %d", va_arg(args, int));
            va_end(args);
            printf("\\n");
        }
        noinline functype4 get(char c) {
            return c == '1' ? (functype4) test1 : (functype4) test2;
        }
        int main(int argc, const char* argv[]) {
            functype4 f = get(argv[1][0]);
            f(2,1,0);
            return 0;
        }
        '''
        args = [['1'], ['2']]
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_vararg(self):
        SAMPLE = '''
        #include <stdarg.h>
        typedef void (*functype3)(int x, ...);
        void test1(int x, ...) {
            va_list args; va_start(args, x);
            printf("[test1]");
            for (int i = 0; i < x; i++) printf(" %d", va_arg(args, int));
            va_end(args);
            printf("\\n");
        }
        void test2(int x, ...) {
            va_list args; va_start(args, x);
            printf("[test2]");
            for (int i = 0; i < x; i++) printf(" %d", va_arg(args, int));
            va_end(args);
            printf("\\n");
        }
        noinline functype3 get(char c) {
            return c == '1' ? test1 : test2;
        }
        int main(int argc, const char* argv[]) {
            functype3 f = get(argv[1][0]);
            f(5, 4, 3, 2, 1, 0);
            return 0;
        }
        '''
        args = [['1'], ['2']]
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_libc_functions(self):
        SAMPLE = '''
        #include <stdlib.h>
        #include <stdio.h>
        #include <signal.h>
        #include <unistd.h>
        typedef void (*sighandler_t)(int);
        typedef void (*exitfunc)(void);
        void f1(void) { printf("f1()\\n"); }
        void f2(void) { printf("f2()\\n"); }
        void g1(int x) { printf("g1(%d)\\n", x); }
        void g2(int x) { printf("g2(%d)\\n", x); }
        int main(int argc, const char* argv[]) {
            sighandler_t fs = argv[1][0] == '1' ? g1 : g2;
            exitfunc fe = argv[1][0] == '2' ? f2 : f1;
            signal(SIGUSR1, fs);
            atexit(fe);
            kill(getpid(), SIGUSR1);
            return 0;
        }
        '''
        args = [['0'], ['1'], ['2']]
        source = SourceCode.from_string(SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_libc_functions_direct(self):
        SAMPLE = '''
        #include <stdlib.h>
        #include <stdio.h>
        #include <signal.h>
        #include <unistd.h>
        void f1(void) { printf("f1()\\n"); }
        void g1(int x) { printf("g1(%d)\\n", x); }
        int main(int argc, const char* argv[]) {
            signal(SIGUSR1, g1);
            atexit(f1);
            kill(getpid(), SIGUSR1);
            return 0;
        }
        '''
        args = [[]]
        source = SourceCode.from_string(SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_sigaction(self):
        SAMPLE = '''
        #include <stdlib.h>
        #include <stdio.h>
        #include <signal.h>
        #include <unistd.h>
        typedef void (*sighandler_t)(int);
        void g1(int x) { printf("g1(%d)\\n", x); }
        void g2(int x) { printf("g2(%d)\\n", x); }
        int main(int argc, const char* argv[]) {
            struct sigaction sa;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sa.sa_handler = argv[1][0] == '1' ? g1 : g2;
            sa.sa_handler(18);
            sigaction(SIGUSR1, &sa, NULL);
            kill(getpid(), SIGUSR1);
            return 0;
        }
        '''
        args = [['0'], ['1'], ['2']]
        source = SourceCode.from_string(SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_sigaction_multi(self):
        SAMPLE = '''
        #include <stdlib.h>
        #include <stdio.h>
        #include <signal.h>
        #include <unistd.h>
        typedef void (*sighandler_t)(int);
        void g1(int x) { printf("g1(%d)\\n", x); }
        void g2(int x) { printf("g2(%d)\\n", x); }
        int main(int argc, const char* argv[]) {
            struct sigaction sa;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sa.sa_handler = argv[1][0] == '1' ? g1 : g2;
            sa.sa_handler(18);
            sigaction(SIGUSR1, &sa, NULL);
            sigaction(SIGUSR2, &sa, NULL);
            kill(getpid(), SIGUSR1);
            kill(getpid(), SIGUSR2);
            return 0;
        }
        '''
        args = [['0'], ['1'], ['2']]
        source = SourceCode.from_string(SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_zero_targets(self):
        SAMPLE = '''
        functype noinline get_one(char c) { return 0; }
        ''' + SAMPLE_GET_MAIN
        args = [[]]
        source = SourceCode.from_string(SAMPLE_PREFIX_HEADER + SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

        SAMPLE = '''
        functype2 noinline get_one() { return 0; }
        int main(int argc, const char* argv[]) {
            functype2 f = get_one();
            if (f) {
                printf("%d\\n", f(1));
            }
            puts("DONE");
            return 0;
        }
        '''
        source = SourceCode.from_string(SAMPLE_PREFIX_HEADER + SAMPLE)
        self.__compare_enforced_unenforced(source, args)
        self.__compare_enforced_unenforced(source.to_cxx(), args)

    def test_mixed_usage(self):
        SAMPLE = '''
        functype noinline get_one(char c) {
            f1(1); f2(2); printf("%d %d\\n", g1(3), g2(4));
            
            if (c == '1') { f1(1); } else { f2(1); }
            
            if (c == '1') return f1;
            if (c == '2') return f2;
            if (c == '3') return (functype) g1;
            return NULL;
        }
        ''' + SAMPLE_GET_MAIN
        source = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        self.__compare_enforced_unenforced(source, [['1'], ['2']])
        self.__compare_enforced_unenforced(source.to_cxx(), [['1'], ['2']])

    def test_obstack(self):
        SAMPLE = '''
        #include <stdlib.h>
        #include <stdio.h>
        #include <obstack.h>
        #define obstack_chunk_alloc malloc
        #define obstack_chunk_free free

        void* __attribute__((noinline)) get() { return malloc(1) == 0 ? 0 : free; }

        int main() {
            void (*test)(void *) = get();

            struct obstack ob_stack;
            obstack_init(&ob_stack);

            fprintf(stderr, "%p %p %p %p\\n", &free, test, ob_stack.chunkfun, ob_stack.freefun);
            test(0);

            int* testobj = (int*) obstack_alloc(&ob_stack, 16);
            int* testobj2 = (int*) obstack_alloc(&ob_stack, 4096);
            int* testobj3 = (int*) obstack_alloc(&ob_stack, 4096);
            testobj[0] = 0x1337;
            testobj2[0] = 0x1337;
            obstack_free(&ob_stack, testobj3);
            obstack_free(&ob_stack, testobj2);
            obstack_free(&ob_stack, testobj);

            obstack_free(&ob_stack, NULL);
            return 0;
        }
        '''
        code = SourceCode.from_string(SAMPLE)
        binary, outputs = compile_link_run(code, [[]], enforce=True, output_graph=True)
        # Typegraph.load_from_file(binary + '.after.typegraph').build_dot()
        print(outputs[0].decode())

    def test_direct_and_indirect_calls(self):
        SAMPLE1 = SAMPLE_PREFIX_HEADER + '''
        noinline void f(long l) { fprintf(stderr, "f %ld\\n", l); }
        '''
        SAMPLE2 = SAMPLE_PREFIX_HEADER + '''
        noinline void f(int i);
        noinline void g(int i) {}
        noinline functype getone(char c) {
            return c == '1' ? &f : &g;
        }
        int main(int argc, const char* argv[]) {
            getone(argv[1][0])(1);
            f(2);
            return 0;
        }
        '''
        code = SourceCode.from_string(SAMPLE1)
        code.add_file(SourceCodeFile(SAMPLE2))
        self.__compare_enforced_unenforced(code, [['1'], ['2']])


class DynamicLinkingTests(unittest.TestCase):
    library = None

    # compile library once and for all
    @classmethod
    def __get_library(cls) -> str:
        if cls.library is None:
            code = SourceCode.from_string(LIBRARY_CODE)
            code.make_shared()
            code.env['TG_DYNLIB_SUPPORT'] = '1'
            code.env['TG_ENFORCE_ID_BITWIDTH'] = '20'
            cls.library = compile_link(code, enforce=True)
            atexit.register(lambda: os.remove(cls.library) if os.path.exists(cls.library) else None)
        return cls.library

    def __compile_library(self, sourcecode: str):
        code = SourceCode.from_string(sourcecode)
        code.make_shared()
        code.env['TG_DYNLIB_SUPPORT'] = '1'
        code.env['TG_ENFORCE_ID_BITWIDTH'] = '20'
        library = compile_link(code, enforce=True)
        atexit.register(lambda: os.remove(library) if os.path.exists(library) else None)
        return library

    def test_graph_leaking_functions(self):
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        int noinline h1(long x) { return 13; }
        
        // these calls / uses are internal
        functype2 noinline get2(char c) { return c == '1' ? g1 : g2; }
        void run_int(char c) { get2(c)(1); }
        // these calls / uses are external
        functype noinline get1(char c) { return c == '1' ? f1 : f2; }
        void run_ext(char c) {
            library_get1(c)(1);
            library_call1(get1(c));
            library_call2(h1);
        }
        int main(int argc, const char* argv[]) { run_int(argv[1][0]); run_ext(argv[1][0]); return 0; }
        '''
        code = SourceCode.from_string(SAMPLE).add_library(self.__get_library())
        binary = compile_link(code, output_graph=True, enforce=True)
        graph = Typegraph.load_from_file(binary + '.min.typegraph')
        # assert: only one call is external
        self.assertEqual(1, len(graph.calls))
        self.assertIn('in run_ext', list(graph.calls.keys())[0])
        # assert: only f1/f2/h1 are leaking
        used_functions = list(sorted(graph.usedFunctions.keys()))
        self.assertEqual(['f1', 'f2', 'h1'], used_functions)
        # assert: context h1 is external
        self.assertNotEqual(-1, graph.type_context_to_vertex[('long', 'h1')])
        # assert: no context g1/g2 is external
        self.assertNotIn(('long', 'g1'), graph.type_context_to_vertex)
        self.assertNotIn(('long', 'g2'), graph.type_context_to_vertex)
        # current: 10 nodes total
        self.assertLess(graph.graph.num_vertices(), 15)

    # Test linking
    def test_simple_linking(self):
        SAMPLE = LIBRARY_HEADER + '''
        void noinline run(char c) {
            printf("global = 0x%x\\n", lib_global);
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(self.__get_library())
        binary, outputs = compile_link_run(code, [[]], enforce=True)
        self.assertEqual(b'global = 0x1337\n', outputs[0])

    # Test: defined=prog, used=prog, called=prog
    def test_prog_defined_prog_called(self):
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        functype noinline get(char c) {
            return c == '1' ? f1 : f2;
        }
        void noinline run(char c) {
            assert_function_is_id(get(c));
            get(c)(1);
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(self.__get_library())
        binary, outputs = compile_link_run(code, [['1'], ['2']], enforce=True)
        self.assertEqual(b'f1(1)\n', outputs[0])
        self.assertEqual(b'f2(1)\n', outputs[1])

    # Test: defined=prog, used=prog, called=lib
    def test_prog_defined_lib_called(self):
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        void noinline run(char c) {
            library_call1(c == '1' ? f1 : f2);
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(self.__get_library())
        binary, outputs = compile_link_run(code, [['1'], ['2']], enforce=True)
        self.assertEqual(b'f1(1)\n', outputs[0])
        self.assertEqual(b'f2(1)\n', outputs[1])

    # Test: defined=lib, used=lib, called=prog
    def test_lib_defined_prog_called(self):
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        void noinline run(char c) {
            functype f = library_get1(c);
            assert_function_is_id(f);
            f(1);
            functype2 g = library_get2(c);
            assert_function_is_id2(g);
            printf("Result = %d\\n", g(2));
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(self.__get_library())
        binary, outputs = compile_link_run(code, [['1'], ['2']], enforce=True)
        self.assertEqual(b'lib11 1\nlib21 2\nResult = 21\n', outputs[0])
        self.assertEqual(b'lib12 1\nlib22 2\nResult = 22\n', outputs[1])

    # Test: defined=lib, used=lib, called=lib
    def test_lib_defined_lib_called(self):
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        void noinline run(char c) {
            library_call1(library_get1(c));
            library_call2(library_get2(c));
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(self.__get_library())
        binary, outputs = compile_link_run(code, [['1'], ['2']], enforce=True)
        self.assertEqual(b'lib11 1\nlib21 2\nlibrary_call2 => 21\n', outputs[0])
        self.assertEqual(b'lib12 1\nlib22 2\nlibrary_call2 => 22\n', outputs[1])

    # Test: defined=lib, used=prog, called=prog
    def test_lib_defined_prog_used_prog_called(self):
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        functype noinline get(char c) {
            return c == '1' ? lib11 : &lib12;
        }
        void noinline run(char c) {
            assert_function_is_id(get(c));
            get(c)(1);
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(self.__get_library())
        binary, outputs = compile_link_run(code, [['1'], ['2']], enforce=True)
        self.assertEqual(b'lib11 1\n', outputs[0])
        self.assertEqual(b'lib12 1\n', outputs[1])

    # Test: defined=lib, used=prog, called=lib
    def test_lib_defined_prog_used_lib_called(self):
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        void noinline run(char c) {
            library_call1(c == '1' ? lib11 : &lib12);
            library_call2(c == '1' ? lib21 : &lib22);
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(self.__get_library())
        binary, outputs = compile_link_run(code, [['1'], ['2']], enforce=True)
        self.assertEqual(b'lib11 1\nlib21 2\nlibrary_call2 => 21\n', outputs[0])
        self.assertEqual(b'lib12 1\nlib22 2\nlibrary_call2 => 22\n', outputs[1])

    # Test: defined/used=prog, converted=lib, called=prog
    def test_prog_defined_lib_converted_prog_called(self):
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        functype2 noinline get(char c) {
            return c == '1' ? g1 : &g2;
        }
        void noinline run(char c) {
            functype f = convert_2to1(get(c));
            assert_function_is_id(f);
            f(1);
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(self.__get_library())
        binary, outputs = compile_link_run(code, [['1'], ['2']], enforce=True)
        self.assertEqual(b'g1(1)\n', outputs[0])
        self.assertEqual(b'g2(1)\n', outputs[1])

    # Test: defined/used=prog, converted=lib, called=lib
    def test_prog_defined_lib_converted_lib_called(self):
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        void noinline run(char c) {
            functype f = convert_2to1(c == '1' ? g1 : &g2);
            library_call1(f);
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(self.__get_library())
        binary, outputs = compile_link_run(code, [['1'], ['2']], enforce=True)
        self.assertEqual(b'g1(1)\n', outputs[0])
        self.assertEqual(b'g2(1)\n', outputs[1])

    # Test: defined/used=lib, converted=lib, called=prog
    def test_lib_defined_lib_converted_prog_called(self):
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        void noinline run(char c) {
            functype f = convert_2to1(library_get2(c));
            assert_function_is_id(f);
            f(1);
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(self.__get_library())
        binary, outputs = compile_link_run(code, [['1'], ['2']], enforce=True)
        self.assertEqual(b'lib21 1\n', outputs[0])
        self.assertEqual(b'lib22 1\n', outputs[1])

    # Test: defined/used=lib, converted=lib, called=lib
    def test_lib_defined_lib_converted_lib_called(self):
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        void noinline run(char c) {
            functype f = convert_2to1(library_get2(c));
            library_call1(f);
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(self.__get_library())
        binary, outputs = compile_link_run(code, [['1'], ['2']], enforce=True)
        self.assertEqual(b'lib21 1\n', outputs[0])
        self.assertEqual(b'lib22 1\n', outputs[1])

    # Test: defined/used=lib, converted=prog, called=prog
    def test_lib_defined_prog_converted_prog_called(self):
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        void noinline run(char c) {
            functype f = (functype) library_get2(c);
            assert_function_is_id(f);
            f(1);
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(self.__get_library())
        binary, outputs = compile_link_run(code, [['1'], ['2']], enforce=True)
        self.assertEqual(b'lib21 1\n', outputs[0])
        self.assertEqual(b'lib22 1\n', outputs[1])

    # Test: defined/used=lib, converted=prog, called=lib
    def test_lib_defined_prog_converted_lib_called(self):
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        void noinline run(char c) {
            functype f = (functype) library_get2(c);
            library_call1(f);
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(self.__get_library())
        binary, outputs = compile_link_run(code, [['1'], ['2']], enforce=True)
        self.assertEqual(b'lib21 1\n', outputs[0])
        self.assertEqual(b'lib22 1\n', outputs[1])

    # instrumentation output (tgcfi.json)
    def test_tgcfi_output(self):
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        void noinline run(char c) {
            library_get1(c)(1);
            printf("Result = %d\\n", library_get2(c)(2));
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(self.__get_library())
        output_base = os.path.join(TMPDIR, os.path.basename(code.binary_fname))
        code.env['TG_CFI_OUTPUT'] = output_base + '-tgcfi.json'
        binary = compile_link(code, enforce=True)
        env = dict(os.environ.items())
        env['TG_CFI_OUTPUT_RT'] = output_base + '-tgcfi-rt.json'
        subprocess.check_call(RUN_PREFIX + [binary, '1'], timeout=7, env=env)
        with open(output_base + '-tgcfi.json', 'r') as f:
            tgcfi = json.loads(f.read())
        with open(output_base + '-tgcfi-rt.json', 'r') as f:
            tgcfi_rt = json.loads(f.read())
        self.assertEqual([], tgcfi['tg_targets']['call#80.0 in run'])
        self.assertEqual([], tgcfi['tg_targets']['call#80.1 in run'])
        self.assertEqual([], tgcfi['tg_targets_argnum']['call#80.0 in run'])
        self.assertEqual([], tgcfi['tg_targets_argnum']['call#80.1 in run'])
        self.assertEqual(2, len(tgcfi_rt['modules']))
        self.assertIn('lib11', tgcfi_rt['tg_additional_targets']['call#80.0 in run'])
        self.assertIn('lib12', tgcfi_rt['tg_additional_targets']['call#80.0 in run'])
        self.assertIn('lib21', tgcfi_rt['tg_additional_targets']['call#80.1 in run'])
        self.assertIn('lib22', tgcfi_rt['tg_additional_targets']['call#80.1 in run'])
        self.assertEqual(2, len(tgcfi_rt['tg_additional_targets']['call#80.0 in run']))
        self.assertEqual(2, len(tgcfi_rt['tg_additional_targets']['call#80.1 in run']))
        self.assertEqual(0, len(tgcfi_rt['tg_additional_targets']['call#80.0 in library_call1']))
        self.assertEqual(0, len(tgcfi_rt['tg_additional_targets']['call#80.0 in library_call2']))

    # TODO Test: interface with more complex structure (struct etc to check LLVM naming consistency)
    # TODO Test: interface with unique nodes data transfer
    # TODO Test: dispatch with more parameters (registers + stack) or complex return values (struct on stack etc)
    # TODO Test: multiple libraries (+lib/lib interaction)
    def test_multiple_libraries(self):
        SAMPLE = '''
        void run(char c) {
            // prog => lib2 => lib1
            xlibrary_delegate_call1(c == '1' ? f1 : f2);
            // lib2 => lib1
            library_call1(xlibrary_get1(c));
            // lib1 => lib2
            xlibrary_call2(library_get2(c));
            xlibrary_delegate_call2(library_get2(c));
            // libs => prog
            functype f = c == '1' ? library_get1(c) : xlibrary_get1(c);
            f(17);
        }
        '''
        code = SourceCode.from_string(LIBRARY2).make_shared().add_library(self.__get_library())
        code.env['TG_ENFORCE_ID_BITWIDTH'] = '20'
        library2 = compile_link(code, enforce=True)
        atexit.register(lambda: os.remove(library2) if os.path.exists(library2) else None)
        code = SourceCode.from_string(LIBRARY2_HEADER + SAMPLE_PREFIX_DEFS + SAMPLE + LIBRARY_MAIN)
        code.add_library(self.__get_library()).add_library(library2)

        binary, outputs = compile_link_run(code, [['1'], ['2']], enforce=True)
        self.assertEqual(b'f1(1)\nxlib11 1\nlib21 8\nxlibrary_call2 => 21\nlib21 2\nlibrary_call2 => 21\nlib11 17\n', outputs[0])
        self.assertEqual(b'f2(1)\nxlib12 1\nlib22 8\nxlibrary_call2 => 22\nlib22 2\nlibrary_call2 => 22\nxlib12 17\n', outputs[1])

    def test_dynamic_loading(self):
        SAMPLE = LIBRARY_HEADER + '''
        #include <dlfcn.h>
        int main() {
            void* handle = dlopen("LIBRARY_PATH", RTLD_LAZY);
            if (handle) {
                functype f = (functype) dlsym(handle, "lib3");
                if (f) f(1);
                else fprintf(stderr, "Function not found!\\n");
            } else {
                fprintf(stderr, "Library could not be loaded!\\n");
            }
            return 0;
        }
        '''.replace('LIBRARY_PATH', self.__get_library())
        code = SourceCode.from_string(SAMPLE)
        code.env['TG_DYNLIB_SUPPORT'] = '1'
        code.linker_flags.append('-ldl')
        binary, outputs = compile_link_run(code, [[]], enforce=True)
        self.assertEqual(b'lib3 1\n', outputs[0])

    def test_dynamic_loading_2(self):
        SAMPLE = LIBRARY_HEADER + '''
        #include <dlfcn.h>
        int main(int argc, const char* argv[]) {
            const char* funcname = argv[1][0] == '1' ? "lib3" : "lib11";
            void* handle = dlopen("LIBRARY_PATH", RTLD_LAZY);
            if (handle) {
                functype f = (functype) dlsym(handle, funcname);
                if (f) f(1);
                else fprintf(stderr, "Function not found!\\n");
            } else {
                fprintf(stderr, "Library could not be loaded!\\n");
            }
            return 0;
        }
        '''.replace('LIBRARY_PATH', self.__get_library())
        code = SourceCode.from_string(SAMPLE)
        code.env['TG_DYNLIB_SUPPORT'] = '1'
        code.linker_flags.append('-ldl')
        binary, outputs = compile_link_run(code, [['1'], ['2']], enforce=True)
        self.assertEqual(b'lib3 1\n', outputs[0])
        self.assertEqual(b'lib11 1\n', outputs[1])

    def test_dynamic_loading_indirect(self):
        SAMPLE = SAMPLE_PREFIX + LIBRARY_HEADER + '''
        #include <dlfcn.h>
        typedef void (*higherorder)(functype f);
        int main() {
            void* handle = dlopen("LIBRARY_PATH", RTLD_LAZY);
            if (handle) {
                higherorder f = (higherorder) dlsym(handle, "library_call1_10");
                if (f) f(f1);
                else fprintf(stderr, "Function not found!\\n");
            } else {
                fprintf(stderr, "Library could not be loaded!\\n");
            }
            return 0;
        }
        '''.replace('LIBRARY_PATH', self.__get_library())
        code = SourceCode.from_string(SAMPLE)
        code.env['TG_DYNLIB_SUPPORT'] = '1'
        code.linker_flags.append('-ldl')
        # binary = compile_link(code, enforce=True, output_graph=True)
        # Typegraph.load_from_file(binary + ".after.typegraph").build_dot()
        # Typegraph.load_from_file(binary + ".min.typegraph").build_dot()
        binary, outputs = compile_link_run(code, [[]], enforce=True)
        self.assertEqual(b'f1(10)\n', outputs[0])

    def test_dynamic_loading_2_indirect(self):
        SAMPLE = SAMPLE_PREFIX + LIBRARY_HEADER + '''
        #include <dlfcn.h>
        typedef void (*higherorder)(functype f);
        int main(int argc, const char* argv[]) {
            const char* funcname = argv[1][0] == '1' ? "library_call1_10" : "library_call1_20";
            void* handle = dlopen("LIBRARY_PATH", RTLD_LAZY);
            if (handle) {
                higherorder f = (higherorder) dlsym(handle, funcname);
                if (f) f(argv[1][0] == '2' ? f2 : f1);
                else fprintf(stderr, "Function not found!\\n");
            } else {
                fprintf(stderr, "Library could not be loaded!\\n");
            }
            return 0;
        }
        '''.replace('LIBRARY_PATH', self.__get_library())
        code = SourceCode.from_string(SAMPLE)
        code.env['TG_DYNLIB_SUPPORT'] = '1'
        code.linker_flags.append('-ldl')
        binary, outputs = compile_link_run(code, [['0'], ['1'], ['2']], enforce=True)
        self.assertEqual(b'f1(20)\n', outputs[0])
        self.assertEqual(b'f1(10)\n', outputs[1])
        self.assertEqual(b'f2(20)\n', outputs[2])

    def test_autoconf_compat(self):
        SAMPLE = '''
        char dlsym ();
        int main () { return dlsym (); }
        '''
        code = SourceCode.from_string(SAMPLE)
        code.env['TG_DYNLIB_SUPPORT'] = '1'
        code.linker_flags.append('-ldl')
        binary = compile_link(code)
        os.remove(binary)

    def test_jit_tree(self):
        HEADER = SAMPLE_PREFIX_HEADER + '''
        functype get_func(int i);
        void do_call(functype f);
        '''
        LIBRARY = '''
        static void func1(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func2(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func3(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func4(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func5(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func6(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func7(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static functype functions[] = {NULL, func1, func2, func3, func4, func5, func6, func7};
        functype get_func(int i) { return functions[i]; }
        void do_call(functype f) { if (f) f(1); else printf("<null>\\n"); }
        '''
        PROGRAM = '''
        static void pfunc0(int x) { printf("prog %s(%d)\\n", __func__, x); }
        static void pfunc1(int x) { printf("prog %s(%d)\\n", __func__, x); }
        static void pfunc2(int x) { printf("prog %s(%d)\\n", __func__, x); }
        static void pfunc3(int x) { printf("prog %s(%d)\\n", __func__, x); }
        static void pfunc4(int x) { printf("prog %s(%d)\\n", __func__, x); }
        static void pfunc5(int x) { printf("prog %s(%d)\\n", __func__, x); }
        static void pfunc6(int x) { printf("prog %s(%d)\\n", __func__, x); }
        static void pfunc7(int x) { printf("prog %s(%d)\\n", __func__, x); }
        static functype program_functions[] = {pfunc0, pfunc1, pfunc2, pfunc3, pfunc4, pfunc5, pfunc6, pfunc7};
        int main(int argc, const char *argv[]) {
            for (int i = 1; i < argc; i++) {
                if (argv[i][0] == 'c') {
                    do_call((functype) atoi(argv[i]+1));
                } else {
                    int k = atoi(argv[i]);
                    do_call(k >= 0 ? program_functions[k] : get_func(-k));
                }
            }
        }
        '''
        library = self.__compile_library(HEADER + LIBRARY)
        code = SourceCode.from_string(HEADER + PROGRAM)
        code.add_library(library)

        binary, outputs = compile_link_run(code, [[str(i) for i in range(-7, 8)]], enforce=True)
        atexit.register(lambda: os.remove(binary) if os.path.exists(binary) else None)
        expected = ''.join(f'lib func{i}(1)\n' for i in range(7, 0, -1))
        expected += ''.join(f'prog pfunc{i}(1)\n' for i in range(0, 8))
        self.assertEqual(outputs[0], expected.encode())
        # test some error cases
        for c in [1, 10, 11, 10000, 100000, -1, -10]:
            r = subprocess.run(RUN_PREFIX + [binary, f'c{c}'], stdout=subprocess.PIPE)
            self.assertEqual(ARCH['signal'], r.returncode)

    def test_jit_jumptable(self):
        HEADER = SAMPLE_PREFIX_HEADER + '''
        functype get_func(int i);
        void do_call(functype f);
        '''
        LIBRARY = '''
        static void func1(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func2(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func3(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func4(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func5(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func6(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func7(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func8(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func9(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func10(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func11(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static void func12(int x) { printf("lib %s(%d)\\n", __func__, x); }
        static functype functions[] = {NULL, func1, func2, func3, func4, func5, func6, func7, func8, func9, func10, func11, func12};
        functype get_func(int i) { return functions[i]; }
        void do_call(functype f) { if (f) f(1); else printf("<null>\\n"); }
        '''
        PROGRAM = '''
        int main(int argc, const char *argv[]) {
            for (int i = 1; i < argc; i++) {
                if (argv[i][0] == 'c') {
                    do_call((functype) atoi(argv[i]+1));
                } else {
                    int k = atoi(argv[i]);
                    do_call(get_func(k));
                }
            }
        }
        '''
        library = self.__compile_library(HEADER + LIBRARY)
        code = SourceCode.from_string(HEADER + PROGRAM)
        code.add_library(library)

        binary, outputs = compile_link_run(code, [[str(i) for i in range(1, 13)]], enforce=True)
        atexit.register(lambda: os.remove(binary) if os.path.exists(binary) else None)
        expected = ''.join(f'lib func{i}(1)\n' for i in range(1, 13))
        self.assertEqual(outputs[0], expected.encode())
        # test some error cases
        for c in [1, 10, 11, 10000, 100000, -1, -10]:
            r = subprocess.run(RUN_PREFIX + [binary, f'c{c}'], stdout=subprocess.PIPE)
            print(c, r.returncode, r.stdout)
            self.assertEqual(ARCH['signal'], r.returncode)

    def test_jit_jumptable_2(self):
        HEADER = SAMPLE_PREFIX_HEADER + '''
        functype get_func(int i);
        void do_call(functype f);
        '''
        LIBRARY = '''
        void do_call(functype f) { if (f) f(1); else printf("<null>\\n"); }
        '''
        PROGRAM = '''
        static void func1(int x) { printf("%s(%d)\\n", __func__, x); }
        static void func2(int x) { printf("%s(%d)\\n", __func__, x); }
        static void func3(int x) { printf("%s(%d)\\n", __func__, x); }
        static void func4(int x) { printf("%s(%d)\\n", __func__, x); }
        static void func5(int x) { printf("%s(%d)\\n", __func__, x); }
        static void func6(int x) { printf("%s(%d)\\n", __func__, x); }
        static void func7(int x) { printf("%s(%d)\\n", __func__, x); }
        static void func8(int x) { printf("%s(%d)\\n", __func__, x); }
        static void func9(int x) { printf("%s(%d)\\n", __func__, x); }
        static void func10(int x) { printf("%s(%d)\\n", __func__, x); }
        static void func11(int x) { printf("%s(%d)\\n", __func__, x); }
        static void func12(int x) { printf("%s(%d)\\n", __func__, x); }
        static functype functions[] = {NULL, func1, func2, func3, func4, func5, func6, func7, func8, func9, func10, func11, func12};
        functype get_func(int i) { return functions[i]; }
        
        int main(int argc, const char *argv[]) {
            for (int i = 1; i < argc; i++) {
                if (argv[i][0] == 'c') {
                    do_call((functype) atoi(argv[i]+1));
                } else {
                    int k = atoi(argv[i]);
                    do_call(get_func(k));
                }
            }
        }
        '''
        library = self.__compile_library(HEADER + LIBRARY)
        code = SourceCode.from_string(HEADER + PROGRAM)
        code.add_library(library)

        binary, outputs = compile_link_run(code, [[str(i) for i in range(1, 13)]], enforce=True)
        atexit.register(lambda: os.remove(binary) if os.path.exists(binary) else None)
        expected = ''.join(f'func{i}(1)\n' for i in range(1, 13))
        self.assertEqual(outputs[0], expected.encode())
        # test some error cases
        for c in [1, 14, 15, 10000, 100000, -1, -5, -10]:
            r = subprocess.run(RUN_PREFIX + [binary, f'c{c}'], stdout=subprocess.PIPE)
            print(c, r.returncode, r.stdout)
            self.assertEqual(ARCH['signal'], r.returncode)

    def test_resolve_points(self):
        SAMPLE = LIBRARY_HEADER + SAMPLE_PREFIX_DEFS + '''
        #include <signal.h>
        #include <unistd.h>
        void noinline run(char c) {
            functype f = library_get1(c);
            signal(10, f);
            kill(getpid(), 10);
        }
        ''' + LIBRARY_MAIN
        code = SourceCode.from_string(SAMPLE).add_library(self.__get_library())
        binary, outputs = compile_link_run(code, [['1'], ['2']], enforce=True)
        self.assertEqual(b'lib11 10\n', outputs[0])
        self.assertEqual(b'lib12 10\n', outputs[1])

    pass


class MuslLibcTestCases(unittest.TestCase):
    def _make_musl(self, code: Union[SourceCode,str]) -> SourceCode:
        if isinstance(code, str):
            code = SourceCode.from_string(code)
        code.add_library_support()
        code.compiler_binary = MUSL_CLANG
        for f in code.files.values():
            f.compiler_binary = MUSL_CLANG
        return code

    def _compile_link_run(self, code: Union[SourceCode,str], arguments: List[List[str]], enforce: bool = False, instrument: bool = False) -> Tuple[str, List[bytes]]:
        code = self._make_musl(code)
        return compile_link_run(code, arguments, enforce=enforce, instrument_collect_calltargets=instrument)

    def _compare_enforced_unenforced(self, program: Union[SourceCode,str], arguments: List[List[str]], compare_stderr=True) -> Tuple[str, str]:
        # compile stuff with GNU
        program.binary_fname = program.binary_fname.replace('.enf.bin', '.bin').replace('.bin', '.ref.bin')
        binary_ref = compile_link(program, enforce=False)
        atexit.register(lambda: os.remove(binary_ref) if os.path.exists(binary_ref) else None)
        # compile stuff with MUSL / enforced
        program = self._make_musl(program)
        program.binary_fname = program.binary_fname.replace('.ref.bin', '.enf.bin')
        binary_enf = compile_link(program, enforce=True)
        atexit.register(lambda: os.remove(binary_enf) if os.path.exists(binary_enf) else None)
        # run and compare
        for args in arguments:
            result_ref = subprocess.run(RUN_PREFIX + [binary_ref] + args, timeout=7, stdout=subprocess.PIPE, stderr=subprocess.PIPE, input=b'')
            result_enf = subprocess.run(RUN_PREFIX + [binary_enf] + args, timeout=7, stdout=subprocess.PIPE, stderr=subprocess.PIPE, input=b'')
            print('ref', result_ref)
            print('enf', result_enf)
            if result_ref.returncode != result_enf.returncode:
                print(f'Different return codes (expected {result_ref.returncode}, was {result_enf.returncode}) for inputs {args}')
                sys.stdout.flush()
                sys.stdout.write('=== stdout ===\n')
                sys.stdout.write(result_enf.stdout.decode(errors='ignore'))
                sys.stdout.write('\n=== stderr ===\n')
                sys.stdout.write(result_enf.stderr.decode(errors='ignore'))
                self.assertEqual(result_ref.returncode, result_enf.returncode)
            if result_ref.stdout != result_enf.stdout:
                print(f'Different output for inputs {args}')
                sys.stdout.flush()
                sys.stdout.write('=== stdout (expected) ===\n')
                sys.stdout.write(result_ref.stdout.decode(errors='ignore'))
                sys.stdout.write('=== stdout (actual) ===\n')
                sys.stdout.write(result_enf.stdout.decode(errors='ignore'))
                sys.stdout.write('=== stderr (actual) ===\n')
                sys.stdout.write(result_enf.stderr.decode(errors='ignore'))
                self.assertEqual(result_ref.stdout, result_enf.stdout)
            if compare_stderr and result_ref.stderr != result_enf.stderr:
                print(f'Different stderr output for inputs {args}')
                sys.stdout.flush()
                sys.stdout.write('=== stderr (expected) ===\n')
                sys.stdout.write(result_ref.stderr.decode(errors='ignore'))
                sys.stdout.write('=== stderr (actual) ===\n')
                sys.stdout.write(result_enf.stderr.decode(errors='ignore'))
                self.assertEqual(result_ref.stderr, result_enf.stderr)
        return binary_ref, binary_enf

    def test_hello_world(self):
        code = SourceCode.from_string('#include <stdio.h>\nint main() { puts("Hello World!"); return 0; }')
        binary, outputs = self._compile_link_run(code, [[]], enforce=True)
        self.assertEqual(outputs, [b'Hello World!\n'])
        os.remove(binary)

    def test_getone_enforce(self):
        binary, outputs = self._compile_link_run(SAMPLE_PREFIX + SAMPLE_GETONE_2, [['1'], ['2'], ['3'], ['4']], enforce=True)
        self.assertEqual(outputs[0].decode().strip(), 'f1(1)')
        self.assertEqual(outputs[1].decode().strip(), 'f2(1)')
        self.assertEqual(outputs[2].decode().strip(), 'g1(1)')
        self.assertEqual(outputs[3].decode().strip(), 'g2(1)')
        rc = subprocess.run(RUN_PREFIX + [binary, '5'], timeout=10)
        os.remove(binary)
        self.assertEqual(rc.returncode, ARCH['signal'])

    def test_libc_functions(self):
        SAMPLE = '''
        #include <stdlib.h>
        #include <stdio.h>
        #include <signal.h>
        #include <unistd.h>
        typedef void (*sighandler_t)(int);
        typedef void (*exitfunc)(void);
        void f1(void) { printf("f1()\\n"); }
        void f2(void) { printf("f2()\\n"); }
        void g1(int x) { printf("g1(%d)\\n", x); }
        void g2(int x) { printf("g2(%d)\\n", x); }
        int main(int argc, const char* argv[]) {
            sighandler_t fs = argv[1][0] == '1' ? g1 : g2;
            exitfunc fe = argv[1][0] == '2' ? f2 : f1;
            signal(SIGUSR1, fs);
            atexit(fe);
            kill(getpid(), SIGUSR1);
            return 0;
        }
        '''
        args = [['0'], ['1'], ['2']]
        source = SourceCode.from_string(SAMPLE)
        self._compare_enforced_unenforced(source, args)
        self._compare_enforced_unenforced(source.to_cxx(), args)

    def test_libc_functions_direct(self):
        SAMPLE = '''
        #include <stdlib.h>
        #include <stdio.h>
        #include <signal.h>
        #include <unistd.h>
        void f1(void) { printf("f1()\\n"); }
        void g1(int x) { printf("g1(%d)\\n", x); }
        int main(int argc, const char* argv[]) {
            signal(SIGUSR1, g1);
            atexit(f1);
            kill(getpid(), SIGUSR1);
            return 0;
        }
        '''
        args = [[]]
        source = SourceCode.from_string(SAMPLE)
        self._compare_enforced_unenforced(source, args)
        self._compare_enforced_unenforced(source.to_cxx(), args)

    def test_sigaction(self):
        SAMPLE = '''
        #include <stdlib.h>
        #include <stdio.h>
        #include <signal.h>
        #include <unistd.h>
        typedef void (*sighandler_t)(int);
        void g1(int x) { printf("g1(%d)\\n", x); }
        void g2(int x) { printf("g2(%d)\\n", x); }
        int main(int argc, const char* argv[]) {
            struct sigaction sa;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sa.sa_handler = argv[1][0] == '1' ? g1 : g2;
            sa.sa_handler(18);
            sigaction(SIGUSR1, &sa, NULL);
            kill(getpid(), SIGUSR1);
            return 0;
        }
        '''
        args = [['0'], ['1'], ['2']]
        source = SourceCode.from_string(SAMPLE)
        self._compare_enforced_unenforced(source, args)
        self._compare_enforced_unenforced(source.to_cxx(), args)

    def test_sigaction_multi(self):
        SAMPLE = '''
        #include <stdlib.h>
        #include <stdio.h>
        #include <signal.h>
        #include <unistd.h>
        typedef void (*sighandler_t)(int);
        void g1(int x) { printf("g1(%d)\\n", x); }
        void g2(int x) { printf("g2(%d)\\n", x); }
        int main(int argc, const char* argv[]) {
            struct sigaction sa;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sa.sa_handler = argv[1][0] == '1' ? g1 : g2;
            sa.sa_handler(18);
            sigaction(SIGUSR1, &sa, NULL);
            sigaction(SIGUSR2, &sa, NULL);
            kill(getpid(), SIGUSR1);
            kill(getpid(), SIGUSR2);
            return 0;
        }
        '''
        args = [['0'], ['1'], ['2']]
        source = SourceCode.from_string(SAMPLE)
        self._compare_enforced_unenforced(source, args)
        self._compare_enforced_unenforced(source.to_cxx(), args)

    def test_fgets(self):
        code = SourceCode.from_string('''
        #include <stdio.h>
        #include <stdint.h>
        #include <stdlib.h>
        int main(int argc, const char* argv[]) {
          char buffer[7];
          FILE *f = fopen("/etc/passwd", "r");
          fgets(buffer, 6, f);
          puts(buffer);
          return 0;
        }
        ''')
        binary, output = self._compile_link_run(code, [[]], enforce=True)
        os.remove(binary)
        self.assertEqual(output[0], b'root:\n')

    def test_printf(self):
        code = SourceCode.from_string('''
        #include <stdio.h>
        int main(int argc, const char* argv[]) {
          printf("argc=%d\\n", argc);
          return 0;
        }
        ''')
        binary, output = self._compile_link_run(code, [[]], enforce=True)
        os.remove(binary)
        self.assertEqual(output[0], b'argc=1\n')

    def test_clock_gettime(self):
        code = SourceCode.from_string('''
        #include <stdio.h>
        #include <time.h>
        int main(int argc, const char* argv[]) {
            struct timespec ts;
            printf("%d\\n", clock_gettime(CLOCK_REALTIME, &ts));
        }
        ''')
        binary, output = self._compile_link_run(code, [[]], enforce=True)
        os.remove(binary)
        self.assertEqual(output[0], b'0\n')

    def test_qsort(self):
        code = SourceCode.from_string('''
        #include <stdio.h>
        #include <stdlib.h>
        int intcmp(const void *p1, const void* p2) {
            return *((int*) p1) - *((int*) p2);
        }
        int main(int argc, const char* argv[]) {
            int numbers[3] = {3,1,2};
            qsort(numbers, 3, sizeof(int), intcmp);
            printf("%d - %d - %d\\n", numbers[0], numbers[1], numbers[2]);
            return 0;
        }
        ''')
        binary, output = self._compile_link_run(code, [[]], enforce=True)
        os.remove(binary)
        self.assertEqual(output[0], b'1 - 2 - 3\n')

    def test_pthread(self):
        SAMPLE = '''
        #include <pthread.h>
        void *target(void *f) {
            ((functype) f)(1);
            return NULL;
        }
        int main() {
            void *vp = (void *) f1;
            pthread_t thread;
            pthread_create(&thread, NULL, target, vp);
            pthread_join(thread, 0);
            return 0;
        }
        '''
        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE)
        binary, output = self._compile_link_run(code, [[]], enforce=True)
        os.remove(binary)
        self.assertEqual(output[0], b'f1(1)\n')

    def _get_library(self) -> str:
        code = SourceCode.from_string(LIBRARY_CODE + '''
        #include <stdio.h>
        #include <stdlib.h>
        static int intcmp(const void *p1, const void* p2) {
            return *((int*) p1) - *((int*) p2);
        }
        void test_qsort() {
            int numbers[5] = {3,1,2,4,8};
            qsort(numbers, 5, sizeof(int), intcmp);
            printf("%d - %d - %d - %d - %d\\n", numbers[0], numbers[1], numbers[2], numbers[3], numbers[4]);
        }
        ''')
        code = self._make_musl(code.make_shared())
        library = compile_link(code, enforce=True)
        atexit.register(lambda: os.remove(library))
        return library

    def test_libraries(self):
        library = self._get_library()
        code = SourceCode.from_string(LIBRARY_HEADER + SAMPLE_PREFIX + '''
        void test_qsort();
        
        int main(int argc, const char *argv[]) {
            if (argc == 1) {
                test_qsort();
            } else if (argc == 2) {
                library_get1(argv[1][0])(1);
            } else if (argc == 3) {
                library_call1(argv[1][0] == '1' ? f1 : f2);
            }
            return 0;
        }
        ''')
        code.add_library(library)
        binary, output = self._compile_link_run(code, [[], ['1'], ['1', '1']], enforce=True)
        self.assertEqual(output[0], b'1 - 2 - 3 - 4 - 8\n')
        self.assertEqual(output[1], b'lib11 1\n')
        self.assertEqual(output[2], b'f1(1)\n')

    def test_dlopen(self):
        library = self._get_library()
        SAMPLE = LIBRARY_HEADER + '''
        #include <dlfcn.h>
        
        typedef void (*functype3)();
        
        int main() {
            void* handle = dlopen("LIBRARY_PATH", RTLD_LAZY);
            if (handle) {
                functype f = (functype) dlsym(handle, "lib3");
                if (f) f(1);
                else fprintf(stderr, "Function not found!\\n");
                
                functype3 f3 = (functype3) dlsym(handle, "test_qsort");
                if (f3) f3();
                else fprintf(stderr, "Function test_qsort not found!\\n");
            } else {
                fprintf(stderr, "Library could not be loaded!\\n");
            }
            return 0;
        }
        '''.replace('LIBRARY_PATH', library)
        code = self._make_musl(SourceCode.from_string(SAMPLE))
        code.env['TG_DYNLIB_SUPPORT'] = '1'
        code.linker_flags.append('-ldl')
        binary, outputs = compile_link_run(code, [[]], enforce=True)
        self.assertEqual(b'lib3 1\n1 - 2 - 3 - 4 - 8\n', outputs[0])

    def test_fork(self):
        code = SourceCode.from_string('''
        #include <unistd.h>
        #include <stdio.h>
        #include <stdlib.h>
        static int intcmp(const void *p1, const void* p2) { return *((int*) p1) - *((int*) p2); }
        static void require_computation() {
            int numbers[3] = {3,1,2};
            qsort(numbers, 3, sizeof(int), intcmp);
            printf("%d - %d - %d\\n", numbers[0], numbers[1], numbers[2]);
        } 
        int main(int argc, const char* argv[]) {
            if (argc > 1)
                require_computation();
            if (fork() == 0) {
                // child
                puts("Hello child!");
                require_computation();
                exit(0);
            } else {
                // parent
                sleep(1);
            }
            return 0;
        }
        ''')
        binary, output = self._compile_link_run(code, [[], ['1']], enforce=True)
        self.assertEqual(output[0], b'Hello child!\n1 - 2 - 3\n')
        self.assertEqual(output[1], b'1 - 2 - 3\nHello child!\n1 - 2 - 3\n')

    def test_atfork(self):
        code = SourceCode.from_string('''
        #include <unistd.h>
        #include <stdio.h>
        #include <stdlib.h>
        #include <pthread.h>
        static int intcmp(const void *p1, const void* p2) { return *((int*) p1) - *((int*) p2); }
        static void require_computation() {
            int numbers[3] = {3,1,2};
            qsort(numbers, 3, sizeof(int), intcmp);
            printf("%d - %d - %d\\n", numbers[0], numbers[1], numbers[2]);
        }
        static void parent(void) { sleep(1); puts("parent"); }
        static void child(void) { puts("child"); }
        int main(int argc, const char* argv[]) {
            if (argc > 1)
                require_computation();
            pthread_atfork(0, parent, child);
            if (fork() == 0) {
                // child
                exit(0);
            } else {
                // parent
                sleep(1);
            }
            return 0;
        }
        ''')
        binary, output = self._compile_link_run(code, [[], ['1']], enforce=True)
        self.assertEqual(output[0], b'child\nparent\n')
        self.assertEqual(output[1], b'1 - 2 - 3\nchild\nparent\n')


class MuslLibcStaticTestCases(MuslLibcTestCases):
    def _make_musl(self, code: Union[SourceCode, str]) -> SourceCode:
        if isinstance(code, str):
            code = SourceCode.from_string(code)
        code.compiler_binary = MUSL_CLANG
        code.linker_flags += ['-static', '-g']
        for f in code.files.values():
            f.compiler_binary = MUSL_CLANG
            f.flags += ['-g']
        return code

    def test_instrumentation(self):
        code = SourceCode.from_string(SAMPLE_PREFIX + SAMPLE_GETONE)
        try:
            binary, outputs = self._compile_link_run(code, [['1'], ['3']], enforce=False, instrument=True)
            self.assertEqual(outputs, [b'f1(1)\n', b'g1(1)\n'])
            ct = CallTargets.from_pattern(f'{binary}.calltargets*.json')
            self.assertEqual(len(ct.binaries), 1)
            self.assertEqual(len(ct.cmdlines), 2)
            self.assertIn('call#80.0 in main', ct.calls)
            self.assertSetEqual(ct.calls['call#80.0 in main'], {'f1', 'g1'})
            self.assertIn('call#80.1 in __fwritex', ct.calls)
            self.assertSetEqual(ct.calls['call#80.1 in __fwritex'], {'__stdout_write'})
        finally:
            code.cleanup(True)

    test_libraries = None
    test_dlopen = None


if ARCH['name'] != 'native':
    MuslLibcTestCases = unittest.skip("needs further config")(MuslLibcTestCases)
    MuslLibcStaticTestCases = unittest.skip("needs further config")(MuslLibcStaticTestCases)


del OneCallTestCaseBase
del OneCallTestCase

if __name__ == '__main__':
    unittest.main()
