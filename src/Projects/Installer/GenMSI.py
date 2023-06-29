# coding=utf-8
# Above line is for python
#
# Generate MSI files
# Must be run from the directory this file is in.
#
# vbs scripts copied from:
# C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Samples\SysMgmt\Msi\Scripts
#
# Revision History
# 2023-06-29 Replace msilib usage with direct COM (msilib is deprecated in py3.13)
# 2022-04-10 Change default compiler to vc143
# 2021-11-11 Add vc143 support, changed default (no args) to '-b all'
# 2020-01-26 Change default compiler to vc142
# 2019-09-21 Change to getopt
# 2019-09-20 Create distribution files automagically.
# 2017-05-10 Fix vcver/platformTools issues.
# 2017-04-09 Added vc141 support.
# 2016-10-19 Changed RmMinusRF to using shutil.rmtree()
# 2016-07-01 Add signing
# 2016-06-10 Convert to Python3
# 2015-07-23 Added vc14 support.
# 2014-11-17 wix3.9 deprecated -sadv
# 2014-11-17 Removed .mo files (moved into resource)
# 2013-11-27 Fix arbdll.dll access when building x64.
# 2013-08-15 Added support for multiple wxs files.
# 2013-08-12 Added vc11/12 support, remove pre-10 support.
# 2012-06-01 Changed VC10 output directory
# 2011-11-23 Add internet shortcut to start menu
# 2011-11-14 Treat wix warnings as errors
# 2011-09-23 Allow support of admin installs "msiexec /a msi"
# 2011-08-19 Fixed /test option (it was always writing the csv file)
# 2011-08-13 Change to vc10 for default.
# 2011-04-27 Generate unique prodcodes for each architecture.
#            Also discovered script wasn't writing the cvs file correctly.
# 2010-11-18 Optimize the build by caching the cab file.
# 2010-11-16 Turned on pedantic warnings, changed how languages are handled.
# 2010-10-08 Suppress wixpdb creation.
#            Allow packaging to pull from specified VC build (8/9/10)
# 2010-08-25 Stop using old prodcode. Detect same version as old.
# 2010-06-11 Convert vbs scripts to python
# 2010-05-22 Added /test2 option.
# 2010-05-07 Added /user option (default: perMachine)
#            Merge languages into one msi.
# 2009-12-23 Auto-detect current WiX installation (%WIX%)
# 2009-10-19 Added 'create' option.
# 2009-08-08 Tweaked code to remove ICE61 warning.
#            Allow user specification of where WiX is (/wix)
# 2009-07-26 Removed Win98 support.
# 2009-05-02 Upgraded to wix3.
# 2009-04-16 Enable v2 to upgrade v1 when the beta period ends.
#            Removed Inno support since I don't maintain it anymore.
# 2009-03-01 Made v2 coexist with v1, updated for wxWidgets
#            Fixed bug where french and arbhelp would always be installed
# 2008-10-14 Added x64 specific tags
# 2007-11-14 Wix works! (it's now the default)
# 2007-11-05 Add WiX back (addition)
#            Note, WiX doesn't work yet, that's why I added Inno
# 2007-10-31 Changed from WiX to InnoSetup
# 2007-03-07 Created

"""GenMSI.py [-w path] [-b user|32|64|all]* [-x] [-e] [-t target] [-d distrib]
	-w: Override internal wix path (c:\Tools\wix3, default %WIX%)
	-b: Build target (multiple can be specified
	    user: Create msi as a per-user install (default: per-machine)
	    32: Create 32bit Unicode msi
	    64: Create 64bit Unicode msi
	    all: Create all of them (default)
	-x: Do not clean up generated files
	-e: Generate .msi for test purposes (don't write to InstallGUIDs.csv)
	-t target: Generate msi using specified vc version (Default: vc142)
	-d distrib: Directory to write distribution files to (Default: ./distrib)
"""

import datetime
import getopt
import os
import pythoncom
import shutil
import stat
import string
import subprocess
import sys
import win32com.client as wc
import zipfile

msiOpenDatabaseModeReadOnly = 0
msiOpenDatabaseModeTransact = 1
msiViewModifyAssign = 3

