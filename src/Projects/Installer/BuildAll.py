# coding=utf-8
# Above line is for python
#
# Revision History
# 2012-07-25 DRC Only run version include update from here.
# 2012-07-04 DRC Update to supported compilers (vc9+)
#                Specifically set WXWIN to official release branch.
#                Changed program options.
# 2012-06-01 DRC Changed VC10 output directory
# 2010-11-07 DRC Updated to use vc10pro or sdk
# 2010-06-11 DRC Support building on x64 OS
# 2010-05-30 DRC Converted .bat to .py (keeps environment clean!)
"""BuildAll.py -w wxwin [-b fullupdate | clean] compiler*
   -w wxwin: Root of wx tree, normally %WXBASE%\\wxWidgets-2.8.12'
   -b type:  type is 'fullupdate', 'clean', or 'dirty' (default, dirty)
   -t:       Testing, just print commands to run
   compiler: vc9, vc10, vc11 (default: vc9,vc10)
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
wxBranch = r'\wxWidgets-2.8.12'

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


# 7.1, 8.0, 9.0, 10.0, 11.0 (as observed on my machine)
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
	global vc9Base, vc10Base, vc11Base
	baseDir = ''
	testFile = ''
	if c == 'vc9':
		vc9Base = GetVSDir("9.0")
		baseDir = vc9Base
		testFile = baseDir + r'\VC\vcvarsall.bat'
	elif c == 'vc10':
		vc10Base = GetVSDir("10.0")
		baseDir = vc10Base
		testFile = baseDir + r'\VC\vcvarsall.bat'
	elif c == 'vc11':
		vc11Base = GetVSDir("11.0")
		baseDir = vc11Base
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
	clean = False
	try:
		opts, args = getopt.getopt(sys.argv[1:], 'w:b:t')
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
		elif '-t' == o:
			testing = True

	for c in args:
		if not AddCompiler(compilers, c):
			print 'Unknown compiler:', c
			print 'Usage:', __doc__
			return 1

	if 0 == len(compilers):
		AddCompiler(compilers, 'vc9')
		AddCompiler(compilers, 'vc10')

	if len(wxwin) == 0:
		print 'Usage:', __doc__
		return 1
	os.environ['WXWIN'] = wxwin

	if updateBuildNumber:
		cmds = (
			r'cd ..\..\Include',
			r'python SetBuildNumber.py --official')
		RunCmds(cmds)

	# Targets:
	# VC9/VC10/VC11
	#  Configuration: 'Release'/'Debug'
	#  Platform: Win32, x64
	#  Targets: AgilityBook, ARBHelp, ARBUpdater, LibARB, LibTidy, TestARB

	for compiler in compilers:
		if compiler == 'vc9':
			vc9Base = GetVSDir("9.0")
			setvcvars = vc9Base + r'\VC\vcvarsall.bat'
			if not os.access(vc9Base, os.F_OK) or not os.access(setvcvars, os.F_OK):
				print 'ERROR: "' + vc9Base + '" does not exist'
				return 1
			if clean:
				RmMinusRF('../../../bin/VC9Win32')
			remove(r'../VC9/bldWin32.txt')
			cmds = (
				r'title VC9 Release Win32',
				r'cd ..\VC9',
				r'call "' + setvcvars + r'" x86',
				r'devenv AgilityBook.sln /out bldWin32.txt /build "Release|Win32"')
			RunCmds(cmds)
			if not testing and not os.access('../../../bin/VC9Win32/Release/AgilityBook.exe', os.F_OK):
				print 'ERROR: Compile failed, bailing out'
				return 1
			if clean:
				RmMinusRF('../../../bin/VC9x64')
			remove(r'../VC9/bldWin64.txt')
			cmds = (
				r'title VC9 Release x64',
				r'cd ..\VC9',
				r'call "' + setvcvars + r'" x86_amd64',
				r'devenv AgilityBook.sln /out bldWin64.txt /build "Release|x64"')
			RunCmds(cmds)
			if not testing and not os.access('../../../bin/VC9x64/Release/AgilityBook.exe', os.F_OK):
				print 'ERROR: Compile failed, bailing out'
				return 1

		elif compiler == 'vc10':
			vc10Base = GetVSDir("10.0")
			PlatformTools = '100'
			if useVC10SDK:
				setvcvars = ProgramFiles + r'\Microsoft SDKs\Windows\v7.1\bin\setenv.cmd'
			else:
				setvcvars = vc10Base + r'\VC\vcvarsall.bat'
			if not os.access(vc10Base, os.F_OK):
				print 'ERROR: "' + vc10Base + '" does not exist'
				return 1
			if not os.access(setvcvars, os.F_OK):
				print 'ERROR: "' + setvcvars + '" does not exist'
				return 1
			if clean:
				RmMinusRF('../../../bin/VC' + PlatformTools + 'Win32')
			if useVC10SDK:
				cmds = (
					r'title VC10 Release Win32',
					r'cd ..\VC10',
					r'call "' + setvcvars + r'" /Release /x86 /xp',
					r'msbuild AgilityBook.sln /t:Build /p:Configuration=Release;Platform=Win32')
			else:
				cmds = (
					r'title VC10 Release Win32',
					r'cd ..\VC10',
					r'call "' + setvcvars + r'" x86',
					r'msbuild AgilityBook.sln /t:Build /p:Configuration=Release;Platform=Win32')
			RunCmds(cmds)
			if not testing and not os.access('../../../bin/VC' + PlatformTools + 'Win32/Release/AgilityBook.exe', os.F_OK):
				print 'ERROR: Compile failed, bailing out'
				return 1
			if clean:
				RmMinusRF('../../../bin/VC' + PlatformTools + 'x64')
			if useVC10SDK:
				cmds = (
					r'title VC10 Release x64',
					r'cd ..\VC10',
					r'call "' + setvcvars + r'" /Release /x64 /xp',
					r'msbuild AgilityBook.sln /t:Build /p:Configuration=Release;Platform=x64',
					r'color 07')
			else:
				envTarget = 'x86_amd64'
				if bit64on64:
					envTarget = 'amd64'
				cmds = (
					r'title VC10 Release x64',
					r'cd ..\VC10',
					r'call "' + setvcvars + r'" ' + envTarget,
					r'msbuild AgilityBook.sln /t:Build /p:Configuration=Release;Platform=x64')
			RunCmds(cmds)
			if not testing and not os.access('../../../bin/VC' + PlatformTools + 'x64/Release/AgilityBook.exe', os.F_OK):
				print 'ERROR: Compile failed, bailing out'
				return 1

		elif compiler == 'vc11':
			vc11Base = GetVSDir("11.0")
			PlatformTools = '110'
			setvcvars = vc11Base + r'\VC\vcvarsall.bat'
			if not os.access(vc11Base, os.F_OK):
				print 'ERROR: "' + vc11Base + '" does not exist'
				return 1
			if not os.access(setvcvars, os.F_OK):
				print 'ERROR: "' + setvcvars + '" does not exist'
				return 1
			if clean:
				RmMinusRF('../../../bin/VC' + PlatformTools + 'Win32')
			cmds = (
				r'title VC11 Release Win32',
				r'cd ..\VC11',
				r'call "' + setvcvars + r'" x86',
				r'msbuild AgilityBook.sln /t:Build /p:Configuration=Release;Platform=Win32')
			RunCmds(cmds)
			if not testing and not os.access('../../../bin/VC' + PlatformTools + 'Win32/Release/AgilityBook.exe', os.F_OK):
				print 'ERROR: Compile failed, bailing out'
				return 1
			if clean:
				RmMinusRF('../../../bin/VC' + PlatformTools + 'x64')
			envTarget = 'x86_amd64'
			if bit64on64:
				envTarget = 'amd64'
			cmds = (
				r'title VC11 Release x64',
				r'cd ..\VC11',
				r'call "' + setvcvars + r'" ' + envTarget,
				r'msbuild AgilityBook.sln /t:Build /p:Configuration=Release;Platform=x64')
			RunCmds(cmds)
			if not testing and not os.access('../../../bin/VC' + PlatformTools + 'x64/Release/AgilityBook.exe', os.F_OK):
				print 'ERROR: Compile failed, bailing out'
				return 1

	return 0


if __name__ == '__main__':
	sys.exit(main())
