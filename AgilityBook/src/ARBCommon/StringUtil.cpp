/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Utility classes
 * @author David Connet
 *
 * Revision History
 * 2014-11-17 Enhanced FormatBytes (added true units as they're meant to be!)
 * 2014-08-28 Enhanced FormatBytes
 * 2014-06-24 Added CompareNoCase.
 * 2013-01-25 Better non-wx support.
 * 2012-08-12 Moved FormatBytes to StringUtil
 * 2012-04-10 Based on wx-group thread, use std::string for internal use
 * 2010-12-30 Fix a memory leak when transforming a stream.
 * 2009-11-24 Optimize locale usage when reading/writing the ARB file.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-03-30 Remove Convert and replaced with stringA/etc
 * 2008-06-29 Moved string stuff out of ARBTypes.
 */

#include "stdafx.h"
#include "ARBCommon/StringUtil.h"

#include "ARBCommon/ARBMisc.h"
#include "ARBCommon/ARBTypes.h"
#include <algorithm>
#include <locale>
#include <sstream>
#if defined(__WXWINDOWS__)
#include <wx/mstream.h>
#include <wx/strconv.h>
#endif

#if defined(WIN32) && (_WIN32_IE >= _WIN32_IE_IE55) && (WINVER < _WIN32_WINNT_WIN7)
#include "Shlwapi.h"
#pragma comment(lib, "shlwapi.lib")
#endif

#ifdef _WIN32
#define HAS_WIDECHARTOMULTIBYTE

#if !defined(SORT_DIGITSASNUMBERS)
// Only defined when: (WINVER >= _WIN32_WINNT_WIN7)
// Note: Move to CompareStringEx once XP support is dropped
#define SORT_DIGITSASNUMBERS      0x00000008  // use digits as numbers sort method
#endif

#else
#if !defined(__WXWINDOWS__)
#pragma PRAGMA_TODO(This will not produce UTF8, hence will fail tests)
#endif
#endif

// Conversion methods all use wxWidgets by default.
// This is useful for testing.
#define USE_CRT 0

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

namespace StringUtil
{

std::wstring GetTranslation(wchar_t const* const inId)
{
#if defined(__WXWINDOWS__)
	return stringW(wxGetTranslation(inId));
#else
#pragma PRAGMA_TODO(nonWx i18n)
	return std::wstring(inId);
#endif
}

#if defined(__WXWINDOWS__)

wxString stringWX(wchar_t const* const inStr, size_t inLen)
{
	return wxString(inStr, wxConvUTF8, inLen);
}


wxString stringWX(std::wstring const& inStr)
{
	return wxString(inStr.c_str(), wxConvUTF8, inStr.length());
}


wxString stringWX(char const* const inStr, size_t inLen)
{
	return wxString(inStr, wxConvUTF8, inLen);
}


wxString stringWX(std::string const& inStr)
{
	return wxString(inStr.c_str(), wxConvUTF8, inStr.length());
}


std::wstring stringW(wxString const& inStr)
{
#if wxCHECK_VERSION(3, 0, 0) && defined(wxUSE_STD_STRING) && wxUSE_STD_STRING
	return inStr.ToStdWstring();
#else
	return std::wstring(inStr.wx_str());
#endif
}


std::string stringA(wxMemoryOutputStream const& inStr)
{
	std::string str;
	if (inStr.IsOk())
	{
		size_t len = static_cast<size_t>(inStr.GetLength());
		char* buffer = new char[len+1];
		inStr.CopyTo(buffer, len);
		buffer[len] = 0;
		str = std::string(buffer, len);
		delete [] buffer;
	}
	return str;
}

#endif // __WXWINDOWS__

// Note, if not on Windows (HAS_WIDECHARTOMULTIBYTE not set) and not using
// wxWidgets, MBCS/Unicode conversions won't necessarily work right.

std::string stringA(wchar_t const* const inStr, size_t inLen)
{
#if defined(__WXWINDOWS__) && !USE_CRT
	return std::string(stringWX(inStr, inLen).ToUTF8());
#else // __WXWINDOWS__
	std::string str;
	if (inStr && *inStr)
	{
#if defined(HAS_WIDECHARTOMULTIBYTE)
		int bytes = ::WideCharToMultiByte(CP_UTF8, 0, inStr, static_cast<int>(inLen), 0, 0, nullptr, nullptr);
		if (bytes > 0)
		{
			char* ansistr = new char[bytes + 1];
			::WideCharToMultiByte(CP_UTF8, 0, inStr, static_cast<int>(inLen), ansistr, bytes, nullptr, nullptr);
			ansistr[bytes] = 0;
			str = std::string(ansistr, bytes);
			delete [] ansistr;
		}
		else
		{
			// handle the error
			//DWORD dwErr = GetLastError();
		}
#else
		// Create a copy because wcstombs doesn't use length.
		str = stringA(std::wstring(inStr, inLen));
#endif
	}
	return str;
#endif // __WXWINDOWS__
}


std::string stringA(std::wstring const& inStr)
{
#if defined(__WXWINDOWS__) && !USE_CRT
	return std::string(stringWX(inStr).ToUTF8());
#else // __WXWINDOWS__
	std::string str;
	if (!inStr.empty())
	{
#if defined(HAS_WIDECHARTOMULTIBYTE)
		str = stringA(inStr.c_str(), inStr.length());
#else
		size_t nNeeded;
#if defined(ARB_HAS_SECURE_MBS_WCS)
		wcstombs_s(&nNeeded, nullptr, 0, inStr.c_str(), inStr.length());
#else
		nNeeded = wcstombs(nullptr, inStr.c_str(), inStr.length());
#endif
		if (0 < nNeeded)
		{
			char *ansistr = new char[nNeeded+1];
#if defined(ARB_HAS_SECURE_MBS_WCS)
			wcstombs_s(nullptr, ansistr, nNeeded+1, inStr.c_str(), _TRUNCATE);
#else
			wcstombs(ansistr, inStr.c_str(), nNeeded);
#endif
			ansistr[nNeeded] = nullptr;
			str = ansistr;
			delete [] ansistr;
		}
#endif
	}
	return str;
#endif // __WXWINDOWS__
}


std::wstring stringW(char const* const inStr, size_t inLen)
{
#if defined(__WXWINDOWS__) && !USE_CRT
	return StringUtil::stringW(stringWX(inStr, inLen));
#else // __WXWINDOWS__
	std::wstring str;
	if (inStr && *inStr)
	{
#if defined(HAS_WIDECHARTOMULTIBYTE)
		int chars = ::MultiByteToWideChar(CP_UTF8, 0, inStr, static_cast<int>(inLen), 0, 0);
		if (chars > 0)
		{
			// Check whether conversion was successful
			wchar_t* unicodestr = new wchar_t[chars + 1];
			::MultiByteToWideChar(CP_UTF8, 0, inStr, static_cast<int>(inLen), unicodestr, chars);
			unicodestr[chars] = 0;
			str = unicodestr;
			delete [] unicodestr;
		}
		else
		{
			// handle the error
			//DWORD dwErr = GetLastError();
		}
#else
		// Create a copy because mbstowcs doesn't use length.
		str = stringW(std::string(inStr, inLen));
#endif
	}
	return str;
#endif // __WXWINDOWS__
}


std::wstring stringW(std::string const& inStr)
{
#if defined(__WXWINDOWS__) && !USE_CRT
	return StringUtil::stringW(stringWX(inStr));
#else // __WXWINDOWS__
	std::wstring str;
	if (!inStr.empty())
	{
#if defined(HAS_WIDECHARTOMULTIBYTE)
		str = stringW(inStr.c_str(), inStr.length());
#else
		size_t nNeeded;
#if defined(ARB_HAS_SECURE_MBS_WCS)
		mbstowcs_s(&nNeeded, nullptr, 0, inStr.c_str(), 0);
#else
		nNeeded = mbstowcs(nullptr, inStr.c_str(), 0);
#endif
		if (0 < nNeeded)
		{
			wchar_t *unicodestr = new wchar_t[nNeeded+1];
#if defined(ARB_HAS_SECURE_MBS_WCS)
			mbstowcs_s(nullptr, unicodestr, nNeeded+1, inStr.c_str(), _TRUNCATE);
#else
			mbstowcs(unicodestr, inStr.c_str(), nNeeded);
#endif
			unicodestr[nNeeded] = nullptr;
			str = unicodestr;
			delete [] unicodestr;
		}
#endif
	}
	return str;
#endif // __WXWINDOWS__
}


bool ToLong(std::wstring const& inStr, long& outValue)
{
#if defined(__WXWINDOWS__) && !USE_CRT
	wxString s(inStr.c_str());
	return s.ToLong(&outValue);
#else
	wchar_t* end = nullptr;
	outValue = wcstol(inStr.c_str(), &end, 10);
	return !end || !*end;
#endif
}


long ToLong(std::wstring const& inStr)
{
	long val = 0;
	ToLong(inStr, val);
	return val;
}


bool ToULong(std::wstring const& inStr, unsigned long& outValue)
{
#if defined(__WXWINDOWS__) && !USE_CRT
	wxString s(inStr.c_str());
	return s.ToULong(&outValue);
#else
	wchar_t* end = nullptr;
	outValue = wcstoul(inStr.c_str(), &end, 10);
	return !end || !*end;
#endif
}


unsigned long ToULong(std::wstring const& inStr)
{
	unsigned long val = 0;
	ToULong(inStr, val);
	return val;
}


bool ToDouble(std::wstring const& inStr, double& outValue)
{
	bool rc = false;
#if defined(__WXWINDOWS__) && !USE_CRT
	wxString s(inStr.c_str());
	rc = s.ToDouble(&outValue);
#else
	wchar_t* end = nullptr;
	outValue = wcstod(inStr.c_str(), &end);
	rc = (!end || !*end);
#endif
	if (!rc)
	{
		// This may have failed for 2 reasons:
		// - Bad data.
		// - Different decimal point from Locale.
		wchar_t pt = L'.';
#if defined(__WXWINDOWS__) && !USE_CRT
		wxString decimalPt = wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER);
		if (0 < decimalPt.length())
			pt = decimalPt.GetChar(0);
#else
		{
			struct lconv* l = localeconv();
			if (l && l->decimal_point)
				pt = *(l->decimal_point);
		}
#endif
		// So we only reparse if the incoming string does not contain
		// the locale's decimal point.
		if (pt != L'.' && std::wstring::npos == inStr.find(pt))
		{
#if defined(__WXWINDOWS__) && !USE_CRT
			wxLocale locale(wxLANGUAGE_ENGLISH_US, 0);
			rc = s.ToDouble(&outValue);
#else
			wchar_t* end = nullptr;
			_locale_t locale = _create_locale(LC_NUMERIC, "C");
			outValue = _wcstod_l(inStr.c_str(), &end, locale);
			_free_locale(locale);
			rc = (!end || !*end);
#endif
		}
	}
	return rc;
}


double ToDouble(std::wstring const& inStr)
{
	double val = 0.0;
	ToDouble(inStr, val);
	return val;
}


bool ToCLong(std::wstring const& inStr, long& outValue, bool bRetry)
{
#if defined(__WXWINDOWS__) && !USE_CRT
	wxString s(inStr.c_str());
#if wxCHECK_VERSION(3, 0, 0)
	bool bOk = s.ToCLong(&outValue);
	// The above fails for "123-45" and returns 0. Before it returned 123.
	// That's the behavior I'm relying on. (Needed when reading dates)
	if (!bOk && bRetry)
	{
		std::wistringstream str(inStr);
		str >> outValue;
	}
	return bOk;
#else
	wxLocale locale(wxLANGUAGE_ENGLISH_US, 0);
	return s.ToLong(&outValue);
#endif
#else
	wchar_t* end = nullptr;
	_locale_t locale = _create_locale(LC_NUMERIC, "C");
	outValue = _wcstol_l(inStr.c_str(), &end, 10, locale);
	_free_locale(locale);
	return !end || !*end;
#endif
}


long ToCLong(std::wstring const& inStr)
{
	long val = 0;
	ToCLong(inStr, val, true);
	return val;
}


bool ToCULong(std::wstring const& inStr, unsigned long& outValue, bool bRetry)
{
#if defined(__WXWINDOWS__) && !USE_CRT
	wxString s(inStr.c_str());
#if wxCHECK_VERSION(3, 0, 0)
	bool bOk = s.ToCULong(&outValue);
	// The above fails for "123-45" and returns 0. Before it returned 123.
	// That's the behavior I'm relying on. (Needed when reading dates)
	if (!bOk && bRetry)
	{
		std::wistringstream str(inStr);
		str >> outValue;
	}
	return bOk;
#else
	wxLocale locale(wxLANGUAGE_ENGLISH_US, 0);
	return s.ToULong(&outValue);
#endif
#else
	wchar_t* end = nullptr;
	_locale_t locale = _create_locale(LC_NUMERIC, "C");
	outValue = _wcstoul_l(inStr.c_str(), &end, 10, locale);
	_free_locale(locale);
	return !end || !*end;
#endif
}


unsigned long ToCULong(std::wstring const& inStr)
{
	unsigned long val = 0;
	ToCULong(inStr, val, true);
	return val;
}


bool ToCDouble(std::wstring const& inStr, double& outValue)
{
#if defined(__WXWINDOWS__) && !USE_CRT
#if wxCHECK_VERSION(3, 0, 0)
	// This will fail on "1.2-3". That's ok. The only time this is used
	// is for parsing an actual number in Element.
	return stringWX(inStr).ToCDouble(&outValue);
#else
	wxLocale locale(wxLANGUAGE_ENGLISH_US, 0);
	return stringWX(inStr).ToDouble(&outValue);
#endif
#else
	wchar_t* end = nullptr;
	_locale_t locale = _create_locale(LC_NUMERIC, "C");
	outValue = _wcstod_l(inStr.c_str(), &end, locale);
	_free_locale(locale);
	return !end || !*end;
#endif
}


double ToCDouble(std::wstring const& inStr)
{
	double val = 0.0;
	ToCDouble(inStr, val);
	return val;
}


static const char* const sc_Whitespace = "\r\n\t ";
static const wchar_t* const sc_wWhitespace = L"\r\n\t ";
enum TrimType
{
	eTrimLeft = 0x1,
	eTrimRight = 0x2,
	eTrimBoth = eTrimLeft | eTrimRight
};


template <typename T> T TrimImpl(T const& inStr, T const& toTrim, TrimType type)
{
	if (0 == inStr.length())
		return inStr;
	typename T::size_type posFirst = 0;
	if (type & eTrimLeft)
	{
		posFirst = inStr.find_first_not_of(toTrim);
		if (T::npos == posFirst)
			return T();
	}
	typename T::size_type posLast = inStr.length() - 1;
	if (type & eTrimRight)
	{
		posLast = inStr.find_last_not_of(toTrim);
		if (T::npos == posLast)
			return T();
	}
	if (inStr.length() == posLast - posFirst + 1)
		return inStr;
	typename T::size_type posLength = posLast - posFirst + 1;
	return inStr.substr(posFirst, posLength);
}


static bool UseCompareString()
{
#ifdef WIN32
#if (WINVER >= _WIN32_WINNT_WIN7)
	return true;
#else
	static bool bCheckedOS = false;
	static bool bUseCompareString = false;

	if (!bCheckedOS)
	{
		bCheckedOS = true;
		bUseCompareString = IsWin7OrBetter();
	}

	return bUseCompareString;
#endif
#else
	return false;
#endif
}


bool CanCompareDigits()
{
#if defined(WIN32) && (_WIN32_IE >= _WIN32_IE_IE55) && (WINVER < _WIN32_WINNT_WIN7)
	return true;
#else 
	return UseCompareString();
#endif
}


int CompareNoCase(std::wstring const& inStr1, std::wstring const& inStr2)
{
#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PC_APP || WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
	// Yeah, changing from case insensitive to sensitive. Deal.
	return inStr1.compare(inStr2);
#elif defined(WIN32)
	// WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
	if (UseCompareString())
	{
		switch (CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNORECASE | SORT_DIGITSASNUMBERS, inStr1.c_str(), (int)inStr1.length(), inStr2.c_str(), (int)inStr2.length()))
		{
		case CSTR_LESS_THAN:
			return -1;
		case CSTR_EQUAL:
			return 0;
		case CSTR_GREATER_THAN:
			return 1;
		// Fall thru on error
		default:
			break;
		}
	}
#if (_WIN32_IE >= _WIN32_IE_IE55)
	return StrCmpLogicalW(inStr1.c_str(), inStr2.c_str());
#else
	// Yeah, changing from case insensitive to sensitive. Deal.
	return inStr1.compare(inStr2);
#endif
#else
	// Yeah, changing from case insensitive to sensitive. Deal.
	return inStr1.compare(inStr2);
#endif
}


