import glob
import json
import multiprocessing
import os
import shutil
import statistics
import subprocess
import time
from functools import partial
from typing import List, Tuple, Callable, Dict

import numpy

from typegraph_utils import Typegraph, CallTargets, CallPrecision
from utils import draw_table, invert_table, geomean, draw_table_tex

TYPEGRAPH_COMPILER_DIR = '/typro/build'
if not os.path.exists(TYPEGRAPH_COMPILER_DIR):
    TYPEGRAPH_COMPILER_DIR = '../llvm-typro/cmake-build-minsizerel'
COLLECTED_GRAPHS = '../collected-graphs/SPEC2006'
SPEC_ROOT = '/spec'
if not os.path.exists(SPEC_ROOT):
    SPEC_ROOT = '../../spec2006'
SPEC_BENCH_PATH = SPEC_ROOT + '/benchspec/CPU2006'
ALL_SPEC_BENCHES = list(os.path.basename(path) for path in glob.glob(SPEC_BENCH_PATH + '/4??.*'))
ALL_SPEC_BENCHES.sort()
# SPEC_SUFFIX = 'typegraph'
SPEC_SUFFIX = 'typegraph-enforce'


def copy_instrumentation_results():
    """
    Copy results from instrumented runs to the collected-graphs directory
    :return:
    """
    num = '0000'
    os.makedirs(COLLECTED_GRAPHS, exist_ok=True)
    count = 0
    count_benches = 0
    for bench in ALL_SPEC_BENCHES:
        base = SPEC_BENCH_PATH + '/' + bench
        files = glob.glob(f'{base}/run/run_base_*_clang-64bit-o3-typegraph.{num}/*_base.clang-64bit-o3-typegraph.calltargets*.json')
        if not files: continue
        print(f'- {len(files)} json files for {bench} ...')
        target = COLLECTED_GRAPHS + '/' + bench
        os.makedirs(target, exist_ok=True)
        for f in files:
            f2 = os.path.basename(os.path.dirname(f)).split('_')[2] + '_' + os.path.basename(f)
            shutil.copy(f, os.path.join(target, f2))
            count += 1
        count_benches += 1
    print(f'Copied {count} json files for {count_benches} benchmarks')


def copy_cfi_build_results():
    num = '0000'
    os.makedirs(COLLECTED_GRAPHS, exist_ok=True)
    count = 0
    count_benches = 0
    for bench in ALL_SPEC_BENCHES:
        base = SPEC_BENCH_PATH + '/' + bench
        files = glob.glob(f'{base}/build/build_base_clang-64bit-o3-cfi-icall.{num}/icfi_icall.json')
        files += glob.glob(f'{base}/build/build_base_clang-64bit-o3-cfi-icall.{num}/ifcc.json')
        files += glob.glob(f'{base}/build/build_base_clang-64bit-o3-{SPEC_SUFFIX}.{num}/ifcc.json')
        files += glob.glob(f'{base}/build/build_base_clang-64bit-o3-{SPEC_SUFFIX}.{num}/tgcfi.json')
        if not files: continue
        target = COLLECTED_GRAPHS + '/' + bench
        os.makedirs(target, exist_ok=True)
        for f in files:
            shutil.copy(f, os.path.join(target, os.path.basename(f)))
            count += 1
        count_benches += 1
    print(f'Copied {count} json files for {count_benches} benchmarks')


def get_spec_benches() -> List[str]:
    # return list(set(os.path.basename(os.path.dirname(f)) for f in glob.glob(COLLECTED_GRAPHS + '/*/*.json') if 'perl' in f))
    # return list(set(os.path.basename(os.path.dirname(f)) for f in glob.glob(COLLECTED_GRAPHS + '/*/*.json') if 'bzip2' in f))
    return list(set(os.path.basename(os.path.dirname(f)) for f in glob.glob(COLLECTED_GRAPHS + '/*/*.json')))


def spec_fix_name(name: str) -> str:
    if name not in ALL_SPEC_BENCHES:
        for b in ALL_SPEC_BENCHES:
            if b.endswith('.'+name):
                return b
        raise Exception(f'Invalid spec benchmark name: {name}')
    return name


