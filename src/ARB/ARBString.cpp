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
 *
 * @brief Utility classes
 * @author David Connet
 *
 * Revision History
 * @li 2009-11-24 DRC Optimize locale usage when reading/writing the ARB file.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-03-30 DRC Remove Convert and replaced with tstringA/etc
 * @li 2008-06-29 DRC Moved string stuff out of ARBTypes.
 */

#include "stdafx.h"
#include "ARBString.h"

#include <algorithm>
#include <sstream>
#include <wx/mstream.h>
#include <wx/strconv.h>

#ifdef UNICODE
#define TCERR std::wcerr
#else
#define TCERR std::cerr
#endif

#if defined(_WIN32)
#if _MSC_VER < 1300
#define _tstol		_ttol
#endif
#else // WIN32
// Need to have _tstol and _tcstod defined for other platforms.
// These are atol/strtod functions and need to take wxString::wx_str
// as a parameter (either char or wchar_t depending on compile options)
#define _tstol		atol
#define _tcstod		strtod
#endif // WIN32

/////////////////////////////////////////////////////////////////////////////

wxString tstringUtil::TString(wchar_t const* const inStr, size_t inLen)
{
	return wxString(inStr, wxConvUTF8, inLen);
}


wxString tstringUtil::TString(std::wstring const& inStr)
{
	return wxString(inStr.c_str(), wxConvUTF8, inStr.length());
}


wxString tstringUtil::TString(char const* const inStr, size_t inLen)
{
	return wxString(inStr, wxConvUTF8, inLen);
}


wxString tstringUtil::TString(std::string const& inStr)
{
	return wxString(inStr.c_str(), wxConvUTF8, inStr.length());
}


std::string tstringUtil::tstringA(wxMemoryOutputStream const& inStr)
{
	if (inStr.IsOk())
	{
		wxFileOffset len = inStr.GetLength();
		char* buffer = new char[len+1];
		inStr.CopyTo(buffer, len);
		buffer[len] = 0;
		return std::string(buffer, len);
	}
	else
		return std::string();
}


std::string tstringUtil::tstringA(wxString const& inStr)
{
	return std::string(inStr.ToUTF8());
}


bool tstringUtil::ToDouble(wxString const& inStr, double& outValue)
{
	bool rc = inStr.ToDouble(&outValue);
	if (!rc)
	{
		// This may have failed for 2 reasons:
		// - Bad data.
		// - Different decimal point from Locale.
#if wxCHECK_VERSION(2, 9, 0)
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


bool tstringUtil::ToCLong(wxString const& inStr, long& outValue, bool bRetry)
{
#if wxCHECK_VERSION(2, 9, 0)
	bool bOk = inStr.ToCLong(&outValue);
	// The above fails for "123-45" and returns 0. Before it returned 123.
	// That's the behavior I'm relying on. (Needed when reading dates)
	if (!bOk && bRetry)
	{
		std::string tmp(tstringUtil::tstringA(inStr.wx_str()));
		std::istringstream str(tmp);
		str >> outValue;
	}
	return bOk;
#else
	wxLocale locale(wxLANGUAGE_ENGLISH_US, 0);
	return inStr.ToLong(&outValue);
#endif
}


bool tstringUtil::ToCDouble(wxString const& inStr, double& outValue)
{
#if wxCHECK_VERSION(2, 9, 0)
	// This will fail on "1.2-3". That's ok. The only time this is used
	// is for parsing an actual number in Element.
	return inStr.ToCDouble(&outValue);
#else
	wxLocale locale(wxLANGUAGE_ENGLISH_US, 0);
	return inStr.ToDouble(&outValue);
#endif
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


wxString tstringUtil::Trim(wxString const& inStr)
{
	wxString str(inStr);
	str.Trim(true);
	str.Trim(false);
	return str;
}


std::string tstringUtil::Replace(
		std::string const& inStr,
		std::string const& inReplace,
		std::string const& inReplaceWith)
{
	return ReplaceImpl<std::string, std::ostringstream>(inStr, inReplace, inReplaceWith);
}


std::wstring tstringUtil::Replace(
		std::wstring const& inStr,
		std::wstring const& inReplace,
		std::wstring const& inReplaceWith)
{
	return ReplaceImpl<std::wstring, std::wostringstream>(inStr, inReplace, inReplaceWith);
}
