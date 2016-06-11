#!/usr/bin/env python
# Generate the message libraries
#
# Requires msgcat/msgfmt (gettext) in PATH
#
# Revision History
# 2016-06-10 Convert to Python3
# 2016-03-28 Cleanup lockfile on exception
# 2015-10-29 Put --use-fuzzy back, remove --check-format.
# 2014-11-16 Separated PO/MO language from DAT file generation.
# 2012-05-16 Add multiprocessing awareness. Kind of.
# 2012-03-03 Fixed writing file into dat file.
# 2012-02-19 Remove --use-fuzzy option.
# 2012-01-29 Change msgfmt options to not do header checks
# 2011-12-30 Write files with their relative name, not the full path.
# 2011-12-17 Added -f option
# 2011-11-11 Made generic for easy use in other projects.
# 2009-03-05 Fixed some parameter passing issues (spaces/hyphens in names)
# 2009-03-01 Support multiple po files (by using msgcat)
# 2009-01-02 Updated to support creation of data files
"""CompileDatafile.py [-d] sourceDir firstFile outputDir targetname
-d: Debugging mode (does not delete generated autogen.po file)
sourceDir: Directory where .po files are [assumption - all subdirs are lang]
firstFile: First .po file to process
outputDir: Directory where 'lang' dir is created
targetname: Name of .mo and .dat files to generate
"""

import getopt
import glob
import os, os.path
import stat
import sys
import subprocess
import time
import zipfile

# This file is generated with msgcat of all po files ('firstFile' is first)
autogenFile = 'autogen.po'


class LockFile:
	def __init__(self, filename):
		self.m_filename = filename
		self.m_fd = None
		self.m_pid = os.getpid()

	def __del__(self):
		self.release()

	def acquire(self):
		try:
			self.m_fd = os.open(self.m_filename, os.O_CREAT|os.O_EXCL|os.O_RDWR)
			os.write(self.m_fd, b"%d" % self.m_pid)
			return 1
		except OSError:
			self.m_fd = None
			return 0

	def release(self):
		if not self.m_fd:
			return 0
		try:
			os.close(self.m_fd)
			time.sleep(1)
			os.remove(self.m_filename)
			return 1
		except OSError:
			return 0


def ReadPipe(logFile, cmd):
	while (1):
		line = cmd.readline()
		if line:
			line = str.rstrip(line.decode())
			print(line, file=logFile)
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


# wxBaseName: base of wx ('trunk', 'wxWidgets-2.8.12', 'wxWidgets-3.0.2', etc)
def CompilePoFiles(wxBaseName, sourceDir, firstFile, outputDir, targetname, bDebug):
	if not os.access(sourceDir, os.F_OK):
		print(sourceDir, 'does not exist')
		return False

	if not os.access(outputDir, os.F_OK):
		print(outputDir, 'does not exist')
		return False

	langDir = os.path.join(outputDir, r'lang')
	if not os.access(langDir, os.F_OK):
		os.mkdir(langDir)

	for srcPath in glob.glob(os.path.join(sourceDir, r'*')):
		langName= os.path.basename(srcPath)
		if os.access(srcPath, os.F_OK):
			mode = os.stat(srcPath)[stat.ST_MODE]
			if stat.S_ISDIR(mode):
				autogen = os.path.join(outputDir, autogenFile)
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
				cmd = ['msgfmt', '--verbose', '--check-domain', '--use-fuzzy', '--strict', '-o', os.path.join(installPath, targetname + r'.mo'), autogen]
				# msgfmt generates interesting messages to stderr, don't toast them.
				RunCommand(cmd, 0)
				if not bDebug and os.access(autogen, os.F_OK):
					os.remove(autogen)
	return True


def main():
	bDebug = 0
	try:
		opts, args = getopt.getopt(sys.argv[1:], 'd')
	except getopt.error as msg:
		print(msg)
		print('Usage:', __doc__)
		return 1
	for o, a in opts:
		if '-d' == o:
			bDebug = 1
	if not len(args) == 4:
		print('Usage:', __doc__)
		return 1

	if 'WXWIN' not in os.environ:
		print('ERROR: WXWIN environment variable is not set')
		return 1
	wxBaseName = os.path.basename(os.environ['WXWIN'])

	outputDir = args[2]

	rc = 0
	lockfile = LockFile(os.path.join(outputDir, "CompileDatafile.lck"))
	if lockfile.acquire():
		try:
			if not CompilePoFiles(wxBaseName, args[0], args[1], outputDir, args[3], bDebug):
				rc = 1
		except:
			lockfile.release()
			raise
		lockfile.release()
	else:
		print("CompileDatafile is locked")
	return rc


if __name__ == '__main__':
	sys.exit(main())