#if defined(__WXWINDOWS__)
int CompareNoCase(wxString const& inStr1, wxString const& inStr2)
{
#ifdef WIN32
#if wxCHECK_VERSION(3, 0, 0) && defined(wxUSE_STD_STRING) && wxUSE_STD_STRING
	return CompareNoCase(inStr1.ToStdWstring(), inStr2.ToStdWstring());
#else
	return CompareNoCase(std::wstring(inStr1.wx_str()), std::wstring(inStr2.wx_str()));
#endif
#else
	return inStr1.CmpNoCase(inStr2);
#endif
}
#endif


std::string Trim(std::string const& inStr)
{
	return TrimImpl<std::string>(inStr, sc_Whitespace, eTrimBoth);
}


std::wstring Trim(std::wstring const& inStr)
{
	return TrimImpl<std::wstring>(inStr, sc_wWhitespace, eTrimBoth);
}


std::string Trim(std::string const& inStr, char toTrim)
{
	return TrimImpl<std::string>(inStr, std::string(1, toTrim), eTrimBoth);
}


std::wstring Trim(std::wstring const& inStr, wchar_t toTrim)
{
	return TrimImpl<std::wstring>(inStr, std::wstring(1, toTrim), eTrimBoth);
}


std::string TrimLeft(std::string const& inStr)
{
	return TrimImpl<std::string>(inStr, sc_Whitespace, eTrimLeft);
}


