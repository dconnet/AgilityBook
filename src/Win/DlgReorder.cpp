/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgReorder class
 * @author David Connet
 *
 * Revision History
 * 2015-05-19 Size dialog so all runs are visible.
 * 2015-01-01 Changed pixels to dialog units.
 * 2012-12-31 Select run on startup.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2011-10-14 Changed how reorder dlg is used, made run reorder a tree.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-09 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "stdafx.h"
#include "DlgReorder.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "Widgets.h"

#include "ARB/ARBDog.h"
#include "ARBCommon/ARBDate.h"
#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


class CReorderListData : public wxClientData
{
public:
	CReorderListData(ARBDogPtr const& inDog)
		: m_pDog(inDog)
	{
	}
	ARBDogPtr m_pDog;
};


class CReorderTreeData : public wxTreeItemData
{
public:
	CReorderTreeData(ARBDogRunPtr const& inRun)
		: m_pRun(inRun)
	{
	}
	ARBDogRunPtr m_pRun;
};


BEGIN_EVENT_TABLE(CDlgReorder, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgReorder::OnOk)
END_EVENT_TABLE()


CDlgReorder::CDlgReorder(
		CAgilityBookDoc* pDoc,
		ARBDogList* dogs,
		wxWindow* pParent)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_Dogs(dogs)
	, m_Trial()
	, m_Run()
	, m_ctrlList(nullptr)
	, m_ctrlTree(nullptr)
	, m_ctrlUp(nullptr)
	, m_ctrlDown(nullptr)
{
	Init(pParent);
}


CDlgReorder::CDlgReorder(
		CAgilityBookDoc* pDoc,
		ARBDogTrialPtr const& inTrial,
		ARBDogRunPtr const& inRun,
		wxWindow* pParent)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_Dogs(nullptr)
	, m_Trial(inTrial)
	, m_Run(inRun)
	, m_ctrlList(nullptr)
	, m_ctrlTree(nullptr)
	, m_ctrlUp(nullptr)
	, m_ctrlDown(nullptr)
{
	Init(pParent);
}


