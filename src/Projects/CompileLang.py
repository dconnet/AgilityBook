#!/usr/bin/env python
# Generate the message libraries
#
# Requires msgcat/msgfmt (gettext) in PATH
#
# Revision History
# 2020-12-27 Add missing '-u' option in msgcat to remove duplicates.
# 2019-05-20 Changed sourceDir argument to '-s' so multiple can be supported.
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
"""CompileDatafile.py [-d] -s sourceDir firstFile outputDir targetname
-d: Debugging mode (does not delete generated autogen.po file)
-s sourceDir: Directory where .po files are [assumption - all subdirs are lang (en-US)]
              Note: Can be specified multiple times.
firstFile: First .po file to process - this filename must be unique across all sourceDirs
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
			try:
				now = time.time()
				st = os.stat(self.m_filename)
				# If the lock file is older than 300 seconds, just nuke it.
				# Probably left from a killed build.
				print(now - st.st_ctime)
				if now - st.st_ctime > 300:
					os.remove(self.m_filename)
					self.m_fd = os.open(self.m_filename, os.O_CREAT|os.O_EXCL|os.O_RDWR)
					os.write(self.m_fd, b"%d" % self.m_pid)
					return 1
			except OSError:
				self.m_fd = None
				return 0
			self.m_fd = None
			return 0

	def release(self):
		if not self.m_fd:
			return 0
		try:
			os.close(self.m_fd)
			self.m_fd = None
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
# sourceDirs: set of directories to search, each is assumed to contain language names (like 'en-US')
# firstFile: Name of the first file is list in msgcat command
# outputDir: Directory where we will create a 'lang' dir containing language names
# targetname: Name of the MO file to be created in outputDir/lang/<langName>/
def CompilePoFiles(wxBaseName, sourceDirs, firstFile, outputDir, targetname, bDebug):
	if not os.access(outputDir, os.F_OK):
		print(outputDir, 'does not exist')
		return False

	langNames = set()
	langPaths = []

	# Get all the languages and language source paths
	for sourceDir in sourceDirs:
		if not os.access(sourceDir, os.F_OK):
			print(sourceDir, 'does not exist')
			return False
		for srcPath in glob.glob(os.path.join(sourceDir, r'*')):
			if os.access(srcPath, os.F_OK):
				mode = os.stat(srcPath)[stat.ST_MODE]
				if stat.S_ISDIR(mode):
					langPaths += [srcPath]
					langName = os.path.basename(srcPath)
					langNames.add(langName)

	if len(langNames) == 0:
		print('ERROR: No languages found');
		return False

	langDir = os.path.join(outputDir, r'lang')
	if not os.access(langDir, os.F_OK):
		os.mkdir(langDir)

	skipped = 0
	for langName in langNames:
		poFile1 = ''
		poFiles = []

		installPath = os.path.join(langDir, langName)
		if not os.access(installPath, os.F_OK):
			os.mkdir(installPath)

		# This is the temporary file created by msgcat
		autogen = os.path.join(installPath, autogenFile)

		# Get all the PO files
		for srcPath in langPaths:
			if os.path.basename(srcPath) == langName:
				srcPathFirstFile = os.path.join(srcPath, firstFile)
				for po in glob.glob(os.path.join(srcPath, r'*.po')):
					if po == srcPathFirstFile:
						poFile1 = srcPathFirstFile
					else:
						poFiles += [po]
				# Look for additional PO files based on wx version
				wxLangDir = os.path.join(srcPath, wxBaseName)
				for po in glob.glob(os.path.join(wxLangDir, r'*.po')):
					poFiles += [po]

		# If we can't find the firstFile, assume we've included some library PO
		# files for languages that the program does not (yet) support.
		if len(poFile1) == 0:
			++skipped
			continue

		# -t: output encoding
		cmd = ['msgcat', '-u', '-t', 'utf-8', '-o', autogen, poFile1]
		for po in poFiles:
			cmd += [po];
		RunCommand(cmd, 0)

		cmd = ['msgfmt', '--verbose', '--check-domain', '--use-fuzzy', '--strict', '-o', os.path.join(installPath, targetname + r'.mo'), autogen]
		# msgfmt generates interesting messages to stderr, don't toast them.
		RunCommand(cmd, 0)

		if not bDebug and os.access(autogen, os.F_OK):
			os.remove(autogen)

	if skipped == len(langNames):
		print('ERROR: No languages compiled')
		return False

	return True


def main():
	bDebug = False
	sourceDirs = set()
	wxwin = ''
	try:
		opts, args = getopt.getopt(sys.argv[1:], 'ds:w:')
	except getopt.error as msg:
		print(msg)
		print('Usage:', __doc__)
		return 1
	for o, a in opts:
		if '-d' == o:
			bDebug = True
		elif '-s' == o:
			sourceDirs.add(a)
		elif '-w' == o:
			wxwin = a
	if not len(args) == 3:
		print('Usage:', __doc__)
		return 1

	if len(wxwin) == 0:
		if 'WXWIN' not in os.environ:
			print('ERROR: WXWIN environment variable is not set')
			return 1
		wxwin = os.environ['WXWIN']
	if not os.access(wxwin, os.F_OK):
		print(outputDir, 'does not exist')
		return False
 
	wxBaseName = os.path.basename(wxwin)
	firstFile = args[0]
	outputDir = args[1]
	targetname = args[2]

	rc = 0
	lockfile = LockFile(os.path.join(outputDir, "CompileDatafile.lck"))
	if lockfile.acquire():
		try:
			if not CompilePoFiles(wxBaseName, sourceDirs, firstFile, outputDir, targetname, bDebug):
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
