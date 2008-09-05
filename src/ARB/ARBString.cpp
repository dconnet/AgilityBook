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
 *
 * @brief Utility classes
 * @author David Connet
 *
 * Revision History
 * @li 2008-06-29 DRC Moved string stuff out of ARBTypes.
 */

#include "stdafx.h"
#include "ARBString.h"
#include <algorithm>
#include <locale>

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

#ifdef UNICODE
#define TCERR std::wcerr
#else
#define TCERR std::cerr
#endif

/////////////////////////////////////////////////////////////////////////////

std::string tstringUtil::Convert(wchar_t const* const inStr, size_t inLen)
{
	std::string str;
	if (inStr && *inStr)
	{
#ifdef _WIN32
#if defined(_MFC_VER) && _MSC_VER >= 1300
		CStringA convert(inStr, static_cast<int>(inLen));
		str = (LPCSTR)convert;
#else
		int lenA = ::WideCharToMultiByte(CP_UTF8, 0, inStr, inLen, 0, 0, NULL, NULL);
		if (lenA > 0)
		{
			char* ansistr = new char[lenA + 1];
			::WideCharToMultiByte(CP_UTF8, 0, inStr, inLen, ansistr, lenA, NULL, NULL);
			ansistr[lenA] = 0;
			str = std::string(ansistr, lenA);
			delete [] ansistr;
		}
		else
		{
			// handle the error
			//DWORD dwErr = GetLastError();
		}
#endif
#else
#error Not yet implemented
#endif
	}
	return str;
}


std::wstring tstringUtil::Convert(char const* const inStr, size_t inLen)
{
	std::wstring str;
	if (inStr && *inStr)
	{
#ifdef _WIN32
#if defined(_MFC_VER) && _MSC_VER >= 1300
		CStringW convert(inStr, static_cast<int>(inLen));
		str = (LPCWSTR)convert;
#else
		int lenW = ::MultiByteToWideChar(CP_UTF8, 0, inStr, inLen, 0, 0);
		if (lenW > 0)
		{
			// Check whether conversion was successful
			wchar_t* unicodestr = new wchar_t[lenW + 1];
			::MultiByteToWideChar(CP_UTF8, 0, inStr, inLen, unicodestr, lenW);
			unicodestr[lenW] = 0;
			str = unicodestr;
			delete [] unicodestr;
		}
		else
		{
			// handle the error
			//DWORD dwErr = GetLastError();
		}
#endif
#else
#error Not yet implemented
#endif
	}
	return str;
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


std::string tstringUtil::Trim(std::string const& inStr)
{
	return TrimImpl<std::string>(inStr, sc_Whitespace, eTrimBoth);
}


std::wstring tstringUtil::Trim(std::wstring const& inStr)
{
	return TrimImpl<std::wstring>(inStr, sc_wWhitespace, eTrimBoth);
}


std::string tstringUtil::Trim(std::string const& inStr, char toTrim)
{
	return TrimImpl<std::string>(inStr, std::string(1, toTrim), eTrimBoth);
}


std::wstring tstringUtil::Trim(std::wstring const& inStr, wchar_t toTrim)
{
	return TrimImpl<std::wstring>(inStr, std::wstring(1, toTrim), eTrimBoth);
}


std::string tstringUtil::TrimLeft(std::string const& inStr)
{
	return TrimImpl<std::string>(inStr, sc_Whitespace, eTrimLeft);
}


std::wstring tstringUtil::TrimLeft(std::wstring const& inStr)
{
	return TrimImpl<std::wstring>(inStr, sc_wWhitespace, eTrimLeft);
}


std::string tstringUtil::TrimLeft(std::string const& inStr, char toTrim)
{
	return TrimImpl<std::string>(inStr, std::string(1, toTrim), eTrimLeft);
}


std::wstring tstringUtil::TrimLeft(std::wstring const& inStr, wchar_t toTrim)
{
	return TrimImpl<std::wstring>(inStr, std::wstring(1, toTrim), eTrimLeft);
}


std::string tstringUtil::TrimRight(std::string const& inStr)
{
	return TrimImpl<std::string>(inStr, sc_Whitespace, eTrimRight);
}


std::wstring tstringUtil::TrimRight(std::wstring const& inStr)
{
	return TrimImpl<std::wstring>(inStr, sc_wWhitespace, eTrimRight);
}


std::string tstringUtil::TrimRight(std::string const& inStr, char toTrim)
{
	return TrimImpl<std::string>(inStr, std::string(1, toTrim), eTrimRight);
}


std::wstring tstringUtil::TrimRight(std::wstring const& inStr, wchar_t toTrim)
{
	return TrimImpl<std::wstring>(inStr, std::wstring(1, toTrim), eTrimRight);
}


static void my_tolower(char& ch)
{
	ch = std::tolower(ch, std::locale::classic());
}


std::string tstringUtil::ToLower(std::string const& inStr)
{
	std::string out(inStr);
	std::for_each(out.begin(), out.end(), my_tolower);
	return out;
}


static void my_wtolower(wchar_t& ch)
{
	ch = std::tolower(ch, std::locale::classic());
}


std::wstring tstringUtil::ToLower(std::wstring const& inStr)
{
	std::wstring out(inStr);
	std::for_each(out.begin(), out.end(), my_wtolower);
	return out;
}


static void my_toupper(char& ch)
{
	ch = std::toupper(ch, std::locale::classic());
}


std::string tstringUtil::ToUpper(std::string const& inStr)
{
	std::string out(inStr);
	std::for_each(out.begin(), out.end(), my_toupper);
	return out;
}


static void my_wtoupper(wchar_t& ch)
{
	ch = std::toupper(ch, std::locale::classic());
}


std::wstring tstringUtil::ToUpper(std::wstring const& inStr)
{
	std::wstring out(inStr);
	std::for_each(out.begin(), out.end(), my_wtoupper);
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
		T::size_type pos = text.find(inReplace);
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
