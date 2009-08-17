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
 * @brief implementation of the CAgilityBookTree class
 * @author David Connet
 *
 * Revision History
 * @li 2009-08-17 DRC Fix r-click selection issues.
 * @li 2009-07-13 DRC Changing language didn't update dog's age.
 * @li 2009-07-11 DRC Change how runs are synced with list to reduce reloading.
 * @li 2009-02-08 DRC Ported to wxWidgets.
 * @li 2008-11-19 DRC Added SelectDog()
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-22 DRC Support pasting a dog when no dogs exist in file.
 * @li 2005-08-31 DRC The wrong item in the tree was selected after reordering.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2004-10-04 DRC Added div-by-0 tests.
 * @li 2004-04-15 DRC Added Duplicate menu item.
 * @li 2003-12-30 DRC Implemented customized text in tree.
 * @li 2003-12-28 DRC Implemented Find/FindNext.
 * @li 2003-08-30 DRC Added GetPrintLine to allow future differences between
 *                    printing and viewing (already in the listctrl)
 * @li 2003-08-28 DRC Added printing.
 * @li 2003-08-27 DRC Cleaned up selection synchronization.
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 */

#include "stdafx.h"
#include "AgilityBookTreeView.h"

#include "AgilityBook.h"
#include "AgilityBookOptions.h"
#include "AgilityBookPanels.h"
#include "AgilityBookRunsView.h"
#include "AgilityBookTreeData.h"
#include "ARBDog.h"
#include "ARBDogRun.h"
#include "ARBDogTrial.h"
#include "ClipBoard.h"
#include "DlgAssignColumns.h"
#include "DlgDog.h"
#include "DlgFind.h"
#include "Element.h"
#include "FilterOptions.h"
#include "Globals.h"
#include "MainFrm.h"
#include "Print.h"
#include <wx/config.h>

#ifdef WIN32
#include "res/CalEmpty.xpm"
#include "res/CalPlan.xpm"
#endif


/////////////////////////////////////////////////////////////////////////////
// Find

void CFindTree::FillTree(wxTreeItemId hItem) const
{
	if (!hItem.IsOk())
		return;

	if (m_pView->m_Ctrl->GetRootItem() != hItem)
		m_Items.push_back(hItem);

	wxTreeItemIdValue cookie;
	wxTreeItemId hChildItem = m_pView->m_Ctrl->GetFirstChild(hItem, cookie);
	while (hChildItem.IsOk())
	{
		FillTree(hChildItem);
		hChildItem = m_pView->m_Ctrl->GetNextChild(hItem, cookie);
	}
}


wxTreeItemId CFindTree::GetNextItem() const
{
	wxTreeItemId hItem;
	if (SearchDown())
	{
		++m_Iter;
		if (m_Iter == m_Items.end())
			hItem = NULL;
		else
			hItem = *m_Iter;
	}
	else
	{
		if (m_Iter == m_Items.begin())
			hItem = NULL;
		else
		{
			--m_Iter;
			hItem = *m_Iter;
		}
	}
	return hItem;
}


bool CFindTree::Search(CDlgFind* pDlg) const
{
	bool bFound = false;
	m_Items.clear();
	FillTree(m_pView->m_Ctrl->GetRootItem());
	if (0 == m_Items.size())
		return bFound;

	wxTreeItemId hItem = m_pView->m_Ctrl->GetSelection();
	if (!hItem.IsOk())
	{
		if (SearchDown())
			m_Iter = m_Items.begin();
		else
		{
			m_Iter = m_Items.end();
			--m_Iter;
		}
	}
	else
	{
		for (m_Iter = m_Items.begin(); m_Iter != m_Items.end(); ++m_Iter)
		{
			if (*m_Iter == hItem)
				break;
		}
		hItem = GetNextItem();
	}
	wxString search = Text();
	if (!MatchCase())
		search.MakeLower();
	while (!bFound && hItem.IsOk())
	{
		std::set<tstring> strings;
		if (SearchAll())
		{
			CAgilityBookTreeData* pData = m_pView->GetTreeItem(hItem);
			if (pData)
				pData->GetARBBase()->GetSearchStrings(strings);
		}
		else
		{
			strings.insert(m_pView->m_Ctrl->GetItemText(hItem).c_str());
		}
		for (std::set<tstring>::iterator iter = strings.begin(); iter != strings.end(); ++iter)
		{
			wxString str((*iter).c_str());
			if (!MatchCase())
				str.MakeLower();
			if (0 <= str.Find(search))
			{
				m_pView->ChangeSelection(hItem);
				m_pView->m_Ctrl->EnsureVisible(hItem);
				bFound = true;
			}
		}
		hItem = GetNextItem();
	}
	if (!bFound)
	{
		wxString msg = wxString::Format(_("IDS_CANNOT_FIND"), m_strSearch.c_str());
		wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
	}
	return bFound;
}

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(CAgilityBookTreeView, CAgilityBookBaseExtraView)


