# Update build number in VersionNumber.h (Number of days since ARB was born)
# and ../../configure.in
#
# This assumes the current directory is 'Include'
#
# @li 2011-08-20 DRC Added a nicer file lock.
# @li 2010-03-05 DRC Removed CalVerNum
# @li 2009-12-20 DRC Added -x argument
# @li 2009-10-06 DRC Add multiprocessing awareness. Kind of.
# @li 2009-08-12 DRC Make sure full version is the same in configure.in
# @li 2009-04-12 DRC Added CalVerNum.h to be autoincremented.
#                Check to see if file actually changed before re-writing

import datetime
import os
import string
import sys


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
			os.remove(self.m_filename)
			return 1
		except OSError:
			return 0


def doWork():
	update = 0
	vMajARB = '0'
	vMinARB = '0'
	vDotARB = '0'
	vBldARB = '0'
	ver = open('VersionNumber.h', 'r')
	verOut = open('VersionNumber.h.new', 'w')
	while 1:
		line = ver.readline()
		if line:
			defineMaj = '#define ARB_VER_MAJOR'
			defineMin = '#define ARB_VER_MINOR'
			defineDot = '#define ARB_VER_DOT'
			defineBld = '#define ARB_VER_BUILD'
			pos = line.find(defineMaj)
			if 0 == pos:
				vMajARB = string.strip(line[pos+len(defineMaj):])
			pos = line.find(defineMin)
			if 0 == pos:
				vMinARB = string.strip(line[pos+len(defineMin):])
			pos = line.find(defineDot)
			if 0 == pos:
				vDotARB = string.strip(line[pos+len(defineDot):])
			pos = line.find(defineBld)
			if 0 == pos:
				vBldOld = string.strip(line[pos+len(defineBld):])
				vBldARB = str((datetime.date.today() - datetime.date(2002,12,28)).days)
				if not vBldOld == vBldARB:
					update = 1
					print >>verOut, defineBld + '\t\t\t\t\t' + vBldARB
				else:
					print >>verOut, line,
			else:
				print >>verOut, line,
		else:
			break
	ver.close()
	verOut.close()
	if update:
		print "VersionNumber.h updated to " + vMajARB + '.' + vMinARB + '.' + vDotARB + '.' + vBldARB
		os.remove('VersionNumber.h')
		os.rename('VersionNumber.h.new', 'VersionNumber.h')
	else:
		print "VersionNumber.h is up-to-date"
		os.remove('VersionNumber.h.new')

	update = 0
	conf = open('../../configure.in', 'r')
	confOut = open('../../configure.in.new', 'wb')
	while 1:
		line = conf.readline()
		if line:
			if 0 == line.find('AC_INIT('):
				newLine = 'AC_INIT(Agility Record Book, ' + vMajARB + '.' + vMinARB + '.' + vDotARB + '.' + vBldARB + ', [help@agilityrecordbook.com])\n'
				if not line == newLine:
					update = 1
					print >>confOut, newLine,
				else:
					print >>confOut, line,
			else:
				print >>confOut, line,
		else:
			break
	conf.close()
	confOut.close()
	if update:
		print "../../configure.in updated to " + vMajARB + '.' + vMinARB + '.' + vDotARB + '.' + vBldARB
		os.remove('../../configure.in')
		os.rename('../../configure.in.new', '../../configure.in')
	else:
		print "../../configure.in is up-to-date"
		os.remove('../../configure.in.new')

	return 0


if __name__ == '__main__':
	if 2 == len(sys.argv) and sys.argv[1] == "-x":
		print "SetBuildNumber skipped"
	else:
		lockfile = LockFile("SetBuildNumber.lck")
		if lockfile.acquire():
			doWork()
			lockfile.release()
		else:
			print "SetBuildNumber is locked"
	sys.exit(0)
