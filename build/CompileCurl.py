# coding=utf-8
# Above line is for python
#
# This is my quick-and-easy way to compile curl on Windows.
#
# Revision History
# 2020-11-28 Created
"""CompileCurl.py -p curlsrc [-e] [-a] [-d] [-s] [-r config] compiler*
	-w wxwin: Root of wx tree, normally %WXWIN%
	-e:       Just show the environment, don't do it
	-a:       Compile all (vc142, vc142x64, vc142arm64)
	-d:       Compile for DLLs (default: both)
	-s:       Compile for static (default: both)
	-r config: config: release/debug
	compiler: vc142, vc142x64, vc142arm64
"""

import getopt
import glob
import os
import string
import subprocess
import sys

import pyDcon

tmpfile = 'tmpcomp' + os.environ['USERDOMAIN'] + '.bat'

compileIt = True
buildDll = True
buildStatic = True


def AddCompiler(compilers, c):
	vcBaseDir, vcvarsall, platformDir, platform = pyDcon.VSPaths.GetCompilerPaths(c)

	if len(vcBaseDir) == 0:
		return False

	compilers.add(c)
	return True


def Build(compilers, curlsrc, buildLib, release, debug):
	global compileIt

	for compiler in compilers:
		vcBaseDir, vcvarsall, platformDir, platform = pyDcon.VSPaths.GetCompilerPaths(compiler)

		newenv = os.environ.copy()

		bat = open(tmpfile, 'w')
		print('title ' + compiler, file=bat)

		setenv_rel = ''
		resetColor = False

		setenv_rel = 'call ' + vcvarsall

		build_rel = ''
		build_dbg = ''
		output_rel = r'..\builds\libcurl-release-' + platformDir + platform
		output_dbg = r'..\builds\libcurl-debug-' + platformDir + platform
		if buildLib:
			output_rel += '-mt'
			output_dbg += '-mt'
		else:
			output_rel += '-md'
			output_dbg += '-md'
		build_common = 'nmake -f Makefile.vc mode=static MACHINE=' + platform
		if buildLib:
			build_common += ' RTLIBCFG=static'
		build_rel += build_common + r' WITH_PREFIX=' + output_rel
		build_dbg += build_common + r' WITH_PREFIX=' + output_dbg + ' DEBUG=yes'

		print('set "VSCMD_START_DIR=%CD%"', file=bat)
		print(setenv_rel, file=bat)
		if release:
			print(build_rel, file=bat)
		if debug:
			print(build_dbg, file=bat)
		if resetColor:
			print('color 07', file=bat)

		bat.close()
		if compileIt:
			proc = subprocess.Popen(tmpfile, env=newenv)
		else:
			proc = subprocess.Popen('cmd /c type ' + tmpfile, env=newenv)
		proc.wait()
		os.remove(tmpfile)


def main():
	global compileIt, buildDll, buildStatic

	curlsrc = ''
	compilers = set()
	debug = True
	release = True
	try:
		print(sys.argv)
		opts, args = getopt.getopt(sys.argv[1:], 'p:eadsr:')
		print(opts,args)
	except getopt.error as msg:
		print(msg)
		print('Usage:', __doc__)
		return 1
	for o, a in opts:
		if '-p' == o:
			curlsrc = a
		elif '-e' == o:
			compileIt = False
		elif '-a' == o:
			AddCompiler(compilers, 'vc142')
			AddCompiler(compilers, 'vc142x64')
			AddCompiler(compilers, 'vc142arm64')
		elif '-d' == o:
			buildDll = True
			buildStatic = False
		elif '-s' == o:
			buildDll = False
			buildStatic = True
		elif '-r' == o:
			if a == 'release':
				release = True
				debug = False
			elif a == 'debug':
				release = False
				debug = True
			else:
				print('ERROR: Unknown option for -r: Must be "release" or "debug"')
				print('Usage:', __doc__)
				return 1

	# Made -w required since normal WXWIN rarely needs rebuilding.
	if len(curlsrc) == 0:
		print('ERROR: -p option not specified')
		print('Usage:', __doc__)
		return 1

	if not os.access(curlsrc, os.F_OK):
		print('ERROR: ' + curlsrc + ' doesn\'t exist')
		return 1

	for c in args:
		if not AddCompiler(compilers, c):
			print('Usage:', __doc__)
			return 1

	if 0 == len(compilers):
		print('Usage:', __doc__)
		return 1


	os.environ['INCLUDE'] = ''
	os.environ['LIB'] = ''

	os.chdir(curlsrc + r'\winbuild')

	if buildDll:
		Build(compilers, curlsrc, False, release, debug)
	if buildStatic:
		Build(compilers, curlsrc, True, release, debug)
	return 0


if __name__ == '__main__':
	sys.exit(main())
