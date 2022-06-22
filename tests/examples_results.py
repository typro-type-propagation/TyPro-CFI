import glob
import json
import os
import re
import shutil
from typing import Dict, List

import numpy

from spec_utils import raw_numbers_to_cscan_cols
from typegraph_utils import CallTargets
from utils import geomean, draw_table, invert_table, draw_table_tex

COLLECTED_GRAPHS = '../collected-graphs/examples'
COLLECTED_BINARIES = '../../typegraph-data/examples-binaries'
SAMPLE_PROGRAMS = [
    # program name / directory name / binary name
    ('lighttpd', 'lighttpd-1.4.59', 'src/lighttpd'),
    ('httpd', 'httpd-2.4.51', 'httpd'),
    ('memcached', 'memcached-1.6.10', 'memcached-debug'),
    ('nginx', 'nginx-release-1.20.0', 'objs/nginx'),
    ('redis', 'redis', 'src/redis-server'),
    ('pureftpd', 'pureftpd', 'src/pure-ftpd'),
    ('vsftpd', 'vsftpd-3.0.5', 'vsftpd')
]


def copy_results():
    os.makedirs(COLLECTED_GRAPHS, exist_ok=True)
    for progname, dirname, binary in SAMPLE_PROGRAMS:
        base = f'../examples/{progname}/{dirname}'
        target_dir = os.path.join(COLLECTED_GRAPHS, progname)
        binaryname = os.path.basename(binary)
        os.makedirs(target_dir, exist_ok=True)
        files = [
            (f'{base}-icall/{binary}.icfi.json', f'{target_dir}/{binaryname}.icfi.json'),
            (f'{base}-instrument/{binary}.ifcc.json', f'{target_dir}/{binaryname}.ifcc.json'),
            (f'{base}-instrument/{binary}.tgcfi.json', f'{target_dir}/{binaryname}.tgcfi.json')
        ]
        for f in glob.glob(f'{base}-instrument/{binary}.calltargets*.json'):
            files.append((f, f'{target_dir}/{os.path.basename(f)}'))
        for f in glob.glob(f'{base}-enforce/{binary}.dynamic-tgcfi*.json'):
            files.append((f, f'{target_dir}/{os.path.basename(f)}'))
        for src, dst in files:
            try:
                shutil.copy2(src, dst)
            except Exception as e:
                print(f'[copy error] {e}')


def copy_binaries():
    os.makedirs(COLLECTED_BINARIES, exist_ok=True)
    files = []
    for mode in ['ref', 'enforce', 'enforcestatic', 'instrument', 'icall']:
        os.makedirs(os.path.join(COLLECTED_BINARIES, mode), exist_ok=True)
        for progname, dirname, binary in SAMPLE_PROGRAMS:
            src = f'../examples/{progname}/{dirname}-{mode}/{binary}'
            target = os.path.join(COLLECTED_BINARIES, mode, os.path.basename(binary))
            files.append((src, target))
            if binary.endswith('-debug'):
                binary = binary[:-6]
                src = f'../examples/{progname}/{dirname}-{mode}/{binary}'
                target = os.path.join(COLLECTED_BINARIES, mode, os.path.basename(binary))
                files.append((src, target))
    for src, dst in files:
        try:
            shutil.copy2(src, dst)
        except Exception as e:
            print(f'[copy error] {e}')


def cleanup_function_name(funcname: str) -> str:
    if '__from__' in funcname:
        funcname = funcname.replace('-instrument', '-X').replace('-icall', '-X')
    if '.o at ' in funcname:
        funcname = re.sub(r'\.o at \d+\)', '.o)', funcname)
    return funcname


def cleanup_function_name_dict(d: Dict[str, List[str]]) -> Dict[str, List[str]]:
    return {k: [cleanup_function_name(f) for f in v] for k, v in d.items()}


def analysis_table():
    ifcc = {}
    icall = {}
    icall_generalized = {}
    tgcfi = {}
    cfguard = {}
    for progname, dirname, binary in SAMPLE_PROGRAMS:
        base = f'{COLLECTED_GRAPHS}/{progname}'
        binary = os.path.basename(binary)
        # load call targets
        call_targets = CallTargets()
        for f in glob.glob(f'{base}/{binary}.calltargets*.json'):
            call_targets.load_file(f)
        call_targets.rename_functions(cleanup_function_name)
        # load protections
        with open(f'{base}/{binary}.tgcfi.json', 'r') as f:
            tgcfi_data = cleanup_function_name_dict(json.loads(f.read())['tg_targets_argnum'])
        with open(f'{base}/{binary}.icfi.json', 'r') as f:
            data = json.loads(f.read())
            icall_data = cleanup_function_name_dict(data['icfi_targets'])
            icall_generalized_data = cleanup_function_name_dict(data['icfi_targets_generalized'])
        with open(f'{base}/{binary}.ifcc.json', 'r') as f:
            ifcc_data = cleanup_function_name_dict(json.loads(f.read())['ifcc_targets_vararg'])
        # precisions
        icall[progname] = call_targets.compute_precision_from_dict(icall_data)
        if progname == 'nginx':
            call_targets.print_errors_from_dict(icall_data)
        icall_generalized[progname] = call_targets.compute_precision_from_dict(icall_generalized_data)
        tgcfi[progname] = call_targets.compute_precision_from_dict(tgcfi_data)
        ifcc[progname] = call_targets.compute_precision_from_dict(ifcc_data)
        cfguard[progname] = call_targets.compute_precision_naive(tgcfi_data, ifcc_data, icall_data)
        # if progname == 'lighttpd': call_targets.print_errors_from_dict(tgcfi_data)
        # coverage
        reached = 0
        for _, precision in tgcfi[progname].items():
            if precision.allowed_and_called + precision.called_but_not_allowed > 0:
                reached += 1
        print(f'{reached * 100.0 / max(1, len(tgcfi[progname])):5.1f}% call coverage | {progname}')

    programs = list(sorted(tgcfi.keys()))
    table = [[''] + programs + ['avg. %']]
    reference_raw, _, _, _ = raw_numbers_to_cscan_cols(programs, icall)
    for title, data in [('Clang CFI', icall), ('Clang CFI (generalized)', icall_generalized), ('TyPro', tgcfi), ('IFCC', ifcc), ('CFGuard', cfguard)]:
        raw_avg, raw_median, formatted_avg, formatted_median = raw_numbers_to_cscan_cols(programs, data)
        try:
            p = geomean(numpy.array(raw_avg) / numpy.array(reference_raw)) - 1
            p = f'{p * 100:+5.1f}%'
        except:
            p = '-'
        table.append([title] + formatted_avg + [p])

    print('')
    textable = invert_table(table)
    textable[0] = [s.replace(' (', '\n(') for s in textable[0]]
    print(draw_table_tex(textable, [0, len(programs)]))
    print(draw_table(invert_table(table), [0, len(programs)]))


if __name__ == '__main__':
    # copy_binaries()
    # copy_results()
    analysis_table()
    ...
