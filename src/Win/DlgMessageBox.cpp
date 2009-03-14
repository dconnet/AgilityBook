/*
 * Copyright © 2004-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 *
 * @brief Message box with a details button.
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-25 DRC Ported to wxWidgets.
 * @li 2004-09-11 DRC Created
 */

#include "stdafx.h"
#include "DlgMessageBox.h"

#include <wx/artprov.h>

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
		ctrlDetails->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgMessageBox::OnDetails), NULL, this);
	}

	long btnFlags = inFlags & (wxOK | wxCANCEL | wxYES | wxNO | wxHELP | wxNO_DEFAULT);

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerText = new wxBoxSizer(wxHORIZONTAL);
	if (ctrlIcon)
		sizerText->Add(ctrlIcon, 0, wxALL, 5);
	sizerText->Add(ctrlText, 1, wxALL|wxEXPAND, 5);

	bSizer->Add(sizerText, 1, wxEXPAND, 5);

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
