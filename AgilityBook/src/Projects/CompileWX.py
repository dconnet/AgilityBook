# coding=utf-8
# Above line is for python
#
# This is my quick-and-easy way to compile wxWidgets on Windows.
#
# Note: XP is not supported for VS2012+
# http://blogs.msdn.com/b/vcblog/archive/2012/10/08/10357555.aspx?PageIndex=3
# (setting up the projects are easy - command line is more involved)
# However, compiling WX as-is and setting the projects does appear to create
# an EXE that will run on XP.
#
# Revision History
# 2017-01-24 Changed GetCompilerPaths api.
# 2016-11-22 Added vc15, removed vc9. Changed GetCompilerPaths api.
# 2016-06-10 Convert to Python3
# 2015-10-11 Added -r option.
# 2015-04-24 Added vc14.
# 2013-10-14 Allow x64-on-x64 compilation to fall back to x86_amd (VCExpress)
# 2013-07-30 Modify tmpfile to allow multiple builds
#            (a VM can now build vc12 and the same time vc11 builds locally)
# 2013-07-01 Added VC12 support. Removed _BIND_TO... for VC10+.
# 2012-07-07 wx2.9 added x64 target - use that instead of amd64
# 2012-06-01 Added VC11 support.
# 2011-07-30 Require -w option.
# 2010-11-20 Automatically determine hasprefix support.
# 2010-11-07 Removed DEBUG_FLAG from build - set in setup.h instead.
# 2010-10-19 Remove all debug info from release build
# 2010-10-16 Added -w option
# 2010-07-17 Convert to VC10Pro (no need for SDK now)
# 2010-05-30 Add '/xp' option to vc10 setenv.cmd
# 2010-05-29 Converted .bat to .py
# 2009-12-31 Changed 'hasprefix' to 'noprefix'.
#            (2.8 is rarely rebuilt, default to the active build)
# 2009-09-26 Tweak compile options
# 2009-09-12 Fix dll creation
"""CompileWX.py -w wxwin [-e] [-a] [-p] [-d] [-m] [-s name]* [-r config] compiler*
	-w wxwin: Root of wx tree, normally %WXWIN%
	-e:       Just show the environment, don't do it
	-a:       Compile all (vc10, vc10x64)
	-d:       Compile as DLLs (default: static)
	-m:       Compile as MBCS (default: Unicode)
	-s name:  Compile sample 'name'
	-r config: config: release/debug
	compiler: vc10, vc10x64, vc11, vc11x64, vc12, vc12x64, vc14, vc14x64, vc15, vc15x64
"""

import getopt
import glob
import os
import string
import subprocess
import sys
import win32api
import win32con

tmpfile = 'tmpcomp' + os.environ['USERDOMAIN'] + '.bat'

compileIt = True
hasPrefix = True
useStatic = True
useUnicode = True


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


# Return SDK path (with ending slash)
def GetWindowsSdkDir():
	WindowsSdkDir = GetRegString(win32con.HKEY_LOCALMACHINE, r'SOFTWARE\Microsoft\Microsoft SDKs\Windows', 'CurrentInstallFolder')
	if 0 == len(WindowsSdkDir):
		WindowsSdkDir = GetRegString(win32con.HKEY_CURRENT_USER, r'SOFTWARE\Microsoft\Microsoft SDKs\Windows', 'CurrentInstallFolder')
	return WindowsSdkDir


