# coding=utf-8
# Above line is for python
#
# This is my quick-and-easy way to compile wxWidgets on Windows.
# It assumes the default install location of c:\progfiles
#
# Revision History
# 2010-05-29 DRC Converted .bat to .py
# 2009-12-31 DRC Changed 'hasprefix' to 'noprefix'.
#                (2.8 is rarely rebuilt, default to the active build)
# 2009-09-26 DRC Tweak compile options
# 2009-09-12 DRC Fix dll creation
"""CompileWX.py [-e] [-a] [-p] [-d] [-m] [-s name]* compiler*
	-e:       Just show the environment, don't do it
	-a:       Compile all (vc9, vc9x64)
	-p:       No prefix (2.8.x and before) (default: hasPrefix)
	-d:       Compile as DLLs (default: static)
	-m:       Compile as MBCS (default: Unicode)
	-s name:  Compile sample 'name'
	compiler: vc7, vc8, vc9, vc10, vc9x64, vc10x64
"""

import getopt
import glob
import os
import string
import subprocess
import sys
import win32api
import win32con

tmpfile = 'tmpcomp.bat'

ProgramFiles = r'c:\Program Files'
ProgramFiles64 = r'c:\Program Files'

compileIt = True
hasPrefix = True
useStatic = True
useUnicode = True

vc6Base     = ProgramFiles + r'\Microsoft Visual Studio'
vc7Base     = r'\Microsoft Visual Studio .NET 2003'
vc8Base     = r'\Microsoft Visual Studio 8'
vc9Base     = r'\Microsoft Visual Studio 9.0'
vc10Base    = r'\Microsoft Visual Studio 10.0'


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
	global vc6Base, vc7Base, vc8Base, vc9Base, vc10Base
	baseDir = ''
	testFile = ''
	if c == 'vc6':
		if useUnicode:
			print 'ERROR: VC6 does not do UNICODE'
			return False
		if useStatic:
			print 'ERROR: VC6 does not do static libraries'
			return False
		baseDir = vc6Base
		testFile = baseDir + r'\VC98\bin\vcvars32.bat'
	elif c == 'vc7':
		if useUnicode:
			print 'ERROR: VC7 does not do UNICODE'
			return False
		vc7Base = GetVSDir("7.1")
		baseDir = vc7Base
		testFile = baseDir + r'\Common7\Tools\vsvars32.bat'
	elif c == 'vc8':
		vc8Base = GetVSDir("8.0")
		baseDir = vc8Base
		testFile = baseDir + r'\VC\vcvarsall.bat'
	elif c == 'vc9':
		vc9Base = GetVSDir("9.0")
		baseDir = vc9Base
		testFile = baseDir + r'\VC\vcvarsall.bat'
	elif c == 'vc9x64':
		vc9Base = GetVSDir("9.0")
		if not useUnicode:
			# Well, it might. I don't.
			print 'ERROR: VC9x64 does not do MBCS'
			return False
		baseDir = vc9Base
		testFile = baseDir + r'\VC\vcvarsall.bat'
	elif c == 'vc10':
		vc10Base = GetVSDir("10.0")
		baseDir = vc10Base
		testFile = baseDir + r'\VC\vcvarsall.bat'
	elif c == 'vc10x64':
		vc10Base = GetVSDir("10.0")
		if not useUnicode:
			print 'ERROR: VC10x64 does not do MBCS'
			return False
		baseDir = vc10Base
		# Doesn't matter what the "current" sdk is. We must have 7.1.
		testFile = ProgramFiles + r'\Microsoft SDKs\Windows\v7.1\bin\setenv.cmd'
	else:
		return False
	if not os.access(baseDir, os.F_OK) or not os.access(testFile, os.F_OK):
		print 'ERROR: "' + baseDir + '" does not exist'
		return False
	compilers.add(c)
	return True


