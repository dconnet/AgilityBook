/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgAbout class
 * @author David Connet
 *
 * Revision History
 * @li 2008-12-14 DRC Ported to wxWidgets.
 * @li 2006-12-10 DRC Simplified download links.
 * @li 2005-02-08 DRC Added another link.
 * @li 2004-07-20 DRC This now takes a Doc ptr so the update feature works.
 * @li 2004-03-05 DRC Moved versioninfo into new class, added new links/buttons.
 * @li 2004-03-02 DRC Added %VERSION% keyword in hotlinks.
 * @li 2003-09-17 DRC Added a 'check for updates' control.
 */

#include "stdafx.h"
#include "DlgAbout.h"

#include "AgilityBook.h"
#include "ARBString.h"
#include "VersionNumber.h"

#include "res/AgilityBook32.xpm"


// Add a focus rect to the control
class CHyperlinkCtrl : public wxHyperlinkCtrl
{
    DECLARE_DYNAMIC_CLASS(CHyperlinkCtrl)
public:
	CHyperlinkCtrl() {}
	CHyperlinkCtrl(wxWindow *pParent,
			wxWindowID id,
			const wxString& label, const wxString& url,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxHL_DEFAULT_STYLE,
			const wxString& name = wxHyperlinkCtrlNameStr);
	DECLARE_EVENT_TABLE()
	void OnPaint(wxPaintEvent& WXUNUSED(evt));
	void OnFocus(wxFocusEvent& evt);
};


IMPLEMENT_DYNAMIC_CLASS(CHyperlinkCtrl, wxHyperlinkCtrl)


BEGIN_EVENT_TABLE(CHyperlinkCtrl, wxHyperlinkCtrl)
    EVT_PAINT(CHyperlinkCtrl::OnPaint)
    EVT_SET_FOCUS(CHyperlinkCtrl::OnFocus)
    EVT_KILL_FOCUS(CHyperlinkCtrl::OnFocus)
END_EVENT_TABLE()


CHyperlinkCtrl::CHyperlinkCtrl(wxWindow *pParent,
		wxWindowID id,
		const wxString& label, const wxString& url,
		const wxPoint& pos,
		const wxSize& size,
		long style,
		const wxString& name)
	: wxHyperlinkCtrl()
{
	Create(pParent, id, label, url, pos, size, style, name);
}


void CHyperlinkCtrl::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxPaintDC dc(this);
    dc.SetFont(GetFont());
    dc.SetTextForeground(GetForegroundColour());
    dc.SetTextBackground(GetBackgroundColour());

    dc.DrawText(GetLabel(), GetLabelRect().GetTopLeft());
	if (wxWindow::DoFindFocus() == this)
	{
		wxBrush br(wxColour(0,0,0), wxTRANSPARENT);
		dc.SetBrush(br);
		wxColour cr = GetBackgroundColour();
		// Invert the background color.
		wxPen pen(wxColor(~cr.Red(),~cr.Green(),~cr.Blue()), 1, wxDOT);
		dc.SetPen(pen);
		wxRect r = GetClientRect();
		dc.DrawRectangle(r);
	}
}


void CHyperlinkCtrl::OnFocus(wxFocusEvent& evt)
{
	Refresh();
}

/////////////////////////////////////////////////////////////////////////////

