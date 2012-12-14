/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CAgilityBookTree class
 * @author David Connet
 *
 * Revision History
 * @li 2012-10-03 DRC Fixed a printing problem in the tree.
 * @li 2012-09-29 DRC Strip the Runs View.
 * @li 2012-07-04 DRC Add option to use run time or opening time in gamble OPS.
 * @li 2011-12-22 DRC Switch to using Bind on wx2.9+.
 * @li 2011-10-11 DRC Fixed bug on Mac when deleting via context menu.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
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
#include "AgilityBookTreeData.h"
#include "AgilityBookTreeModel.h"
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
#include "RegItems.h"
#include "StringUtil.h"
#include <wx/config.h>
#include <wx/dataview.h>

#ifdef WX_TREE_HAS_STATE
#include "res/CalEmpty.xpm"
#include "res/CalPlan.xpm"
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// Find

#pragma PRAGMA_TODO("Fix me")
#if 0
void CFindTree::FillTree(wxDataViewItem hItem) const
{
	if (!hItem.IsOk())
		return;

	if (m_pView->m_Ctrl->GetRootItem() != hItem)
		m_Items.push_back(hItem);

	wxTreeItemIdValue cookie;
	wxDataViewItem hChildItem = m_pView->m_Ctrl->GetFirstChild(hItem, cookie);
	while (hChildItem.IsOk())
	{
		FillTree(hChildItem);
		hChildItem = m_pView->m_Ctrl->GetNextChild(hItem, cookie);
	}
}