std::wstring TrimLeft(std::wstring const& inStr)
{
	return TrimImpl<std::wstring>(inStr, sc_wWhitespace, eTrimLeft);
}


std::string TrimLeft(std::string const& inStr, char toTrim)
{
	return TrimImpl<std::string>(inStr, std::string(1, toTrim), eTrimLeft);
}


std::wstring TrimLeft(std::wstring const& inStr, wchar_t toTrim)
{
	return TrimImpl<std::wstring>(inStr, std::wstring(1, toTrim), eTrimLeft);
}


std::string TrimRight(std::string const& inStr)
{
	return TrimImpl<std::string>(inStr, sc_Whitespace, eTrimRight);
}


std::wstring TrimRight(std::wstring const& inStr)
{
	return TrimImpl<std::wstring>(inStr, sc_wWhitespace, eTrimRight);
}


std::string TrimRight(std::string const& inStr, char toTrim)
{
	return TrimImpl<std::string>(inStr, std::string(1, toTrim), eTrimRight);
}


std::wstring TrimRight(std::wstring const& inStr, wchar_t toTrim)
{
	return TrimImpl<std::wstring>(inStr, std::wstring(1, toTrim), eTrimRight);
}


#ifndef ARB_HAS_LAMBDA
static void my_tolower(char& ch)
{
	ch = std::tolower(ch, std::locale::classic());
}
#endif

