#!/usr/bin/python3
######
 # ЛИЦЕНЗИЯ: эта программа лицензирована также, как и ``forschar.h``.
 #
 # Автор --- Menshikov Sviatoslav Sergeevich, 2024 год.
######

import sys
from xmconcc import *
from os import system

if len(sys.argv) == 1:
	print('usage:  ./compiler.py INCLUDE_PATH IN_FILE OUT_FILE', file=sys.stderr)
	exit(1)

f = open(sys.argv[2], 'r')
tree = maketree(preprocess(sys.argv[1], f.read()))
f.close()

labels = {}
strings = {}

# определение меток и строк
i = 0
for obj in tree:
	if obj[0] == 'reset_stack_pointer':
		i += 1
	elif obj[0] == 'push_number' or obj[0] == 'push_string_addr' or obj[0] == 'push_label':
		i += 2
	elif obj[0] == 'string':
		strings[obj[1][0]] = i
		i += len(obj[1][1]) + 1
	elif obj[0] == 'call':
		i += len(obj[1][0]) + 2
	elif obj[0] == 'label':
		labels[obj[1][0]] = i

result = []
# компиляция
for obj in tree:
	if obj[0] == 'reset_stack_pointer':
		result += ['RESET_STACK_PTR']
	elif obj[0] == 'push_number':
		result += ['PUSH_NUM', obj[1][0]]
	elif obj[0] == 'push_string_addr':
		result += ['PUSH_NUM', strings[obj[1][0]]]
	elif obj[0] == 'string':
		for c in obj[1][1]:
			result += [ord(c)]
		result += [0]
	elif obj[0] == 'push_label':
		result += ['PUSH_NUM', labels[obj[1][0]]]
	elif obj[0] == 'asm':
		print('warning: asm is not supported', file=sys.stderr)
	elif obj[0] == 'call':
		result += ['CALL']
		for c in obj[1][0]:
			result += [ord(c)]
		result += [0]

f = open('program_code.h', 'w')
f.write('#define PROGRAM_CODE {');
for obj in result:
	f.write(str(obj) + ', ')
f.write('}\n')
f.close()

system('gcc -o \'' + sys.argv[3].replace('\'', '\\\'') + '\' vm.c -lm -pthread')
system('rm program_code.h')
