# coding=utf-8
# Above line is for python
#
# Revision History
# 2010-05-30 DRC Converted .bat to .py (keeps environment clean!)
"""BuildAll.py [fullupdate | clean] [vc9 | vc10]
   Default is to do a dirty build
"""

import glob
import os
import stat
import string
import subprocess
import sys


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


def main():
	if len(sys.argv) > 3:
		print 'Usage:', __doc__
		return

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
			return

	if not updateBuildNumber:
		os.environ['SETBUILDNUMBER_UPDATE'] = '-x'

	# Targets:
	# VC7.1
	#  Configuration: 'Release'/'Debug' (non-unicode)
	#  Platform: Win32
	#  Targets: AgilityBook, cal_usdaa, LibTidy
	# VC8
	#  Configuration: 'Release'/'Debug'/'Unicode Release'/'Unicode Debug'
	#  Platform: Win32, x64
	#  ARBHelp: 'Release'/'Debug'
	#  Targets:
	#   AgilityBook: all
	#   ARBHelp: no unicode
	#   cal_usdaa: all
	#   LibTidy: no unicode
	# VC9/VC10
	#  Configuration: 'Release'/'Debug'/'Release - No Unicode'/'Debug - No Unicode'
	#  Platform: Win32, x64
	#  Targets:
	#   AgilityBook: unicode only
	#   ARBHelp: unicode only
	#   cal_usdaa: unicode only
	#   LibTidy: no unicode [debug/release=non unicode]
	#   TestARB: all

	if buildVC9:
		if clean:
			RmMinusRF('../../../bin/VC9Win32')
		remove(r'../VC9/bldWin32.txt')
		cmds = (
			r'title VC9 Release Win32',
			r'cd ..\VC9',
			r'call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86',
			r'devenv AgilityBook.sln /out bldWin32.txt /build "Release|Win32"')
		RunCmds(cmds)
		if not os.access('../../../bin/VC9Win32/Release/AgilityBook.exe', os.F_OK):
			print 'ERROR: Compile failed, bailing out'
			return
		if clean:
			RmMinusRF('../../../bin/VC9x64')
		remove(r'../VC9/bldWin64.txt')
		cmds = (
			r'title VC9 Release x64',
			r'cd ..\VC9',
			r'call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86_amd64',
			r'devenv AgilityBook.sln /out bldWin64.txt /build "Release|x64"')
		RunCmds(cmds)
		if not os.access('../../../bin/VC9x64/Release/AgilityBook.exe', os.F_OK):
			print 'ERROR: Compile failed, bailing out'
			return

	if buildVC10:
		if clean:
			RmMinusRF('../../../bin/VC10Win32')
		cmds = (
			r'title VC10 Release Win32',
			r'cd ..\VC10',
			r'call "C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86',
			r'msbuild AgilityBook.sln /t:Build /p:Configuration=Release;Platform=Win32')
		RunCmds(cmds)
		if not os.access('../../../bin/VC10Win32/Release/AgilityBook.exe', os.F_OK):
			print 'ERROR: Compile failed, bailing out'
			return
		if clean:
			RmMinusRF('../../../bin/VC10x64')
		cmds = (
			r'title VC10 Release x64',
			r'cd ..\VC10',
			r'call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.Cmd" /release /x64 /xp',
			r'msbuild AgilityBook.sln /t:Build /p:Configuration=Release;Platform=x64')
		RunCmds(cmds)
		if not os.access('../../../bin/VC10x64/Release/AgilityBook.exe', os.F_OK):
			print 'ERROR: Compile failed, bailing out'
			return


main()
