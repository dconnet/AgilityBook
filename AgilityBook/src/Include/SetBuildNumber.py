# Update build number in VersionNumber.h (Number of days since ARB was born)
# and ../../configure.in
#
# This assumes the current directory is 'Include'

import datetime
import os
import string
import sys

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
			vBld = str((datetime.date.today() - datetime.date(2002,12,28)).days)
			print >>verOut, defineBld + '\t\t\t\t\t' + vBld
		else:
			print >>verOut, line,
	else:
		break
ver.close()
verOut.close()
os.remove('VersionNumber.h')
os.rename('VersionNumber.h.new', 'VersionNumber.h')

conf = open('../../configure.in', 'r')
confOut = open('../../configure.in.new', 'wb')
while 1:
	line = conf.readline()
	if line:
		if 0 == line.find('AC_INIT('):
			print >>confOut, 'AC_INIT(Agility Record Book, ' + vMaj + '.' + vMin + '.' + vDot + '.' + vBld + ', [help@agilityrecordbook.com])'
		else:
			print >>confOut, line,
	else:
		break
conf.close()
confOut.close()
os.remove('../../configure.in.')
os.rename('../../configure.in.new', '../../configure.in')
