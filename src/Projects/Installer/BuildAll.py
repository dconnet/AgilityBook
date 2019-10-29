# coding=utf-8
# Above line is for python
#
# Revision History
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
   -w wxwin: Root of wx tree (default: %WXBASE%\\wxWidgets-3.1.3)'
   -b type:  type is 'fullupdate', 'clean', or 'dirty' (default, dirty)
   -c config: 'release' or 'debug' (default, release)
   -t:       Testing, just print commands to run
   compiler: vc141 (default: vc141)
"""

import getopt
import glob
import os
import shutil
import stat
import string
import subprocess
import sys
import win32api
import win32con

# ARB is officially released using this branch. This is located under $WXBASE.
wxBranch = r'\wxWidgets-3.1.3'

testing = False

def errprint(*args):
	# Used to print exception messages
	strings = list(map(str, args))
	msg = ' '.join(strings)
	if msg[-1:] != '\n':
		msg += '\n'
	sys.stderr.write(msg)


# Quietly remove a file
def remove(file):
	if os.access(file, os.F_OK):
		os.remove(file)


# Kill an entire directory
def del_rw(func, path, exc_info):
	os.chmod(path, stat.S_IWUSR)
	func(path)
def RmMinusRF(name):
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


def RunCmds(cmds):
	global testing
	filename = 'runcmds.bat'
	bat = open(filename, 'w')
	if testing:
		print('@echo off', file=bat)
		for cmd in cmds:
			print('echo ' + cmd, file=bat)
	else:
		for cmd in cmds:
			print(cmd, file=bat)
	bat.close()
	newenv = os.environ.copy()
	proc = subprocess.Popen(filename, env=newenv)
	proc.wait()
	os.remove(filename)
	return proc.returncode


def GetRegString(hkey, path, value):
	key = None
	try:
		key = win32api.RegOpenKeyEx(hkey, path, 0, win32con.KEY_READ)
	except Exception as msg:
		return ""
	try:
		return win32api.RegQueryValueEx(key, value)[0]
	except Exception as msg:
		return ""


# 7.1, 8.0, 9.0, 10.0, 11.0, 12.0, 14.0, 15.0 (as observed on my machine)
# Yes, VisualStudio2017 == reg("15.0") == _msc_ver191x
# 16.0 (VS2019RC) does not install a key - hard code it
def GetVSDir(version):
	vsdir = GetRegString(win32con.HKEY_LOCAL_MACHINE, r'SOFTWARE\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir):
		vsdir = GetRegString(win32con.HKEY_CURRENT_USER, r'SOFTWARE\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir):
		vsdir = GetRegString(win32con.HKEY_LOCAL_MACHINE, r'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir):
		vsdir = GetRegString(win32con.HKEY_CURRENT_USER, r'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir) and version == '16.0':
		vsdir = r'C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional' + '\\'
		if not os.access(vsdir, os.F_OK):
			vsdir = r'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community' + '\\'
			if not os.access(vsdir, os.F_OK):
				vsdir = ''
	return vsdir


def GetX64Target(vcBase):
	target = 'x86_amd64'

	# 64bit on 64bit
	if 'PROCESSOR_ARCHITECTURE' in os.environ and os.environ['PROCESSOR_ARCHITECTURE'] == 'AMD64':
		# Note: We used to check for the existence of <vcBase>\VC\bin\amd64.
		# VS2017 moved that directory. Just assume that if we're compiling
		# for 64bit on 64bit that the user installed that. With current VS,
		# that's just done - not like older versions where it was a choice.
		target = 'amd64'
	return target


def GetCompilerPaths(c):
	baseDir = ''
	vcvarsall = ''
	target = ''
	extraargs = ''
	platformDir = ''
	platform = ''

	if c == 'vc141':
		#vcvarsall [arch]
		#vcvarsall [arch] [version]
		#vcvarsall [arch] [platform_type] [version]
		# [arch]: x86 | amd64 | x86_amd64 | x86_arm | x86_arm64 | amd64_x86 | amd64_arm | amd64_arm64
		# [platform_type]: {empty} | store | uwp
		# [version] : full Windows 10 SDK number (e.g. 10.0.10240.0) or "8.1" to use the Windows 8.1 SDK.
		baseDir = GetVSDir("15.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = 'x86'
		# Can target specific SDKs
		#extraargs = ' 10.0.14393.0'
		platformDir = 'vc141'
		platform = 'Win32'

	elif c == 'vc141x64':
		baseDir = GetVSDir("15.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetX64Target(baseDir)
		platformDir = 'vc141'
		platform = 'x64'

	elif c == 'vc142':
		baseDir = GetVSDir("16.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = 'x86'
		platformDir = 'vc142'
		platform = 'Win32'

	elif c == 'vc142x64':
		baseDir = GetVSDir("16.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetX64Target(baseDir)
		platformDir = 'vc142'
		platform = 'x64'

	else:
		print('ERROR: Unknown target: ' + c)
		return ('', '', '', '')

	if len(baseDir) == 0:
		print('ERROR: Unknown target: ' + c)
		return ('', '', '', '')
	if not os.access(baseDir, os.F_OK):
		print('ERROR: "' + baseDir + '" does not exist')
		return ('', '', '', '')
	if not os.access(vcvarsall, os.F_OK):
		print('ERROR: "' + vcvarsall + '" does not exist')
		return ('', '', '', '')

	return (baseDir, '"' + vcvarsall + '" ' + target + extraargs, platformDir, platform)


def AddCompilers(compilers, c):
	if not AddCompiler(compilers, c):
		return False
	compilersCheck = set()
	return AddCompiler(compilersCheck, c + 'x64')


def AddCompiler(compilers, c):
	vcBaseDir, vcvarsall, platformDir, platform = GetCompilerPaths(c)

	if len(vcBaseDir) == 0:
		return False

	compilers.add(c)
	return True


def main():
	global testing
	bit64on64 = False
	# 64bit on 64bit
	if 'PROCESSOR_ARCHITECTURE' in os.environ and os.environ['PROCESSOR_ARCHITECTURE'] == 'AMD64':
		bit64on64 = True

	wxwin = ''
	if 'WXBASE' in os.environ:
		wxwin= os.environ['WXBASE'] + wxBranch

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
			testing = True

	for c in args:
		if not AddCompilers(compilers, c):
			print('Usage:', __doc__)
			return 1

	if 0 == len(compilers):
		AddCompilers(compilers, 'vc141')

	if 0 == len(compilers) or len(wxwin) == 0:
		print('Usage:', __doc__)
		return 1

	os.environ['WXWIN'] = wxwin

	if updateBuildNumber:
		cmds = (
			r'cd ..\..\Include',
			r'python SetBuildNumber.py --official',
			r'cd ..\Projects\Installer')
		RunCmds(cmds)

	# Targets:
	# VC141
	#  Configuration: 'Release'/'Debug'
	#  Platform: Win32, x64
	#  Targets: AgilityBook, ARBHelp, ARBUpdater, LibARB, LibTidy, TestARB

	for compiler in compilers:
		vcBaseDir, vcvarsall, platformDir, platform = GetCompilerPaths(compiler)

		cmds32 = (
			r'title ' + compiler + ' ' + configuration + ' ' + platform,
			r'cd ..\\' + compiler,
			r'set "VSCMD_START_DIR=%CD%"',
			r'call ' + vcvarsall,
			r'msbuild AgilityBook.sln /m /t:Build /p:Configuration=' + configuration + ';Platform=' + platform)

		if clean:
			RmMinusRF('../../../bin/' + platformDir + platform)
		RunCmds(cmds32)
		if not testing and not os.access('../../../bin/' + platformDir + platform + '/' + configuration + '/AgilityBook.exe', os.F_OK):
			print('ERROR: Compile failed, bailing out')
			return 1

		vcBaseDir, vcvarsall, platformDir, platform = GetCompilerPaths(compiler + 'x64')
		cmds64 = (
			r'title ' + compiler + ' ' + configuration + ' ' + platform,
			r'cd ..\\' + compiler,
			r'set "VSCMD_START_DIR=%CD%"',
			r'call ' + vcvarsall,
			r'msbuild AgilityBook.sln /m /t:Build /p:Configuration=' + configuration + ';Platform=' + platform)

		if clean:
			RmMinusRF('../../../bin/' + platformDir + platform)
		RunCmds(cmds64)
		if not testing and not os.access('../../../bin/' + platformDir + platform + '/' + configuration + '/AgilityBook.exe', os.F_OK):
			print('ERROR: Compile failed, bailing out')
			return 1

	return 0


if __name__ == '__main__':
	sys.exit(main())
