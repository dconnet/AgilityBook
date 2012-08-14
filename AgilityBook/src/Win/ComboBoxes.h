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
 * @li 2012-05-07 DRC Added CAutoFillComboBox
 * @li 2009-02-20 DRC Ported to wxWidgets.
 * @li 2008-01-05 DRC Added CVenueComboBox
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2006-02-12 DRC Created
 */

#include "ARB_Q.h"
#include "ARBTypes.h"

/// Enable autofill (wx2.9+) API on wx2.8.
class CAutoFillComboBox : public wxComboBox
{
public:
	CAutoFillComboBox(
			wxWindow* parent,
			wxWindowID id,
			wxString const& value = wxEmptyString,
			wxPoint const& pos = wxDefaultPosition,
			wxSize const& size = wxDefaultSize,
			int n = 0,
			wxString const choices[] = NULL,
			long style = 0,
			wxValidator const& validator = wxDefaultValidator,
			wxString const& name = wxComboBoxNameStr)
		: wxComboBox(parent, id, value, pos, size, n, choices, style, validator, name)
	{
	}

	CAutoFillComboBox(
			wxWindow * parent,
			wxWindowID id,
			wxString const& value,
			wxPoint const& pos,
			wxSize const& size,
			wxArrayString const& choices,
			long style = 0,
			wxValidator const& validator = wxDefaultValidator,
			wxString const& name = wxComboBoxNameStr)
		: wxComboBox(parent, id, value, pos, size, choices, style, validator, name)
	{
	}

#if !wxCHECK_VERSION(2, 9, 4)
	bool AutoComplete(wxArrayString const& choices)
	{
		return false;
	}
#endif
};


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
