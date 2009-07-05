/*
 * Copyright � 2005-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgConfigMultiQ class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-07-30 DRC Created.
 */

#include "stdafx.h"
#include "DlgConfigMultiQ.h"

#include "ARBConfigDivision.h"
#include "ARBConfigEvent.h"
#include "ARBConfigLevel.h"
#include "ARBConfigSubLevel.h"
#include "ARBConfigVenue.h"
#include "DlgEventSelect.h"
#include "Globals.h"
#include "Validators.h"
#include <wx/datectrl.h>
#include <wx/listctrl.h>


BEGIN_EVENT_TABLE(CDlgConfigMultiQ, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgConfigMultiQ::OnOk)
END_EVENT_TABLE()


CDlgConfigMultiQ::CDlgConfigMultiQ(
		ARBConfigVenuePtr inVenue,
		ARBConfigMultiQPtr inMultiQ,
		wxWindow* pParent)
	: wxDialog()
	, m_pVenue(inVenue)
	, m_pOrigMultiQ(inMultiQ)
	, m_pMultiQ(inMultiQ->Clone())
	, m_Name(inMultiQ->GetName().c_str())
	, m_ShortName(inMultiQ->GetShortName().c_str())
	, m_bFrom(inMultiQ->GetValidFrom().IsValid())
	, m_ctrlDateFrom(NULL)
	, m_DateFrom(inMultiQ->GetValidFrom())
	, m_bTo(inMultiQ->GetValidTo().IsValid())
	, m_ctrlDateTo(NULL)
	, m_DateTo(inMultiQ->GetValidTo())
	, m_ctrlName(NULL)
	, m_ctrlItems(NULL)
	, m_ctrlEdit(NULL)
	, m_ctrlRemove(NULL)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	Create(pParent, wxID_ANY, _("IDD_CONFIG_MULTI_Q"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	// Controls (these are done first to control tab order)

	wxStaticText* textName = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_MULTIQ_NAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	m_ctrlName = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_Name, _("IDS_ENTER_NAME")));
	m_ctrlName->SetHelpText(_("HIDC_CONFIG_MULTIQ_NAME"));
	m_ctrlName->SetToolTip(_("HIDC_CONFIG_MULTIQ_NAME"));

	wxStaticText* textAbbrev = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_MULTIQ_SHORTNAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textAbbrev->Wrap(-1);

	wxTextCtrl* ctrlAbbrev = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_ShortName, _("IDS_ENTER_SHORTNAME")));
	ctrlAbbrev->SetHelpText(_("HIDC_CONFIG_MULTIQ_SHORTNAME"));
	ctrlAbbrev->SetToolTip(_("HIDC_CONFIG_MULTIQ_SHORTNAME"));

	wxCheckBox* ctrlValidFrom = new wxCheckBox(this, wxID_ANY,
		_("IDC_CONFIG_MULTIQ_DATE_VALID_FROM"),
		wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT,
		wxGenericValidator(&m_bFrom));
	ctrlValidFrom->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CDlgConfigMultiQ::OnClickFrom), NULL, this);
	ctrlValidFrom->SetHelpText(_("HIDC_CONFIG_MULTIQ_DATE_VALID_FROM"));
	ctrlValidFrom->SetToolTip(_("HIDC_CONFIG_MULTIQ_DATE_VALID_FROM"));

	m_ctrlDateFrom = new wxDatePickerCtrl(this, wxID_ANY, wxDefaultDateTime,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY,
		CGenericValidator(&m_DateFrom));
	m_ctrlDateFrom->SetHelpText(_("HIDC_CONFIG_MULTIQ_DATE_START"));
	m_ctrlDateFrom->SetToolTip(_("HIDC_CONFIG_MULTIQ_DATE_START"));

	wxCheckBox* ctrlValidTo = new wxCheckBox(this, wxID_ANY,
		_("IDC_CONFIG_MULTIQ_DATE_VALID_TO"),
		wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT,
		wxGenericValidator(&m_bTo));
	ctrlValidTo->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CDlgConfigMultiQ::OnClickTo), NULL, this);
	ctrlValidTo->SetHelpText(_("HIDC_CONFIG_MULTIQ_DATE_VALID_TO"));
	ctrlValidTo->SetToolTip(_("HIDC_CONFIG_MULTIQ_DATE_VALID_TO"));

	m_ctrlDateTo = new wxDatePickerCtrl(this, wxID_ANY, wxDefaultDateTime,
		wxDefaultPosition, wxDefaultSize, wxDP_DROPDOWN|wxDP_SHOWCENTURY,
		CGenericValidator(&m_DateTo));
	m_ctrlDateTo->SetHelpText(_("HIDC_CONFIG_MULTIQ_DATE_END"));
	m_ctrlDateTo->SetToolTip(_("HIDC_CONFIG_MULTIQ_DATE_END"));

	m_ctrlItems = new wxListView(this, wxID_ANY,
		wxDefaultPosition, wxSize(300, 95), wxLC_REPORT|wxLC_SINGLE_SEL);
	m_ctrlItems->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CDlgConfigMultiQ::OnItemchanged), NULL, this);
	m_ctrlItems->Connect(wxEVT_COMMAND_LEFT_DCLICK, wxMouseEventHandler(CDlgConfigMultiQ::OnDblclkItem), NULL, this);
	m_ctrlItems->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(CDlgConfigMultiQ::OnKeydownItem), NULL, this);
	m_ctrlItems->SetHelpText(_("HIDC_CONFIG_MULTIQ_ITEMS"));
	m_ctrlItems->SetToolTip(_("HIDC_CONFIG_MULTIQ_ITEMS"));
	m_ctrlItems->InsertColumn(0, _("IDS_COL_DIVISION"));
	m_ctrlItems->InsertColumn(1, _("IDS_COL_LEVEL"));
	m_ctrlItems->InsertColumn(2, _("IDS_COL_EVENT"));

	wxButton* btnAdd = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_MULTIQ_ADD"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnAdd->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigMultiQ::OnAdd), NULL, this);
	btnAdd->SetHelpText(_("HIDC_CONFIG_MULTIQ_ADD"));
	btnAdd->SetToolTip(_("HIDC_CONFIG_MULTIQ_ADD"));

	m_ctrlEdit = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_MULTIQ_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigMultiQ::OnEdit), NULL, this);
	m_ctrlEdit->SetHelpText(_("HIDC_CONFIG_MULTIQ_EDIT"));
	m_ctrlEdit->SetToolTip(_("HIDC_CONFIG_MULTIQ_EDIT"));

	m_ctrlRemove = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_MULTIQ_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRemove->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigMultiQ::OnRemove), NULL, this);
	m_ctrlRemove->SetHelpText(_("HIDC_CONFIG_MULTIQ_DELETE"));
	m_ctrlRemove->SetToolTip(_("HIDC_CONFIG_MULTIQ_DELETE"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerName->Add(m_ctrlName, 1, wxALL, 5);

	bSizer->Add(sizerName, 0, wxEXPAND, 5);

	wxBoxSizer* sizerAbbrev = new wxBoxSizer(wxHORIZONTAL);
	sizerAbbrev->Add(textAbbrev, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerAbbrev->Add(ctrlAbbrev, 0, wxALL, 3);

	bSizer->Add(sizerAbbrev, 0, wxEXPAND, 5);

	wxFlexGridSizer* sizerDates = new wxFlexGridSizer(2, 2, 0, 0);
	sizerDates->SetFlexibleDirection(wxBOTH);
	sizerDates->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	sizerDates->Add(ctrlValidFrom, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	sizerDates->Add(m_ctrlDateFrom, 0, wxALL, 3);
	sizerDates->Add(ctrlValidTo, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	sizerDates->Add(m_ctrlDateTo, 0, wxALL, 3);

	bSizer->Add(sizerDates, 0, wxEXPAND, 5);
	bSizer->Add(m_ctrlItems, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5);

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerBtns->Add(btnAdd, 0, wxALL, 5);
	sizerBtns->Add(m_ctrlEdit, 0, wxALL, 5);
	sizerBtns->Add(m_ctrlRemove, 0, wxALL, 5);

	bSizer->Add(sizerBtns, 0, wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	size_t n = m_pMultiQ->GetNumItems();
	for (size_t i = 0; i < n; ++i)
	{
		tstring div, level, evt;
		if (m_pMultiQ->GetItem(i, div, level, evt))
		{
			int idx = m_ctrlItems->InsertItem(static_cast<int>(i), div.c_str());
			SetListColumnText(m_ctrlItems, idx, 1, level.c_str());
			SetListColumnText(m_ctrlItems, idx, 2, evt.c_str());
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
			if (m_pMultiQ->RemoveItem(div.c_str(), level.c_str(), evt.c_str()))
				m_ctrlItems->DeleteItem(idx);
			if (m_pMultiQ->AddItem(dlg.GetDivision().c_str(), dlg.GetLevel().c_str(), dlg.GetEvent().c_str()))
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


void CDlgConfigMultiQ::OnDblclkItem(wxMouseEvent& evt)
{
	EditItem();
}


void CDlgConfigMultiQ::OnKeydownItem(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		EditItem();
		break;
	}
	evt.Skip();
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
		if (m_pMultiQ->AddItem(dlg.GetDivision().c_str(), dlg.GetLevel().c_str(), dlg.GetEvent().c_str()))
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
		if (m_pMultiQ->RemoveItem(div.c_str(), level.c_str(), evnt.c_str()))
			m_ctrlItems->DeleteItem(idx);
		else
			wxBell();
	}
}


void CDlgConfigMultiQ::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	if (m_pMultiQ->GetName() != m_Name.c_str())
	{
		if (m_pVenue->GetMultiQs().FindMultiQ(m_Name.c_str()))
		{
			wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
			m_ctrlName->SetFocus();
			return;
		}
	}

	m_pMultiQ->SetName(m_Name.c_str());
	m_pMultiQ->SetShortName(m_ShortName.c_str());
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
		m_pMultiQ->AddItem(div.c_str(), level.c_str(), evnt.c_str());
	}
	*m_pOrigMultiQ = *m_pMultiQ;

	EndDialog(wxID_OK);
}
