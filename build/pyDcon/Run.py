# coding=utf-8
# Above line is for python
#
# Run(cmd, onlyTest = False)
#  Run the cmd
#
# 2016-06-10 Made into library
#

import os
import subprocess
import sys


def Run(cmd, onlyTest = False):
	print(cmd)
	if onlyTest:
		return 0
	newenv = os.environ.copy()
	proc = subprocess.Popen(cmd, env=newenv)
	proc.wait()
	return proc.returncode


def RunCmds(cmds, onlyTest = False, filename = 'runcmds.bat'):
	bat = open(filename, 'w')
	for cmd in cmds:
		print(cmd, file=bat)
	bat.close()
	return RunFile(filename, True, onlyTest)


def RunFile(file, remove = True, onlyTest = False):
	newenv = os.environ.copy()
	if onlyTest:
		proc = subprocess.Popen('cmd /c type ' + file, env=newenv)
	else:
		proc = subprocess.Popen(file, env=newenv)
	proc.wait()
	if remove:
		os.remove(file)


if __name__ == '__main__':
	sys.exit(0)
