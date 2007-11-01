# Generate MSI files
#
# Revision History
# 2007-10-31 DRC Changed from WiX to InnoSetup
# 2007-03-07 DRC Created

"""GenMSI.py [/32] [/64] [/w98] [/all] [/notidy]
32: Create 32bit Unicode msi
64: Create 64bit Unicode msi
w98: Create 32bit MBCS msi
all: Create all of them (default)
notidy: Do not clean up .iss files
"""

import datetime
import os
import string
import sys

AgilityBookDir = "..\\..\\.."
InnoDir = "c:\\Program Files\\Inno Setup 5"
ISTool = "c:\\Program Files\\ISTool"
WinSrcDir = AgilityBookDir + "\\src\\Win"
Compiler = "VC8"
ResString = "FILEVERSION "
code32 = 1
code64 = 3
code98 = 2

def getversion():
	ver = "0.0.0.0"
	res = open(WinSrcDir + "\\AgilityBook.rc", "r")
	while (1):
		line = res.readline()
		if line:
			line = string.strip(line)
			pos = line.find(ResString)
			#FILEVERSION 1,9,0,12
			if 0 == pos:
				line = line.split(' ')
				line = line[1].split(',')
				ver = line[0] + '.' + line[1] + '.' + line[2] + '.' + line[3]
				break
		else:
			break
	return ver

#def genuuid():
#	(childin, childout) = os.popen4("uuidgen -c")
#	childin.close()
#	line = string.rstrip(childout.readline())
#	#line = string.upper(line) [-c means upper, leaving in case I change how guids are gotten]
#	childout.close()
#	return line

def runcmd(command):
	(childin, childout) = os.popen4(command)
	childin.close()
	while (1):
		line = childout.readline()
		if line:
			print >>sys.stdout, line,
		else:
			break

# Code:
#  1: Win32/Unicode
#  2: Win32/MBCS
#  3: Win64/Unicode
def genmsi(version, code, tidy):
	outputFile = ""
	baseDir = ""
	if code32 == code:
		outputFile = "AgilityBook"
		baseDir = AgilityBookDir + "\\bin\\" + Compiler + "Win32\\Unicode Release\\"
	elif code98 == code:
		outputFile = "AgilityBook-w98"
		baseDir = AgilityBookDir + "\\bin\\" + Compiler + "Win32\\Release\\"
	elif code64 == code:
		outputFile = "AgilityBook-x64"
		baseDir = AgilityBookDir + "\\bin\\" + Compiler + "x64\\Unicode Release\\"
	else:
		raise Exception, "Invalid code"
	if tidy and not os.access(baseDir + "AgilityBook.exe", os.F_OK):
		print baseDir + "AgilityBook.exe does not exist, MSI skipped"
		return 0
	setup = open(outputFile + ".iss", "w")
	print >>setup, '#define ARBName "Agility Record Book"'
	print >>setup, '#define ARBVersion "' + version + '"'
	print >>setup, ''
	print >>setup, '[Setup]'
	print >>setup, 'AppName={#ARBName}'
	print >>setup, 'AppId={#ARBName} Setup'
	print >>setup, 'AppVersion={#ARBVersion}'
	print >>setup, 'AppVerName={#ARBName} {#ARBVersion}'
	print >>setup, 'AppPublisher=David Connet'
	print >>setup, 'AppPublisherURL=http://www.agilityrecordbook.com/'
	print >>setup, 'AppSupportURL=http://www.agilityrecordbook.com/'
	print >>setup, 'AppUpdatesURL=http://www.agilityrecordbook.com/'
	print >>setup, 'DefaultDirName={pf}\dcon Software\{#ARBName}'
	print >>setup, 'DefaultGroupName={#ARBName}'
	print >>setup, 'Compression=lzma/ultra'
	print >>setup, 'InternalCompressLevel=ultra'
	print >>setup, 'SolidCompression=true'
	print >>setup, 'UninstallDisplayIcon={app}\AgilityBook.exe'
	print >>setup, 'SetupIconFile=..\\..\\win\\res\\AgilityBook.ico'
	print >>setup, 'MinVersion=4.90.3000,4.0.1381'
	print >>setup, 'PrivilegesRequired=none'
	print >>setup, 'OutputBaseFilename=ARBSetup_{#ARBVersion}'
	print >>setup, 'OutputDir=' + baseDir + 'Setup'
	print >>setup, ''
	print >>setup, '[Files]'
	print >>setup, 'Source: ' + baseDir + 'AgilityBook.exe; DestDir: {app}'
	print >>setup, 'Source: ' + baseDir + 'AgilityBook.chm; DestDir: {app}'
	print >>setup, 'Source: ' + baseDir + 'AgilityBookFRA.dll; DestDir: {app}'
	print >>setup, 'Source: ' + baseDir + 'cal_usdaa.dll; DestDir: {app}'
	print >>setup, ''
	print >>setup, '[Icons]'
	print >>setup, 'Name: {commondesktop}\{#ARBName}; Filename: {app}\AgilityBook.exe'
	print >>setup, 'Name: {group}\{#ARBName}; Filename: {app}\AgilityBook.exe; IconFilename: {app}\AgilityBook.exe; IconIndex: 0'
	print >>setup, 'Name: {group}\{#ARBName} Help; Filename: {app}\AgilityBook.chm'
	print >>setup, 'Name: {group}\{#ARBName} Web Site; Filename: http://www.agilityrecordbook.com/'
	print >>setup, 'Name: {group}\Yahoo Discussion Group; Filename: http://groups.yahoo.com/group/AgilityRecordBook/'
	setup.close()

	if os.access(baseDir + "AgilityBook.exe", os.F_OK):
		fullpath = os.getcwd() + '\\' + outputFile + '.iss'
		runcmd('istool -compile ' + fullpath)
		if tidy:
			if os.access(outputFile + ".iss", os.F_OK):
				os.remove(outputFile + ".iss")
	return 1

def main():
	tidy = 1
	b32 = 0
	b64 = 0
	b98 = 0
	for o in sys.argv:
		if o == "/32":
			b32 = 1
		elif o == "/64":
			b64 = 1
		elif o == "/w98":
			b98 = 1
		elif o == "/all":
			b32 = 1
			b64 = 1
			b98 = 1
		elif o == "/notidy":
			tidy = 0

	if b32 + b64 + b98 == 0:
		b32 = 1
		b64 = 1
		b98 = 1

	version = getversion()
	os.environ['PATH'] += ';' + ISTool

	b32ok = 0
	b64ok = 0
	b98ok = 0

	if b32:
		if genmsi(version, code32, tidy):
			b32ok = 1
	if b64:
		if genmsi(version, code64, tidy):
			b64ok = 1
	if b98:
		if genmsi(version, code98, tidy):
			b98ok = 1

	#if b32ok or b64ok or b98ok:
	#	d = datetime.datetime.now().isoformat(' ')
	#	codes = open(AgilityBookDir + "\\Misc\\Installation\\InstallGUIDs.csv", "a")
	#	installs = ""
	#	if b32ok:
	#		installs = installs + ",win32"
	#	if b64ok:
	#		installs = installs + ",x64"
	#	if b98ok:
	#		installs = installs + ",win98"
	#	print >>codes, "v" + version + "," + d + "," + productId + "," + UpgradeCode + "," + Compiler + installs

main()
