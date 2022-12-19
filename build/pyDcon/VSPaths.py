# coding=utf-8
# Above line is for python
#
# GetCompilerPaths(c)
#  c: vc9, vc9x64, etc...
#  returns tuple (vcDir, vcvarsall cmd, platformDir, platform)
#    baseDir, baseDir+r'\VC\vcvarsall.bat target', vcNNN, x64/x86
#
# 2022-12-19 Added vc143arm support.
# 2021-11-09 Add vc143 support
# 2020-11-28 Make target names case insensitive.
# 2020-09-13 Changed Win32 target to x86
# 2019-02-28 Add vc142 support
# 2018-11-16 Add ARM support
# 2017-09-19 Rename vc15 to vc141, fix GetCompilerPaths tuple name
# 2017-04-07 Reverted after installing 15063 SDK (didn't happen in VS update)
#            Fixed GetX64Target to work with vs2017.
# 2017-04-06 Added 10.0.14393.0 SDK to VS2017 env (for now).
# 2017-01-24 Added platform into return tuple.
# 2016-11-22 Added vc141 support, removed vc9, added platformDir to return tuple
# 2016-06-10 Made into library
#

from .GetVSDir import GetVSDir
import os
import sys


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

	# Note: These targets could change 'amd64' to 'x64', but I forget which
	# compiler added 'x64'. So use the old name for old compiler compatibility.
	if bIs64Bit:
		if bIsARM:
			if b64On64:
				target = 'amd64_arm64'
			else:
				target = 'x86_arm64'
		else:
			if b64On64:
				target = 'amd64'
			else:
				target = 'x86_amd64'

	elif not bIs64Bit:
		if bIsARM:
			if b64On64:
				target = 'amd64_arm'
			else:
				target = 'x86_arm'
		else:
			if b64On64:
				target = 'amd64_x86'
			else:
				target = 'x86'

	return target


