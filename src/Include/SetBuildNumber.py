# Update build number in VersionNumber.h
# and ../configure.in
#
# This assumes the current directory is 'Include'
#
# 2020-11-12 Changed padding in ARB_VER_BUILD output.
# 2020-08-21 Moved configure.in into src (moved top build directory down)
# 2019-08-24 Fix writing configure.in
# 2016-06-10 Convert to Python3
# 2016-03-28 Cleanup lockfile on exception
# 2012-09-24 Sync copyright in configure.in with VersionNumber.h
# 2012-07-18 Change build number from days-since-birth to sequential.
# 2011-08-20 Added a nicer file lock.
# 2010-03-05 Removed CalVerNum
# 2009-12-20 Added -x argument
# 2009-10-06 Add multiprocessing awareness. Kind of.
# 2009-08-12 Make sure full version is the same in configure.in
# 2009-04-12 Added CalVerNum.h to be autoincremented.
#            Check to see if file actually changed before re-writing

import datetime
import os
import string
import sys
import time

# Was '\t\t\t\t\t' - changed after updating header with clang-format
verBuildPadding = ' '

verInfo = 'VersionInfoARB.h'
verNum = 'VersionNumber.h'
configure = '../../configure.in'
baseDefine = 'ARB_'


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
			os.write(self.m_fd, b'%d' % self.m_pid)
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
					os.write(self.m_fd, b'%d' % self.m_pid)
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


def doWork():
	update = False
	copyRight = ''
	vMajARB = '0'
	vMinARB = '0'
	vDotARB = '0'
	vBldARB = '0'

	ver = open(verInfo, 'r')
	while True:
		line = ver.readline()
		if not line:
			break
		line = str.rstrip(line)
		defineCopy = '#define ' + baseDefine + 'VERSION_LegalCopyright'
		pos = line.find(defineCopy)
		if 0 == pos:
			copyRight = str.strip(line[pos+len(defineCopy):])
	ver.close()

	ver = open(verNum, 'r')
	verOut = open(verNum + '.new', 'w')
	while True:
		line = ver.readline()
		if not line:
			break
		line = str.rstrip(line)
		defineMaj = '#define ' + baseDefine + 'VER_MAJOR'
		defineMin = '#define ' + baseDefine + 'VER_MINOR'
		defineDot = '#define ' + baseDefine + 'VER_DOT'
		defineBld = '#define ' + baseDefine + 'VER_BUILD'
		pos = line.find(defineMaj)
		if 0 == pos:
			vMajARB = str.strip(line[pos+len(defineMaj):])
		pos = line.find(defineMin)
		if 0 == pos:
			vMinARB = str.strip(line[pos+len(defineMin):])
		pos = line.find(defineDot)
		if 0 == pos:
			vDotARB = str.strip(line[pos+len(defineDot):])
		pos = line.find(defineBld)
		if 0 == pos:
			vBldOld = str.strip(line[pos+len(defineBld):])
			vBldARB = str(int(vBldOld) + 1)
			update = True
			print(defineBld + verBuildPadding + vBldARB, file=verOut)
			# Old code where Bld was number of days since first release
			#vBldARB = str((datetime.date.today() - datetime.date(2002,12,28)).days)
			#if not vBldOld == vBldARB:
			#	update = True
			#	print >>verOut, defineBld + verBuildPadding + vBldARB
			#else:
			#	print >>verOut, line,
		else:
			print(line, file=verOut)
	ver.close()
	verOut.close()

	if update:
		print(verNum + ' updated to ' + vMajARB + '.' + vMinARB + '.' + vDotARB + '.' + vBldARB)
		os.remove(verNum)
		os.rename(verNum + '.new', verNum)
	else:
		print(verNum + ' is up-to-date')
		os.remove(verNum + '.new')

	update = False
	conf = open(configure, 'r')
	confOut = open(configure + '.new', 'w', newline='\n')
	while 1:
		line = conf.readline()
		if not line:
			break
		line = str.rstrip(line)
		if 0 == line.find('AC_INIT('):
			newLine = 'AC_INIT(Agility Record Book, ' + vMajARB + '.' + vMinARB + '.' + vDotARB + '.' + vBldARB + ', [help@agilityrecordbook.com])'
			if not line == newLine:
				update = True
				print(newLine, file=confOut)
			else:
				print(line, file=confOut)
		elif 0 == line.find('AC_SUBST(PACKAGE_COPYRIGHT,'):
			newLine = 'AC_SUBST(PACKAGE_COPYRIGHT, ' + copyRight + ')'
			if not line == newLine:
				update = True
				print(newLine, file=confOut)
			else:
				print(line, file=confOut)
		else:
			print(line, file=confOut)
	conf.close()
	confOut.close()

	if update:
		print(configure + ' updated to ' + vMajARB + '.' + vMinARB + '.' + vDotARB + '.' + vBldARB)
		os.remove(configure)
		os.rename(configure + '.new', configure)
	else:
		print(configure + ' is up-to-date')
		os.remove(configure + '.new')

	return 0


if __name__ == '__main__':
	if 2 == len(sys.argv) and sys.argv[1] == '--official':
		lockfile = LockFile('SetBuildNumber.lck')
		if lockfile.acquire():
			try:
				doWork()
			except:
				lockfile.release()
				raise
			lockfile.release()
		else:
			print('SetBuildNumber is locked')
	else:
		print('SetBuildNumber skipped')
	sys.exit(0)
