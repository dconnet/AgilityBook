/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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

#include "StdAfx.h"
#include "ARBTypes.h"
#include <float.h>
#include <math.h>
#include <time.h>

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBString ARBVersion::str() const
{
	ARBostringstream buffer;
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

ARBString ARB_Q::GetValidTypes()
{
	ARBString types;
	for (int i = 0; i < sc_nQs; ++i)
	{
		if (0 < i)
			types += _T(", ");
		types += sc_Qs[i].pQ;
	}
	return types;
}

void ARB_Q::GetValidTypes(std::vector<ARBString>& outTypes)
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

ARBString ARB_Q::str() const
{
	ARBString s(_T("?"));
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
		ARBString const& inAttrib,
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
		Element& ioTree,
		TCHAR const* const inAttribName) const
{
	// If, somehow, m_Q is set to a value we don't understand,
	// it will be written as "NA".
	ASSERT(inAttribName != NULL);
	bool bOk = false;
	ARBString q(_T("NA"));
	for (int i = 0; i < sc_nQs; ++i)
	{
		if (m_Q == sc_Qs[i].q)
		{
			q = sc_Qs[i].pQ;
			bOk = true;
		}
	}
	ioTree.AddAttrib(inAttribName, q);
	return bOk;
}

/////////////////////////////////////////////////////////////////////////////

// Trailing zeros are trimmed unless inPrec=2.
// Then they are only trimmed if all zero (and inPrec=2).
ARBString ARBDouble::str(
		double inValue,
		int inPrec)
{
	ARBostringstream str;
	if (0 < inPrec)
		str.precision(inPrec);
	str << std::fixed << inValue;
	ARBString retVal = str.str();
	ARBString::size_type pos = retVal.find('.');
	if (ARBString::npos != pos)
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

bool ARBDouble::equal(double const& inVal1, double const& inVal2)
{
	double diff = abs(inVal1 - inVal2);
	return diff <= DBL_EPSILON;
}
