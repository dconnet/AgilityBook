/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CAgilityBookTree class
 * @author David Connet
 *
 * Revision History
 * 2022-04-15 Use wx DPI support.
 * 2019-01-01 Fix selection on initial load.
 * 2018-09-15 Refactored how tree/list handle common actions.
 * 2014-04-11 Fix switching dogs
 * 2013-04-22 Changing dogs didn't update runs when viewing all runs.
 * 2012-10-03 Fixed a printing problem in the tree.
 * 2012-07-04 Add option to use run time or opening time in gamble OPS.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2011-10-11 Fixed bug on Mac when deleting via context menu.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-17 Fix r-click selection issues.
 * 2009-07-13 Changing language didn't update dog's age.
 * 2009-07-11 Change how runs are synced with list to reduce reloading.
 * 2009-02-08 Ported to wxWidgets.
 * 2008-11-19 Added SelectDog()
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-22 Support pasting a dog when no dogs exist in file.
 * 2005-08-31 The wrong item in the tree was selected after reordering.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-12-31 Make F1 invoke context help.
 * 2004-10-04 Added div-by-0 tests.
 * 2004-04-15 Added Duplicate menu item.
 * 2003-12-30 Implemented customized text in tree.
 * 2003-12-28 Implemented Find/FindNext.
 * 2003-08-30 Added GetPrintLine to allow future differences between
 *            printing and viewing (already in the listctrl)
 * 2003-08-28 Added printing.
 * 2003-08-27 Cleaned up selection synchronization.
 * 2003-08-24 Optimized filtering by adding boolean into ARBBase to
 *            prevent constant re-evaluation.
 */

#include "stdafx.h"
#include "AgilityBookTreeView.h"

#include "AgilityBook.h"
#include "AgilityBookOptions.h"
#include "AgilityBookPanels.h"
#include "AgilityBookRunsView.h"
#include "AgilityBookTreeData.h"
#include "ClipBoard.h"
#include "DlgAssignColumns.h"
#include "DlgDog.h"
#include "DlgReorder.h"
#include "FilterOptions.h"
#include "ImageHelper.h"
#include "MainFrm.h"
#include "Print.h"
#include "RegItems.h"

#include "ARB/ARBDog.h"
#include "ARB/ARBDogRun.h"
#include "ARB/ARBDogTrial.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/DlgFind.h"
#include "LibARBWin/Logger.h"
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

/////////////////////////////////////////////////////////////////////////////
// Find

CAgilityBookTreeView::CFindData::CFindData(CAgilityBookTreeView* pView)
	: m_pView(pView)
{
}


void CAgilityBookTreeView::CFindData::FillTree(wxTreeItemId hItem)
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


wxTreeItemId CAgilityBookTreeView::CFindData::GetNextItem()
{
	wxTreeItemId hItem;
	if (SearchDown())
	{
		++m_Iter;
		if (m_Iter == m_Items.end())
			hItem = nullptr;
		else
			hItem = *m_Iter;
	}
	else
	{
		if (m_Iter == m_Items.begin())
			hItem = nullptr;
		else
		{
			--m_Iter;
			hItem = *m_Iter;
		}
	}
	return hItem;
}


bool CAgilityBookTreeView::CFindData::Search(CDlgFind* pDlg)
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
	while (!bFound && hItem.IsOk())
	{
		std::set<wxString> strings;
		if (SearchAll())
		{
			CAgilityBookTreeData* pData = m_pView->GetTreeItem(hItem);
			if (pData)
				pData->GetARBBase()->GetSearchStrings(strings);
		}
		else
		{
			strings.insert(m_pView->m_Ctrl->GetItemText(hItem));
		}
		for (auto const& str : strings)
		{
			if (Compare(str))
			{
				m_pView->ChangeSelection(hItem);
				bFound = true;
			}
		}
		hItem = GetNextItem();
	}
	if (!bFound)
	{
		auto msg = wxString::Format(_("IDS_CANNOT_FIND"), m_search);
		wxMessageBox(msg, _("Agility Record Book"), wxOK | wxCENTRE | wxICON_INFORMATION);
	}
	return bFound;
}

/////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_CLASS(CAgilityBookTreeView, CAgilityBookBaseExtraView)