# platform is one of: x86, x64, ARM, ARM64
def GetCompilerPaths(c, verbose = True):
	baseDir = ''
	vcvarsall = ''
	target = ''
	extraargs = ''
	platformDir = ''
	platform = ''

	comp = c.lower()
	if comp == 'vc10':
		baseDir = GetVSDir("10.0")
		vcvarsall = baseDir + r'\VC\vcvarsall.bat'
		target = GetTarget(baseDir, False, False)
		platformDir = 'vc100'
		platform = 'x86'

	elif comp == 'vc10x64':
		baseDir = GetVSDir("10.0")
		vcvarsall = baseDir + r'\VC\vcvarsall.bat'
		target = GetTarget(baseDir, True, False)
		platformDir = 'vc100'
		platform = 'x64'

	elif comp == 'vc11':
		baseDir = GetVSDir("11.0")
		vcvarsall = baseDir + r'\VC\vcvarsall.bat'
		target = GetTarget(baseDir, False, False)
		platformDir = 'vc110'
		platform = 'x86'

	elif comp == 'vc11x64':
		baseDir = GetVSDir("11.0")
		vcvarsall = baseDir + r'\VC\vcvarsall.bat'
		target = GetTarget(baseDir, True, False)
		platformDir = 'vc110'
		platform = 'x64'

	elif comp == 'vc12':
		baseDir = GetVSDir("12.0")
		vcvarsall = baseDir + r'\VC\vcvarsall.bat'
		target = GetTarget(baseDir, False, False)
		platformDir = 'vc120'
		platform = 'x86'

	elif comp == 'vc12x64':
		baseDir = GetVSDir("12.0")
		vcvarsall = baseDir + r'\VC\vcvarsall.bat'
		target = GetTarget(baseDir, True, False)
		platformDir = 'vc120'
		platform = 'x64'

	elif comp == 'vc14':
		baseDir = GetVSDir("14.0")
		vcvarsall = baseDir + r'\VC\vcvarsall.bat'
		target = GetTarget(baseDir, False, False)
		platformDir = 'vc140'
		platform = 'x86'

	elif comp == 'vc14x64':
		baseDir = GetVSDir("14.0")
		vcvarsall = baseDir + r'\VC\vcvarsall.bat'
		target = GetTarget(baseDir, True, False)
		platformDir = 'vc140'
		platform = 'x64'

	elif comp == 'vc141':
		#vcvarsall [arch]
		#vcvarsall [arch] [version]
		#vcvarsall [arch] [platform_type] [version]
		# [arch]      Compiler            Host       Output
		# x86         x86 32-bit native   x86, x64   x86
		# x86_amd64   x64 on x86 cross    x86, x64   x64
		# x86_x64     x64 on x86 cross    x86, x64   x64
		# x86_arm     ARM on x86 cross    x86, x64   ARM
		# x86_arm64   ARM64 on x86 cross  x86, x64   ARM64
		# amd64       x64 64-bit native   x64        x64
		# x64         x64 64-bit native   x64        x64
		# amd64_x86   x86 on x64 cross    x64        x86
		# x64_x86     x86 on x64 cross    x64        x86
		# amd64_arm   ARM on x64 cross    x64        ARM
		# x64_arm     ARM on x64 cross    x64        ARM
		# amd64_arm64 ARM64 on x64 cross  x64        ARM64
		# x64_arm64   ARM64 on x64 cross  x64        ARM64
		# [platform_type]: {empty} | store | uwp
		# [version] : full Windows 10 SDK number (e.g. 10.0.10240.0) or "8.1" to use the Windows 8.1 SDK.
		baseDir = GetVSDir("15.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetTarget(baseDir, False, False)
		# Can target specific SDKs
		#extraargs = ' 10.0.14393.0'
		platformDir = 'vc141'
		platform = 'x86'

	elif comp == 'vc141x64':
		baseDir = GetVSDir("15.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetTarget(baseDir, True, False)
		platformDir = 'vc141'
		platform = 'x64'

	elif comp == 'vc141arm64':
		baseDir = GetVSDir("15.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetTarget(baseDir, True, True)
		platformDir = 'vc141'
		platform = 'ARM64'

	elif comp == 'vc142':
		baseDir = GetVSDir("16.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetTarget(baseDir, False, False)
		platformDir = 'vc142'
		platform = 'x86'

	elif comp == 'vc142x64':
		baseDir = GetVSDir("16.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetTarget(baseDir, True, False)
		platformDir = 'vc142'
		platform = 'x64'

	elif comp == 'vc142arm64':
		baseDir = GetVSDir("16.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetTarget(baseDir, True, True)
		platformDir = 'vc142'
		platform = 'ARM64'

	elif comp == 'vc143':
		baseDir = GetVSDir("17.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetTarget(baseDir, False, False)
		platformDir = 'vc143'
		platform = 'x86'

	elif comp == 'vc143x64':
		baseDir = GetVSDir("17.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetTarget(baseDir, True, False)
		platformDir = 'vc143'
		platform = 'x64'

	elif comp == 'vc143arm':
		baseDir = GetVSDir("17.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetTarget(baseDir, False, True)
		platformDir = 'vc143'
		platform = 'ARM'

	elif comp == 'vc143arm64':
		baseDir = GetVSDir("17.0")
		vcvarsall = baseDir + r'\VC\Auxiliary\Build\vcvarsall.bat'
		target = GetTarget(baseDir, True, True)
		platformDir = 'vc143'
		platform = 'ARM64'

	else:
		if verbose:
			print('ERROR (pyDcon/VSPaths): Unknown target (not installed?): ' + c)
		return ('', '', '', '')

	if len(baseDir) == 0:
		if verbose:
			print('ERROR (pyDcon/VSPaths): Unknown target (not installed?): ' + c)
		return ('', '', '', '')
	if not os.access(baseDir, os.F_OK):
		if verbose:
			print('ERROR (pyDcon/VSPaths): "' + baseDir + '" does not exist')
		return ('', '', '', '')
	if not os.access(vcvarsall, os.F_OK):
		if verbose:
			print('ERROR (pyDcon/VSPaths): "' + vcvarsall + '" does not exist')
		return ('', '', '', '')

	return (baseDir, '"' + vcvarsall + '" ' + target + extraargs, platformDir, platform)


if __name__ == '__main__':
	sys.exit(0)