# Where top-level AgilityBook directory is relative to this script.
AgilityBookDir = r'..\..\..'

# Where to assemble all the files
DistribDir = r'.\distrib'

FilesToCopy = [
	'AgilityBook.pdb',
	'AgilityBook.exe',
	'ARBHelp.pdb',
	'ARBHelp.exe',
	'ARBUpdater.pdb',
	'ARBUpdater.exe',
	'CalSites.pdb',
	'CalSites.exe']

# Where is WiX? Can be overriden on command line.
# This is initially set to %WIX%/bin, if WiX is actually installed.
WiXdir = r'c:\Tools\wix3'

# This is only used to update the InstallGUIDs file. Make sure it stays in
# sync with the current code in AgilityBook.wxi (UPGRADECODE)
UpgradeCode = '4D018FAD-2CBC-4A92-B6AC-4BAAECEED8F4'

# Name used for (wix culture name, language id) when building msi.
supportedLangs = [
	('en-us', '1033'),
	('fr-fr', '1036')]

wxsFiles = [
	('AgilityBook'),
	('Dialogs')]

CA_dll = 'arbdll.dll'


def errprint(*args):
	# Used to print exception messages
	strings = list(map(str, args))
	msg = ' '.join(strings)
	if msg[-1:] != '\n':
		msg += '\n'
	sys.stderr.write(msg)


# Kill an entire directory
def del_rw(func, path, exc_info):
	os.chmod(path, stat.S_IWUSR)
	func(path)
def RmMinusRF(name):
	if os.access(name, os.F_OK):
		mode = os.stat(name)[stat.ST_MODE]
		if stat.S_ISDIR(mode):
			shutil.rmtree(name, onerror=del_rw)
		else:
			if not os.access(name, os.W_OK):
				os.chmod(name, 0o666)
			if os.access(name, os.W_OK):
				try:
					os.remove(name)
				except OSError as msg:
					errprint('os.remove:', msg)


def getversion(numParts):
	global AgilityBookDir
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
	res = open(AgilityBookDir + r'\AgilityBookLibs\Include\VersionNumber.h', 'r')
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
def getoutputvars(b32, version, vcver):
	global AgilityBookDir
	outputFile = ''
	baseDir = ''
	if b32:
		outputFile = 'AgilityBook-' + version + '-x86'
		baseDir = AgilityBookDir + r'\bin\vc' + vcver + 'x86\Release'
		distDir = 'vc' + vcver
	else:
		outputFile = 'AgilityBook-' + version + '-x64'
		baseDir = AgilityBookDir + r'\bin\vc' + vcver + 'x64\Release'
		distDir = 'vc' + vcver + 'x64'
	return baseDir, outputFile, distDir


def runcmd(command):
	print(command)
	newenv = os.environ.copy()
	proc = subprocess.Popen(command, env=newenv)
	proc.wait()
	if not proc.returncode == 0:
		raise Exception('Command returned non-zero code')


# This is the equivalent of msilib's 'comobj.StringData(index)'
def GetProperty(comobj, prop, index):
	dispid = comobj.GetIDsOfNames(prop)
	# the '1' after 'GET' is 'yes, I want the return value'
	# https://stackoverflow.com/questions/72531735/setting-value-of-a-iterable-property-of-a-python-com-object
	return comobj.Invoke(dispid, 0, pythoncom.INVOKE_PROPERTYGET, 1, index)


def SetProperty(comobj, prop, index, data):
	dispid = comobj.GetIDsOfNames(prop)
	comobj.Invoke(dispid, 0, pythoncom.INVOKE_PROPERTYPUT, 0, index, data)


# No error checking - just let the exception kill it.
def WiSubStg(baseMsi, langId):
	installer = wc.Dispatch('WindowsInstaller.Installer')
	database = installer.OpenDatabase(baseMsi, msiOpenDatabaseModeTransact)
	view = database.OpenView("SELECT `Name`,`Data` FROM _Storages")
	record = installer.CreateRecord(2)
	SetProperty(record._oleobj_, 'StringData', 1, langId)
	view.Execute(record)
	record.SetStream(2, langId + '.mst')
	view.Modify(msiViewModifyAssign, record)
	database.Commit()
	del view
	del database
	del installer


