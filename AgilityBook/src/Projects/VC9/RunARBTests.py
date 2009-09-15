#!/usr/bin/env python
# This script is called in the post-build step to run the unittest program
#
# Typical usage from VC project file:
#  python $(ProjectDir)RunARBTests.py "$(OutDir)" $(TargetName) $(PlatformName)
#
# Note, using "$(TargetDir)" is bad - you have "c:\...\dir\" - that final
# slash-quote causes a serious command line parsing problem when this is
# run via the post-build
#
# Revision History
# 2009-03-05 DRC Moved bat file to python (removes dependency on wzzip)
"""RunARBTests.py TargetDirectory TargetName PlatformName
PlatformName = Win32 x64"""

import glob
import os, os.path
import sys
import subprocess
import zipfile


def ReadPipe(logFile, cmd):
	while (1):
		line = cmd.readline()
		if line:
			print >>logFile, line,
		else:
			break


# Some commands generate messages on stderr that are interesting.
# Some are just plain annoying (wzzip)
def RunCommand(command, toastErr):
	print 'Running:', command
	if toastErr:
		# Map stderr to a pipe that we ignore
		p = subprocess.Popen(command, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
	else:
		# Map stderr to stdout
		p = subprocess.Popen(command, stderr=subprocess.STDOUT, stdout=subprocess.PIPE)
	ReadPipe(sys.stdout, p.stdout)


def main():
	if 4 != len(sys.argv):
		print 'Usage:', __doc__
		return
	executableDir = sys.argv[1]
	targetname = sys.argv[2]
	platform = sys.argv[3]

	if not "Win32" == platform and not "x64" == platform:
		print 'Unknown platform:', platform
		return

	# Create "TestARB.dat"
	zip = zipfile.ZipFile(os.path.join(executableDir, targetname + '.dat'), 'w')
	zip.write(r'..\..\Win\res\DefaultConfig.xml', 'DefaultConfig.xml')
	zip.write(r'..\..\Win\res\AgilityRecordBook.dtd', 'AgilityRecordBook.dtd')
	for file in glob.glob(r'..\..\TestARB\res\*.xml'):
		zip.write(file, os.path.basename(file))
	zip.close()

	if "Win32" == platform:
		# 32bit on 32bit
		if os.environ['PROCESSOR_ARCHITECTURE'] == "x86":
			cmd = [os.path.join(executableDir, targetname + '.exe')]
			RunCommand(cmd, 0)
		else:
			print 'WARNING: Unable to run ' + platform + ' binary on ' + os.environ['PROCESSOR_ARCHITECTURE']

	if "x64" == platform:
		# 64bit on 64bit
		if os.getenv('PROCESSOR_ARCHITECTURE', '') == "AMD64":
			cmd = [os.path.join(executableDir, targetname + '.exe')]
			RunCommand(cmd, 0)
		# 64bit on Wow64 (32bit cmd shell spawned from msdev)
		elif os.getenv('PROCESSOR_ARCHITEW6432', '') == "AMD64":
			cmd = [os.path.join(executableDir, targetname + '.exe')]
			RunCommand(cmd, 0)
		else:
			print 'WARNING: Unable to run ' + platform + ' binary on ' + os.environ['PROCESSOR_ARCHITECTURE']


main()
