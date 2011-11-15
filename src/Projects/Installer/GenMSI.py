# Above line is for python
#
# Generate MSI files
#
# vbs scripts copied from:
# C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Samples\SysMgmt\Msi\Scripts
#
# Revision History
# 2011-11-14 DRC Treat wix warnings as errors
# 2011-09-23 DRC Allow support of admin installs "msiexec /a msi"
# 2011-08-19 DRC Fixed /test option (it was always writing the csv file)
# 2011-08-13 DRC Change to vc10 for default.
# 2011-04-27 DRC Generate unique prodcodes for each architecture.
#            Also discovered script wasn't writing the cvs file correctly.
# 2010-11-18 DRC Optimize the build by caching the cab file.
# 2010-11-16 DRC Turned on pedantic warnings, changed how languages are handled.
# 2010-10-08 DRC Suppress wixpdb creation.
#            Allow packaging to pull from specified VC build (8/9/10)
# 2010-08-25 DRC Stop using old prodcode. Detect same version as old.
# 2010-06-11 DRC Convert vbs scripts to python
# 2010-05-22 DRC Added /test2 option.
# 2010-05-07 DRC Added /user option (default: perMachine)
#            Merge languages into one msi.
# 2009-12-23 DRC Auto-detect current WiX installation (%WIX%)
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

"""GenMSI.py [/wix path] [/user] [/32] [/64] [/all] [/notidy] [/test] [/VC[8|9|10]]
	wix: Override internal wix path (c:\Tools\wix3)
	user: Create msi as a per-user install (default: per-machine)
	32: Create 32bit Unicode msi
	64: Create 64bit Unicode msi
	all: Create all of them (default)
	notidy: Do not clean up generated files
	test: Generate .msi for test purposes (don't write to InstallGUIDs.csv)
	VC: Generate msi using specified vc version (Default: 10)
"""

import datetime
import glob
import msilib
import os
import stat
import string
import sys

# Where top-level AgilityBook directory is relative to this script.
AgilityBookDir = r'..\..\..'

# Where is WiX? Can be overriden on command line.
# This is initially set to %WIX%/bin, if WiX is actually installed.
WiXdir = r'c:\Tools\wix3'

WinSrcDir = AgilityBookDir + r'\src'

# Code:
#  1: Win32/Unicode
#  2: Win32/MBCS
#  3: Win64/Unicode
code32 = 1
code64 = 3

# This is only used to update the InstallGUIDs file. Make sure it stays in
# sync with the current code in AgilityBook.wxi (UPGRADECODE)
UpgradeCode = '4D018FAD-2CBC-4A92-B6AC-4BAAECEED8F4'

# Name used for (wix culture name, language id) when building msi.
supportedLangs = [
	('en-us', '1033'),
	('fr-fr', '1036')]
# List of mo files to be included in installer in xx_YY format.
# (xx is ISO 639 language code, YY is ISO 3166 country code)
langNames = 'en_US;fr_FR'


def errprint(*args):
	# Used to print exception messages
	strings = map(str, args)
	msg = ' '.join(strings)
	if msg[-1:] != '\n':
		msg += '\n'
	sys.stderr.write(msg)


# Kill an entire directory
def RmMinusRF(name):
	if os.access(name, os.F_OK):
		mode = os.stat(name)[stat.ST_MODE]
		if stat.S_ISDIR(mode):
			curDir = os.getcwd()
			os.chdir(name)
			for file in glob.glob('./*'):
				RmMinusRF(file)
			os.chdir(curDir)
			os.rmdir(name)
		else:
			if not os.access(name, os.W_OK):
				os.chmod(name, 0666)
			if os.access(name, os.W_OK):
				try:
					os.remove(name)
				except OSError, msg:
					errprint('os.remove:', msg)


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


# returns baseDir, outputFile
def getoutputvars(code, version, vcver):
	outputFile = ''
	baseDir = ''
	if code32 == code:
		outputFile = 'AgilityBook-' + version + '-win'
		baseDir = AgilityBookDir + r'\bin\VC' + vcver + 'Win32\Release'
	elif code64 == code:
		outputFile = 'AgilityBook-' + version + '-x64'
		baseDir = AgilityBookDir + r'\bin\VC' + vcver + 'x64\Release'
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


# This is the old WiSubStg.vbs script
#runcmd('cscript /nologo WiSubStg.vbs ' + baseMsi + ' ' + langId + '.mst ' + langId)
# No error checking - just let the exception kill it.
def WiSubStg(baseMsi, langId):
	database = msilib.OpenDatabase(baseMsi, msilib.MSIDBOPEN_TRANSACT)
	view = database.OpenView("SELECT `Name`,`Data` FROM _Storages")
	record = msilib.CreateRecord(2)
	record.SetString(1, langId)
	view.Execute(record)
	record.SetStream(2, langId + '.mst')
	view.Modify(msilib.MSIMODIFY_ASSIGN, record)
	database.Commit()
	view = None
	database = None


# This is the old WiLangId.vbs script
#runcmd('cscript /nologo WiLangId.vbs ' + baseMsi + ' Package ' + sumInfoStream)
# No error checking - just let the exception kill it.
def WiLangId(baseMsi, sumInfoStream):
	database = msilib.OpenDatabase(baseMsi, msilib.MSIDBOPEN_TRANSACT)
	sumInfo = database.GetSummaryInformation(1)
	template = sumInfo.GetProperty(7)
	iDelim = template.find(';')
	platform = ';'
	if 0 <= iDelim:
		platform = template[0:iDelim+1]
	sumInfo.SetProperty(7, platform + sumInfoStream)
	sumInfo.Persist()
	database.Commit()
	database = None


