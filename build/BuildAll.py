# coding=utf-8
# Above line is for python
#
# Must be run from the directory this file is in.
#
# Revision History
# 2021-11-11 Moved from Projects/Installer to build, pyDcon usage
# 2021-11-09 Add vc143 support
# 2020-01-26 Change default compiler to vc142, default wx to WXWIN
# 2019-10-28 wx default is 3.1.3
# 2019-02-28 Add vc142 support
# 2018-12-24 wx default is 3.1.2
# 2018-10-06 Dropping support for pre VS2017 (and XP).
# 2018-01-27 Fix vcvarsall now changing directory.
# 2017-09-19 Rename vc15 to vc141, fix GetCompilerPaths tuple name
# 2017-04-09 Updated for vc141
# 2016-10-19 Changed RmMinusRF to using shutil.rmtree()
# 2016-06-10 Convert to Python3, cleaned up error checks.
# 2016-02-29 Changed to wx3.1 as default.
# 2015-09-10 Added -c (configuration) option.
# 2014-11-17 Add VC12 support, restructured for less code duplication.
# 2013-09-12 Add /m option to msbuild (multiple processor build)
# 2012-12-30 Drop VC9.
# 2012-09-23 Migrate to wx2.9 for ARBv3.
# 2012-07-25 Only run version include update from here.
# 2012-07-04 Update to supported compilers (vc9+)
#            Specifically set WXWIN to official release branch.
#            Changed program options.
# 2012-06-01 Changed VC10 output directory
# 2010-11-07 Updated to use vc10pro or sdk
# 2010-06-11 Support building on x64 OS
# 2010-05-30 Converted .bat to .py (keeps environment clean!)
"""BuildAll.py -w wxwin [-b type] compiler*
   -w wxwin: Root of wx tree (default: %WXWIN%)'
   -b type:  type is 'fullupdate', 'clean', or 'dirty' (default, dirty)
   -c config: 'release' or 'debug' (default, release)
   -t:       Testing, just print commands to run
   compiler: vc141, vc142, vc143 (default: vc142)
"""

import getopt
import os
import string
import sys

import pyDcon

onlyTest = False

# Verbosity:detailed, multiprocessors
# Verbosity:q/m/n/d/diag
msbuildOfficial = 'official.props'
msbuildOpts = '/v:n /m'


def AddCompilers(compilers, c):
	if not AddCompiler(compilers, c):
		return False
	compilersCheck = set()
	return AddCompiler(compilersCheck, c + 'x64')


def AddCompiler(compilers, c):
	vcBaseDir, vcvarsall, platformDir, platform = pyDcon.VSPaths.GetCompilerPaths(c)

	if len(vcBaseDir) == 0:
		return False

	compilers.add(c)
	return True


def main():
	global onlyTest, msbuildOfficial, msbuildOpts

	bit64on64 = False
	# 64bit on 64bit
	if 'PROCESSOR_ARCHITECTURE' in os.environ and os.environ['PROCESSOR_ARCHITECTURE'] == 'AMD64':
		bit64on64 = True

	wxwin = ''
	if 'WXWIN' in os.environ:
		wxwin = os.environ['WXWIN']

	compilers = set()
	updateBuildNumber = False
	configuration = 'Release'
	clean = False
	try:
		opts, args = getopt.getopt(sys.argv[1:], 'w:b:c:t')
	except getopt.error as msg:
		print(msg)
		print('Usage:', __doc__)
		return 1
	for o, a in opts:
		if '-w' == o:
			wxwin = a
		elif '-b' == o:
			if 'fullupdate' == a:
				updateBuildNumber = True
				clean = True
			elif 'clean' == a:
				updateBuildNumber = False
				clean = True
			elif 'dirty' == a:
				updateBuildNumber = False
				clean = False
			else:
				print('Usage:', __doc__)
				return 1
		elif '-c' == o:
			if 'release' == a:
				configuration = 'Release'
			elif 'debug' == a:
				configuration = 'Debug'
			else:
				print('Usage:', __doc__)
				return 1
		elif '-t' == o:
			onlyTest = True

	for c in args:
		if not AddCompilers(compilers, c):
			print('Usage:', __doc__)
			return 1

	if 0 == len(compilers):
		AddCompilers(compilers, 'vc142')

	if 0 == len(compilers) or len(wxwin) == 0:
		print('Usage:', __doc__)
		return 1

	os.environ['WXWIN'] = wxwin

	curDir = os.getcwd()

	# Add the official build props. Note: Do not use Directory.Build.props
	# for automatic inclusion. Building directly in devenv will pick that up.
	msbuildOpts = msbuildOpts + ' /p:ForceImportBeforeCppTargets=' + os.path.abspath(curDir + '\\..\\AgilityBookLibs\\Projects\\props\\' + msbuildOfficial)

	if updateBuildNumber:
		cmds = [
			r'cd ' + curDir + '\\..\\AgilityBookLibs\\Include',
			r'python SetBuildNumber.py --official',
			r'cd ' + curDir,
			r'cd ' + curDir + '\\..\\src\\Include',
			r'python SetBuildNumber.py --official',
			r'cd ' + curDir]
		pyDcon.Run.RunCmds(cmds, onlyTest)

	# Targets:
	# VC141
	#  Configuration: 'Release'/'Debug'
	#  Platform: x86, x64
	#  Targets: AgilityBook, ARBHelp, ARBUpdater, LibARB, LibTidy, TestARB

	for compiler in compilers:
		vcBaseDir, vcvarsall, platformDir, platform = pyDcon.VSPaths.GetCompilerPaths(compiler)

		cmds32 = [
			r'title ' + compiler + ' ' + configuration + ' ' + platform,
			r'cd ' + curDir + '\\..\\src\\Projects\\' + compiler,
			r'set "VSCMD_START_DIR=%CD%"',
			r'call ' + vcvarsall,
			r'msbuild AgilityBook.sln ' + msbuildOpts + ' /t:Build /p:Configuration=' + configuration + ';Platform=' + platform]

		if clean:
			pyDcon.RmMinusRF.RmMinusRF(curDir + '/../src/bin/' + platformDir + platform)
		pyDcon.Run.RunCmds(cmds32, onlyTest)
		if not onlyTest and not os.access(curDir + '/../src/bin/' + platformDir + platform + '/' + configuration + '/AgilityBook.exe', os.F_OK):
			print('ERROR: Compile failed, bailing out')
			return 1

		vcBaseDir, vcvarsall, platformDir, platform = pyDcon.VSPaths.GetCompilerPaths(compiler + 'x64')
		cmds64 = [
			r'title ' + compiler + ' ' + configuration + ' ' + platform,
			r'cd ' + curDir + '\\..\\src\\Projects\\' + compiler,
			r'set "VSCMD_START_DIR=%CD%"',
			r'call ' + vcvarsall,
			r'msbuild AgilityBook.sln ' + msbuildOpts + ' /t:Build /p:Configuration=' + configuration + ';Platform=' + platform]

		if clean:
			pyDcon.RmMinusRF.RmMinusRF(curDir + '/../src/bin/' + platformDir + platform)
		pyDcon.Run.RunCmds(cmds64, onlyTest)
		if not onlyTest and not os.access(curDir + '/../src/bin/' + platformDir + platform + '/' + configuration + '/AgilityBook.exe', os.F_OK):
			print('ERROR: Compile failed, bailing out')
			return 1

	return 0


if __name__ == '__main__':
	sys.exit(main())
