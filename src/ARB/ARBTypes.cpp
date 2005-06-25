/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @li 2005-06-25 DRC Removed ARBDouble.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "StdAfx.h"
#include "ARBTypes.h"
#include <sstream>
#include <time.h>

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

std::string ARBVersion::str() const
{
	std::ostringstream buffer;
	buffer << Major() << "." << Minor();
	return buffer.str();
}

/////////////////////////////////////////////////////////////////////////////

static struct Q2Enum
{
	char const* pQ;
	ARB_Q::eQ q;
} const sc_Qs[] = {
	{ARBQ_TYPE_NA, ARB_Q::eQ_NA},
	{ARBQ_TYPE_Q,  ARB_Q::eQ_Q},
	{ARBQ_TYPE_NQ, ARB_Q::eQ_NQ},
	{ARBQ_TYPE_E,  ARB_Q::eQ_E},
	{ARBQ_TYPE_SQ, ARB_Q::eQ_SuperQ}
}; ///< This is a list of the various types of "Q"s we support.
static int const sc_nQs = sizeof(sc_Qs) / sizeof(sc_Qs[0]);

std::string ARB_Q::GetValidTypes()
{
	std::string types;
	for (int i = 0; i < sc_nQs; ++i)
	{
		if (0 < i)
			types += ", ";
		types += sc_Qs[i].pQ;
	}
	return types;
}

void ARB_Q::GetValidTypes(std::vector<std::string>& outTypes)
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

std::string ARB_Q::str() const
{
	std::string s("?");
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
	std::string const& inAttrib,
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

bool ARB_Q::Save(Element& ioTree, char const* const inAttribName) const
{
	// If, somehow, m_Q is set to a value we don't understand,
	// it will be written as "NA".
	ASSERT(inAttribName != NULL);
	bool bOk = false;
	std::string q("NA");
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
std::string ARBDouble::str(double inValue, int inPrec)
{
	std::ostringstream str;
	if (0 < inPrec)
		str.precision(inPrec);
	str << std::fixed << inValue;
	std::string retVal = str.str();
	std::string::size_type pos = retVal.find('.');
	if (std::string::npos != pos)
	{
		// Strip trailing zeros iff they are all 0.
		if (2 == inPrec)
		{
			if (retVal.substr(pos) == ".00")
			{
				// Input is ".00", so simplify
				if (0 == pos)
					retVal = "0";
				// Strip the ".00".
				else
					retVal = retVal.substr(0, pos);
			}
		}
		// Strip all trailing 0s.
		else
		{
			pos = retVal.find_last_not_of('0');
			if (std::string::npos != pos)
			{
				if (retVal[pos-1] == '.')
					--pos;
			}
			else
			{
				if (retVal[retVal.length()-1] == '.')
					pos = retVal.length() - 1;
			}
			if (std::string::npos != pos)
				retVal = retVal.substr(0, pos);
		}
	}
	return retVal;
}
