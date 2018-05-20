#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief BreakLine and CSV handling.
 * @author David Connet
 *
 * Revision History
 * 2010-10-30 Moved BreakLine from Globals.h, added CSV routines.
 */

#include <string>
#include <vector>


/**
 * Separate a line into substrings.
 * Very simple routine that has no escape chars
 * @param inSep Separator character
 * @param inStr String to separate
 * @param outFields Separated pieces
 * @param inRemoveEmpties Remove any empty subfields
 * @return Number of fields parsed.
 * @note Return value may be larger than vector size due to removed entries.
 */
extern size_t BreakLine(
		wchar_t inSep,
		std::wstring const& inStr,
		std::vector<std::wstring>& outFields,
		bool inRemoveEmpties = false);


typedef enum
{
	DataError,		///< Data was ill-formed (quoting)
	DataNeedMore,	///< Last field is incomplete - need more data
	DataOk,			///< Fields were all parsed
} ReadStatus;

/**
 * Read a line from a CSV file [Based on RFC 4180 (October 2005)]
 * @param inSep Separator character
 * @param inRecord Input buffer
 * @param ioFields Parsed fields
 * @param bContinuation If true, the first field parsed will be appended to
 *                      last existing one.
 * @param newLine On continuation, append this.
 * @return Status of whether more data is needed (due to quoting multiline data)
 */
extern ReadStatus ReadCSV(
		wchar_t inSep,
		std::wstring inRecord,
		std::vector<std::wstring>& ioFields,
		bool bContinuation = false,
		std::wstring newLine = std::wstring(L"\n"));


/**
 * Write a line for a CSV file [Based on RFC 4180 (October 2005)]
 * @param inSep Separator character
 * @param inFields Fields to write
 * @return A string that can be written to a file. (not newline terminated)
 */
extern std::wstring WriteCSV(
		wchar_t inSep,
		std::vector<std::wstring> const& inFields);


/**
 * Write a field for a CSV file [Based on RFC 4180 (October 2005)]
 * @param inSep Separator character
 * @param inField Fields to write
 * @return A string that can be written to a file. (not newline terminated)
 */
extern std::wstring WriteCSVField(
		wchar_t inSep,
		std::wstring const& inField);