def instrumentation_precision_overview():
    def compute_precision(graph, call_targets: CallTargets) -> Dict[str, CallPrecision]:
        # check compatibility
        call_targets.assert_matches_graph(graph)
        return call_targets.compute_precision(graph)

    programs = []  # [(name, json pattern)]
    num = '0000'
    for bench in get_spec_benches():
        typegraph_filename = f'{SPEC_BENCH_PATH}/{bench}/build/build_base_clang-64bit-o3-{SPEC_SUFFIX}.{num}/graph.after.typegraph'
        if not os.path.exists(typegraph_filename): continue
        print(f'... loading {typegraph_filename} ...')
        graph = Typegraph.load_from_file(typegraph_filename)
        programs.append((bench, [f'{COLLECTED_GRAPHS}/{bench}/*.calltargets*.json'], partial(compute_precision, graph)))
    instrumentation_precision_overview_parameterized(programs)


def cfi_icall_precision_overview(generalized=False):
    def compute_precision(fname, call_targets: CallTargets) -> Dict[str, CallPrecision]:
        with open(fname, 'r') as f:
            d = json.loads(f.read())['icfi_targets' if not generalized else 'icfi_targets_generalized']
        return call_targets.compute_precision_from_dict(d)

    programs = []  # [(name, json pattern)]
    for bench in get_spec_benches():
        fname = f'{COLLECTED_GRAPHS}/{bench}/icfi_icall.json'
        if not os.path.exists(fname): continue
        programs.append((bench, [f'{COLLECTED_GRAPHS}/{bench}/*.calltargets*.json'], partial(compute_precision, fname)))
    instrumentation_precision_overview_parameterized(programs)


def ifcc_precision_overview(varargs=False):
    def compute_precision(fname, call_targets: CallTargets) -> Dict[str, CallPrecision]:
        with open(fname, 'r') as f:
            d = json.loads(f.read())['ifcc_targets' if not varargs else 'ifcc_targets_vararg']
        return call_targets.compute_precision_from_dict(d)

    programs = []  # [(name, json pattern)]
    for bench in get_spec_benches():
        fname = f'{COLLECTED_GRAPHS}/{bench}/ifcc.json'
        if not os.path.exists(fname): continue
        programs.append((bench, [f'{COLLECTED_GRAPHS}/{bench}/*.calltargets*.json'], partial(compute_precision, fname)))
    instrumentation_precision_overview_parameterized(programs)


def instrumentation_precision_overview_parameterized(programs: List[Tuple[str, List[str], Callable[[CallTargets], Dict[str, CallPrecision]]]]):
    """
    - Grab current typegraphs from SPEC directory
    - Grab stored instrumentation results
    - Compare + compute statistics
    :return:
    """
    # build overview
    reports = []
    total_sum = CallPrecision(0, 0, 0)
    average_okp = 0.0
    average_errp = 0.0
    for program_name, inst_patterns, compute_precision in programs:
        call_targets = CallTargets()
        for p in inst_patterns:
            if '*' in p:
                for f in glob.glob(p):
                    call_targets.load_file(f)
            else:
                call_targets.load_file(p)
        # compute
        precision = compute_precision(call_targets)
        cp = CallPrecision(0, 0, 0)
        cp_covered = CallPrecision(0, 0, 0)
        for name, prec in precision.items():
            cp += prec
            if prec.is_covered():
                cp_covered += prec
        total_sum += cp_covered
        # report
        total = cp_covered.sum()
        #okp = cp_covered.allowed_and_called * 100.0 / total
        #errp = cp_covered.called_but_not_allowed * 100.0 / total
        okp = cp_covered.allowed_and_called * 100.0 / (cp_covered.allowed_and_called + cp_covered.allowed_but_not_called)
        errp = cp_covered.called_but_not_allowed * 100.0 / (cp_covered.called_but_not_allowed + cp_covered.allowed_and_called)
        average_okp += okp
        average_errp += errp
        reports.append(
            f'{program_name:15s}  ok={cp_covered.allowed_and_called:5d}    over={cp_covered.allowed_but_not_called:5d}    err={cp_covered.called_but_not_allowed:5d}    ok%={okp:6.2f}%    err%={errp:6.2f}%')
    # report all
    print('\n'.join(reports))
    print('-' * 125)
    average_okp /= len(reports)
    average_errp /= len(reports)
    total = total_sum.sum()
    okp = total_sum.allowed_and_called * 100.0 / total
    errp = total_sum.called_but_not_allowed * 100.0 / total
    print(
        f'{"(all)":15s}  ok={total_sum.allowed_and_called:5d}    over={total_sum.allowed_but_not_called:5d}    err={total_sum.called_but_not_allowed:5d}    ok%={okp:6.2f}%    err%={errp:6.2f}%    avg.ok%={average_okp:6.2f}%    avg.err%={average_errp:6.2f}%')
    # print(f'{"(average)":30s}                                            ok%={average_okp:6.2f}%     err%={average_errp:6.2f}%')


