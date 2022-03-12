#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Generic multiline edit dialog
 * @author David Connet
 *
 * Revision History
 * 2022-03-11 Moved from Win to LibARBWin, tweaked arguments
 * 2009-02-09 Ported to wxWidgets.
 */

#include "LibwxARBWin.h"

class CTextCtrl;


class ARBWIN_API CDlgMessage : public wxDialog
{
public:
	CDlgMessage(
		wxString const& msg,
		wxString caption = wxString(),
		wxWindow* pParent = nullptr,
		// This is the size of the text control in Dialog Units
		wxSize szDlgUnits = wxSize(260, 160));

	void SetMessage(wxString const& msg);

	bool LoadFile(wxString const& filename);

private:
	CTextCtrl* m_textCtrl;
};
