# coding=utf-8
# Above line is for python
#
# Generate MSI files
#
# Revision History
# 2009-10-19 DRC Added 'create' option.
# 2009-08-08 DRC Tweaked code to remove ICE61 warning.
#            Allow user specification of where WiX is (/wix)
# 2009-07-26 DRC Removed Win98 support.
# 2009-05-02 DRC Upgraded to wix3.
# 2009-04-16 DRC Enable v2 to upgrade v1 when the beta period ends.
#            Removed Inno support since I don't maintain it anymore.
# 2009-03-01 DRC Made v2 coexist with v1, updated for wxWidgets
#            Fixed bug where french and arbhelp would always be installed
# 2008-10-14 DRC Added x64 specific tags
# 2007-11-14 DRC Wix works! (it's now the default)
# 2007-11-05 DRC Add WiX back (addition)
#            Note, WiX doesn't work yet, that's why I added Inno
# 2007-10-31 DRC Changed from WiX to InnoSetup
# 2007-03-07 DRC Created

"""GenMSI.py [/wix path] [/32] [/64] [/all] [/notidy] [/test] [/create]
	wix: Override internal wix path (c:\Tools\wix3)
	32: Create 32bit Unicode msi
	64: Create 64bit Unicode msi
	all: Create all of them (default)
	notidy: Do not clean up generated files
	test: Generate .msi for test purposes (don't write to InstallGUIDs.csv or check for existing versions)
	create: Only create the wix files, do not compile.
"""

import datetime
import os
import string
import sys

# Where top-level AgilityBook directory is relative to this script.
AgilityBookDir = r'..\..\..'

# Where is WiX? Can be overriden on command line.
WiXdir = r'c:\Tools\wix3'

WinSrcDir = AgilityBookDir + r'\src'
code32 = 1
code64 = 3

# This is only used to update the InstallGUIDs file. Make sure it stays in
# sync with the current code in AgilityBook.wxi
UpgradeCode = '4D018FAD-2CBC-4A92-B6AC-4BAAECEED8F4'

# Name used for filenames, culture name
supportedLangs = [
	('en', 'en-us'),
	('fr', 'fr-fr')]


def getversion(numParts):
	ver = '0'
	ver2 = '0'
	for i in range(1, numParts):
		ver = ver + '.0'
		ver2 = ver2 + '_0'
	resStr = [
		'#define ARB_VER_MAJOR',
		'#define ARB_VER_MINOR',
		'#define ARB_VER_DOT',
		'#define ARB_VER_BUILD'
		]
	found = 0;
	version = ['0', '0', '0', '0']
	res = open(WinSrcDir + r'\Include\VersionNumber.h', 'r')
	while (1):
		line = res.readline()
		if line:
			line = line.strip()
			for i in range(0, 4):
				pos = line.find(resStr[i])
				if 0 == pos:
					found = found + 1
					version[i] = line[pos+len(resStr[i]):].strip()
			if found == 4:
				break
		else:
			break
	ver = version[0]
	ver2 = version[0]
	for i in range(1, numParts):
		ver = ver + '.' + version[i]
		ver2 = ver2 + '_' + version[i]
	return ver, ver2


def genuuid():
	(childin, childout) = os.popen4('uuidgen -c')
	childin.close()
	line = childout.readline().rstrip()
	#line = line.upper() [-c means upper, leaving in case I change how guids are gotten]
	childout.close()
	return line


# returns baseDir, outputFile
def getoutputvars(code, version):
	outputFile = ''
	baseDir = ''
	if code32 == code:
		outputFile = 'AgilityBook-' + version + '-win'
		baseDir = AgilityBookDir + r'\bin\VC9Win32\Release'
	elif code64 == code:
		outputFile = 'AgilityBook-' + version + '-x64'
		baseDir = AgilityBookDir + r'\bin\VC9x64\Release'
	else:
		raise Exception, 'Invalid code'
	return baseDir, outputFile


