/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Message box with a details button.
 * @author David Connet
 *
 * Revision History
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-02-25 Ported to wxWidgets.
 * 2004-09-11 Created
 */

#include "stdafx.h"
#include "DlgMessageBox.h"

#include <wx/artprov.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

IMessageBoxCallback::~IMessageBoxCallback()
{
}

/////////////////////////////////////////////////////////////////////////////

class CDlgMessageBox : public wxDialog
{
public:
	CDlgMessageBox(
			wxString const& inText,
			long inFlags,
			IMessageBoxCallback* inCallback,
			wxWindow* inParent);

private:
	IMessageBoxCallback* m_Callback;

	void OnDetails(wxCommandEvent& evt);
};


CDlgMessageBox::CDlgMessageBox(
		wxString const& inText,
		long inFlags,
		IMessageBoxCallback* inCallback,
		wxWindow* inParent)
	: wxDialog()
	, m_Callback(inCallback)
{
	Create(inParent, wxID_ANY, _("Agility Record Book"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	wxStaticBitmap* ctrlIcon = NULL;
	if (inFlags & (wxICON_EXCLAMATION | wxICON_HAND | wxICON_ERROR
		| wxICON_QUESTION | wxICON_INFORMATION))
	{
		wxIcon icon;
		if (inFlags & wxICON_EXCLAMATION)
			icon = wxArtProvider::GetIcon(wxART_WARNING);
		else if (inFlags & (wxICON_HAND | wxICON_ERROR))
			icon = wxArtProvider::GetIcon(wxART_ERROR);
		else if (inFlags & wxICON_QUESTION)
			icon = wxArtProvider::GetIcon(wxART_QUESTION);
		else if (inFlags & wxICON_INFORMATION)
			icon = wxArtProvider::GetIcon(wxART_INFORMATION);
		ctrlIcon = new wxStaticBitmap(this, wxID_ANY, icon,
			wxDefaultPosition, wxDefaultSize, 0);
	}

	wxStaticText* ctrlText = new wxStaticText(this, wxID_ANY, inText,
		wxDefaultPosition, wxDefaultSize, 0);
	ctrlText->Wrap(-1);

	wxButton* ctrlDetails = NULL;
	if (inCallback)
	{
		ctrlDetails = new wxButton(this, wxID_ANY, _("IDC_MESSAGE_BOX_DETAILS"),
			wxDefaultPosition, wxDefaultSize, 0);
		BIND_OR_CONNECT_CTRL(ctrlDetails, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgMessageBox::OnDetails);
	}

	long btnFlags = inFlags & (wxOK | wxCANCEL | wxYES | wxNO | wxHELP | wxNO_DEFAULT);

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerText = new wxBoxSizer(wxHORIZONTAL);
	if (ctrlIcon)
		sizerText->Add(ctrlIcon, 0, wxALL, 5);
	sizerText->Add(ctrlText, 1, wxALL|wxEXPAND, 5);

	bSizer->Add(sizerText, 1, wxEXPAND, 0);

	if (ctrlDetails)
		bSizer->Add(ctrlDetails, 0, wxALIGN_RIGHT|wxALL, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(btnFlags);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	CenterOnParent();
}


void CDlgMessageBox::OnDetails(wxCommandEvent& evt)
{
	if (m_Callback)
		m_Callback->OnDetails(this);
}

/////////////////////////////////////////////////////////////////////////////

int AfxMessageBox2(
		wxString const& inText,
		long inFlags,
		IMessageBoxCallback* inCallback,
		wxWindow* inParent)
{
	CDlgMessageBox dlg(inText, inFlags, inCallback, inParent);
	return dlg.ShowModal();
}
