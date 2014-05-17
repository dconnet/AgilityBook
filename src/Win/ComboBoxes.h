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
 * 2012-05-07 Added CAutoFillComboBox
 * 2009-02-20 Ported to wxWidgets.
 * 2008-01-05 Added CVenueComboBox
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2006-02-12 Created
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
			wxString const choices[] = nullptr,
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
