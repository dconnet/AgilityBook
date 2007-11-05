# Generate MSI files
#
# Revision History
# 2007-11-05 DRC Add WiX back (addition)
#            Note, WiX doesn't work yet, that's why I added Inno
# 2007-10-31 DRC Changed from WiX to InnoSetup
# 2007-03-07 DRC Created

"""GenMSI.py [/32] [/64] [/w98] [/all] [/notidy] [/wix | /inno]
32: Create 32bit Unicode msi
64: Create 64bit Unicode msi
w98: Create 32bit MBCS msi
all: Create all of them (default)
notidy: Do not clean up generated files
wix: Use WiX installer
inno: Use Inno installer (default)
"""

import datetime
import os
import string
import sys

AgilityBookDir = "..\\..\\.."

WiXdir = AgilityBookDir + "\\..\\wix"
ISTool = "c:\\Program Files\\ISTool"

WinSrcDir = AgilityBookDir + "\\src\\Win"
Compiler = "VC8"
ResString = "FILEVERSION "
code32 = 1
code64 = 3
code98 = 2
UpgradeCode = "DD9A3E2B-5363-4BA7-9870-B5E1D227E7DB"


def getversion(numParts):
	ver = "0"
	for i in range(1, numParts):
		ver = ver + ".0"
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
				ver = line[0]
				for i in range(1, numParts):
					ver = ver + '.' + line[i]
				break
		else:
			break
	return ver


def genuuid():
	(childin, childout) = os.popen4("uuidgen -c")
	childin.close()
	line = string.rstrip(childout.readline())
	#line = string.upper(line) [-c means upper, leaving in case I change how guids are gotten]
	childout.close()
	return line


