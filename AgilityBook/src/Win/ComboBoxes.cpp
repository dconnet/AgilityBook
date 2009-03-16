/*
 * Copyright © 2006-2009 David Connet. All Rights Reserved.
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
 * @brief Special comboboxes
 * @author David Connet
 *
 * Revision History
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
		int index;
		if (useLongName)
			index = Append(pVenue->GetLongName().c_str());
		else
			index = Append(pVenue->GetName().c_str());
		SetClientObject(index, new CVenueComboData(pVenue));
		if (!inSelectVenue.empty() && pVenue->GetName() == inSelectVenue.c_str())
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
		wxDefaultPosition, wxSize(50, -1),
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
		wxDefaultPosition, wxSize(50, -1),
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
		wxDefaultPosition, wxSize(50, -1),
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
		// Allow non-titling runs to only have 'NA' and 'E'
		if (!bHasTitling && !(ARB_Q::eQ_E == q || ARB_Q::eQ_NA == q))
			continue;
		int idx = Append(q.str().c_str());
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
