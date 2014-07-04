# coding=utf-8
# Above line is for python
#
# This is my quick-and-easy way to compile libxml2
#
# Revision History
# 2014-06-06 Added vc12
# 2012-06-15 Fixed multiple compile problem
# 2012-06-01 Added vc11 switches, removed VC9 from all
# 2011-03-08 /MD/MT was not being specified properly.
# 2010-07-17 Created
"""CompileLibxml.py [-t] [-a] compiler*
	[-t]: Test, do not run commands
	[-a]: All compilers (vc10/vc10x64)
	compiler: vc9, vc9x64, vc10, vc10x64, vc11, vc11x64, vc12, vc12x64
"""
import getopt
import glob
import os
import string
import subprocess
import sys
import time
import win32api
import win32con

libxml2Dir = 'libxml2-2.9.1'

tmpfile = 'tmpcomp.bat'

vc9Base     = r'\Microsoft Visual Studio 9.0'
vc10Base    = r'\Microsoft Visual Studio 10.0'
vc11Base    = r'\Microsoft Visual Studio 11.0'
vc12Base    = r'\Microsoft Visual Studio 12.0'


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


# Return SDK path (with ending slash)
def GetWindowsSdkDir(vcinstalldir):
	WindowsSdkDir = GetRegString(win32con.HKEY_LOCALMACHINE, r'SOFTWARE\Microsoft\Microsoft SDKs\Windows', 'CurrentInstallFolder')
	if 0 == len(WindowsSdkDir):
		WindowsSdkDir = GetRegString(win32con.HKEY_CURRENT_USER, r'SOFTWARE\Microsoft\Microsoft SDKs\Windows', 'CurrentInstallFolder')
	if 0 == len(WindowsSdkDir):
		WindowsSdkDir = vcinstalldir + '\\PlatformSDK\\'
	return WindowsSdkDir


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


def AddCompiler(compilers, c):
	global vc9Base, vc10Base, vc11Base, vc12Base
	baseDir = ''
	testFile = ''

	if c == 'vc9':
		vc9Base = GetVSDir("9.0")
		baseDir = vc9Base
		testFile = baseDir + r'\VC\vcvarsall.bat'
	elif c == 'vc9x64':
		vc9Base = GetVSDir("9.0")
		baseDir = vc9Base
		testFile = baseDir + r'\VC\vcvarsall.bat'

	elif c == 'vc10':
		vc10Base = GetVSDir("10.0")
		baseDir = vc10Base
		testFile = baseDir + r'\VC\vcvarsall.bat'
	elif c == 'vc10x64':
		vc10Base = GetVSDir("10.0")
		baseDir = vc10Base
		testFile = baseDir + r'\VC\vcvarsall.bat'

	elif c == 'vc11':
		vc11Base = GetVSDir("11.0")
		baseDir = vc11Base
		testFile = baseDir + r'\VC\vcvarsall.bat'
	elif c == 'vc11x64':
		vc11Base = GetVSDir("11.0")
		baseDir = vc11Base
		testFile = baseDir + r'\VC\vcvarsall.bat'

	elif c == 'vc12':
		vc12Base = GetVSDir("12.0")
		baseDir = vc12Base
		testFile = baseDir + r'\VC\vcvarsall.bat'
	elif c == 'vc12x64':
		vc12Base = GetVSDir("12.0")
		baseDir = vc12Base
		testFile = baseDir + r'\VC\vcvarsall.bat'

	else:
		return False
	if not os.access(baseDir, os.F_OK) or not os.access(testFile, os.F_OK):
		print 'ERROR: "' + baseDir + '" does not exist'
		return False
	compilers.add(c)
	return True