BEGIN_EVENT_TABLE(CAgilityBookTreeView, CAgilityBookBaseExtraView)
	EVT_CONTEXT_MENU(CAgilityBookTreeView::OnViewContextMenu)
	EVT_UPDATE_UI(wxID_DUPLICATE, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(wxID_DUPLICATE, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(wxID_CUT, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(wxID_CUT, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(wxID_COPY, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(wxID_COPY, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(wxID_PASTE, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(wxID_PASTE, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(wxID_SELECTALL, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(wxID_SELECTALL, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_REORDER, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_REORDER, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(wxID_FIND, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(wxID_FIND, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_EDIT_FIND_NEXT, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_EDIT_FIND_NEXT, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_EDIT_FIND_PREVIOUS, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_EDIT_FIND_PREVIOUS, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_EDIT_DOG, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_EDIT_DOG, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_NEW_DOG, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_NEW_DOG, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_DELETE_DOG, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_DELETE_DOG, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_NEW_TITLE, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_NEW_TITLE, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_EDIT_TRIAL, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_EDIT_TRIAL, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_NEW_TRIAL, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_NEW_TRIAL, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_DELETE_TRIAL, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_DELETE_TRIAL, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_PRINT_TRIAL, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_PRINT_TRIAL, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_EDIT_RUN, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_EDIT_RUN, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_NEW_RUN, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_NEW_RUN, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_DELETE_RUN, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_DELETE_RUN, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_PRINT_RUNS, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_PRINT_RUNS, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_CUSTOMIZE, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_CUSTOMIZE, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_SORTRUNS, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_SORTRUNS, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_RUNS_BY_TRIAL, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_RUNS_BY_TRIAL, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_TABLE_IN_YPS, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_TABLE_IN_YPS, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_EXPAND, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_EXPAND, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_COLLAPSE, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_COLLAPSE, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_EXPAND_ALL, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_EXPAND_ALL, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_COLLAPSE_ALL, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_COLLAPSE_ALL, CAgilityBookTreeView::OnViewCmd)
	EVT_MENU(wxID_PRINT, CAgilityBookTreeView::OnPrint)
	EVT_MENU(wxID_PREVIEW, CAgilityBookTreeView::OnPreview)
END_EVENT_TABLE()


#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4355)
#endif
CAgilityBookTreeView::CAgilityBookTreeView(
		CTabView* pTabView,
		wxDocument* doc)
	: CAgilityBookBaseExtraView(pTabView, doc)
	, m_Ctrl(NULL)
	, m_ImageList()
#ifdef WIN32
	, m_ImageListStates(16,16)
	, m_idxEmpty(-1)
	, m_idxChecked(-1)
#endif
	, m_Columns()
	, m_bReset(false)
	, m_bSuppressSelect(false)
	, m_Callback(this)
	, m_pDog()
{
#ifdef WIN32
	// Note: Position 0 cannot be used.
	m_ImageListStates.Add(wxIcon(CalEmpty_xpm));
	m_idxEmpty = m_ImageListStates.Add(wxIcon(CalEmpty_xpm));
	m_idxChecked = m_ImageListStates.Add(wxIcon(CalPlan_xpm));
#endif

	CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_DOG, m_Columns[0]);
	CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_TRIAL, m_Columns[1]);
	CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_RUN, m_Columns[2]);
}
#ifdef _MSC_VER
#pragma warning (pop)
#endif


CAgilityBookTreeView::~CAgilityBookTreeView()
{
}


bool CAgilityBookTreeView::Create(
		CBasePanel* parentView,
		wxWindow* parentCtrl,
		wxDocument* doc,
		long flags,
		wxSizer* sizer,
		int proportion,
		int sizerFlags,
		int border)
{
	m_Ctrl = new wxTreeCtrl(parentCtrl, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxTR_SINGLE|wxTR_FULL_ROW_HIGHLIGHT|wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|wxNO_BORDER, wxDefaultValidator);
	m_Ctrl->Connect(wxEVT_COMMAND_SET_FOCUS, wxFocusEventHandler(CAgilityBookTreeView::OnCtrlSetFocus), NULL, this);
	m_Ctrl->Connect(wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler(CAgilityBookTreeView::OnCtrlContextMenu), NULL, this);
	m_Ctrl->Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(CAgilityBookTreeView::OnCtrlSelectionChanged), NULL, this);
	m_Ctrl->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(CAgilityBookTreeView::OnCtrlItemActivated), NULL, this);
	m_Ctrl->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(CAgilityBookTreeView::OnCtrlKeyDown), NULL, this);
	m_Ctrl->SetImageList(&m_ImageList);
#ifdef WIN32
	m_Ctrl->SetStateImageList(&m_ImageListStates);
#endif
	return CAgilityBookBaseExtraView::Create(parentView, parentCtrl, doc, flags, sizer, proportion, sizerFlags, border);
}


bool CAgilityBookTreeView::PrevPane()
{
	if (m_parentPanel)
		return m_parentPanel->ActivateView(1);
	return false;
}


bool CAgilityBookTreeView::NextPane()
{
	if (m_parentPanel)
		return m_parentPanel->ActivateView(1);
	return false;
}


void CAgilityBookTreeView::DetachView()
{
	// The control is actually owned by the panel, the view is not.
	m_Ctrl = NULL;
}


bool CAgilityBookTreeView::OnCreate(
		wxDocument* doc,
		long flags)
{
	return true;
}


void CAgilityBookTreeView::DoActivateView(
		bool activate,
		wxView* activeView,
		wxView* deactiveView)
{
	if (m_Ctrl && activate && wxWindow::DoFindFocus() != m_Ctrl)
		m_Ctrl->SetFocus();
}


void CAgilityBookTreeView::OnDraw(wxDC* dc)
{
}


void CAgilityBookTreeView::OnUpdate(
		wxView* sender,
		wxObject* inHint)
{
	if (!m_Ctrl)
		return;
	CUpdateHint* hint = NULL;
	if (inHint)
		hint = reinterpret_cast<CUpdateHint*>(inHint);
	if (!hint || hint->IsSet(UPDATE_TREE_VIEW)
	|| hint->IsEqual(UPDATE_CONFIG) || hint->IsEqual(UPDATE_OPTIONS))
	{
		LoadData();
	}
	else if (hint && hint->IsEqual(UPDATE_NEW_TRIAL))
	{
		LoadData();
		ARBDogTrialPtr pTrial = tr1::dynamic_pointer_cast<ARBDogTrial, ARBBase>(hint->GetObj());
		assert(pTrial);
		CAgilityBookTreeData* pData = FindData(pTrial);
		assert(pData);
		ChangeSelection(pData->GetId());
		m_Ctrl->EnsureVisible(pData->GetId());
	}
	else if (hint && hint->IsEqual(UPDATE_CUSTOMIZE))
	{
		CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_DOG, m_Columns[0]);
		CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_TRIAL, m_Columns[1]);
		CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_RUN, m_Columns[2]);
		UpdateData(m_Ctrl->GetRootItem());
	}
	else if (hint && hint->IsEqual(UPDATE_LANG_CHANGE))
	{
		RefreshTreeItem(m_Ctrl, m_Ctrl->GetRootItem(), true);
	}
}


bool CAgilityBookTreeView::IsFiltered() const
{
	return CFilterOptions::Options().IsFilterEnabled();
}


bool CAgilityBookTreeView::GetMessage(wxString& msg) const
{
	CAgilityBookRunsView* pView = GetDocument()->GetRunsView();
	if (pView)
		return pView->GetMessage(msg);
	msg.Empty();
	return false;
}


bool CAgilityBookTreeView::GetMessage2(wxString& msg) const
{
	if (GetDocument()->GetCurrentDog())
	{
		msg = GetDocument()->GetCurrentDog()->GetCallName().c_str();
		return true;
	}
	else
	{
		msg.Empty();
		return false;
	}
}


bool CAgilityBookTreeView::AllowStatusContext(int field) const
{
	return STATUS_DOG == field || STATUS_FILTERED == field;
}


void CAgilityBookTreeView::RefreshItem(
		wxTreeItemId item,
		bool bRecurse)
{
	RefreshTreeItem(m_Ctrl, item, bRecurse);
}


CAgilityBookTreeData* CAgilityBookTreeView::GetCurrentTreeItem() const
{
	if (m_Ctrl && m_Ctrl->GetSelection().IsOk())
		return GetTreeItem(m_Ctrl->GetSelection());
	return NULL;
}


CAgilityBookTreeData* CAgilityBookTreeView::GetTreeItem(wxTreeItemId hItem) const
{
	if (m_Ctrl && hItem.IsOk())
		return dynamic_cast<CAgilityBookTreeData*>(m_Ctrl->GetItemData(hItem));
	return NULL;
}


bool CAgilityBookTreeView::SelectDog(ARBDogPtr dog)
{
	bool bSelected = false;
	if (m_Ctrl)
	{
		CAgilityBookTreeData* pData = FindData(m_Ctrl->GetRootItem(), dog);
		if (pData)
		{
			bSelected = true;
			m_Ctrl->SelectItem(pData->GetId());
			m_Ctrl->EnsureVisible(pData->GetId());
		}
	}
	return bSelected;
}


CAgilityBookTreeData* CAgilityBookTreeView::FindData(
		wxTreeItemId hItem,
		ARBBasePtr pBase) const
{
	if (!pBase || !m_Ctrl)
		return NULL;
	CAgilityBookTreeData* pData = NULL;
	if (m_Ctrl->GetRootItem() != hItem)
	{
		CAgilityBookTreeData* pCheck = dynamic_cast<CAgilityBookTreeData*>(m_Ctrl->GetItemData(hItem));
		if (pCheck && pCheck->GetARBBase() == pBase)
			pData = pCheck;
	}
	if (!pData)
	{
		wxTreeItemIdValue cookie;
		wxTreeItemId hChildItem = m_Ctrl->GetFirstChild(hItem, cookie);
		while (hChildItem.IsOk() && !pData)
		{
			pData = FindData(hChildItem, pBase);
			hChildItem = m_Ctrl->GetNextChild(hItem, cookie);
		}
	}
	return pData;
}


CAgilityBookTreeData* CAgilityBookTreeView::FindData(
		wxTreeItemId hItem,
		ARBDogPtr pDog) const
{
	if (!pDog || !m_Ctrl)
		return NULL;
	CAgilityBookTreeData* pData = NULL;
	if (m_Ctrl->GetRootItem() != hItem)
	{
		CAgilityBookTreeDataDog* pCheck = dynamic_cast<CAgilityBookTreeDataDog*>(m_Ctrl->GetItemData(hItem));
		// Don't use virtual 'GetDog' on pCheck as that can give back a parent
		// node which isn't what we want here. We want the real dog node.
		if (pCheck && pCheck->GetDog() == pDog)
			pData = pCheck;
	}
	if (!pData)
	{
		wxTreeItemIdValue cookie;
		wxTreeItemId hChildItem = m_Ctrl->GetFirstChild(hItem, cookie);
		while (hChildItem.IsOk() && !pData)
		{
			pData = FindData(hChildItem, pDog);
			hChildItem = m_Ctrl->GetNextChild(hItem, cookie);
		}
	}
	return pData;
}


CAgilityBookTreeData* CAgilityBookTreeView::FindData(
		wxTreeItemId hItem,
		ARBDogTrialPtr pTrial) const
{
	if (!pTrial || !m_Ctrl)
		return NULL;
	CAgilityBookTreeData* pData = NULL;
	if (m_Ctrl->GetRootItem() != hItem)
	{
		CAgilityBookTreeDataTrial* pCheck = dynamic_cast<CAgilityBookTreeDataTrial*>(m_Ctrl->GetItemData(hItem));
		if (pCheck && pCheck->GetTrial() == pTrial)
			pData = pCheck;
	}
	if (!pData)
	{
		wxTreeItemIdValue cookie;
		wxTreeItemId hChildItem = m_Ctrl->GetFirstChild(hItem, cookie);
		while (hChildItem.IsOk() && !pData)
		{
			pData = FindData(hChildItem, pTrial);
			hChildItem = m_Ctrl->GetNextChild(hItem, cookie);
		}
	}
	return pData;
}


CAgilityBookTreeData* CAgilityBookTreeView::FindData(
		wxTreeItemId hItem,
		ARBDogRunPtr pRun) const
{
	if (!pRun || !m_Ctrl)
		return NULL;
	CAgilityBookTreeData* pData = NULL;
	if (m_Ctrl->GetRootItem() != hItem)
	{
		CAgilityBookTreeDataRun* pCheck = dynamic_cast<CAgilityBookTreeDataRun*>(m_Ctrl->GetItemData(hItem));
		if (pCheck && pCheck->GetRun() == pRun)
			pData = pCheck;
	}
	if (!pData)
	{
		wxTreeItemIdValue cookie;
		wxTreeItemId hChildItem = m_Ctrl->GetFirstChild(hItem, cookie);
		while (hChildItem.IsOk() && !pData)
		{
			pData = FindData(hChildItem, pRun);
			hChildItem = m_Ctrl->GetNextChild(hItem, cookie);
		}
	}
	return pData;
}


wxTreeItemId CAgilityBookTreeView::InsertDog(
		ARBDogPtr pDog,
		bool bSelect)
{
	wxTreeItemId hItem;
	if (pDog && m_Ctrl)
	{
		CAgilityBookTreeDataDog* pDataDog = new CAgilityBookTreeDataDog(this, pDog);
		int idxImage = pDataDog->OnNeedIcon();
		hItem = m_Ctrl->AppendItem(
			m_Ctrl->GetRootItem(),
			pDataDog->OnNeedText(),
			idxImage, idxImage,
			pDataDog);
		for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
		{
			InsertTrial((*iterTrial), hItem);
		}
		if (bSelect)
		{
			ChangeSelection(hItem);
			m_Ctrl->EnsureVisible(hItem);
		}
	}
	return hItem;
}


wxTreeItemId CAgilityBookTreeView::InsertTrial(
		ARBDogTrialPtr pTrial,
		wxTreeItemId hParent)
{
	wxTreeItemId hTrial;
	if (pTrial && !pTrial->IsFiltered() && m_Ctrl)
	{
		CAgilityBookTreeDataTrial* pDataTrial = new CAgilityBookTreeDataTrial(this, pTrial);
		int idxImage = pDataTrial->OnNeedIcon();
		hTrial = m_Ctrl->AppendItem(
			hParent,
			pDataTrial->OnNeedText(),
			idxImage, idxImage,
			pDataTrial);
#ifdef WIN32
		int state = pDataTrial->GetTrial()->IsVerified() ? m_idxChecked : m_idxEmpty;
		m_Ctrl->SetState(hTrial, state);
#endif
		for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
			iterRun != pTrial->GetRuns().end();
			++iterRun)
		{
			InsertRun(pTrial, (*iterRun), hTrial);
		}
	}
	return hTrial;
}


wxTreeItemId CAgilityBookTreeView::InsertRun(
		ARBDogTrialPtr pTrial,
		ARBDogRunPtr pRun,
		wxTreeItemId hParent)
{
	wxTreeItemId hRun;
	if (pRun && !pRun->IsFiltered() && m_Ctrl)
	{
		CAgilityBookTreeDataRun* pDataRun = new CAgilityBookTreeDataRun(this, pRun);
		int idxImage = pDataRun->OnNeedIcon();
		hRun = m_Ctrl->AppendItem(
			hParent,
			pDataRun->OnNeedText(),
			idxImage, idxImage,
			pDataRun);
	}
	return hRun;
}


bool CAgilityBookTreeView::PasteDog(bool& bLoaded)
{
	if (!m_Ctrl)
		return false;
	ElementNodePtr tree(ElementNode::New());
	CClipboardDataReader clpData;
	if (clpData.GetData(eFormatDog, tree))
	{
		if (CLIPDATA == tree->GetName())
		{
			ARBDogPtr pDog(ARBDog::New());
			if (pDog)
			{
				CErrorCallback err;
				if (pDog->Load(GetDocument()->Book().GetConfig(), tree->GetNthElementNode(0), ARBAgilityRecordBook::GetCurrentDocVersion(), err))
				{
					bLoaded = true;
					pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
					std::vector<CVenueFilter> venues;
					CFilterOptions::Options().GetFilterVenue(venues);
					if (!GetDocument()->Book().GetDogs().AddDog(pDog))
					{
						bLoaded = false;
						wxMessageBox(_("IDS_CREATEDOG_FAILED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
					}
					else
					{
						GetDocument()->Modify(true);
						GetDocument()->ResetVisibility(venues, pDog);
						m_Ctrl->Freeze();
						InsertDog(pDog, true);
						m_Ctrl->Thaw();
						m_Ctrl->Refresh();
						CUpdateHint hint(UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW | UPDATE_TREE_VIEW);
						GetDocument()->UpdateAllViews(NULL, &hint);
					}
				}
				else if (0 < err.m_ErrMsg.length())
					wxMessageBox(err.m_ErrMsg.c_str(), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
			}
		}
		return true;
	}
	return false;
}


wxString CAgilityBookTreeView::GetPrintLine(wxTreeItemId hItem) const
{
	if (hItem.IsOk() && m_Ctrl)
		return m_Ctrl->GetItemText(hItem);
	return wxT("");
}


void CAgilityBookTreeView::UpdateData(wxTreeItemId hItem)
{
	if (m_Ctrl && m_Ctrl->GetRootItem() != hItem)
	{
		CAgilityBookTreeData* pData = dynamic_cast<CAgilityBookTreeData*>(m_Ctrl->GetItemData(hItem));
		if (pData && pData->GetId().IsOk())
		{
			m_Ctrl->SetItemText(pData->GetId(), pData->OnNeedText());
		}
	}
	wxTreeItemIdValue cookie;
	wxTreeItemId hChildItem = m_Ctrl->GetFirstChild(hItem, cookie);
	while (hChildItem.IsOk())
	{
		UpdateData(hChildItem);
		hChildItem = m_Ctrl->GetNextChild(hItem, cookie);
	}
}


void CAgilityBookTreeView::ChangeSelection(wxTreeItemId hItem)
{
	m_Ctrl->SelectItem(hItem);
	DoSelectionChange(hItem);
}


void CAgilityBookTreeView::DoSelectionChange(wxTreeItemId hItem)
{
	if (!m_bSuppressSelect)
	{
		CAgilityBookTreeData* pData = GetTreeItem(hItem);
		ARBBasePtr pBase;
		unsigned int iHint = 0;
		if (pData)
		{
			// Set the current dog
			ARBDogPtr pDog = pData->GetDog();
			if (!m_pDog || !pDog || m_pDog != pDog)
				iHint |= UPDATE_POINTS_VIEW;
			m_pDog = pDog;
			CAgilityBookRunsView* pView = GetDocument()->GetRunsView();
			if (CAgilityBookOptions::GetViewRunsByTrial()
			&& !pView->IsTrial(pData->GetTrial()))
			{
				iHint |= UPDATE_RUNS_VIEW;
			}
			// Pass the selected run
			else if (CAgilityBookTreeData::eTreeRun == pData->GetType())
			{
				pBase = pData->GetRun();
				iHint |= UPDATE_RUNS_SELECTION_VIEW;
			}
		}
		else
			m_pDog.reset();
		if (iHint)
		{
			CUpdateHint hint(iHint, pBase);
			GetDocument()->UpdateAllViews(this, &hint);
		}
	}
}


void CAgilityBookTreeView::LoadData()
{
	if (!m_Ctrl)
		return;

	m_bSuppressSelect = true;

	m_Ctrl->Freeze();

	wxBusyCursor wait;
	// Remember the currently selected item.
	tstring strCallName;
	CAgilityBookTreeData const* pData = GetCurrentTreeItem();
	if (!pData)
	{
		strCallName = wxConfig::Get()->Read(wxT("Settings/LastDog"), wxString());
	}
	std::list<ARBBasePtr> baseItems;
	while (pData)
	{
		baseItems.push_front(pData->GetARBBase());
		pData = pData->GetParent();
	}

	m_Ctrl->DeleteAllItems();

	// Load the data
	m_Ctrl->AddRoot(wxT("Root"));
	wxTreeItemId hItem = NULL;
	for (ARBDogList::const_iterator iterDog = GetDocument()->Book().GetDogs().begin();
		iterDog != GetDocument()->Book().GetDogs().end();
		++iterDog)
	{
		wxTreeItemId hItem2 = InsertDog((*iterDog));
		if (0 < strCallName.length() && (*iterDog)->GetCallName() == strCallName)
			hItem = hItem2;
	}
	if (!hItem)
	{
		// Work thru all the base items...
		// Otherwise, after a reorder, the wrong item is selected.
		hItem = m_Ctrl->GetRootItem();
		for (std::list<ARBBasePtr>::iterator iter = baseItems.begin();
			iter != baseItems.end();
			++iter)
		{
			pData = FindData(hItem, (*iter));
			if (pData)
				hItem = pData->GetId();
		}
		if (hItem == m_Ctrl->GetRootItem())
			hItem = wxTreeItemId();
	}
	if (!hItem.IsOk())
	{
		wxTreeItemIdValue cookie;
		hItem = m_Ctrl->GetFirstChild(m_Ctrl->GetRootItem(), cookie);
	}
	m_Ctrl->SelectItem(hItem);
	m_Ctrl->EnsureVisible(hItem);
	m_Ctrl->Expand(hItem);
	// Reset the context menu reset.
	// We may have reloaded during a context menu operation.
	m_bReset = false;

	if (m_Ctrl->IsShownOnScreen())
		UpdateMessages();

	m_Ctrl->Thaw();
	m_Ctrl->Refresh();
	m_bSuppressSelect = false;
}


void CAgilityBookTreeView::PrintLine(
		otstringstream& data,
		wxTreeItemId id,
		int indent) const
{
	static wxChar const* const spaces = wxT("&nbsp;&nbsp;&nbsp;");
	if (id.IsOk() && id != m_Ctrl->GetRootItem())
	{
		for (int idx = 0; idx < indent; ++idx)
			data << spaces;
		data << m_Ctrl->GetItemText(id).c_str() << wxT("<br />\n"); // Note, wxWidgets needs the space before the slash
	}
	wxTreeItemIdValue cookie;
	wxTreeItemId hChildItem = m_Ctrl->GetFirstChild(id, cookie);
	while (hChildItem.IsOk())
	{
		PrintLine(data, hChildItem, indent + 1);
		hChildItem = m_Ctrl->GetNextChild(id, cookie);
	}
}


wxString CAgilityBookTreeView::GetPrintDataAsHtmlTable() const
{
	otstringstream data;
	data << wxT("<html><body><p>\n");
	PrintLine(data, m_Ctrl->GetRootItem(), -1);
	data << wxT("</p></body></html>\n");
	return data.str().c_str();
}


void CAgilityBookTreeView::OnCtrlSetFocus(wxFocusEvent& evt)
{
	// We need this as clicking directly in the control does not change
	// the active view - we have to do it. Conversely, setting the active
	// view does not change the focused window, so we have to do that too.
	if (GetDocumentManager()->GetCurrentView() != this)
	{
		m_parentPanel->OnActivateView(this);
		Activate(true);
	}
}


void CAgilityBookTreeView::OnCtrlContextMenu(wxTreeEvent& evt)
{
	if (!m_Ctrl)
		return;
	bool bSkip = true;
	wxPoint point;
	if (GetMenuPosition(point, *m_Ctrl, evt))
	{
		CAgilityBookTreeData* pData = GetTreeItem(evt.GetItem());
		if (pData)
		{
			bSkip = false;
			m_bSuppressSelect = true;
			wxTreeItemId item = m_Ctrl->GetSelection();
			// On a r-click context, the highlighting is set, but the current item
			// doesn't change. The wxTreeCtrl api does not appear to have the
			// concept of the current highlight item like windows does. So we'll
			// force the current item, then reset. But ignore the changes!
			// (do not use the changing msg and veto it - that kills the change!)
			m_Ctrl->SelectItem(evt.GetItem());
			wxMenu* menu = CreatePopup(pData->GetMenuID());
			if (menu)
			{
				m_Ctrl->PopupMenu(menu, point);
				delete menu;
			}
			m_Ctrl->SelectItem(item);
			m_bSuppressSelect = false;
		}
	}
	if (bSkip)
		evt.Skip();
}


void CAgilityBookTreeView::OnCtrlSelectionChanged(wxTreeEvent& evt)
{
	DoSelectionChange(evt.GetItem());
	evt.Skip();
}


void CAgilityBookTreeView::OnCtrlItemActivated(wxTreeEvent& evt)
{
	CAgilityBookTreeData* pData = GetCurrentTreeItem();
	if (pData)
	{
		// eat the dbl-click when we do this - it's consistent with v1.x
		pData->Properties();
	}
	else
		evt.Skip();
}


void CAgilityBookTreeView::OnCtrlKeyDown(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
	case WXK_RETURN:
	case WXK_NUMPAD_ENTER:
		{
			CAgilityBookTreeData* pData = GetCurrentTreeItem();
			if (pData)
				pData->Properties();
		}
		break;
	}
	evt.Skip();
}


void CAgilityBookTreeView::OnViewContextMenu(wxContextMenuEvent& evt)
{
	if (!m_Ctrl)
		return;
	bool bSkip = true;
	CAgilityBookTreeData* pData = GetCurrentTreeItem();
	if (pData)
	{
		wxPoint point;
		if (GetMenuPosition(point, *m_Ctrl, evt))
		{
			wxMenu* menu = CreatePopup(pData->GetMenuID());
			if (menu)
			{
				bSkip = false;
				m_Ctrl->PopupMenu(menu, point);
				delete menu;
			}
		}
	}
	if (bSkip)
		evt.Skip();
}


void CAgilityBookTreeView::OnViewUpdateCmd(wxUpdateUIEvent& evt)
{
	if (!m_Ctrl)
	{
		evt.Skip();
		return;
	}
	bool bEnable = false;
	switch (evt.GetId())
	{
	default:
		{
			CAgilityBookTreeData* pData = GetCurrentTreeItem();
			if (pData)
			{
				pData->OnUpdateCmd(evt.GetId(), bEnable);
			}
			else
			{
				if (ID_AGILITY_NEW_DOG == evt.GetId()
				|| (wxID_PASTE == evt.GetId()
				&& CClipboardDataReader::IsFormatAvailable(eFormatDog)))
				{
					bEnable = true;
				}
			}
		}
		break;
	case wxID_FIND:
	case ID_EDIT_FIND_NEXT:
	case ID_EDIT_FIND_PREVIOUS:
		bEnable = true;
		break;
	case ID_EXPAND:
		{
			wxTreeItemId item = m_Ctrl->GetSelection();
			if (item.IsOk() && m_Ctrl->ItemHasChildren(item))
			{
				if (!m_Ctrl->IsExpanded(item))
					bEnable = true;
			}
		}
		break;
	case ID_EXPAND_ALL:
		{
			wxTreeItemId item = m_Ctrl->GetSelection();
			if (item.IsOk() && m_Ctrl->ItemHasChildren(item))
				bEnable = true;
		}
		break;
	case ID_COLLAPSE:
		{
			wxTreeItemId item = m_Ctrl->GetSelection();
			if (item.IsOk() && m_Ctrl->ItemHasChildren(item))
			{
				if (m_Ctrl->IsExpanded(item))
					bEnable = true;
			}
		}
		break;
	case ID_COLLAPSE_ALL:
		{
			wxTreeItemId item = m_Ctrl->GetSelection();
			if (item.IsOk() && m_Ctrl->ItemHasChildren(item))
				bEnable = true;
		}
		break;
	case ID_VIEW_CUSTOMIZE:
	case ID_VIEW_SORTRUNS:
	case ID_VIEW_RUNS_BY_TRIAL:
	case ID_VIEW_TABLE_IN_YPS:
		bEnable = true;
		break;
	}
	evt.Enable(bEnable);
}


bool CAgilityBookTreeView::OnCmd(int id)
{
	if (!m_Ctrl)
		return false;

	bool bHandled = false;
	switch (id)
	{
	default:
		{
			CAgilityBookTreeData* pData = GetCurrentTreeItem();
			if (pData)
			{
				bool bModified = false;
				bHandled = pData->OnCmd(id, bModified, NULL);
				if (bModified)
					GetDocument()->Modify(true);
			}
			else if (ID_AGILITY_NEW_DOG == id)
			{
				ARBDogPtr dog(ARBDog::New());
				CDlgDog dlg(GetDocument(), dog);
				if (wxID_OK == dlg.ShowModal())
				{
					if (GetDocument()->Book().GetDogs().AddDog(dog))
						InsertDog(dog, true);
					// For some reason, the first dog isn't showing up.
					if (1 == GetDocument()->Book().GetDogs().size())
						LoadData();
				}
				bHandled = true;
			}
			else if (wxID_PASTE == id)
			{
				wxBusyCursor wait;
				bool bLoaded = false;
				PasteDog(bLoaded);
			}
		}
		break;

	case wxID_FIND:
		bHandled = true;
		{
			CDlgFind dlg(m_Callback, m_Ctrl);
			dlg.ShowModal();
		}
		break;

	case ID_EDIT_FIND_NEXT:
		bHandled = true;
		{
			m_Callback.SearchDown(true);
			if (m_Callback.Text().IsEmpty())
				OnCmd(wxID_FIND);
			else
				m_Callback.Search(NULL);
		}
		break;

	case ID_EDIT_FIND_PREVIOUS:
		bHandled = true;
		{
			m_Callback.SearchDown(false);
			if (m_Callback.Text().IsEmpty())
				OnCmd(wxID_FIND);
			else
				m_Callback.Search(NULL);
		}
		break;

	case ID_EXPAND:
		bHandled = true;
		{
			wxTreeItemId item = m_Ctrl->GetSelection();
			if (item.IsOk())
			{
				m_Ctrl->Expand(item);
				m_Ctrl->EnsureVisible(item);
			}
		}
		break;

	case ID_EXPAND_ALL:
		bHandled = true;
		{
			wxTreeItemId item = m_Ctrl->GetSelection();
			if (item.IsOk())
			{
				m_Ctrl->ExpandAllChildren(item);
				m_Ctrl->EnsureVisible(item);
			}
		}
		break;

	case ID_COLLAPSE:
		bHandled = true;
		{
			wxTreeItemId item = m_Ctrl->GetSelection();
			if (item.IsOk())
			{
				m_Ctrl->Collapse(item);
				m_Ctrl->EnsureVisible(item);
			}
		}
		break;

	case ID_COLLAPSE_ALL:
		bHandled = true;
		{
			wxTreeItemId item = m_Ctrl->GetSelection();
			if (item.IsOk())
			{
				m_Ctrl->CollapseAllChildren(item);
				m_Ctrl->EnsureVisible(item);
			}
		}
		break;

	case ID_VIEW_CUSTOMIZE:
		bHandled = true;
		{
			CDlgAssignColumns dlg(CAgilityBookOptions::eView, m_Ctrl, GetDocument(), IO_TYPE_VIEW_TREE_DOG);
			dlg.ShowModal();
		}
		break;

	case ID_VIEW_SORTRUNS:
		bHandled = true;
		CAgilityBookOptions::SetNewestDatesFirst(!CAgilityBookOptions::GetNewestDatesFirst());
		GetDocument()->SortDates();
		LoadData();
		break;

	case ID_VIEW_RUNS_BY_TRIAL:
		bHandled = true;
		CAgilityBookOptions::SetViewRunsByTrial(!CAgilityBookOptions::GetViewRunsByTrial());
		{
			CUpdateHint hint(UPDATE_RUNS_VIEW);
			GetDocument()->UpdateAllViews(this, &hint);
		}
		break;

	case ID_VIEW_TABLE_IN_YPS:
		bHandled = true;
		CAgilityBookOptions::SetTableInYPS(!CAgilityBookOptions::GetTableInYPS());
		if (wxGetApp().GetTopWindow())
			wxGetApp().GetTopWindow()->Refresh();
		break;
	}

	return bHandled;
}


void CAgilityBookTreeView::OnViewCmd(wxCommandEvent& evt)
{
	OnCmd(evt.GetId());
}


void CAgilityBookTreeView::OnPrint(wxCommandEvent& evt)
{
	wxGetApp().GetHtmlPrinter()->PrintText(GetPrintDataAsHtmlTable());
}


void CAgilityBookTreeView::OnPreview(wxCommandEvent& evt)
{
	wxGetApp().GetHtmlPrinter()->PreviewText(GetPrintDataAsHtmlTable());
}
