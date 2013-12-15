# Generate the help file.
#
# This could be supported on all compilers, but I only run it on my active
# release compiler (VC8). That's the only solution the ParseResourceHeader
# project is defined in.
#
# Revision History
# 2012-07-04 This file is wildly out of date. Just return an error now.
#            (above comments are wrong too)
# 2007-11-21 Add VC9 support. VC9 is Unicode-only, for win98 (non-unicode)
#            builds, use VC8. VC9 config no longer has 'unicode' in name
# 2006-12-15 Translated BuildHelp.bat to python, add Unicode support.
# 2005-06-24 Add better support for multiple compilers
# 2005-01-23 Added History.html creation from a template.
# 2004-05-20 Created

"""BuildHelp configuration compiler [wx/win]
configuration:
  Release, Debug (all compilers [mbcs: vc6/7/8, unicode: vc9])
  Unicode Release, Unicode Debug (vc6/7/8)
  Debug - No Unicode, Release - No Unicode (vc9)
compiler: VC8Win32, VC8x64, VC9Win32, VC9x64 [compiler+$(PlatformName)]
wx/win: Build help for wx (default) or windows chm
"""

hhc = "c:\\Program Files\\HTML Help Workshop\\hhc.exe"
# Command line addon to winzip
winzip = "c:\Program Files\WinZip\WZZIP.EXE"

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
	print >>sys.stderr, "This file is no longer supported. There currently is no help file."
	return 1

	if len(sys.argv) != 3 and len(sys.argv) != 4:
		print >>sys.stderr, "Usage: ", __doc__
		return 1

	bChmFile = 0
	bin = ""
	if sys.argv[2] == "VC8Win32":
		bin = "..\\bin\\" + sys.argv[2]
	elif sys.argv[2] == "VC9Win32":
		bin = "..\\bin\\" + sys.argv[2]
	else:
		print >>sys.stderr, "Usage: ", __doc__
		return 1
	if len(sys.argv) == 4 and sys.argv[3] == "win":
		bChmFile = 1

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
		return 1

	parseHeader = bin + "\\ParseResourceHeader.exe"

	if not os.access(parseHeader, os.F_OK):
		print >>sys.stderr, "Error: " + parseHeader + " is missing"
		return 1

	print "Build Help file..."

	# Generate the context header files from the windows code
	RunCommand("\"" + parseHeader + "\" Win\\resource.h Win\\resource.hm Help\\AgilityBook.txt Help\\AgilityBook.h Help\\contextid.h")
	RunCommand("\"" + parseHeader + "\" Win\\resource.h Win\\resource.hm Help\\AgilityBookFRA.txt Help\\AgilityBook.h Help\\contextid.h")

	# Generate History.html
	RunCommand("python UpdateHistory.py -h")

	# Now generate the chm file
	if bChmFile:
		RunCommand("\"" + hhc + "\" AgilityBook.hhp")
		RunCommand("\"" + hhc + "\" AgilityBookFRA.hhp")
	else:
		RunCommand("\"" + winzip + "\" -a -r -P Help\\AgilityBook-en.htb AgilityBook.hhp AgilityBook.hhc Index.hhk Help\AgilityBook.h Help\\AgilityBook.txt Help\\contextid.h Help\\stoplist.stp Help\\html\\*.*")
		RunCommand("\"" + winzip + "\" -a -r -P Help\\AgilityBook-fr.htb AgilityBookFRA.hhp AgilityBook.hhc Index.hhk Help\AgilityBook.h Help\\AgilityBookFRA.txt Help\\contextid.h Help\\stoplist.stp Help\\html\\*.*")

	# Finally, copy the chm file into various locations.
	# /r:overwrite readonly, /q: don't show copied filename, /y:no prompt
	print
	if bChmFile:
		print "Copying chm file to build output directories"
		for dir in (r'"..\bin\VC6\Release\"', r'"..\bin\VC7\Release\"', r'"..\bin\VC8Win32\Release\"', r'"..\bin\VC9Win32\Release\"', r'"..\bin\VC9Win32\Debug\"'):
			RunCommand(r'%systemroot%\system32\xcopy /r/q/y Help\*.chm ' + dir)
	else:
		print "Copying htb file to build output directories"
		for langdir in ("en", "fr"):
			for dir in (r'..\bin\VC6\Release', r'..\bin\VC7\Release', r'..\bin\VC8Win32\Release', r'..\bin\VC9Win32\Release', r'..\bin\VC9Win32\Debug'):
				if not os.access(dir, os.F_OK):
					os.mkdir(dir)
				if not os.access(dir + '\\lang', os.F_OK):
					os.mkdir(dir + '\\lang')
				if not os.access(dir + '\\lang\\' + langdir, os.F_OK):
					os.mkdir(dir + '\\lang\\' + langdir)
				RunCommand(r'copy /b/y Help\AgilityBook-' + langdir + r'.htb ' + dir + '\\lang\\' + langdir + r'\AgilityBook.htb')

	return 0


if __name__ == '__main__':
	sys.exit(main())
