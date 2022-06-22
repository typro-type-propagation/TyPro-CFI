import datetime
import getpass
import os.path
import signal
import subprocess
import sys
import time
import traceback
from typing import NamedTuple, TextIO, Optional, List, Union, Dict

BASE = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'examples')

USERNAME = getpass.getuser()
FTP_USER = os.environ.get('FTP_USER', USERNAME)
FTP_PASS = os.environ['FTP_PASS']
os.makedirs('/dev/shm/ftp', exist_ok=True)


class BenchConfig(NamedTuple):
    tool: List[str]
    n: int
    c: int = 1
    k: bool = False

    def test_tool(self, address: str) -> List[str]:
        if 'ftpbench' in self.tool[0]:
            host, port = address.split(':', 1)
            user = ['-u', 'ftp', '-p', 'ftp'] if self.k else ['-u', FTP_USER, '-p', FTP_PASS]
            return self.tool + user + ['-H', host, '-P', port, '-b', 'transfer']
        if 'memaslap' in self.tool[0]:
            return self.tool + [f'--time=1s', '-s', address]
        return self.tool + ['-n', '100', '-q'] + address.split(' ')

    def cmd(self, address: str) -> List[str]:
        if 'ftpbench' in self.tool[0]:
            host, port = address.split(':', 1)
            user = ['-u', 'ftp', '-p', 'ftp'] if self.k else ['-u', FTP_USER, '-p', FTP_PASS]
            return self.tool + user + ['-H', host, '-P', port, '-b', 'all'] + (['--vsftpd'] if self.k else [])
        if 'memaslap' in self.tool[0]:
            result = self.tool + [f'--time={self.n}s', '-c', f'{self.c}', '-s', address]
            if '-T' not in self.tool:
                result += ['-T', '2']
            return result
        # ab / redis_benchmark
        result = self.tool + ['-n', str(self.n), '-c', str(self.c), '-q']
        if 'redis-benchmark' in self.tool[0]:
            result += ['-k', 'true' if self.k else 'false']
        else:
            if self.k:
                result += ['-k']
        result += address.split(' ')
        return result


OUTPUT_FILE = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'collected-graphs', 'benchmarks', 'bench_servers.csv')

SERVERS = {
    'apache': {
        'cwd': 'httpd/httpd-2.4.51-enforce',
        'cmd': ['./httpd', '-DFOREGROUND'],
        'address': 'http://localhost:12346/',
        'test_config': BenchConfig(['ab'], 1000, 1, False)
    },
    'nginx': {
        'cwd': 'nginx/nginx-release-1.20.0-enforce/objs',
        'cmd': ['./nginx'],
        'address': 'http://localhost:8000/',
        'test_config': BenchConfig(['ab'], 1000, 1, False)
    },
    'lighttpd': {
        'cwd': 'lighttpd/lighttpd-1.4.59-enforce/src',
        'cmd': ['./lighttpd', '-f', '../../../lighttpd_sample.conf', '-D'],
        'address': 'http://localhost:9999/',
        'test_config': BenchConfig(['ab'], 1000, 1, False)
    },
    'redis': {
        'cwd': 'redis/redis-enforce/src',
        'cmd': ['./redis-server', '--port', '7777', '--loglevel', 'warning', '--save', ''],
        'address': '-h 127.0.0.1 -p 7777',
        'test_config': BenchConfig(['redis-benchmark'], 100, 1, False),
        'after_server_start': lambda: os.system('redis-cli -p 7777 FLUSHALL')
    },
    'memcached': {
        'cwd': 'memcached/memcached-1.6.10-enforce',
        'cmd': ['./memcached', '-p', '11233', '-m', '4096'],
        'address': '127.0.0.1:11233',
        'test_config': BenchConfig(['memaslap'], 1, 8, False)
    },
    'pureftpd': {
        'cwd': 'pureftpd/pureftpd-enforce/src',
        'cmd': ['./pure-ftpd', '../../../pure-ftpd.conf'],
        'address': '127.0.0.1:2121',
        'test_config': BenchConfig([BASE + '/ftpbench'], 0, 0, False),
        'root': True
    },
    'vsftpd': {
        'cwd': 'vsftpd/vsftpd-3.0.5-enforce',
        'cmd': ['./vsftpd', '../../vsftpd3.conf'],
        'address': '127.0.0.1:2121',
        'test_config': BenchConfig([BASE + '/ftpbench'], 0, 0, True),
    }
}