def runcmd(command):
	print command
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
def genWiX(productId, ver3Dot, ver4Line, code, tidy, bTesting):
	baseDir, outputFile = getoutputvars(code, ver4Line)
	if tidy and not os.access(baseDir + r'\AgilityBook.exe', os.F_OK):
		print baseDir + r'\AgilityBook.exe does not exist, MSI skipped'
		return 0

	if not bTesting == 2:
		if os.access(baseDir + r'\AgilityBook.exe', os.F_OK):
			candleCmd = 'candle -nologo'
			candleCmd += ' -dCURRENT_VERSION=' + ver3Dot
			if code == code64:
				candleCmd += ' -arch x64'
			else:
				candleCmd += ' -arch x86'
			if bTesting:
				candleCmd += ' -dTESTING'
			candleCmd += ' -dBASEDIR="' + baseDir + '"'
			candleCmd += ' -dPRODUCTID=' + productId
			runcmd(candleCmd + ' AgilityBook.wxs')
			lightCmd = 'light -nologo -ext WixUIExtension -ext WixUtilExtension '
			for fname, culture in supportedLangs:
				basename = outputFile + '-' + fname
				runcmd(lightCmd + '-dWixUILicenseRtf=License-' + fname + '.rtf -cultures:' + culture + ' -loc AgilityBook-' + fname + '.wxl -out ' + basename + '.msi AgilityBook.wixobj')
				if tidy:
					if os.access(basename + '.wixpdb', os.F_OK):
						os.remove(basename + '.wixpdb')
		if tidy:
			if os.access('AgilityBook.wixobj', os.F_OK):
				os.remove('AgilityBook.wixobj')
		else:
			print baseDir + r'\AgilityBook.exe does not exist, MSI skipped'
	return 1


def main():
	global WiXdir
	b32 = 0
	b64 = 0
	tidy = 1
	bTesting = 0
	if 1 == len(sys.argv):
		b32 = 1
		bTesting = 1
	error = 0
	i = 1
	while i < len(sys.argv):
		o = sys.argv[i]
		if o == '/wix':
			if i == len(sys.argv) - 1:
				error = 1
				break;
			WiXdir = sys.argv[i+1]
			i += 1
		elif o == '/32':
			b32 = 1
		elif o == '/64':
			b64 = 1
		elif o == '/all':
			b32 = 1
			b64 = 1
		elif o == '/notidy':
			tidy = 0
		elif o == '/test':
			bTesting = 1
		elif o == '/create':
			bTesting = 2
			tidy = 0
		else:
			error = 1
			break
		i += 1
	if error:
		print 'Usage:', __doc__
		return

	if b32 + b64 == 0:
		b32 = 1
		b64 = 1

	b32ok = 0
	b64ok = 0

	productId = genuuid()
	ver3Dot, ver3Line = getversion(3)
	ver4Dot, ver4Line = getversion(4)

	# Get the last generated info. By doing this, we can maintain the same
	# product id while doing beta builds. This will force the user to uninstall
	# the existing version first since we're only bumping the build number,
	# not the actual version number.
	#
	# While the detect-same-version CA will do that during install, this has
	# the added benefit of immediately bailing during the install, rather than
	# showing the final 'failed' dialog.
	if bTesting == 1:
		print 'Remember, uninstall the old version first. This does not check.'
	else:
		codes = open(AgilityBookDir + r'\Misc\InstallGUIDs.csv', 'r')
		items = []
		while (1):
			#version,date,productid,upgradecode,compiler,target
			#v2.0.0.2304,2009-04-19 21:13:36.703000,31B601BB-5343-48E1-A96E-79EDEBEED9E0,4D018FAD-2CBC-4A92-B6AC-4BAAECEED8F4,VC9,win32
			line = codes.readline()
			if line:
				line = line.strip()
				if 0 < len(line):
					tmp = line.split(',', 4)
					if 5 == len(tmp):
						items = tmp
			else:
				break
		codes.close()
		if 0 == len(items):
			print 'Error: Could not parse existing GUIDs!'
		lastVersion = items[0][1:]
		lastVersion = lastVersion[0:lastVersion.rindex('.')]
		# If the version numbers are the same, use the same productId
		if lastVersion == ver3Dot:
			productId = items[2]

	# Wix
	os.environ['PATH'] += ';' + WiXdir
	if b32:
		if genWiX(productId, ver3Dot, ver4Line, code32, tidy, bTesting):
			b32ok = 1
	if b64:
		if genWiX(productId, ver3Dot, ver4Line, code64, tidy, bTesting):
			b64ok = 1
	if bTesting == 0 and (b32ok or b64ok):
		d = datetime.datetime.now().isoformat(' ')
		codes = open(AgilityBookDir + r'\Misc\InstallGUIDs.csv', 'a')
		installs = ''
		if b32ok:
			installs = 'VC9,win32'
		if b64ok:
			installs = 'VC9,x64'
		print >>codes, 'v' + ver4Dot + ',' + d + ',' + productId + ',' + UpgradeCode + ',' + installs


main()
