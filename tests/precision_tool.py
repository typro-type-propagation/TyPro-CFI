import glob
import os.path
import pathlib
import sys
import json
import re
from typing import List, Dict

from typegraph_utils import CallTargets, CallPrecision
from utils import draw_table, geomean


binary_names = [
    'bzip2', 'gcc', 'perlbench', 'sphinx_livepretend', 'h264ref', 'milc', 'sjeng', 'hmmer', 'gobmk',
    'lighttpd', 'memcached-debug', 'redis', 'httpd', 'nginx', 'vsftpd', 'pure-ftpd', 'redis-server'
]


def find_jsons(folder: str) -> List[str]:
    return [str(path) for path in pathlib.Path(folder).rglob('*.json') if 'musl' not in str(path) and 'arm64' not in str(path) and '-os-noic' not in str(path)]


def cleanup_function_name(funcname: str) -> str:
    if '__from__' in funcname:
        funcname = funcname.replace('-instrument', '-X').replace('-icall', '-X')
    if '.o at ' in funcname:
        funcname = re.sub(r'\.o at \d+\)', '.o)', funcname)
    return funcname


def cleanup_function_name_dict(d: Dict[str, List[str]]) -> Dict[str, List[str]]:
    return {k: [cleanup_function_name(f) for f in v] for k, v in d.items()}


def read_folder(folder: str) -> Dict[str, Dict[str, CallPrecision]]:
    call_targets = CallTargets()
    schemes = {}
    for jsonfile in find_jsons(folder):
        jsonfile_base = os.path.basename(jsonfile)
        if jsonfile_base == 'tgcfi.json' or (jsonfile_base.endswith('.tgcfi.json') and jsonfile_base[:-11] in binary_names):
            with open(jsonfile, 'r') as f:
                schemes['TyPro'] = json.loads(f.read())['tg_targets_argnum']
        elif jsonfile_base == 'ifcc.json' or (jsonfile_base.endswith('.ifcc.json') and jsonfile_base[:-10] in binary_names):
            with open(jsonfile, 'r') as f:
                schemes['IFCC'] = json.loads(f.read())['ifcc_targets_vararg']
        elif jsonfile_base == 'icfi_icall.json' or (jsonfile_base.endswith('.icfi.json') and jsonfile_base[:-10] in binary_names):
            with open(jsonfile, 'r') as f:
                d = json.loads(f.read())
                schemes['Clang CFI'] = d['icfi_targets']
                schemes['Clang (generalized)'] = d['icfi_targets_generalized']
        elif 'calltargets' in jsonfile_base:
            progname = jsonfile_base.split('.')[0]
            # get "raw" filename for SPEC benches
            if progname.endswith('_base'):
                progname = progname[:-5]
            # ignore irrelevant binaries (unittest executables, server clients, ...)
            if progname in binary_names:
                # special rule: redis-cli saves its results as redis-server...
                if 'redis' in progname:
                    with open(jsonfile, 'r') as f:
                        d = json.loads(f.read())
                        if 'redis-server' not in d['binary']:
                            continue
                call_targets.load_file(jsonfile)
    call_targets.rename_functions(cleanup_function_name)
    return {k: call_targets.compute_precision_from_dict(cleanup_function_name_dict(v)) for k, v in schemes.items()}


def main():
    schemes = ['Clang CFI', 'Clang (generalized)', 'TyPro', 'IFCC']
    table = []
    summary = {scheme: [] for scheme in schemes}

    for folder in sys.argv[1:]:
        if not os.path.exists(folder) or not os.path.isdir(folder):
            continue
        precisions = read_folder(folder)
        if any(scheme not in precisions for scheme in schemes):
            print('not all scheme files found:', folder)
            continue
        all_targets = sum(p.allowed_and_called + p.called_but_not_allowed for scheme in schemes for p in precisions[scheme].values())
        if all_targets == 0:
            print('call targets not found:', folder)
            continue
        title = os.path.basename(folder)
        row = [title]
        for scheme in schemes:
            has_error = any(prec.called_but_not_allowed > 0 for call, prec in precisions[scheme].items())
            targets = [prec.allowed_and_called + prec.allowed_but_not_called for call, prec in precisions[scheme].items() if prec.allowed_and_called > 0]
            avg_targets = sum(targets) / len(targets)
            row.append(f'{avg_targets:7.2f}{" [X]" if has_error else ""}')
            summary[scheme].append(avg_targets)
        table.append(row)

    summary_row = ['(average)']
    for scheme in schemes:
        #summary_row.append(f'{sum(summary[scheme])/len(summary[scheme]):7.2f}')
        p = geomean([a/b for a, b in zip(summary[scheme], summary['Clang CFI'])])
        summary_row.append(f'+{p*100-100:4.1f}%')

    table.sort()
    table = [['Benchmark'] + schemes] + table + [summary_row]
    print('')
    print(draw_table(table, (0, len(table)-2)))


if __name__ == '__main__':
    main()
