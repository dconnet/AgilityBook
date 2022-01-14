#!/usr/bin/env python
# This script is called in the post-build step to run the unittest program
#
# Typical usage from VC project file:
#  python $(ProjectDir)RunARBTests.py "$(ProjectDir)..\.." "$(OutDir) " "$(TargetName) " $(PlatformShortName)
#
# Note, using "$(TargetDir)" is bad - you have "c:\...\dir\" - that final
# slash-quote causes a serious command line parsing problem when this is
# run via the post-build. So we pad a blank in the quotes and strip it here.
#
# Revision History
# 2022-01-13 Per 2014-11-19, stop it. (Mac/unix Makefile generates it)
# 2018-11-16 Added ARM64 support.
# 2016-06-10 Convert to Python3
# 2014-11-19 DAT file is now embedded on Windows. Don't generate file here.
# 2011-01-22 Allow 32bit to run on 64bit.
# 2009-03-05 Moved bat file to python (removes dependency on wzzip)
"""RunARBTests.py SourceDir TargetDirectory TargetName PlatformShortName
SourceDir = .../AgilityBook/src
PlatformShortName = x86 x64 Mac"""

import glob
import os, os.path
import string
import sys
import subprocess
import zipfile


def ReadPipe(logFile, cmd):
	while (1):
		line = cmd.readline()
		if line:
			print(str.rstrip(line.decode()), file=logFile)
		else:
			break


# Some commands generate messages on stderr that are interesting.
# Some are just plain annoying (wzzip)
def RunCommand(command, toastErr):
	print('Running:', command)
	if toastErr:
		# Map stderr to a pipe that we ignore
		p = subprocess.Popen(command, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
	else:
		# Map stderr to stdout
		p = subprocess.Popen(command, stderr=subprocess.STDOUT, stdout=subprocess.PIPE)
	ReadPipe(sys.stdout, p.stdout)
	return p.returncode


def main():
	if 5 != len(sys.argv):
		print('Usage:', __doc__)
		return 1
	srcDir = str.strip(sys.argv[1])
	executableDir = str.strip(sys.argv[2])
	targetname = str.strip(sys.argv[3])
	platform = sys.argv[4]

	if not "x86" == platform and not "x64" == platform and not "ARM64" == platform and not "Mac" == platform:
		print('Unknown platform:', platform)
		return 1

	os.chdir(executableDir)

	if "x86" == platform:
		cmd = [os.path.join(executableDir, targetname + '.exe')]
		return RunCommand(cmd, 0)

	elif "x64" == platform:
		# 64bit on 64bit
		if os.getenv('PROCESSOR_ARCHITECTURE', '') == "AMD64":
			cmd = [os.path.join(executableDir, targetname + '.exe')]
			return RunCommand(cmd, 0)
		# 64bit on Wow64 (32bit cmd shell spawned from msdev)
		elif os.getenv('PROCESSOR_ARCHITEW6432', '') == "AMD64":
			cmd = [os.path.join(executableDir, targetname + '.exe')]
			return RunCommand(cmd, 0)
		else:
			print('WARNING: Unable to run ' + platform + ' binary on ' + os.environ['PROCESSOR_ARCHITECTURE'])
			return 0
	
	elif "ARM64" == platform:
		if os.getenv('PROCESSOR_ARCHITECTURE', '') == "ARM64":
			cmd = [os.path.join(executableDir, targetname + '.exe')]
			return RunCommand(cmd, 0)
		else:
			print('WARNING: Unable to run ' + platform + ' binary on ' + os.environ['PROCESSOR_ARCHITECTURE'])
			return 0
	
	elif "Mac" == platform:
		cmd = [os.path.join(executableDir, targetname)]
		return RunCommand(cmd, 0)

	print('Unknown platform:', platform)
	return 1


if __name__ == '__main__':
	sys.exit(main())
