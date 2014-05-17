/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Utility classes
 * @author David Connet
 *
 * Revision History
 * 2013-08-14 Moved out of ARBTypes.cpp
 */

#include "stdafx.h"
#include "ARB/ARB_Q.h"

#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

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
		types << StringUtil::GetTranslation(sc_Qs[i].trans);
	}
	return types.str();
}


void ARB_Q::GetValidTypes(std::vector<std::wstring>& outTypes)
{
	outTypes.clear();
	for (int i = 0; i < sc_nQs; ++i)
	{
		outTypes.push_back(StringUtil::GetTranslation(sc_Qs[i].trans));
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
			s = StringUtil::GetTranslation(sc_Qs[i].trans);
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
	assert(!!inAttribName);
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
