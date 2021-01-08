/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Base class for dog panels
 * @author David Connet
 */

#include "stdafx.h"
#include "DlgDogPanelBase.h"

#include "LibARBWin/ReportListCtrl.h"
#include "LibARBWin/ReportListHeader.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

CDlgDogPanelBase::CDlgDogPanelBase(CAgilityBookDoc* pDoc, ARBDogPtr const& inDog, wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
	, m_pDoc(pDoc)
	, m_pDog(inDog)
{
}

/////////////////////////////////////////////////////////////////////////////

CDlgDogPanelReportBase::CDlgDogPanelReportBase(CAgilityBookDoc* pDoc, ARBDogPtr const& inDog, wxWindow* parent)
	: CDlgDogPanelBase(pDoc, inDog, parent)
	, m_ctrlList(nullptr)
	, m_reportColumn()
	, m_btnNew(nullptr)
	, m_btnEdit(nullptr)
	, m_btnDelete(nullptr)
	, m_sizerButtons(nullptr)
{
}


wxWindow* CDlgDogPanelReportBase::GetInitialControl()
{
	return m_ctrlList;
}


// CViewNameNumberDataPtr CDlgDogPanelReportBase::GetData(long index) const
//{
//	if (!m_ctrlList)
//		return CViewNameNumberDataPtr();
//	return std::dynamic_pointer_cast<CViewNameNumberData, CListData>(m_ctrlList->GetData(index));
//}
//
//
// void CDlgDogPanelReportBase::GetNameNumberData(std::vector<NameNumber>& items) const
//{
//	items.clear();
//	for (int idx = 0; idx < m_ctrlList->GetItemCount(); ++idx)
//	{
//		auto data = GetData(idx);
//		items.push_back(data->Data());
//	}
//}


void CDlgDogPanelReportBase::DoCreate(bool hasMultiSel, bool hasImageList)
{
	m_ctrlList = new CReportListCtrl(this, hasMultiSel, CReportListCtrl::SortHeader::Sort, true, hasImageList);
	// TODO: not getting selected when selecting 2nd item via shift-click (wx bug)
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, &CDlgDogPanelReportBase::OnItemSelected, this);
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_ITEM_DESELECTED, &CDlgDogPanelReportBase::OnItemUnselected, this);
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &CDlgDogPanelReportBase::OnItemActivated, this);
	m_ctrlList->Bind(wxEVT_KEY_DOWN, &CDlgDogPanelReportBase::OnKeyDown, this);
	m_reportColumn.Initialize(this, m_ctrlList);

	m_btnNew = new wxButton(this, wxID_ANY, _("New..."), wxDefaultPosition, wxDefaultSize, 0);
	m_btnNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgDogPanelReportBase::OnNew, this);

	m_btnEdit = new wxButton(this, wxID_ANY, _("Edit..."), wxDefaultPosition, wxDefaultSize, 0);
	m_btnEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgDogPanelReportBase::OnEdit, this);

	m_btnDelete = new wxButton(this, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0);
	m_btnDelete->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgDogPanelReportBase::OnDelete, this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(m_ctrlList, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	m_sizerButtons = new wxBoxSizer(wxHORIZONTAL);
	m_sizerButtons->Add(m_btnNew, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	m_sizerButtons->Add(m_btnEdit, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	m_sizerButtons->Add(m_btnDelete, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));

	bSizer->Add(m_sizerButtons, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	DoInitList();
	m_reportColumn.SizeColumns();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);

	UpdateControls();
}


void CDlgDogPanelReportBase::UpdateControls()
{
	std::vector<long> items;
	m_ctrlList->GetSelection(items);

	m_btnEdit->Enable(items.size() == 1);
	if (m_ctrlList->GetWindowStyleFlag() & wxLC_SINGLE_SEL)
		m_btnDelete->Enable(items.size() == 1);
	else
		m_btnDelete->Enable(items.size() > 0);
}


void CDlgDogPanelReportBase::DoDelete()
{
	std::vector<long> items;
	if (0 < m_ctrlList->GetSelection(items))
	{
		for (size_t idx = items.size(); idx > 0; --idx)
		{
			m_ctrlList->DeleteItem(items[idx - 1]);
		}
	}
}


void CDlgDogPanelReportBase::OnItemSelected(wxListEvent& evt)
{
	UpdateControls();
}


void CDlgDogPanelReportBase::OnItemUnselected(wxListEvent& evt)
{
	UpdateControls();
}


void CDlgDogPanelReportBase::OnItemActivated(wxListEvent& evt)
{
	DoEdit();
}


void CDlgDogPanelReportBase::OnKeyDown(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		evt.Skip();
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		DoEdit();
		break;
	}
}


void CDlgDogPanelReportBase::OnNew(wxCommandEvent& evt)
{
	DoNew();
}


void CDlgDogPanelReportBase::OnEdit(wxCommandEvent& evt)
{
	DoEdit();
}


void CDlgDogPanelReportBase::OnDelete(wxCommandEvent& evt)
{
	DoDelete();
}
