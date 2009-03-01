# Generate the message libraries and data files
#
# Requires gettext (msgcat/msgfmt) and winzip command line addon (wzzip)
#
# Revision History
# 2009-03-01 DRC Support multiple po files (by using msgcat)
# 2009-01-02 DRC Updated to support creation of data files
"""compile.py [-d] executableDir targetname
-d: Debugging mode (does not delete generated autogen.po file)
executableDir: Directory where executable is (where 'lang' dir is created)
targetname: Name of .mo file to generate
"""

import getopt
import glob
import os
import shutil
import sys

# By default, gettext is in the path, winzip is not.
# Command line addon to winzip
winzip = 'c:\Program Files\WinZip\WZZIP.EXE'

# This file is generated with msgcat of all po files (arb.po is first)
autogenFile = 'autogen.po'


def ReadPipe(logFile, cmd):
	while (1):
		line = cmd.readline()
		if line:
			print >>logFile, line,
		else:
			break

def RunCommand(command):
	print 'Running:', command
	(childin, childout) = os.popen4(command)
	childin.close()
	ReadPipe(sys.stdout, childout)
	childout.close()


def main():
	bDebug = 0
	executableDir = ''
	try:
		opts, args = getopt.getopt(sys.argv[1:], 'd')
	except getopt.error, msg:
		print msg
		print 'Usage:', __doc__
		return
	for o, a in opts:
		if '-d' == o:
			bDebug = 1
	if 2 != len(args):
		print 'Usage:', __doc__
		return
	executableDir = args[0]
	targetname = args[1]

	if not os.access(executableDir, os.F_OK):
		print executableDir, 'does not exist'

	langDir = executableDir + r'\lang'
	if not os.access(langDir, os.F_OK):
		os.mkdir(langDir)

	for file in ('en', 'fr'):
		autogen = file + '\\' + autogenFile
		# -t: output encoding
		cmd = 'msgcat -t "utf-8" -o ' + autogen + ' ' + file + r'\arb.po'
		for po in glob.glob(file + r'\*.po'):
			if po != file + r'\arb.po' and po != autogen:
				cmd += ' ' + po
		RunCommand(cmd)
		installPath = langDir + '\\' + file
		if not os.access(installPath, os.F_OK):
			os.mkdir(installPath)
		# -v: verbose
		# -c: perform all checks (format,header,domain)
		# -f: Use fuzzy entres in output
		cmd = 'msgfmt -v -c -f --strict -o "' + installPath + r'\arb.mo" ' + autogen
		RunCommand(cmd)
		if not bDebug:
			os.remove(autogen)

	RunCommand('"' + winzip + '" -a ' + executableDir + '\\' + targetname + '.dat DefaultConfig.xml AgilityRecordBook.dtd')


main()