SERVERS_HTTP = ['apache', 'lighttpd', 'nginx']

MODES = ['enforcestatic', 'ref']

HTTP_CONFIGS = [
    # BenchConfig(['ab'], 100000, 1, False),
    # BenchConfig(['ab'], 100000, 2, False),
    # BenchConfig(['ab'], 100000, 4, False),
    # BenchConfig(['ab'], 100000, 8, False),
    # BenchConfig(['ab'], 100000, 16, False),
    # BenchConfig(['ab'], 100000, 32, False),
    # BenchConfig(['ab'], 100000, 64, False),
    # BenchConfig(['ab'], 100000, 128, False),
    # BenchConfig(['ab'], 100000, 1, True),
    # BenchConfig(['ab'], 100000, 2, True),
    # BenchConfig(['ab'], 100000, 4, True),
    # BenchConfig(['ab'], 100000, 8, True),
    # BenchConfig(['ab'], 100000, 16, True),
    # BenchConfig(['ab'], 100000, 32, True),
    # BenchConfig(['ab'], 100000, 64, True),
    # BenchConfig(['ab'], 100000, 128, True),

    # BenchConfig(['ab'], 5000000, 10, False),
    # BenchConfig(['ab'], 5000000, 10, True),
    # BenchConfig(['ab'], 10000000, 10, True),
    BenchConfig(['ab'], 1000000, 10, False),
]
REDIS_CONFIGS = [
    # BenchConfig(['redis-benchmark'], 100000, 1, False),
    # BenchConfig(['redis-benchmark'], 100000, 2, False),
    # BenchConfig(['redis-benchmark'], 100000, 4, False),
    # BenchConfig(['redis-benchmark'], 100000, 8, False),
    # BenchConfig(['redis-benchmark'], 100000, 16, False),
    # BenchConfig(['redis-benchmark'], 100000, 32, False),
    # BenchConfig(['redis-benchmark'], 100000, 64, False),
    # BenchConfig(['redis-benchmark'], 100000, 128, False),
    # BenchConfig(['redis-benchmark'], 100000, 1, True),
    # BenchConfig(['redis-benchmark'], 100000, 2, True),
    # BenchConfig(['redis-benchmark'], 100000, 4, True),
    # BenchConfig(['redis-benchmark'], 100000, 8, True),
    # BenchConfig(['redis-benchmark'], 100000, 16, True),
    # BenchConfig(['redis-benchmark'], 100000, 32, True),
    # BenchConfig(['redis-benchmark'], 100000, 64, True),
    # BenchConfig(['redis-benchmark'], 100000, 128, True),
    # BenchConfig(['redis-benchmark'], 500000, 10, True),
    BenchConfig(['redis-benchmark'], 200000, 10, False)
]
MEMCACHED_CONFIGS = [
    # BenchConfig(['memaslap'], 300, 64, False),
    BenchConfig(['memaslap'], 300, 128, False),
    # BenchConfig(['memaslap'], 600, 64, False),
]
FTP_CONFIGS = {
    'vsftpd': [BenchConfig([BASE + '/ftpbench'], 0, 0, True)],
    'pureftpd': [BenchConfig([BASE + '/ftpbench'], 0, 0, False)]
}

SERVER_RUN_PREFIX = f'sudo cset shield --exec -- sudo -u {USERNAME} -H chrt -f 99 nice -n -20'.split() + [os.path.dirname(os.path.abspath(__file__)) + '/pid-wrapper.sh']
SERVER_RUN_PREFIX_ROOT = 'sudo cset shield --exec -- chrt -f 99 nice -n -20'.split()
BENCH_RUN_PREFIX = f'sudo cset shield --exec -- sudo -u {USERNAME} -H chrt -f 99 nice -n -20'.split()


