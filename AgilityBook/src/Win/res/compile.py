# Generate the message libraries and data files
#
# Requires gettext (msgfmt.exe) and winzip command line addon (wzzip.exe)
#
# Revision History
# 2009-01-02 DRC Updated to support creation of data files
"""compile.py executableDir targetname"""

import getopt
import glob
import os
import shutil
import sys

# By default, gettext is in the path, winzip is not.
# Command line addon to winzip
winzip = "c:\Program Files\WinZip\WZZIP.EXE"


def ReadPipe(logFile, cmd):
	while (1):
		line = cmd.readline()
		if line:
			print >>logFile, line,
		else:
			break

def RunCommand(command):
	print "Running:", command
	(childin, childout) = os.popen4(command)
	childin.close()
	ReadPipe(sys.stdout, childout)
	childout.close()


def main():
	bDebug = 0
	executableDir = ""
	try:
		opts, args = getopt.getopt(sys.argv[1:], "d")
	except getopt.error, msg:
		print msg
		print "Usage:", __doc__
		return
	for o, a in opts:
		if "-d" == o:
			bDebug = 1
	if 2 != len(args):
		print "Usage:", __doc__
		return
	executableDir = args[0]
	targetname = args[1]

	if not os.access(executableDir, os.F_OK):
		print executableDir, "does not exist"

	langDir = executableDir + r'\lang'
	if not os.access(langDir, os.F_OK):
		os.mkdir(langDir)

	for file in ("en", "fr"):
		installPath = langDir + '\\' + file
		if not os.access(installPath, os.F_OK):
			os.mkdir(installPath)
		cmd = 'msgfmt -v -c -f --strict -o "' + installPath + r'\arb.mo" ' + file + r'\arb.po'
		RunCommand(cmd)

	RunCommand('"' + winzip + '" -a ' + executableDir + '\\' + targetname + '.dat DefaultConfig.xml AgilityRecordBook.dtd')


main()
