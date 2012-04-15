/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Special comboboxes
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-20 DRC Ported to wxWidgets.
 * @li 2008-01-05 DRC Added CVenueComboBox
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2006-02-12 DRC Created
 */

#include "stdafx.h"
#include "ComboBoxes.h"

#include "ARBConfigVenue.h"
#include "ARBDogReferenceRun.h"
#include "ARBDogRun.h"
#include "ARBString.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


// The combobox for the Q on a Mac is just too small.
#ifdef __WXMAC__
#define DEF_Q_WIDTH	70
#else
#define DEF_Q_WIDTH	50
#endif

IMPLEMENT_CLASS(CVenueComboBox, wxComboBox)
IMPLEMENT_CLASS(CQualifyingComboBox, wxChoice)

/////////////////////////////////////////////////////////////////////////////

class CVenueComboData : public wxClientData
{
public:
	CVenueComboData(ARBConfigVenuePtr venue) : m_Venue(venue) {}
	ARBConfigVenuePtr m_Venue;
};


CVenueComboBox::CVenueComboBox(
		wxWindow* parent,
		ARBConfigVenueList const& inVenues,
		wxString const& inSelectVenue,
		bool useLongName,
		wxValidator const& validator,
		bool bEditable)
	: wxComboBox()
{
	wxComboBox::Create(parent, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, NULL,
		wxCB_DROPDOWN | (bEditable ? 0 : wxCB_READONLY) | wxCB_SORT,
		validator);

	for (ARBConfigVenueList::const_iterator iterVenue = inVenues.begin();
		iterVenue != inVenues.end();
		++iterVenue)
	{
		ARBConfigVenuePtr pVenue = (*iterVenue);
		wxString wxName = StringUtil::stringWX(pVenue->GetName());
		int index;
		if (useLongName)
			index = Append(StringUtil::stringWX(pVenue->GetLongName()));
		else
			index = Append(wxName);
		SetClientObject(index, new CVenueComboData(pVenue));
		if (!inSelectVenue.empty() && wxName == inSelectVenue)
			SetSelection(index);
	}
}


ARBConfigVenuePtr CVenueComboBox::GetVenue(int index) const
{
	CVenueComboData* pData = dynamic_cast<CVenueComboData*>(GetClientObject(index));
	if (pData)
		return pData->m_Venue;
	return ARBConfigVenuePtr();
}

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
		wxDefaultPosition, wxSize(DEF_Q_WIDTH, -1),
		0, NULL, 0, validator);
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
		wxDefaultPosition, wxSize(DEF_Q_WIDTH, -1),
		0, NULL, 0, validator);
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
		wxDefaultPosition, wxSize(DEF_Q_WIDTH, -1),
		0, NULL, 0, validator);
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
		if (!bHasTitling && !q.AllowForNonTitling())
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
