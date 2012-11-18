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
	EVT_MENU(wxID_PRINT, CAgilityBookTreeView::OnPrint)
	EVT_MENU(wxID_PREVIEW, CAgilityBookTreeView::OnPreview)
END_EVENT_TABLE()


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
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, wxDataViewEventHandler, CAgilityBookTreeView::OnCtrlContextMenu);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler, CAgilityBookTreeView::OnCtrlSelectionChanged);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler, CAgilityBookTreeView::OnCtrlItemActivated);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_KEY_DOWN, wxKeyEventHandler, CAgilityBookTreeView::OnCtrlKeyDown);
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
		LoadData();
	}
	else if (hint && hint->IsEqual(UPDATE_NEW_TRIAL))
	{
		LoadData();
		ARBDogTrialPtr pTrial = std::tr1::dynamic_pointer_cast<ARBDogTrial, ARBBase>(hint->GetObj());
		assert(pTrial);
#pragma PRAGMA_TODO("Fix me")
#if 0
		CAgilityBookTreeData* pData = FindData(pTrial);
		assert(pData);
		ChangeSelection(pData->GetId());
#endif
	}
	else if (hint && hint->IsEqual(UPDATE_CUSTOMIZE))
	{
#pragma PRAGMA_TODO("Fix me")
		//CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_DOG, m_Columns[0]);
		//CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_TRIAL, m_Columns[1]);
		//CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TREE_RUN, m_Columns[2]);
		//UpdateData(m_Ctrl->GetRootItem());
	}
	else if (hint && hint->IsEqual(UPDATE_LANG_CHANGE))
	{
#pragma PRAGMA_TODO("Fix me")
		//RefreshTreeItem(m_Ctrl, m_Ctrl->GetRootItem(), true);
	}
}


bool CAgilityBookTreeView::IsFiltered() const
{
	return CFilterOptions::Options().IsFilterEnabled();
}


