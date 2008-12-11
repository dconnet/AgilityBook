#pragma once

/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @li 2008-06-29 DRC Moved string stuff out of ARBTypes.
 */

#include <string>
#include <sstream>

// Note, for non-Windows systems, see stdafx.h for additional requirements.
typedef std::basic_ostringstream<TCHAR> otstringstream;
typedef std::basic_string<TCHAR> tstring;
typedef std::basic_ostream<TCHAR, std::char_traits<TCHAR> > otstream;
typedef std::basic_ofstream<TCHAR, std::char_traits<TCHAR> > oftstream;

/**
 * Convert between ansi/unicode
 */
class tstringUtil
{
public:
	static std::string Convert(wchar_t const* const inStr, size_t inLen);
	static std::string Convert(std::wstring const& inStr);
	static std::wstring Convert(char const* const inStr, size_t inLen);
	static std::wstring Convert(std::string const& inStr);

	static long atol(tstring const& inStr);
	static long atol(TCHAR const* inStr);
	static double strtod(tstring const& inStr);
	static double strtod(TCHAR const* inStr);

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
