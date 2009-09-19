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

/////////////////////////////////////////////////////////////////////////////

wxString tstringUtil::TString(wchar_t const* const inStr, size_t inLen)
{
#ifdef UNICODE
	return wxString(inStr, inLen);
#else
	return tstringA(inStr, inLen).c_str();
#endif
}


wxString tstringUtil::TString(std::wstring const& inStr)
{
	return TString(inStr.c_str(), inStr.length());
}


wxString tstringUtil::TString(char const* const inStr, size_t inLen)
{
#ifdef UNICODE
	return wxString(inStr, wxMBConvUTF8());
#else
	return wxString(inStr, inLen);
#endif
}


wxString tstringUtil::TString(std::string const& inStr)
{
	return TString(inStr.c_str(), inStr.length());
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


long tstringUtil::atol(wxString const& inStr, bool* bParsedOk)
{
	long l = 0;
#if wxCHECK_VERSION(2, 9, 0)
	if (!inStr.ToCLong(&l))
	{
		// The above fails for "123-45". Before it returned 123.
		// That's the behavior I'm relying on. (Needed when reading dates)
		std::basic_istringstream<wxChar> str(inStr.wx_str());
		str >> l;
	}
	bool bParsed = true;
#else
	bool bParsed = inStr.ToLong(&l);
#endif
	if (bParsedOk)
		*bParsedOk = bParsed;
	return l;
}


double tstringUtil::strtod(wxString const& inStr, bool* bParsedOk)
{
	double d = 0.0;
#if wxCHECK_VERSION(2, 9, 0)
	// This will fail on "1.2-3". That's ok. The only time this is used
	// is for parsing an actual number.
	bool bParsed = inStr.ToCDouble(&d);
#else
	bool bParsed = inStr.ToDouble(&d);
#endif
	if (bParsedOk)
		*bParsedOk = bParsed;
	return d;
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
