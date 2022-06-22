import glob
import json
import os.path
import sys

from typegraph_utils import CallTargets


def main():
    calltargets_json = []
    tgcfi_json = None
    for arg in sys.argv[1:]:
        if os.path.isdir(arg):
            calltargets_json += list(glob.glob(arg + '/*calltargets*.json'))
        elif 'calltargets' in arg:
            calltargets_json.append(arg)
        elif arg.endswith('.json'):
            tgcfi_json = arg
    if not tgcfi_json:
        print('USAGE:', sys.argv[0], ' <tgcfi.json> <calltargets0.json>...')
        sys.exit(1)

    call_targets = CallTargets()
    for f in calltargets_json:
        call_targets.load_file(f)
    with open(tgcfi_json, 'r') as f:
        tgcfi = json.loads(f.read())
        tgcfi_functions = tgcfi['tg_targets_argnum']
        without_filter = tgcfi['tg_targets']

    call_targets.print_errors_from_dict(tgcfi_functions, without_filter)
    precision = call_targets.compute_precision_from_dict(tgcfi_functions)
    vals = [v for v in precision.values() if v.allowed_and_called + v.called_but_not_allowed > 0]
    precision_average = sum(ct.allowed_and_called + ct.allowed_but_not_called for ct in vals) / len(vals)
    print(f'[DONE] Precision = {precision_average}')



if __name__ == '__main__':
    main()
