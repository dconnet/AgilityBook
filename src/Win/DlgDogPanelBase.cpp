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

#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/ReportListCtrl.h"
#include "LibARBWin/ReportListHeader.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBWin;

CDlgDogPanelBase::CDlgDogPanelBase(CAgilityBookDoc* pDoc, ARBDogPtr const& inDog, wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
	, m_pDoc(pDoc)
	, m_pDog(inDog)
{
}

/////////////////////////////////////////////////////////////////////////////

// TODO: not getting selected when selecting 2nd item via shift-click (wx bug)
// Listen to focused because of https://github.com/wxWidgets/wxWidgets/issues/4541
// Use EventTable because Binding allows a focused event after destroy on mac
constexpr int LIST_CTRL = 1000;
wxBEGIN_EVENT_TABLE(CDlgDogPanelReportBase, CDlgDogPanelBase)
	EVT_LIST_ITEM_FOCUSED(LIST_CTRL, CDlgDogPanelReportBase::OnItemFocused)
wxEND_EVENT_TABLE()

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


bool CDlgDogPanelReportBase::Save(unsigned int& hint)
{
	m_reportColumn.Save();
	return false;
}


wxWindow* CDlgDogPanelReportBase::GetInitialControl()
{
	return m_ctrlList;
}


void CDlgDogPanelReportBase::DeleteListItem(long index)
{
	m_ctrlList->DeleteItem(index);
	if (index == m_ctrlList->GetItemCount())
		--index;
	if (0 <= index)
		m_ctrlList->Select(index);
}


void CDlgDogPanelReportBase::DoCreate(bool isSingleSel, bool hasImageList)
{
	m_ctrlList = new CReportListCtrl(this, isSingleSel, CReportListCtrl::SortHeader::Sort, true, hasImageList);
	m_ctrlList->SetId(LIST_CTRL);
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, [this](wxListEvent& evt) { UpdateControls(); });
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_ITEM_DESELECTED, [this](wxListEvent& evt) { UpdateControls(); });
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &CDlgDogPanelReportBase::OnItemActivated, this);
	m_ctrlList->Bind(wxEVT_KEY_DOWN, &CDlgDogPanelReportBase::OnKeyDown, this);
	m_reportColumn.Initialize(this, m_ctrlList);

	m_btnNew = new wxButton(this, wxID_ANY, _("New..."), wxDefaultPosition, wxDefaultSize, 0);
	m_btnNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgDogPanelReportBase::OnNew, this);

	m_btnEdit = new wxButton(this, wxID_ANY, _("Edit..."), wxDefaultPosition, wxDefaultSize, 0);
	m_btnEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgDogPanelReportBase::OnEdit, this);

	m_btnDelete = new wxButton(this, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0);
	m_btnDelete->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgDogPanelReportBase::OnDelete, this);

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(m_ctrlList, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	m_sizerButtons = new wxBoxSizer(wxHORIZONTAL);
	m_sizerButtons->Add(m_btnNew, 0, wxRIGHT, padding.Inner());
	m_sizerButtons->Add(m_btnEdit, 0, wxRIGHT, padding.Inner());
	m_sizerButtons->Add(m_btnDelete, 0, wxRIGHT, padding.Inner());

	bSizer->Add(m_sizerButtons, 0, wxEXPAND | wxALL, padding.Controls());

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


void CDlgDogPanelReportBase::OnItemFocused(wxListEvent& evt)
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

} // namespace dconSoft