wxDataViewItem CFindTree::GetNextItem() const
{
	wxDataViewItem hItem;
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
#endif


bool CFindTree::Search(CDlgFind* pDlg) const
{
	bool bFound = false;
#pragma PRAGMA_TODO("Fix me")
#if 0
	m_Items.clear();
	FillTree(m_pView->m_Ctrl->GetRootItem());
	if (0 == m_Items.size())
		return bFound;

	wxDataViewItem hItem = m_pView->m_Ctrl->GetSelection();
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
	std::wstring search = Text();
	if (!MatchCase())
		search = StringUtil::ToLower(search);
	while (!bFound && hItem.IsOk())
	{
		std::set<std::wstring> strings;
		if (SearchAll())
		{
			CAgilityBookTreeData* pData = m_pView->GetTreeItem(hItem);
			if (pData)
				pData->GetARBBase()->GetSearchStrings(strings);
		}
		else
		{
			strings.insert(StringUtil::stringW(m_pView->m_Ctrl->GetItemText(hItem)));
		}
		for (std::set<std::wstring>::iterator iter = strings.begin(); iter != strings.end(); ++iter)
		{
			std::wstring str((*iter));
			if (!MatchCase())
				str = StringUtil::ToLower(str);
			if (std::wstring::npos != str.find(search))
			{
				m_pView->ChangeSelection(hItem);
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
#endif
	return bFound;
}

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(CAgilityBookTreeCtrl, wxDataViewCtrl)


CAgilityBookTreeCtrl::CAgilityBookTreeCtrl()
{
}


CAgilityBookTreeCtrl::CAgilityBookTreeCtrl(
		wxWindow* parent,
		wxWindowID id,
		const wxPoint& pos,
		const wxSize& size,
		long style,
		const wxValidator& validator)
{
	Create(parent, id, pos, size, style, validator);
}


CAgilityBookTreeCtrl::~CAgilityBookTreeCtrl()
{
}


bool CAgilityBookTreeCtrl::Create(
		wxWindow* parent,
		wxWindowID id,
		const wxPoint& pos,
		const wxSize& size,
		long style,
		const wxValidator& validator)
{
	if (!wxDataViewCtrl::Create(parent, id, pos, size, style, validator))
		return false;

	CAgilityBookTreeModel* store = new CAgilityBookTreeModel();
	AssociateModel(store);
	store->DecRef();

	return true;
}


void CAgilityBookTreeCtrl::CollapseAllChildren(wxDataViewItem const& item)
{
	if (!item.IsOk() || !GetStore()->IsContainer(item))
		return;

	wxDataViewItemArray items;
	unsigned int n = GetStore()->GetChildren(item, items);
	for (unsigned int i = 0; i < n; ++i)
	{
		CollapseAllChildren(items[i]);
	}

	if (IsExpanded(item))
		Collapse(item);
}


void CAgilityBookTreeCtrl::ExpandAllChildren(wxDataViewItem const& item)
{
	if (!item.IsOk() || !GetStore()->IsContainer(item))
		return;
	if (!IsExpanded(item))
		Expand(item);

	wxDataViewItemArray items;
	unsigned int n = GetStore()->GetChildren(item, items);
	for (unsigned int i = 0; i < n; ++i)
	{
		ExpandAllChildren(items[i]);
	}
}


CAgilityBookTreeModel* CAgilityBookTreeCtrl::GetStore()
{
	return (CAgilityBookTreeModel*)GetModel();
}


const CAgilityBookTreeModel* CAgilityBookTreeCtrl::GetStore() const
{
	return (const CAgilityBookTreeModel*)GetModel();
}

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(CAgilityBookTreeView, CAgilityBookBaseExtraView)


#ifdef ARB_HAS_PRAGMAPUSHPOP
#pragma warning (push)
#pragma warning (disable : 4355)
#endif
CAgilityBookTreeView::CAgilityBookTreeView(
		CTabView* pTabView,
		wxDocument* doc)
	: CAgilityBookBaseExtraView(pTabView, doc)
	, m_Ctrl(NULL)
	//, m_ImageList()
#ifdef WX_TREE_HAS_STATE
	//, m_ImageListStates(16,16)
	//, m_idxEmpty(-1)
	//, m_idxChecked(-1)
#endif
	, m_Callback(this)
	, m_pDog()
{
#pragma PRAGMA_TODO("Fix me")
#if 0
#ifdef WX_TREE_HAS_STATE
	// Note: Position 0 cannot be used.
	m_ImageListStates.Add(wxIcon(CalEmpty_xpm));
	m_idxEmpty = m_ImageListStates.Add(wxIcon(CalEmpty_xpm));
	m_idxChecked = m_ImageListStates.Add(wxIcon(CalPlan_xpm));
#endif
#endif
}
#ifdef ARB_HAS_PRAGMAPUSHPOP
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
	m_Ctrl = new CAgilityBookTreeCtrl(parentCtrl, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEventHandler, CAgilityBookTreeView::OnContextMenu);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler, CAgilityBookTreeView::OnSelectionChanged);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler, CAgilityBookTreeView::OnItemActivated);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_KEY_DOWN, wxKeyEventHandler, CAgilityBookTreeView::OnKeyDown);

	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, wxID_DUPLICATE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, wxID_DUPLICATE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, wxID_CUT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, wxID_CUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, wxID_COPY, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, wxID_PASTE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, wxID_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_REORDER, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_REORDER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, wxID_FIND, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, wxID_FIND, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_EDIT_FIND_NEXT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_EDIT_FIND_NEXT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_EDIT_FIND_PREVIOUS, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_EDIT_FIND_PREVIOUS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_EDIT_DOG, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_EDIT_DOG, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_NEW_DOG, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_NEW_DOG, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_DELETE_DOG, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_DELETE_DOG, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_NEW_TITLE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_NEW_TITLE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_EDIT_TRIAL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_EDIT_TRIAL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_NEW_TRIAL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_NEW_TRIAL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_DELETE_TRIAL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_DELETE_TRIAL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_PRINT_TRIAL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_PRINT_TRIAL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_EDIT_RUN, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_EDIT_RUN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_NEW_RUN, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_NEW_RUN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_DELETE_RUN, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_DELETE_RUN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_PRINT_RUNS, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_AGILITY_PRINT_RUNS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_VIEW_CUSTOMIZE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_VIEW_CUSTOMIZE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_VIEW_SORTRUNS, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_VIEW_SORTRUNS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_VIEW_RUNS_BY_TRIAL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_VIEW_RUNS_BY_TRIAL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_VIEW_TABLE_IN_YPS, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_VIEW_TABLE_IN_YPS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_VIEW_RUNTIME_IN_OPS, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_VIEW_RUNTIME_IN_OPS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_EXPAND, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_EXPAND, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_COLLAPSE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_COLLAPSE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_EXPAND_ALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_EXPAND_ALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_COLLAPSE_ALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnViewUpdateCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, ID_COLLAPSE_ALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCmd);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, wxID_PRINT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnPrint);
	BIND_OR_CONNECT_ID_CTRL(m_Ctrl, wxID_PREVIEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnPreview);

