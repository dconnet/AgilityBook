/*
 * Copyright © 2006-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgOptionsPrint class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-11-22 DRC Moved printing to new page.
 */

#include "stdafx.h"
#include "DlgOptionsPrint.h"

#include "Validators.h"


CDlgOptionsPrint::CDlgOptionsPrint(wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
	, m_fontPrintInfo()
	, m_fontPrint()
	, m_Left(0.0)
	, m_Right(0.0)
	, m_Top(0.0)
	, m_Bottom(0.0)
	, m_ctrlFontPrint(NULL)
{
	CAgilityBookOptions::GetPrinterFontInfo(m_fontPrintInfo);
	long l, r, t, b;
	CAgilityBookOptions::GetPrinterMargins(l, r, t, b);
	m_Left = l / 100.0;
	m_Right = r / 100.0;
	m_Top = t / 100.0;
	m_Bottom = b / 100.0;
	m_fontPrintInfo.CreateFont(m_fontPrint);

	// Controls (these are done first to control tab order)

	m_ctrlFontPrint = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PRINT_FONT_TEXT"),
		wxDefaultPosition, wxSize(110 ,55), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxSUNKEN_BORDER);
	m_ctrlFontPrint->SetFont(m_fontPrint);
	m_ctrlFontPrint->Wrap(-1);

	wxButton* btnFont = new wxButton(this, wxID_ANY,
		_("IDC_OPT_PRINT_FONT"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnFont->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgOptionsPrint::OnFontPrint), NULL, this);
	btnFont->SetHelpText(_("HIDC_OPT_PRINT_FONT"));
	btnFont->SetToolTip(_("HIDC_OPT_PRINT_FONT"));

	wxStaticText* textLeft = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PRINT_MARGIN_L"),
		wxDefaultPosition, wxDefaultSize, 0);
	textLeft->Wrap(-1);

	wxTextCtrl* ctrlLeft = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CGenericValidator(&m_Left));
	ctrlLeft ->SetHelpText(_("HIDC_OPT_PRINT_MARGIN_L"));
	ctrlLeft ->SetToolTip(_("HIDC_OPT_PRINT_MARGIN_L"));

	wxStaticText* textRight = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PRINT_MARGIN_R"),
		wxDefaultPosition, wxDefaultSize, 0);
	textRight->Wrap(-1);

	wxTextCtrl* ctrlRight = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CGenericValidator(&m_Right));
	ctrlRight->SetHelpText(_("HIDC_OPT_PRINT_MARGIN_R"));
	ctrlRight->SetToolTip(_("HIDC_OPT_PRINT_MARGIN_R"));

	wxStaticText* textTop = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PRINT_MARGIN_T"),
		wxDefaultPosition, wxDefaultSize, 0);
	textTop->Wrap(-1);

	wxTextCtrl* ctrlTop = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CGenericValidator(&m_Top));
	ctrlTop ->SetHelpText(_("HIDC_OPT_PRINT_MARGIN_T"));
	ctrlTop ->SetToolTip(_("HIDC_OPT_PRINT_MARGIN_T"));

	wxStaticText* textBottom = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PRINT_MARGIN_B"),
		wxDefaultPosition, wxDefaultSize, 0);
	textBottom->Wrap(-1);

	wxTextCtrl* ctrlBottom = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CGenericValidator(&m_Bottom));
	ctrlBottom->SetHelpText(_("HIDC_OPT_PRINT_MARGIN_B"));
	ctrlBottom->SetToolTip(_("HIDC_OPT_PRINT_MARGIN_B"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* sizerPrint = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sizerFont = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("IDC_OPT_PRINT_FONT_GROUP")), wxVERTICAL);
	sizerFont->Add(m_ctrlFontPrint, 1, wxALL|wxEXPAND, 5);
	sizerFont->Add(btnFont, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	sizerPrint->Add(sizerFont, 0, wxALL, 5);

	wxStaticBoxSizer* sizerMargins = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("IDC_OPT_PRING_MARGIN")), wxHORIZONTAL);

	wxFlexGridSizer* sizerLT = new wxFlexGridSizer(2, 2, 0, 0);
	sizerLT->SetFlexibleDirection(wxBOTH);
	sizerLT->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerLT->Add(textLeft, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	sizerLT->Add(ctrlLeft, 0, wxALL, 5);
	sizerLT->Add(textTop, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	sizerLT->Add(ctrlTop, 0, wxALL, 5);

	sizerMargins->Add(sizerLT, 1, wxEXPAND, 5);

	wxFlexGridSizer* sizerRB = new wxFlexGridSizer(2, 2, 0, 0);
	sizerRB->SetFlexibleDirection(wxBOTH);
	sizerRB->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerRB->Add(textRight, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	sizerRB->Add(ctrlRight, 0, wxALL, 5);
	sizerRB->Add(textBottom, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	sizerRB->Add(ctrlBottom, 0, wxALL, 5);

	sizerMargins->Add(sizerRB, 1, wxEXPAND, 5);

	sizerPrint->Add(sizerMargins, 0, wxALL, 5);

	SetSizer(sizerPrint);
	Layout();
	sizerPrint->Fit(this);
}


void CDlgOptionsPrint::Save()
{
	CAgilityBookOptions::SetPrinterFontInfo(m_fontPrintInfo);
	CAgilityBookOptions::SetPrinterMargins(
		static_cast<long>(m_Left * 100),
		static_cast<long>(m_Right * 100),
		static_cast<long>(m_Top * 100),
		static_cast<long>(m_Bottom * 100));
}


void CDlgOptionsPrint::OnFontPrint(wxCommandEvent& evt)
{
	TransferDataFromWindow();

	wxFontData data;
	data.SetAllowSymbols(false);
	data.SetInitialFont(m_fontPrint);
	wxFontDialog dlg(this, data);
	if (wxID_OK == dlg.ShowModal())
	{
		m_fontPrintInfo.CreateFont(dlg, m_fontPrint);
		m_ctrlFontPrint->SetFont(m_fontPrint);
	}
}
