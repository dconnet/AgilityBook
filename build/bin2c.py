# coding=utf-8
# Above line is for python
#
# Revision History
# 2022-01-04 Added -n option
# 2021-12-22 Added -v (verbose) option, fixed globbing
# 2021-12-21 Created from CPP source
"""bin2c.py [-b] [-h] [-H] [-P] [-i indent] [-l] [-m] [-n] [-o] [-v] file [file...]
  -h:   Usage message
  -b:   Use basename of file (default: full path)
  -H:   Generate .h file also (default: false)
  -P:   Pretty print output file (hex, 8 values per line) (default: false)
  -i indent: Indent 'indent' spaces (default (indent==0): one tab)
  -l:   Use 'unsigned long' (default: size_t)
  -m:   Use macro instead of variable for size, implies -H (default: false)
  -n:   Append a null byte at end of array (default: false)
  -o:   Overwrite existing file (default: false)
  -v:   Verbose
"""

import getopt
import glob
import os, os.path
import re
import stat
import sys

k_maxLineLength = 70
header = '/* Generated by bin2c.py, do not edit manually */\n'


def GenName(fileName, useBasename):
	newName = fileName
	if useBasename:
		newName = os.path.basename(newName)
	newName = re.sub(r'\W', '_', newName, re.ASCII)
	fileHeader = newName + '.h'
	fileSrc = newName + '.c'

	return newName, fileHeader, fileSrc


def main():
	bUsage = False
	useBasename = False
	writeHeader = False
	doPrettyPrint = False
	indent = 0
	useLong = False
	useMacro = False
	appendNull = False
	bOverwrite = False
	verbose = False

	try:
		opts, args = getopt.getopt(sys.argv[1:], 'hbHPi:lmnpov') 
	except getopt.error as msg:
		print(msg)
		print('Usage:', __doc__)
		return 1

	for o, a in opts:
		if '-h' == o:
			bUsage = True
		elif '-b' == o:
			useBasename = True
		elif '-H' == o:
			writeHeader = True
		elif '-P' == o:
			doPrettyPrint = True
		elif '-i' == o:
			try:
				indent = int(a)
			except ValueError as msg:
				print(msg)
				print('Usage:', __doc__)
				return 1
		elif '-l' == o:
			useLong = True
		elif '-m' == o:
			useMacro = True
		elif '-n' == o:
			appendNull = True
		elif '-o' == o:
			bOverwrite = True
		elif '-v' == o:
			verbose = True
		else:
			bUsage = True

	if bUsage or len(args) == 0:
		print('Usage:', __doc__)
		return 1

	rawfiles = set()
	for fileName in args:
		for f in glob.glob(fileName):
			rawfiles.add(f)

	files = set()
	for fileName in rawfiles:
		if not os.access(fileName, os.F_OK):
			print('ERROR: Cannot read "' + fileName + '"')
			return 1
		if not bOverwrite:
			newName, fileHeader, fileSrc = GenName(fileName, useBasename)
			if os.access(fileHeader, os.F_OK):
				print('ERROR: File "' + fileHeader + '" already exists. (Use -o to overwrite)')
				return 1
			if os.access(fileSrc, os.F_OK):
				print('ERROR: File "' + fileSrc + '" already exists. (Use -o to overwrite)')
				return 1
		files.add(fileName)

	scope = ''
	for fileName in files:
		newName, fileHeader, fileSrc = GenName(fileName, useBasename)
		if verbose:
			msg = 'Processing ' + fileName + ' into\n   '
			if writeHeader:
				msg = msg + fileHeader + '\n   '
			msg = msg + fileSrc
			print(msg)

		fileSize = os.stat(fileName)[stat.ST_SIZE]
		if appendNull:
			fileSize = fileSize + 1

		fileInput = open(fileName, 'rb')

		if writeHeader:
			outHeader = open(fileHeader, 'w')
			outHeader.write(header)
			outHeader.write('#ifndef __{}_guard\n'.format(newName))
			outHeader.write('#define __{}_guard\n'.format(newName))
			if useMacro:
				outHeader.write('#define {}_size {}\n'.format(newName, fileSize))
			else:
				if useLong:
					outHeader.write('extern const unsigned long {}_size;\n'.format(newName))
				else:
					outHeader.write('extern const size_t {}_size;\n'.format(newName))
			outHeader.write('extern const unsigned char {}[];\n'.format(newName))
			outHeader.write('#endif /* __{}_guard */\n'.format(newName))
			outHeader.close()

		else:
			scope = 'static '

		outSrc = open(fileSrc, 'w')
		outSrc.write(header)
		outSrc.write('/* Contents of \'{}\' */\n'.format(fileName))
		if useMacro:
			outSrc.write('{}const unsigned char {}[{}_size] = {}\n'.format(scope, newName, newName, '{'))
		else:
			if useLong:
				outSrc.write('{}const unsigned long {}_size = {};\n'.format(scope, newName, fileSize))
			else:
				outSrc.write('{}const size_t {}_size = {};\n'.format(scope, newName, fileSize))
			outSrc.write('{}const unsigned char {}[{}] = {}\n'.format(scope, newName, fileSize, '{'))

		fileBytesWritten = 0
		lineLen = 0
		while True:
			buffer = fileInput.read(1024)
			if not len(buffer):
				break

			for b in buffer:
				if type(b) is str:
					b = ord(b)
				if b < 10:
					lineLen = lineLen + 2
				elif b < 100:
					lineLen = lineLen + 3
				else:
					lineLen = lineLen + 4
				bComma = True
				if (doPrettyPrint and fileBytesWritten % 8 == 0) or (not doPrettyPrint and lineLen > k_maxLineLength):
					bComma = False
					if 0 < fileBytesWritten:
						lineLen = 0;
						outSrc.write(',\n')
					if doPrettyPrint:
						if 0 == indent:
							outSrc.write('\t')
						else:
							outSrc.write(' ' * indent)
				if bComma and fileBytesWritten > 0:
					if doPrettyPrint:
						outSrc.write(', ')
					else:
						outSrc.write(',')
				if doPrettyPrint:
					outSrc.write('0x{:02x}'.format(b))
				else:
					outSrc.write('{}'.format(b))
				fileBytesWritten = fileBytesWritten + 1

		if appendNull:
			outSrc.write(',0')
		outSrc.write('};\n')
		outSrc.close()
		fileInput.close()

	print('Processed', len(files), 'files')

	return 0


if __name__ == '__main__':
	sys.exit(main())
