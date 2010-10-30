#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief BreakLine and CSV handling.
 * @author David Connet
 *
 * Revision History
 * @li 2010-10-30 DRC Moved BreakLine from Globals.h, added CSV routines.
 */

#include "ARBString.h"
#include <vector>


/**
 * Separate a line into substrings. Very simply routine that has no escape chars
 * @param inSep Separator character
 * @param inStr String to separate
 * @param outFields Separated pieces
 * @return Number of fields
 * @note In Globals.cpp
 */
extern size_t BreakLine(
		wxChar inSep,
		wxString inStr,
		std::vector<wxString>& outFields);


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
 * @return Status of whether more data is needed (due to quoting multiline data)
 */
extern ReadStatus ReadCSV(
		wxChar const inSep,
		wxString inRecord,
		std::vector<wxString>& ioFields,
		bool bContinuation = false);


/**
 * Write a line for a CSV file [Based on RFC 4180 (October 2005)]
 * @param inSep Separator character
 * @param inFields Fields to write
 * @return A string that can be written to a file. (not newline terminated)
 */
extern wxString WriteCSV(
		wxChar const inSep,
		std::vector<wxString> const& inFields);
