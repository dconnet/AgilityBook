/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgConfigMultiQ class
 * @author David Connet
 *
 * Revision History
 * 2015-01-01 Changed pixels to dialog units.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-07-30 Created.
 */

#include "stdafx.h"
#include "DlgConfigMultiQ.h"

#include "AgilityBook.h"
#include "DlgEventSelect.h"

#include "ARB/ARBConfigDivision.h"
#include "ARB/ARBConfigEvent.h"
#include "ARB/ARBConfigLevel.h"
#include "ARB/ARBConfigSubLevel.h"
#include "ARB/ARBConfigVenue.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/ReportListCtrl.h"
#include "LibARBWin/Validators.h"
#include <wx/datectrl.h>
#include <wx/listctrl.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;


CDlgConfigMultiQ::CDlgConfigMultiQ(
	ARBConfigVenuePtr const& inVenue,
	ARBConfigMultiQPtr const& inMultiQ,
	wxWindow* pParent)
	: wxDialog()
	, m_pVenue(inVenue)
	, m_pOrigMultiQ(inMultiQ)
	, m_pMultiQ(inMultiQ->Clone())
	, m_Name(inMultiQ->GetName())
	, m_ShortName(inMultiQ->GetShortName())
	, m_bFrom(inMultiQ->GetValidFrom().IsValid())
	, m_ctrlDateFrom(nullptr)
	, m_DateFrom(inMultiQ->GetValidFrom())
	, m_bTo(inMultiQ->GetValidTo().IsValid())
	, m_ctrlDateTo(nullptr)
	, m_DateTo(inMultiQ->GetValidTo())
	, m_ctrlName(nullptr)
	, m_ctrlItems(nullptr)
	, m_ctrlEdit(nullptr)
	, m_ctrlRemove(nullptr)
	, m_trace("CDlgConfigMultiQ")
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_CONFIG_MULTI_Q"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	// Controls (these are done first to control tab order)

	wxStaticText* textName
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_MULTIQ_NAME"), wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	m_ctrlName = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		CTrimValidator(&m_Name, TRIMVALIDATOR_DEFAULT, _("IDS_ENTER_NAME")));
	m_ctrlName->SetHelpText(_("HIDC_CONFIG_MULTIQ_NAME"));
	m_ctrlName->SetToolTip(_("HIDC_CONFIG_MULTIQ_NAME"));

	wxStaticText* textAbbrev
		= new wxStaticText(this, wxID_ANY, _("IDC_CONFIG_MULTIQ_SHORTNAME"), wxDefaultPosition, wxDefaultSize, 0);
	textAbbrev->Wrap(-1);

	CTextCtrl* ctrlAbbrev = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		CTrimValidator(&m_ShortName, TRIMVALIDATOR_DEFAULT, _("IDS_ENTER_SHORTNAME")));
	ctrlAbbrev->SetHelpText(_("HIDC_CONFIG_MULTIQ_SHORTNAME"));
	ctrlAbbrev->SetToolTip(_("HIDC_CONFIG_MULTIQ_SHORTNAME"));

	wxCheckBox* ctrlValidFrom = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_MULTIQ_DATE_VALID_FROM"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_RIGHT,
		wxGenericValidator(&m_bFrom));
	ctrlValidFrom->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgConfigMultiQ::OnClickFrom, this);
	ctrlValidFrom->SetHelpText(_("HIDC_CONFIG_MULTIQ_DATE_VALID_FROM"));
	ctrlValidFrom->SetToolTip(_("HIDC_CONFIG_MULTIQ_DATE_VALID_FROM"));

	m_ctrlDateFrom = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		wxDefaultDateTime,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY,
		CGenericValidator(&m_DateFrom));
	m_ctrlDateFrom->SetHelpText(_("HIDC_CONFIG_MULTIQ_DATE_START"));
	m_ctrlDateFrom->SetToolTip(_("HIDC_CONFIG_MULTIQ_DATE_START"));

	wxCheckBox* ctrlValidTo = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_CONFIG_MULTIQ_DATE_VALID_TO"),
		wxDefaultPosition,
		wxDefaultSize,
		wxALIGN_RIGHT,
		wxGenericValidator(&m_bTo));
	ctrlValidTo->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgConfigMultiQ::OnClickTo, this);
	ctrlValidTo->SetHelpText(_("HIDC_CONFIG_MULTIQ_DATE_VALID_TO"));
	ctrlValidTo->SetToolTip(_("HIDC_CONFIG_MULTIQ_DATE_VALID_TO"));

	m_ctrlDateTo = new wxDatePickerCtrl(
		this,
		wxID_ANY,
		wxDefaultDateTime,
		wxDefaultPosition,
		wxDefaultSize,
		wxDP_DROPDOWN | wxDP_SHOWCENTURY,
		CGenericValidator(&m_DateTo));
	m_ctrlDateTo->SetHelpText(_("HIDC_CONFIG_MULTIQ_DATE_END"));
	m_ctrlDateTo->SetToolTip(_("HIDC_CONFIG_MULTIQ_DATE_END"));

	m_ctrlItems = new CListCtrl(
		this,
		wxID_ANY,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(170, 50)),
		wxLC_REPORT | wxLC_SINGLE_SEL);
	m_ctrlItems->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, &CDlgConfigMultiQ::OnItemchanged, this);
	m_ctrlItems->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &CDlgConfigMultiQ::OnItemActivated, this);
	m_ctrlItems->Bind(wxEVT_KEY_DOWN, &CDlgConfigMultiQ::OnKeydownItem, this);
	m_ctrlItems->SetHelpText(_("HIDC_CONFIG_MULTIQ_ITEMS"));
	m_ctrlItems->SetToolTip(_("HIDC_CONFIG_MULTIQ_ITEMS"));
	m_ctrlItems->InsertColumn(0, _("IDS_COL_DIVISION"));
	m_ctrlItems->InsertColumn(1, _("IDS_COL_LEVEL"));
	m_ctrlItems->InsertColumn(2, _("IDS_COL_EVENT"));

	wxButton* btnAdd = new wxButton(this, wxID_ANY, _("IDC_CONFIG_MULTIQ_ADD"), wxDefaultPosition, wxDefaultSize, 0);
	btnAdd->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigMultiQ::OnAdd, this);
	btnAdd->SetHelpText(_("HIDC_CONFIG_MULTIQ_ADD"));
	btnAdd->SetToolTip(_("HIDC_CONFIG_MULTIQ_ADD"));

	m_ctrlEdit = new wxButton(this, wxID_ANY, _("IDC_CONFIG_MULTIQ_EDIT"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigMultiQ::OnEdit, this);
	m_ctrlEdit->SetHelpText(_("HIDC_CONFIG_MULTIQ_EDIT"));
	m_ctrlEdit->SetToolTip(_("HIDC_CONFIG_MULTIQ_EDIT"));

	m_ctrlRemove = new wxButton(this, wxID_ANY, _("IDC_CONFIG_MULTIQ_DELETE"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRemove->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigMultiQ::OnRemove, this);
	m_ctrlRemove->SetHelpText(_("HIDC_CONFIG_MULTIQ_DELETE"));
	m_ctrlRemove->SetToolTip(_("HIDC_CONFIG_MULTIQ_DELETE"));

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerName->Add(m_ctrlName, 1, wxALIGN_CENTER_VERTICAL);

	bSizer->Add(sizerName, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerAbbrev = new wxBoxSizer(wxHORIZONTAL);
	sizerAbbrev->Add(textAbbrev, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Inner());
	sizerAbbrev->Add(ctrlAbbrev, 0, wxALIGN_CENTER_VERTICAL);

	bSizer->Add(sizerAbbrev, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxFlexGridSizer* sizerDates = new wxFlexGridSizer(2, 2, padding.Inner(), padding.Controls());
	sizerDates->SetFlexibleDirection(wxBOTH);
	sizerDates->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	sizerDates->Add(ctrlValidFrom, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerDates->Add(m_ctrlDateFrom, 0, wxALIGN_CENTER_VERTICAL);
	sizerDates->Add(ctrlValidTo, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerDates->Add(m_ctrlDateTo, 0, wxALIGN_CENTER_VERTICAL);

	bSizer->Add(sizerDates, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	bSizer->Add(m_ctrlItems, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerBtns->Add(btnAdd, 0, wxRIGHT, padding.Inner());
	sizerBtns->Add(m_ctrlEdit, 0, wxRIGHT, padding.Inner());
	sizerBtns->Add(m_ctrlRemove);

	bSizer->Add(sizerBtns, 0, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigMultiQ::OnOk, this, wxID_OK);

	size_t n = m_pMultiQ->GetNumItems();
	for (size_t i = 0; i < n; ++i)
	{
		wxString div, level, evt;
		if (m_pMultiQ->GetItem(i, div, level, evt))
		{
			int idx = m_ctrlItems->InsertItem(static_cast<int>(i), div);
			SetListColumnText(m_ctrlItems, idx, 1, level);
			SetListColumnText(m_ctrlItems, idx, 2, evt);
		}
	}
	m_ctrlItems->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
	m_ctrlItems->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
	m_ctrlItems->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);

	UpdateControls();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	IMPLEMENT_ON_INIT(m_ctrlName)
}


void CDlgConfigMultiQ::UpdateControls()
{
	m_ctrlDateFrom->Enable(m_bFrom);
	m_ctrlDateTo->Enable(m_bTo);
	bool bEnable = (0 <= m_ctrlItems->GetFirstSelected());
	m_ctrlEdit->Enable(bEnable);
	m_ctrlRemove->Enable(bEnable);
}


void CDlgConfigMultiQ::EditItem()
{
	int idx = m_ctrlItems->GetFirstSelected();
	if (0 <= idx)
	{
		wxString div = GetListColumnText(m_ctrlItems, idx, 0);
		wxString level = GetListColumnText(m_ctrlItems, idx, 1);
		wxString evt = GetListColumnText(m_ctrlItems, idx, 2);
		ARBDate date = ARBDate::Today();
		if (m_bFrom)
			date = m_DateFrom;
		else if (m_bTo)
			date = m_DateTo;
		CDlgEventSelect dlg(m_pVenue, date, div, level, evt, this);
		if (wxID_OK == dlg.ShowModal())
		{
			if (m_pMultiQ->RemoveItem(div, level, evt))
				m_ctrlItems->DeleteItem(idx);
			if (m_pMultiQ->AddItem(dlg.GetDivision(), dlg.GetLevel(), dlg.GetEvent()))
			{
				idx = m_ctrlItems->InsertItem(idx, dlg.GetDivision());
				SetListColumnText(m_ctrlItems, idx, 1, dlg.GetLevel());
				SetListColumnText(m_ctrlItems, idx, 2, dlg.GetEvent());
				m_ctrlItems->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
				m_ctrlItems->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
				m_ctrlItems->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
			}
			else
				wxBell();
		}
	}
}


void CDlgConfigMultiQ::OnItemchanged(wxListEvent& evt)
{
	TransferDataFromWindow();
	UpdateControls();
}


void CDlgConfigMultiQ::OnItemActivated(wxListEvent& evt)
{
	EditItem();
}


void CDlgConfigMultiQ::OnKeydownItem(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		evt.Skip();
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		EditItem();
		break;
	}
}


void CDlgConfigMultiQ::OnClickFrom(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	UpdateControls();
}


void CDlgConfigMultiQ::OnClickTo(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	UpdateControls();
}


void CDlgConfigMultiQ::OnAdd(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	ARBDate date = ARBDate::Today();
	if (m_bFrom)
		date = m_DateFrom;
	else if (m_bTo)
		date = m_DateTo;
	CDlgEventSelect dlg(m_pVenue, date, wxEmptyString, wxEmptyString, wxEmptyString, this);
	if (wxID_OK == dlg.ShowModal())
	{
		if (m_pMultiQ->AddItem(dlg.GetDivision(), dlg.GetLevel(), dlg.GetEvent()))
		{
			int idx = m_ctrlItems->InsertItem(m_ctrlItems->GetItemCount(), dlg.GetDivision());
			SetListColumnText(m_ctrlItems, idx, 1, dlg.GetLevel());
			SetListColumnText(m_ctrlItems, idx, 2, dlg.GetEvent());
			m_ctrlItems->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
			m_ctrlItems->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
			m_ctrlItems->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
		}
		else
			wxBell();
	}
}


void CDlgConfigMultiQ::OnEdit(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	EditItem();
}


void CDlgConfigMultiQ::OnRemove(wxCommandEvent& evt)
{
	int idx = m_ctrlItems->GetFirstSelected();
	if (0 <= idx)
	{
		wxString div = GetListColumnText(m_ctrlItems, idx, 0);
		wxString level = GetListColumnText(m_ctrlItems, idx, 1);
		wxString evnt = GetListColumnText(m_ctrlItems, idx, 2);
		if (m_pMultiQ->RemoveItem(div, level, evnt))
			m_ctrlItems->DeleteItem(idx);
		else
			wxBell();
	}
}


void CDlgConfigMultiQ::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	wxString stdName(m_Name);
	if (m_pMultiQ->GetName() != stdName)
	{
		if (m_pVenue->GetMultiQs().FindMultiQ(stdName))
		{
			wxMessageBox(_("IDS_NAME_IN_USE"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_EXCLAMATION);
			m_ctrlName->SetFocus();
			return;
		}
	}

	m_pMultiQ->SetName(m_Name);
	m_pMultiQ->SetShortName(m_ShortName);
	if (!m_bFrom)
		m_DateFrom.clear();
	m_pMultiQ->SetValidFrom(m_DateFrom);
	if (!m_bTo)
		m_DateTo.clear();
	m_pMultiQ->SetValidTo(m_DateTo);
	m_pMultiQ->RemoveAllItems();
	for (int idx = m_ctrlItems->GetItemCount() - 1; idx >= 0; --idx)
	{
		wxString div = GetListColumnText(m_ctrlItems, idx, 0);
		wxString level = GetListColumnText(m_ctrlItems, idx, 1);
		wxString evnt = GetListColumnText(m_ctrlItems, idx, 2);
		m_pMultiQ->AddItem(div, level, evnt);
	}
	*m_pOrigMultiQ = *m_pMultiQ;

	EndDialog(wxID_OK);
}

} // namespace dconSoft
