# Generate the help file.
#
# This could be supported on all compilers, but I only run it on my active
# release compiler (VC8). That's the only solution the ParseResourceHeader
# project is defined in.
#
# Revision History
# 2007-11-21 DRC Add VC9 support. VC9 is Unicode-only, for win98 (non-unicode)
#                builds, use VC8. VC9 config no longer has 'unicode' in name
# 2006-12-15 DRC Translated BuildHelp.bat to python, add Unicode support.
# 2005-06-24 DRC Add better support for multiple compilers
# 2005-01-23 DRC Added History.html creation from a template.
# 2004-05-20 DRC Created

"""BuildHelp configuration compiler
configuration: Release, Debug, Unicode Release, Unicode Debug
compiler: VC8Win32, VC8x64, VC9Win32, VC9x64
"""

hhc = "c:\\Program Files\\HTML Help Workshop\\hhc.exe"

import os
import sys

def RunCommand(cmd):
	print cmd
	(childin, childout) = os.popen4(cmd)
	childin.close()
	while (1):
		line = childout.readline()
		if line:
			print >>sys.stdout, line,
		else:
			break
	childout.close()

def main():
	if len(sys.argv) != 3:
		print >>sys.stderr, "Usage: ", __doc__
		return

	bin = ""
	if sys.argv[2] == "VC8Win32":
		bin = "..\\bin\\" + sys.argv[2]
	elif sys.argv[2] == "VC9Win32":
		bin = "..\\bin\\" + sys.argv[2]
	# 64bit Parse header won't work on a 32bit os!
	# TODO: autodetect os to allow compiling on 64bit os
	#elif sys.argv[2] == "VC8x64":
	#	bin = "..\\bin\\" + sys.argv[2]
	else:
		print >>sys.stderr, "Usage: ", __doc__
		return

	if sys.argv[1] == "Debug":
		bin = bin + "\\" + sys.argv[1]
	elif sys.argv[1] == "Release":
		bin = bin + "\\" + sys.argv[1]
	elif sys.argv[1] == "Unicode Debug":
		bin = bin + "\\" + sys.argv[1]
	elif sys.argv[1] == "Unicode Release":
		bin = bin + "\\" + sys.argv[1]
	else:
		print >>sys.stderr, "Usage: ", __doc__
		return

	parseHeader = bin + "\\ParseResourceHeader.exe"

	if not os.access(parseHeader, os.F_OK):
		print >>sys.stderr, "Error: " + parseHeader + " is missing"
		return

	print "Build Help file..."

	# Generate the context header files from the windows code
	RunCommand("\"" + parseHeader + "\" Win\\resource.h Win\\resource.hm Help\\AgilityBook.txt Help\\AgilityBook.h Help\\contextid.h")
	RunCommand("\"" + parseHeader + "\" Win\\resource.h Win\\resource.hm Help\\AgilityBookFRA.txt Help\\AgilityBook.h Help\\contextid.h")

	# Generate History.html
	RunCommand("UpdateHistory.py -h")

	# Now generate the chm file
	RunCommand("\"" + hhc + "\" AgilityBook.hhp")
	RunCommand("\"" + hhc + "\" AgilityBookFRA.hhp")

	# Finally, copy the chm file into various locations.
	# /r:overwrite readonly, /q: don't show copied filename, /y:no prompt
	print
	print "Copying chm file to build output directories"
	RunCommand(r'%systemroot%\system32\xcopy /r/q/y Help\*.chm "..\bin\VC6\Release\"')
	RunCommand(r'%systemroot%\system32\xcopy /r/q/y Help\*.chm "..\bin\VC7\Release\"')
	RunCommand(r'%systemroot%\system32\xcopy /r/q/y Help\*.chm "..\bin\VC8Win32\Release\"')
	RunCommand(r'%systemroot%\system32\xcopy /r/q/y Help\*.chm "..\bin\VC9Win32\Release\"')
	RunCommand(r'%systemroot%\system32\xcopy /r/q/y Help\*.chm "..\bin\VC9Win32\Debug\"')
	RunCommand(r'%systemroot%\system32\xcopy /r/q/y Help\*.chm "..\bin\VC9x64\Release\"')

main()