#pragma PRAGMA_TODO("Fix me")
	//m_Ctrl->SetImageList(&m_ImageList);
#ifdef WX_TREE_HAS_STATE
	//m_Ctrl->SetStateImageList(&m_ImageListStates);
#endif
#if defined(__WXMAC__)
	m_Ctrl->SetDropTarget(new CFileDropTarget(doc->GetDocumentManager()));
#endif
	return CAgilityBookBaseExtraView::Create(parentView, parentCtrl, doc, flags, sizer, proportion, sizerFlags, border);
}


wxWindow* CAgilityBookTreeView::GetControl()
{
	return m_Ctrl;
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
	assert(m_Ctrl);
	m_Ctrl->GetStore()->CreateColumns(m_Ctrl, (CAgilityBookDoc*)doc);
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
		hint = wxDynamicCast(inHint, CUpdateHint);
	if (!hint || hint->IsSet(UPDATE_TREE_VIEW)
	|| hint->IsEqual(UPDATE_CONFIG) || hint->IsEqual(UPDATE_OPTIONS))
	{
		LoadData(false);
	}
	else if (hint && hint->IsEqual(UPDATE_NEW_TRIAL))
	{
		LoadData(false);
		ARBDogTrialPtr pTrial = std::tr1::dynamic_pointer_cast<ARBDogTrial, ARBBase>(hint->GetObj());
		assert(pTrial);
		wxDataViewItem pData = FindData(pTrial);
		ChangeSelection(pData);
	}
	else if (hint && hint->IsEqual(UPDATE_CUSTOMIZE))
	{
		LoadData(true);
	}
	else if (hint && hint->IsEqual(UPDATE_LANG_CHANGE))
	{
		LoadData(true);
	}
}


bool CAgilityBookTreeView::IsFiltered() const
{
	return CFilterOptions::Options().IsFilterEnabled();
}


bool CAgilityBookTreeView::GetMessage(std::wstring& msg) const
{
	if (!m_Ctrl)
		return false;
	int nQs = 0;
	int nTotal = 0;
	GetQCount(nQs, nTotal);
	msg = StringUtil::stringW(wxString::Format(_("IDS_NUM_RUNS_QS"), nTotal, nQs));
	return true;
}


bool CAgilityBookTreeView::GetMessage2(std::wstring& msg) const
{
	if (GetDocument()->GetCurrentDog())
	{
		msg = GetDocument()->GetCurrentDog()->GetCallName();
		return true;
	}
	else
	{
		msg.clear();
		return false;
	}
}


bool CAgilityBookTreeView::AllowStatusContext(int field) const
{
	return STATUS_DOG == field || STATUS_FILTERED == field;
}


CAgilityBookTreeData* CAgilityBookTreeView::GetCurrentTreeItem() const
{
	return GetTreeItem(m_Ctrl->GetSelection());
}


CAgilityBookTreeData* CAgilityBookTreeView::GetTreeItem(wxDataViewItem const& item) const
{
	if (m_Ctrl && item.IsOk())
		return m_Ctrl->GetStore()->GetNode(item);
	return NULL;
}


