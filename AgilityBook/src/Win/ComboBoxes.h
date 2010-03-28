#pragma once

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
 * @li 2009-02-20 DRC Ported to wxWidgets.
 * @li 2008-01-05 DRC Added CVenueComboBox
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2006-02-12 DRC Created
 */

#include "ARBTypes.h"


/// Combobox specifically for listing venues
class CVenueComboBox : public wxComboBox
{
DECLARE_CLASS(CVenueComboBox)
public:
	CVenueComboBox(
			wxWindow* parent,
			ARBConfigVenueList const& inVenues,
			wxString const& inSelectVenue,
			bool useLongName = false,
			wxValidator const& validator = wxDefaultValidator,
			bool bEditable = false);

	ARBConfigVenuePtr GetVenue(int index) const;
};


/// Combobox for Qs
class CQualifyingComboBox : public wxChoice
{
DECLARE_CLASS(CQualifyingComboBox)
public:
	CQualifyingComboBox(
			wxWindow* parent,
			ARBDogReferenceRunPtr refRun,
			wxValidator const& validator = wxDefaultValidator);
	CQualifyingComboBox(
			wxWindow* parent,
			ARBDogRunPtr run,
			wxValidator const& validator = wxDefaultValidator);
	CQualifyingComboBox(
			wxWindow* parent,
			ARBDogRunPtr run,
			ARBConfigScoringPtr scoring,
			wxValidator const& validator = wxDefaultValidator);

	void ResetContent(ARBConfigScoringPtr scoring);
	ARB_Q GetQ(int index) const;

private:
	ARBDogReferenceRunPtr m_refRun;
	ARBDogRunPtr m_Run;
};
