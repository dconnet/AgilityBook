# Update build number in VersionNumber.h (Number of days since ARB was born)
# and ../../configure.in
#
# This assumes the current directory is 'Include'
# @li 2009-04-12 DRC Added CalVerNum.h to be autoincremented.
#                Check to see if file actually changed before re-writing

import datetime
import os
import string
import sys

update = 0
vMaj = '0'
vMin = '0'
vDot = '0'
vBld = '0'
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
			vMaj = string.strip(line[pos+len(defineMaj):])
		pos = line.find(defineMin)
		if 0 == pos:
			vMin = string.strip(line[pos+len(defineMin):])
		pos = line.find(defineDot)
		if 0 == pos:
			vDot = string.strip(line[pos+len(defineDot):])
		pos = line.find(defineBld)
		if 0 == pos:
			vBldOld = string.strip(line[pos+len(defineBld):])
			vBld = str((datetime.date.today() - datetime.date(2002,12,28)).days)
			if not vBldOld == vBld:
				update = 1
				print >>verOut, defineBld + '\t\t\t\t\t' + vBld
			else:
				print >>verOut, line,
		else:
			print >>verOut, line,
	else:
		break
ver.close()
verOut.close()
if update:
	print "VersionNumber.h updated to " + vMaj + '.' + vMin + '.' + vDot + '.' + vBld
	os.remove('VersionNumber.h')
	os.rename('VersionNumber.h.new', 'VersionNumber.h')
else:
	print "VersionNumber.h is up-to-date"
	os.remove('VersionNumber.h.new')

update = 0
vMaj = '0'
vMin = '0'
vDot = '0'
vBld = '0'
ver = open('../cal_usdaa/CalVerNum.h', 'r')
verOut = open('../cal_usdaa/CalVerNum.h.new', 'w')
while 1:
	line = ver.readline()
	if line:
		defineMaj = '#define CAL_VER_MAJOR'
		defineMin = '#define CAL_VER_MINOR'
		defineDot = '#define CAL_VER_DOT'
		defineBld = '#define CAL_VER_BUILD'
		pos = line.find(defineMaj)
		if 0 == pos:
			vMaj = string.strip(line[pos+len(defineMaj):])
		pos = line.find(defineMin)
		if 0 == pos:
			vMin = string.strip(line[pos+len(defineMin):])
		pos = line.find(defineDot)
		if 0 == pos:
			vDot = string.strip(line[pos+len(defineDot):])
		pos = line.find(defineBld)
		if 0 == pos:
			vBldOld = string.strip(line[pos+len(defineBld):])
			vBld = str((datetime.date.today() - datetime.date(2002,12,28)).days)
			if not vBldOld == vBld:
				update = 1
				print >>verOut, defineBld + '\t\t\t\t\t' + vBld
			else:
				print >>verOut, line,
		else:
			print >>verOut, line,
	else:
		break
ver.close()
verOut.close()
if update:
	print "../cal_usdaa/CalVerNum.h updated to " + vMaj + '.' + vMin + '.' + vDot + '.' + vBld
	os.remove('../cal_usdaa/CalVerNum.h')
	os.rename('../cal_usdaa/CalVerNum.h.new', '../cal_usdaa/CalVerNum.h')
else:
	print "../cal_usdaa/CalVerNum.h is up-to-date"
	os.remove('../cal_usdaa/CalVerNum.h.new')

update = 0
conf = open('../../configure.in', 'r')
confOut = open('../../configure.in.new', 'wb')
while 1:
	line = conf.readline()
	if line:
		if 0 == line.find('AC_INIT('):
			newLine = 'AC_INIT(Agility Record Book, ' + vMaj + '.' + vMin + '.' + vDot + '.' + vBld + ', [help@agilityrecordbook.com])\n'
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
	print "../../configure.in updated to " + vMaj + '.' + vMin + '.' + vDot + '.' + vBld
	os.remove('../../configure.in')
	os.rename('../../configure.in.new', '../../configure.in')
else:
	print "../../configure.in is up-to-date"
	os.remove('../../configure.in.new')