wxDataViewItem CAgilityBookTreeView::FindData(ARBBasePtr pBase) const
{
	wxDataViewItem item;
	wxDataViewItemArray roots;
	unsigned int n = m_Ctrl->GetStore()->GetChildren(wxDataViewItem(), roots);
	for (unsigned int i = 0; !item.IsOk() && i < n; ++i)
	{
		item = FindData(roots[i], pBase);
	}
	return item;
}


wxDataViewItem CAgilityBookTreeView::FindData(
		wxDataViewItem const& inItem,
		ARBBasePtr pBase) const
{
	wxDataViewItem item;
	if (pBase && m_Ctrl && inItem.IsOk())
	{
		CAgilityBookTreeData* pData = m_Ctrl->GetStore()->GetNode(inItem);
		if (pData->GetARBBase() == pBase)
			item = inItem;
		else
		{
			wxDataViewItemArray kids;
			unsigned int n = m_Ctrl->GetStore()->GetChildren(inItem, kids);
			for (unsigned int i = 0; !item.IsOk() && i < n; ++i)
			{
				item = FindData(kids[i], pBase);
			}
		}
	}
	return item;
}


wxDataViewItem CAgilityBookTreeView::FindData(ARBDogPtr pDog) const
{
	wxDataViewItem item;
	wxDataViewItemArray roots;
	unsigned int n = m_Ctrl->GetStore()->GetChildren(wxDataViewItem(), roots);
	for (unsigned int i = 0; !item.IsOk() && i < n; ++i)
	{
		item = FindData(roots[i], pDog);
	}
	return item;
}


wxDataViewItem CAgilityBookTreeView::FindData(
		wxDataViewItem const& inItem,
		ARBDogPtr pDog) const
{
	wxDataViewItem item;
	if (pDog && m_Ctrl && inItem.IsOk())
	{
		CAgilityBookTreeData* pData = m_Ctrl->GetStore()->GetNode(inItem);
		if (CAgilityBookTreeData::eTreeDog == pData->Type())
		{
			CAgilityBookTreeDataDog* pCheck = dynamic_cast<CAgilityBookTreeDataDog*>(pData);
			// Don't use virtual 'GetDog' on pCheck as that can give back a parent
			// node which isn't what we want here. We want the real dog node.
			if (pCheck && pCheck->GetDog() == pDog)
				item = inItem;
		}
	}
	return item;
}


wxDataViewItem CAgilityBookTreeView::FindData(ARBDogTrialPtr pTrial) const
{
	wxDataViewItem item;
	wxDataViewItemArray roots;
	unsigned int n = m_Ctrl->GetStore()->GetChildren(wxDataViewItem(), roots);
	for (unsigned int i = 0; !item.IsOk() && i < n; ++i)
	{
		item = FindData(roots[i], pTrial);
	}
	return item;
}


wxDataViewItem CAgilityBookTreeView::FindData(
		wxDataViewItem const& inItem,
		ARBDogTrialPtr pTrial) const
{
	wxDataViewItem item;
	if (pTrial && m_Ctrl && inItem.IsOk())
	{
		CAgilityBookTreeData* pData = m_Ctrl->GetStore()->GetNode(inItem);
		switch (pData->Type())
		{
		case CAgilityBookTreeData::eTreeDog:
			{
				wxDataViewItemArray kids;
				unsigned int n = m_Ctrl->GetStore()->GetChildren(inItem, kids);
				for (unsigned int i = 0; !pData && i < n; ++i)
				{
					item = FindData(kids[i], pTrial);
				}
			}
			break;
		case CAgilityBookTreeData::eTreeTrial:
			{
				CAgilityBookTreeDataTrial* pCheck = dynamic_cast<CAgilityBookTreeDataTrial*>(pData);
				if (pCheck && pCheck->GetTrial() == pTrial)
					item = inItem;
			}
			break;
		}
	}
	return item;
}