def recompile_typegraph(custom_data=False):
    if not custom_data:
        os.environ['TG_SOUFFLE_NO_CUSTOM_DATA'] = '1'
    ts = time.time()
    subprocess.check_call(['nice', '-10', 'make', '-j8', 'clang', 'lld', 'llvm-typegraph', 'typro-instrumentation', 'typro-rt'], cwd=TYPEGRAPH_COMPILER_DIR)
    ts = time.time() - ts
    print(f'Rebuiling LLVM/Clang took {ts:.1f} seconds')

def clear_compiler_cache():
    shutil.rmtree('/dev/shm/compilercache', ignore_errors=True)


def recompile_spec_benchmark(bench: str, threads: int = 1, kind=SPEC_SUFFIX, output_facts=False):
    bench = spec_fix_name(bench)
    num = '0000'
    directory = os.path.abspath(f'{SPEC_BENCH_PATH}/{bench}/build/build_base_clang-64bit-o3-{kind}.{num}/')
    cmd = f'cd "{os.path.abspath(SPEC_ROOT)}" && . ./shrc && cd {directory} && make clean && '
    if output_facts:
        output_facts_dir = os.path.abspath(os.path.dirname(COLLECTED_GRAPHS)) + '/facts-'+bench.split('.')[-1]
        cmd += f'TG_FACTS_OUTPUT={output_facts_dir} '
    cmd += f'exec make -j{threads}'
    cmd += ' 1> >(tee remake.stdout.txt ) 2> >(tee remake.stderr.txt >&2 )'
    args = ['nice', '-10', 'bash', '-c', cmd]
    print('RUNNING:', args)
    ts = time.time()
    subprocess.check_call(args)
    ts = time.time() - ts
    print(f'Compiling {bench} took {ts:.1f} seconds')


def recompile_all_spec_benchmarks(**kwargs):
    clear_compiler_cache()
    t = time.time()
    for bench in get_spec_benches():
        tt = time.time() - t
        if tt > 1:
            print(f'Recompilation took {tt:.1f} seconds so far.')
        print(f' ===== Recompiling {bench} =====')
        recompile_spec_benchmark(bench, multiprocessing.cpu_count(), **kwargs)
    t = time.time() - t
    print(f'Recompilation took {t:.1f} seconds')





def instrumentation_precision_overview_raw_numbers(use_ifcc_cut=False):
    def compute_precision(graph, call_targets: CallTargets, fname: str) -> Dict[str, CallPrecision]:
        # check compatibility
        call_targets.assert_matches_graph(graph)
        if use_ifcc_cut:
            with open(fname, 'r') as f:
                d = json.loads(f.read())['ifcc_targets_vararg']
            return call_targets.compute_precision_with_dict(graph, d)
        return call_targets.compute_precision(graph)

    num = '0000'
    precision = {}
    for bench in get_spec_benches():
        typegraph_filename = f'{SPEC_BENCH_PATH}/{bench}/build/build_base_clang-64bit-o3-{SPEC_SUFFIX}.{num}/graph.after.typegraph'
        if not os.path.exists(typegraph_filename): continue
        print(f'... loading {typegraph_filename} ...')
        graph = Typegraph.load_from_file(typegraph_filename)
        call_targets = CallTargets()
        for p in [f'{COLLECTED_GRAPHS}/{bench}/*.calltargets*.json']:
            if '*' in p:
                for f in glob.glob(p):
                    call_targets.load_file(f)
            else:
                call_targets.load_file(p)
        # compute
        fname = f'{COLLECTED_GRAPHS}/{bench}/ifcc.json'
        precision[bench] = compute_precision(graph, call_targets, fname)
    return precision


def tgcfi_precision_overview_raw_numbers(with_argnum=False) -> Dict[str, Dict[str, CallPrecision]]:
    def compute_precision(fname, call_targets: CallTargets, with_argnum) -> Dict[str, CallPrecision]:
        with open(fname, 'r') as f:
            d = json.loads(f.read())['tg_targets' if not with_argnum else 'tg_targets_argnum']
        print(f'=== TGCFI ERRORS {fname} ===')
        call_targets.print_errors_from_dict(d)
        print('=== /TGCFI ERRORS ===')
        return call_targets.compute_precision_from_dict(d)

    precision = {}
    for bench in get_spec_benches():
        fname = f'{COLLECTED_GRAPHS}/{bench}/tgcfi.json'
        if not os.path.exists(fname): continue
        call_targets = CallTargets()
        for p in [f'{COLLECTED_GRAPHS}/{bench}/*.calltargets*.json']:
            if '*' in p:
                for f in glob.glob(p):
                    call_targets.load_file(f)
            else:
                call_targets.load_file(p)
        # compute
        precision[bench] = compute_precision(fname, call_targets, with_argnum)
    return precision


