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
 * 2014-12-31 Changed pixels to dialog units.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-02-25 Ported to wxWidgets.
 * 2004-09-11 Created
 */

#include "stdafx.h"
#include "DlgMessageBox.h"

#include "ImageHelper.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/Logger.h"
#include <wx/artprov.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBWin;

class CDlgMessageBox : public wxDialog
{
public:
	CDlgMessageBox(wxString const& inText, long inFlags, IMessageBoxCallback* inCallback, wxWindow* inParent);

private:
	IMessageBoxCallback* m_Callback;
	ARBWin::CStackLogger m_trace;

	void OnDetails(wxCommandEvent& evt);
};


CDlgMessageBox::CDlgMessageBox(
	wxString const& inText,
	long inFlags,
	IMessageBoxCallback* inCallback,
	wxWindow* inParent)
	: wxDialog()
	, m_Callback(inCallback)
	, m_trace("CDlgMessageBox")
{
	Create(
		inParent,
		wxID_ANY,
		_("Agility Record Book"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	wxStaticBitmap* ctrlIcon = nullptr;
	if (inFlags & (wxICON_EXCLAMATION | wxICON_HAND | wxICON_ERROR | wxICON_QUESTION | wxICON_INFORMATION))
	{
		wxIcon icon;
		if (inFlags & wxICON_EXCLAMATION)
			icon = ImageHelper::GetIcon(this, wxART_WARNING);
		else if (inFlags & (wxICON_HAND | wxICON_ERROR))
			icon = ImageHelper::GetIcon(this, wxART_ERROR);
		else if (inFlags & wxICON_QUESTION)
			icon = ImageHelper::GetIcon(this, wxART_QUESTION);
		else if (inFlags & wxICON_INFORMATION)
			icon = ImageHelper::GetIcon(this, wxART_INFORMATION);
		ctrlIcon = new wxStaticBitmap(this, wxID_ANY, icon, wxDefaultPosition, wxDefaultSize, 0);
	}

	wxStaticText* ctrlText = new wxStaticText(this, wxID_ANY, inText, wxDefaultPosition, wxDefaultSize, 0);
	ctrlText->Wrap(-1);

	wxButton* ctrlDetails = nullptr;
	if (inCallback)
	{
		ctrlDetails = new wxButton(this, wxID_ANY, _("IDC_MESSAGE_BOX_DETAILS"), wxDefaultPosition, wxDefaultSize, 0);
		ctrlDetails->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgMessageBox::OnDetails, this);
	}

	long btnFlags = inFlags & (wxOK | wxCANCEL | wxYES | wxNO | wxHELP | wxNO_DEFAULT);

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerText = new wxBoxSizer(wxHORIZONTAL);
	if (ctrlIcon)
		sizerText->Add(ctrlIcon, 0, wxRIGHT, padding.Controls());
	sizerText->Add(ctrlText, 1, wxEXPAND);

	bSizer->Add(sizerText, 1, wxEXPAND | wxALL, padding.Controls());

	if (ctrlDetails)
		bSizer->Add(ctrlDetails, 0, wxALIGN_RIGHT | wxLEFT | wxRIGHT | wxBOTTOM, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(btnFlags);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());

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

int AfxMessageBox2(wxString const& inText, long inFlags, IMessageBoxCallback* inCallback, wxWindow* inParent)
{
	CDlgMessageBox dlg(inText, inFlags, inCallback, inParent);
	return dlg.ShowModal();
}

} // namespace dconSoft