std::string ToLower(std::string const& inStr)
{
	std::string out(inStr);
#ifdef ARB_HAS_LAMBDA
	std::for_each(out.begin(), out.end(), [](char& ch) {ch = std::tolower(ch, std::locale::classic());});
#else
	std::for_each(out.begin(), out.end(), my_tolower);
#endif
	return out;
}


#ifndef ARB_HAS_LAMBDA
static void my_wtolower(wchar_t& ch)
{
	ch = std::tolower(ch, std::locale::classic());
}
#endif


std::wstring ToLower(std::wstring const& inStr)
{
	std::wstring out(inStr);
#ifdef ARB_HAS_LAMBDA
	std::for_each(out.begin(), out.end(), [](wchar_t& ch) {ch = std::tolower(ch, std::locale::classic());});
#else
	std::for_each(out.begin(), out.end(), my_wtolower);
#endif
	return out;
}


#ifndef ARB_HAS_LAMBDA
static void my_toupper(char& ch)
{
	ch = std::toupper(ch, std::locale::classic());
}
#endif


std::string ToUpper(std::string const& inStr)
{
	std::string out(inStr);
#ifdef ARB_HAS_LAMBDA
	std::for_each(out.begin(), out.end(), [](char& ch) {ch = std::toupper(ch, std::locale::classic());});
#else
	std::for_each(out.begin(), out.end(), my_toupper);
#endif
	return out;
}