bool CAgilityBookTreeView::GetMessage(std::wstring& msg) const
{
#pragma PRAGMA_TODO("Fix me")
	// runs view code
	/*
{
	if (!m_Ctrl)
		return false;
	int nQs = 0;
	for (int index = 0; index < m_Ctrl->GetItemCount(); ++index)
	{
		CAgilityBookRunsViewDataPtr pData = GetItemRunData(index);
		if (pData && pData->GetRun() && pData->GetRun()->GetQ().Qualified())
			++nQs;
	}
	msg = StringUtil::stringW(wxString::Format(_("IDS_NUM_RUNS_QS"), m_Ctrl->GetItemCount(), nQs));
	return true;
}
*/
	_("IDS_NUM_RUNS_QS"); // just here so poedit sees it
	msg.clear();
	return false;
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


#pragma PRAGMA_TODO("Fix me")
#if 0
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
#endif


bool CAgilityBookTreeView::SelectDog(ARBDogPtr dog)
{
	bool bSelected = false;
	if (m_Ctrl)
	{
#pragma PRAGMA_TODO("Fix me")
#if 0
		CAgilityBookTreeData* pData = FindData(m_Ctrl->GetRootItem(), dog);
		if (pData)
		{
			bSelected = true;
			m_Ctrl->SelectItem(pData->GetId());
			m_Ctrl->EnsureVisible(pData->GetId());
		}
#endif
	}
	return bSelected;
}


bool CAgilityBookTreeView::PasteDog(bool& bLoaded)
{
#pragma PRAGMA_TODO("Fix me")
#if 0
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
#endif
	return false;
}


std::wstring CAgilityBookTreeView::GetPrintLine(long item) const
{
#pragma PRAGMA_TODO("Fix me")
	//if (hItem.IsOk() && m_Ctrl)
	//	return StringUtil::stringW(m_Ctrl->GetItemText(hItem));
	return std::wstring();
}


void CAgilityBookTreeView::UpdateData(long item)
{
#pragma PRAGMA_TODO("Fix me")
#if 0
	if (m_Ctrl && m_Ctrl->GetRootItem() != hItem)
	{
		CAgilityBookTreeData* pData = dynamic_cast<CAgilityBookTreeData*>(m_Ctrl->GetItemData(hItem));
		if (pData && pData->GetId().IsOk())
		{
			m_Ctrl->SetItemText(pData->GetId(), StringUtil::stringWX(pData->OnNeedText()));
		}
	}
	wxTreeItemIdValue cookie;
	wxTreeItemId hChildItem = m_Ctrl->GetFirstChild(hItem, cookie);
	while (hChildItem.IsOk())
	{
		UpdateData(hChildItem);
		hChildItem = m_Ctrl->GetNextChild(hItem, cookie);
	}
#endif
}


#pragma PRAGMA_TODO("Fix me")
#if 0
void CAgilityBookTreeView::ChangeSelection(
		wxTreeItemId hItem,
		bool bEnsureVisible)
{
	m_Ctrl->SelectItem(hItem);
	if (bEnsureVisible)
		m_Ctrl->EnsureVisible(hItem);
	DoSelectionChange(hItem);
}


void CAgilityBookTreeView::DoSelectionChange(wxTreeItemId hItem)
{
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
			// Pass the selected run
			if (CAgilityBookTreeData::eTreeRun == pData->GetType())
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
#endif


void CAgilityBookTreeView::LoadData()
{
	if (!m_Ctrl)
		return;
	m_Ctrl->GetStore()->LoadData();
}


void CAgilityBookTreeView::PrintLine(
		std::wostringstream& data,
		long id,
		int indent) const
{
#pragma PRAGMA_TODO("Fix me")
#if 0
	static std::wstring const spaces(L"&nbsp;&nbsp;&nbsp;");
	if (id.IsOk() && id != m_Ctrl->GetRootItem())
	{
		for (int idx = 0; idx < indent; ++idx)
			data << spaces;
		data << m_Ctrl->GetItemText(id).wx_str() << L"<br />\n"; // Note, wxWidgets needs the space before the slash
	}
	wxTreeItemIdValue cookie;
	wxTreeItemId hChildItem = m_Ctrl->GetFirstChild(id, cookie);
	while (hChildItem.IsOk())
	{
		PrintLine(data, hChildItem, indent + 1);
		hChildItem = m_Ctrl->GetNextChild(id, cookie);
	}
#endif
}


std::wstring CAgilityBookTreeView::GetPrintDataAsHtmlTable() const
{
	std::wostringstream data;
	data << L"<html><body><p>\n";
#pragma PRAGMA_TODO("Fix me")
#if 0
	PrintLine(data, m_Ctrl->GetRootItem(), -1);
#endif
	data << L"</p></body></html>\n";
	return data.str();
}


void CAgilityBookTreeView::OnCtrlContextMenu(wxDataViewEvent& evt)
{
	bool bSkip = true;
#pragma PRAGMA_TODO("Fix me")
#if 0
	wxPoint point;
	if (GetMenuPosition(point, *this, evt))
	{
		CAgilityBookTreeData* pData = GetTreeItem(evt.GetItem());
		if (pData)
		{
			bSkip = false;
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
			wxMenu* menu = CreatePopup(pData->GetMenuID());
			if (menu)
			{
				m_Ctrl->PopupMenu(menu, point);
				delete menu;
			}
			if (item.IsOk())
				m_Ctrl->SelectItem(item);
		}
	}
#endif
	if (bSkip)
		evt.Skip();
}


void CAgilityBookTreeView::OnCtrlSelectionChanged(wxDataViewEvent& evt)
{
#pragma PRAGMA_TODO("Fix me")
	//DoSelectionChange(evt.GetItem());
}


void CAgilityBookTreeView::OnCtrlItemActivated(wxDataViewEvent& evt)
{
#pragma PRAGMA_TODO("Fix me")
#if 0
	CAgilityBookTreeData* pData = GetCurrentTreeItem();
	if (pData)
	{
		// eat the dbl-click when we do this - it's consistent with v1.x
		pData->Properties();
	}
	else
#endif
		evt.Skip();
}


void CAgilityBookTreeView::OnCtrlKeyDown(wxKeyEvent& evt)
{
		evt.Skip();
#pragma PRAGMA_TODO("Fix me")
#if 0
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
#endif
}


void CAgilityBookTreeView::OnViewContextMenu(wxContextMenuEvent& evt)
{
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
#pragma PRAGMA_TODO("Fix me")
#if 0
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
#endif
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
#pragma PRAGMA_TODO("Fix me")
#if 0
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
	case ID_VIEW_RUNTIME_IN_OPS:
		bEnable = true;
		break;
	}
#endif
	evt.Enable(bEnable);
}


bool CAgilityBookTreeView::OnCmd(int id)
{
	if (!m_Ctrl)
		return false;

	bool bHandled = false;
#pragma PRAGMA_TODO("Fix me")
#if 0
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
#endif

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
