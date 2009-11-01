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
#include <iostream>
#include <sstream>
#include <math.h>
#include <time.h>
#include <wx/log.h>

#include "ARBLocalization.h"
#include "ARBStructure.h"
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

wxString SanitizeStringForHTML(
		wxString const& inRawData,
		bool bConvertCR)
{
	wxString::size_type pos = inRawData.find_first_of(wxT("&<>"));
	if (wxString::npos == pos && bConvertCR)
		pos = inRawData.find_first_of(wxT("\r\n"));
	if (wxString::npos == pos)
		return inRawData;
	wxString data;
	for (size_t nChar = 0; nChar < inRawData.length(); ++nChar)
	{
#if wxCHECK_VERSION(2, 9, 0)
		switch (inRawData[nChar].GetValue())
#else
		switch (inRawData[nChar])
#endif
		{
		case wxT('&'):
			data << wxT("&amp;");
			break;
		case wxT('<'):
			data << wxT("&lt;");
			break;
		case wxT('>'):
			data << wxT("&gt;");
			break;
		case wxT('\r'):
			if (bConvertCR)
			{
				if (nChar + 1 < inRawData.length() && '\n' == inRawData[nChar+1])
					continue;
				else
					data << wxT("<br/>");
			}
			else
				data << inRawData[nChar];
			break;
		case '\n':
			if (bConvertCR)
				data << wxT("<br/>");
			else
				data << inRawData[nChar];
			break;
		default:
			data << inRawData[nChar];
			break;
		}
	}
	return data;
}

/////////////////////////////////////////////////////////////////////////////

wxString ARBVersion::str() const
{
	wxString buffer;
	buffer << Major() << wxT(".") << Minor();
	return buffer;
}

/////////////////////////////////////////////////////////////////////////////

static struct Q2Enum
{
	wxChar const* pQ;		///< Actual text in file
	ARB_Q::eQ q;			///< Enum type
	wxChar const* trans;	///< Translation
} const sc_Qs[] =
{
	{ATTRIB_QTYPE_NA,  ARB_Q::eQ_NA,     wxT("IDS_QTYPE_NA")},
	{ATTRIB_QTYPE_Q,   ARB_Q::eQ_Q,      wxT("IDS_QTYPE_Q")},
	{ATTRIB_QTYPE_NQ,  ARB_Q::eQ_NQ,     wxT("IDS_QTYPE_NQ")},
	{ATTRIB_QTYPE_E,   ARB_Q::eQ_E,      wxT("IDS_QTYPE_E")},
	{ATTRIB_QTYPE_DNR, ARB_Q::eQ_DNR,    wxT("IDS_QTYPE_DNR")},
	{ATTRIB_QTYPE_SQ,  ARB_Q::eQ_SuperQ, wxT("IDS_QTYPE_SQ")}
}; ///< This is a list of the various types of "Q"s we support.
static int const sc_nQs = sizeof(sc_Qs) / sizeof(sc_Qs[0]);


wxString ARB_Q::GetValidTypes()
{
	wxString types;
	for (int i = 0; i < sc_nQs; ++i)
	{
		if (0 < i)
			types += wxT(", ");
		types += wxGetTranslation(sc_Qs[i].trans);
	}
	return types;
}


void ARB_Q::GetValidTypes(std::vector<wxString>& outTypes)
{
	outTypes.clear();
	for (int i = 0; i < sc_nQs; ++i)
	{
		outTypes.push_back(wxGetTranslation(sc_Qs[i].trans));
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


wxString ARB_Q::str() const
{
	wxString s(wxT("?"));
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
		wxString const& inAttrib,
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
		wxChar const* const inAttribName) const
{
	// If, somehow, m_Q is set to a value we don't understand,
	// it will be written as "NA".
	assert(inAttribName != NULL);
	bool bOk = false;
	wxString q(wxT("NA"));
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
wxString ARBDouble::ToString(
		double inValue,
		int inPrec,
		LocaleType eUseDefaultLocale)
{
	wxString retVal;
	if (0 < inPrec)
		retVal = wxString::Format(wxT("%.*f"), inPrec, inValue);
	else
		retVal = wxString::Format(wxT("%g"), inValue);
#if wxCHECK_VERSION(2, 9, 0)
	wxUniChar pt = '.';
#else
	wxChar pt = '.';
#endif
	wxLocale* locale = NULL;
	if (eNone != eUseDefaultLocale)
	{
		if (eDefault == eUseDefaultLocale)
			locale = new wxLocale(wxLANGUAGE_DEFAULT);
		wxString decimalPt = wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER);
		if (0 < decimalPt.length())
			pt = decimalPt.GetChar(0);
	}
	wxString::size_type pos = retVal.find(pt);
	if (wxString::npos != pos)
	{
		// Strip trailing zeros iff they are all 0.
		if (2 == inPrec)
		{
			wxString twoZeros(pt);
			twoZeros += wxT("00");
			if (retVal.substr(pos) == twoZeros)
			{
				// Input is ".00", so simplify
				if (0 == pos)
					retVal = wxT("0");
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
	delete locale;
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
#ifdef __WXMAC__
	// On Mac, I'm getting 'int abs(int)'. Doesn't seem to be a double version.
	double diff = inVal1 - inVal2;
	if (0.0 > diff)
		diff *= -1.0;
#else
	double diff = abs(inVal1 - inVal2);
#endif
	return diff <= epsilon;
}