#ifndef ARB_HAS_LAMBDA
static void my_wtoupper(wchar_t& ch)
{
	ch = std::toupper(ch, std::locale::classic());
}
#endif


std::wstring ToUpper(std::wstring const& inStr)
{
	std::wstring out(inStr);
#ifdef ARB_HAS_LAMBDA
	std::for_each(out.begin(), out.end(), [](wchar_t& ch) {ch = std::toupper(ch, std::locale::classic());});
#else
	std::for_each(out.begin(), out.end(), my_wtoupper);
#endif
	return out;
}


template <typename T, typename S> T ReplaceImpl(
		T const& inStr,
		T const& inReplace,
		T const& inReplaceWith)
{
	if (inReplace.empty())
		return inStr;
	S str;
	T text(inStr);
	while (!text.empty())
	{
		typename T::size_type pos = text.find(inReplace);
		if (T::npos != pos)
		{
			if (0 < pos)
			{
				str << text.substr(0, pos);
			}
			if (!inReplaceWith.empty())
				str << inReplaceWith;
			text = text.substr(pos+inReplace.length(), T::npos);
		}
		else
		{
			str << text;
			text.erase();
		}
	}
	return str.str();
}


std::string Replace(
		std::string const& inStr,
		std::string const& inReplace,
		std::string const& inReplaceWith)
{
	return ReplaceImpl<std::string, std::ostringstream>(inStr, inReplace, inReplaceWith);
}


