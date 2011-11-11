#!/usr/bin/env python
# Generate the message libraries and data files
#
# Requires gettext (msgcat/msgfmt) in PATH
#
# Revision History
# 2011-11-11 DRC Made generic for easy use in other projects.
# 2009-03-05 DRC Fixed some parameter passing issues (spaces/hyphens in names)
# 2009-03-01 DRC Support multiple po files (by using msgcat)
# 2009-01-02 DRC Updated to support creation of data files
"""compile.py [-d] sourceDir firstFile executableDir targetname [lang;lang...]
-d: Debugging mode (does not delete generated autogen.po file)
sourceDir: Directory where .po files are
firstFile: First .po file to process
executableDir: Directory where executable is (where 'lang' dir is created)
targetname: Name of .mo file to generate
lang: Addition semi-colon separated languages (like 'fr_FR')
"""

import getopt
import glob
import os, os.path
import sys
import subprocess
import zipfile

# By default, gettext is in the path
# This file is generated with msgcat of all po files ('firstFile' is first)
autogenFile = 'autogen.po'


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
	bDebug = 0
	try:
		opts, args = getopt.getopt(sys.argv[1:], 'd')
	except getopt.error, msg:
		print msg
		print 'Usage:', __doc__
		return 1
	for o, a in opts:
		if '-d' == o:
			bDebug = 1
	if len(args) < 4 or len(args) > 5:
		print 'Usage:', __doc__
		return 1

	langs = ['en_US']

	if 5 == len(args):
		langs += args[4].split(';')

	sourceDir = args[0]
	firstFile = args[1]
	executableDir = args[2]
	targetname = args[3]

	if not os.access(sourceDir, os.F_OK):
		print sourceDir, 'does not exist'
		return 1

	if not os.access(executableDir, os.F_OK):
		print executableDir, 'does not exist'
		return 1

	langDir = os.path.join(executableDir, r'lang')
	if not os.access(langDir, os.F_OK):
		os.mkdir(langDir)

	for langName in langs:
		srcPath = os.path.join(sourceDir, langName)
		autogen = os.path.join(langName, autogenFile)
		# -t: output encoding
		cmd = ['msgcat', '-t', 'utf-8', '-o', autogen, os.path.join(srcPath, firstFile)]
		for po in glob.glob(os.path.join(srcPath, r'*.po')):
			if po != os.path.join(srcPath, firstFile) and po != autogen:
				cmd += [po]
		RunCommand(cmd, 0)
		installPath = os.path.join(langDir, langName)
		if not os.access(installPath, os.F_OK):
			os.mkdir(installPath)
		# -v: verbose
		# -c: perform all checks (format,header,domain)
		# -f: Use fuzzy entres in output
		cmd = ['msgfmt', '-v', '-c', '-f', '--strict', '-o', os.path.join(installPath, r'arb.mo'), autogen]
		# msgfmt generates interesting messages to stderr, don't toast them.
		RunCommand(cmd, 0)
		if not bDebug and os.access(autogen, os.F_OK):
			os.remove(autogen)

	zip = zipfile.ZipFile(os.path.join(executableDir, targetname + '.dat'), 'w')
	zip.write('DefaultConfig.xml')
	zip.write('AgilityRecordBook.dtd')
	if os.access(executableDir + r'\ARBUpdater.exe', os.F_OK):
		zip.write(executableDir + r'\ARBUpdater.exe', 'ARBUpdater.exe')
	zip.close()

	return 0


if __name__ == '__main__':
	sys.exit(main())