def wipe_all_caches():
    subprocess.check_call(['sudo', 'sh', '-c', 'free && sync && echo 3 > /proc/sys/vm/drop_caches && free'], stdout=subprocess.DEVNULL)


def start_benches():
    subprocess.check_call(['sudo', 'governor-set', 'f'])
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
    os.system('echo 0 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo')
    os.system('sudo cset shield -r')


def parse_result_ab(output: bytes) -> Optional[float]:
    d = {}
    for l in output.decode().split('\n'):
        if ':' in l:
            l = l.split(':', 1)
            d[l[0].strip()] = l[1].strip()
    if d['Failed requests'] != '0':
        print('ERROR: failed requests')
        print(output.decode())
        return
    return float(d['Requests per second'].split(' ')[0])


def parse_result_redis(output: bytes) -> Optional[float]:
    numbers = {}
    for l in output.decode().split('\n'):
        l = l.split('\r')[-1]
        l = l.split(', p50=')[0]
        if l.endswith('requests per second'):
            l = l.split(': ', 1)
            numbers[l[0].strip()] = float(l[1].split(' ')[0])
    return sum(numbers.values()) / len(numbers)


def parse_result_memaslap(output: bytes) -> Optional[float]:
    for l in output.decode().split('\n'):
        if l.startswith('Run time:'):
            return float(l.split('Ops: ')[1].split(' ')[0])


def parse_result_ftpbench(output: bytes) -> Optional[Union[float, Dict[str, float]]]:
    value = 1.0
    count = 0
    values = {}
    for line in output.decode().split('\n'):
        if 'QUIT' in line:
            continue
        if line.endswith('/sec'):
            v = float(line.split(' ')[-2])
            value *= v
            count += 1
            k = line.split('       ')[0].strip()
            values[k] = v
        if line.endswith(' secs'):
            v = float(line.split(' ')[-2])
            value /= v
            count += 1
            k = line.split('       ')[0].strip()
            values[k] = v
    # print('VALUES', values)
    if count != 7 and count != 6:
        return
    # return value
    values['all'] = value
    return values


def terminate_process(proc: subprocess.Popen):
    try:
        proc.send_signal(signal.SIGINT)
    except Exception as e:
        if proc.poll() is None:
            # print(f'sudo sigint to {proc.pid}')
            subprocess.check_call(['sudo', 'kill', '-s', 'SIGINT', f'-{proc.pid}'])
    time.sleep(2)
    if proc.poll() is None:
        try:
            proc.kill()
        except Exception as e:
            # print(f'sudo kill to {proc.pid}')
            subprocess.check_call(['sudo', 'kill', '-9', f'-{proc.pid}'])
        time.sleep(1)


def terminate_server():
    if os.path.exists('/dev/shm/server_pid'):
        with open('/dev/shm/server_pid', 'r') as f:
            pid = int(f.read().strip())
            subprocess.run(['kill', '-s', 'SIGINT', f'{pid}'])
            time.sleep(1)
            subprocess.run(['kill', '-s', 'SIGKILL', f'{pid}'])
            time.sleep(0.5)
        os.remove('/dev/shm/server_pid')


