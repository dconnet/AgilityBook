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
 * @li 2009-03-30 DRC Remove Convert and replaced with stringA/etc
 * @li 2009-03-12 DRC Converting all TCHAR stuff to wxWidgets
 * @li 2008-06-29 DRC Moved string stuff out of ARBTypes.
 */

#include <string>
#include <wx/string.h>
class wxMemoryOutputStream;


/**
 * String utility functions
 */
namespace StringUtil
{
	// Some explicit conversion between wxString and std.
	// In general, we want to always work in std (based on discussion on list)
	wxString stringWX(wchar_t const* const inStr, size_t inLen);
	wxString stringWX(std::wstring const& inStr);
	wxString stringWX(char const* const inStr, size_t inLen);
	wxString stringWX(std::string const& inStr);
	std::wstring stringW(wxString const& inStr);
	std::string stringA(wxMemoryOutputStream const& inStr);

	// String conversion
	std::string stringA(wchar_t const* const inStr, size_t inLen);
	std::string stringA(std::wstring const& inStr);
	std::wstring stringW(char const* const inStr, size_t inLen);
	std::wstring stringW(std::string const& inStr);

	// Some wxString-style functions

	// For consistency with "ToC" routines
	bool ToLong(std::wstring const& inStr, long& outValue);
	long ToLong(std::wstring const& inStr);
	bool ToULong(std::wstring const& inStr, unsigned long& outValue);
	unsigned long ToULong(std::wstring const& inStr);
	/// @note This function differs from the others - instead of only
	/// parsing in the current locale, this will also fall back to English
	/// ('.' for decimal)
	bool ToDouble(std::wstring const& inStr, double& outValue);
	double ToDouble(std::wstring const& inStr);

	// These functions mimic wx2.9.x. They allow wx2.8 usage.
	// The ToC<type> use the "C" locale.
	// Typically, the "ToC" routines are only used in Element.
	// bRetry is for wx2.9+: It fails on "123-23" now. Old behavior returned "123".
	bool ToCLong(std::wstring const& inStr, long& outValue, bool bRetry = false);
	long ToCLong(std::wstring const& inStr);
	bool ToCULong(std::wstring const& inStr, unsigned long& outValue, bool bRetry = false);
	unsigned long ToCULong(std::wstring const& inStr);
	bool ToCDouble(std::wstring const& inStr, double& outValue);
	double ToCDouble(std::wstring const& inStr);

	// Some CString-style functions

	std::string Trim(std::string const& inStr);
	std::wstring Trim(std::wstring const& inStr);
	std::string Trim(std::string const& inStr, char toTrim);
	std::wstring Trim(std::wstring const& inStr, wchar_t toTrim);

	std::string TrimLeft(std::string const& inStr);
	std::wstring TrimLeft(std::wstring const& inStr);
	std::string TrimLeft(std::string const& inStr, char toTrim);
	std::wstring TrimLeft(std::wstring const& inStr, wchar_t toTrim);

	std::string TrimRight(std::string const& inStr);
	std::wstring TrimRight(std::wstring const& inStr);
	std::string TrimRight(std::string const& inStr, char toTrim);
	std::wstring TrimRight(std::wstring const& inStr, wchar_t toTrim);

	std::string ToLower(std::string const& inStr);
	std::wstring ToLower(std::wstring const& inStr);
	std::string ToUpper(std::string const& inStr);
	std::wstring ToUpper(std::wstring const& inStr);

	std::string Replace(
			std::string const& inStr,
			std::string const& inReplace,
			std::string const& inReplaceWith);
	std::wstring Replace(
			std::wstring const& inStr,
			std::wstring const& inReplace,
			std::wstring const& inReplaceWith);
};
