#!/usr/bin/env python
# Generate the message libraries and data files
#
# Requires gettext (msgcat/msgfmt) in PATH
#
# Revision History
# 2012-02-19 DRC Remove --use-fuzzy option, reoganized how po files.
# 2012-01-29 DRC Change msgfmt options to not do header checks
# 2011-12-17 DRC Add -x option.
# 2011-11-11 DRC Made generic for easy use in other projects.
# 2009-03-05 DRC Fixed some parameter passing issues (spaces/hyphens in names)
# 2009-03-01 DRC Support multiple po files (by using msgcat)
# 2009-01-02 DRC Updated to support creation of data files
"""compile.py [-x] [-d] sourceDir firstFile executableDir targetname
-x: Exclude ARBUpdater (not needed in test program)
-d: Debugging mode (does not delete generated autogen.po file)
-f filelist: List of files to include in .dat file
sourceDir: Directory where .po files are [assumption - all subdirs are lang]
firstFile: First .po file to process
executableDir: Directory where executable is (where 'lang' dir is created)
targetname: Name of .mo and .dat files to generate
"""

import getopt
import glob
import os, os.path
import stat
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
	bIncUpdater = 1
	bDebug = 0
	filelist = ''
	try:
		opts, args = getopt.getopt(sys.argv[1:], 'df:x')
	except getopt.error, msg:
		print msg
		print 'Usage:', __doc__
		return 1
	for o, a in opts:
		if '-d' == o:
			bDebug = 1
		if '-f' == o:
			filelist = a
			if not os.access(filelist, os.F_OK):
				print 'ERROR: "' + filelist + '" does not exist!'
				print 'Usage:', __doc__
				return 1;
		if '-x' == o:
			bIncUpdater = 0
	if not len(args) == 4:
		print 'Usage:', __doc__
		return 1

	if not os.environ.has_key('WXWIN'):
		print 'ERROR: WXWIN environment variable is not set'
		return 1
	wxBaseName = os.path.basename(os.environ['WXWIN'])

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

	for srcPath in glob.glob(os.path.join(sourceDir, r'*')):
		langName= os.path.basename(srcPath)
		if os.access(srcPath, os.F_OK):
			mode = os.stat(srcPath)[stat.ST_MODE]
			if stat.S_ISDIR(mode):
				autogen = os.path.join(srcPath, autogenFile)
				# -t: output encoding
				cmd = ['msgcat', '-t', 'utf-8', '-o', autogen, os.path.join(srcPath, firstFile)]
				for po in glob.glob(os.path.join(srcPath, r'*.po')):
					if po != os.path.join(srcPath, firstFile) and po != autogen:
						cmd += [po]
				wxLangDir = os.path.join(srcPath, wxBaseName)
				for po in glob.glob(os.path.join(wxLangDir, r'*.po')):
					cmd += [po]
				RunCommand(cmd, 0)
				installPath = os.path.join(langDir, langName)
				if not os.access(installPath, os.F_OK):
					os.mkdir(installPath)
				cmd = ['msgfmt', '--verbose', '--check-format', '--check-domain', '--strict', '-o', os.path.join(installPath, targetname + r'.mo'), autogen]
				# msgfmt generates interesting messages to stderr, don't toast them.
				RunCommand(cmd, 0)
				if not bDebug and os.access(autogen, os.F_OK):
					os.remove(autogen)

	zip = zipfile.ZipFile(os.path.join(executableDir, targetname + '.dat'), 'w')
	# The files listed in the list are relative to the filelist location.
	if 0 < len(filelist):
		basepath = os.path.dirname(filelist)
		files = open(filelist)
		while 1:
			line = files.readline()
			if line:
				line = line.rstrip()
				inputfile = os.path.join(basepath, line)
				if os.access(inputfile, os.F_OK):
					zip.write(inputfile, line)
				else:
					print 'ERROR: File "' + inputfile + '" in "' + filelist + '" does not exist!'
					return 1
			else:
				break
	if bIncUpdater and os.access(executableDir + r'\ARBUpdater.exe', os.F_OK):
		zip.write(executableDir + r'\ARBUpdater.exe', 'ARBUpdater.exe')
	zip.close()

	return 0


if __name__ == '__main__':
	sys.exit(main())