std::wstring Replace(
		std::wstring const& inStr,
		std::wstring const& inReplace,
		std::wstring const& inReplaceWith)
{
	return ReplaceImpl<std::wstring, std::wostringstream>(inStr, inReplace, inReplaceWith);
}


// Using IEC binary prefixes
std::wstring FormatBytes(
		double inSize,
		int inPrec,
		ByteSizeStyle inSizeStyle)
{
	// byte, kibibyte, mebibyte, gibibyte, tebibyte, pebibyte, exbibyte
	static wchar_t const * const sc_unitsSI[]     = {L" B", L" kB",  L" MB",  L" GB",  L" TB",  L" PB",  L" EB"};
	static wchar_t const * const sc_unitsBinary[] = {L" B", L" KiB", L" MiB", L" GiB", L" TiB", L" PiB", L" EiB"};
	static wchar_t const * const sc_unitsTrue[]   = {L" B", L" KB",  L" MB",  L" GB",  L" TB",  L" PB",  L" EB"};
	static const struct
	{
		wchar_t const* const* units;
		size_t numUnits;
		double base;
	} sc_units[] = {
		{sc_unitsSI, ARRAY_SIZE(sc_unitsSI), 1000.0},
		{sc_unitsBinary, ARRAY_SIZE(sc_unitsBinary), 1024.0},
		{sc_unitsTrue, ARRAY_SIZE(sc_unitsTrue), 1024.0},
	};

	size_t unitIndex = 0;
	switch (inSizeStyle)
	{
	default:
		assert(0);
		// fall thru
	case eBytesSI:
		unitIndex = 0;
		break;
	case eBytesBinary:
		unitIndex = 1;
		break;
	case eBytesTrue:
		unitIndex = 2;
		break;
	}

	int index = 0;
	if (abs(inSize) != 0.0)
	{
		index = std::min(static_cast<int>(log(inSize) / log(sc_units[unitIndex].base)), static_cast<int>(sc_units[unitIndex].numUnits) - 1);
		inSize /= pow(sc_units[unitIndex].base, index);
	}
	std::wstring form(sc_units[unitIndex].units[index]);

	std::wstring val = ARBDouble::ToString(inSize, inPrec, ARBDouble::eDefault, true) + form;
	return val;
}

};