# No error checking - just let the exception kill it.
def WiLangId(baseMsi, sumInfoStream):
	installer = wc.Dispatch('WindowsInstaller.Installer')
	database = installer.OpenDatabase(baseMsi, msiOpenDatabaseModeTransact)
	sumInfo = GetProperty(database._oleobj_, 'SummaryInformation', 1)
	template = GetProperty(sumInfo, 'Property', 7)
	iDelim = template.find(';')
	platform = ';'
	if 0 <= iDelim:
		platform = template[0:iDelim+1]
	SetProperty(sumInfo, 'Property', 7, platform + sumInfoStream)
	dispid = sumInfo.GetIDsOfNames('Persist')
	sumInfo.Invoke(dispid, 0, pythoncom.INVOKE_FUNC, 0)
	database.Commit()
	del database
	del installer


# No error checking - just let the exception kill it.
def WiProdCode(baseMsi):
	installer = wc.Dispatch('WindowsInstaller.Installer')
	database = installer.OpenDatabase(baseMsi, msiOpenDatabaseModeReadOnly)
	view = database.OpenView("SELECT `Value` FROM Property WHERE `Property`='ProductCode'")
	view.Execute(None)
	record = view.Fetch()
	prodcode = GetProperty(record._oleobj_, 'StringData', 1)
	del view
	del database
	del installer
	return prodcode


def WriteCode(baseMsi, ver4Dot, b32, vcver):
	global AgilityBookDir, UpgradeCode
	prodcode = WiProdCode(baseMsi)

	# {BAB76981-245D-452C-92ED-FCC9DBF2D914}
	if len(prodcode) == 38:
		d = datetime.datetime.now().isoformat(' ')
		codes = open(AgilityBookDir + r'\Misc\InstallGUIDs.csv', 'a')
		installs = ''
		if b32:
			installs = 'VC' + vcver + ',win32'
		else:
			installs = 'VC' + vcver + ',x64'
		print('v' + ver4Dot + ',' + d + ',' + prodcode + ',' + UpgradeCode + ',' + installs, file=codes)
	else:
		print(len(prodcode))
		print('Warning: Unknown ProductCode:', prodcode)
		print('Not recorded in InstallGUIDs.csv')


def GetWxsFiles(extension):
	global wxsFiles
	files = []
	for file in wxsFiles:
		files += [file + extension]
	return files


def GetWxsFilesAsString(extension):
	str = ''
	files = GetWxsFiles(extension)
	for file in files:
		str += ' ' + file
	return str


def CopyCAdll(ver4Line, vcver):
	global CA_dll
	baseDir, outputFile, distDir = getoutputvars(True, ver4Line, vcver)
	arbdll = baseDir + '\\' + CA_dll
	if not os.access(arbdll, os.F_OK):
		print(arbdll + r' does not exist, MSI skipped')
		return 0
	shutil.copy(arbdll, '.')
	return 1


