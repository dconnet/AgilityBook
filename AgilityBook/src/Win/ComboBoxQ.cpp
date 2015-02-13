/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Special comboboxes
 * @author David Connet
 *
 * Revision History
 * 2015-02-13 Added Unknown Q type.
 * 2014-12-31 Changed pixels to dialog units.
 * 2012-06-16 Do not enable autocomplete on readonly combos.
 * 2012-05-07 Added autocompletion.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-20 Ported to wxWidgets.
 * 2008-01-05 Added CVenueComboBox
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2006-02-12 Created
 */

#include "stdafx.h"
#include "ComboBoxQ.h"

#include "ARB/ARBDogReferenceRun.h"
#include "ARB/ARBDogRun.h"
#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


// The combobox for the Q on a Mac is just too small.
#ifdef __WXMAC__
#define DEF_Q_WIDTH	50
#else
#define DEF_Q_WIDTH	30
#endif

IMPLEMENT_CLASS(CQualifyingComboBox, wxChoice)

/////////////////////////////////////////////////////////////////////////////

class CQualifyingComboData : public wxClientData
{
public:
	CQualifyingComboData(ARB_Q q) : m_Q(q) {}
	ARB_Q m_Q;
};


CQualifyingComboBox::CQualifyingComboBox(
		wxWindow* parent,
		ARBDogReferenceRunPtr refRun,
		wxValidator const& validator)
	: wxChoice()
	, m_refRun(refRun)
	, m_Run()
{
	wxChoice::Create(parent, wxID_ANY,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(parent, DEF_Q_WIDTH), -1),
		0, nullptr, 0, validator);
	ResetContent(ARBConfigScoringPtr());
}


CQualifyingComboBox::CQualifyingComboBox(
		wxWindow* parent,
		ARBDogRunPtr run,
		wxValidator const& validator)
	: wxChoice()
	, m_refRun()
	, m_Run(run)
{
	wxChoice::Create(parent, wxID_ANY,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(parent, DEF_Q_WIDTH), -1),
		0, nullptr, 0, validator);
	ResetContent(ARBConfigScoringPtr());
}


CQualifyingComboBox::CQualifyingComboBox(
		wxWindow* parent,
		ARBDogRunPtr run,
		ARBConfigScoringPtr scoring,
		wxValidator const& validator)
	: wxChoice()
	, m_refRun()
	, m_Run(run)
{
	wxChoice::Create(parent, wxID_ANY,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(parent, DEF_Q_WIDTH), -1),
		0, nullptr, 0, validator);
	ResetContent(scoring);
}


void CQualifyingComboBox::ResetContent(ARBConfigScoringPtr scoring)
{
	Clear();
	bool bHasTitling = true;
	if (scoring)
		bHasTitling = (0 < scoring->GetTitlePoints().size() || 0 < scoring->GetLifetimePoints().size());
	ARB_Q curQ = ARB_Q::eQ_NA;
	if (m_refRun)
		curQ = m_refRun->GetQ();
	else if (m_Run)
		curQ = m_Run->GetQ();

	int nQs = ARB_Q::GetNumValidTypes();
	for (int index = 0; index < nQs; ++index)
	{
		ARB_Q q = ARB_Q::GetValidType(index);
		if (scoring && ARB_Q::eQ_SuperQ == q && !scoring->HasSuperQ())
			continue;
		// Allow non-titling runs to only have certain types.
		// 0 is special - it's the Unknown case.
		if (0 < index && !bHasTitling && !q.AllowForNonTitling())
			continue;
		int idx = Append(StringUtil::stringWX(q.str()));
		SetClientObject(idx, new CQualifyingComboData(q));
		if (curQ == q)
			SetSelection(idx);
	}
	Enable(0 < GetCount());
}


ARB_Q CQualifyingComboBox::GetQ(int index) const
{
	wxClientData* pData = GetClientObject(index);
	if (pData)
		return dynamic_cast<CQualifyingComboData*>(pData)->m_Q;
	return ARB_Q::eQ_NA;
}
