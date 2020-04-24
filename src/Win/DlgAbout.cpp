/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgAbout class
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2016-11-29 Fix sizer assertions.
 * 2014-12-30 Changed pixels to dialog units.
 * 2012-09-23 Fix text wrapping on Mac (wx2.9).
 * 2012-07-13 Fix text wrapping on Mac.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2011-04-26 wx2.9.2 added native hyperlinks and generic support.
 * 2010-09-30 Allow 'space' to activate a hyperlink.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-12-14 Ported to wxWidgets.
 * 2006-12-10 Simplified download links.
 * 2005-02-08 Added another link.
 * 2004-07-20 This now takes a Doc ptr so the update feature works.
 * 2004-03-05 Moved versioninfo into new class, added new links/buttons.
 * 2004-03-02 Added %VERSION% keyword in hotlinks.
 * 2003-09-17 Added a 'check for updates' control.
 */

#include "stdafx.h"
#include "DlgAbout.h"

#include "AgilityBook.h"
#include "ImageHelper.h"
#include "VersionNumber.h"

#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CDlgAbout::CDlgAbout(CAgilityBookDoc* pDoc, wxWindow* pParent)
	: wxDialog()
	, m_pDoc(pDoc)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_ABOUTBOX"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	// Controls (these are done first to control tab order)

	wxStaticBitmap* icon = new wxStaticBitmap(
		this,
		wxID_ANY,
		CImageManager::Get()->GetIcon(ImageMgrApp, wxART_MESSAGE_BOX),
		wxDLG_UNIT(this, wxPoint(2, 1)),
		wxDefaultSize,
		0);

	wxStaticText* version = new wxStaticText(
		this,
		wxID_ANY,
		fmt::format(
			_("AboutVersion").wx_str(),
			ARB_VER_MAJOR,
			ARB_VER_MINOR,
			ARB_VER_DOT,
			ARB_VER_BUILD,
			_("Agility Record Book").wx_str()),
		wxDefaultPosition,
		wxDefaultSize,
		0);
	version->Wrap(wxDLG_UNIT_X(this, 250));

	wxHyperlinkCtrl* link1 = new wxHyperlinkCtrl(
		this,
		wxID_ANY,
		_("LinkGroupText"),
		_("LinkGroupUrl"),
		wxDefaultPosition,
		wxDefaultSize,
		wxHL_DEFAULT_STYLE);
	link1->SetToolTip(_("LinkGroupUrl"));
	link1->SetHelpText(_("LinkGroupUrl"));

	wxHyperlinkCtrl* link2 = new wxHyperlinkCtrl(
		this,
		wxID_ANY,
		_("LinkHelpText"),
		_("LinkHelpUrl"),
		wxDefaultPosition,
		wxDefaultSize,
		wxHL_DEFAULT_STYLE);
	link2->SetToolTip(_("LinkHelpUrl"));
	link2->SetHelpText(_("LinkHelpUrl"));
	link2->Bind(wxEVT_COMMAND_HYPERLINK, &CDlgAbout::OnHelpEmail, this);

	wxStaticText* usefulLinks = new wxStaticText(this, wxID_ANY, _("UsefulLinks"), wxDefaultPosition, wxDefaultSize, 0);
	usefulLinks->Wrap(-1);

	wxHyperlinkCtrl* link3 = new wxHyperlinkCtrl(
		this,
		wxID_ANY,
		_("LinkArbText"),
		_("LinkArbUrl"),
		wxDefaultPosition,
		wxDefaultSize,
		wxHL_DEFAULT_STYLE);
	link3->SetToolTip(_("LinkArbUrl"));
	link3->SetHelpText(_("LinkArbUrl"));

	wxHyperlinkCtrl* link4 = new wxHyperlinkCtrl(
		this,
		wxID_ANY,
		_("LinkArbDownloadText"),
		_("LinkArbDownloadUrl"),
		wxDefaultPosition,
		wxDefaultSize,
		wxHL_DEFAULT_STYLE);
	link4->SetToolTip(_("LinkArbDownloadUrl"));
	link4->SetHelpText(_("LinkArbDownloadUrl"));

	wxStaticText* credits = new wxStaticText(this, wxID_ANY, _("AboutCredits"), wxDefaultPosition, wxDefaultSize, 0);

	wxButton* updates = new wxButton(this, wxID_ANY, _("CheckForUpdates"), wxDefaultPosition, wxDefaultSize, 0);
	updates->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgAbout::OnCheckForUpdates, this);
	updates->SetHelpText(_("HIDC_ABOUT_UPDATE"));

	wxButton* ok = new wxButton(this, wxID_OK);
	ok->SetDefault();

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxHORIZONTAL);
	bSizer->Add(icon, 0, wxALIGN_LEFT | wxALIGN_TOP | wxALL, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerBox = new wxBoxSizer(wxVERTICAL);
	sizerBox->Add(version, 1, wxEXPAND, 0);

	wxBoxSizer* sizerLinksBox1 = new wxBoxSizer(wxHORIZONTAL);
	sizerLinksBox1->Add(wxDLG_UNIT_X(this, 10), 0, 0, wxEXPAND, 0);

	wxBoxSizer* sizerLinks1 = new wxBoxSizer(wxVERTICAL);
	sizerLinks1->Add(link1, 0, 0, 0);
	sizerLinks1->Add(link2, 0, wxTOP, wxDLG_UNIT_X(this, 1));

	sizerLinksBox1->Add(sizerLinks1, 1, wxEXPAND, 0);

	sizerBox->Add(sizerLinksBox1, 0, wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 1));
	sizerBox->Add(usefulLinks, 0, wxTOP, wxDLG_UNIT_X(this, 3));

	wxBoxSizer* sizerLinksBox2 = new wxBoxSizer(wxHORIZONTAL);
	sizerLinksBox2->Add(wxDLG_UNIT_X(this, 10), 0, 0, wxEXPAND, 0);

	wxBoxSizer* sizerLinks2 = new wxBoxSizer(wxVERTICAL);
	sizerLinks2->Add(link3, 0, 0, 0);
	sizerLinks2->Add(link4, 0, wxTOP, wxDLG_UNIT_X(this, 1));

	sizerLinksBox2->Add(sizerLinks2, 1, wxEXPAND, 0);

	sizerBox->Add(sizerLinksBox2, 0, wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 1));

	sizerBox->Add(credits, 0, wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerButtons = new wxBoxSizer(wxHORIZONTAL);
	sizerButtons->Add(updates, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerButtons->Add(0, 0, 1, wxEXPAND, 0);
	sizerButtons->Add(ok, 0, 0, 0);

	sizerBox->Add(sizerButtons, 0, wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 5));

	bSizer->Add(sizerBox, 1, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	ok->SetFocus();
}


void CDlgAbout::OnHelpEmail(wxHyperlinkEvent& evt)
{
	wxString url = evt.GetURL();
	std::wstring str = fmt::format(L"{}.{}.{}", ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT);
	url.Replace(L"%VERSION%", str);
	wxLaunchDefaultBrowser(url);
}


void CDlgAbout::OnCheckForUpdates(wxCommandEvent& evt)
{
	bool close = false;
	wxGetApp().UpdateConfiguration(m_pDoc, close);
	if (close)
	{
		EndModal(wxID_OK);
		wxGetApp().GetTopWindow()->Close(true);
	}
}
