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
 * 2015-05-05 Reorder "NA" lower in list.
 * 2015-03-15 Fixed Unknown-Q usage.
 * 2015-02-13 Added Unknown state.
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


namespace dconSoft
{
using namespace ARBCommon;
namespace ARB
{

namespace
{
constexpr struct Q2Enum
{
	wchar_t const* pQ;    ///< Actual text in file
	Q q;                  ///< Enum type
	wchar_t const* trans; ///< Translation
} const sc_Qs[] = {
	// clang-format off
	{nullptr,          Q::UNK,    nullptr},
	{ATTRIB_QTYPE_SQ,  Q::SuperQ, arbT("IDS_QTYPE_SQ")},
	{ATTRIB_QTYPE_Q,   Q::Q,      arbT("IDS_QTYPE_Q")},
	{ATTRIB_QTYPE_NQ,  Q::NQ,     arbT("IDS_QTYPE_NQ")},
	{ATTRIB_QTYPE_E,   Q::E,      arbT("IDS_QTYPE_E")},
	{ATTRIB_QTYPE_FEO, Q::FEO,    arbT("IDS_QTYPE_FEO")},
	{ATTRIB_QTYPE_DNR, Q::DNR,    arbT("IDS_QTYPE_DNR")},
	{ATTRIB_QTYPE_NA,  Q::NA,     arbT("IDS_QTYPE_NA")},
	// clang-format on
}; ///< This is a list of the various types of "Q"s we support.
constexpr int sc_nQs = sizeof(sc_Qs) / sizeof(sc_Qs[0]);
} // namespace


wxString ARB_Q::GetValidTypes()
{
	wxString types;
	bool bComma = false;
	for (int i = 0; i < sc_nQs; ++i)
	{
		if (sc_Qs[i].trans)
		{
			if (bComma)
				types << L", ";
			bComma = true;
			types << wxGetTranslation(sc_Qs[i].trans);
		}
	}
	return types;
}


void ARB_Q::GetValidTypes(std::vector<wxString>& outTypes)
{
	outTypes.clear();
	for (int i = 0; i < sc_nQs; ++i)
	{
		if (sc_Qs[i].trans)
			outTypes.push_back(wxGetTranslation(sc_Qs[i].trans));
		else
			outTypes.push_back(wxString());
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
	wxString s;
	for (int i = 0; i < sc_nQs; ++i)
	{
		if (sc_Qs[i].q == m_Q)
		{
			if (sc_Qs[i].trans)
				s = wxGetTranslation(sc_Qs[i].trans);
			else
				s = wxString();
			break;
		}
	}
	return s;
}


bool ARB_Q::Load(wxString const& inAttrib, ARBVersion const& inVersion, ARBErrorCallback& ioCallback)
{
	for (int i = 0; i < sc_nQs; ++i)
	{
		if (sc_Qs[i].pQ && inAttrib == sc_Qs[i].pQ)
		{
			m_Q = sc_Qs[i].q;
			return true;
		}
	}
	// Any Q that is not recognized is changed into unknown.
	m_Q = Q::UNK;
	return false;
}


bool ARB_Q::Save(ElementNodePtr const& ioTree, wchar_t const* const inAttribName) const
{
	assert(!!inAttribName);
	bool bOk = false;
	wxString q;
	for (int i = 0; i < sc_nQs; ++i)
	{
		if (m_Q == sc_Qs[i].q)
		{
			if (sc_Qs[i].pQ)
				q = sc_Qs[i].pQ;
			bOk = true;
		}
	}
	if (!q.empty())
		ioTree->AddAttrib(inAttribName, q);
	return bOk;
}

} // namespace ARB
} // namespace dconSoft
