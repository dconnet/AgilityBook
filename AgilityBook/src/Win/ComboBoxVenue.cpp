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
 * 2012-06-16 Do not enable autocomplete on readonly combos.
 * 2012-05-07 Added autocompletion.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-20 Ported to wxWidgets.
 * 2008-01-05 Added CVenueComboBox
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2006-02-12 Created
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

	wxArrayString choices;
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
		choices.Add(wxName);
		SetClientObject(index, new CVenueComboData(pVenue));
		if (!inSelectVenue.empty() && wxShortName == inSelectVenue)
			SetSelection(index);
	}
	if (bEditable)
		AutoComplete(choices);
}


ARBConfigVenuePtr CVenueComboBox::GetVenue(int index) const
{
	CVenueComboData* pData = dynamic_cast<CVenueComboData*>(GetClientObject(index));
	if (pData)
		return pData->m_Venue;
	return ARBConfigVenuePtr();
}