wxDataViewItem CAgilityBookTreeView::FindData(ARBDogRunPtr pRun) const
{
	wxDataViewItem item;
	wxDataViewItemArray roots;
	unsigned int n = m_Ctrl->GetStore()->GetChildren(wxDataViewItem(), roots);
	for (unsigned int i = 0; !item.IsOk() && i < n; ++i)
	{
		item = FindData(roots[i], pRun);
	}
	return item;
}


wxDataViewItem CAgilityBookTreeView::FindData(
		wxDataViewItem const& inItem,
		ARBDogRunPtr pRun) const
{
	wxDataViewItem item;
	if (pRun && m_Ctrl && inItem.IsOk())
	{
		CAgilityBookTreeData* pData = m_Ctrl->GetStore()->GetNode(inItem);
		switch (pData->Type())
		{
		case CAgilityBookTreeData::eTreeDog:
		case CAgilityBookTreeData::eTreeTrial:
			{
				wxDataViewItemArray kids;
				unsigned int n = m_Ctrl->GetStore()->GetChildren(inItem, kids);
				for (unsigned int i = 0; !item.IsOk() && i < n; ++i)
				{
					item = FindData(kids[i], pRun);
				}
			}
			break;
		case CAgilityBookTreeData::eTreeRun:
			{
				CAgilityBookTreeDataRun* pCheck = dynamic_cast<CAgilityBookTreeDataRun*>(pData);
				if (pCheck && pCheck->GetRun() == pRun)
					item = inItem;
			}
			break;
		}
	}
	return item;
}


bool CAgilityBookTreeView::SelectDog(ARBDogPtr dog)
{
	bool bSelected = false;
	if (m_Ctrl)
	{
		wxDataViewItem item = FindData(dog);
		if (item.IsOk())
		{
			bSelected = true;
			m_Ctrl->Select(item);
			m_Ctrl->EnsureVisible(item);
		}
	}
	return bSelected;
}


wxDataViewItem CAgilityBookTreeView::InsertDog(
		ARBDogPtr pDog,
		bool bSelect)
{
	wxDataViewItem item;
	if (pDog && m_Ctrl)
	{
		item = m_Ctrl->GetStore()->LoadData(pDog);
		if (bSelect)
		{
			ChangeSelection(item);
		}
	}
	return item;
}


wxDataViewItem CAgilityBookTreeView::InsertTrial(
		ARBDogTrialPtr pTrial,
		wxDataViewItem parent)
{
	wxDataViewItem trial;
	if (pTrial && !pTrial->IsFiltered() && m_Ctrl)
	{
		trial = m_Ctrl->GetStore()->LoadData(pTrial, parent);
	}
	return trial;
}


wxDataViewItem CAgilityBookTreeView::InsertRun(
		ARBDogRunPtr pRun,
		wxDataViewItem parent)
{
	wxDataViewItem run;
	if (pRun && !pRun->IsFiltered() && m_Ctrl)
	{
		run = m_Ctrl->GetStore()->LoadData(pRun, parent);
	}
	return run;
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
						CUpdateHint hint(UPDATE_POINTS_VIEW | UPDATE_TREE_VIEW);
						GetDocument()->UpdateAllViews(NULL, &hint);
					}
				}
				else if (0 < err.m_ErrMsg.str().length())
					wxMessageBox(StringUtil::stringWX(err.m_ErrMsg.str()), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
			}
		}
		return true;
	}
	return false;
}


std::wstring CAgilityBookTreeView::GetPrintLine(wxDataViewItem const& item) const
{
	if (item.IsOk() && m_Ctrl)
		return StringUtil::stringW(m_Ctrl->GetStore()->GetPrintLine(item));
	return std::wstring();
}


