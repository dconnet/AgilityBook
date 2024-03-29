#pragma once

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
 * 2013-04-15 Separated from ComboBoxes.h
 */

#include "ARB/ARBTypes2.h"


namespace dconSoft
{


/// Combobox specifically for listing venues
class CVenueComboBox : public wxComboBox
{
	DECLARE_CLASS(CVenueComboBox)
public:
	CVenueComboBox(
		wxWindow* parent,
		ARB::ARBConfigVenueList const& inVenues,
		wxString const& inSelectVenue,
		bool useLongName = false,
		wxValidator const& validator = wxDefaultValidator,
		bool bEditable = false);

	ARB::ARBConfigVenuePtr GetVenue(int index) const;
};

} // namespace dconSoft
