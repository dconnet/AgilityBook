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
 * @li 2012-04-10 DRC Based on wx-group thread, use std::string for internal use
 * @li 2009-11-24 DRC Optimize locale usage when reading/writing the ARB file.
 * @li 2009-09-28 DRC Fix abs() on Mac.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9.
 * @li 2008-06-29 DRC Moved string stuff to ARBString.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Removed ARBDouble.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "stdafx.h"
#include "ARBTypes.h"

#include "ARBLocalization.h"
#include "ARBString.h"
#include "ARBStructure.h"
#include "Element.h"
#include <iostream>
#include <math.h>
#include <sstream>
#include <time.h>
#include <wx/string.h>

#if defined(__WXWINDOWS__)
#include <wx/string.h>
#endif

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

std::wstring SanitizeStringForHTML(
		std::wstring const& inRawData,
		bool bConvertCR)
{
	std::wstring::size_type pos = inRawData.find_first_of(L"&<>");
	if (std::wstring::npos == pos && bConvertCR)
		pos = inRawData.find_first_of(L"\r\n");
	if (std::wstring::npos == pos)
		return inRawData;
	std::wostringstream data;
	for (size_t nChar = 0; nChar < inRawData.length(); ++nChar)
	{
		switch (inRawData[nChar])
		{
		case L'&':
			data << L"&amp;";
			break;
		case L'<':
			data << L"&lt;";
			break;
		case L'>':
			data << L"&gt;";
			break;
		case L'\r':
			if (bConvertCR)
			{
				if (nChar + 1 < inRawData.length() && '\n' == inRawData[nChar+1])
					continue;
				else
					data << L"<br/>";
			}
			else
				data << inRawData[nChar];
			break;
		case '\n':
			if (bConvertCR)
				data << L"<br/>";
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

std::wstring ARBVersion::str() const
{
	std::wostringstream buffer;
	buffer << Major() << L"." << Minor();
	return buffer.str();
}

/////////////////////////////////////////////////////////////////////////////

static struct Q2Enum
{
	wchar_t const* pQ;		///< Actual text in file
	ARB_Q::eQ q;			///< Enum type
	wchar_t const* trans;	///< Translation
} const sc_Qs[] =
{
	{ATTRIB_QTYPE_NA,  ARB_Q::eQ_NA,     arbT("IDS_QTYPE_NA")},
	{ATTRIB_QTYPE_Q,   ARB_Q::eQ_Q,      arbT("IDS_QTYPE_Q")},
	{ATTRIB_QTYPE_NQ,  ARB_Q::eQ_NQ,     arbT("IDS_QTYPE_NQ")},
	{ATTRIB_QTYPE_E,   ARB_Q::eQ_E,      arbT("IDS_QTYPE_E")},
	{ATTRIB_QTYPE_DNR, ARB_Q::eQ_DNR,    arbT("IDS_QTYPE_DNR")},
	{ATTRIB_QTYPE_SQ,  ARB_Q::eQ_SuperQ, arbT("IDS_QTYPE_SQ")}
}; ///< This is a list of the various types of "Q"s we support.
static int const sc_nQs = sizeof(sc_Qs) / sizeof(sc_Qs[0]);


std::wstring ARB_Q::GetValidTypes()
{
	std::wostringstream types;
	for (int i = 0; i < sc_nQs; ++i)
	{
		if (0 < i)
			types << L", ";
		types << wxGetTranslation(sc_Qs[i].trans).wx_str();
	}
	return types.str();
}


void ARB_Q::GetValidTypes(std::vector<std::wstring>& outTypes)
{
	outTypes.clear();
	for (int i = 0; i < sc_nQs; ++i)
	{
		outTypes.push_back(wxGetTranslation(sc_Qs[i].trans).wx_str());
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


std::wstring ARB_Q::str() const
{
	std::wstring s(L"?");
	for (int i = 0; i < sc_nQs; ++i)
	{
		if (sc_Qs[i].q == m_Q)
		{
			s = wxGetTranslation(sc_Qs[i].trans);
			break;
		}
	}
	return s;
}


bool ARB_Q::Load(
		std::wstring const& inAttrib,
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
		wchar_t const* const inAttribName) const
{
	// If, somehow, m_Q is set to a value we don't understand,
	// it will be written as "NA".
	assert(inAttribName != NULL);
	bool bOk = false;
	std::wstring q(L"NA");
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
std::wstring ARBDouble::ToString(
		double inValue,
		int inPrec,
		LocaleType eUseDefaultLocale)
{
#if !defined(__WXWINDOWS__)
	eUseDefaultLocale = eNone;
#endif
	std::wstring retVal;
	wchar_t pt = '.';
	if (eNone == eUseDefaultLocale)
	{
		// When we don't want locales, avoid the wx functions as they use them.
		std::wostringstream str;
		if (0 < inPrec)
			str.precision(inPrec);
		str << std::fixed << inValue;
		retVal = str.str();
	}
#if defined(__WXWINDOWS__)
	else
	{
		wxLocale* locale = NULL;
		if (eDefault == eUseDefaultLocale)
			locale = new wxLocale(wxLANGUAGE_DEFAULT, 0);
		wxString decimalPt = wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER);
		if (0 < decimalPt.length())
			pt = decimalPt.GetChar(0);
		wxString tmp;
		if (0 < inPrec)
			tmp = wxString::Format(wxT("%.*f"), inPrec, inValue);
		else
			tmp = wxString::Format(wxT("%g"), inValue);
		retVal = StringUtil::stringW(tmp);
		delete locale;
	}
#endif
	std::wstring::size_type pos = retVal.find(pt);
	if (std::wstring::npos != pos)
	{
		// Strip trailing zeros iff they are all 0.
		if (2 == inPrec)
		{
			std::wstring twoZeros;
			twoZeros = pt;
			twoZeros += L"00";
			if (retVal.substr(pos) == twoZeros)
			{
				// Input is ".00", so simplify
				if (0 == pos)
					retVal = L"0";
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
			if (0 < len && retVal[len-1] == pt)
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
#if defined(__WXMAC__)
	// On Mac, I'm getting 'int abs(int)'. Doesn't seem to be a double version.
	double diff = inVal1 - inVal2;
	if (0.0 > diff)
		diff *= -1.0;
#else
	double diff = abs(inVal1 - inVal2);
#endif
	return diff <= epsilon;
}