def WriteCode(baseMsi, ver4Dot, code, vcver):
	database = msilib.OpenDatabase(baseMsi, msilib.MSIDBOPEN_READONLY)
	view = database.OpenView("SELECT `Value` FROM Property WHERE `Property`='ProductCode'")
	record = msilib.CreateRecord(1)
	view.Execute(record)
	data = view.Fetch()
	view = None
	database = None
	prodcode = data.GetString(1)

	if len(prodcode) > 0:
		d = datetime.datetime.now().isoformat(' ')
		codes = open(AgilityBookDir + r'\Misc\InstallGUIDs.csv', 'a')
		installs = ''
		if code == code64:
			installs = 'VC' + vcver + ',x64'
		else:
			installs = 'VC' + vcver + ',win32'
		print >>codes, 'v' + ver4Dot + ',' + d + ',' + prodcode + ',' + UpgradeCode + ',' + installs


def genWiX(ver3Dot, ver4Dot, ver4Line, code, tidy, perUser, testing, vcver):
	baseDir, outputFile = getoutputvars(code, ver4Line, vcver)
	if tidy and not os.access(baseDir + r'\AgilityBook.exe', os.F_OK):
		print baseDir + r'\AgilityBook.exe does not exist, MSI skipped'
		return 0

	if os.access(baseDir + r'\AgilityBook.exe', os.F_OK):
		cabcache = 'cabcache'
		RmMinusRF(cabcache)
		# -wx: Treat warnings as errors
		candleCmd = 'candle -nologo -wx -pedantic'
		candleCmd += ' -dCURRENT_VERSION=' + ver3Dot
		if code == code64:
			candleCmd += ' -arch x64'
		else:
			candleCmd += ' -arch x86'
		candleCmd += ' -dBASEDIR="' + baseDir + '"'
		candleCmd += ' -dSUPPORTED_LANGS=' + langNames
		candleCmd += ' -dINSTALL_SCOPE=' + perUser
		runcmd(candleCmd + ' AgilityBook.wxs')
		processing = 0
		baseMsi = ''
		sumInfoStream = ''
		for culture, langId in supportedLangs:
			processing += 1
			basename = outputFile
			if processing > 1:
				basename += '_' + culture
			# -wx: Treat warnings as errors
			# -sadv: Suppress AdvtExecuteSequence table
			lightCmd = 'light -nologo -wx -pedantic -spdb -sadv'
			lightCmd += ' -dcl:high -cc ' + cabcache
			if not processing == 1:
				lightCmd += ' -reusecab'
			lightCmd += ' -ext WixUIExtension -ext WixUtilExtension'
			lightCmd += ' -dWixUILicenseRtf=License_' + culture + '.rtf'
			lightCmd += ' -cultures:' + culture
			lightCmd += ' -loc AgilityBook_' + culture + '.wxl'
			lightCmd += ' -out ' + basename + '.msi'
			runcmd(lightCmd + ' AgilityBook.wixobj')
			if processing == 1:
				baseMsi = basename + '.msi'
				sumInfoStream += langId
			else:
				sumInfoStream += ',' + langId
				runcmd('torch -nologo -p -t language ' + baseMsi + ' ' + basename + '.msi -out ' + langId + '.mst')
				WiSubStg(baseMsi, langId)
			if tidy:
				if os.access(langId + '.mst', os.F_OK):
					os.remove(langId + '.mst')
				if processing > 1:
					if os.access(basename + '.msi', os.F_OK):
						os.remove(basename + '.msi')
		if processing > 1:
			WiLangId(baseMsi, sumInfoStream)
		if tidy:
			RmMinusRF(cabcache)
		if not testing:
			WriteCode(baseMsi, ver4Dot, code, vcver)
	else:
		print baseDir + r'\AgilityBook.exe does not exist, MSI skipped'

	if tidy:
		if os.access('AgilityBook.wixobj', os.F_OK):
			os.remove('AgilityBook.wixobj')

	return 1


def main():
	global WiXdir
	# When WiX is installed, it sets "WIX" to point to the top-level directory
	if os.environ.has_key('WIX'):
		WiXdir = os.environ['WIX'] + r'\bin'
	# This must be set to a valid Package/@InstallScope value.
	perUser = "perMachine"
	b32 = 0
	b64 = 0
	tidy = 1
	testing = 0
	vcver = '10'
	if 1 == len(sys.argv):
		print 'Setting /32 /test'
		b32 = 1
		testing = 1
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
		elif o == '/user':
			perUser = "perUser"
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
			testing = 1
		elif o == '/VC8':
			vcver = '8'
		elif o == '/VC9':
			vcver = '9'
		elif o == '/VC10':
			vcver = '10'
		else:
			error = 1
			break
		i += 1
	if error:
		print 'Usage:', __doc__
		return 1

	if b32 + b64 == 0:
		b32 = 1
		b64 = 1

	ver3Dot, ver3Line = getversion(3)
	ver4Dot, ver4Line = getversion(4)

	# Wix
	os.environ['PATH'] += ';' + WiXdir
	if b32:
		genWiX(ver3Dot, ver4Dot, ver4Line, code32, tidy, perUser, testing, vcver)
	if b64:
		genWiX(ver3Dot, ver4Dot, ver4Line, code64, tidy, perUser, testing, vcver)

	return 0


if __name__ == '__main__':
	sys.exit(main())
