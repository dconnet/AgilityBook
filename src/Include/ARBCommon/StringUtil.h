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
 * 2018-04-19 Fixed string/double parsing for locales.
 * 2014-11-17 Enhanced FormatBytes
 * 2014-06-24 Added CompareNoCase.
 * 2012-08-12 Moved FormatBytes to StringUtil
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-03-30 Remove Convert and replaced with stringA/etc
 * 2009-03-12 Converting all TCHAR stuff to wxWidgets
 * 2008-06-29 Moved string stuff out of ARBTypes.
 */

#include "LibwxARBCommon.h"

#include <string>

#if defined(__WXWINDOWS__)
#include <wx/string.h>
class wxMemoryOutputStream;
#endif


/**
 * String utility functions
 */
namespace StringUtil
{
ARBCOMMON_API std::wstring GetTranslation(wchar_t const* const inId);

#if defined(__WXWINDOWS__)
// Some explicit conversion between wxString and std.
// In general, we want to always work in std (based on discussion on list)
ARBCOMMON_API wxString stringWX(wchar_t const* const inStr, size_t inLen);
ARBCOMMON_API wxString stringWX(std::wstring const& inStr);
ARBCOMMON_API wxString stringWX(char const* const inStr, size_t inLen);
ARBCOMMON_API wxString stringWX(std::string const& inStr);
ARBCOMMON_API std::wstring stringW(wxString const& inStr);
ARBCOMMON_API std::string stringA(wxString const& inStr);
ARBCOMMON_API std::string stringA(wxMemoryOutputStream const& inStr);
#endif

// String conversion
ARBCOMMON_API std::string stringA(wchar_t const* const inStr, size_t inLen);
ARBCOMMON_API std::string stringA(std::wstring const& inStr);
ARBCOMMON_API std::wstring stringW(char const* const inStr, size_t inLen);
ARBCOMMON_API std::wstring stringW(std::string const& inStr);

// Some wxString-style functions

// For consistency with "ToC" routines
ARBCOMMON_API bool ToLong(std::wstring const& inStr, long& outValue);
ARBCOMMON_API long ToLong(std::wstring const& inStr);
ARBCOMMON_API bool ToULong(std::wstring const& inStr, unsigned long& outValue);
ARBCOMMON_API unsigned long ToULong(std::wstring const& inStr);
/// @note This function differs from the others - instead of only
/// parsing in the current locale, this will also fall back to English
/// ('.' for decimal)
ARBCOMMON_API bool ToDouble(std::wstring const& inStr, double& outValue);
ARBCOMMON_API double ToDouble(std::wstring const& inStr);

// These functions mimic wx2.9.x. They allow wx2.8 usage.
// The ToC<type> use the "C" locale.
// Typically, the "ToC" routines are only used in Element.
// bRetry is for wx2.9+: It fails on "123-23" now. Old behavior returned "123".
ARBCOMMON_API bool ToCLong(std::wstring const& inStr, long& outValue, bool bRetry = false);
ARBCOMMON_API long ToCLong(std::wstring const& inStr);
ARBCOMMON_API bool ToCULong(std::wstring const& inStr, unsigned long& outValue, bool bRetry = false);
ARBCOMMON_API unsigned long ToCULong(std::wstring const& inStr);
ARBCOMMON_API bool ToCDouble(std::wstring const& inStr, double& outValue);
ARBCOMMON_API double ToCDouble(std::wstring const& inStr);

// The following CompareNoCase functions can compare strings with digits
// ("x2" will work before "x10")
ARBCOMMON_API bool CanCompareDigits();

// Case insensitive comparison (for better lexical comparison)
// It does not mean that lower case sorts before (or after) upper case.
ARBCOMMON_API int CompareNoCase(std::wstring const& inStr1, std::wstring const& inStr2);
#if defined(__WXWINDOWS__)
ARBCOMMON_API int CompareNoCase(wxString const& inStr1, wxString const& inStr2);
#endif

// Some CString-style functions

ARBCOMMON_API std::string Trim(std::string const& inStr);
ARBCOMMON_API std::wstring Trim(std::wstring const& inStr);
ARBCOMMON_API std::string Trim(std::string const& inStr, char toTrim);
ARBCOMMON_API std::wstring Trim(std::wstring const& inStr, wchar_t toTrim);

ARBCOMMON_API std::string TrimLeft(std::string const& inStr);
ARBCOMMON_API std::wstring TrimLeft(std::wstring const& inStr);
ARBCOMMON_API std::string TrimLeft(std::string const& inStr, char toTrim);
ARBCOMMON_API std::wstring TrimLeft(std::wstring const& inStr, wchar_t toTrim);

ARBCOMMON_API std::string TrimRight(std::string const& inStr);
ARBCOMMON_API std::wstring TrimRight(std::wstring const& inStr);
ARBCOMMON_API std::string TrimRight(std::string const& inStr, char toTrim);
ARBCOMMON_API std::wstring TrimRight(std::wstring const& inStr, wchar_t toTrim);

ARBCOMMON_API std::string ToLower(std::string const& inStr);
ARBCOMMON_API std::wstring ToLower(std::wstring const& inStr);
ARBCOMMON_API std::string ToUpper(std::string const& inStr);
ARBCOMMON_API std::wstring ToUpper(std::wstring const& inStr);

ARBCOMMON_API std::string Replace(
	std::string const& inStr,
	std::string const& inReplace,
	std::string const& inReplaceWith);
ARBCOMMON_API std::wstring Replace(
	std::wstring const& inStr,
	std::wstring const& inReplace,
	std::wstring const& inReplaceWith);

enum class ByteSizeStyle
{
	SI,     // KB == 1000 btyes
	Binary, // KiB == 1024 bytes
	True,   // KB == 1024 bytes (non-IEC)
};

/**
 * Return a formatted string using IEC binary prefixes.
 * @param inSize Number of bytes
 * @param inPrec Precision (passed to ARBDouble::ToString)
 * @param inSizeStyle Use SI, binary, or programmer (true!) units
 */
ARBCOMMON_API std::wstring FormatBytes(
	double inSize,
	int inPrec = 2,
	ByteSizeStyle inSizeStyle = ByteSizeStyle::Binary);
}; // namespace StringUtil
