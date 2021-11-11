# coding=utf-8
# Above line is for python
#
# RmMinusRF(name, bPrint, onlyTest)
#
# 2017-01-21 Forgot to add errprint
# 2016-10-19 Changed RmMinusRF to using shutil.rmtree()
# 2016-06-10 Made into library
#

import os
import shutil
import stat
import sys


def errprint(*args):
	# Used to print exception messages
	strings = list(map(str, args))
	msg = ' '.join(strings)
	if msg[-1:] != '\n':
		msg += '\n'
	sys.stderr.write(msg)


def del_rw(func, path, exc_info):
	os.chmod(path, stat.S_IWUSR)
	func(path)


# Kill an entire directory
def RmMinusRF(name, bPrint = True, onlyTest = False):
	if bPrint:
		print('rd /s', name)
	if onlyTest:
		return
	if os.access(name, os.F_OK):
		mode = os.stat(name)[stat.ST_MODE]
		if stat.S_ISDIR(mode):
			shutil.rmtree(name, onerror=del_rw)
		else:
			if not os.access(name, os.W_OK):
				os.chmod(name, 0o666)
			if os.access(name, os.W_OK):
				try:
					os.remove(name)
				except OSError as msg:
					errprint('os.remove:', msg)


if __name__ == '__main__':
	sys.exit(0)
