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
#include <wx/string.h>
class wxMemoryOutputStream;


/**
 * String utility functions
 */
class tstringUtil
{
public:
	static wxString TString(wchar_t const* const inStr, size_t inLen);
	static wxString TString(std::wstring const& inStr);
	static wxString TString(char const* const inStr, size_t inLen);
	static wxString TString(std::string const& inStr);
	static std::string tstringA(wxMemoryOutputStream const& inStr);
	static std::string tstringA(wxString const& inStr);

	// For consistency with "ToC" routines
	static bool ToLong(wxString const& inStr, long& outValue)
	{
		return inStr.ToLong(&outValue);
	}
	static long ToLong(wxString const& inStr)
	{
		long val = 0;
		ToLong(inStr, val);
		return val;
	}
	/// @note This function differs from the others - instead of only
	/// parsing in the current locale, this will also fall back to English
	/// ('.' for decimal)
	static bool ToDouble(wxString const& inStr, double& outValue);
	static double ToDouble(wxString const& inStr)
	{
		double val = 0.0;
		ToDouble(inStr, val);
		return val;
	}

	// These functions mimic wx2.9.x. They allow wx2.8 usage.
	// The ToC<type> use the "C" locale.
	// Typically, the "ToC" routines are only used in Element.
	// bRetry is for wx2.9+: It fails on "123-23" now. Old behavior returned "123".
	static bool ToCLong(wxString const& inStr, long& outValue, bool bRetry = false);
	static int ToCLong(wxString const& inStr)
	{
		long val = 0;
		ToCLong(inStr, val, true);
		return static_cast<int>(val);
	}
	static bool ToCDouble(wxString const& inStr, double& outValue);
	static double ToCDouble(wxString const& inStr)
	{
		double val = 0.0;
		ToCDouble(inStr, val);
		return val;
	}

	static wxString Trim(wxString const& inStr);

	static std::string Replace(
			std::string const& inStr,
			std::string const& inReplace,
			std::string const& inReplaceWith);
	static std::wstring Replace(
			std::wstring const& inStr,
			std::wstring const& inReplace,
			std::wstring const& inReplaceWith);
};