def cfi_icall_precision_overview_raw_numbers(generalized=False) -> Dict[str, Dict[str, CallPrecision]]:
    def compute_precision(fname, call_targets: CallTargets) -> Dict[str, CallPrecision]:
        with open(fname, 'r') as f:
            d = json.loads(f.read())['icfi_targets' if not generalized else 'icfi_targets_generalized']
        #print('=== CFI ERRORS ===')
        #call_targets.print_errors_from_dict(d)
        #print('=== /CFI ERRORS ===')
        return call_targets.compute_precision_from_dict(d)

    precision = {}
    for bench in get_spec_benches():
        fname = f'{COLLECTED_GRAPHS}/{bench}/icfi_icall.json'
        if not os.path.exists(fname): continue
        call_targets = CallTargets()
        for p in [f'{COLLECTED_GRAPHS}/{bench}/*.calltargets*.json']:
            if '*' in p:
                for f in glob.glob(p):
                    call_targets.load_file(f)
            else:
                call_targets.load_file(p)
        # compute
        precision[bench] = compute_precision(fname, call_targets)
    return precision


def ifcc_precision_overview_raw_numbers(varargs=False):
    def compute_precision(fname, call_targets: CallTargets) -> Dict[str, CallPrecision]:
        with open(fname, 'r') as f:
            d = json.loads(f.read())['ifcc_targets' if not varargs else 'ifcc_targets_vararg']
        #print('=== IFCC ERRORS ===')
        #call_targets.print_errors_from_dict(d)
        #print('=== /IFCC ERRORS ===')
        return call_targets.compute_precision_from_dict(d)

    precision = {}
    for bench in get_spec_benches():
        fname = f'{COLLECTED_GRAPHS}/{bench}/ifcc.json'
        if not os.path.exists(fname): continue
        call_targets = CallTargets()
        for p in [f'{COLLECTED_GRAPHS}/{bench}/*.calltargets*.json']:
            if '*' in p:
                for f in glob.glob(p):
                    call_targets.load_file(f)
            else:
                call_targets.load_file(p)
        # compute
        precision[bench] = compute_precision(fname, call_targets)
    return precision


def raw_numbers_to_cscan_cols(programs: List[str], data: Dict[str, Dict[str, CallPrecision]]):
    raw_avg = []
    raw_median = []
    formatted_avg = []
    formatted_median = []
    for program in programs:
        if program in data:
            errors = 0
            targets = []
            for call, precision in data[program].items():
                if precision.allowed_and_called > 0:
                    targets.append(precision.allowed_and_called + precision.allowed_but_not_called)
                if precision.called_but_not_allowed > 0:
                    errors += 1
            if targets:
                a = sum(targets) / len(targets)
                m = statistics.median(targets)
                raw_avg.append(a)
                raw_median.append(m)
                formatted_avg.append(f'{a:6.2f}' + (' (!)' if errors else ''))
                formatted_median.append(f'{m:.1f}')
                continue
        # no data available
        raw_avg.append(None)
        raw_median.append(None)
        formatted_avg.append('  -')
        formatted_median.append('-')
    return (raw_avg, raw_median, formatted_avg, formatted_median)





