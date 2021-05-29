# coding=utf-8
# Above line is for python
#
# Dual sign binaries (at least until XP support is dropped)
# signtool.exe v6.3+ must be one of:
# - in the PATH
# - specified with SignStuff.py -s option
# - specified in environment with SIGNTOOL_EXE
# The last allows the build machine to work correctly when in the vc10
# environment as that version of signtool is too old.
#
# This script, by default, will only sign when on the build machine.
#
# Revision History
# 2021-05-29 Changed comodoca to sectigo (name change)
# 2018-10-06 Dropping XP support, remove dual signing.
# 2017-10-29 Added -2,-t,-e options.
# 2017-04-09 Fix MSI signing.
# 2016-12-23 Add test to only target dll/exe. Allows script to be put into common project properties.
# 2016-07-05 Fix variables
# 2016-07-01 Created
"""SignStuff.py [-s signtool] [-n CertName] [-f pfx_file] [-p password] [-e] target
	-s signtool: Full path to signtool.exe (Default: SignTool is assumed in PATH)
	-n CertName: Name of installed cert
	-f pfx_file: Path to PFX file (Default: use named cert)
	-p password: Password for PFX file
    -e: Just show the environment, don't do it
	target: exe to sign
Note: The -n/-f options will override the build machine name check
"""

import getopt
import os
import os.path
import subprocess
import sys
import time

# Name of build machine
BuildMachine = 'dcon-build'
CertName = 'David Connet'

SignTool = 'signtool.exe'
SignSHA1cmd = '/t http://timestamp.sectigo.com /v'
SignSHA256cmd = '/fd sha256 /tr http://timestamp.sectigo.com/?td=sha256 /td sha256 /v'

validExt = ['.dll', '.exe', '.msi']


def Run(cmd, onlyTest = False):
	print(cmd)
	if onlyTest:
		return 0
	newenv = os.environ.copy()
	proc = subprocess.Popen(cmd, env=newenv)
	proc.wait()
	return proc.returncode


def CheckFiletype(filename):
	fileext = os.path.splitext(filename)[-1].lower()
	for ext in validExt:
		if fileext == ext:
			return True
	return False


def main():
	global BuildMachine, CertName, SignTool
	global SignSHA1cmd, SignSHA256cmd

	if 'SIGNTOOL_EXE' in os.environ:
		sign = os.environ['SIGNTOOL_EXE']
		if os.access(sign, os.F_OK):
			SignTool = '"' + sign + '"'

	override = False
	pfxFile = ''
	password = ''
	testOnly = False
	try:
		opts, args = getopt.getopt(sys.argv[1:], 's:n:f:p:e')
	except getopt.error as msg:
		print(msg)
		print('Usage:', __doc__)
		return 1
	for o, a in opts:
		if '-s' == o:
			SignTool = '"' + a + '"'
		elif '-n' == o:
			override = True
			CertName = a
		elif '-f' == o:
			override = True
			pfxFile = a
		elif '-p' == o:
			password = a
		elif '-e' == o:
			testOnly = True

	if not len(args) == 1:
		print('Usage:', __doc__)
		return 1

	filename = args[0]
	if not CheckFiletype(filename):
		# Quietly exit. No error.
		return 0

	# Just do a simple compare.
	# Don't care about unicode weirdness with machine names.
	if not override and not os.environ['USERDOMAIN'].lower() == BuildMachine.lower():
		# Quietly exit. No error.
		return 0

	# Put together the command line
	command = SignTool + ' sign'
	if not override or len(pfxFile) == 0:
		command = command + ' /n "' +  CertName + '"'
	else:
		command = command + ' /f "' + pfxFile + '"'
		if len(password) > 0:
			command = command + ' /p "' + password + '"'

	cmdSHA1 = command + ' ' + SignSHA1cmd + ' "' + filename + '"'
	cmdSHA256 = command + ' ' + SignSHA256cmd + ' "' + filename + '"'

	# Sign with sha256
	Run(cmdSHA256, testOnly)

	return 0


if __name__ == '__main__':
	sys.exit(main())
