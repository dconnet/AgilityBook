# coding=utf-8
# Above line is for python
#
# This is my quick-and-easy way to compile curl on Windows.
#
# Revision History
# 2020-12-08 Added install option
# 2020-11-28 Created
"""CompileCurl.py -p curlsrc [-e] [-x] [-a] [-d] [-s] [-r config] [-i path] [-I] compiler*
	-p curlsrc: Root of curl tree
	-e:         Just show the environment, don't do it
	-x:         Do not cleanup build output
	-a:         Compile all (vc142, vc142x64, vc142arm64)
	-d:         Compile for DLLs (default: both)
	-s:         Compile for static (default: both)
	-i path:    Install compiled libs to path
	-I:         Install compiled libs to %CURLPATH%
	-r config: config: release/debug
	compiler: vc142, vc142x64, vc142arm64
"""

import getopt
import glob
import os
import stat
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


# example: vc142x64-mt
def GetBuildPath(buildLib, platformDir, platform):
	path = platformDir + platform
	if buildLib:
		path += r'-mt'
	else:
		path += r'-md'
	return path


def GetCurlBuildDir(release, buildLib, platformDir, platform):
	path = r'..\builds\libcurl-' 
	if release:
		path += 'release-'
	else:
		path += 'debug-'
	path += GetBuildPath(buildLib, platformDir, platform)
	return path


def Build(compilers, curlsrc, buildLib, release, debug, installdir):
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
		output_rel = GetCurlBuildDir(True, buildLib, platformDir, platform)
		output_dbg = GetCurlBuildDir(False, buildLib, platformDir, platform)
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

		if len(installdir) > 0:
			print(r'rd /s/q ' + installdir + r'\include', file=bat)
			incsrc = GetCurlBuildDir(release, buildLib, platformDir, platform) + r'\include'
			print(r'xcopy /y/s ' + incsrc + ' ' + installdir + '\\include\\', file=bat)
			if release:
				print(r'xcopy /y/s ' + GetCurlBuildDir(True, buildLib, platformDir, platform) + r'\lib\*' + ' ' + installdir + '\\lib\\' + GetBuildPath(buildLib, platformDir, platform) + '\\', file=bat)
			if debug:
				print(r'xcopy /y/s ' + GetCurlBuildDir(False, buildLib, platformDir, platform) + r'\lib\*' + ' ' + installdir + '\\lib\\' + GetBuildPath(buildLib, platformDir, platform) + '\\', file=bat)

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
	installdir = ''
	cleanup = True
	try:
		opts, args = getopt.getopt(sys.argv[1:], 'p:eadsr:i:I')
	except getopt.error as msg:
		print(msg)
		print('Usage:', __doc__)
		return 1
	for o, a in opts:
		if '-p' == o:
			curlsrc = a
		elif '-e' == o:
			compileIt = False
		elif '-x' == o:
			cleanup = False
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
		elif '-i' == o:
			installdir = os.path.abspath(a)
		elif '-I' == o:
			if 'CURLPATH' not in os.environ:
				print('ERROR: CURLPATH is not set')
				return 1
			installdir = os.environ['CURLPATH']

	if len(curlsrc) == 0:
		print('ERROR: -p option not specified')
		print('Usage:', __doc__)
		return 1

	if len(installdir) > 0:
		if os.access(installdir, os.F_OK):
			mode = os.stat(installdir)[stat.ST_MODE]
			if not stat.S_ISDIR(mode):
				print('ERROR: ' + installdir + ' is not a directory')
				return 1
		else:
			os.mkdir(installdir)
			if not os.access(installdir, os.F_OK):
				print('ERROR: Cannot create directory: ' + installdir)
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
		Build(compilers, curlsrc, False, release, debug, installdir)
	if buildStatic:
		Build(compilers, curlsrc, True, release, debug, installdir)

	os.chdir('..')

	if cleanup:
		pyDcon.RmMinusRF.RmMinusRF('builds', True)
	return 0


if __name__ == '__main__':
	sys.exit(main())
