# Generate MSI files
#
# Revision History
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

WiXdir = r"c:\Tools\wix3"

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
def genWiX(productId, ver3Dot, ver4Line, code, tidy):
	baseDir, outputFile = getoutputvars(code, ver4Line)
	if tidy and not os.access(baseDir + "AgilityBook.exe", os.F_OK):
		print baseDir + "AgilityBook.exe does not exist, MSI skipped"
		return 0

	setup = open(outputFile + ".wxs", "w")
	print >>setup,		r'<?xml version="1.0" encoding="utf-8"?>'
	print >>setup,		r'<Wix xmlns="http://schemas.microsoft.com/wix/2006/wi">'

	print >>setup,		r'  <Product Id="' + productId + r'"'
	print >>setup,		r'      UpgradeCode="' + UpgradeCodeV2 + r'"'
	print >>setup,		r'      Name="Agility Record Book ' + ver3Dot + '"'
	print >>setup,		r'      Language="1033"'
	print >>setup,		r'      Version="' + ver3Dot + '"'
	print >>setup,		r'      Manufacturer="dcon Software">'
	print >>setup,		r'    <Package'
	print >>setup,		r'        Description="Agility Record Book"'
	print >>setup,		r'        Comments="Track all your agility records in one convenient place."'
	print >>setup,		r'        InstallerVersion="200"'
	if code64 == code:
		print >>setup,	r'        Platform="x64"'
	print >>setup,		r'        Compressed="yes" />'

	print >>setup,		r'    <Icon Id="IconAgilityBook.ico" SourceFile="..\..\..\src\win\res\AgilityBook.ico" />'

	print >>setup,		r'    <Property Id="ARPCOMMENTS">Track all your agility records in one convenient place.</Property>'
	print >>setup,		r'    <Property Id="ARPCONTACT">David Connet</Property>'
	print >>setup,		r'    <Property Id="ARPHELPLINK">http://www.agilityrecordbook.com</Property>'
	print >>setup,		r'    <Property Id="ARPURLINFOABOUT">http://www.agilityrecordbook.com</Property>'
	print >>setup,		r'    <Property Id="ARPPRODUCTICON" Value="IconAgilityBook.ico" />'
	# Install to all-users if admin, per-user otherwise
	print >>setup,		r'    <Property Id="ALLUSERS">2</Property>'
	# To pick up all features by default
	print >>setup,		r'    <Property Id="INSTALLLEVEL">100</Property>'
	print >>setup,		r'    <Property Id="AppName">Agility Record Book</Property>'

	# Launch Conditions
	if code32 == code:
		print >>setup,	r'    <Condition Message="This application only runs on Windows NT and above">VersionNT</Condition>'
	elif code98 == code:
		print >>setup,	r'    <Condition Message="This application only runs on Windows ME and above">Version9X&gt;=490 OR VersionNT</Condition>'
	elif code64 == code:
		print >>setup,	r'    <Condition Message="This application only runs on 64bit Windows systems">VersionNT64</Condition>'

	if not beta:
		print >>setup,	r'    <Upgrade Id="' + UpgradeCodeV1 + '">'
		print >>setup,	r'      <UpgradeVersion'
		print >>setup,	r'          OnlyDetect="no"'
		print >>setup,	r'          Minimum="0.0.0"'
		print >>setup,	r'          IncludeMinimum="yes"'
		print >>setup,	r'          Maximum="' + ver3Dot + '"'
		print >>setup,	r'          Property="OLDVERSIONFOUND" />'
		print >>setup,	r'    </Upgrade>'
	print >>setup,		r'    <Upgrade Id="' + UpgradeCodeV2 + '">'
	print >>setup,		r'      <UpgradeVersion'
	print >>setup,		r'          OnlyDetect="no"'
	print >>setup,		r'          Minimum="0.0.0"'
	print >>setup,		r'          IncludeMinimum="yes"'
	print >>setup,		r'          Maximum="' + ver3Dot + '"'
	print >>setup,		r'          Property="OLDVERSIONFOUND" />'
	print >>setup,		r'      <UpgradeVersion'
	print >>setup,		r'          OnlyDetect="yes"'
	print >>setup,		r'          Minimum="' + ver3Dot + '"'
	print >>setup,		r'          IncludeMinimum="no"'
	print >>setup,		r'          Property="NEWERVERSIONDETECTED" />'
	print >>setup,		r'    </Upgrade>'

	print >>setup,		r'    <Condition Message="A later version of [ProductName] is already installed. In order to install an older version, you must first uninstall the current product. Setup will now exit.">NOT NEWERVERSIONDETECTED OR Installed</Condition>'

	print >>setup,		r'    <Media Id="1" Cabinet="AgilityBook.cab" EmbedCab="yes" />'

	print >>setup,		r'    <Directory Id="TARGETDIR" Name="SourceDir">'
	print >>setup,		r'      <Directory Id="DesktopFolder" />'
	print >>setup,		r'      <Directory Id="ProgramMenuFolder">'
	print >>setup,		r'        <Directory Id="ARBMenuFolder" Name="' + defFolder + '" />'
	print >>setup,		r'      </Directory>'
	if code64 == code:
		print >>setup,	r'      <Directory Id="ProgramFiles64Folder">'
	else:
		print >>setup,	r'      <Directory Id="ProgramFilesFolder">'
	print >>setup,		r'        <Directory Id="CompanyFolder" Name="dcon Software">'
	print >>setup,		r'          <Directory Id="APPLICATIONFOLDER" Name="' + defFolder + '">'
	print >>setup,		r'            <Directory Id="dirLang" Name="lang">'
	print >>setup,		r'              <Directory Id="dirFR" Name="fr" />'
	print >>setup,		r'              <Directory Id="dirEN" Name="en" />'
	print >>setup,		r'            </Directory>'
	print >>setup,		r'          </Directory>'
	print >>setup,		r'        </Directory>'
	print >>setup,		r'      </Directory>'
	print >>setup,		r'    </Directory>'

	print >>setup,		r'    <DirectoryRef Id="APPLICATIONFOLDER">'
	if code64 == code:
		print >>setup,	r'      <Component Id="C_AgilityBook.exe" Guid="B48C351B-3E9C-4C66-AC0B-150D26047499" Win64="yes">'
	else:
		print >>setup,	r'      <Component Id="C_AgilityBook.exe" Guid="55CB5388-14EA-4927-B2A4-96315EA09418">'
	print >>setup,		r'        <File Id="_AgilityBook.exe"'
	print >>setup,		r'            Name="AgilityBook.exe"'
	print >>setup,		r'            Source="' + baseDir + r'AgilityBook.exe"'
	print >>setup,		r'            KeyPath="yes"'
	print >>setup,		r'            Checksum="yes"'
	print >>setup,		r'            DiskId="1" />'
	#print >>setup,		r'        <!-- The description is the same since that was what MFC set it to (v1) -->'
	print >>setup,		r'        <ProgId Id="AgilityBook.Document"'
	print >>setup,		r'            Description="AgilityBook.Document"'
	print >>setup,		r'            Icon="_AgilityBook.exe">'
	print >>setup,		r'          <Extension Id="arb">'
	print >>setup,		r'            <Verb Id="open"'
	print >>setup,		r'                Command="Open"'
	print >>setup,		r'                TargetFile="_AgilityBook.exe"'
	print >>setup,		r'                Argument="&quot;%1&quot;" />'
	print >>setup,		r'          </Extension>'
	print >>setup,		r'        </ProgId>'
	print >>setup,		r'      </Component>'
	if code64 == code:
		print >>setup,	r'      <Component Id="C_AgilityBook.dat" Guid="C44F2D55-7128-423F-926A-AAE9848D1842" Win64="yes">'
	else:
		print >>setup,	r'      <Component Id="C_AgilityBook.dat" Guid="007C505E-3872-4A92-AF77-2C6C1BC2A9C7">'
	print >>setup,		r'        <File Id="_AgilityBook.dat"'
	print >>setup,		r'            Name="AgilityBook.dat"'
	print >>setup,		r'            Source="' + baseDir + r'AgilityBook.dat"'
	print >>setup,		r'            KeyPath="yes"'
	print >>setup,		r'            DiskId="1" />'
	print >>setup,		r'      </Component>'
	if code64 == code:
		print >>setup,	r'      <Component Id="C_ARBHelp.exe" Guid="6E5D1584-06A0-4024-A197-84B9E0326EA1" Win64="yes">'
	else:
		print >>setup,	r'      <Component Id="C_ARBHelp.exe" Guid="9CB60EE5-C5A3-4219-83C4-A59D2872DC8F">'
	print >>setup,		r'        <File Id="_ARBHelp.exe"'
	print >>setup,		r'            Name="ARBHelp.exe"'
	print >>setup,		r'            Source="' + baseDir + r'ARBHelp.exe"'
	print >>setup,		r'            KeyPath="yes"'
	print >>setup,		r'            Checksum="yes"'
	print >>setup,		r'            DiskId="1" />'
	print >>setup,		r'      </Component>'
	if code64 == code:
		print >>setup,	r'      <Component Id="C_cal_usdaa.dll" Guid="16F399ED-BA94-4CBF-B85E-D79C737D3232" Win64="yes">'
	else:
		print >>setup,	r'      <Component Id="C_cal_usdaa.dll" Guid="8C9B6F50-7F0A-46AA-B813-A66A8300E421">'
	print >>setup,		r'        <File Id="_cal_usdaa.dll"'
	print >>setup,		r'            Name="cal_usdaa.dll"'
	print >>setup,		r'            Source="' + baseDir + r'cal_usdaa.dll"'
	print >>setup,		r'            KeyPath="yes"'
	print >>setup,		r'            Checksum="yes"'
	print >>setup,		r'            DiskId="1" />'
	print >>setup,		r'      </Component>'

	if code64 == code:
		print >>setup,	r'      <Component Id="empty" Guid="" KeyPath="yes" Win64="yes" />'
	else:
		print >>setup,	r'      <Component Id="empty" Guid="" KeyPath="yes" />'
	print >>setup,		r'    </DirectoryRef>'

	print >>setup,		r'    <DirectoryRef Id="dirFR">'
	if code64 == code:
		print >>setup,	r'      <Component Id="C_langFR" Guid="581CB1F1-0846-4205-8551-5FEE8A03AF62" Win64="yes">'
	else:
		print >>setup,	r'      <Component Id="C_langFR" Guid="53DA3663-A90A-44AF-A921-5EC0E104ABAD">'
	print >>setup,		r'        <File Id="_fr_arb.mo"'
	print >>setup,		r'            Name="arb.mo"'
	print >>setup,		r'            Source="' + baseDir + r'lang\fr\arb.mo"'
	print >>setup,		r'            KeyPath="yes"'
	print >>setup,		r'            DiskId="1" />'
	print >>setup,		r'      </Component>'
	print >>setup,		r'    </DirectoryRef>'

	print >>setup,		r'    <DirectoryRef Id="dirEN">'
	if code64 == code:
		print >>setup,	r'      <Component Id="C_langEN" Guid="27FF13B7-5D71-414B-9BA6-1C6D04DE0BA6" Win64="yes">'
	else:
		print >>setup,	r'      <Component Id="C_langEN" Guid="9AFD56DF-F038-4877-A35A-00036A508FD8">'
	print >>setup,		r'        <File Id="_en_arb.mo"'
	print >>setup,		r'            Name="arb.mo"'
	print >>setup,		r'            Source="' + baseDir + r'lang\en\arb.mo"'
	print >>setup,		r'            KeyPath="yes"'
	print >>setup,		r'            DiskId="1" />'
	print >>setup,		r'      </Component>'
	print >>setup,		r'    </DirectoryRef>'

	print >>setup,		r'    <DirectoryRef Id="ARBMenuFolder">'
	if code64 == code:
		print >>setup,	r'      <Component Id="C_MenuShortcutsARB" Guid="74105C3C-0E12-4CE4-B955-DA4551365A97" Win64="yes">'
	else:
		print >>setup,	r'      <Component Id="C_MenuShortcutsARB" Guid="9A2365F0-D0F9-4881-BB49-8CC9B0887FC2">'
	print >>setup,		r'        <RegistryValue'
	print >>setup,		r'            Root="HKCU"'
	print >>setup,		r'            Key="Software\[Manufacturer]\[AppName]\Settings"'
	print >>setup,		r'            Name="inst_help"'
	print >>setup,		r'            Type="integer"'
	print >>setup,		r'            Value="1" />'
	print >>setup,		r'        <Shortcut Id="ShortcutARBExe"'
	print >>setup,		r'            Name="' + defFolder + '"'
	print >>setup,		r'            Target="[#_AgilityBook.exe]" />'
	print >>setup,		r'        <RemoveFolder Id="ARBMenuFolder" On="uninstall" />'
	print >>setup,		r'      </Component>'
	if code64 == code:
		print >>setup,	r'      <Component Id="C_MenuShortcutsHelp" Guid="2F642C5F-5C66-40C8-B548-D7794CBF580A" Win64="yes">'
	else:
		print >>setup,	r'      <Component Id="C_MenuShortcutsHelp" Guid="00212137-B9F3-4E3E-BFFB-7F6442F97741">'
	print >>setup,		r'        <RegistryValue'
	print >>setup,		r'            Root="HKCU"'
	print >>setup,		r'            Key="Software\[Manufacturer]\[AppName]\Settings"'
	print >>setup,		r'            Name="inst_arb"'
	print >>setup,		r'            Type="integer"'
	print >>setup,		r'            Value="1" />'
	print >>setup,		r'        <Shortcut Id="ShortcutARBHelp"'
	print >>setup,		r'            Name="ARB Helper"'
	print >>setup,		r'            Target="[#_ARBHelp.exe]" />'
	print >>setup,		r'      </Component>'
	print >>setup,		r'    </DirectoryRef>'

	print >>setup,		r'    <DirectoryRef Id="DesktopFolder">'
	if code64 == code:
		print >>setup,	r'      <Component Id="C_DeskShortcuts" Guid="988854A1-81B4-4B57-A51D-5C08BE963E5F" Win64="yes">'
	else:
		print >>setup,	r'      <Component Id="C_DeskShortcuts" Guid="99C9A5EE-AF6E-4271-98BC-5B8CE9308CBC">'
	print >>setup,		r'        <RegistryValue'
	print >>setup,		r'            Root="HKCU"'
	print >>setup,		r'            Key="Software\[Manufacturer]\[AppName]\Settings"'
	print >>setup,		r'            Name="inst_desk"'
	print >>setup,		r'            Type="integer"'
	print >>setup,		r'            Value="1" />'
	print >>setup,		r'        <Shortcut Id="AppDesktop"'
	print >>setup,		r'            Name="' + defFolder + '"'
	print >>setup,		r'            Target="[#_AgilityBook.exe]" />'
	print >>setup,		r'      </Component>'
	print >>setup,		r'    </DirectoryRef>'

	print >>setup,		r'    <Feature Id="Complete"'
	print >>setup,		r'        Display="expand"'
	print >>setup,		r'        Level="1"'
	print >>setup,		r'        AllowAdvertise="no"'
	print >>setup,		r'        Title="Agility Record Book"'
	print >>setup,		r'        Description="Main Agility Record Book files"'
	print >>setup,		r'        ConfigurableDirectory="APPLICATIONFOLDER">'
	print >>setup,		r'      <ComponentRef Id="C_MenuShortcutsARB" />'
	print >>setup,		r'      <ComponentRef Id="C_DeskShortcuts" />'
	print >>setup,		r'      <ComponentRef Id="C_AgilityBook.exe" />'
	print >>setup,		r'      <ComponentRef Id="C_AgilityBook.dat" />'
	print >>setup,		r'      <ComponentRef Id="C_langEN" />'
	print >>setup,		r'      <Feature Id="ARBHelp"'
	print >>setup,		r'          Display="expand"'
	print >>setup,		r'          Level="100"'
	print >>setup,		r'          AllowAdvertise="no"'
	print >>setup,		r'          Title="ARBHelp"'
	print >>setup,		r'          Description="Help diagnose problems by assembling necessary files for debugging">'
	print >>setup,		r'        <ComponentRef Id="C_ARBHelp.exe" />'
	print >>setup,		r'        <ComponentRef Id="C_MenuShortcutsHelp" />'
	print >>setup,		r'      </Feature>'
	print >>setup,		r'      <Feature Id="Languages"'
	print >>setup,		r'          Display="expand"'
	print >>setup,		r'          Level="100"'
	print >>setup,		r'          AllowAdvertise="no"'
	print >>setup,		r'          Title="Languages" Description="Supported languages that can be changed while running the program (English is always available)">'
	# Need an empty component to work around a bug with features (empty features add network options)
	print >>setup,		r'        <ComponentRef Id="empty" />'
	print >>setup,		r'        <Feature Id="French"'
	print >>setup,		r'            Level="100"'
	print >>setup,		r'            AllowAdvertise="no"'
	print >>setup,		r'            Title="French" Description="French">'
	print >>setup,		r'          <ComponentRef Id="C_langFR" />'
	print >>setup,		r'        </Feature>'
	print >>setup,		r'      </Feature>'
	print >>setup,		r'      <Feature Id="Plugins"'
	print >>setup,		r'          Display="expand"'
	print >>setup,		r'          Level="100"'
	print >>setup,		r'          AllowAdvertise="no"'
	print >>setup,		r'          Title="Calendar Addins" Description="Addins to download Calendar data directly from a website">'
	# Need an empty component to work around a bug with features (to suppress advertising)
	print >>setup,		r'        <ComponentRef Id="empty" />'
	print >>setup,		r'        <Feature Id="Calendar"'
	print >>setup,		r'            Level="100"'
	print >>setup,		r'            AllowAdvertise="no"'
	print >>setup,		r'            Title="USDAA" Description="Download Calendar entries from usdaa.com">'
	print >>setup,		r'          <ComponentRef Id="C_cal_usdaa.dll" />'
	print >>setup,		r'        </Feature>'
	print >>setup,		r'      </Feature>'
	print >>setup,		r'    </Feature>'

	# Just FYI of how to do this.
	#print >>setup,		r'    <CustomAction Id="LaunchFile" FileKey="App" ExeCommand="/register" Return="ignore" />'

	print >>setup,		r'    <InstallExecuteSequence>'
	print >>setup,		r'      <RemoveExistingProducts After="InstallValidate" />'
	# FYI
	#print >>setup,		r'      <Custom Action="LaunchFile" After="InstallFinalize">NOT Installed</Custom>'
	print >>setup,		r'    </InstallExecuteSequence>'

	print >>setup,		r'    <UI>'
	# Copied from WixUI_FeatureTree.wxs (in order to use my exit dialog which makes the 'launch' checkbox prettier
	print >>setup,		r'      <TextStyle Id="WixUI_Font_Normal" FaceName="Tahoma" Size="8" />'
	print >>setup,		r'      <TextStyle Id="WixUI_Font_Bigger" FaceName="Tahoma" Size="12" />'
	print >>setup,		r'      <TextStyle Id="WixUI_Font_Title" FaceName="Tahoma" Size="9" Bold="yes" />'
	print >>setup,		r'      <Property Id="DefaultUIFont" Value="WixUI_Font_Normal" />'
	print >>setup,		r'      <Property Id="WixUI_Mode" Value="FeatureTree" />'
	print >>setup,		r'      <DialogRef Id="ErrorDlg" />'
	print >>setup,		r'      <DialogRef Id="FatalError" />'
	print >>setup,		r'      <DialogRef Id="FilesInUse" />'
	print >>setup,		r'      <DialogRef Id="MsiRMFilesInUse" />'
	print >>setup,		r'      <DialogRef Id="PrepareDlg" />'
	print >>setup,		r'      <DialogRef Id="ProgressDlg" />'
	print >>setup,		r'      <DialogRef Id="ResumeDlg" />'
	print >>setup,		r'      <DialogRef Id="UserExit" />'
	print >>setup,		r'      <Publish Dialog="MyExitDialog" Control="Finish" Event="EndDialog" Value="Return" Order="999">1</Publish>'
	print >>setup,		r'      <Publish Dialog="WelcomeDlg" Control="Next" Event="NewDialog" Value="LicenseAgreementDlg">1</Publish>'
	print >>setup,		r'      <Publish Dialog="LicenseAgreementDlg" Control="Back" Event="NewDialog" Value="WelcomeDlg">1</Publish>'
	print >>setup,		r'      <Publish Dialog="LicenseAgreementDlg" Control="Next" Event="NewDialog" Value="CustomizeDlg">LicenseAccepted = "1"</Publish>'
	print >>setup,		r'      <Publish Dialog="CustomizeDlg" Control="Back" Event="NewDialog" Value="MaintenanceTypeDlg" Order="1">Installed</Publish>'
	print >>setup,		r'      <Publish Dialog="CustomizeDlg" Control="Back" Event="NewDialog" Value="LicenseAgreementDlg" Order="2">NOT Installed</Publish>'
	print >>setup,		r'      <Publish Dialog="CustomizeDlg" Control="Next" Event="NewDialog" Value="VerifyReadyDlg">1</Publish>'
	print >>setup,		r'      <Publish Dialog="VerifyReadyDlg" Control="Back" Event="NewDialog" Value="CustomizeDlg" Order="1">NOT Installed OR WixUI_InstallMode = "Change"</Publish>'
	print >>setup,		r'      <Publish Dialog="VerifyReadyDlg" Control="Back" Event="NewDialog" Value="MaintenanceTypeDlg" Order="2">Installed</Publish>'
	print >>setup,		r'      <Publish Dialog="MaintenanceWelcomeDlg" Control="Next" Event="NewDialog" Value="MaintenanceTypeDlg">1</Publish>'
	print >>setup,		r'      <Publish Dialog="MaintenanceTypeDlg" Control="ChangeButton" Event="NewDialog" Value="CustomizeDlg">1</Publish>'
	print >>setup,		r'      <Publish Dialog="MaintenanceTypeDlg" Control="RepairButton" Event="NewDialog" Value="VerifyReadyDlg">1</Publish>'
	print >>setup,		r'      <Publish Dialog="MaintenanceTypeDlg" Control="RemoveButton" Event="NewDialog" Value="VerifyReadyDlg">1</Publish>'
	print >>setup,		r'      <Publish Dialog="MaintenanceTypeDlg" Control="Back" Event="NewDialog" Value="MaintenanceWelcomeDlg">1</Publish>'

	print >>setup,		r'      <UIRef Id="WixUI_Common" />'
	print >>setup,		r'      <UIRef Id="WixUI_ErrorProgressText" />'

	# Copied from ExitDialog.wxs and tweaked like http://www.dizzymonkeydesign.com/blog/misc/adding-and-customizing-dlgs-in-wix-3/ suggests
	print >>setup,		r'      <Dialog Id="MyExitDialog" Width="370" Height="270" Title="!(loc.ExitDialog_Title)">'
	print >>setup,		r'        <Control Id="Finish" Type="PushButton" X="236" Y="243" Width="56" Height="17" Default="yes" Cancel="yes" Text="!(loc.WixUIFinish)" />'
	print >>setup,		r'        <Control Id="Cancel" Type="PushButton" X="304" Y="243" Width="56" Height="17" Disabled="yes" Text="!(loc.WixUICancel)" />'
	print >>setup,		r'        <Control Id="Bitmap" Type="Bitmap" X="0" Y="0" Width="370" Height="234" TabSkip="no" Text="!(loc.ExitDialogBitmap)" />'
	print >>setup,		r'        <Control Id="Back" Type="PushButton" X="180" Y="243" Width="56" Height="17" Disabled="yes" Text="!(loc.WixUIBack)" />'
	print >>setup,		r'        <Control Id="BottomLine" Type="Line" X="0" Y="234" Width="370" Height="0" />'
	print >>setup,		r'        <Control Id="Description" Type="Text" X="135" Y="70" Width="220" Height="40" Transparent="yes" NoPrefix="yes" Text="!(loc.ExitDialogDescription)" />'
	print >>setup,		r'        <Control Id="Title" Type="Text" X="135" Y="20" Width="220" Height="60" Transparent="yes" NoPrefix="yes" Text="!(loc.ExitDialogTitle)" />'
	print >>setup,		r'        <Control Id="LaunchCheckBox" Type="CheckBox" X="10" Y="243" Width="170" Height="17" Property="WIXUI_EXITDIALOGOPTIONALCHECKBOX" Hidden="yes" CheckBoxValue="1" Text="Launch [AppName] when setup exits">'
	print >>setup,		r'          <Condition Action="show">NOT Installed</Condition>'
	print >>setup,		r'        </Control>'
	print >>setup,		r'      </Dialog>'

	print >>setup,		r'      <InstallUISequence>'
	print >>setup,		r'        <Show Dialog="MyExitDialog" OnExit="success" />'
	print >>setup,		r'      </InstallUISequence>'
	print >>setup,		r'      <AdminUISequence>'
	print >>setup,		r'        <Show Dialog="MyExitDialog" OnExit="success" />'
	print >>setup,		r'      </AdminUISequence>'

	print >>setup,		r'      <Publish Dialog="MyExitDialog"'
	print >>setup,		r'          Control="Finish"'
	print >>setup,		r'          Event="DoAction"'
	print >>setup,		r'          Value="LaunchApplication">WIXUI_EXITDIALOGOPTIONALCHECKBOX = 1 and NOT Installed</Publish>'
	print >>setup,		r'    </UI>'

	print >>setup,		r'    <Property Id="WIXUI_EXITDIALOGOPTIONALCHECKBOX" Value="1" />'
	print >>setup,		r'    <Property Id="WIXUI_EXITDIALOGOPTIONALCHECKBOXTEXT" Value="Launch Agility Record Book" />'
	print >>setup,		r'    <Property Id="WixShellExecTarget" Value="[#_AgilityBook.exe]" />'
	print >>setup,		r'    <CustomAction Id="LaunchApplication" BinaryKey="WixCA" DllEntry="WixShellExec" Impersonate="yes" />'

	print >>setup,		r'  </Product>'
	print >>setup,		r'</Wix>'

	setup.close()

	if os.access(baseDir + "AgilityBook.exe", os.F_OK):
		runcmd('candle -nologo ' + outputFile + '.wxs')
		# ICE61: This product should remove only older versions of itself. No Maximum version was detected for the current product. (OLDVERSIONFOUND)
		#  - No, I want this to install THIS!
		# ICE69: Mismatched component reference. Entry 'AppDesktop' of the Shortcut table belongs to component 'C_DeskShortcuts'. However, the formatted string in column 'Target' references file '_AgilityBook.exe' which belongs to component 'C_AgilityBook.exe'. Components are in the same feature.
		#  - This is due to making the shortcuts use [#comp] references rather than [directory]exename
		runcmd('light -nologo -sice:ICE61 -sice:ICE69 -dWixUILicenseRtf=License.rtf -ext WixUIExtension -ext WixUtilExtension -cultures:en-us -out "' + outputFile + '.msi" "' + outputFile + '.wixobj"')
		if tidy:
			if os.access(outputFile + ".wxs", os.F_OK):
				os.remove(outputFile + ".wxs")
			if os.access(outputFile + ".wixobj", os.F_OK):
				os.remove(outputFile + ".wixobj")
	else:
		print baseDir + "AgilityBook.exe does not exist, MSI skipped"
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