wxBEGIN_EVENT_TABLE(CAgilityBookTreeView, CAgilityBookBaseExtraView)
	EVT_CONTEXT_MENU(CAgilityBookTreeView::OnViewContextMenu)
	EVT_UPDATE_UI(wxID_DUPLICATE, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(wxID_DUPLICATE, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(wxID_CUT, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(wxID_CUT, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(wxID_COPY, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(wxID_COPY, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(wxID_PASTE, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(wxID_PASTE, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(wxID_DELETE, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(wxID_DELETE, CAgilityBookTreeView::OnViewCmd)
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
	EVT_UPDATE_UI(ID_VIEW_RUNTIME_IN_OPS, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_RUNTIME_IN_OPS, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_EXPAND, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_EXPAND, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_COLLAPSE, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_COLLAPSE, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_EXPAND_ALL, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_EXPAND_ALL, CAgilityBookTreeView::OnViewCmd)
	EVT_UPDATE_UI(ID_COLLAPSE_ALL, CAgilityBookTreeView::OnViewUpdateCmd)
	EVT_MENU(ID_COLLAPSE_ALL, CAgilityBookTreeView::OnViewCmd)
	EVT_MENU(wxID_PRINT, CAgilityBookTreeView::OnPrintView)
	EVT_MENU(wxID_PREVIEW, CAgilityBookTreeView::OnPreview)
wxEND_EVENT_TABLE()


#include "Platform/arbWarningPush.h"
CAgilityBookTreeView::CAgilityBookTreeView(CTabView* pTabView, wxDocument* doc)
	: CAgilityBookBaseExtraView(pTabView, doc)
	, m_Ctrl(nullptr)
	, m_ImageList()
#ifdef WX_TREE_HAS_STATE
	, m_ImageListStates()
	, m_idxEmpty(-1)
	, m_idxChecked(-1)
#endif
	, m_Columns()
	, m_bReset(false)
	, m_bSuppressSelect(false)
	, m_bInPopup(false)
	, m_bInDelete(false)
	, m_bDeleteChanged(false)
	, m_itemPopup()
	, m_find(this)
{
	CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_DOG, m_Columns[0]);
	CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_TRIAL, m_Columns[1]);
	CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_RUN, m_Columns[2]);
}
#include "Platform/arbWarningPop.h"


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
	m_Ctrl = new CTreeCtrl(
		parentCtrl,
		wxID_ANY,
		wxDefaultPosition,
		wxDefaultSize,
		wxTR_SINGLE | wxTR_FULL_ROW_HIGHLIGHT | wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT | wxTR_LINES_AT_ROOT | wxTR_NO_LINES
			| wxNO_BORDER,
		wxDefaultValidator);
	m_Ctrl->Bind(wxEVT_SET_FOCUS, &CAgilityBookTreeView::OnCtrlSetFocus, this);
	m_Ctrl->Bind(wxEVT_COMMAND_TREE_ITEM_MENU, &CAgilityBookTreeView::OnCtrlContextMenu, this);
	m_Ctrl->Bind(wxEVT_COMMAND_TREE_SEL_CHANGED, &CAgilityBookTreeView::OnCtrlSelectionChanged, this);
	m_Ctrl->Bind(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &CAgilityBookTreeView::OnCtrlItemActivated, this);
	m_ImageList.Create(m_Ctrl);
	m_Ctrl->SetImageList(&m_ImageList);
#ifdef WX_TREE_HAS_STATE
	m_ImageListStates.Create(m_Ctrl->FromDIP(16), m_Ctrl->FromDIP(16));
	// Note: Position 0 cannot be used.
	m_ImageListStates.Add(CResourceManager::Get()->GetIcon(ImageMgrBlank));
	m_idxEmpty = m_ImageListStates.Add(CResourceManager::Get()->GetIcon(ImageMgrBlank));
	m_idxChecked = m_ImageListStates.Add(CResourceManager::Get()->GetIcon(ImageMgrCheck));
	m_Ctrl->SetStateImageList(&m_ImageListStates);
#endif
#if defined(__WXMAC__)
	m_Ctrl->SetDropTarget(new CFileDropTarget(doc->GetDocumentManager()));
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
	m_Ctrl = nullptr;
}


bool CAgilityBookTreeView::OnCreate(wxDocument* doc, long flags)
{
	return true;
}


void CAgilityBookTreeView::DoActivateView(bool activate, wxView* activeView, wxView* deactiveView)
{
	if (m_Ctrl && activate && wxWindow::DoFindFocus() != m_Ctrl)
		m_Ctrl->SetFocus();
}


void CAgilityBookTreeView::OnDraw(wxDC* dc)
{
}


void CAgilityBookTreeView::OnUpdate(wxView* sender, wxObject* inHint)
{
	STACK_TRACE(stack, L"CAgilityBookTreeView::OnUpdate");

	if (!m_Ctrl)
		return;
	CUpdateHint* hint = nullptr;
	if (inHint)
		hint = wxDynamicCast(inHint, CUpdateHint);
	if (!hint || hint->IsSet(UPDATE_TREE_VIEW) || hint->IsEqual(UPDATE_CONFIG) || hint->IsEqual(UPDATE_OPTIONS))
	{
		LoadData();
	}
	else if (hint && hint->IsEqual(UPDATE_NEW_TRIAL))
	{
		LoadData();
		ARBDogTrialPtr pTrial = std::dynamic_pointer_cast<ARBDogTrial, ARBBase>(hint->GetObj());
		assert(pTrial);
		CAgilityBookTreeData* pData = FindData(pTrial);
		assert(pData);
		ChangeSelection(pData->GetId());
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
	msg.clear();
	return false;
}


bool CAgilityBookTreeView::GetMessage2(wxString& msg) const
{
	if (GetDocument()->GetCurrentDog())
		msg = GetDocument()->GetCurrentDog()->GetCallName();
	else
		msg = _("IDS_INDICATOR_BLANK");
	// If there's no dog, still return true. Or we won't blank an existing name.
	// We only need to worry about this here and the Runs view since this is the
	// only place we can delete a dog.
	return true;
}


bool CAgilityBookTreeView::AllowStatusContext(int field) const
{
	return STATUS_DOG == field || STATUS_FILTERED == field;
}


void CAgilityBookTreeView::RefreshItem(wxTreeItemId item, bool bRecurse)
{
	RefreshTreeItem(m_Ctrl, item, bRecurse);
}


CAgilityBookTreeData* CAgilityBookTreeView::GetCurrentTreeItem() const
{
	if (m_Ctrl && m_Ctrl->GetSelection().IsOk())
		return GetTreeItem(m_Ctrl->GetSelection());
	return nullptr;
}


CAgilityBookTreeData* CAgilityBookTreeView::GetTreeItem(wxTreeItemId hItem) const
{
	if (m_Ctrl && hItem.IsOk())
		return dynamic_cast<CAgilityBookTreeData*>(m_Ctrl->GetItemData(hItem));
	return nullptr;
}


bool CAgilityBookTreeView::SelectDog(ARBDogPtr const& inDog)
{
	bool bSelected = false;
	if (m_Ctrl)
	{
		CAgilityBookTreeData* pData = FindData(m_Ctrl->GetRootItem(), inDog);
		if (pData)
		{
			bSelected = true;
			m_Ctrl->SelectItem(pData->GetId());
			m_Ctrl->EnsureVisible(pData->GetId());
		}
	}
	return bSelected;
}


CAgilityBookTreeData* CAgilityBookTreeView::FindData(wxTreeItemId hItem, ARBBasePtr const& inBase) const
{
	if (!inBase || !m_Ctrl)
		return nullptr;
	CAgilityBookTreeData* pData = nullptr;
	if (m_Ctrl->GetRootItem() != hItem)
	{
		CAgilityBookTreeData* pCheck = dynamic_cast<CAgilityBookTreeData*>(m_Ctrl->GetItemData(hItem));
		if (pCheck && pCheck->GetARBBase() == inBase)
			pData = pCheck;
	}
	if (!pData)
	{
		wxTreeItemIdValue cookie;
		wxTreeItemId hChildItem = m_Ctrl->GetFirstChild(hItem, cookie);
		while (hChildItem.IsOk() && !pData)
		{
			pData = FindData(hChildItem, inBase);
			hChildItem = m_Ctrl->GetNextChild(hItem, cookie);
		}
	}
	return pData;
}


CAgilityBookTreeData* CAgilityBookTreeView::FindData(wxTreeItemId hItem, ARB::ARBDogPtr const& inDog) const
{
	if (!inDog || !m_Ctrl)
		return nullptr;
	CAgilityBookTreeData* pData = nullptr;
	if (m_Ctrl->GetRootItem() != hItem)
	{
		CAgilityBookTreeDataDog* pCheck = dynamic_cast<CAgilityBookTreeDataDog*>(m_Ctrl->GetItemData(hItem));
		// Don't use virtual 'GetDog' on pCheck as that can give back a parent
		// node which isn't what we want here. We want the real dog node.
		if (pCheck && pCheck->GetDog() == inDog)
			pData = pCheck;
	}
	if (!pData)
	{
		wxTreeItemIdValue cookie;
		wxTreeItemId hChildItem = m_Ctrl->GetFirstChild(hItem, cookie);
		while (hChildItem.IsOk() && !pData)
		{
			pData = FindData(hChildItem, inDog);
			hChildItem = m_Ctrl->GetNextChild(hItem, cookie);
		}
	}
	return pData;
}


CAgilityBookTreeData* CAgilityBookTreeView::FindData(wxTreeItemId hItem, ARB::ARBDogTrialPtr const& inTrial) const
{
	if (!inTrial || !m_Ctrl)
		return nullptr;
	CAgilityBookTreeData* pData = nullptr;
	if (m_Ctrl->GetRootItem() != hItem)
	{
		CAgilityBookTreeDataTrial* pCheck = dynamic_cast<CAgilityBookTreeDataTrial*>(m_Ctrl->GetItemData(hItem));
		if (pCheck && pCheck->GetTrial() == inTrial)
			pData = pCheck;
	}
	if (!pData)
	{
		wxTreeItemIdValue cookie;
		wxTreeItemId hChildItem = m_Ctrl->GetFirstChild(hItem, cookie);
		while (hChildItem.IsOk() && !pData)
		{
			pData = FindData(hChildItem, inTrial);
			hChildItem = m_Ctrl->GetNextChild(hItem, cookie);
		}
	}
	return pData;
}


CAgilityBookTreeData* CAgilityBookTreeView::FindData(wxTreeItemId hItem, ARB::ARBDogRunPtr const& inRun) const
{
	if (!inRun || !m_Ctrl)
		return nullptr;
	CAgilityBookTreeData* pData = nullptr;
	if (m_Ctrl->GetRootItem() != hItem)
	{
		CAgilityBookTreeDataRun* pCheck = dynamic_cast<CAgilityBookTreeDataRun*>(m_Ctrl->GetItemData(hItem));
		if (pCheck && pCheck->GetRun() == inRun)
			pData = pCheck;
	}
	if (!pData)
	{
		wxTreeItemIdValue cookie;
		wxTreeItemId hChildItem = m_Ctrl->GetFirstChild(hItem, cookie);
		while (hChildItem.IsOk() && !pData)
		{
			pData = FindData(hChildItem, inRun);
			hChildItem = m_Ctrl->GetNextChild(hItem, cookie);
		}
	}
	return pData;
}


wxTreeItemId CAgilityBookTreeView::InsertDog(ARBDogPtr const& inDog, bool bSelect)
{
	wxTreeItemId hItem;
	if (inDog && m_Ctrl)
	{
		CAgilityBookTreeDataDog* pDataDog = new CAgilityBookTreeDataDog(this, inDog);
		int idxImage = pDataDog->OnNeedIcon();
		hItem = m_Ctrl->AppendItem(m_Ctrl->GetRootItem(), pDataDog->OnNeedText(), idxImage, idxImage, pDataDog);
		for (ARBDogTrialList::const_iterator iterTrial = inDog->GetTrials().begin();
			 iterTrial != inDog->GetTrials().end();
			 ++iterTrial)
		{
			InsertTrial((*iterTrial), hItem);
		}
		if (bSelect)
		{
			ChangeSelection(hItem);
		}
	}
	return hItem;
}


wxTreeItemId CAgilityBookTreeView::InsertTrial(ARBDogTrialPtr const& inTrial, wxTreeItemId hParent)
{
	wxTreeItemId hTrial;
	if (inTrial && !inTrial->IsFiltered() && m_Ctrl)
	{
		CAgilityBookTreeDataTrial* pDataTrial = new CAgilityBookTreeDataTrial(this, inTrial);
		int idxImage = pDataTrial->OnNeedIcon();
		hTrial = m_Ctrl->AppendItem(hParent, pDataTrial->OnNeedText(), idxImage, idxImage, pDataTrial);
#ifdef WX_TREE_HAS_STATE
		int state = pDataTrial->GetTrial()->IsVerified() ? m_idxChecked : m_idxEmpty;
		m_Ctrl->SetItemState(hTrial, state);
#endif
		for (ARBDogRunList::const_iterator iterRun = inTrial->GetRuns().begin(); iterRun != inTrial->GetRuns().end();
			 ++iterRun)
		{
			InsertRun(inTrial, (*iterRun), hTrial);
		}
	}
	return hTrial;
}


wxTreeItemId CAgilityBookTreeView::InsertRun(
	ARBDogTrialPtr const& inTrial,
	ARBDogRunPtr const& inRun,
	wxTreeItemId hParent)
{
	wxTreeItemId hRun;
	if (inRun && !inRun->IsFiltered() && m_Ctrl)
	{
		CAgilityBookTreeDataRun* pDataRun = new CAgilityBookTreeDataRun(this, inRun);
		int idxImage = pDataRun->OnNeedIcon();
		hRun = m_Ctrl->AppendItem(hParent, pDataRun->OnNeedText(), idxImage, idxImage, pDataRun);
	}
	return hRun;
}


bool CAgilityBookTreeView::PasteDog(bool& bLoaded)
{
	if (!m_Ctrl)
		return false;
	ElementNodePtr tree(ElementNode::New());
	CClipboardDataReader clpData;
	if (clpData.GetData(ARBClipFormat::Dog, tree))
	{
		if (CLIPDATA == tree->GetName())
		{
			ARBDogPtr pDog(ARBDog::New());
			if (pDog)
			{
				CErrorCallback err;
				if (pDog->Load(
						GetDocument()->Book().GetConfig(),
						tree->GetNthElementNode(0),
						ARBAgilityRecordBook::GetCurrentDocVersion(),
						err))
				{
					bLoaded = true;
					pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
					std::vector<CVenueFilter> venues;
					CFilterOptions::Options().GetFilterVenue(venues);
					if (!GetDocument()->Book().GetDogs().AddDog(pDog))
					{
						bLoaded = false;
						wxMessageBox(
							_("IDS_CREATEDOG_FAILED"),
							_("Agility Record Book"),
							wxOK | wxCENTRE | wxICON_STOP);
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
						GetDocument()->UpdateAllViews(nullptr, &hint);
					}
				}
				else if (0 < err.m_ErrMsg.size())
					wxMessageBox(err.m_ErrMsg, _("Agility Record Book"), wxOK | wxCENTRE | wxICON_WARNING);
			}
		}
		return true;
	}
	return false;
}


bool CAgilityBookTreeView::PasteRuns(
	ARBDogPtr const& inDog,
	ARBDogTrialPtr const& inTrial,
	bool& bLoaded,
	bool* bTreeSelectionSet)
{
	if (!m_Ctrl || !inDog || !inTrial)
		return false;

	CAgilityBookTreeData* pData = FindData(inTrial);
	if (!pData)
		return false;

	ElementNodePtr tree(ElementNode::New());
	CClipboardDataReader clpData;
	if (clpData.GetData(ARBClipFormat::Run, tree))
	{
		if (CLIPDATA == tree->GetName())
		{
			CErrorCallback err;
			std::vector<ARBDogRunPtr> runs;
			for (int iRun = 0; iRun < tree->GetElementCount(); ++iRun)
			{
				ElementNodePtr element = tree->GetElementNode(iRun);
				if (!element)
					continue;
				ARBDogRunPtr pRun(ARBDogRun::New());
				if (pRun)
				{
					if (pRun->Load(
							GetDocument()->Book().GetConfig(),
							inTrial->GetClubs(),
							element,
							ARBAgilityRecordBook::GetCurrentDocVersion(),
							err))
						runs.push_back(pRun);
				}
			}
			if (0 < runs.size())
			{
				size_t nFailed = 0;
				bLoaded = true;
				std::vector<CVenueFilter> venues;
				CFilterOptions::Options().GetFilterVenue(venues);
				for (std::vector<ARBDogRunPtr>::iterator iter = runs.begin(); iter != runs.end(); ++iter)
				{
					ARBDogRunPtr pRun = *iter;
					if (!inTrial->GetRuns().AddRun(pRun))
					{
						++nFailed;
						wxMessageBox(
							_("IDS_CREATERUN_FAILED"),
							_("Agility Record Book"),
							wxOK | wxCENTRE | wxICON_STOP);
					}
					else
						GetDocument()->ResetVisibility(venues, inTrial, pRun);
				}
				if (runs.size() == nFailed)
					bLoaded = false;
				else
				{
					inTrial->GetRuns().sort();
					inDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
					Freeze();
					unsigned int iHint = 0;
					ARBBasePtr pBase;
					wxTreeItemId hItem;
					for (std::vector<ARBDogRunPtr>::iterator iter = runs.begin(); iter != runs.end(); ++iter)
					{
						ARBDogRunPtr pRun = *iter;
						if (!pBase)
						{
							iHint |= UPDATE_RUNS_SELECTION_VIEW;
							pBase = pRun;
						}
						hItem = InsertRun(inTrial, pRun, pData->GetId());
					}
					Thaw();
					Refresh();
					bool bOk = true;
					if (!hItem.IsOk())
					{
						bOk = false;
						if (CFilterOptions::Options().IsFilterEnabled())
							wxMessageBox(
								_("IDS_CREATERUN_FILTERED"),
								_("Agility Record Book"),
								wxOK | wxCENTRE | wxICON_STOP);
					}
					else
					{
						SelectItem(hItem);
						if (bTreeSelectionSet)
							*bTreeSelectionSet = true;
					}
					if (bOk)
					{
						inTrial->SetMultiQs(GetDocument()->Book().GetConfig());
						CUpdateHint hint(UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW | UPDATE_TREE_VIEW | iHint, pBase);
						GetDocument()->UpdateAllViews(nullptr, &hint);
					}
				}
			}
			if (!bLoaded && 0 < err.m_ErrMsg.size())
				wxMessageBox(err.m_ErrMsg, _("Agility Record Book"), wxOK | wxCENTRE | wxICON_WARNING);
		}
		return true;
	}

	return false;
}


wxString CAgilityBookTreeView::GetPrintLine(wxTreeItemId hItem) const
{
	if (hItem.IsOk() && m_Ctrl)
		return m_Ctrl->GetItemText(hItem);
	return wxString();
}


void CAgilityBookTreeView::UpdateData(wxTreeItemId hItem)
{
	if (!m_Ctrl)
		return;
	if (m_Ctrl->GetRootItem() != hItem)
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


void CAgilityBookTreeView::ChangeSelection(wxTreeItemId hItem, bool bEnsureVisible)
{
	m_Ctrl->SelectItem(hItem);
	if (bEnsureVisible)
		m_Ctrl->EnsureVisible(hItem);
	DoSelectionChange(hItem);
}


void CAgilityBookTreeView::DoSelectionChange(wxTreeItemId hItem)
{
	if (m_bSuppressSelect)
	{
		if (m_bInPopup)
		{
			m_itemPopup = hItem;
			if (m_bInDelete)
				m_bDeleteChanged = true;
		}
	}
	else
	{
		CAgilityBookTreeData* pData = GetTreeItem(hItem);
		ARBBasePtr pBase;
		unsigned int iHint = 0;
		if (pData)
		{
			// Set the current dog
			ARBDogPtr pDogCurrent = GetDocument()->GetCurrentDog();
			ARBDogPtr pDog = pData->GetDog();
			if (!pDogCurrent || !pDog || pDogCurrent != pDog)
				iHint |= UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW;
			GetDocument()->SetCurrentDog(pDog, true);
			CAgilityBookRunsView* pView = GetDocument()->GetRunsView();
			if (ARBViewRuns::RunsByTrial == CAgilityBookOptions::GetViewRunsStyle()
				&& !pView->IsTrial(pData->GetTrial()))
			{
				iHint |= UPDATE_RUNS_VIEW;
			}
			// Pass the selected run
			else if (ARBTreeDataType::Run == pData->GetType())
			{
				pBase = pData->GetRun();
				iHint |= UPDATE_RUNS_SELECTION_VIEW;
			}
		}
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

	STACK_TRACE(stack, L"CAgilityBookTreeView::LoadData");

	m_bSuppressSelect = true;

	m_Ctrl->Freeze();

	wxBusyCursor wait;
	// Remember the currently selected item.
	wxString strCallName;
	CAgilityBookTreeData const* pData = GetCurrentTreeItem();
	if (!pData)
	{
		strCallName = wxConfig::Get()->Read(CFG_SETTINGS_LASTDOG, wxString());
	}
	std::list<ARBBasePtr> baseItems;
	while (pData)
	{
		baseItems.push_front(pData->GetARBBase());
		pData = pData->GetParent();
	}

	m_Ctrl->DeleteAllItems();

	// Load the data
	m_Ctrl->AddRoot(L"Root");
	wxTreeItemId hItem = nullptr;
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
		for (std::list<ARBBasePtr>::iterator iter = baseItems.begin(); iter != baseItems.end(); ++iter)
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

		CAgilityBookTreeData* pCurrent = GetTreeItem(hItem);
		if (pCurrent)
		{
			if (pCurrent->GetDog() != GetDocument()->GetCurrentDog())
				GetDocument()->SetCurrentDog(pCurrent->GetDog());
		}
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


void CAgilityBookTreeView::PrintLine(wxString& data, wxTreeItemId id, int indent) const
{
	static wxString const spaces(L"&nbsp;&nbsp;&nbsp;");
	if (id.IsOk() && id != m_Ctrl->GetRootItem())
	{
		for (int idx = 0; idx < indent; ++idx)
			data << spaces;
		data << m_Ctrl->GetItemText(id) << L"<br />\n"; // Note, wxWidgets needs the space before the slash
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
	wxString data;
	data << L"<html><body><p>\n";
	PrintLine(data, m_Ctrl->GetRootItem(), -1);
	data << L"</p></body></html>\n";
	return data;
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
	evt.Skip();
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
			if (item == evt.GetItem())
			{
				// If it's the same one, don't reset.
				// (or we die on delete on a Mac)
				item = wxTreeItemId();
			}
			else
			{
				// On a r-click context, the highlighting is set, but the
				// current item doesn't change. The wxTreeCtrl api does not
				// appear to have the concept of the current highlight item
				// like windows does. So we'll force the current item, then
				// reset. But ignore the changes! (do not use the changing msg
				// and veto it - that kills the change!)
				m_Ctrl->SelectItem(evt.GetItem());
			}
			wxMenu* menu = wxGetApp().GetMenus().CreatePopupMenu(m_Ctrl, pData->GetMenuID());
			if (menu)
			{
				m_bInPopup = true;
				m_Ctrl->PopupMenu(menu, point);
				m_bInPopup = false;
				delete menu;
			}
			if (m_itemPopup.IsOk() && m_itemPopup != item)
			{
				// Override the select and force an item if we're not deleting.
				// But we need to actively reselect on delete still.
				if (!m_bDeleteChanged)
				{
					item = m_itemPopup;
					m_itemPopup = wxTreeItemId();
				}
				m_bSuppressSelect = false;
			}
			if (item.IsOk())
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
		evt.Skip();
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
}


void CAgilityBookTreeView::OnViewContextMenu(wxContextMenuEvent& evt)
{
	CLogger::Log(L"MENU: CAgilityBookTreeView::OnViewContextMenu");
#ifdef __WXMAC__
	// Something is wrong on Mac. A right click generates the context menu
	// in the control (above), but we get the message here also. When we
	// right click on the blank view area, windows gets the view message,
	// but the Mac gets nothing. For now, just don't support the context menu
	// in the view for the Mac.
	evt.Skip();
#else
	if (!m_Ctrl)
		return;
	bool bSkip = true;
	CAgilityBookTreeData* pData = GetCurrentTreeItem();
	if (pData)
	{
		wxPoint point;
		if (GetMenuPosition(point, *m_Ctrl, evt))
		{
			wxMenu* menu = wxGetApp().GetMenus().CreatePopupMenu(m_Ctrl, pData->GetMenuID());
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
#endif
}


void CAgilityBookTreeView::OnViewUpdateCmd(wxUpdateUIEvent& evt)
{
	if (!m_Ctrl)
	{
		evt.Skip();
		return;
	}

	bool bEnable = false;
	CAgilityBookTreeData* pData = GetCurrentTreeItem();
	wxTreeItemId item = m_Ctrl->GetSelection();

	switch (evt.GetId())
	{
	case wxID_DUPLICATE:
	case wxID_CUT:
	case wxID_COPY:
	case wxID_DELETE:
		bEnable = !!pData;
		break;

	case wxID_PASTE:
		if (CClipboardDataReader::IsFormatAvailable(ARBClipFormat::Dog))
			bEnable = true;
		else if (pData && pData->GetDog() && CClipboardDataReader::IsFormatAvailable(ARBClipFormat::Trial))
			bEnable = true;
		else if (pData && pData->GetTrial() && CClipboardDataReader::IsFormatAvailable(ARBClipFormat::Run))
			bEnable = true;
		break;

	case ID_REORDER:
		if (pData && pData->GetTrial() && 1 < pData->GetTrial()->GetRuns().size())
			bEnable = true;
		else if (1 < GetDocument()->Book().GetDogs().size())
			bEnable = true;
		break;

	case wxID_FIND:
	case ID_EDIT_FIND_NEXT:
	case ID_EDIT_FIND_PREVIOUS:
		bEnable = true;
		break;

	case ID_AGILITY_EDIT_DOG:
	case ID_AGILITY_DELETE_DOG:
		bEnable = (pData && pData->GetDog());
		break;

	case ID_AGILITY_NEW_TITLE:
		bEnable = (pData && pData->GetDog());
		break;

	case ID_AGILITY_EDIT_TRIAL:
		bEnable = (pData && pData->GetTrial());
		break;

	case ID_AGILITY_NEW_TRIAL:
		bEnable = (pData && pData->GetDog());
		break;

	case ID_AGILITY_DELETE_TRIAL:
		bEnable = (pData && pData->GetTrial());
		break;

	case ID_AGILITY_PRINT_TRIAL:
		bEnable = (pData && pData->GetTrial() && 0 < pData->GetTrial()->GetRuns().size());
		break;

	case ID_AGILITY_EDIT_RUN:
		bEnable = (pData && pData->GetRun());
		break;

	case ID_AGILITY_NEW_RUN:
		bEnable = (pData && pData->GetTrial() && 0 < pData->GetTrial()->GetClubs().size());
		break;

	case ID_AGILITY_DELETE_RUN:
	case ID_AGILITY_PRINT_RUNS:
		bEnable = (pData && pData->GetRun());
		break;

	case ID_VIEW_CUSTOMIZE:
	case ID_VIEW_SORTRUNS:
	case ID_VIEW_RUNS_BY_TRIAL:
	case ID_VIEW_TABLE_IN_YPS:
	case ID_VIEW_RUNTIME_IN_OPS:
		bEnable = true;
		break;

	case ID_EXPAND:
		bEnable = (item.IsOk() && m_Ctrl->ItemHasChildren(item) && !m_Ctrl->IsExpanded(item));
		break;

	case ID_COLLAPSE:
		bEnable = (item.IsOk() && m_Ctrl->ItemHasChildren(item) && m_Ctrl->IsExpanded(item));
		break;

	case ID_EXPAND_ALL:
		bEnable = (item.IsOk() && m_Ctrl->ItemHasChildren(item));
		break;

	case ID_COLLAPSE_ALL:
		bEnable = (item.IsOk() && m_Ctrl->ItemHasChildren(item));
		break;

	default:
		break;
	}

	evt.Enable(bEnable);
}


bool CAgilityBookTreeView::OnCmd(int id)
{
	if (!m_Ctrl)
		return false;

	bool bHandled = true;
	CAgilityBookTreeData* pData = GetCurrentTreeItem();

	switch (id)
	{
	default:
		bHandled = false;
		break;

	case wxID_DUPLICATE:
		if (pData)
			pData->DoDuplicate();
		break;

	case wxID_CUT:
		if (pData && pData->DoCopy())
		{
			m_bInDelete = true;
			pData->DoDelete(true);
			m_bInDelete = false;
		}
		break;

	case wxID_COPY:
		if (pData)
			pData->DoCopy();
		break;

	case wxID_PASTE:
		if (pData)
		{
			bool bLoaded = false;
			wxBusyCursor wait;
			ElementNodePtr tree(ElementNode::New());
			CClipboardDataReader clpData;
			ARBDogTrialPtr pTrial = pData->GetTrial();
			ARBDogPtr pDog = pData->GetDog();
			if (PasteDog(bLoaded))
			{
				// Done.
			}
			else if (pTrial && clpData.GetData(ARBClipFormat::Run, tree) && PasteRuns(pDog, pTrial, bLoaded, nullptr))
			{
				// Done.
			}
			else if (pDog && clpData.GetData(ARBClipFormat::Trial, tree))
			{
				if (CLIPDATA == tree->GetName())
				{
					ARBDogTrialPtr pNewTrial(ARBDogTrial::New());
					if (pNewTrial)
					{
						CErrorCallback err;
						if (pNewTrial->Load(
								GetDocument()->Book().GetConfig(),
								tree->GetNthElementNode(0),
								ARBAgilityRecordBook::GetCurrentDocVersion(),
								err))
						{
							bLoaded = true;
							std::vector<CVenueFilter> venues;
							CFilterOptions::Options().GetFilterVenue(venues);
							if (!pDog->GetTrials().AddTrial(pNewTrial, !CAgilityBookOptions::GetNewestDatesFirst()))
							{
								bLoaded = false;
								wxMessageBox(
									_("IDS_CREATETRIAL_FAILED"),
									_("Agility Record Book"),
									wxOK | wxCENTRE | wxICON_STOP);
							}
							else
							{
								GetDocument()->ResetVisibility(venues, pNewTrial);
								Freeze();
								wxTreeItemId hItem = InsertTrial(pNewTrial, pData->GetDataDog()->GetId());
								Thaw();
								Refresh();
								bool bOk = true;
								if (!hItem.IsOk())
								{
									bOk = false;
									wxMessageBox(
										_("IDS_CREATETRIAL_FILTERED"),
										_("Agility Record Book"),
										wxOK | wxCENTRE | wxICON_STOP);
								}
								else
								{
									SelectItem(hItem);
								}
								if (bOk)
								{
									pNewTrial->SetMultiQs(GetDocument()->Book().GetConfig());
									CUpdateHint hint(UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW | UPDATE_TREE_VIEW);
									GetDocument()->UpdateAllViews(nullptr, &hint);
								}
							}
						}
						else if (0 < err.m_ErrMsg.size())
							wxMessageBox(err.m_ErrMsg, _("Agility Record Book"), wxOK | wxCENTRE | wxICON_WARNING);
					}
				}
			}
			if (bLoaded)
				GetDocument()->Modify(true);
		}
		else
		{
			wxBusyCursor wait;
			bool bLoaded = false;
			PasteDog(bLoaded);
		}
		break;

	case wxID_DELETE:
		if (pData)
		{
			m_bInDelete = true;
			pData->DoDelete(false);
			m_bInDelete = false;
		}
		break;

	case ID_REORDER:
		if (pData && pData->GetTrial())
		{
			CDlgReorder dlg(GetDocument(), pData->GetTrial(), pData->GetRun());
			dlg.ShowModal();
		}
		else
		{
			CDlgReorder dlg(GetDocument(), &(GetDocument()->Book().GetDogs()));
			dlg.ShowModal();
		}
		break;

	case wxID_FIND:
	{
		CDlgFind dlg(m_find, m_Ctrl);
		dlg.ShowModal();
	}
	break;

	case ID_EDIT_FIND_NEXT:
	{
		m_find.SearchDown(true);
		if (m_find.Text().empty())
			OnCmd(wxID_FIND);
		else
			m_find.Search(nullptr);
	}
	break;

	case ID_EDIT_FIND_PREVIOUS:
	{
		m_find.SearchDown(false);
		if (m_find.Text().empty())
			OnCmd(wxID_FIND);
		else
			m_find.Search(nullptr);
	}
	break;

	case ID_AGILITY_EDIT_DOG:
		if (pData)
			GetDocument()->EditDog(pData->GetDog());
		break;

	case ID_AGILITY_DELETE_DOG:
		if (pData)
		{
			CAgilityBookTreeDataDog* pDataDog = dynamic_cast<CAgilityBookTreeDataDog*>(pData);
			if (pDataDog)
			{
				m_bInDelete = true;
				pDataDog->DoDelete(false);
				m_bInDelete = false;
			}
		}
		break;

	case ID_AGILITY_NEW_TITLE:
		if (pData && pData->GetDog())
			GetDocument()->AddTitle(pData->GetDog());
		break;

	case ID_AGILITY_EDIT_TRIAL:
		if (pData && pData->GetTrial())
			GetDocument()->EditTrial(pData->GetDog(), pData->GetTrial());
		break;

	case ID_AGILITY_NEW_TRIAL:
		if (pData && pData->GetDog())
			GetDocument()->EditTrial(pData->GetDog(), nullptr);
		break;

	case ID_AGILITY_DELETE_TRIAL:
		if (pData)
		{
			m_bInDelete = true;
			CAgilityBookTreeData* pTrialData = FindData(pData->GetTrial());
			if (GetDocument()->DeleteTrial(pData->GetDog(), pData->GetTrial()))
			{
				// Delete() will cause 'pData' to be deleted.
				if (pTrialData)
					Delete(pTrialData->GetId());
				CUpdateHint hint(UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW);
				GetDocument()->UpdateAllViews(nullptr, &hint);
			}
			m_bInDelete = false;
		}
		break;

	case ID_AGILITY_PRINT_TRIAL:
		if (pData && pData->GetTrial())
		{
			ARBDogPtr dog = pData->GetDog();
			std::vector<RunInfo> runs;
			for (ARBDogRunList::iterator iRun = pData->GetTrial()->GetRuns().begin();
				 iRun != pData->GetTrial()->GetRuns().end();
				 ++iRun)
			{
				if (!(*iRun)->IsFiltered())
					runs.push_back(RunInfo(dog, pData->GetTrial(), *iRun));
			}
			PrintRuns(&(GetDocument()->Book().GetConfig()), runs);
		}
		break;

	case ID_AGILITY_EDIT_RUN:
		if (pData && pData->GetRun())
			GetDocument()->EditRun(pData->GetDog(), pData->GetTrial(), pData->GetRun());
		break;

	case ID_AGILITY_NEW_RUN:
		if (pData && pData->GetTrial() && 0 < pData->GetTrial()->GetClubs().size())
			GetDocument()->EditRun(pData->GetDog(), pData->GetTrial(), nullptr);
		break;

	case ID_AGILITY_DELETE_RUN:
		if (pData && pData->GetRun())
		{
			m_bInDelete = true;
			GetDocument()->DeleteRuns({pData->GetRun()});
			m_bInDelete = false;
		}
		break;

	case ID_AGILITY_PRINT_RUNS:
		if (pData && pData->GetRun())
		{
			std::vector<RunInfo> runs;
			runs.push_back(RunInfo(pData->GetDog(), pData->GetTrial(), pData->GetRun()));
			PrintRuns(&(GetDocument()->Book().GetConfig()), runs);
		}
		break;

	case ID_VIEW_CUSTOMIZE:
	{
		CDlgAssignColumns dlg(CAgilityBookOptions::eView, m_Ctrl, GetDocument(), IO_TYPE_VIEW_TREE_DOG);
		dlg.ShowModal();
	}
	break;

	case ID_VIEW_SORTRUNS:
		CAgilityBookOptions::SetNewestDatesFirst(!CAgilityBookOptions::GetNewestDatesFirst());
		GetDocument()->SortDates();
		LoadData();
		break;

	case ID_VIEW_RUNS_BY_TRIAL:
		if (ARBViewRuns::RunsByTrial == CAgilityBookOptions::GetViewRunsStyle())
			CAgilityBookOptions::SetViewRunsStyle(ARBViewRuns::RunsByList);
		else
			CAgilityBookOptions::SetViewRunsStyle(ARBViewRuns::RunsByTrial);
		{
			CUpdateHint hint(UPDATE_RUNS_VIEW);
			GetDocument()->UpdateAllViews(this, &hint);
		}
		break;

	case ID_VIEW_TABLE_IN_YPS:
		CAgilityBookOptions::SetTableInYPS(!CAgilityBookOptions::GetTableInYPS());
		{
			CUpdateHint hint(UPDATE_RUNS_VIEW);
			GetDocument()->UpdateAllViews(nullptr, &hint);
		}
		break;

	case ID_VIEW_RUNTIME_IN_OPS:
		CAgilityBookOptions::SetRunTimeInOPS(!CAgilityBookOptions::GetRunTimeInOPS());
		{
			CUpdateHint hint(UPDATE_RUNS_VIEW);
			GetDocument()->UpdateAllViews(nullptr, &hint);
		}
		break;

	case ID_EXPAND:
	{
		wxTreeItemId item = m_Ctrl->GetSelection();
		if (item.IsOk())
		{
			m_Ctrl->Expand(item);
			m_Ctrl->EnsureVisible(item);
		}
	}
	break;

	case ID_COLLAPSE:
	{
		wxTreeItemId item = m_Ctrl->GetSelection();
		if (item.IsOk())
		{
			m_Ctrl->Collapse(item);
			m_Ctrl->EnsureVisible(item);
		}
	}
	break;

	case ID_EXPAND_ALL:
	{
		wxTreeItemId item = m_Ctrl->GetSelection();
		if (item.IsOk())
		{
			m_Ctrl->ExpandAllChildren(item);
			m_Ctrl->EnsureVisible(item);
		}
	}
	break;

	case ID_COLLAPSE_ALL:
	{
		wxTreeItemId item = m_Ctrl->GetSelection();
		if (item.IsOk())
		{
			m_Ctrl->CollapseAllChildren(item);
			m_Ctrl->EnsureVisible(item);
		}
	}
	break;
	}

	return bHandled;
}


void CAgilityBookTreeView::OnViewCmd(wxCommandEvent& evt)
{
	CLogger::Log(wxString::Format(L"MENU: CAgilityBookTreeView::OnViewCmd %d", evt.GetId()));
	OnCmd(evt.GetId());
}


void CAgilityBookTreeView::OnPrintView(wxCommandEvent& evt)
{
	CLogger::Log(L"MENU: CAgilityBookTreeView::OnPrintView");
	wxGetApp().GetHtmlPrinter()->PrintText(GetPrintDataAsHtmlTable());
}


void CAgilityBookTreeView::OnPreview(wxCommandEvent& evt)
{
	CLogger::Log(L"MENU: CAgilityBookTreeView::OnPreview");
	wxGetApp().GetHtmlPrinter()->PreviewText(GetPrintDataAsHtmlTable());
}

} // namespace dconSoft
