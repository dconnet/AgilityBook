#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Autodetect URLs and open a window when clicked.
 * @author David Connet
 *
 * Revision History
 * 2018-10-11 Moved to Win LibARBWin
 * 2009-02-19 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-01-30 Created
 */

#include "LibwxARBWin.h"
#include "Widgets.h"
class wxTextUrlEvent;


class ARBWIN_API CRichEditCtrl2 : public CTextCtrl
{
public:
	CRichEditCtrl2(
		wxWindow* parent,
		wxWindowID id,
		const wxString& value = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		bool bReadOnly = true); // Initial state of readonly

	void SetEditable(bool editable) override;

protected:
	void OnUrl(wxTextUrlEvent& evt);
};
