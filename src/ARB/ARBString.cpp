/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Utility classes
 * @author David Connet
 *
 * Revision History
 * @li 2010-12-30 DRC Fix a memory leak when transforming a stream.
 * @li 2009-11-24 DRC Optimize locale usage when reading/writing the ARB file.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-03-30 DRC Remove Convert and replaced with stringA/etc
 * @li 2008-06-29 DRC Moved string stuff out of ARBTypes.
 */

#include "stdafx.h"
#include "ARBString.h"

#include <algorithm>
#include <locale>
#include <sstream>
#include <wx/mstream.h>
#include <wx/strconv.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

namespace StringUtil
{

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


std::string stringA(wxMemoryOutputStream const& inStr)
{
	std::string str;
	if (inStr.IsOk())
	{
		wxFileOffset len = inStr.GetLength();
		char* buffer = new char[len+1];
		inStr.CopyTo(buffer, len);
		buffer[len] = 0;
		str = std::string(buffer, len);
		delete [] buffer;
	}
	return str;
}


std::string stringA(wxString const& inStr)
{
	return std::string(inStr.ToUTF8());
}

// Note, if not on Windows (HAS_WIDECHARTOMULTIBYTE not set) and not using
// wxWidgets, MBCS/Unicode conversions won't necessarily work right.

std::string stringA(wchar_t const* const inStr, size_t inLen)
{
#if defined(__WXWINDOWS__)
	return std::string(stringWX(inStr, inLen).ToUTF8());
#else // __WXWINDOWS__
	std::string str;
	if (inStr && *inStr)
	{
#if defined(HAS_WIDECHARTOMULTIBYTE)
		int bytes = ::WideCharToMultiByte(CP_UTF8, 0, inStr, static_cast<int>(inLen), 0, 0, NULL, NULL);
		if (bytes > 0)
		{
			char* ansistr = new char[bytes + 1];
			::WideCharToMultiByte(CP_UTF8, 0, inStr, static_cast<int>(inLen), ansistr, bytes, NULL, NULL);
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
		str = tstringA(std::wstring(inStr, inLen));
#endif
	}
	return str;
#endif // __WXWINDOWS__
}


std::string stringA(std::wstring const& inStr)
{
#if defined(__WXWINDOWS__)
	return std::string(stringWX(inStr).ToUTF8());
#else // __WXWINDOWS__
	std::string str;
	if (!inStr.empty())
	{
#if defined(HAS_WIDECHARTOMULTIBYTE)
		str = tstringA(inStr.c_str(), inStr.length());
#else
		size_t nNeeded;
#if defined(__WXWINDOWS__)
		wxMBConvUTF8 converter(wxMBConvUTF8::MAP_INVALID_UTF8_TO_PUA);
		nNeeded = converter.FromWChar(NULL, 0, inStr.c_str());
#elif defined(ARB_HAS_SECURE_MBS_WCS)
		wcstombs_s(&nNeeded, (char*)NULL, 0, inStr.c_str(), 0);
#else
		nNeeded = wcstombs((char*)NULL, inStr.c_str(), 0);
#endif
		if (0 < nNeeded)
		{
			char *ansistr = new char[nNeeded+1];
#if defined(__WXWINDOWS__)
			converter.FromWChar(ansistr, nNeeded+1, inStr.c_str());
#elif defined(ARB_HAS_SECURE_MBS_WCS)
			wcstombs_s(NULL, ansistr, nNeeded+1, inStr.c_str(), _TRUNCATE);
#else
			wcstombs(ansistr, inStr.c_str(), nNeeded);
#endif
			ansistr[nNeeded] = NULL;
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
#if defined(__WXWINDOWS__)
	return std::wstring(stringWX(inStr, inLen).wx_str());
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
		str = tstringW(std::string(inStr, inLen));
#endif
	}
	return str;
#endif // __WXWINDOWS__
}


std::wstring stringW(std::string const& inStr)
{
#if defined(__WXWINDOWS__)
	return std::wstring(stringWX(inStr).wx_str());
#else // __WXWINDOWS__
	std::wstring str;
	if (!inStr.empty())
	{
#if defined(HAS_WIDECHARTOMULTIBYTE)
		str = tstringW(inStr.c_str(), inStr.length());
#else
		size_t nNeeded;
#if defined(__WXWINDOWS__)
		wxMBConvUTF8 converter(wxMBConvUTF8::MAP_INVALID_UTF8_TO_PUA);
		nNeeded = converter.ToWChar(NULL, 0, inStr.c_str());
#elif defined(ARB_HAS_SECURE_MBS_WCS)
		mbstowcs_s(&nNeeded, (wchar_t*)NULL, 0, inStr.c_str(), 0);
#else
		nNeeded = mbstowcs((wchar_t*)NULL, inStr.c_str(), 0);
#endif
		if (0 < nNeeded)
		{
			wchar_t *unicodestr = new wchar_t[nNeeded+1];
#if defined(__WXWINDOWS__)
			converter.ToWChar(unicodestr, nNeeded+1, inStr.c_str());
#elif defined(ARB_HAS_SECURE_MBS_WCS)
			mbstowcs_s(NULL, unicodestr, nNeeded+1, inStr.c_str(), _TRUNCATE);
#else
			mbstowcs(unicodestr, inStr.c_str(), nNeeded);
#endif
			unicodestr[nNeeded] = NULL;
			str = unicodestr;
			delete [] unicodestr;
		}
#endif
	}
	return str;
#endif // __WXWINDOWS__
}


bool ToLong(wxString const& inStr, long& outValue)
{
	return inStr.ToLong(&outValue);
}


long ToLong(wxString const& inStr)
{
	long val = 0;
	ToLong(inStr, val);
	return val;
}


bool ToDouble(wxString const& inStr, double& outValue)
{
	bool rc = inStr.ToDouble(&outValue);
	if (!rc)
	{
		// This may have failed for 2 reasons:
		// - Bad data.
		// - Different decimal point from Locale.
#if wxCHECK_VERSION(2, 9, 3)
		wxUniChar pt = '.';
#else
		wxChar pt = '.';
#endif
		wxString decimalPt = wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER);
		if (0 < decimalPt.length())
			pt = decimalPt.GetChar(0);
		// So we only reparse if the incoming string does not contain
		// the locale's decimal point.
		if (pt != wxT('.') && wxString::npos == inStr.find(pt))
		{
			wxLocale locale(wxLANGUAGE_ENGLISH_US, 0);
			rc = inStr.ToDouble(&outValue);
		}
	}
	return rc;
}


double ToDouble(wxString const& inStr)
{
	double val = 0.0;
	ToDouble(inStr, val);
	return val;
}


bool ToCLong(wxString const& inStr, long& outValue, bool bRetry)
{
#if wxCHECK_VERSION(2, 9, 3)
	bool bOk = inStr.ToCLong(&outValue);
	// The above fails for "123-45" and returns 0. Before it returned 123.
	// That's the behavior I'm relying on. (Needed when reading dates)
	if (!bOk && bRetry)
	{
		std::string tmp(StringUtil::stringA(inStr.ToStdWstring()));
		std::istringstream str(tmp);
		str >> outValue;
	}
	return bOk;
#else
	wxLocale locale(wxLANGUAGE_ENGLISH_US, 0);
	return inStr.ToLong(&outValue);
#endif
}


long ToCLong(wxString const& inStr)
{
	long val = 0;
	ToCLong(inStr, val, true);
	return val;
}


bool ToCDouble(wxString const& inStr, double& outValue)
{
#if wxCHECK_VERSION(2, 9, 3)
	// This will fail on "1.2-3". That's ok. The only time this is used
	// is for parsing an actual number in Element.
	return inStr.ToCDouble(&outValue);
#else
	wxLocale locale(wxLANGUAGE_ENGLISH_US, 0);
	return inStr.ToDouble(&outValue);
#endif
}


double ToCDouble(wxString const& inStr)
{
	double val = 0.0;
	ToCDouble(inStr, val);
	return val;
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


wxString Trim(wxString const& inStr)
{
	wxString str(inStr);
	str.Trim(true);
	str.Trim(false);
	return str;
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

};