def main():
	global ProgramFiles, ProgramFiles64
	global compileIt, hasPrefix, useStatic, useUnicode

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

	if not os.environ.has_key('WXWIN'):
		print 'ERROR: WXWIN environment variable is not set'
		return

	if not os.access(os.environ['WXWIN'], os.F_OK):
		print 'ERROR: ' + os.environ['WXWIN'] + ' doesn\'t exist'
		return

	samples = set()
	compilers = set()
	try:
		opts, args = getopt.getopt(sys.argv[1:], 'eapdms:')
	except getopt.error, msg:
		print msg
		print 'Usage:', __doc__
		return
	for o, a in opts:
		if '-e' == o:
			compileIt = False
		elif '-a' == o:
			AddCompiler(compilers, 'vc9')
			AddCompiler(compilers, 'vc9x64')
		elif '-p' == o:
			hasPrefix = False
		elif '-d' == o:
			useStatic = False
		elif '-m' == o:
			useUnicode = False
		elif '-s' == o:
			samples.add(a)

	for c in args:
		if not AddCompiler(compilers, c):
			print 'Unknown compiler:', c
			print 'Usage:', __doc__
			return

	if 0 == len(compilers):
		print 'Usage:', __doc__
		return

	vendor = ''
	if not useStatic:
		vendor = 'VENDOR=dconsoft'

	# Used in wx2.9.1+, not used earlier so won't hurt anything
	common_cppflags = '/DwxMSVC_VERSION_AUTO=1'

	#compileIt = True
	#useStatic = True
	#useUnicode = True
	#samples = set()
	#compilers = set()

	os.environ['INCLUDE'] = ''
	os.environ['LIB'] = ''

	if 0 == len(samples):
		os.chdir(os.environ['WXWIN'] + r'\build\msw')

	for compiler in compilers:
		newenv = os.environ.copy()

		bat = open(tmpfile, 'w')
		print >>bat, 'title ' + compiler

		# rel will be called first, so if no different, just set that.
		setenv_rel = ''
		setenv_dbg = ''
		cfg = ''
		target_cpu = ''
		cppflags = ''

		if compiler == 'vc6':
			setenv_rel = 'call "' + vc6Base + r'\VC98\bin\vcvars32.bat"'
			if hasPrefix:
				cfg = 'COMPILER_PREFIX=vc60 '
			else:
				cfg = 'CFG=_VC60 '
			cppflags = common_cppflags

		elif compiler == 'vc7':
			setenv_rel = 'call "' + vc7Base + r'\Common7\Tools\vsvars32.bat"'
			if hasPrefix:
				cfg = 'COMPILER_PREFIX=vc71 '
			else:
				cfg = 'CFG=_VC71 '
			cppflags = common_cppflags

		elif compiler == 'vc8':
			setenv_rel = 'call "' + vc8Base + r'\VC\vcvarsall.bat"'
			if hasPrefix:
				cfg = 'COMPILER_PREFIX=vc80 '
			else:
				cfg = 'CFG=_VC80 '
			cppflags = common_cppflags

		elif compiler == 'vc9':
			setenv_rel = 'call "' + vc9Base + r'\VC\vcvarsall.bat" x86'
			if hasPrefix:
				cfg = 'COMPILER_PREFIX=vc90 '
			else:
				cfg = 'CFG=_VC90 '
			cppflags = common_cppflags + r' /D_SECURE_SCL=1 /D_SECURE_SCL_THROWS=1 /D_BIND_TO_CURRENT_VCLIBS_VERSION=1'

		elif compiler == 'vc9x64':
			setenv_rel = 'call "' + vc9Base + r'\VC\vcvarsall.bat" '
			if not bit64on64:
				setenv_rel += 'x86_amd64'
			else:
				setenv_rel += 'amd64'
			target_cpu = 'TARGET_CPU=amd64 '
			if hasPrefix:
				cfg = 'COMPILER_PREFIX=vc90 '
			else:
				cfg = 'CFG=_VC90 '
			cppflags = common_cppflags + r' /D_SECURE_SCL=1 /D_SECURE_SCL_THROWS=1 /D_BIND_TO_CURRENT_VCLIBS_VERSION=1'

		elif compiler == 'vc10':
			setenv_rel = 'call "' + vc10Base + r'\VC\vcvarsall.bat" x86'
			if hasPrefix:
				cfg = 'COMPILER_PREFIX=vc100 '
			else:
				cfg = 'CFG=_VC100 '
			cppflags = common_cppflags + r' /D_BIND_TO_CURRENT_VCLIBS_VERSION=1'

		elif compiler == 'vc10x64':
			setenv_rel = ProgramFiles + r'\Microsoft SDKs\Windows\v7.1\bin\setenv.cmd /release /x64'
			setenv_dbg = ProgramFiles + r'\Microsoft SDKs\Windows\v7.1\bin\setenv.cmd /debug /x64'
			target_cpu = 'TARGET_CPU=amd64 '
			if hasPrefix:
				cfg = 'COMPILER_PREFIX=vc100 '
			else:
				cfg = 'CFG=_VC100 '
			cppflags = common_cppflags + r' /D_BIND_TO_CURRENT_VCLIBS_VERSION=1'

		build_rel = ''
		build_dbg = ''
		if not compileIt:
			build_rel += 'echo '
			build_dbg += 'echo '
		build_rel += 'nmake -f makefile.vc BUILD=release'
		build_dbg += 'nmake -f makefile.vc BUILD=debug DEBUG_INFO=1'
		# In 2.9, wxDEBUG_LEVEL defaults to 1 (hasprefix infers the active wx
		# trunk) Set to '2' for more debugging (datepicker asserts alot tho)
		if hasPrefix:
			build_rel += ' wxDEBUG_LEVEL=0'
			build_dbg += ' wxDEBUG_LEVEL=1'
		build_flags = 'UNICODE='
		if useUnicode:
			build_flags += '1'
		else:
			build_flags += '0'
		build_flags += ' SHARED='
		if useStatic:
			build_flags += '0 RUNTIME_LIBS=static '
		else:
			build_flags += '1 RUNTIME_LIBS=dynamic '
		build_flags += target_cpu + cfg + 'CPPFLAGS="' + cppflags + '" ' + vendor

		if 0 < len(samples):
			for s in samples:
				if os.access(os.environ['WXWIN'] + '\\samples\\' + s, os.F_OK):
					print >>bat, 'cd /d "' + os.environ['WXWIN'] + '\\samples\\' + s + '"'
					print >>bat, setenv_rel
					print >>bat, build_rel + ' ' + build_flags
					print >>bat, setenv_dbg
					print >>bat, build_dbg + ' ' + build_flags
				else:
					print 'ERROR: sample "' + s + '" does not exist'
		else:
			print >>bat, setenv_rel
			print >>bat, build_rel + ' ' + build_flags
			print >>bat, setenv_dbg
			print >>bat, build_dbg + ' ' + build_flags

		bat.close()
		if compileIt:
			proc = subprocess.Popen(tmpfile, env=newenv)
		else:
			proc = subprocess.Popen('cmd /c type ' + tmpfile, env=newenv)
		proc.wait()
		os.remove(tmpfile)


main()
