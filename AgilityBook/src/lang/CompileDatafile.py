#!/usr/bin/env python
# Generate the message libraries and data files
#
# Revision History
# 2014-11-16 Separated PO/MO language from DAT file generation.
# 2014-05-17 Add exception handling in RunCommand
# 2012-05-16 Add multiprocessing awareness. Kind of.
# 2012-03-03 Fixed writing file into dat file.
# 2012-02-19 Remove --use-fuzzy option, reoganized how po files.
# 2012-01-29 Change msgfmt options to not do header checks
# 2011-12-17 Add -x option.
# 2011-11-11 Made generic for easy use in other projects.
# 2009-03-05 Fixed some parameter passing issues (spaces/hyphens in names)
# 2009-03-01 Support multiple po files (by using msgcat)
# 2009-01-02 Updated to support creation of data files
"""CompileDatafile.py [-x] [-d] filelist executableDir targetname
-x: Exclude ARBUpdater (not needed in test program)
-d: Debugging mode (does not delete generated autogen.po file)
filelist: List of files to include in .dat file
executableDir: Directory where executable is (where dat file is created)
targetname: Name of .dat files to generate
"""

import getopt
import glob
import os, os.path
import stat
import sys
import subprocess
import time
import zipfile

# By default, gettext is in the path
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
			os.write(self.m_fd, "%d" % self.m_pid)
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
		try:
			p = subprocess.Popen(command, stderr=subprocess.STDOUT, stdout=subprocess.PIPE)
		except OSError, msg:
			print 'ERROR:', command
			print 'EXCEPTION:', msg
			raise
	ReadPipe(sys.stdout, p.stdout)


def main():
	bIncUpdater = 1
	bDebug = 0
	try:
		opts, args = getopt.getopt(sys.argv[1:], 'dx')
	except getopt.error, msg:
		print msg
		print 'Usage:', __doc__
		return 1
	for o, a in opts:
		if '-d' == o:
			bDebug = 1
		if '-x' == o:
			bIncUpdater = 0
	if not len(args) == 3:
		print 'Usage:', __doc__
		return 1

	filelist = args[0]
	executableDir = args[1]
	targetname = args[2]

	# The files listed in the list are relative to the filelist location.
	if not os.access(filelist, os.F_OK):
		print 'ERROR: "' + filelist + '" does not exist!'
		print 'Usage:', __doc__
		return 1;

	zip = zipfile.ZipFile(os.path.join(executableDir, targetname + '.dat'), 'w')
	basepath = os.path.dirname(filelist)
	files = open(filelist)
	while 1:
		line = files.readline()
		if line:
			line = line.rstrip()
			inputfile = os.path.abspath(os.path.join(basepath, line))
			filename = os.path.split(inputfile)[1]
			if os.access(inputfile, os.F_OK):
				zip.write(inputfile, filename)
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
	rc = 0
	lockfile = LockFile("CompileDatafile.lck")
	if lockfile.acquire():
		rc = main()
		lockfile.release()
	else:
		print "CompileDatafile is locked"
	sys.exit(rc)
