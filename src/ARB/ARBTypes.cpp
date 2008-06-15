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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Removed ARBDouble.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "stdafx.h"
#include "ARBTypes.h"
#include <iostream>
#include <math.h>
#include <time.h>

#include "ARBLocalization.h"
#include "Element.h"

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
#ifdef _MFC_VER
		CStringA convert(inStr, static_cast<int>(inLen));
		str = (LPCSTR)convert;
#else
		int lenA = ::WideCharToMultiByte(CP_ACP, 0, inStr, inLen, 0, 0, NULL, NULL);
		if (lenA > 0)
		{
			char* ansistr = new char[lenA + 1];
			::WideCharToMultiByte(CP_ACP, 0, inStr, inLen, ansistr, lenA, NULL, NULL);
			ansistr[lenA] = 0;
			str = ansistr;
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
#ifdef _MFC_VER
		CStringW convert(inStr, static_cast<int>(inLen));
		str = (LPCWSTR)convert;
#else
		int lenW = ::MultiByteToWideChar(CP_ACP, 0, inStr, inLen, 0, 0);
		if (lenW > 0)
		{
			// Check whether conversion was successful
			wchar_t* unicodestr = new wchar_t[lenW + 1];
			::MultiByteToWideChar(CP_ACP, 0, inStr, inLen, unicodestr, lenW);
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


template <typename T, typename C> T TrimImpl(T const& inStr, C toTrim)
{
	T::size_type posFirst = inStr.find_first_not_of(toTrim);
	if (T::npos == posFirst)
		return T();
	T::size_type posLast = inStr.find_last_not_of(toTrim);
	if (inStr.length() == posLast - posFirst + 1)
		return inStr;
	T::size_type posLength = posLast - posFirst + 1;
	return inStr.substr(posFirst, posLength);
}


std::string tstringUtil::Trim(std::string const& inStr, char toTrim)
{
	return TrimImpl<std::string, char>(inStr, toTrim);
}


std::wstring tstringUtil::Trim(std::wstring const& inStr, wchar_t toTrim)
{
	return TrimImpl<std::wstring, wchar_t>(inStr, toTrim);
}


template <typename T, typename C> T TrimLeftImpl(T const& inStr, C toTrim)
{
	T::size_type pos = inStr.find_first_not_of(toTrim);
	if (T::npos == pos)
		return T();
	else if (0 == pos)
		return inStr;
	return inStr.substr(pos);
}


std::string tstringUtil::TrimLeft(std::string const& inStr, char toTrim)
{
	return TrimLeftImpl<std::string, char>(inStr, toTrim);
}


std::wstring tstringUtil::TrimLeft(std::wstring const& inStr, wchar_t toTrim)
{
	return TrimLeftImpl<std::wstring, wchar_t>(inStr, toTrim);
}


template <typename T, typename C> T TrimRightImpl(T const& inStr, C toTrim)
{
	T::size_type pos = inStr.find_last_not_of(toTrim);
	if (T::npos == pos)
		return T();
	else if (inStr.length() == pos + 1)
		return inStr;
	return inStr.substr(0, pos + 1);
}


std::string tstringUtil::TrimRight(std::string const& inStr, char toTrim)
{
	return TrimRightImpl<std::string, char>(inStr, toTrim);
}


std::wstring tstringUtil::TrimRight(std::wstring const& inStr, wchar_t toTrim)
{
	return TrimRightImpl<std::wstring, wchar_t>(inStr, toTrim);
}


template <typename T, typename C> T ToLowerImpl(T const& inStr)
{
	T out;
	if (!inStr.empty())
	{
		size_t len = inStr.length();
		out.append(len, ' ');
		for (size_t i = 0; i < len; ++i)
		{
			out[i] = static_cast<C>(tolower(inStr[i]));
		}
	}
	return out;
}


std::string tstringUtil::ToLower(std::string const& inStr)
{
	return ToLowerImpl<std::string, char>(inStr);
}


std::wstring tstringUtil::ToLower(std::wstring const& inStr)
{
	return ToLowerImpl<std::wstring, wchar_t>(inStr);
}


template <typename T, typename C> T ToUpperImpl(T const& inStr)
{
	T out;
	if (!inStr.empty())
	{
		size_t len = inStr.length();
		out.append(len, ' ');
		for (size_t i = 0; i < len; ++i)
		{
			out[i] = static_cast<C>(toupper(inStr[i]));
		}
	}
	return out;
}


std::string tstringUtil::ToUpper(std::string const& inStr)
{
	return ToUpperImpl<std::string, char>(inStr);
}


std::wstring tstringUtil::ToUpper(std::wstring const& inStr)
{
	return ToUpperImpl<std::wstring, wchar_t>(inStr);
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

/////////////////////////////////////////////////////////////////////////////

void DumpErrorMessage(tstring const& inMsg, bool bIncNewLine)
{
#if defined(_MFC_VER)
	if (bIncNewLine)
		TRACE(_T("%s\n"), inMsg.c_str());
	else
		TRACE(_T("%s"), inMsg.c_str());
#else
	if (bIncNewLine)
		TCERR << inMsg << std::endl;
	else
		TCERR << inMsg;
#endif
}


tstring SanitizeStringForHTML(
		tstring const& inRawData,
		bool bConvertCR)
{
	tstring::size_type pos = inRawData.find_first_of(_T("&<>"));
	if (tstring::npos == pos && bConvertCR)
		pos = inRawData.find_first_of(_T("\r\n"));
	if (tstring::npos == pos)
		return inRawData;
	otstringstream data;
	for (size_t nChar = 0; nChar < inRawData.length(); ++nChar)
	{
		switch (inRawData[nChar])
		{
		case _T('&'):
			data << _T("&amp;");
			break;
		case _T('<'):
			data << _T("&lt;");
			break;
		case _T('>'):
			data << _T("&gt;");
			break;
		case _T('\r'):
			if (bConvertCR)
			{
				if (nChar + 1 < inRawData.length() && '\n' == inRawData[nChar+1])
					continue;
				else
					data << _T("<br/>");
			}
			else
				data << inRawData[nChar];
			break;
		case '\n':
			if (bConvertCR)
				data << _T("<br/>");
			else
				data << inRawData[nChar];
			break;
		default:
			data << inRawData[nChar];
			break;
		}
	}
	return data.str();
}

/////////////////////////////////////////////////////////////////////////////

tstring ARBVersion::str() const
{
	otstringstream buffer;
	buffer << Major() << _T(".") << Minor();
	return buffer.str();
}

/////////////////////////////////////////////////////////////////////////////

static struct Q2Enum
{
	TCHAR const* pQ;
	ARB_Q::eQ q;
} const sc_Qs[] = {
	{ARBQ_TYPE_NA, ARB_Q::eQ_NA},
	{ARBQ_TYPE_Q,  ARB_Q::eQ_Q},
	{ARBQ_TYPE_NQ, ARB_Q::eQ_NQ},
	{ARBQ_TYPE_E,  ARB_Q::eQ_E},
	{ARBQ_TYPE_SQ, ARB_Q::eQ_SuperQ}
}; ///< This is a list of the various types of "Q"s we support.
static int const sc_nQs = sizeof(sc_Qs) / sizeof(sc_Qs[0]);


tstring ARB_Q::GetValidTypes()
{
	tstring types;
	for (int i = 0; i < sc_nQs; ++i)
	{
		if (0 < i)
			types += _T(", ");
		types += sc_Qs[i].pQ;
	}
	return types;
}


void ARB_Q::GetValidTypes(std::vector<tstring>& outTypes)
{
	outTypes.clear();
	for (int i = 0; i < sc_nQs; ++i)
	{
		outTypes.push_back(sc_Qs[i].pQ);
	}
}


int ARB_Q::GetNumValidTypes()
{
	return sc_nQs;
}


ARB_Q ARB_Q::GetValidType(int inIndex)
{
	ARB_Q q;
	if (0 <= inIndex && inIndex < sc_nQs)
	{
		q.m_Q = sc_Qs[inIndex].q;
	}
	return q;
}


tstring ARB_Q::str() const
{
	tstring s(_T("?"));
	for (int i = 0; i < sc_nQs; ++i)
	{
		if (sc_Qs[i].q == m_Q)
		{
			s = sc_Qs[i].pQ;
			break;
		}
	}
	return s;
}


bool ARB_Q::Load(
		tstring const& inAttrib,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	for (int i = 0; i < sc_nQs; ++i)
	{
		if (inAttrib == sc_Qs[i].pQ)
		{
			m_Q = sc_Qs[i].q;
			return true;
		}
	}
	// Any Q that is not recognized is changed into a "NA".
	m_Q = eQ_NA;
	return false;
}


bool ARB_Q::Save(
		ElementNodePtr ioTree,
		TCHAR const* const inAttribName) const
{
	// If, somehow, m_Q is set to a value we don't understand,
	// it will be written as "NA".
	assert(inAttribName != NULL);
	bool bOk = false;
	tstring q(_T("NA"));
	for (int i = 0; i < sc_nQs; ++i)
	{
		if (m_Q == sc_Qs[i].q)
		{
			q = sc_Qs[i].pQ;
			bOk = true;
		}
	}
	ioTree->AddAttrib(inAttribName, q);
	return bOk;
}

/////////////////////////////////////////////////////////////////////////////

// Trailing zeros are trimmed unless inPrec=2.
// Then they are only trimmed if all zero (and inPrec=2).
tstring ARBDouble::str(
		double inValue,
		int inPrec)
{
	otstringstream str;
	if (0 < inPrec)
		str.precision(inPrec);
	str << std::fixed << inValue;
	tstring retVal = str.str();
	tstring::size_type pos = retVal.find('.');
	if (tstring::npos != pos)
	{
		// Strip trailing zeros iff they are all 0.
		if (2 == inPrec)
		{
			if (retVal.substr(pos) == _T(".00"))
			{
				// Input is ".00", so simplify
				if (0 == pos)
					retVal = _T("0");
				// Strip the ".00".
				else
					retVal = retVal.substr(0, pos);
			}
		}
		// Strip all trailing 0s.
		else
		{
			size_t len = retVal.length();
			size_t oldLen = len;
			while (0 < len && retVal[len-1] == '0')
				--len;
			if (0 < len && retVal[len-1] == '.')
				--len;
			if (len != oldLen)
				retVal = retVal.substr(0, len);
		}
	}
	return retVal;
}


// http://groups.google.com/group/comp.lang.c++.moderated/msg/518274ddc6fb8541?hl=en&
bool ARBDouble::equal(
		double const& inVal1,
		double const& inVal2,
		double inPrec)
{
	int mag1, mag2;
	frexp(inVal1, &mag1);
	frexp(inVal2, &mag2);
	if (mag1 != mag2)
		return false;

	double epsilon = ldexp(inPrec, mag1);
	double diff = abs(inVal1 - inVal2);
	return diff <= epsilon;
}
