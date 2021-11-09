# coding=utf-8
# Above line is for python
#
# GetVSDir(version)
#  Return the given VS directory.
#  version: 7.1, 8.0, 9.0, 10.0, 11.0, 12.0, 14.0, 15.0
#
# GetWindowsSdkDir()
#  Return the currect SDK directory
#
# 2021-11-09 Add vc143 support
# 2019-02-28 Add vc142 support
# 2016-06-10 Made into library
#

import os
import sys
import win32api
import win32con


def GetRegString(hkey, path, value):
	key = None
	try:
		key = win32api.RegOpenKeyEx(hkey, path, 0, win32con.KEY_READ)
	except Exception as msg:
		return ""
	try:
		return win32api.RegQueryValueEx(key, value)[0]
	except Exception as msg:
		return ""


# Return SDK path (with ending slash)
def GetWindowsSdkDir():
	WindowsSdkDir = GetRegString(win32con.HKEY_LOCALMACHINE, r'SOFTWARE\Microsoft\Microsoft SDKs\Windows', 'CurrentInstallFolder')
	if 0 == len(WindowsSdkDir):
		WindowsSdkDir = GetRegString(win32con.HKEY_CURRENT_USER, r'SOFTWARE\Microsoft\Microsoft SDKs\Windows', 'CurrentInstallFolder')
	return WindowsSdkDir


# 7.1, 8.0, 9.0, 10.0, 11.0, 12.0, 14.0, 15.0 (as observed on my machine)
# Yes, VisualStudio2017 == reg("15.0") == _msc_ver191x
# v16+: Reg paths are in %localappdata%\Microsoft\VisualStudio\<version>\privateregistry.bin
# 16.0 (VS2019RC) does not install a key - hard code it
# 17.0 (VS2022) is 64bit now
def GetVSDir(version):
	vsdir = GetRegString(win32con.HKEY_LOCAL_MACHINE, r'SOFTWARE\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir):
		vsdir = GetRegString(win32con.HKEY_CURRENT_USER, r'SOFTWARE\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir):
		vsdir = GetRegString(win32con.HKEY_LOCAL_MACHINE, r'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir):
		vsdir = GetRegString(win32con.HKEY_CURRENT_USER, r'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7', version)
	if 0 == len(vsdir) and version == '16.0':
		vsdir = r'C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional' + '\\'
		if not os.access(vsdir, os.F_OK):
			vsdir = r'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community' + '\\'
			if not os.access(vsdir, os.F_OK):
				vsdir = ''
	if 0 == len(vsdir) and version == '17.0':
		vsdir = r'C:\Program Files\Microsoft Visual Studio\2022\Professional' + '\\'
		if not os.access(vsdir, os.F_OK):
			vsdir = r'C:\Program Files\Microsoft Visual Studio\2022\Community' + '\\'
	return vsdir


if __name__ == '__main__':
	sys.exit(0)
