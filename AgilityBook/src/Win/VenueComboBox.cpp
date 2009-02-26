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
 * @brief Make inserting items in a listbox easier.
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-20 DRC Ported to wxWidgets.
 * @li 2008-01-05 DRC Added CVenueComboBox
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2006-02-12 DRC Created
 */

#include "stdafx.h"
#include "VenueComboBox.h"

#include "ARBConfigVenue.h"


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
		wxValidator const& validator)
	: wxChoice()
{
	wxChoice::Create(parent, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		0, NULL,
		0, validator);

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
		if (!inSelectVenue.empty() && pVenue->GetName() == inSelectVenue)
			SetSelection(index);
	}
}


ARBConfigVenuePtr CVenueComboBox::GetVenue(int index) const
{
	wxClientData* pData = GetClientObject(index);
	if (pData)
		return dynamic_cast<CVenueComboData*>(pData)->m_Venue;
	return ARBConfigVenuePtr();
}