void CAgilityBookTreeView::GetQCount(
		int& ioCount,
		int& ioTotal) const
{
	wxDataViewItem item;
	wxDataViewItemArray roots;
	unsigned int nRoots = m_Ctrl->GetStore()->GetChildren(wxDataViewItem(), roots);
	for (unsigned int iRoot = 0; iRoot < nRoots; ++iRoot)
	{
		wxDataViewItemArray trials;
		unsigned int nTrials = m_Ctrl->GetStore()->GetChildren(roots[iRoot], trials);
		for (unsigned int iTrial = 0; iTrial < nTrials; ++iTrial)
		{
			wxDataViewItemArray runs;
			unsigned int nRuns = m_Ctrl->GetStore()->GetChildren(trials[iTrial], runs);
			for (unsigned int iRun = 0; iRun < nRuns; ++iRun)
			{
				CAgilityBookTreeData* pData = m_Ctrl->GetStore()->GetNode(runs[iRun]);
				if (pData && pData->GetRun() && pData->GetRun()->GetQ().Qualified())
					++ioCount;
			}
			ioTotal += static_cast<int>(nRuns);
		}
	}
}


void CAgilityBookTreeView::ChangeSelection(
		wxDataViewItem const& item,
		bool bEnsureVisible)
{
	m_Ctrl->Select(item);
	if (bEnsureVisible)
		m_Ctrl->EnsureVisible(item);
	DoSelectionChange(item);
}


