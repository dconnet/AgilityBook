# Update the history files in the help and web.
# The file Help/html/HistoryData.html is used with:
#   \AgilityBook\www\templateHistory.html
#  Help/html/HistoryTemplate.html
# to produce:
#   \AgilityBook\www\history.html
#   Help\html\History.html
#
# Revision History
# 2005-01-23 DRC Created

"""UpdateHistory -h -w -p path
-h      Generate Help file History.html (Help\html\History.html)
-w      Generate history file for web (\AgilityBook\www\history.html)
-p path Path to where web files are (default "\AgilityBook\www")
This program should be run from the ...\AgilityBook\src directory.
"""

# This line must exist in any template file.
gTrigger = "<!-- Just copy the help file's HistoryData.html into here -->\n"

import getopt
import string
import sys

def errprint(*args):
	# Used to print exception messages
    strings = map(str, args)
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
		if not line: break
		if line == gTrigger:
			bProcessed = 1
			i = 0
			while i < len(data):
				print >>fileOutput, data[i],
				i = i + 1
		else:
			print >>fileOutput, line,
	fileTemplate.close()
	fileOutput.close()
	if not bProcessed:
		errprint("Warning: History data not inserted into template file (" + inFilename + "), key phrase not found")

def main():
	bHelpfile = 0
	bWebfile = 0
	webPath = "\\AgilityBook\\www"
	rawData = "Help\\html\\HistoryData.html"
	try:
		opts, args = getopt.getopt(sys.argv[1:], "hwp:")
	except getopt.error, msg:
		errprint(msg)
		errprint("Usage: ", __doc__)
		return
	for o, a in opts:
		if "-h" == o:
			bHelpfile = 1
		elif "-w" == o:
			bWebfile = 1
		elif "-p" == o:
			webPath = a
	if 0 != len(args):
		errprint("Usage:", __doc__)
		return

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
		return

	if bHelpfile:
		GenerateFile(data, "Help\\html\\HistoryTemplate.html", "Help\\html\\History.html")

	if bWebfile:
		GenerateFile(data, webPath + "\\template\\templateHistory.html", webPath + "\\history.html")

main()
