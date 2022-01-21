#!/usr/bin/env python3
import os
import subprocess
import sys


def get_souffle_version():
	line = subprocess.check_output(['souffle', '--version']).decode().split('\n')[0]
	version = line.split(': ')[1].replace('-', '.').replace('(64bit Domains)', '').split('.')
	while len(version) < 4:
		version.append('0')
	if len(version[3]) > 4: version[3] = '0'
	return tuple(int(x) for x in version[:4])


def main(fname: str):
	print('Patching disabled for now ...')
	patching = False
	if get_souffle_version() < (2, 0, 2, 1188):
		print('Souffle version too old, not patching')
		patching = False

	with open(fname, 'r') as f:
		content = f.read()
	# content = content.replace('mk<t_brie_ii__1_0__0_1__11__10__01>()', 'mk<CastRelationDataUsed>()')
	content = content.replace('struct t_brie_ii__1_0__0_1__11__10__01 {',
							  '#include "llvm/Typegraph/souffle-datastructures.h"\n\nstruct t_brie_ii__1_0__0_1__11__10__01 {')
	if patching and 'TG_SOUFFLE_NO_CUSTOM_DATA' not in os.environ:
		content = content.replace('<t_brie_ii__1_0__0_1__11__10__01>', '<CastRelationDataUsed>')
		content = content.replace(',t_brie_ii__1_0__0_1__11__10__01,', ',CastRelationDataUsed,')
		content = content.replace('<t_brie_i__0__1>', '<CastRelationDataUsed1>')
		content = content.replace(',t_brie_i__0__1,', ',CastRelationDataUsed1,')
	with open(fname, 'w') as f:
		f.write(content)
	print(f'Patched "{fname}"')


if __name__ == '__main__':
	main(sys.argv[1])