def genWiX(ver3Dot, ver4Dot, ver4Line, b32, tidy, perUser, testing, vcver, distrib):
	global FilesToCopy, supportedLangs
	baseDir, outputFile, distDir = getoutputvars(b32, ver4Line, vcver)

	vcdir = os.path.join(distrib, distDir)
	if not os.access(vcdir, os.F_OK):
		os.mkdir(vcdir)

	for file in FilesToCopy:
		fullpath = os.path.join(baseDir, file)
		if not os.access(fullpath, os.F_OK):
			print(fullpath + r' does not exist, MSI skipped')
			return 0
		shutil.copy(fullpath, vcdir)

	cabcache = 'cabcache'
	RmMinusRF(cabcache)
	# -wx: Treat warnings as errors
	candleCmd = 'candle -nologo -wx -pedantic'
	candleCmd += ' -dCURRENT_VERSION=' + ver3Dot
	if b32:
		candleCmd += ' -arch x86'
	else:
		candleCmd += ' -arch x64'
	candleCmd += ' -ext WixUIExtension -ext WixUtilExtension'
	candleCmd += ' -dBASEDIR="' + baseDir + '"'
	candleCmd += ' -dINSTALL_SCOPE=' + perUser
	runcmd(candleCmd + GetWxsFilesAsString('.wxs'))
	processing = 0
	baseMsi = ''
	baseMsiZip = ''
	sumInfoStream = ''
	for culture, langId in supportedLangs:
		processing += 1
		basename = os.path.join(distrib, outputFile)
		if processing > 1:
			basename += '_' + culture
		# -wx: Treat warnings as errors
		lightCmd = 'light -nologo -wx -pedantic -spdb'
		lightCmd += ' -dcl:high -cc ' + cabcache
		if not processing == 1:
			lightCmd += ' -reusecab'
		lightCmd += ' -ext WixUIExtension -ext WixUtilExtension'
		lightCmd += ' -dWixUILicenseRtf=License_' + culture + '.rtf'
		lightCmd += ' -cultures:' + culture
		lightCmd += ' -loc AgilityBook_' + culture + '.wxl'
		lightCmd += ' -out ' + basename + '.msi'
		runcmd(lightCmd + GetWxsFilesAsString('.wixobj'))
		if processing == 1:
			baseMsi = basename + '.msi'
			baseMsiZip = basename + '.zip'
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
		WriteCode(baseMsi, ver4Dot, b32, vcver)
	runcmd(r'python ..\..\..\AgilityBookLibs\Projects\SignStuff.py ' + baseMsi)

	zip = zipfile.ZipFile(baseMsiZip, 'w')
	zip.write(baseMsi, outputFile + '.msi')
	zip.close()

	if tidy:
		for file in GetWxsFiles('.wixobj'):
			if os.access(file, os.F_OK):
				os.remove(file)

	return 1


def main():
	global CA_dll, DistribDir, WiXdir
	# When WiX is installed, it sets "WIX" to point to the top-level directory
	if 'WIX' in os.environ:
		WiXdir = os.environ['WIX'] + r'\bin'
	# This must be set to a valid Package/@InstallScope value.
	perUser = "perMachine"
	b32 = False
	b64 = False
	tidy = True
	testing = False
	vcver = '143'
	distrib = DistribDir

	try:
		opts, args = getopt.getopt(sys.argv[1:], "d:w:b:t:xe")
	except getopt.error as msg:
		print(msg)
		print('Usage', __doc__)
		return 1

	usage = False

	for o, a in opts:
		if '-d' == o:
			distrib = a
		elif '-w' == o:
			WiXdir = a
		elif '-b' == o:
			if a == '32':
				b32 = True
			elif a == '64':
				b64 = True
			elif a == 'all':
				b32 = True
				b64 = True
			elif a == 'user':
				perUser = "perUser"
		elif '-t' == o:
			if a == 'vc141':
				vcver = '141'
			elif a == 'vc142':
				vcver = '142'
			elif a == 'vc143':
				vcver = '143'
			else:
				usage = True
		elif '-x' == o:
			tidy = False
		elif '-e' == o:
			testing = True
		else:
			usage = True
			break

	if len(args) > 0 or usage:
		print('Usage:', __doc__)
		return 1

	if not b32 and not b64:
		b32 = True
		b64 = True

	if os.access(distrib, os.F_OK):
		print('ERROR: "' + distrib + '" exists. Please clean this up first.')
		return 1

	if not os.access(WiXdir, os.W_OK):
		print('ERROR: "' + WiXdir + '" does not exist!')
		return 1

	ver3Dot, ver3Line = getversion(3)
	ver4Dot, ver4Line = getversion(4)

	os.mkdir(distrib)

	# Wix
	os.environ['PATH'] += ';' + WiXdir
	if not CopyCAdll(ver4Line, vcver):
		return 1
	if b32:
		genWiX(ver3Dot, ver4Dot, ver4Line, True, tidy, perUser, testing, vcver, distrib)
	if b64:
		genWiX(ver3Dot, ver4Dot, ver4Line, False, tidy, perUser, testing, vcver, distrib)
	if os.access(CA_dll, os.W_OK):
		os.remove(CA_dll)

	return 0


if __name__ == '__main__':
	sys.exit(main())
