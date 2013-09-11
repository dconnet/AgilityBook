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
 * @li 2012-06-16 DRC Do not enable autocomplete on readonly combos.
 * @li 2012-05-07 DRC Added autocompletion.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-20 DRC Ported to wxWidgets.
 * @li 2008-01-05 DRC Added CVenueComboBox
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2006-02-12 DRC Created
 */

#include "stdafx.h"
#include "ComboBoxVenue.h"

#include "ARB/ARBConfigVenue.h"
#include "ARB/ARBDogReferenceRun.h"
#include "ARB/ARBDogRun.h"
#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


IMPLEMENT_CLASS(CVenueComboBox, wxComboBox)

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

#if wxCHECK_VERSION(3, 0, 0)
	wxArrayString choices;
#endif
	for (ARBConfigVenueList::const_iterator iterVenue = inVenues.begin();
		iterVenue != inVenues.end();
		++iterVenue)
	{
		ARBConfigVenuePtr pVenue = (*iterVenue);
		wxString wxShortName = StringUtil::stringWX(pVenue->GetName());
		wxString wxName(wxShortName);
		int index;
		if (useLongName)
			wxName = StringUtil::stringWX(pVenue->GetLongName());
		index = Append(wxName);
#if wxCHECK_VERSION(3, 0, 0)
		choices.Add(wxName);
#endif
		SetClientObject(index, new CVenueComboData(pVenue));
		if (!inSelectVenue.empty() && wxShortName == inSelectVenue)
			SetSelection(index);
	}
#if wxCHECK_VERSION(3, 0, 0)
	if (bEditable)
		AutoComplete(choices);
#endif
}


ARBConfigVenuePtr CVenueComboBox::GetVenue(int index) const
{
	CVenueComboData* pData = dynamic_cast<CVenueComboData*>(GetClientObject(index));
	if (pData)
		return pData->m_Venue;
	return ARBConfigVenuePtr();
}