void CAgilityBookTreeView::DoSelectionChange(wxDataViewItem const& item)
{
	CAgilityBookTreeData* pData = m_Ctrl->GetStore()->GetNode(item);
	ARBBasePtr pBase;
	unsigned int iHint = 0;
	if (pData)
	{
		// Set the current dog
		ARBDogPtr pDog = pData->GetDog();
		if (!m_pDog || !pDog || m_pDog != pDog)
			iHint |= UPDATE_POINTS_VIEW;
		m_pDog = pDog;
		// Pass the selected run
		if (CAgilityBookTreeData::eTreeRun == pData->Type())
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


void CAgilityBookTreeView::LoadData(bool bColumns)
{
	if (!m_Ctrl)
		return;
	if (bColumns)
		m_Ctrl->GetStore()->UpdateColumns();
	m_Ctrl->GetStore()->LoadData();
}


void CAgilityBookTreeView::PrintLine(
		std::wostringstream& data,
		wxDataViewItem item,
		int indent) const
{
	static std::wstring const spaces(L"&nbsp;&nbsp;&nbsp;");
	if (item.IsOk())
	{
		for (int idx = 0; idx < indent; ++idx)
			data << spaces;
		data << m_Ctrl->GetStore()->GetPrintLine(item).wx_str() << L"<br />\n"; // Note, wxWidgets needs the space before the slash
	}

	wxDataViewItemArray kids;
	unsigned int n = m_Ctrl->GetStore()->GetChildren(item, kids);
	for (unsigned int i = 0; i < n; ++i)
	{
		PrintLine(data, kids[i], indent + 1);
	}
}


std::wstring CAgilityBookTreeView::GetPrintDataAsHtmlTable() const
{
	std::wostringstream data;
	data << L"<html><body><p>\n";
	wxDataViewItemArray roots;
	unsigned int n = m_Ctrl->GetStore()->GetChildren(wxDataViewItem(), roots);
	for (unsigned int i = 0; i < n; ++i)
	{
		PrintLine(data, roots[i], -1);
	}
	data << L"</p></body></html>\n";
	return data.str();
}


void CAgilityBookTreeView::OnContextMenu(wxDataViewEvent& evt)
{
	bool bSkip = true;

	wxDataViewItem item = evt.GetItem();
	if (!item.IsOk())
	{
		item = m_Ctrl->GetSelection();
	}

	CAgilityBookTreeData* pData = GetTreeItem(item);
	if (pData)
	{
		bSkip = false;
		if (item == evt.GetItem())
		{
			// If it's the same one, don't reset.
			// (or we die on delete on a Mac)
			//TODO: Check this with new DVC
			item = wxDataViewItem();
		}
		else
		{
			// On a r-click context, the highlighting is set, but the
			// current item doesn't change. The wxTreeCtrl api does not
			// appear to have the concept of the current highlight item
			// like windows does. So we'll force the current item, then
			// reset. But ignore the changes! (do not use the changing msg
			// and veto it - that kills the change!)
			m_Ctrl->Select(evt.GetItem());
		}
		wxMenu* menu = CreatePopup(pData->GetMenuID());
		if (menu)
		{
			m_Ctrl->PopupMenu(menu, evt.GetPosition());
			delete menu;
		}
		if (item.IsOk())
			m_Ctrl->Select(item);
	}

	if (bSkip)
		evt.Skip();
}


void CAgilityBookTreeView::OnSelectionChanged(wxDataViewEvent& evt)
{
	DoSelectionChange(evt.GetItem());
}


void CAgilityBookTreeView::OnItemActivated(wxDataViewEvent& evt)
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


void CAgilityBookTreeView::OnKeyDown(wxKeyEvent& evt)
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
			wxDataViewItem item = m_Ctrl->GetSelection();
			if (item.IsOk() && m_Ctrl->GetStore()->IsContainer(item))
			{
				if (!m_Ctrl->IsExpanded(item))
					bEnable = true;
			}
		}
		break;
	case ID_EXPAND_ALL:
		{
			wxDataViewItem item = m_Ctrl->GetSelection();
			if (item.IsOk() && m_Ctrl->GetStore()->IsContainer(item))
				bEnable = true;
		}
		break;
	case ID_COLLAPSE:
		{
			wxDataViewItem item = m_Ctrl->GetSelection();
			if (item.IsOk() && m_Ctrl->GetStore()->IsContainer(item))
			{
				if (m_Ctrl->IsExpanded(item))
					bEnable = true;
			}
		}
		break;
	case ID_COLLAPSE_ALL:
		{
			wxDataViewItem item = m_Ctrl->GetSelection();
			if (item.IsOk() && m_Ctrl->GetStore()->IsContainer(item))
				bEnable = true;
		}
		break;
	case ID_VIEW_CUSTOMIZE:
	case ID_VIEW_SORTRUNS:
	case ID_VIEW_RUNS_BY_TRIAL:
	case ID_VIEW_TABLE_IN_YPS:
	case ID_VIEW_RUNTIME_IN_OPS:
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
						LoadData(false);
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
			if (m_Callback.Text().empty())
				OnCmd(wxID_FIND);
			else
				m_Callback.Search(NULL);
		}
		break;

	case ID_EDIT_FIND_PREVIOUS:
		bHandled = true;
		{
			m_Callback.SearchDown(false);
			if (m_Callback.Text().empty())
				OnCmd(wxID_FIND);
			else
				m_Callback.Search(NULL);
		}
		break;

	case ID_EXPAND:
		bHandled = true;
		{
			wxDataViewItem item = m_Ctrl->GetSelection();
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
			wxDataViewItem item = m_Ctrl->GetSelection();
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
			wxDataViewItem item = m_Ctrl->GetSelection();
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
			wxDataViewItem item = m_Ctrl->GetSelection();
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
		LoadData(false);
		break;

	case ID_VIEW_RUNS_BY_TRIAL:
		bHandled = true;
		CAgilityBookOptions::SetViewRunsByTrial(!CAgilityBookOptions::GetViewRunsByTrial());
		break;

	case ID_VIEW_TABLE_IN_YPS:
		bHandled = true;
		CAgilityBookOptions::SetTableInYPS(!CAgilityBookOptions::GetTableInYPS());
		break;

	case ID_VIEW_RUNTIME_IN_OPS:
		bHandled = true;
		CAgilityBookOptions::SetRunTimeInOPS(!CAgilityBookOptions::GetRunTimeInOPS());
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
	wxGetApp().GetHtmlPrinter()->PrintText(StringUtil::stringWX(GetPrintDataAsHtmlTable()));
}


void CAgilityBookTreeView::OnPreview(wxCommandEvent& evt)
{
	wxGetApp().GetHtmlPrinter()->PreviewText(StringUtil::stringWX(GetPrintDataAsHtmlTable()));
}
