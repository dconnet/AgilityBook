# Generate MSI files
#
# Revision History
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

"""GenMSI.py [/32] [/64] [/w98] [/all] [/notidy] /test
	32: Create 32bit Unicode msi
	64: Create 64bit Unicode msi
	w98: Create 32bit MBCS msi
	all: Create all of them (default)
	notidy: Do not clean up generated files
	test: Generate .msi for test purposes (don't write to InstallGUIDs.csv)
"""

import datetime
import os
import string
import sys

AgilityBookDir = "..\\..\\.."

WiXdir = r"c:\Tools\wix2"

WinSrcDir = AgilityBookDir + "\\src"
code32 = 1
code64 = 3
code98 = 2

# The reason for a different upgrade codes is that is allows beta testing
# side-by-side. When the next version is actually released, it will upgrade
# (uninstall) all previous versions.

# When the beta ends, set this flag to 0. That will then include the V1 upgrade
# code. (Any future open betas will follow this same route - a beta will only
# upgrade itself)
beta = 1

# v1 upgrade code
UpgradeCodeV1 = "DD9A3E2B-5363-4BA7-9870-B5E1D227E7DB"
#defFolder = 'Agility Record Book'

# v2 upgrade code
UpgradeCodeV2 = "4D018FAD-2CBC-4A92-B6AC-4BAAECEED8F4"
defFolder = 'Agility Record Book v2'


def getversion(numParts):
	ver = "0"
	ver2 = "0"
	for i in range(1, numParts):
		ver = ver + ".0"
		ver2 = ver2 + "_0"
	resStr = [
		"#define ARB_VER_MAJOR",
		"#define ARB_VER_MINOR",
		"#define ARB_VER_DOT",
		"#define ARB_VER_BUILD"
		]
	found = 0;
	version = ["0", "0", "0", "0"]
	res = open(WinSrcDir + r"\Include\VersionNumber.h", "r")
	while (1):
		line = res.readline()
		if line:
			line = string.strip(line)
			for i in range(0, 4):
				pos = line.find(resStr[i])
				if 0 == pos:
					found = found + 1
					version[i] = string.strip(line[pos+len(resStr[i]):])
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
	(childin, childout) = os.popen4("uuidgen -c")
	childin.close()
	line = string.rstrip(childout.readline())
	#line = string.upper(line) [-c means upper, leaving in case I change how guids are gotten]
	childout.close()
	return line