def print_cscan_like_table():
    programs = list(sorted(cfi_icall_precision_overview_raw_numbers().keys()))
    table = [[''] + programs + ['avg. %']]

    raw_avg, raw_median, formatted_avg, formatted_median = raw_numbers_to_cscan_cols(programs, cfi_icall_precision_overview_raw_numbers())
    base_avg = raw_avg
    p = '(base)'
    table.append(['Clang CFI'] + formatted_avg + [p])
    #table.append(['(median)'] + formatted_median)

    raw_avg, raw_median, formatted_avg, formatted_median = raw_numbers_to_cscan_cols(programs, cfi_icall_precision_overview_raw_numbers(True))
    try:
        p = geomean(numpy.array(raw_avg) / numpy.array(base_avg)) - 1
        p = f'{p * 100:+5.1f}%'
    except:
        p = '-'
    table.append(['Clang CFI (generalized)'] + formatted_avg + [p])

    #raw_avg, raw_median, formatted_avg, formatted_median = raw_numbers_to_cscan_cols(programs, instrumentation_precision_overview_raw_numbers(True))
    raw_avg, raw_median, formatted_avg, formatted_median = raw_numbers_to_cscan_cols(programs, tgcfi_precision_overview_raw_numbers(False))
    try:
        p = geomean(numpy.array(raw_avg) / numpy.array(base_avg)) - 1
        p = f'{p*100:+5.1f}%'
    except:
        p = '-'
    table.append(['= TG ='] + formatted_avg + [p])
    # table.append(['(median)'] + formatted_median)

    raw_avg, raw_median, formatted_avg, formatted_median = raw_numbers_to_cscan_cols(programs, tgcfi_precision_overview_raw_numbers(True))
    try:
        p = geomean(numpy.array(raw_avg) / numpy.array(base_avg)) - 1
        p = f'{p * 100:+5.1f}%'
    except:
        p = '-'
    table.append(['+ argnum'] + formatted_avg + [p])
    # table.append(['(median)'] + formatted_median)

    raw_avg, raw_median, formatted_avg, formatted_median = raw_numbers_to_cscan_cols(programs, ifcc_precision_overview_raw_numbers())
    try:
        p = geomean(numpy.array(raw_avg) / numpy.array(base_avg)) - 1
        p = f'{p * 100:+5.1f}%'
    except:
        p = '-'
    table.append(['IFCC'] + formatted_avg + [p])
    #table.append(['(median)'] + formatted_median)

    clang_paper_raw = [22.03, 1, 8.91, 2, 600.84, 10, 7, 2.06, 5]
    clang_paper_err = [False, False, True, False, False, False, False, False, False]
    try:
        p = geomean(numpy.array(clang_paper_raw) / numpy.array(base_avg))-1
        p = f'{p * 100:+5.1f}%'
    except:
        p = '-'
    table.append(
        ['Clang (cscan paper)'] + [f'{a:6.2f}' + (' (x)' if errors else '') for a, errors in zip(clang_paper_raw, clang_paper_err)] + [p])
    # table.append(['Clang Paper', ' 22.03', '  1', '  8.91', '  2', '600.84', ' 10', '  7', '  2.06', '  5'])

    mcfi_paper_raw = [23.27, 1, 32.63, 2, 605.51, 10, 7, 2.06, 5]
    mcfi_paper_err = [True, False, True, False, False, False, False, False, False]
    try:
        p = geomean(numpy.array(mcfi_paper_raw) / numpy.array(base_avg)) - 1
        p = f'{p * 100:+5.1f}%'
    except:
        p = '-'
    table.append(['MCFI (cscan paper)'] + [f'{a:6.2f}' + (' (x)' if errors else '') for a, errors in zip(mcfi_paper_raw, mcfi_paper_err)] + [p])

    print(draw_table(invert_table(table), [0, len(programs)]))
    print(draw_table_tex(invert_table(table), [0, len(programs)]))



if __name__ == '__main__':
    ts = time.time()
    try:
        recompile_typegraph()
        clear_compiler_cache()
        recompile_all_spec_benchmarks()
        #recompile_spec_benchmark('bzip2', 8, output_facts=True)
        #recompile_spec_benchmark('gcc', 2, output_facts=True)
        #recompile_spec_benchmark('gobmk', 8, output_facts=True)
        #recompile_spec_benchmark('h264ref', 8, output_facts=True)
        #recompile_spec_benchmark('hmmer', 8, output_facts=True)
        #recompile_spec_benchmark('milc', 8, output_facts=True)
        # os.environ['TG_CLANG_MINIMIZE_GRAPH'] = '0'
        #recompile_spec_benchmark('perlbench', 8, output_facts=True)
        #recompile_spec_benchmark('sjeng', 8, output_facts=True)
        #recompile_spec_benchmark('sphinx3', 8, output_facts=True)
        #instrumentation_precision_overview()
        #recompile_spec_benchmark('hmmer', 8, 'cfi-icall')
        #recompile_all_spec_benchmarks(kind='cfi-icall')
        # copy_instrumentation_results()
        copy_cfi_build_results()
        # instrumentation_precision_overview()
        #cfi_icall_precision_overview()
        #cfi_icall_precision_overview(generalized=True)
        #ifcc_precision_overview()
        #ifcc_precision_overview(varargs=True)
        print_cscan_like_table()
    finally:
        ts = time.time() - ts
        os.system(f"notify-send 'SPEC finished' 'SPEC has finished computing. Took {ts:.1f} seconds.' 2>/dev/null")
    pass
