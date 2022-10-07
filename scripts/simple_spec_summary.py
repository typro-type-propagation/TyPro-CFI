#!/usr/bin/env python3
import glob
import os.path
import re
import statistics
import sys
from collections import defaultdict
from typing import List, Dict


"""
USAGE:
./simple_spec_summary.py   # all files in /spec/result/
./simple_spec_summary.py 1 10   # result 1-10 from /spec/result/
./simple_Spec_summary.py <list> <of> <csv> <files>
"""


def draw_table(table: List[List[str]], hline_after=()):
    column_width = defaultdict(lambda: 0)
    for row in table:
        for i, col in enumerate(row):
            column_width[i] = max(column_width[i], len(col))
    txt = []
    for i, row in enumerate(table):
        for j, col in enumerate(row):
            txt.append(col + ' ' * (column_width[j] - len(col)))
            if j < len(row) - 1:
                txt.append(' | ')
        txt.append('\n')
        if i in hline_after:
            # txt.append('-' * (sum(column_width.values()) + 3 * len(row) - 3) + '\n')
            txt.append('-|-'.join('-' * v for k, v in sorted(column_width.items(), key=lambda x: x[0])) + '\n')
    return ''.join(txt)


def load_spec_files(files: List[str]) -> Dict[str, Dict[str, List[float]]]:
    """
    :param files:
    :return: {benchmark type: {benchmark name: [list, of, results]}}
    """
    results = {}
    for fname in files:
        if not os.path.exists(fname):
            print('MISSING FILE', fname)
            continue
        with open(fname, 'r') as f:
            text = f.read()
            name = [l[12:-1] for l in text.split('\n') if l.startswith('"test name: ')][0]
            if name == 'llvm-o3-typegraph':
                name = 'llvm-o3-typro'
            if name not in results:
                results[name] = {}
            table = text.split('"Selected Results Table"')[1].split('"Run number:"')[0]
            for l in table.split('\n'):
                if l.startswith('4'):
                    elements = l.split(',')
                    if elements[2]:
                        bench_name = elements[0]
                        if re.match(r'\d{3}\.\w+', bench_name):
                            bench_name = bench_name.split('.', 1)[1]
                        if bench_name not in results[name]:
                            results[name][bench_name] = []
                        results[name][bench_name].append(float(elements[2]))
    return results


def summarize_spec_files(files: List[str]):
    results = load_spec_files(files)
    assert 'llvm-o3-typro' in results, 'No typro runs!'
    assert 'llvm-o3-ref' in results, 'No reference runs!'
    benchmarks = list(sorted(results['llvm-o3-typro']))
    table = [['Benchmark', 'Typro runtime (stdev)', 'Ref runtime (stdev)', 'Overhead']]
    for bench in benchmarks:
        runtime_typro = sum(results['llvm-o3-typro'][bench]) / len(results['llvm-o3-typro'][bench])
        runtime_ref = sum(results['llvm-o3-ref'][bench]) / len(results['llvm-o3-ref'][bench])
        stdev_typro = statistics.stdev(results['llvm-o3-typro'][bench]) / runtime_typro
        stdev_ref = statistics.stdev(results['llvm-o3-ref'][bench]) / runtime_ref
        overhead = runtime_typro / runtime_ref - 1
        table.append([
            bench,
            f'{runtime_typro:5.1f} s     (+-{stdev_typro*100:4.1f}%)',
            f'{runtime_ref:5.1f} s   (+-{stdev_ref*100:4.1f}%)',
            f'{overhead * 100:5.2f}%'])
    print(draw_table(table, (0,)))


if __name__ == '__main__':
    if len(sys.argv) == 3 and re.match(r'\d+', sys.argv[1]) and re.match(r'\d+', sys.argv[2]):
        files = []
        for i in range(int(sys.argv[1]), int(sys.argv[2]) + 1):
            files.append(f'/spec/result/CINT2006.{i:03d}.ref.csv')
            files.append(f'/spec/result/CFP2006.{i:03d}.ref.csv')
        summarize_spec_files(files)
    elif len(sys.argv) > 1:
        summarize_spec_files(sys.argv[1:])
    else:
        summarize_spec_files(glob.glob('/spec/result/*.ref.csv'))
