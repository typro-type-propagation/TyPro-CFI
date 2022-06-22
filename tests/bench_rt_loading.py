import datetime
import getpass
import os.path
import signal
import subprocess
import sys
import time
from typing import NamedTuple, TextIO, Optional, List, Union, Dict

BASE_EXAMPLES = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'examples')
BASE_SPEC = os.path.join(os.getenv('HOME'), 'Projekte', 'spec2006', 'benchspec', 'CPU2006')

OUTPUT_FILE = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'collected-graphs', 'benchmarks', 'bench_rt.csv')
USE_CSET = True
USERNAME = getpass.getuser()


class BenchConfig(NamedTuple):
    binary: str
    graph_limit: int
    arguments: List[str] = []

    def name(self) -> str:
        return os.path.basename(self.binary).replace('_livepretend', '')


BENCH_CONFIGS = [
    BenchConfig(BASE_SPEC + '/400.perlbench/build/build_base_clang-64bit-o3-typegraph-enforce-musl-dynamic.0000/perlbench', 2),
    BenchConfig(BASE_SPEC + '/401.bzip2/build/build_base_clang-64bit-o3-typegraph-enforce-musl-dynamic.0000/bzip2', 2),
    BenchConfig(BASE_SPEC + '/403.gcc/build/build_base_clang-64bit-o3-typegraph-enforce-musl-dynamic.0000/gcc', 2),
    BenchConfig(BASE_SPEC + '/433.milc/build/build_base_clang-64bit-o3-typegraph-enforce-musl-dynamic.0000/milc', 2),
    BenchConfig(BASE_SPEC + '/445.gobmk/build/build_base_clang-64bit-o3-typegraph-enforce-musl-dynamic.0000/gobmk', 2),
    BenchConfig(BASE_SPEC + '/456.hmmer/build/build_base_clang-64bit-o3-typegraph-enforce-musl-dynamic.0000/hmmer', 2),
    BenchConfig(BASE_SPEC + '/458.sjeng/build/build_base_clang-64bit-o3-typegraph-enforce-musl-dynamic.0000/sjeng', 2),
    BenchConfig(BASE_SPEC + '/464.h264ref/build/build_base_clang-64bit-o3-typegraph-enforce-musl-dynamic.0000/h264ref', 2),
    BenchConfig(BASE_SPEC + '/482.sphinx3/build/build_base_clang-64bit-o3-typegraph-enforce-musl-dynamic.0000/sphinx_livepretend', 2),

    BenchConfig(BASE_EXAMPLES + '/pureftpd/pureftpd-musl_enforce/src/pure-ftpd', 2),
    BenchConfig(BASE_EXAMPLES + '/memcached/memcached-1.6.10-musl_enforce/memcached', 2),
    #BenchConfig(BASE_EXAMPLES + '/nginx/nginx-release-1.20.0-musl_enforce/objs/nginx', 2),
    BenchConfig(BASE_EXAMPLES + '/redis/redis-musl_enforce/src/redis-server', 2),
]


SERVER_RUN_PREFIX = f'sudo cset shield --exec -- sudo -u {USERNAME} -H chrt -f 99 nice -n -20'.split()
SERVER_RUN_PREFIX_ROOT = 'sudo cset shield --exec -- chrt -f 99 nice -n -20'.split()
BENCH_RUN_PREFIX = f'sudo --preserve-env=TYPRO_RT_MAX_GRAPHS cset shield --exec -- sudo -u {USERNAME} --preserve-env=TYPRO_RT_MAX_GRAPHS -H chrt -f 99 nice -n -20'.split()
BENCH_RUN_PREFIX_WITHOUT_CSET = 'chrt -f 99 nice -n -20'.split()


def wipe_all_caches():
    subprocess.check_call(['sudo', 'sh', '-c', 'free && sync && echo 3 > /proc/sys/vm/drop_caches && free'], stdout=subprocess.DEVNULL)


def start_benches():
    subprocess.check_call(['sudo', 'governor-set', 'f'])
    if USE_CSET:
        os.system("echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo")
        subprocess.check_call(['sudo', 'free'])
        subprocess.check_call(['sudo', 'swapoff', '-a'])
        subprocess.check_call(['sudo', 'cset', 'shield', '-c', '2,3,4,5', '-k', 'on'])
    print('-' * 80)
    print('\n')
    sys.stderr.flush()
    sys.stdout.flush()


def stop_benches():
    print('\n')
    print('-' * 80)
    sys.stderr.flush()
    sys.stdout.flush()
    os.system('sudo governor-set p')
    if USE_CSET:
        os.system('echo 0 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo')
        os.system('sudo cset shield -r')


def parse_results(output: bytes) -> Dict[str, float]:
    # print(output)
    d = {}
    for l in output.decode().split('\n'):
        if l.startswith('[RT BENCH]'):
            value, text = l[10:].split('s ')
            d[text.strip()] = float(value.strip())
    return d


def run_benchmark(file: Optional[TextIO], config: BenchConfig, mode: str):
    wipe_all_caches()
    t = time.time()
    try:
        env = dict(os.environ.items())
        if config.graph_limit:
            env['TYPRO_RT_MAX_GRAPHS'] = str(config.graph_limit)
        # print(config.binary, ' ...')
        output = subprocess.check_output((BENCH_RUN_PREFIX if USE_CSET else BENCH_RUN_PREFIX_WITHOUT_CSET) + [config.binary] + config.arguments, stderr=subprocess.STDOUT, env=env, timeout=45)
    except subprocess.CalledProcessError as e:
        print('EXECUTION FAILED:', e.cmd, e.returncode)
        print(e.output)
        return
    except subprocess.TimeoutExpired:
        print('TIMEOUT')
        return
    data = parse_results(output)
    if len(data) == 0:
        return
    t = time.time() - t
    print(f'[RESULT] {mode} | {config.name():12s} | {data}')

    if file:
        file.write(f'{datetime.datetime.now()},{config.name()},{mode},{data["loading"]:.5f},{data["computation"]:.5f},{data["dispatchers"]:.5f},{data["total"]:.5f},{t}\n')
        file.flush()
    return data


def main():
    REPEAT = 100
    REPEAT_REDIS = 10

    try:
        # prepare environment
        start_benches()

        os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
        exists = os.path.exists(OUTPUT_FILE)
        with open(OUTPUT_FILE, 'a') as f:
            if not exists:
                f.write('date,binary,mode,loading,computation,dispatchers,total,time\n')

            # test stuff
            errors = 0
            for config in BENCH_CONFIGS:
                result = run_benchmark(None, config, 'spec musl')
                if result is None:
                    errors += 1
            if errors > 0:
                print(f'{errors} errors, exiting...')
                return

            print('=== TESTS FINISHED; RUNNING NOW ===')
            time.sleep(10)

            # run for real
            # '''
            for config in BENCH_CONFIGS:
                for _ in range(REPEAT_REDIS if config.name() == 'redis-server' else REPEAT):
                    result = run_benchmark(f, config, 'spec musl')
                    if result is None:
                        errors += 1
            # '''

            print(f'[DONE] {errors} errors')
    finally:
        # reset environment
        stop_benches()


if __name__ == '__main__':
    main()
