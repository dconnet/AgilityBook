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

#if !wxCHECK_VERSION(3, 0, 0)
	bool AutoComplete(wxArrayString const& choices)
	{
		return false;
	}
#endif
};
