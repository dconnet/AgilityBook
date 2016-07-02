# coding=utf-8
# Above line is for python
#
# Dual sign binaries (at least until XP support is dropped)
# signtool.exe v6.3+ must be:
# - in the PATH
# - specified with -s option
# - specified in ENV with SIGNTOOL_EXE
# The last allows the build machine to work correctly when in the vc10
# environment as that version of signtool is too old.
#
# Revision History
# 2016-07-01 Created
"""SignStuff.py [-s signtool] [-n CertName] [-f pfx_file] [-p password] target
	-s signtool: Full path to signtool.exe (Default: SignTool is assumed in PATH)
	-n CertName: Name of installed cert
	-f pfx_file: Path to PFX file (Default: use named cert)
	-p password: Password for PFX file
	target: exe to sign
Note: The -n/-f/-p options will override the build machine name check
"""

import getopt
import os
import subprocess
import sys
import time

# Name of build machine
BuildMachine = 'dcon-build'
CertName = 'David Connet'

SignTool = 'signtool.exe'
SignSHA1 = '/v /t http://timestamp.verisign.com/scripts/timestamp.dll'
SignSHA256 = '/v /fd sha256 /tr http://timestamp.geotrust.com/tsa /td sha256 /as'


def Run(cmd, onlyTest = False):
	print(cmd)
	if onlyTest:
		return 0
	newenv = os.environ.copy()
	proc = subprocess.Popen(cmd, env=newenv)
	proc.wait()
	return proc.returncode


def main():
	global BuildMachine, CertName, SignTool

	if 'SIGNTOOL_EXE' in os.environ:
		sign = os.environ['SIGNTOOL_EXE']
		if os.access(sign, os.F_OK):
			SignTool = '"' + sign + '"'

	override = False
	pfxFile = ''
	password = ''
	try:
		opts, args = getopt.getopt(sys.argv[1:], 's:n:f:p:')
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

	if not len(args) == 1:
		print('Usage:', __doc__)
		return 1

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

	cmdSHA1 = command + ' ' + SignSHA1 + ' "' + args[0] + '"'
	cmdSHA256 = command + ' ' + SignSHA256 + ' "' + args[0] + '"'

	# Sign with sha1 (for XP)
	Run(cmdSHA1)

	# Running the 2nd signtool too quickly results in a file-in-use error
	time.sleep(5)

	# Add dual sign for modern OSs
	Run(cmdSHA256)

	return 0


if __name__ == '__main__':
	sys.exit(main())