# returns baseDir, outputFile
def getoutputvars(code):
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
	return baseDir, outputFile


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
def genWiX(productId, version, code, tidy):
	baseDir, outputFile = getoutputvars(code)
	if tidy and not os.access(baseDir + "AgilityBook.exe", os.F_OK):
		print baseDir + "AgilityBook.exe does not exist, MSI skipped"
		return 0

	setup = open(outputFile + ".wxs", "w")
	print >>setup, "<?xml version='1.0'?>"
	#Generate guids: "uuidgen -c"
	#Candle cmd: candle AgilityBook.wxs
	#Light cmd: light -b \AgilityBook\src\wix -out AgilityBook.msi
	#			AgilityBook.wixobj \AgilityBook\src\wix\arbwixui.wixlib -loc
	#			\AgilityBook\src\wix\WixUI_en-us.wxl
	#	(uses custom wixui)
	print >>setup, "<Wix xmlns='http://schemas.microsoft.com/wix/2003/01/wi'>"
	print >>setup, "\t<Product Id='" + productId + "'"
	print >>setup, "\t\t\tUpgradeCode='" + UpgradeCode + "'"
	print >>setup, "\t\t\tName='Agility Record Book'"
	print >>setup, "\t\t\tManufacturer='dcon Software'"
	print >>setup, "\t\t\tLanguage='1033' Version='" + version + "' >"
	print >>setup, "\t\t<Package Id='????????-????-????-????-????????????'"
	print >>setup, "\t\t\t\tComments='Track all your agility records in one convenient place.'"
	print >>setup, "\t\t\t\tDescription='Agility Record Book'"
	print >>setup, "\t\t\t\tManufacturer='dcon Software'"
	print >>setup, "\t\t\t\tInstallerVersion='200' Compressed='yes' />"
	print >>setup, ""
	print >>setup, "\t\t<Property Id='ARPHELPLINK'>http://www.agilityrecordbook.com</Property>"
	print >>setup, "\t\t<Property Id='ARPCONTACT'>David Connet</Property>"
	print >>setup, "\t\t<Property Id='ARPCOMMENTS'>Track all your agility records in one convenient place.</Property>"
	print >>setup, "\t\t<Property Id='ARPURLINFOABOUT'>http://www.agilityrecordbook.com</Property>"
	print >>setup, "\t\t<Property Id='ALLUSERS'>2</Property>"
	print >>setup, "\t\t<Property Id='ARPPRODUCTICON'>AgilBook.ico</Property>"
	print >>setup, ""
	if code32 == code:
		print >>setup, "\t\t<Condition Message='This application only runs on Windows NT and above'>VersionNT</Condition>"
	elif code98 == code:
		print >>setup, "\t\t<Condition Message='This application only runs on Windows ME and above'>Version9X&gt;=490 OR VersionNT</Condition>"
	elif code64 == code:
		print >>setup, "\t\t<Condition Message='This application only runs on 64bit Windows systems'>VersionNT64</Condition>"
	print >>setup, ""
	print >>setup, "\t\t<InstallExecuteSequence>"
	print >>setup, "\t\t\t<RemoveExistingProducts After='InstallValidate' />"
	print >>setup, "\t\t\t<Custom Action='NewVersionError' After='FindRelatedProducts'>NEWERVERSIONFOUND</Custom>"
	print >>setup, "\t\t</InstallExecuteSequence>"
	print >>setup, ""
	print >>setup, "\t\t<Upgrade Id='" + UpgradeCode + "'>"
	print >>setup, "\t\t\t<UpgradeVersion Property='OLDVERSIONFOUND'"
	print >>setup, "\t\t\t\t\tOnlyDetect='no' Minimum='0.0.0.0' IncludeMinimum='yes' />"
	print >>setup, "\t\t\t<UpgradeVersion Property='NEWERVERSIONFOUND'"
	print >>setup, "\t\t\t\t\tOnlyDetect='yes' Minimum='" + version + "' IncludeMinimum='yes' />"
	print >>setup, "\t\t</Upgrade>"
	print >>setup, ""
	print >>setup, "\t\t<CustomAction Id='NewVersionError' Error='A newer version of [ProductName] is already installed! Cannot continue installation.' />"
	print >>setup, ""
	print >>setup, "\t\t<Media Id='1' Cabinet='Product.cab' EmbedCab='yes' />"
	print >>setup, ""
	print >>setup, "\t\t<Directory Id='TARGETDIR' Name='SourceDir'>"
	print >>setup, "\t\t\t<Directory Id='ProgramFilesFolder' Name='PFiles'>"
	print >>setup, "\t\t\t\t<Directory Id='dconSoft' Name='dconSoft'"
	print >>setup, "\t\t\t\t\t\tLongName='dcon Software'>"
	print >>setup, "\t\t\t\t<Directory Id='INSTALLDIR' Name='ARB'"
	print >>setup, "\t\t\t\t\t\tLongName='Agility Record Book'>"
	print >>setup, ""
	print >>setup, "\t\t\t\t\t<Component Id='FilesMain'"
	print >>setup, "\t\t\t\t\t\t\tGuid='C65F9350-B7D5-4A1B-8C28-333D8928D8AA'>"
	print >>setup, "\t\t\t\t\t\t<File DiskId='1' Id='AgilityBookEXE'"
	print >>setup, "\t\t\t\t\t\t\t\tVital='yes'"
	print >>setup, "\t\t\t\t\t\t\t\tName='AgilBook.exe'"
	print >>setup, "\t\t\t\t\t\t\t\tLongName='AgilityBook.exe'"
	print >>setup, "\t\t\t\t\t\t\t\tsrc='" + baseDir + "AgilityBook.exe'>"
	print >>setup, "\t\t\t\t\t\t\t<Shortcut Id='PgmShortcut'"
	print >>setup, "\t\t\t\t\t\t\t\t\tDirectory='ProgramMenuDir'"
	print >>setup, "\t\t\t\t\t\t\t\t\tName='AgilBook'"
	print >>setup, "\t\t\t\t\t\t\t\t\tLongName='Agility Record Book'"
	print >>setup, "\t\t\t\t\t\t\t\t\tWorkingDirectory='INSTALLDIR'"
	print >>setup, "\t\t\t\t\t\t\t\t\tIcon='AgilBook.exe'"
	print >>setup, "\t\t\t\t\t\t\t\t\tIconIndex='0'"
	print >>setup, "\t\t\t\t\t\t\t\t\tAdvertise='yes'/>"
	print >>setup, "\t\t\t\t\t\t\t<Shortcut Id='DesktopShortcut'"
	print >>setup, "\t\t\t\t\t\t\t\t\tDirectory='DesktopFolder'"
	print >>setup, "\t\t\t\t\t\t\t\t\tName='AgilBook'"
	print >>setup, "\t\t\t\t\t\t\t\t\tLongName='Agility Record Book'"
	print >>setup, "\t\t\t\t\t\t\t\t\tWorkingDirectory='INSTALLDIR'"
	print >>setup, "\t\t\t\t\t\t\t\t\tIcon='AgilBook.exe'"
	print >>setup, "\t\t\t\t\t\t\t\t\tIconIndex='0'"
	print >>setup, "\t\t\t\t\t\t\t\t\tAdvertise='yes'/>"
	print >>setup, "\t\t\t\t\t\t</File>"
	print >>setup, "\t\t\t\t\t\t<File DiskId='1' Id='HelpFile'"
	print >>setup, "\t\t\t\t\t\t\t\tName='AgilBook.chm'"
	print >>setup, "\t\t\t\t\t\t\t\tLongName='AgilityBook.chm'"
	print >>setup, "\t\t\t\t\t\t\t\tsrc='" + baseDir + "AgilityBook.chm'>"
	print >>setup, "\t\t\t\t\t\t\t<Shortcut Id='HelpShortcut'"
	print >>setup, "\t\t\t\t\t\t\t\t\tDirectory='ProgramMenuDir'"
	print >>setup, "\t\t\t\t\t\t\t\t\tName='Help'"
	print >>setup, "\t\t\t\t\t\t\t\t\tLongName='Agility Record Book Help'"
	print >>setup, "\t\t\t\t\t\t\t\t\tWorkingDirectory='INSTALLDIR'"
	print >>setup, "\t\t\t\t\t\t\t\t\tIcon='AgilBook.chm'"
	print >>setup, "\t\t\t\t\t\t\t\t\tIconIndex='0'"
	print >>setup, "\t\t\t\t\t\t\t\t\tAdvertise='yes'/>"
	print >>setup, "\t\t\t\t\t\t</File>"
	print >>setup, "\t\t\t\t\t\t<ProgId Id='AgilityBook.Document'"
	print >>setup, "\t\t\t\t\t\t\t\tDescription='Agility Record Book data file'>"
	print >>setup, "\t\t\t\t\t\t\t<Extension Id='arb'>"
	print >>setup, "\t\t\t\t\t\t\t\t<Verb Id='open' Sequence='1' Command='Open' Target='[!AgilityBook]' Argument='\"%1\"' />"
	print >>setup, "\t\t\t\t\t\t\t</Extension>"
	print >>setup, "\t\t\t\t\t\t</ProgId>"
	print >>setup, "\t\t\t\t\t\t<Registry Id='ArbIcon1' Root='HKCR'"
	print >>setup, "\t\t\t\t\t\t\t\tKey='.arb'"
	print >>setup, "\t\t\t\t\t\t\t\tAction='write' Type='string'"
	print >>setup, "\t\t\t\t\t\t\t\tValue='AgilityBook.Document' />"
	print >>setup, "\t\t\t\t\t\t<Registry Id='ArbIcon2' Root='HKCR'"
	print >>setup, "\t\t\t\t\t\t\t\tKey='AgilityBook.Document'"
	print >>setup, "\t\t\t\t\t\t\t\tAction='write' Type='string'"
	print >>setup, "\t\t\t\t\t\t\t\tValue='Agility Record Book data file' />"
	print >>setup, "\t\t\t\t\t\t<Registry Id='ArbIcon3' Root='HKCR'"
	print >>setup, "\t\t\t\t\t\t\t\tKey='AgilityBook.Document\\DefaultIcon'"
	print >>setup, "\t\t\t\t\t\t\t\tAction='write' Type='string'"
	print >>setup, "\t\t\t\t\t\t\t\tValue='[!AgilityBook],0' />"
	print >>setup, "\t\t\t\t\t</Component>"
	print >>setup, ""
	print >>setup, "\t\t\t\t\t<Component Id='FilesDLL'"
	print >>setup, "\t\t\t\t\t\t\tGuid='C65F9350-B7D5-4A1B-8C28-333D8928D8AA'>"
	print >>setup, "\t\t\t\t\t\t<File DiskId='1' Id='fra'"
	print >>setup, "\t\t\t\t\t\t\t\tVital='yes'"
	print >>setup, "\t\t\t\t\t\t\t\tName='ARBFRA.dll'"
	print >>setup, "\t\t\t\t\t\t\t\tLongName='AgilityBookFRA.dll'"
	print >>setup, "\t\t\t\t\t\t\t\tsrc='" + baseDir + "AgilityBookFRA.dll'>"
	print >>setup, "\t\t\t\t\t\t</File>"
	print >>setup, "\t\t\t\t\t\t<File DiskId='1' Id='usdaa'"
	print >>setup, "\t\t\t\t\t\t\t\tVital='yes'"
	print >>setup, "\t\t\t\t\t\t\t\tName='cal_usdaa.dll'"
	print >>setup, "\t\t\t\t\t\t\t\tsrc='" + baseDir + "cal_usdaa.dll'>"
	print >>setup, "\t\t\t\t\t\t</File>"
	print >>setup, "\t\t\t\t\t</Component>"
	print >>setup, ""
	print >>setup, "\t\t\t\t</Directory>"
	print >>setup, "\t\t\t\t</Directory>"
	print >>setup, "\t\t\t</Directory>"
	print >>setup, ""
	print >>setup, "\t\t\t<Directory Id='ProgramMenuFolder' Name='PMenu'>"
	print >>setup, "\t\t\t\t<Directory Id='ProgramMenuDir' Name='ARB' LongName='Agility Record Book' />"
	print >>setup, "\t\t\t</Directory>"
	print >>setup, ""
	print >>setup, "\t\t\t<Directory Id='DesktopFolder' Name='Desktop' />"
	print >>setup, ""
	print >>setup, "\t\t</Directory>"
	print >>setup, ""
	print >>setup, "\t\t<Feature Id='Complete' Level='1' Display='expand'"
	print >>setup, "\t\t\t\tTitle='Agility Record Book Setup'"
	print >>setup, "\t\t\t\tDescription='Track all your agility records in one convenient place.'"
	print >>setup, "\t\t\t\tConfigurableDirectory='INSTALLDIR'>"
	print >>setup, "\t\t\t<ComponentRef Id='FilesMain' />"
	print >>setup, "\t\t\t<ComponentRef Id='FilesDLL' />"
	print >>setup, "\t\t</Feature>"
	print >>setup, ""
	print >>setup, "\t\t<Property Id='WIXUI_INSTALLDIR' Value='INSTALLDIR' />"
	print >>setup, "\t\t<UIRef Id='WixUI_InstallDir' />"
	print >>setup, "\t\t<UIRef Id='WixUI_ErrorProgressText' />"
	print >>setup, ""
	print >>setup, "\t\t<Icon Id='AgilBook.exe' SourceFile='..\\..\\Win\\res\\AgilityBook.ico' />"
	print >>setup, "\t\t<Icon Id='AgilBook.ico' SourceFile='..\\..\\Win\\res\\AgilityBook.ico' />"
	print >>setup, "\t\t<Icon Id='AgilBook.chm' SourceFile='..\\..\\Win\\res\\AgilityBook.ico' />"
	print >>setup, "\t</Product>"
	print >>setup, "</Wix>"
	setup.close()

	if os.access(baseDir + "AgilityBook.exe", os.F_OK):
		runcmd("candle -nologo " + outputFile + ".wxs")
		runcmd("light -nologo -b " + WiXdir + " -out \"" + baseDir + outputFile + ".msi\" " + outputFile + ".wixobj " + WiXdir + "\\arbwixui.wixlib -loc " + WiXdir + "\\WixUI_en-us.wxl")
		if tidy:
			if os.access(outputFile + ".wxs", os.F_OK):
				os.remove(outputFile + ".wxs")
			if os.access(outputFile + ".wixobj", os.F_OK):
				os.remove(outputFile + ".wixobj")
	return 1


