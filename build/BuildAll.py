# coding=utf-8
# Above line is for python
#
# Must be run from the directory this file is in.
#
# Revision History
# 2022-04-10 Only add 'official.props' on fullupdate build.
# 2022-04-05 Make vc143 default.
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
   -w wxwin:    Root of wx tree (default: %WXWIN%)'
   -b type:     type is 'fullupdate', 'clean', or 'dirty' (default, dirty)
   -c config:   'release' or 'debug' (default, release)
   -t:          Testing, just print commands to run
   compiler:    vc142, vc143 (default: vc143)
"""

import getopt
import os
import string
import sys

import pyDcon

defCompiler = 'vc143'
solution = 'AgilityBook.sln'

# Verbosity:detailed, multiprocessors
# Verbosity:q/m/n/d/diag
msbuildOpts = '/v:n /m'
# Only added on 'fullupdate' build
msbuildOfficial = r'..\AgilityBookLibs\Projects\props\official.props'

# Relative to this directory, used as: curDir + buildBin + ...
buildBin = '\\..\\bin\\' 


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


def DoCompile(projectDir, compiler, config, clean, onlyTest):
	vcBaseDir, vcvarsall, platformDir, platform = pyDcon.VSPaths.GetCompilerPaths(compiler)
	curDir = os.getcwd()

	cmds = [
		'title ' + compiler + ' ' + config + ' ' + platform,
		'cd ' + curDir + '\\..\\src\\Projects\\' + projectDir,
		'set "VSCMD_START_DIR=%CD%"',
		'call ' + vcvarsall,
		'msbuild ' + solution + ' ' + msbuildOpts + ' /t:Build /p:Configuration=' + config + ';Platform=' + platform]

	if clean:
		pyDcon.RmMinusRF.RmMinusRF(curDir + buildBin + platformDir + platform)
	pyDcon.Run.RunCmds(cmds, onlyTest)
	if not onlyTest and not os.access(curDir + buildBin + platformDir + platform + '/' + config + '/AgilityBook.exe', os.F_OK):
		print('ERROR: Compile failed, bailing out')
		return 1


def main():
	global solution, msbuildOfficial, msbuildOpts

	wxwin = ''
	if 'WXWIN' in os.environ:
		wxwin = os.environ['WXWIN']

	compilers = set()
	updateBuildNumber = False
	config = 'Release'
	clean = False
	onlyTest = False

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
				config = 'Release'
			elif 'debug' == a:
				config = 'Debug'
			else:
				print('Usage:', __doc__)
				return 1
		elif '-t' == o:
			onlyTest = True

	if updateBuildNumber:
		if not os.access(msbuildOfficial, os.F_OK):
			print('ERROR: File "' + msbuildOfficial+ '" does not exist')
			return 1

		# Add the official build props. Note: Do not use Directory.Build.props
		# for automatic inclusion. Building directly in devenv will pick that up.
		msbuildOpts = msbuildOpts + ' /p:ForceImportBeforeCppTargets=' + msbuildOfficial

	for c in args:
		if not AddCompilers(compilers, c):
			print('Usage:', __doc__)
			return 1

	if 0 == len(compilers):
		AddCompilers(compilers, defCompiler)

	if 0 == len(compilers) or len(wxwin) == 0:
		print('Usage:', __doc__)
		return 1

	os.environ['WXWIN'] = wxwin

	if updateBuildNumber:
		curDir = os.getcwd()
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
		DoCompile(compiler, compiler, config, clean, onlyTest)
		DoCompile(compiler, compiler + 'x64', config, clean, onlyTest)
	return 0


if __name__ == '__main__':
	sys.exit(main())
