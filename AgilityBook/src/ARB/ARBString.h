#pragma once

/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 * @brief Type definitions.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-03-30 DRC Remove Convert and replaced with tstringA/etc
 * @li 2009-03-12 DRC Converting all TCHAR stuff to wxWidgets
 * @li 2008-06-29 DRC Moved string stuff out of ARBTypes.
 */

#include <string>
#include <sstream>
#include <wx/string.h>

typedef std::basic_ostringstream<wxChar> otstringstream;

/**
 * Convert between ansi/unicode
 */
class tstringUtil
{
public:
	static wxString TString(wchar_t const* const inStr, size_t inLen);
	static wxString TString(std::wstring const& inStr);
	static wxString TString(char const* const inStr, size_t inLen);
	static wxString TString(std::string const& inStr);

	// Construct a string, no matter the input
	static std::string tstringA(wchar_t const* const inStr, size_t inLen);
	static std::string tstringA(std::wstring const& inStr);
	static std::string tstringA(char const* const inStr, size_t inLen);
	static std::string tstringA(std::string const& inStr);
	// Construct a wstring, no matter the input
	static std::wstring tstringW(wchar_t const* const inStr, size_t inLen);
	static std::wstring tstringW(std::wstring const& inStr);
	static std::wstring tstringW(char const* const inStr, size_t inLen);
	static std::wstring tstringW(std::string const& inStr);

	static long atol(wxString const& inStr);
	static double strtod(wxString const& inStr);

	// Some CString-style functions
	static std::string Trim(std::string const& inStr);
	static std::wstring Trim(std::wstring const& inStr);
	static std::string Trim(std::string const& inStr, char toTrim);
	static std::wstring Trim(std::wstring const& inStr, wchar_t toTrim);
	static std::string TrimLeft(std::string const& inStr);
	static std::wstring TrimLeft(std::wstring const& inStr);
	static std::string TrimLeft(std::string const& inStr, char toTrim);
	static std::wstring TrimLeft(std::wstring const& inStr, wchar_t toTrim);
	static std::string TrimRight(std::string const& inStr);
	static std::wstring TrimRight(std::wstring const& inStr);
	static std::string TrimRight(std::string const& inStr, char toTrim);
	static std::wstring TrimRight(std::wstring const& inStr, wchar_t toTrim);
	static std::string ToLower(std::string const& inStr);
	static std::wstring ToLower(std::wstring const& inStr);
	static std::string ToUpper(std::string const& inStr);
	static std::wstring ToUpper(std::wstring const& inStr);
	static std::string Replace(
			std::string const& inStr,
			std::string const& inReplace,
			std::string const& inReplaceWith);
	static std::wstring Replace(
			std::wstring const& inStr,
			std::wstring const& inReplace,
			std::wstring const& inReplaceWith);
};
