/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "StdAfx.h"
#include "ARBTypes.h"
#include <time.h>

#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

std::string ARBVersion::ToString() const
{
	char buffer[100];
	sprintf(buffer, "%hd.%hd", Major(), Minor());
	return buffer;
}

/////////////////////////////////////////////////////////////////////////////

static const struct Q2Enum
{
	const char* pQ;
	ARB_Q::eQ q;
} sc_Qs[] = {
	{"NA", ARB_Q::eQ_NA},
	{"Q",  ARB_Q::eQ_Q},
	{"NQ", ARB_Q::eQ_NQ},
	{"E",  ARB_Q::eQ_E},
	{"SQ", ARB_Q::eQ_SuperQ}
}; ///< This is a list of the various types of "Q"s we support.
static const sc_nQs = sizeof(sc_Qs) / sizeof(sc_Qs[0]);

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
	const std::string& inAttrib,
	const ARBVersion& inVersion,
	std::string& ioErrMsg)
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

bool ARB_Q::Save(CElement& ioTree, const char* const inAttribName) const
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

std::string ARBDouble::str() const
{
	// It's a little odd to do it this way,
	// but we'll always get consistent output!
	CElement element;
	element.AddAttrib("temp", m_Val, m_Prec);
	std::string val;
	if (CElement::eFound != element.GetAttrib("temp", val))
		val = "?";
	return val;
}

bool ARBDouble::Load(
	const std::string& inAttrib,
	const ARBVersion& inVersion,
	std::string& ioErrMsg)
{
	if (0 < inAttrib.length())
		m_Val = strtod(inAttrib.c_str(), NULL);
	else
		m_Val = 0.0;
	return true;
}

bool ARBDouble::Save(CElement& ioTree, const char* const inAttribName) const
{
	ioTree.AddAttrib(inAttribName, m_Val, m_Prec);
	return true;
}