# 7.1, 8.0, 9.0, 10.0, 11.0, 12.0, 14.0, 15.0 (as observed on my machine)
def GetVSDir(version):
	vsdir = GetRegString(win32con.HKEY_LOCAL_MACHINE, r'SOFTWARE\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir):
		vsdir = GetRegString(win32con.HKEY_CURRENT_USER, r'SOFTWARE\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir):
		vsdir = GetRegString(win32con.HKEY_LOCAL_MACHINE, r'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir):
		vsdir = GetRegString(win32con.HKEY_CURRENT_USER, r'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7', version)
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
	config = ''

	if c == 'vc10':
		baseDir = GetVSDir("10.0")
		vcvarsall = baseDir + r'\VC\vcvarsall.bat'
		target = 'x86'
		platformDir = 'vc100'
		config = 'Win32'

	elif c == 'vc10x64':
		baseDir = GetVSDir("10.0")
		vcvarsall = baseDir + r'\VC\vcvarsall.bat'
		target = GetX64Target(baseDir)
		platformDir = 'vc100'
		config = 'x64'

	elif c == 'vc11':
		baseDir = GetVSDir("11.0")
		vcvarsall = baseDir + r'\VC\vcvarsall.bat'
		target = 'x86'
		platformDir = 'vc110'
		config = 'Win32'

	elif c == 'vc11x64':
		baseDir = GetVSDir("11.0")
		vcvarsall = baseDir + r'\VC\vcvarsall.bat'
		target = GetX64Target(baseDir)
		platformDir = 'vc110'
		config = 'x64'

	elif c == 'vc12':
		baseDir = GetVSDir("12.0")
		vcvarsall = baseDir + r'\VC\vcvarsall.bat'
		target = 'x86'
		platformDir = 'vc120'
		config = 'Win32'

	elif c == 'vc12x64':
		baseDir = GetVSDir("12.0")
		vcvarsall = baseDir + r'\VC\vcvarsall.bat'
		target = GetX64Target(baseDir)
		platformDir = 'vc120'
		config = 'x64'

	elif c == 'vc14':
		baseDir = GetVSDir("14.0")
		vcvarsall = baseDir + r'\VC\vcvarsall.bat'
		target = 'x86'
		platformDir = 'vc140'
		config = 'Win32'

	elif c == 'vc14x64':
		baseDir = GetVSDir("14.0")
		vcvarsall = baseDir + r'\VC\vcvarsall.bat'
		target = GetX64Target(baseDir)
		platformDir = 'vc140'
		config = 'x64'

	elif c == 'vc15':
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
		config = 'Win32'

	elif c == 'vc15x64':
		baseDir = GetVSDir("15.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetX64Target(baseDir)
		platformDir = 'vc141'
		config = 'x64'

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

	return (baseDir, '"' + vcvarsall + '" ' + target + extraargs, platformDir, config)


def AddCompiler(compilers, c):
	vcBaseDir, vcvarsall, platformDir, config = GetCompilerPaths(c)

	if len(vcBaseDir) == 0:
		return False

	# Sanity test
	if c == 'vc10x64':
		if not useUnicode:
			print('ERROR: VC10x64 does not do MBCS')
			return False

	elif c == 'vc11x64':
		if not useUnicode:
			print('ERROR: VC11x64 does not do MBCS')
			return False

	elif c == 'vc12x64':
		if not useUnicode:
			print('ERROR: VC12x64 does not do MBCS')
			return False

	elif c == 'vc14x64':
		if not useUnicode:
			print('ERROR: VC14x64 does not do MBCS')
			return False

	elif c == 'vc15x64':
		if not useUnicode:
			print('ERROR: VC15x64 does not do MBCS')
			return False

	compilers.add(c)
	return True


def getversion(file):
	numParts = 4
	ver = '0'
	ver2 = '0'
	for i in range(1, numParts):
		ver = ver + '.0'
		ver2 = ver2 + '_0'
	resStr = [
		'#define wxMAJOR_VERSION',
		'#define wxMINOR_VERSION',
		'#define wxRELEASE_NUMBER',
		'#define wxSUBRELEASE_NUMBER'
		]
	found = 0;
	version = ['0', '0', '0', '0']
	res = open(file, 'r')
	while (1):
		line = res.readline()
		if line:
			line = line.strip()
			for i in range(0, 4):
				pos = line.find(resStr[i])
				if 0 == pos:
					found = found + 1
					version[i] = line[pos+len(resStr[i]):].strip()
			if found == 4:
				break
		else:
			break
	return version


def main():
	global compileIt, hasPrefix, useStatic, useUnicode

	wxwin = ''
	samples = set()
	compilers = set()
	debug = True
	release = True
	try:
		print(sys.argv)
		opts, args = getopt.getopt(sys.argv[1:], 'w:eadms:r:')
		print(opts,args)
	except getopt.error as msg:
		print(msg)
		print('Usage:', __doc__)
		return 1
	for o, a in opts:
		if '-e' == o:
			compileIt = False
		elif '-w' == o:
			wxwin = a
		elif '-a' == o:
			AddCompiler(compilers, 'vc10')
			AddCompiler(compilers, 'vc10x64')
		elif '-d' == o:
			useStatic = False
		elif '-m' == o:
			useUnicode = False
		elif '-s' == o:
			samples.add(a)
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
	if len(wxwin) == 0:
		print('ERROR: -w option not specified')
		print('Usage:', __doc__)
		return 1
	os.environ['WXWIN'] = wxwin

	if 'WXWIN' not in os.environ:
		print('ERROR: WXWIN environment variable is not set')
		return 1

	if not os.access(os.environ['WXWIN'], os.F_OK):
		print('ERROR: ' + os.environ['WXWIN'] + ' doesn\'t exist')
		return 1

	wxInclude = os.environ['WXWIN'] + r'\include\wx\version.h'
	if not os.access(wxInclude, os.F_OK):
		print('ERROR: ' + wxInclude + ' doesn\'t exist')
		return 1
	version = getversion(wxInclude)
	if version[0] == '2' and version[1] == '8':
		hasPrefix = False

	x64Target = 'x64'
	if not hasPrefix:
		x64Target = 'amd64'

	for c in args:
		if not AddCompiler(compilers, c):
			print('Usage:', __doc__)
			return 1

	if 0 == len(compilers):
		print('Usage:', __doc__)
		return 1

	vendor = ''
	if not useStatic:
		vendor = ' VENDOR=dconsoft'

	# Used in wx2.9.1+, not used earlier so won't hurt anything
	common_cppflags = '/DwxMSVC_VERSION_AUTO=1'

	os.environ['INCLUDE'] = ''
	os.environ['LIB'] = ''

	if 0 == len(samples):
		os.chdir(os.environ['WXWIN'] + r'\build\msw')

	for compiler in compilers:
		vcBaseDir, vcvarsall, platformDir, config = GetCompilerPaths(compiler)

		newenv = os.environ.copy()

		bat = open(tmpfile, 'w')
		print('title ' + compiler, file=bat)

		# rel will be called first, so if no different, just set that.
		setenv_rel = ''
		setenv_dbg = ''
		cfg = ''
		target_cpu = ''
		cppflags = ''
		resetColor = False

		# Note: Look into using COMPILER_VERSION instead of COMPILER_PREFIX
		if hasPrefix:
			cfg = ' COMPILER_PREFIX=' + platformDir
		else:
			cfg = ' CFG=_' + platformDir

		setenv_rel = 'call ' + vcvarsall
		cppflags = common_cppflags
		if config == 'x64':
			target_cpu = ' TARGET_CPU=' + x64Target

		build_rel = ''
		build_dbg = ''
		if not compileIt:
			build_rel += 'echo '
			build_dbg += 'echo '
		build_rel += 'nmake -f makefile.vc BUILD=release'
		build_dbg += 'nmake -f makefile.vc BUILD=debug'
		# In 2.9, DEBUG_FLAG defaults to 1 (hasprefix infers wx3.0 or active
		# trunk) Set to '2' for more debugging (datepicker asserts alot tho)
		# Note: Setting DEBUG_FLAG here sets wxDEBUG_LEVEL during the library
		# compile but has no effect on users of the library - make sure
		# wx/msw/setup.h is set. (This is not 'configure'!)
		if hasPrefix:
			build_rel += ' DEBUG_INFO=0'
		else:
			build_dbg += ' DEBUG_INFO=1'
		build_flags = ' UNICODE='
		if useUnicode:
			build_flags += '1'
		else:
			build_flags += '0'
		build_flags += ' SHARED='
		if useStatic:
			build_flags += '0 RUNTIME_LIBS=static'
		else:
			build_flags += '1 RUNTIME_LIBS=dynamic'
		build_flags += target_cpu + cfg + ' CPPFLAGS="' + cppflags + '"' + vendor

		if 0 < len(samples):
			for s in samples:
				if os.access(os.environ['WXWIN'] + '\\samples\\' + s, os.F_OK):
					print('cd /d "' + os.environ['WXWIN'] + '\\samples\\' + s + '"', file=bat)
					if release:
						print(setenv_rel, file=bat)
						print(build_rel + ' ' + build_flags, file=bat)
					if debug:
						if 0 < len(setenv_dbg):
							print(setenv_dbg, file=bat)
						elif not release:
							print(setenv_rel, file=bat)
						print(build_dbg + ' ' + build_flags, file=bat)
				else:
					print('ERROR: sample "' + s + '" does not exist')
		else:
			if release:
				print(setenv_rel, file=bat)
				print(build_rel + ' ' + build_flags, file=bat)
			if debug:
				if 0 < len(setenv_dbg):
					print(setenv_dbg, file=bat)
				elif not release:
					print(setenv_rel, file=bat)
				print(build_dbg + ' ' + build_flags, file=bat)
		if resetColor:
			print('color 07', file=bat)

		bat.close()
		if compileIt:
			proc = subprocess.Popen(tmpfile, env=newenv)
		else:
			proc = subprocess.Popen('cmd /c type ' + tmpfile, env=newenv)
		proc.wait()
		os.remove(tmpfile)

	return 0


if __name__ == '__main__':
	sys.exit(main())
