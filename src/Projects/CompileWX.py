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
# 2019-02-28 Add vc142 support
# 2018-11-16 Added ARM64 support.
# 2018-10-06 Dropping support for pre VS2017 (and XP).
# 2018-01-27 Fix vcvarsall now changing directory.
# 2017-09-19 Rename vc15 to vc141, fix GetCompilerPaths tuple name
# 2017-01-24 Changed GetCompilerPaths api.
# 2016-11-22 Added vc141, removed vc9. Changed GetCompilerPaths api.
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
	-a:       Compile all (vc141, vc141x64)
	-d:       Compile as DLLs (default: static)
	-m:       Compile as MBCS (default: Unicode)
	-s name:  Compile sample 'name'
	-r config: config: release/debug
	compiler: vc141, vc141x64, vc141arm64
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


# Return vcvarsall target
def GetTarget(vcBase, bIs64Bit, bIsARM):
	# 64bit on 64bit
	b64On64 = False
	if 'PROCESSOR_ARCHITECTURE' in os.environ and os.environ['PROCESSOR_ARCHITECTURE'] == 'AMD64':
		# Note: We used to check for the existence of <vcBase>\VC\bin\amd64.
		# VS2017 moved that directory. Just assume that if we're compiling
		# for 64bit on 64bit that the user installed that. With current VS,
		# that's just done - not like older versions where it was a choice.
		b64On64 = True

	target = ''

	if bIs64Bit and bIsARM:
		if b64On64:
			target = 'amd64_arm64'
		else:
			target = 'x86_arm64'

	elif bIs64Bit and not bIsARM:
		if b64On64:
			target = 'amd64'
		else:
			target = 'x86_amd64'

	elif not bIs64Bit and bIsARM:
		if b64On64:
			target = 'amd64_arm'
		else:
			target = 'x86_arm'

	elif not bIs64Bit and not bIsARM:
		if b64On64:
			target = 'amd64_x86'
		else:
			target = 'x86'

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
		target = GetTarget(baseDir, False, False)
		# Can target specific SDKs
		#extraargs = ' 10.0.14393.0'
		platformDir = 'vc141'
		platform = 'Win32'

	elif c == 'vc141x64':
		baseDir = GetVSDir("15.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetTarget(baseDir, True, False)
		platformDir = 'vc141'
		platform = 'x64'

	elif c == 'vc141arm64':
		baseDir = GetVSDir("15.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetTarget(baseDir, True, True)
		platformDir = 'vc141'
		platform = 'ARM64'

	elif c == 'vc142':
		baseDir = GetVSDir("16.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetTarget(baseDir, False, False)
		platformDir = 'vc142'
		platform = 'Win32'

	elif c == 'vc142x64':
		baseDir = GetVSDir("16.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetTarget(baseDir, True, False)
		platformDir = 'vc142'
		platform = 'x64'

	elif c == 'vc142arm64':
		baseDir = GetVSDir("16.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetTarget(baseDir, True, True)
		platformDir = 'vc142'
		platform = 'ARM64'

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


def AddCompiler(compilers, c):
	vcBaseDir, vcvarsall, platformDir, platform = GetCompilerPaths(c)

	if len(vcBaseDir) == 0:
		return False

	# Sanity test
	if c == 'vc141x64':
		if not useUnicode:
			print('ERROR: VC141x64 does not do MBCS')
			return False

	compilers.add(c)
	return True


def main():
	global compileIt, useStatic, useUnicode

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
			AddCompiler(compilers, 'vc141')
			AddCompiler(compilers, 'vc141x64')
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
		vcBaseDir, vcvarsall, platformDir, platform = GetCompilerPaths(compiler)

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
		cfg = ' COMPILER_PREFIX=' + platformDir

		setenv_rel = 'call ' + vcvarsall
		cppflags = common_cppflags
		if platform == 'x64':
			target_cpu = ' TARGET_CPU=x64'
		elif platform == 'ARM64':
			target_cpu = ' TARGET_CPU=ARM64'

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
		build_rel += ' DEBUG_INFO=0'
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
					print('set "VSCMD_START_DIR=%CD%"', file=bat)
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
			print('set "VSCMD_START_DIR=%CD%"', file=bat)
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