def main():
	echo = ''
	bit64on64 = False
	# 64bit on 64bit
	if os.environ.has_key('PROCESSOR_ARCHITECTURE') and os.environ['PROCESSOR_ARCHITECTURE'] == 'AMD64':
		bit64on64 = True
	# 64bit on Wow64 (32bit cmd shell spawned from msdev)
	#if os.environ.has_key('PROCESSOR_ARCHITEW6432') and os.environ['PROCESSOR_ARCHITEW6432'] == 'AMD64':
	#

	compilers = set()
	try:
		opts, args = getopt.getopt(sys.argv[1:], 'at')
	except getopt.error, msg:
		print msg
		print 'Usage:', __doc__
		return 1
	for o, a in opts:
		if '-a' == o:
			AddCompiler(compilers, 'vc10')
			AddCompiler(compilers, 'vc10x64')

		elif '-t' == o:
			echo = 'echo '

	for c in args:
		if not AddCompiler(compilers, c):
			print 'Unknown compiler:', c
			print 'Usage:', __doc__
			return 1

	if 0 == len(compilers):
		print 'Usage:', __doc__
		return 1

	common_config = 'iconv=no compiler=msvc vcmanifest=yes ftp=no'

	os.environ['INCLUDE'] = ''
	os.environ['LIB'] = ''

	os.chdir(libxml2Dir + r'\win32')

	for compiler in compilers:
		newenv = os.environ.copy()

		# compiler output directory prefix
		prefix = ''

		# rel will be called first, so if no different, just set that.
		setenv_rel = ''

		if compiler == 'vc9':
			prefix = 'vc9-Win32'
			setenv_rel = 'call "' + vc9Base + r'\VC\vcvarsall.bat" x86'

		elif compiler == 'vc9x64':
			prefix = 'vc9-x64'
			setenv_rel = 'call "' + vc9Base + r'\VC\vcvarsall.bat" '
			if not bit64on64:
				setenv_rel += 'x86_amd64'
			else:
				setenv_rel += 'amd64'

		elif compiler == 'vc10':
			prefix = 'vc100-Win32'
			setenv_rel = 'call "' + vc10Base + r'\VC\vcvarsall.bat" x86'

		elif compiler == 'vc10x64':
			prefix = 'vc100-x64'
			setenv_rel = 'call "' + vc10Base + r'\VC\vcvarsall.bat" '
			if not bit64on64:
				setenv_rel += 'x86_amd64'
			else:
				setenv_rel += 'amd64'

		elif compiler == 'vc11':
			prefix = 'vc110-Win32'
			setenv_rel = 'call "' + vc11Base + r'\VC\vcvarsall.bat" x86'

		elif compiler == 'vc11x64':
			prefix = 'vc110-x64'
			setenv_rel = 'call "' + vc11Base + r'\VC\vcvarsall.bat" '
			if not bit64on64:
				setenv_rel += 'x86_amd64'
			else:
				setenv_rel += 'amd64'

		elif compiler == 'vc12':
			prefix = 'vc120-Win32'
			setenv_rel = 'call "' + vc12Base + r'\VC\vcvarsall.bat" x86'

		elif compiler == 'vc12x64':
			prefix = 'vc120-x64'
			setenv_rel = 'call "' + vc12Base + r'\VC\vcvarsall.bat" '
			# at least for vc12-express, there is no 'amd64' target
			setenv_rel += 'x86_amd64'

		prefix_rel = ' prefix=build\\' + prefix + r'\Release'
		prefix_dbg = ' prefix=build\\' + prefix + r'\Debug'

		config = 'cscript configure.js ' + common_config

		# There's some weird timing issue here where the 2nd compile will fail
		# if run too quickly. It's like the files are deleted asynchronously.
		# Putting in a sleep for 3 seconds appears to fix this.
		#for i in (1,2,3,4):
		for i in (3,4):
			bat = open(tmpfile, 'w')
			print >>bat, 'title ' + compiler
			if len(echo) > 0:
				print >>bat, '@echo off'
			print >>bat, echo + setenv_rel

			if i == 1:
				print >>bat, echo + config + prefix_rel + 'DLL cruntime=/MD'
				print >>bat, echo + 'nmake -f makefile.msvc rebuild install'
				print >>bat, echo + 'nmake -f makefile.msvc clean'
				print >>bat, echo + 'xcopy /y/q/s build\\' + prefix + '\\ReleaseDLL\\include\\libxml2\\* ..\\..\\include\\' 
				print >>bat, echo + 'xcopy /y build\\' + prefix + '\\ReleaseDLL\\bin\\libxml2.dll ..\\..\\lib\\' + prefix + '\\Release\\'
				print >>bat, echo + 'xcopy /y build\\' + prefix + '\\ReleaseDLL\\lib\\libxml2.lib ..\\..\\lib\\' + prefix + '\\Release\\'

			elif i == 2:
				print >>bat, echo + config + prefix_dbg + 'DLL debug=yes cruntime=/MDd'
				print >>bat, echo + 'nmake -f makefile.msvc rebuild install'
				print >>bat, echo + 'nmake -f makefile.msvc clean'
				print >>bat, echo + 'xcopy /y/q/s build\\' + prefix + '\\DebugDLL\\include\\libxml2\\* ..\\..\\include\\' 
				print >>bat, echo + 'xcopy /y build\\' + prefix + '\\DebugDLL\\bin\\libxml2.dll ..\\..\\lib\\' + prefix + '\\Debug\\'
				print >>bat, echo + 'xcopy /y build\\' + prefix + '\\DebugDLL\\lib\\libxml2.lib ..\\..\\lib\\' + prefix + '\\Debug\\'

			elif i == 3:
				print >>bat, echo + config + prefix_rel + 'LIB cruntime=/MT'
				print >>bat, echo + 'nmake -f makefile.msvc rebuild install'
				print >>bat, echo + 'nmake -f makefile.msvc clean'
				print >>bat, echo + 'xcopy /y/q/s build\\' + prefix + '\\ReleaseLIB\\include\\libxml2\\* ..\\..\\include\\' 
				print >>bat, echo + 'xcopy /y build\\' + prefix + '\\ReleaseLIB\\lib\\libxml2_a.lib ..\\..\\lib\\' + prefix + '\\Release\\'

			elif i == 4:
				print >>bat, echo + config + prefix_dbg + 'LIB debug=yes cruntime=/MTd'
				print >>bat, echo + 'nmake -f makefile.msvc rebuild install'
				print >>bat, echo + 'nmake -f makefile.msvc clean'
				print >>bat, echo + 'xcopy /y/q/s build\\' + prefix + '\\DebugLIB\\include\\libxml2\\* ..\\..\\include\\' 
				print >>bat, echo + 'xcopy /y build\\' + prefix + '\\DebugLIB\\lib\\libxml2_a.lib ..\\..\\lib\\' + prefix + '\\Debug\\'

			bat.close()
			proc = subprocess.Popen(tmpfile, env=newenv)
			proc.wait()
			os.remove(tmpfile)
			time.sleep(3)

	return 0


if __name__ == '__main__':
	sys.exit(main())
