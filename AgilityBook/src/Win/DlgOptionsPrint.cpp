/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgOptionsPrint class
 * @author David Connet
 *
 * Revision History
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-07-14 Fixed group box creation order.
 * 2009-05-31 Added options for print page size.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-11-22 Moved printing to new page.
 */

#include "stdafx.h"
#include "DlgOptionsPrint.h"

#include "AgilityBook.h"
#include "Validators.h"
#include "Widgets.h"
#include <math.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CDlgOptionsPrint::CDlgOptionsPrint(wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
	, m_fontPrintInfo()
	, m_fontPrint()
	, m_PrintData(*wxGetApp().GetPrintData())
	, m_MetricSizes(CAgilityBookOptions::GetUnitsAsMM())
	, m_PageRunWidth(0.0)
	, m_PageRunHeight(0.0)
	, m_Left(0.0)
	, m_Right(0.0)
	, m_Top(0.0)
	, m_Bottom(0.0)
	, m_ctrlFontPrint(NULL)
	, m_Orientation(NULL)
	, m_Metric(NULL)
{
	CAgilityBookOptions::GetPrinterFontInfo(m_fontPrintInfo);
	m_fontPrintInfo.CreateFont(m_fontPrint);
	long l, r, t, b;
	CAgilityBookOptions::GetPrinterMargins(m_MetricSizes, l, r, t, b, NULL);
	m_Left = l;
	m_Right = r;
	m_Top = t;
	m_Bottom = b;
	if (!m_MetricSizes)
	{
		m_Left /= 100.0;
		m_Right /= 100.0;
		m_Top /= 100.0;
		m_Bottom /= 100.0;
	}
	CAgilityBookOptions::GetRunPageSize(m_MetricSizes, l, r, NULL);
	m_PageRunWidth = l;
	m_PageRunHeight = r;
	if (!m_MetricSizes)
	{
		m_PageRunWidth /= 100.0;
		m_PageRunHeight /= 100.0;
	}

	// Controls (these are done first to control tab order)

	wxStaticBox* boxFont = new wxStaticBox(this, wxID_ANY, _("IDC_OPT_PRINT_FONT_GROUP"));

	m_ctrlFontPrint = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PRINT_FONT_TEXT"),
		wxDefaultPosition, wxSize(110, 55), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxSUNKEN_BORDER);
	m_ctrlFontPrint->SetFont(m_fontPrint);
	m_ctrlFontPrint->Wrap(-1);

	wxButton* btnFont = new wxButton(this, wxID_ANY,
		_("IDC_OPT_PRINT_FONT"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(btnFont, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgOptionsPrint::OnFontPrint);
	btnFont->SetHelpText(_("HIDC_OPT_PRINT_FONT"));
	btnFont->SetToolTip(_("HIDC_OPT_PRINT_FONT"));

	wxString m_OrientationChoices[] =
	{
		_("IDC_OPT_PRINT_PORT"),
		_("IDC_OPT_PRINT_LAND")
	};
	int m_OrientationNChoices = sizeof(m_OrientationChoices) / sizeof(m_OrientationChoices[0]);
	m_Orientation = new wxRadioBox(this, wxID_ANY,
		_("IDC_OPT_PRINT_ORIENT"),
		wxDefaultPosition, wxDefaultSize,
		m_OrientationNChoices, m_OrientationChoices, 1, wxRA_SPECIFY_COLS);
	m_Orientation->SetHelpText(_("HIDC_OPT_PRINT_ORIENT"));
	m_Orientation->SetToolTip(_("HIDC_OPT_PRINT_ORIENT"));
	if (m_PrintData.GetPrintData().GetOrientation() == wxLANDSCAPE)
		m_Orientation->SetSelection(1);
	else
		m_Orientation->SetSelection(0);

	wxString m_MetricChoices[] =
	{
		_("IDC_OPT_PRINT_INCHES"),
		_("IDC_OPT_PRINT_METRIC")
	};
	int m_MetricNChoices = sizeof(m_MetricChoices) / sizeof(m_MetricChoices[0]);
	m_idxIn = 0; // Must agree with above
	m_idxMM = 1;
	m_Metric = new wxRadioBox(this, wxID_ANY,
		_("IDC_OPT_PRINT_UNITS"),
		wxDefaultPosition, wxDefaultSize,
		m_MetricNChoices, m_MetricChoices, 1, wxRA_SPECIFY_COLS);
	BIND_OR_CONNECT_CTRL(m_Metric, wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler, CDlgOptionsPrint::OnUnitsChange);
	m_Metric->SetHelpText(_("HIDC_OPT_PRINT_UNITS"));
	m_Metric->SetToolTip(_("HIDC_OPT_PRINT_UNITS"));
	m_Metric->SetSelection(m_MetricSizes ? m_idxMM : m_idxIn);

	wxStaticBox* boxPageSize = new wxStaticBox(this, wxID_ANY, _("IDC_OPT_PRINT_RUNPAGE"));

	wxStaticText* textWidth = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PRINT_RUNPAGE_W"),
		wxDefaultPosition, wxDefaultSize, 0);
	textWidth->Wrap(-1);

	CTextCtrl* ctrlWidth = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0,
		CGenericValidator(&m_PageRunWidth));
	ctrlWidth->SetHelpText(_("HIDC_OPT_PRINT_RUNPAGE_W"));
	ctrlWidth->SetToolTip(_("HIDC_OPT_PRINT_RUNPAGE_W"));

	wxStaticText* textHeight = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PRINT_RUNPAGE_H"),
		wxDefaultPosition, wxDefaultSize, 0);
	textHeight->Wrap(-1);
	
	CTextCtrl* ctrlHeight = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0,
		CGenericValidator(&m_PageRunHeight));
	ctrlHeight->SetHelpText(_("HIDC_OPT_PRINT_RUNPAGE_H"));
	ctrlHeight->SetToolTip(_("HIDC_OPT_PRINT_RUNPAGE_H"));

	wxStaticBox* boxMargins = new wxStaticBox(this, wxID_ANY, _("IDC_OPT_PRINT_MARGIN"));

	wxStaticText* textLeft = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PRINT_MARGIN_L"),
		wxDefaultPosition, wxDefaultSize, 0);
	textLeft->Wrap(-1);

	CTextCtrl* ctrlLeft = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0,
		CGenericValidator(&m_Left));
	ctrlLeft->SetHelpText(_("HIDC_OPT_PRINT_MARGIN_L"));
	ctrlLeft->SetToolTip(_("HIDC_OPT_PRINT_MARGIN_L"));

	wxStaticText* textRight = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PRINT_MARGIN_R"),
		wxDefaultPosition, wxDefaultSize, 0);
	textRight->Wrap(-1);

	CTextCtrl* ctrlRight = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0,
		CGenericValidator(&m_Right));
	ctrlRight->SetHelpText(_("HIDC_OPT_PRINT_MARGIN_R"));
	ctrlRight->SetToolTip(_("HIDC_OPT_PRINT_MARGIN_R"));

	wxStaticText* textTop = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PRINT_MARGIN_T"),
		wxDefaultPosition, wxDefaultSize, 0);
	textTop->Wrap(-1);

	CTextCtrl* ctrlTop = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0,
		CGenericValidator(&m_Top));
	ctrlTop->SetHelpText(_("HIDC_OPT_PRINT_MARGIN_T"));
	ctrlTop->SetToolTip(_("HIDC_OPT_PRINT_MARGIN_T"));

	wxStaticText* textBottom = new wxStaticText(this, wxID_ANY,
		_("IDC_OPT_PRINT_MARGIN_B"),
		wxDefaultPosition, wxDefaultSize, 0);
	textBottom->Wrap(-1);

	CTextCtrl* ctrlBottom = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), 0,
		CGenericValidator(&m_Bottom));
	ctrlBottom->SetHelpText(_("HIDC_OPT_PRINT_MARGIN_B"));
	ctrlBottom->SetToolTip(_("HIDC_OPT_PRINT_MARGIN_B"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* sizerPrint = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sizerFont = new wxStaticBoxSizer(boxFont, wxVERTICAL);
	sizerFont->Add(m_ctrlFontPrint, 1, wxALL|wxEXPAND, 5);
	sizerFont->Add(btnFont, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

	sizerPrint->Add(sizerFont, 0, wxALL, 5);

	wxBoxSizer* sizerRadio = new wxBoxSizer(wxHORIZONTAL);
	sizerRadio->Add(m_Orientation, 0, wxALL, 5);
	sizerRadio->Add(m_Metric, 0, wxALL, 5);

	wxStaticBoxSizer* sizerPageSize = new wxStaticBoxSizer(boxPageSize, wxVERTICAL);

	wxFlexGridSizer* sizerPage = new wxFlexGridSizer(2, 2, 0, 0);
	sizerPage->SetFlexibleDirection(wxBOTH);
	sizerPage->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerPage->Add(textWidth, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	sizerPage->Add(ctrlWidth, 0, wxALL, 5);
	sizerPage->Add(textHeight, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	sizerPage->Add(ctrlHeight, 0, wxALL, 5);

	sizerPageSize->Add(sizerPage, 0, wxEXPAND, 0);
	sizerRadio->Add(sizerPageSize, 0, wxEXPAND|wxALL, 5);

	sizerPrint->Add(sizerRadio, 0, wxEXPAND, 0);

	wxStaticBoxSizer* sizerMargins = new wxStaticBoxSizer(boxMargins, wxHORIZONTAL);

	wxFlexGridSizer* sizerLT = new wxFlexGridSizer(2, 2, 0, 0);
	sizerLT->SetFlexibleDirection(wxBOTH);
	sizerLT->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerLT->Add(textLeft, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	sizerLT->Add(ctrlLeft, 0, wxALL, 5);
	sizerLT->Add(textTop, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	sizerLT->Add(ctrlTop, 0, wxALL, 5);

	sizerMargins->Add(sizerLT, 1, wxEXPAND, 0);

	wxFlexGridSizer* sizerRB = new wxFlexGridSizer(2, 2, 0, 0);
	sizerRB->SetFlexibleDirection(wxBOTH);
	sizerRB->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerRB->Add(textRight, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	sizerRB->Add(ctrlRight, 0, wxALL, 5);
	sizerRB->Add(textBottom, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	sizerRB->Add(ctrlBottom, 0, wxALL, 5);

	sizerMargins->Add(sizerRB, 1, wxEXPAND, 0);

	sizerPrint->Add(sizerMargins, 0, wxALL, 5);

	SetSizer(sizerPrint);
	Layout();
	sizerPrint->Fit(this);
}


void CDlgOptionsPrint::Save()
{
	CAgilityBookOptions::SetPrinterFontInfo(m_fontPrintInfo);
	m_PrintData.GetPrintData().SetOrientation(m_Orientation->GetSelection() == 1 ? wxLANDSCAPE : wxPORTRAIT);
	wxGetApp().SavePrintData(m_PrintData);
	if (!m_MetricSizes)
	{
		m_PageRunWidth *= 100.0;
		m_PageRunHeight *= 100.0;
		m_Left *= 100.0;
		m_Right *= 100.0;
		m_Top *= 100.0;
		m_Bottom *= 100.0;
	}
	CAgilityBookOptions::SetUnitsAsMM(m_MetricSizes);
	CAgilityBookOptions::SetRunPageSize(
		m_MetricSizes,
		static_cast<long>(m_PageRunWidth),
		static_cast<long>(m_PageRunHeight));
	CAgilityBookOptions::SetPrinterMargins(
		m_MetricSizes,
		static_cast<long>(m_Left),
		static_cast<long>(m_Right),
		static_cast<long>(m_Top),
		static_cast<long>(m_Bottom));
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


static double ConvertInchesMM(double val, bool bToMM)
{
	if (bToMM)
		val *= 25.4;
	else
		val /= 25.4;
	// Now round in MMs, where we have no decimals.
	if (bToMM)
		val = floor(val + 0.5);
	return val;
}


void CDlgOptionsPrint::OnUnitsChange(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	bool bMetric = false;
	if (m_Metric->GetSelection() == m_idxMM)
		bMetric = true;
	if (bMetric != m_MetricSizes)
	{
		m_MetricSizes = bMetric;
		m_PageRunWidth = ConvertInchesMM(m_PageRunWidth, bMetric);
		m_PageRunHeight = ConvertInchesMM(m_PageRunHeight, bMetric);
		m_Left = ConvertInchesMM(m_Left, bMetric);
		m_Right = ConvertInchesMM(m_Right, bMetric);
		m_Top = ConvertInchesMM(m_Top, bMetric);
		m_Bottom = ConvertInchesMM(m_Bottom, bMetric);
		TransferDataToWindow();
	}
}
