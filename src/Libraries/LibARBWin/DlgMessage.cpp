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
 * 2014-12-31 Changed pixels to dialog units.
 * 2009-02-09 Ported to wxWidgets.
 */

#include "stdafx.h"
#include "LibARBWin/DlgMessage.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CDlgMessage::CDlgMessage(wxString const& msg, wxString caption, wxWindow* pParent, wxSize szDlgUnits)
	: wxDialog()
	, m_textCtrl(nullptr)
{
	if (caption.empty())
		caption = _("IDD_MESSAGE");
	Create(pParent, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	m_textCtrl = new CTextCtrl(
		this,
		wxID_ANY,
		msg,
		wxDefaultPosition,
		wxDLG_UNIT(this, szDlgUnits),
		wxTE_MULTILINE | wxTE_READONLY | wxTE_WORDWRAP);

	wxButton* btnClose = new wxButton(this, wxID_OK, _("IDC_MESSAGE_CLOSE"));
	btnClose->SetDefault();

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(m_textCtrl, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	bSizer->Add(btnClose, 0, wxALIGN_RIGHT | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	btnClose->SetFocus();
}


void CDlgMessage::SetMessage(wxString const& msg)
{
	m_textCtrl->SetLabel(msg);
}


bool CDlgMessage::LoadFile(wxString const& filename)
{
	if (!wxFileName::Exists(filename))
		return false;

	return m_textCtrl->LoadFile(filename);
}