def genInno(version, code, tidy):
	baseDir, outputFile = getoutputvars(code)
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
	print >>setup, 'Name: "{group}\{cm:UninstallProgram,{#ARBName}}"; Filename: "{uninstallexe}"'
	print >>setup, ''
	print >>setup, '[Run]'
	print >>setup, 'Filename: "{app}\AgilityBook.exe"; Parameters: "/register"; StatusMsg: "Registering File Associations..."'
	print >>setup, ''
	print >>setup, '[UninstallRun]'
	print >>setup, 'Filename: "{app}\AgilityBook.exe"; Parameters: "/unregister"; RunOnceId: "RemoveARBAssoc"'
	setup.close()

	if os.access(baseDir + "AgilityBook.exe", os.F_OK):
		fullpath = os.getcwd() + '\\' + outputFile + '.iss'
		runcmd('istool -compile ' + fullpath)
		if tidy:
			if os.access(outputFile + ".iss", os.F_OK):
				os.remove(outputFile + ".iss")
	return 1


def main():
	doWiX = 0
	doInno = 1
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
		elif o == "/wix":
			doWiX = 1
			doInno = 0
		elif o == "/inno":
			doWiX = 0
			doInno = 1

	if b32 + b64 + b98 == 0:
		b32 = 1
		b64 = 1
		b98 = 1

	b32ok = 0
	b64ok = 0
	b98ok = 0

	# Wix
	if doWiX:
		productId = genuuid()
		version = getversion(3)
		os.environ['PATH'] += ';' + WiXdir
		if b32:
			if genWiX(productId, version, code32, tidy):
				b32ok = 1
		if b64:
			if genWiX(productId, version, code64, tidy):
				b64ok = 1
		if b98:
			if genWiX(productId, version, code98, tidy):
				b98ok = 1
		if b32ok or b64ok or b98ok:
			d = datetime.datetime.now().isoformat(' ')
			codes = open(AgilityBookDir + "\\Misc\\Installation\\InstallGUIDs.csv", "a")
			installs = ""
			if b32ok:
				installs = installs + ",win32"
			if b64ok:
				installs = installs + ",x64"
			if b98ok:
				installs = installs + ",win98"
			print >>codes, "v" + version + "," + d + "," + productId + "," + UpgradeCode + "," + Compiler + installs

	# Inno
	if doInno:
		version = getversion(4)
		os.environ['PATH'] += ';' + ISTool
		if b32:
			if genInno(version, code32, tidy):
				b32ok = 1
		if b64:
			if genInno(version, code64, tidy):
				b64ok = 1
		if b98:
			if genInno(version, code98, tidy):
				b98ok = 1

main()
