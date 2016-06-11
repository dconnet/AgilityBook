# Update the history files in the help and web.
# The file Help/html/HistoryData.html is used with:
#   \dcon\www\agilityrecordbook\template\templateHistory.php
#   Help\html\HistoryTemplate.html
# to produce:
#   \dcon\www\agilityrecordbook\history.php
#   Help\html\History.html
#
# Revision History
# 2016-06-10 Convert to Python3
# 2010-01-05 Updated comments and www directory.
# 2005-01-23 Created

"""UpdateHistory -h -w -p path
-h      Generate Help file History.html (Help\\html\\History.html)
-w      Generate history file for web (\\dcon\\www\\agilityrecordbook\\history.php)
-p path Path to where web files are (default "\\dcon\\www\\agilityrecordbook")
This program should be run from the ...\\AgilityBook\\src directory.
"""

# Path to arb web files
defAgilityBookWeb = r"\dcon\www\agilityrecordbook"

# This line must exist in any template file.
gTrigger = "<!-- Just copy the help file's HistoryData.html into here -->"

import getopt
import string
import sys

def errprint(*args):
	# Used to print exception messages
	strings = list(map(str, args))
	msg = ' '.join(strings)
	if msg[-1:] != '\n':
		msg += '\n'
	sys.stderr.write(msg)

def GenerateFile(data, inFilename, outFilename):
	fileTemplate = open(inFilename, "r")
	fileOutput = open(outFilename, "w")
	bProcessed = 0
	while 1:
		line = fileTemplate.readline()
		if not line:
			break
		line = str.rstrip(line)
		if line == gTrigger:
			bProcessed = 1
			i = 0
			while i < len(data):
				print(data[i], file=fileOutput)
				i = i + 1
		else:
			print(line, file=fileOutput)
	fileTemplate.close()
	fileOutput.close()
	if not bProcessed:
		errprint("Warning: History data not inserted into template file (" + inFilename + "), key phrase not found")

def main():
	bHelpfile = 0
	bWebfile = 0
	webPath = defAgilityBookWeb
	rawData = r"Help\html\HistoryData.html"
	try:
		opts, args = getopt.getopt(sys.argv[1:], "hwp:")
	except getopt.error as msg:
		errprint(msg)
		errprint("Usage: ", __doc__)
		return 1
	for o, a in opts:
		if "-h" == o:
			bHelpfile = 1
		elif "-w" == o:
			bWebfile = 1
		elif "-p" == o:
			webPath = a
	if 0 != len(args):
		errprint("Usage:", __doc__)
		return 1

	# Load the raw data.
	data = {}
	try:
		file = open(rawData, "r")
		i = 0
		while 1:
			line = file.readline()
			if not line: break
			data[i] = line
			i = i + 1
		file.close()
	except:
		errprint("Error: Cannot read " + rawData)
		return 1

	if bHelpfile:
		GenerateFile(data, r"Help\html\HistoryTemplate.html", r"Help\html\History.html")

	if bWebfile:
		GenerateFile(data, webPath + r"\template\templateHistory.php", webPath + r"\history.php")

	return 0


if __name__ == '__main__':
	sys.exit(main())