void CDlgReorder::Init(wxWindow* pParent)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_REORDER"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	wxWindow* ctrl = nullptr;
	if (m_Dogs)
	{
		ctrl = m_ctrlList = new wxListBox(this, wxID_ANY,
			wxDefaultPosition, wxDLG_UNIT(this, wxSize(85, 50)),
			0, nullptr,
			wxLB_NEEDED_SB|wxLB_SINGLE);
		BIND_OR_CONNECT_CTRL(m_ctrlList, wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler, CDlgReorder::OnListSelected);
		m_ctrlList->SetHelpText(_("HIDC_REORDER_LIST"));
		m_ctrlList->SetToolTip(_("HIDC_REORDER_LIST"));
	}
	else if (m_Trial)
	{
		ctrl = m_ctrlTree = new CTreeCtrl(this, wxID_ANY,
			wxDefaultPosition, wxDLG_UNIT(this, wxSize(145, -1)),
			wxTR_FULL_ROW_HIGHLIGHT|wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|wxTR_NO_LINES|wxTR_SINGLE);
		m_ctrlTree->SetQuickBestSize(false);
		BIND_OR_CONNECT_CTRL(m_ctrlTree, wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler, CDlgReorder::OnTreeSelected);
		m_ctrlTree->SetHelpText(_("HIDC_REORDER_LIST"));
		m_ctrlTree->SetToolTip(_("HIDC_REORDER_LIST"));
	}

	wxButton* btnOk = new wxButton(this, wxID_OK);

	wxButton* btnCancel = new wxButton(this, wxID_CANCEL);

	m_ctrlUp = new wxButton(this, wxID_ANY,
		_("IDC_REORDER_MOVE_UP"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlUp, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgReorder::OnMoveUp);
	m_ctrlUp->SetHelpText(_("HIDC_REORDER_MOVE_UP"));
	m_ctrlUp->SetToolTip(_("HIDC_REORDER_MOVE_UP"));

	m_ctrlDown = new wxButton(this, wxID_ANY,
		_("IDC_REORDER_MOVE_DOWN"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlDown, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgReorder::OnMoveDown);
	m_ctrlDown->SetHelpText(_("HIDC_REORDER_MOVE_DOWN"));
	m_ctrlDown->SetToolTip(_("HIDC_REORDER_MOVE_DOWN"));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizerList = new wxBoxSizer(wxHORIZONTAL);
	if (m_ctrlList)
		bSizerList->Add(m_ctrlList, 1, wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));
	else if (m_ctrlTree)
		bSizerList->Add(m_ctrlTree, 1, wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* bSizerSide = new wxBoxSizer(wxVERTICAL);
	bSizerSide->Add(btnOk, 0, 0, 0);
	bSizerSide->Add(btnCancel, 0, wxTOP, wxDLG_UNIT_X(this, 3));
	bSizerSide->Add(0, 0, 1, wxEXPAND, 0);
	bSizerSide->Add(m_ctrlUp, 0, wxTOP, wxDLG_UNIT_X(this, 3));
	bSizerSide->Add(m_ctrlDown, 0, wxTOP, wxDLG_UNIT_X(this, 3));

	bSizerList->Add(bSizerSide, 0, wxEXPAND, 0);

	bSizer->Add(bSizerList, 1, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	if (m_Dogs && m_ctrlList)
	{
		for (ARBDogList::iterator iter = m_Dogs->begin();
			iter != m_Dogs->end();
			++iter)
		{
			m_ctrlList->Append(StringUtil::stringWX((*iter)->GetGenericName()), new CReorderListData((*iter)));
		}
	}
	else if (m_Trial && m_ctrlTree)
	{
		wxTreeItemId idSelect;
		wxTreeItemId root = m_ctrlTree->AddRoot(L"Root");
		std::map<ARBDate, wxTreeItemId> dates;
		for (ARBDogRunList::iterator iter = m_Trial->GetRuns().begin();
			iter != m_Trial->GetRuns().end();
			++iter)
		{
			ARBDogRunPtr pRun = *iter;
			if (dates.end() == dates.find(pRun->GetDate()))
				dates[pRun->GetDate()] = m_ctrlTree->AppendItem(root, StringUtil::stringWX(pRun->GetDate().GetString()));
			wxTreeItemId item = m_ctrlTree->AppendItem(dates[pRun->GetDate()], StringUtil::stringWX(pRun->GetName()), -1, 1, new CReorderTreeData(pRun));
			if (m_Run == pRun || (!m_Run && !idSelect.IsOk()))
				idSelect = item;
		}
		m_ctrlTree->ExpandAll();
		if (idSelect.IsOk())
		{
			m_ctrlTree->EnsureVisible(idSelect);
			m_ctrlTree->SelectItem(idSelect);
		}
	}
	UpdateControls();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);

	// Recompute layout so the tree is fully visible.
	if (m_Trial && m_ctrlTree)
	{
		if (m_ctrlTree->ComputeHeightHint())
			GetSizer()->Fit(this);
	}

	CenterOnParent();

	if (ctrl)
		ctrl->SetFocus();
}



void CDlgReorder::UpdateControls()
{
	bool bEnableUp = false;
	bool bEnableDown = false;
	if (m_ctrlList)
	{
		int index = m_ctrlList->GetSelection();
		if (0 <= index)
		{
			bEnableUp = bEnableDown = true;
			if (0 == index)
				bEnableUp = false;
			if (index == static_cast<int>(m_ctrlList->GetCount()) - 1)
				bEnableDown = false;
		}
	}
	else if (m_ctrlTree)
	{
		wxTreeItemId item = m_ctrlTree->GetSelection();
		if (item.IsOk() && m_ctrlTree->GetItemData(item))
		{
			bEnableUp = m_ctrlTree->GetPrevSibling(item).IsOk();
			bEnableDown = m_ctrlTree->GetNextSibling(item).IsOk();
		}
	}
	m_ctrlUp->Enable(bEnableUp);
	m_ctrlDown->Enable(bEnableDown);
}


void CDlgReorder::OnListSelected(wxCommandEvent& /*evt*/)
{
	UpdateControls();
}


void CDlgReorder::OnTreeSelected(wxTreeEvent& /*evt*/)
{
	UpdateControls();
}


void CDlgReorder::OnMoveUp(wxCommandEvent& evt)
{
	if (m_ctrlList)
	{
		int index = m_ctrlList->GetSelection();
		if (wxNOT_FOUND != index)
		{
			int newIndex = index - 1;
			if (0 <= newIndex)
			{
				wxString str1 = m_ctrlList->GetString(index);
				CReorderListData* pData1 = dynamic_cast<CReorderListData*>(m_ctrlList->GetClientObject(index));
				wxString str2 = m_ctrlList->GetString(newIndex);
				CReorderListData* pData2 = dynamic_cast<CReorderListData*>(m_ctrlList->GetClientObject(newIndex));
				m_ctrlList->SetString(index, str2);
				m_ctrlList->SetString(newIndex, str1);
				ARBDogPtr pTmp = pData1->m_pDog;
				pData1->m_pDog = pData2->m_pDog;
				pData2->m_pDog = pTmp;
				m_ctrlList->SetSelection(newIndex);
				UpdateControls();
			}
		}
	}
	else if (m_ctrlTree)
	{
		wxTreeItemId item = m_ctrlTree->GetSelection();
		if (item.IsOk() && m_ctrlTree->GetItemData(item))
		{
			wxTreeItemId itemPrev = m_ctrlTree->GetPrevSibling(item);
			if (itemPrev.IsOk())
			{
				CReorderTreeData* pData1 = dynamic_cast<CReorderTreeData*>(m_ctrlTree->GetItemData(item));
				CReorderTreeData* pData2 = dynamic_cast<CReorderTreeData*>(m_ctrlTree->GetItemData(itemPrev));
				if (pData1 && pData2)
				{
					wxString str1 = m_ctrlTree->GetItemText(item);
					wxString str2 = m_ctrlTree->GetItemText(itemPrev);
					m_ctrlTree->SetItemText(item, str2);
					m_ctrlTree->SetItemText(itemPrev, str1);
					ARBDogRunPtr pTmp = pData1->m_pRun;
					pData1->m_pRun = pData2->m_pRun;
					pData2->m_pRun = pTmp;
				}
				m_ctrlTree->SelectItem(itemPrev);
				UpdateControls();
			}
		}
	}
}


void CDlgReorder::OnMoveDown(wxCommandEvent& evt)
{
	if (m_ctrlList)
	{
		int index = m_ctrlList->GetSelection();
		if (wxNOT_FOUND != index)
		{
			int newIndex = index + 1;
			if (newIndex < static_cast<int>(m_ctrlList->GetCount()))
			{
				wxString str1 = m_ctrlList->GetString(index);
				CReorderListData* pData1 = dynamic_cast<CReorderListData*>(m_ctrlList->GetClientObject(index));
				wxString str2 = m_ctrlList->GetString(newIndex);
				CReorderListData* pData2 = dynamic_cast<CReorderListData*>(m_ctrlList->GetClientObject(newIndex));
				m_ctrlList->SetString(index, str2);
				m_ctrlList->SetString(newIndex, str1);
				ARBDogPtr pTmp = pData1->m_pDog;
				pData1->m_pDog = pData2->m_pDog;
				pData2->m_pDog = pTmp;
				m_ctrlList->SetSelection(newIndex);
				UpdateControls();
			}
		}
	}
	else if (m_ctrlTree)
	{
		wxTreeItemId item = m_ctrlTree->GetSelection();
		if (item.IsOk() && m_ctrlTree->GetItemData(item))
		{
			wxTreeItemId itemNext = m_ctrlTree->GetNextSibling(item);
			if (itemNext.IsOk())
			{
				CReorderTreeData* pData1 = dynamic_cast<CReorderTreeData*>(m_ctrlTree->GetItemData(item));
				CReorderTreeData* pData2 = dynamic_cast<CReorderTreeData*>(m_ctrlTree->GetItemData(itemNext));
				if (pData1 && pData2)
				{
					wxString str1 = m_ctrlTree->GetItemText(item);
					wxString str2 = m_ctrlTree->GetItemText(itemNext);
					m_ctrlTree->SetItemText(item, str2);
					m_ctrlTree->SetItemText(itemNext, str1);
					ARBDogRunPtr pTmp = pData1->m_pRun;
					pData1->m_pRun = pData2->m_pRun;
					pData2->m_pRun = pTmp;
				}
				m_ctrlTree->SelectItem(itemNext);
				UpdateControls();
			}
		}
	}
}


void CDlgReorder::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	if (m_Dogs && m_ctrlList)
	{
		m_Dogs->clear();
		for (unsigned int idx = 0; idx < m_ctrlList->GetCount(); ++idx)
		{
			CReorderListData* pData = dynamic_cast<CReorderListData*>(m_ctrlList->GetClientObject(idx));
			m_Dogs->AddDog(pData->m_pDog);
		}
		m_pDoc->Modify(true);
		CUpdateHint hint(UPDATE_TREE_VIEW | UPDATE_RUNS_VIEW);
		m_pDoc->UpdateAllViews(nullptr, &hint);
	}
	else if (m_Trial && m_ctrlTree)
	{
		m_Trial->GetRuns().clear();
		wxTreeItemIdValue cookie1;
		for (wxTreeItemId itemDate = m_ctrlTree->GetFirstChild(m_ctrlTree->GetRootItem(), cookie1);
			itemDate.IsOk();
			itemDate = m_ctrlTree->GetNextSibling(itemDate))
		{
			wxTreeItemIdValue cookie2;
			for (wxTreeItemId itemRun = m_ctrlTree->GetFirstChild(itemDate, cookie2);
				itemRun.IsOk();
				itemRun = m_ctrlTree->GetNextSibling(itemRun))
			{
				CReorderTreeData* pData = dynamic_cast<CReorderTreeData*>(m_ctrlTree->GetItemData(itemRun));
				m_Trial->GetRuns().AddRun(pData->m_pRun);
			}
		}
		m_Trial->GetRuns().sort();
		m_pDoc->Modify(true);
		CUpdateHint hint(UPDATE_TREE_VIEW | UPDATE_RUNS_VIEW);
		m_pDoc->UpdateAllViews(nullptr, &hint);
	}

	EndDialog(wxID_OK);
}
