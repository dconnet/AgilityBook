# coding=utf-8
# Above line is for python
#
# Revision History
# 2010-11-07 DRC Updated to use vc10pro or sdk
# 2010-06-11 DRC Support building on x64 OS
# 2010-05-30 DRC Converted .bat to .py (keeps environment clean!)
"""BuildAll.py [fullupdate | clean] [vc9 | vc10]
   Default is to do a dirty build with both compilers
"""

import glob
import os
import stat
import string
import subprocess
import sys
import win32api
import win32con

ProgramFiles = r'c:\Program Files'
ProgramFiles64 = r'c:\Program Files'
useVC10SDK = False


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
	for cmd in cmds:
		print >>bat, cmd
	bat.close()
	proc = subprocess.Popen(filename)
	proc.wait()
	os.remove(filename)


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


# 7.1, 8.0, 9.0, 10.0 (as observed on my machine)
def GetVSDir(version):
	vsdir = GetRegString(win32con.HKEY_LOCAL_MACHINE, r'SOFTWARE\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir):
		vsdir = GetRegString(win32con.HKEY_CURRENT_USER, r'SOFTWARE\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir):
		vsdir = GetRegString(win32con.HKEY_LOCAL_MACHINE, r'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir):
		vsdir = GetRegString(win32con.HKEY_CURRENT_USER, r'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7', version)
	return vsdir


def main():
	if len(sys.argv) > 3:
		print 'Usage:', __doc__
		return 1

	global ProgramFiles, ProgramFiles64
	bit64on64 = False
	if os.environ.has_key('ProgramFiles'):
		ProgramFiles = os.environ['ProgramFiles']
	# 64bit on 64bit
	if os.environ.has_key('PROCESSOR_ARCHITECTURE') and os.environ['PROCESSOR_ARCHITECTURE'] == 'AMD64':
		bit64on64 = True
		ProgramFiles = r'c:\Program Files (x86)'
	# 64bit on Wow64 (32bit cmd shell spawned from msdev)
	if os.environ.has_key('PROCESSOR_ARCHITEW6432') and os.environ['PROCESSOR_ARCHITEW6432'] == 'AMD64':
		ProgramFiles = r'c:\Program Files (x86)'

	buildVC9 = True
	buildVC10 = True
	updateBuildNumber = False
	clean = False

	if len(sys.argv) == 2 or len(sys.argv) == 3:
		vcarg = 1
		if sys.argv[1] == 'fullupdate':
			updateBuildNumber = True
			clean = True
			vcarg = 2
		elif sys.argv[1] == 'clean':
			clean = True
			vcarg = 2
		error = True
		if len(sys.argv) == vcarg:
			error = False
		elif len(sys.argv) == vcarg + 1:
			if sys.argv[vcarg] == "vc9":
				error = False
				buildVC10 = False
			if sys.argv[vcarg] == "vc10":
				error = False
				buildVC9 = False
		if error:
			print 'Usage:', __doc__
			return 1

	if not updateBuildNumber:
		os.environ['SETBUILDNUMBER_UPDATE'] = '-x'

	# Targets:
	# VC7.1
	#  Configuration: 'Release'/'Debug' (non-unicode)
	#  Platform: Win32
	#  Targets:
	#   AgilityBook: all
	#   LibTidy: all
	# VC8
	#  Configuration: 'Release'/'Debug'/'Unicode Release'/'Unicode Debug'
	#  Platform: Win32, x64
	#  ARBHelp: 'Release'/'Debug'
	#  Targets:
	#   AgilityBook: all
	#   ARBHelp: no unicode
	#   LibTidy: no unicode
	# VC9/VC10
	#  Configuration: 'Release'/'Debug'/'Release - No Unicode'/'Debug - No Unicode'
	#  Platform: Win32, x64
	#  Targets:
	#   AgilityBook: unicode only
	#   ARBHelp: unicode only
	#   LibTidy: no unicode [debug/release=non unicode]
	#   TestARB: all

	if buildVC9:
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
		if not os.access('../../../bin/VC9Win32/Release/AgilityBook.exe', os.F_OK):
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
		if not os.access('../../../bin/VC9x64/Release/AgilityBook.exe', os.F_OK):
			print 'ERROR: Compile failed, bailing out'
			return 1

	if buildVC10:
		vc10Base = GetVSDir("10.0")
		if useVC10SDK:
			setvcvars = ProgramFiles + r'\Microsoft SDKs\Windows\v7.1\bin\setenv.cmd'
		else:
			setvcvars = vc10Base + r'\VC\vcvarsall.bat'
		if not os.access(vc10Base, os.F_OK) or not os.access(setvcvars, os.F_OK):
			print 'ERROR: "' + vc10Base + '" does not exist'
			return 1
		if clean:
			RmMinusRF('../../../bin/VC10Win32')
		cmds = (
			r'title VC10 Release Win32',
			r'cd ..\VC10',
			r'call "' + setvcvars + r'" x86',
			r'msbuild AgilityBook.sln /t:Build /p:Configuration=Release;Platform=Win32')
		RunCmds(cmds)
		if not os.access('../../../bin/VC10Win32/Release/AgilityBook.exe', os.F_OK):
			print 'ERROR: Compile failed, bailing out'
			return 1
		if clean:
			RmMinusRF('../../../bin/VC10x64')
		if useVC10SDK:
			cmds = (
				r'title VC10 Release x64',
				r'cd ..\VC10',
				r'call "' + setvcvars + r'" /release /x64 /xp',
				r'msbuild AgilityBook.sln /t:Build /p:Configuration=Release;Platform=x64',
				r'color 07')
		elif bit64on64:
			cmds = (
				r'title VC10 Release x64',
				r'cd ..\VC10',
				r'call "' + setvcvars + r'" amd64',
				r'msbuild AgilityBook.sln /t:Build /p:Configuration=Release;Platform=x64')
		else:
			cmds = (
				r'title VC10 Release x64',
				r'cd ..\VC10',
				r'call "' + setvcvars + r'" x86_amd64',
				r'msbuild AgilityBook.sln /t:Build /p:Configuration=Release;Platform=x64')
		RunCmds(cmds)
		if not os.access('../../../bin/VC10x64/Release/AgilityBook.exe', os.F_OK):
			print 'ERROR: Compile failed, bailing out'
			return 1


sys.exit(main())