CDlgAbout::CDlgAbout(CAgilityBookDoc* pDoc, wxWindow* pParent)
	: wxDialog()
	, m_pDoc(pDoc)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_ABOUTBOX"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	// Controls (these are done first to control tab order)

	wxStaticBitmap* icon = new wxStaticBitmap(this, wxID_ANY,
		wxIcon(AgilityBook32_xpm),
		wxPoint(5, 5), wxDefaultSize, 0);

	wxStaticText* version = new wxStaticText(this, wxID_ANY,
		wxString::Format(_("Version"), wxString::From8BitData(ARB_VERSION_STRING).c_str(), _("Agility Record Book")),
		wxDefaultPosition, wxDefaultSize, 0);
	version->Wrap(400);

	CHyperlinkCtrl* link1 = new CHyperlinkCtrl(this, wxID_ANY,
		_("LinkYahooText"), _("LinkYahooUrl"),
		wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE);
	link1->SetToolTip(_("LinkYahooUrl"));
	link1->SetHelpText(_("LinkYahooUrl"));

	CHyperlinkCtrl* link2 = new CHyperlinkCtrl(this, wxID_ANY,
		_("LinkHelpText"), _("LinkHelpUrl"),
		wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE);
	link2->SetToolTip(_("LinkHelpUrl"));
	link2->SetHelpText(_("LinkHelpUrl"));
	Connect(link2->GetId(), wxEVT_COMMAND_HYPERLINK, wxHyperlinkEventHandler(CDlgAbout::OnHelpEmail));

	wxStaticText* usefulLinks = new wxStaticText(this, wxID_ANY,
		_("UsefulLinks"),
		wxDefaultPosition, wxDefaultSize, 0);
	usefulLinks->Wrap(-1);

	CHyperlinkCtrl* link3 = new CHyperlinkCtrl(this, wxID_ANY,
		_("LinkArbText"), _("LinkArbUrl"),
		wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE);
	link3->SetToolTip(_("LinkArbUrl"));
	link3->SetHelpText(_("LinkArbUrl"));

	CHyperlinkCtrl* link4 = new CHyperlinkCtrl(this, wxID_ANY,
		_("LinkArbDownloadText"), _("LinkArbDownloadUrl"),
		wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE);
	link4->SetToolTip(_("LinkArbDownloadUrl"));
	link4->SetHelpText(_("LinkArbDownloadUrl"));

	wxButton* updates = new wxButton(this, wxID_ANY,
		_("CheckForUpdates"),
		wxDefaultPosition, wxDefaultSize, 0);
	updates->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgAbout::OnCheckForUpdates), NULL, this);
	updates->SetHelpText(_("HIDC_ABOUT_UPDATE"));

	wxButton* ok = new wxButton(this, wxID_OK);
	ok->SetDefault();

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* sizerTop = new wxBoxSizer(wxHORIZONTAL);
	sizerTop->Add(icon, 0, wxALIGN_LEFT|wxALIGN_TOP|wxALL, 10);

	wxBoxSizer* sizerBox = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerText = new wxBoxSizer(wxVERTICAL);
	sizerText->Add(version, 1, wxEXPAND|wxRIGHT|wxTOP, 10);

	wxBoxSizer* sizerLinksBox1 = new wxBoxSizer(wxHORIZONTAL);
	sizerLinksBox1->Add(20, 0, 0, wxEXPAND, 0);

	wxBoxSizer* sizerLinks1 = new wxBoxSizer(wxVERTICAL);
	sizerLinks1->Add(link1, 0, wxALL, 2);
	sizerLinks1->Add(link2, 0, wxALL, 2);

	sizerLinksBox1->Add(sizerLinks1, 1, wxEXPAND, 0);

	sizerText->Add(sizerLinksBox1, 0, wxEXPAND, 0);
	sizerText->Add(0, 5, 0, wxEXPAND, 0);
	sizerText->Add(usefulLinks, 0, wxALL, 0);

	wxBoxSizer* sizerLinksBox2 = new wxBoxSizer(wxHORIZONTAL);
	sizerLinksBox2->Add(20, 0, 0, wxEXPAND, 0);

	wxBoxSizer* sizerLinks2 = new wxBoxSizer(wxVERTICAL);
	sizerLinks2->Add(link3, 0, wxALL, 2);
	sizerLinks2->Add(link4, 0, wxALL, 2);

	sizerLinksBox2->Add(sizerLinks2, 1, wxEXPAND, 0);

	sizerText->Add(sizerLinksBox2, 0, wxEXPAND, 0);

	sizerBox->Add(sizerText, 1, wxEXPAND, 0);

	wxBoxSizer* sizerButtons = new wxBoxSizer(wxHORIZONTAL);
	sizerButtons->Add(updates, 0, wxALIGN_LEFT, 0);
	sizerButtons->Add(0, 0, 1, wxEXPAND, 0);
	sizerButtons->Add(ok, 0, wxALIGN_RIGHT, 0);

	sizerBox->Add(sizerButtons, 0, wxBOTTOM|wxEXPAND|wxRIGHT|wxTOP, 10);

	sizerTop->Add(sizerBox, 1, wxEXPAND, 0);

	SetSizer(sizerTop);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	ok->SetFocus();
}


void CDlgAbout::OnHelpEmail(wxHyperlinkEvent& evt)
{
	wxString url = evt.GetURL();
	otstringstream str;
	str << ARB_VER_MAJOR << '.' << ARB_VER_MINOR << '.' << ARB_VER_DOT;
	url.Replace(wxT("%VERSION%"), str.str().c_str());
	wxLaunchDefaultBrowser(url);
}


void CDlgAbout::OnCheckForUpdates(wxCommandEvent& evt)
{
	wxGetApp().UpdateConfiguration(m_pDoc);
}