# returns baseDir, outputFile
def getoutputvars(code, version):
	outputFile = ""
	baseDir = ""
	if code32 == code:
		outputFile = "AgilityBook_" + version + "-win"
		baseDir = AgilityBookDir + "\\bin\\VC9Win32\\Release\\"
	elif code98 == code:
		outputFile = "AgilityBook_" + version + "-win98"
		baseDir = AgilityBookDir + "\\bin\\VC8Win32\\Release\\"
	elif code64 == code:
		outputFile = "AgilityBook_" + version + "-x64"
		baseDir = AgilityBookDir + "\\bin\\VC9x64\\Release\\"
	else:
		raise Exception, "Invalid code"
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
def genWiX(productId, version, version2, code, tidy):
	baseDir, outputFile = getoutputvars(code, version2)
	if tidy and not os.access(baseDir + "AgilityBook.exe", os.F_OK):
		print baseDir + "AgilityBook.exe does not exist, MSI skipped"
		return 0

	setup = open(outputFile + ".wxs", "w")
	print >>setup, r'<?xml version="1.0" encoding="UTF-8"?>'
	print >>setup, r'<Wix xmlns="http://schemas.microsoft.com/wix/2003/01/wi">'
	print >>setup, r'  <Product Id="' + productId + r'"'
	print >>setup, r'      UpgradeCode="' + UpgradeCodeV2 + r'"'
	print >>setup, r'      Name="Agility Record Book ' + version + '"'
	print >>setup, r'      Language="1033"'
	print >>setup, r'      Version="' + version + '"'
	print >>setup, r'      Manufacturer="dcon Software">'
	print >>setup, r'    <Package Id="????????-????-????-????-????????????"'
	print >>setup, r'        Description="Agility Record Book"'
	print >>setup, r'        Comments="Track all your agility records in one convenient place."'
	print >>setup, r'        InstallerVersion="200"'
	if code64 == code:
		print >>setup, r'        Platforms="x64"'
	print >>setup, r'        Compressed="yes"/>'
	print >>setup, r''
	print >>setup, r'    <Property Id="INSTALLLEVEL">100</Property>'
	print >>setup, r''
	print >>setup, r'    <Property Id="ARPCOMMENTS">Track all your agility records in one convenient place.</Property>'
	print >>setup, r'    <Property Id="ARPCONTACT">David Connet</Property>'
	print >>setup, r'    <Property Id="ARPHELPLINK">http://www.agilityrecordbook.com</Property>'
	print >>setup, r'    <Property Id="ARPURLINFOABOUT">http://www.agilityrecordbook.com</Property>'
	print >>setup, r'    <Property Id="ALLUSERS">2</Property>'
	print >>setup, r''
	if code32 == code:
		print >>setup, r'    <Condition Message="This application only runs on Windows NT and above">VersionNT</Condition>'
	elif code98 == code:
		print >>setup, r'    <Condition Message="This application only runs on Windows ME and above">Version9X&gt;=490 OR VersionNT</Condition>'
	elif code64 == code:
		print >>setup, r'    <Condition Message="This application only runs on 64bit Windows systems">VersionNT64</Condition>'
	print >>setup, r''
	if not beta:
		print >>setup, r'    <Upgrade Id="' + UpgradeCodeV1 + '">'
		print >>setup, r'      <UpgradeVersion OnlyDetect="no" Property="OLDVERSIONFOUND"'
		print >>setup, r'          IncludeMinimum="yes" Minimum="0.0.0"/>'
		print >>setup, r'    </Upgrade>'
	print >>setup, r'    <Upgrade Id="' + UpgradeCodeV2 + '">'
	print >>setup, r'      <UpgradeVersion OnlyDetect="no" Property="OLDVERSIONFOUND"'
	print >>setup, r'          IncludeMinimum="yes" Minimum="0.0.0"/>'
	print >>setup, r'    </Upgrade>'
	print >>setup, r''
	print >>setup, r'    <Media Id="1" Cabinet="AgilityRecordBook.cab" EmbedCab="yes" DiskPrompt="Disk 1" VolumeLabel="DISK1"/>'
	print >>setup, r'    <Property Id="DiskPrompt" Value="[1]"/>'
	print >>setup, r''
	print >>setup, r'    <Directory Id="TARGETDIR" Name="SourceDir">'
	print >>setup, r'      <Directory Id="DesktopFolder"/>'
	print >>setup, r'      <Directory Id="ProgramMenuFolder">'
	print >>setup, r'        <Directory Id="ARBMenuFolder" Name="ARB" LongName="' + defFolder + '"/>'
	print >>setup, r'      </Directory>'
	if code64 == code:
		print >>setup, r'      <Directory Id="ProgramFiles64Folder">'
	else:
		print >>setup, r'      <Directory Id="ProgramFilesFolder">'
	print >>setup, r'        <Directory Id="CompanyFolder" Name="dconSoft" LongName="dcon Software">'
	print >>setup, r'          <Directory Id="INSTALLDIR" Name="ARB" LongName="' + defFolder + '">'
	print >>setup, r''
	if code64 == code:
		print >>setup, r'            <Component Id="ApplicationCore" Guid="B48C351B-3E9C-4C66-AC0B-150D26047499" Win64="yes">'
	else:
		print >>setup, r'            <Component Id="ApplicationCore" Guid="55CB5388-14EA-4927-B2A4-96315EA09418">'
	print >>setup, r'              <File Id="App" Name="ARBexe" LongName="AgilityBook.exe"'
	print >>setup, r'                    Source="' + baseDir + r'AgilityBook.exe"'
	print >>setup, r'                    Vital="yes" DiskId="1">'
	print >>setup, r'                <Shortcut Id="AppDesktop" Name="ARB" LongName="' + defFolder + '" Directory="DesktopFolder" />'
	print >>setup, r'                <Shortcut Id="AppMenu" Name="ARB" LongName="' + defFolder + '" Directory="ARBMenuFolder" />'
	print >>setup, r'              </File>'
	print >>setup, r'              <File Id="ARBData" Name="ARBData" LongName="AgilityBook.dat"'
	print >>setup, r'                    Source="' + baseDir + r'AgilityBook.dat"'
	print >>setup, r'                    Vital="yes" DiskId="1">'
	print >>setup, r'              </File>'
	print >>setup, r'              <File Id="ARBHelp" Name="ARBHelp" LongName="ARBHelp.exe"'
	print >>setup, r'                    Source="' + baseDir + r'ARBHelp.exe"'
	print >>setup, r'                    Vital="yes" DiskId="1">'
	print >>setup, r'                <Shortcut Id="ARBHelpShortcut" Name="ARBHelp" LongName="ARB Helper" Directory="ARBMenuFolder" />'
	print >>setup, r'              </File>'
	print >>setup, r'              <!-- The description is the same since that was what MFC set it to (v1) -->'
	print >>setup, r'              <ProgId Id="AgilityBook.Document" Description="AgilityBook.Document">'
	print >>setup, r'                <Extension Id="arb">'
	print >>setup, r'                  <Verb Id="open" Sequence="1" Command="Open" Target="[!App]" Argument=' + "'\"%1\"'/>"
	print >>setup, r'                </Extension>'
	print >>setup, r'              </ProgId>'
	print >>setup, r'              <Registry Id="AppIcon1" Root="HKCR" Key="AgilityBook.Document\DefaultIcon" Action="write"'
	print >>setup, r'                        Type="string" Value="[INSTALLDIR]AgilityBook.exe,0" />'
	print >>setup, r'            </Component>'
	if code64 == code:
		print >>setup, r'            <Component Id="CALusdaa" Guid="16F399ED-BA94-4CBF-B85E-D79C737D3232" Win64="yes">'
	else:
		print >>setup, r'            <Component Id="CALusdaa" Guid="8C9B6F50-7F0A-46AA-B813-A66A8300E421">'
	print >>setup, r'              <File Id="usdaa" Name="CALusdaa" LongName="cal_usdaa.dll"'
	print >>setup, r'                    Source="' + baseDir + r'cal_usdaa.dll"'
	print >>setup, r'                    Vital="no" DiskId="1" />'
	print >>setup, r'            </Component>'
	if code64 == code:
		print >>setup, r'            <Component Id="ARBHelp" Guid="6E5D1584-06A0-4024-A197-84B9E0326EA1" Win64="yes">'
	else:
		print >>setup, r'            <Component Id="ARBHelp" Guid="9CB60EE5-C5A3-4219-83C4-A59D2872DC8F">'
	print >>setup, r'              <File Id="arbhelp" Name="ARBHelp" LongName="ARBHelp.exe"'
	print >>setup, r'                    Source="' + baseDir + r'ARBHelp.exe"'
	print >>setup, r'                    Vital="no" DiskId="1" />'
	print >>setup, r'            </Component>'
	print >>setup, r''
	print >>setup, r'            <Directory Id="lang" Name="lang">'
	print >>setup, r'              <Directory Id="dirFR" Name="fr">'
	if code64 == code:
		print >>setup, r'                <Component Id="langFR" Guid="581CB1F1-0846-4205-8551-5FEE8A03AF62" Win64="yes">'
	else:
		print >>setup, r'                <Component Id="langFR" Guid="53DA3663-A90A-44AF-A921-5EC0E104ABAD">'
	print >>setup, r'                  <File Id="ResFR" Name="arb.mo"'
	print >>setup, r'                        Source="' + baseDir + r'lang\fr\arb.mo"'
	print >>setup, r'                        Vital="no" DiskId="1" />'
	print >>setup, r'                </Component>'
	print >>setup, r'              </Directory>'
	print >>setup, r'              <Directory Id="dirEN" Name="en">'
	if code64 == code:
		print >>setup, r'                <Component Id="langEN" Guid="27FF13B7-5D71-414B-9BA6-1C6D04DE0BA6" Win64="yes">'
	else:
		print >>setup, r'                <Component Id="langEN" Guid="9AFD56DF-F038-4877-A35A-00036A508FD8">'
	print >>setup, r'                  <File Id="ResEN" Name="arb.mo"'
	print >>setup, r'                        Source="' + baseDir + r'lang\en\arb.mo"'
	print >>setup, r'                        Vital="yes" DiskId="1">'
	print >>setup, r'                  </File>'
	print >>setup, r'                </Component>'
	print >>setup, r'              </Directory>'
	print >>setup, r'            </Directory>'
	print >>setup, r'            <Component Id="empty" Guid="7B73272C-8FB4-4FED-9A73-1C0A534A583C"/>'
	print >>setup, r'          </Directory>'
	print >>setup, r'        </Directory>'
	print >>setup, r'      </Directory>'
	print >>setup, r'    </Directory>'
	print >>setup, r''
	print >>setup, r'    <Feature Id="Complete" Display="expand" Level="1"'
	print >>setup, r'             AllowAdvertise="no"'
	print >>setup, r'             Title="Agility Record Book" Description="Main Agility Record Book files" ConfigurableDirectory="INSTALLDIR">'
	print >>setup, r'      <ComponentRef Id="ApplicationCore"/>'
	print >>setup, r'      <ComponentRef Id="langEN"/>'
	print >>setup, r'      <Feature Id="ARBHelp" Display="expand" Level="100"'
	print >>setup, r'               AllowAdvertise="no"'
	print >>setup, r'               Title="ARBHelp" Description="Help diagnose problems by assembling necessary files for debugging">'
	print >>setup, r'          <ComponentRef Id="ARBHelp"/>'
	print >>setup, r'        </Feature>'
	print >>setup, r'      <Feature Id="Languages" Display="expand" Level="100"'
	print >>setup, r'               AllowAdvertise="no"'
	print >>setup, r'               Title="Languages" Description="Supported languages that can be changed while running the program (English is always available)">'
	# Need an empty component to work around a bug with features (to suppress advertising)
	print >>setup, r'        <ComponentRef Id="empty"/>'
	print >>setup, r'        <Feature Id="French" Level="100"'
	print >>setup, r'                 AllowAdvertise="no"'
	print >>setup, r'                 Title="French" Description="French">'
	print >>setup, r'          <ComponentRef Id="langFR"/>'
	print >>setup, r'        </Feature>'
	print >>setup, r'      </Feature>'
	print >>setup, r'      <Feature Id="Plugins" Display="expand" Level="100"'
	print >>setup, r'               AllowAdvertise="no"'
	print >>setup, r'               Title="Calendar Addins" Description="Addins to download Calendar data directly from a website">'
	# Need an empty component to work around a bug with features (to suppress advertising)
	print >>setup, r'        <ComponentRef Id="empty"/>'
	print >>setup, r'        <Feature Id="Calendar" Level="100"'
	print >>setup, r'                 AllowAdvertise="no"'
	print >>setup, r'                 Title="USDAA" Description="Download Calendar entries from usdaa.com">'
	print >>setup, r'          <ComponentRef Id="CALusdaa"/>'
	print >>setup, r'        </Feature>'
	print >>setup, r'      </Feature>'
	print >>setup, r'    </Feature>'
	print >>setup, r''
	# Just FYI of how to do this.
	#print >>setup, r'    <CustomAction Id="LaunchFile" FileKey="App" ExeCommand="/register" Return="ignore"/>'
	print >>setup, r''
	print >>setup, r'    <InstallExecuteSequence>'
	print >>setup, r'      <RemoveExistingProducts After="InstallValidate"/>'
	# FYI
	#print >>setup, r'      <Custom Action="LaunchFile" After="InstallFinalize">NOT Installed</Custom>'
	print >>setup, r'    </InstallExecuteSequence>'
	print >>setup, r''
	print >>setup, r'    <UIRef Id="WixUI_FeatureTree"/>'
	print >>setup, r'    <UIRef Id="WixUI_ErrorProgressText"/>'
	print >>setup, r''
	print >>setup, r'    <Icon Id="AgilityBook.exe" SourceFile="..\..\..\src\win\res\AgilityBook.ico"/>'
	print >>setup, r'  </Product>'
	print >>setup, r'</Wix>'

	setup.close()

	if os.access(baseDir + "AgilityBook.exe", os.F_OK):
		runcmd('candle -nologo ' + outputFile + '.wxs')
		runcmd('light -nologo -b "' + WiXdir + '" -out "' + outputFile + '.msi" ' + outputFile + '.wixobj "' + WiXdir + r'\wixui.wixlib" -loc "' + WiXdir + r'\WixUI_en-us.wxl"')
		if tidy:
			if os.access(outputFile + ".wxs", os.F_OK):
				os.remove(outputFile + ".wxs")
			if os.access(outputFile + ".wixobj", os.F_OK):
				os.remove(outputFile + ".wixobj")
	return 1


