/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgInfoNoteEdit class
 * @author David Connet
 *
 * Note: An item can go from NotInUse (with an entry in ARBInfoItemList) to
 * InUse and no entry in ARBInfoItemList simply by a club/judge/location being
 * added that uses the entered item. When saved, that item will no longer be
 * stored in the ARB file (because it's in use now). If the item has a comment
 * or is set to not-visible, it will remain in the ARB file.
 *
 * Revision History
 * 2021-09-19 Created
 */

#include "stdafx.h"
#include "DlgInfoNoteEdit.h"

#include "AgilityBook.h"
#include "DlgInfoNote.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/Validators.h"
#include "LibARBWin/Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CDlgInfoNoteEdit::CDlgInfoNoteEdit(
	std::wstring const& inName,
	std::wstring const& inComment,
	bool isVisible,
	CDlgInfoNote* parent)
	: wxDialog()
	, m_parent(parent)
	, m_OrigName(inName)
	, m_Name(inName)
	, m_Comment(inComment)
	, m_isVisible(isVisible)
{
	Create(
		parent,
		wxID_ANY,
		parent->GetCaption(),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	CTextCtrl* ctrlName
		= new CTextCtrl(this, wxID_ANY, m_Name, wxDefaultPosition, wxDefaultSize, 0, CTrimValidator(&m_Name));
	ctrlName->SetHelpText(_("HIDC_INFONOTE"));
	ctrlName->SetToolTip(_("HIDC_INFONOTE"));

	auto ctrlVisible = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_INFONOTE_VISIBLE"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_isVisible));
	ctrlVisible->SetHelpText(_("HIDC_INFONOTE_VISIBLE"));
	ctrlVisible->SetToolTip(_("HIDC_INFONOTE_VISIBLE"));

	m_ctrlNotes = new CSpellCheckCtrl(
		this,
		wxID_ANY,
		m_Comment,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(180, 60)),
		wxTE_MULTILINE,
		wxGenericValidator(&m_Comment));
	m_ctrlNotes->SetHelpText(_("HIDC_INFONOTE_COMMENTS"));
	m_ctrlNotes->SetToolTip(_("HIDC_INFONOTE_COMMENTS"));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(ctrlName, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));
	bSizer->Add(ctrlVisible, 0, wxEXPAND | wxLEFT | wxRIGHT, wxDLG_UNIT_X(this, 5));
	bSizer->Add(m_ctrlNotes, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgInfoNoteEdit::OnOk, this, wxID_OK);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz(GetSize());
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgInfoNoteEdit, ctrlName)
}


void CDlgInfoNoteEdit::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	if (m_OrigName != m_Name)
	{
		if (m_parent->NameExists(StringUtil::stringW(m_Name)))
		{
			wxMessageBox(_("IDS_NAME_IN_USE"));
			return;
		}
	}

	EndDialog(wxID_OK);
}