def run_benchmark(file: Optional[TextIO], server_name: str, mode: str, config: BenchConfig):
    os.environ['BENCH_SERVER_NAME'] = server_name
    wipe_all_caches()
    t = time.time()
    print(f'[START TEST]  server {server_name}, mode {mode}, {config}, {datetime.datetime.now()} ...')
    # start server
    server = SERVERS[server_name]
    cwd = os.path.join(BASE, server['cwd'].replace('enforce', mode))
    try:
        proc = subprocess.Popen((SERVER_RUN_PREFIX_ROOT if server.get('root', False) else SERVER_RUN_PREFIX) + server['cmd'], cwd=cwd,
                                preexec_fn=os.setsid)
    except FileNotFoundError:
        print('Server not build!', server_name, mode)
        return
    try:
        time.sleep(1)
        if 'after_server_start' in server:
            server['after_server_start']()

        # start ab
        try:
            # warmup
            subprocess.check_call(config.test_tool(server['address']), stderr=subprocess.DEVNULL, stdout=subprocess.DEVNULL)

            # check that server is still up
            if proc.poll() is not None:
                print('Server terminated prematurely')
                return

            # real run
            output = subprocess.check_output(BENCH_RUN_PREFIX + config.cmd(server['address']))
        except subprocess.CalledProcessError as e:
            print('EXECUTION FAILED:', e.cmd, e.returncode)
            print(e.output)
            return

        # collect results
        if config.tool[0] == 'ab':
            rps = parse_result_ab(output)
        elif 'redis-benchmark' in config.tool[0]:
            rps = parse_result_redis(output)
        elif 'memaslap' in config.tool[0]:
            rps = parse_result_memaslap(output)
        elif 'ftpbench' in config.tool[0]:
            rps = parse_result_ftpbench(output)
        else:
            assert False
        if rps is None:
            return
        t = time.time() - t
        print(f'[TEST RESULT] {rps} requests / second  |  time: {t:.1f} seconds  |  {server_name}/{mode}/{config.n} {config.c} {config.k}')
        if file:
            if isinstance(rps, list):
                for n, v in enumerate(rps):
                    file.write(f'{datetime.datetime.now()},{server_name},{mode},{n},{config.c},{1 if config.k else 0},{v},{t}\n')
            elif isinstance(rps, dict):
                for n, v in rps.items():
                    n = n.replace(',', '')
                    file.write(f'{datetime.datetime.now()},{server_name},{mode},{n},{config.c},{1 if config.k else 0},{v},{t}\n')
            else:
                file.write(f'{datetime.datetime.now()},{server_name},{mode},{config.n},{config.c},{1 if config.k else 0},{rps},{t}\n')
            file.flush()
        return rps

    finally:
        # stop server
        try:
            # terminate_server()
            pass
        except:
            traceback.print_exc()
        terminate_process(proc)


def main():
    REPEAT = 20

    try:
        # prepare environment
        start_benches()

        os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
        exists = os.path.exists(OUTPUT_FILE)
        with open(OUTPUT_FILE, 'a') as f:
            if not exists:
                f.write('date,server,mode,n,c,k,requests,time\n')

            # test stuff
            errors = 0
            for server in SERVERS:
                for mode in MODES:
                    result = run_benchmark(None, server, mode, SERVERS[server]['test_config'])
                    if result is None:
                        errors += 1
            if errors > 0:
                print(f'{errors} errors, exiting...')
                return

            print('=== TESTS FINISHED; RUNNING NOW ===')
            time.sleep(10)

            # run for real - HTTP
            '''
            for server in SERVERS_HTTP:
                for _ in range(REPEAT):
                    for mode in MODES:
                        for config in HTTP_CONFIGS:
                            result = run_benchmark(f, server, mode, config)
                            if result is None:
                                errors += 1
            # '''
            # run for real - Redis
            '''
            for server in ['redis']:
                for _ in range(REPEAT):
                    for mode in MODES:
                        for config in REDIS_CONFIGS:
                            result = run_benchmark(f, server, mode, config)
                            if result is None:
                                errors += 1
            # '''
            # run for real - Memcached
            '''
            for server in ['memcached']:
                for _ in range(REPEAT):
                    for mode in MODES:
                        for config in MEMCACHED_CONFIGS:
                            result = run_benchmark(f, server, mode, config)
                            if result is None:
                                errors += 1
            # '''
            # run for real - FTP
            # '''
            for server in ['pureftpd', 'vsftpd']:
                for _ in range(REPEAT):
                    for mode in MODES:
                        for config in FTP_CONFIGS[server]:
                            result = run_benchmark(f, server, mode, config)
                            if result is None:
                                errors += 1
            # '''

            print(f'[DONE] {errors} errors')
    finally:
        # reset environment
        stop_benches()


if __name__ == '__main__':
    main()
