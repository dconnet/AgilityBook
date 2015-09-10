# coding=utf-8
# Above line is for python
#
# Revision History
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
   -w wxwin: Root of wx tree (default: %WXBASE%\\wxWidgets-3.0.2)'
   -b type:  type is 'fullupdate', 'clean', or 'dirty' (default, dirty)
   -c config: 'release' or 'debug' (default, release)
   -t:       Testing, just print commands to run
   compiler: vc10, vc11, vc12, vc14 (default: vc10)
"""

import getopt
import glob
import os
import stat
import string
import subprocess
import sys
import win32api
import win32con

ProgramFiles = r'c:\Program Files'
useVC10SDK = False

# ARB is officially released using this branch. This is located under $WXBASE.
wxBranch = r'\wxWidgets-3.0.2'

testing = False

def errprint(*args):
	# Used to print exception messages
	strings = map(str, args)
	msg = ' '.join(strings)
	if msg[-1:] != '\n':
		msg += '\n'
	sys.stderr.write(msg)


# Quietly remove a file
def remove(file):
	if os.access(file, os.F_OK):
		os.remove(file)


# Kill an entire directory
def RmMinusRF(name):
	if os.access(name, os.F_OK):
		mode = os.stat(name)[stat.ST_MODE]
		if stat.S_ISDIR(mode):
			curDir = os.getcwd()
			os.chdir(name)
			for file in glob.glob('./*'):
				RmMinusRF(file)
			os.chdir(curDir)
			os.rmdir(name)
		else:
			if not os.access(name, os.W_OK):
				os.chmod(name, 0666)
			if os.access(name, os.W_OK):
				try:
					os.remove(name)
				except OSError, msg:
					errprint('os.remove:', msg)


def RunCmds(cmds):
	filename = 'runcmds.bat'
	bat = open(filename, 'w')
	if testing:
		print >>bat, '@echo off'
		for cmd in cmds:
			print >>bat, 'echo ' + cmd
	else:
		for cmd in cmds:
			print >>bat, cmd
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
	except Exception, msg:
		return ""
	try:
		return win32api.RegQueryValueEx(key, value)[0]
	except Exception, msg:
		return ""


# 7.1, 8.0, 9.0, 10.0, 11.0, 12.0 (as observed on my machine)
def GetVSDir(version):
	vsdir = GetRegString(win32con.HKEY_LOCAL_MACHINE, r'SOFTWARE\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir):
		vsdir = GetRegString(win32con.HKEY_CURRENT_USER, r'SOFTWARE\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir):
		vsdir = GetRegString(win32con.HKEY_LOCAL_MACHINE, r'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir):
		vsdir = GetRegString(win32con.HKEY_CURRENT_USER, r'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7', version)
	return vsdir


def AddCompiler(compilers, c):
	global vc10Base, vc11Base, vc12Base, vc14Base
	baseDir = ''
	testFile = ''

	if c == 'vc10':
		vc10Base = GetVSDir("10.0")
		baseDir = vc10Base
		testFile = baseDir + r'\VC\vcvarsall.bat'

	elif c == 'vc11':
		vc11Base = GetVSDir("11.0")
		baseDir = vc11Base
		testFile = baseDir + r'\VC\vcvarsall.bat'

	elif c == 'vc12':
		vc12Base = GetVSDir("12.0")
		baseDir = vc12Base
		testFile = baseDir + r'\VC\vcvarsall.bat'

	elif c == 'vc14':
		vc14Base = GetVSDir("14.0")
		baseDir = vc14Base
		testFile = baseDir + r'\VC\vcvarsall.bat'

	else:
		return False

	if not os.access(baseDir, os.F_OK) or not os.access(testFile, os.F_OK):
		print 'ERROR: "' + baseDir + '" does not exist'
		return False
	compilers.add(c)
	return True


def main():
	# ProgramFiles must point to the native one since the SDK installs there.
	global ProgramFiles, testing
	bit64on64 = False
	if os.environ.has_key('ProgramFiles'):
		ProgramFiles = os.environ['ProgramFiles']
	# 64bit on 64bit
	if os.environ.has_key('PROCESSOR_ARCHITECTURE') and os.environ['PROCESSOR_ARCHITECTURE'] == 'AMD64':
		bit64on64 = True
	# 64bit on Wow64 (32bit cmd shell spawned from msdev)
	#if os.environ.has_key('PROCESSOR_ARCHITEW6432') and os.environ['PROCESSOR_ARCHITEW6432'] == 'AMD64':

	wxwin = ''
	if os.environ.has_key('WXBASE'):
		wxwin= os.environ['WXBASE'] + wxBranch

	compilers = set()
	updateBuildNumber = False
	config = 'Release'
	clean = False
	try:
		opts, args = getopt.getopt(sys.argv[1:], 'w:b:c:t')
	except getopt.error, msg:
		print msg
		print 'Usage:', __doc__
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
				print 'Usage:', __doc__
				return 1
		elif '-c' == o:
			if 'release' == a:
				config = 'Release'
			elif 'debug' == a:
				config = 'Debug'
			else:
				print 'Usage:', __doc__
				return 1
		elif '-t' == o:
			testing = True

	for c in args:
		if not AddCompiler(compilers, c):
			print 'Unknown compiler:', c
			print 'Usage:', __doc__
			return 1

	if 0 == len(compilers):
		AddCompiler(compilers, 'vc10')

	if len(wxwin) == 0:
		print 'Usage:', __doc__
		return 1
	os.environ['WXWIN'] = wxwin

	if updateBuildNumber:
		cmds = (
			r'cd ..\..\Include',
			r'python SetBuildNumber.py --official',
			r'cd ..\Projects\Installer')
		RunCmds(cmds)

	# Targets:
	# VC10/VC11/VC12
	#  Configuration: 'Release'/'Debug'
	#  Platform: Win32, x64
	#  Targets: AgilityBook, ARBHelp, ARBUpdater, LibARB, LibTidy, TestARB

	for compiler in compilers:
		if compiler == 'vc10':
			vc10Base = GetVSDir("10.0")
			if not os.access(vc10Base, os.F_OK):
				print 'ERROR: "' + vc10Base + '" does not exist'
				return 1
			PlatformTools = '100'
			if useVC10SDK:
				setvcvars = ProgramFiles + r'\Microsoft SDKs\Windows\v7.1\bin\setenv.cmd'
				cmds32 = (
					r'title VC10 ' + config + ' Win32',
					r'cd ..\VC10',
					r'call "' + setvcvars + r'" /' + config + ' /x86 /xp',
					r'msbuild AgilityBook.sln /m /t:Build /p:Configuration=' + config + ';Platform=Win32')
				cmds64 = (
					r'title VC10 ' + config + ' x64',
					r'cd ..\VC10',
					r'call "' + setvcvars + r'" /' + config + ' /x64 /xp',
					r'msbuild AgilityBook.sln /m /t:Build /p:Configuration=' + config + ';Platform=x64',
					r'color 07')
			else:
				setvcvars = vc10Base + r'\VC\vcvarsall.bat'
				cmds32 = (
					r'title VC10 ' + config + ' Win32',
					r'cd ..\VC10',
					r'call "' + setvcvars + r'" x86',
					r'msbuild AgilityBook.sln /m /t:Build /p:Configuration=' + config + ';Platform=Win32')
				envTarget = 'x86_amd64'
				if bit64on64:
					envTarget = 'amd64'
				cmds64 = (
					r'title VC10 ' + config + ' x64',
					r'cd ..\VC10',
					r'call "' + setvcvars + r'" ' + envTarget,
					r'msbuild AgilityBook.sln /m /t:Build /p:Configuration=' + config + ';Platform=x64')

		elif compiler == 'vc11':
			vc11Base = GetVSDir("11.0")
			if not os.access(vc11Base, os.F_OK):
				print 'ERROR: "' + vc11Base + '" does not exist'
				return 1
			PlatformTools = '110'
			setvcvars = vc11Base + r'\VC\vcvarsall.bat'
			cmds32 = (
				r'title VC11 ' + config + ' Win32',
				r'cd ..\VC11',
				r'call "' + setvcvars + r'" x86',
				r'msbuild AgilityBook.sln /m /t:Build /p:Configuration=' + config + ';Platform=Win32')
			envTarget = 'x86_amd64'
			if bit64on64:
				envTarget = 'amd64'
			cmds64 = (
				r'title VC11 ' + config + ' x64',
				r'cd ..\VC11',
				r'call "' + setvcvars + r'" ' + envTarget,
				r'msbuild AgilityBook.sln /m /t:Build /p:Configuration=' + config + ';Platform=x64')

		elif compiler == 'vc12':
			vc12Base = GetVSDir("12.0")
			if not os.access(vc12Base, os.F_OK):
				print 'ERROR: "' + vc12Base + '" does not exist'
				return 1
			PlatformTools = '120'
			setvcvars = vc12Base + r'\VC\vcvarsall.bat'
			cmds32 = (
				r'title VC12 ' + config + ' Win32',
				r'cd ..\VC12',
				r'call "' + setvcvars + r'" x86',
				r'msbuild AgilityBook.sln /m /t:Build /p:Configuration=' + config + ';Platform=Win32')
			envTarget = 'x86_amd64'
			if bit64on64:
				envTarget = 'amd64'
			cmds64 = (
				r'title VC12 ' + config + ' x64',
				r'cd ..\VC12',
				r'call "' + setvcvars + r'" ' + envTarget,
				r'msbuild AgilityBook.sln /m /t:Build /p:Configuration=' + config + ';Platform=x64')

		elif compiler == 'vc14':
			vc14Base = GetVSDir("14.0")
			if not os.access(vc14Base, os.F_OK):
				print 'ERROR: "' + vc14Base + '" does not exist'
				return 1
			PlatformTools = '140'
			setvcvars = vc14Base + r'\VC\vcvarsall.bat'
			cmds32 = (
				r'title VC14 ' + config + ' Win32',
				r'cd ..\VC14',
				r'call "' + setvcvars + r'" x86',
				r'msbuild AgilityBook.sln /m /t:Build /p:Configuration=' + config + ';Platform=Win32')
			envTarget = 'x86_amd64'
			if bit64on64:
				envTarget = 'amd64'
			cmds64 = (
				r'title VC14 ' + config + ' x64',
				r'cd ..\VC14',
				r'call "' + setvcvars + r'" ' + envTarget,
				r'msbuild AgilityBook.sln /m /t:Build /p:Configuration=' + config + ';Platform=x64')

		if not os.access(setvcvars, os.F_OK):
			print 'ERROR: "' + setvcvars + '" does not exist'
			return 1

		if clean:
			RmMinusRF('../../../bin/vc' + PlatformTools + 'Win32')
		RunCmds(cmds32)
		if not testing and not os.access('../../../bin/vc' + PlatformTools + 'Win32/' + config + '/AgilityBook.exe', os.F_OK):
			print 'ERROR: Compile failed, bailing out'
			return 1

		if clean:
			RmMinusRF('../../../bin/vc' + PlatformTools + 'x64')
		RunCmds(cmds64)
		if not testing and not os.access('../../../bin/vc' + PlatformTools + 'x64/' + config + '/AgilityBook.exe', os.F_OK):
			print 'ERROR: Compile failed, bailing out'
			return 1

	return 0


if __name__ == '__main__':
	sys.exit(main())