def main():
	b32 = 0
	b64 = 0
	b98 = 0
	tidy = 1
	bTesting = 0
	if 1 == len(sys.argv):
		b32 = 1
		bTesting = 1
	for i in range(1, len(sys.argv)):
		o = sys.argv[i]
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
		elif o == "/test":
			bTesting = 1
		else:
			print "Usage:", __doc__
			return

	if b32 + b64 + b98 == 0:
		b32 = 1
		b64 = 1
		b98 = 1

	b32ok = 0
	b64ok = 0
	b98ok = 0

	# Wix
	productId = genuuid()
	ver3Dot, junk = getversion(3)
	ver4Dot, ver4Line = getversion(4)
	os.environ['PATH'] += ';' + WiXdir
	if b32:
		if genWiX(productId, ver3Dot, ver4Line, code32, tidy):
			b32ok = 1
	if b64:
		if genWiX(productId, ver3Dot, ver4Line, code64, tidy):
			b64ok = 1
	if b98:
		if genWiX(productId, ver3Dot, ver4Line, code98, tidy):
			b98ok = 1
	if not bTesting and (b32ok or b64ok or b98ok):
		d = datetime.datetime.now().isoformat(' ')
		codes = open(AgilityBookDir + r"\Misc\InstallGUIDs.csv", "a")
		installs = ""
		if b32ok:
			installs = "VC9,win32"
		if b64ok:
			installs = "VC9,x64"
		if b98ok:
			installs = "VC8,win98"
		print >>codes, "v" + ver4Dot + "," + d + "," + productId + "," + UpgradeCodeV2 + "," + installs


main()
