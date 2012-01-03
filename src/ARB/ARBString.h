#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
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
	static wxString WXString(wchar_t const* const inStr, size_t inLen);
	static wxString WXString(std::wstring const& inStr);
	static wxString WXString(char const* const inStr, size_t inLen);
	static wxString WXString(std::string const& inStr);
	static std::string tstringA(wxMemoryOutputStream const& inStr);
	static std::string tstringA(wxString const& inStr);

	// For consistency with "ToC" routines
	static bool ToLong(wxString const& inStr, long& outValue);
	static long ToLong(wxString const& inStr);
	/// @note This function differs from the others - instead of only
	/// parsing in the current locale, this will also fall back to English
	/// ('.' for decimal)
	static bool ToDouble(wxString const& inStr, double& outValue);
	static double ToDouble(wxString const& inStr);

	// These functions mimic wx2.9.x. They allow wx2.8 usage.
	// The ToC<type> use the "C" locale.
	// Typically, the "ToC" routines are only used in Element.
	// bRetry is for wx2.9+: It fails on "123-23" now. Old behavior returned "123".
	static bool ToCLong(wxString const& inStr, long& outValue, bool bRetry = false);
	static long ToCLong(wxString const& inStr);
	static bool ToCDouble(wxString const& inStr, double& outValue);
	static double ToCDouble(wxString const& inStr);

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
