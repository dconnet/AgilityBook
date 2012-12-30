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
#include "AgilityBookTreeModel.h"
#include "ARBDog.h"
#include "ARBDogRun.h"
#include "ARBDogTrial.h"
#include "ClipBoard.h"
#include "DlgAssignColumns.h"
#include "DlgDog.h"
#include "DlgFind.h"
#include "DlgOptions.h"
#include "DlgReorder.h"
#include "DlgRun.h"
#include "DlgTrial.h"
#include "Element.h"
#include "FilterOptions.h"
#include "Globals.h"
#include "MainFrm.h"
#include "Print.h"
#include "RegItems.h"
#include "StringUtil.h"
#include <wx/config.h>
#include <wx/dataview.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// Find

void CFindTree::FillTree(wxDataViewItem item) const
{
	if (!item.IsOk())
		return;

	m_Items.push_back(item);

	wxDataViewItemArray items;
	unsigned int n = m_pView->GetStore()->GetChildren(item, items);
	for (unsigned int i = 0; i < n; ++i)
	{
		FillTree(items[i]);
	}
}


wxDataViewItem CFindTree::GetNextItem() const
{
	wxDataViewItem item;
	if (SearchDown())
	{
		++m_Iter;
		if (m_Iter == m_Items.end())
			item.Unset();
		else
			item = *m_Iter;
	}
	else
	{
		if (m_Iter == m_Items.begin())
			item.Unset();
		else
		{
			--m_Iter;
			item = *m_Iter;
		}
	}
	return item;
}


bool CFindTree::Search(CDlgFind* pDlg) const
{
	bool bFound = false;

	m_Items.clear();

	wxDataViewItemArray items;
	unsigned int n = m_pView->GetStore()->GetChildren(wxDataViewItem(), items);
	if (0 == n)
		return bFound;

	for (unsigned int i = 0; i < n; ++i)
	{
		FillTree(items[i]);
	}

	wxDataViewItem item = m_pView->m_Ctrl->GetSelection();
	if (!item.IsOk())
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
			if (*m_Iter == item)
				break;
		}
		item = GetNextItem();
	}
	std::wstring search = Text();
	if (!MatchCase())
		search = StringUtil::ToLower(search);
	while (!bFound && item.IsOk())
	{
		std::set<std::wstring> strings;
		if (SearchAll())
		{
			ARBBasePtr pBase = m_pView->GetStore()->GetARBBase(item);
			if (pBase)
				pBase->GetSearchStrings(strings);
		}
		else
		{
			strings.insert(StringUtil::stringW(m_pView->m_Ctrl->GetStore()->GetPrintLine(item)));
		}
		for (std::set<std::wstring>::iterator iter = strings.begin(); iter != strings.end(); ++iter)
		{
			std::wstring str((*iter));
			if (!MatchCase())
				str = StringUtil::ToLower(str);
			if (std::wstring::npos != str.find(search))
			{
				m_pView->ChangeSelection(item);
				bFound = true;
			}
		}
		item = GetNextItem();
	}
	if (!bFound)
	{
		wxString msg = wxString::Format(_("IDS_CANNOT_FIND"), m_strSearch.c_str());
		wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
	}
	return bFound;
}

/////////////////////////////////////////////////////////////////////////////

static bool EditRun(
		ARBDogPtr pDog,
		ARBDogTrialPtr pTrial,
		ARBDogRunPtr pRun,
		CAgilityBookTreeView* pTree)
{
	assert(pDog);
	assert(pTrial);
	bool bOk = false;
	bool bAdd = false;
	if (!pRun)
	{
		ARBDogClubPtr pClub;
		if (!pTrial->GetClubs().GetPrimaryClub(&pClub))
		{
			wxMessageBox(_("IDS_NEED_CLUB"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
			return false;
		}
		if (!pTree->GetDocument()->Book().GetConfig().GetVenues().FindVenue(pClub->GetVenue()))
		{
			wxMessageBox(wxString::Format(_("IDS_VENUE_CONFIG_MISSING"), pClub->GetVenue().c_str()),
				wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
			return false;
		}
		bAdd = true;
		pRun = ARBDogRunPtr(ARBDogRun::New());
		ARBDate date = pTrial->GetRuns().GetEndDate();
		// If this is the first run, the date won't be set.
		if (!date.IsValid())
			date.SetToday();
		pRun->SetDate(date);
	}
	CDlgRun dlg(pTree->GetDocument(), pTrial, pRun);
	if (wxID_OK == dlg.ShowModal())
	{
		bOk = true;
		std::vector<CVenueFilter> venues;
		CFilterOptions::Options().GetFilterVenue(venues);
		if (bAdd)
		{
			if (pTrial->GetRuns().AddRun(pRun))
			{
				// When adding a new trial, we need to reset the multiQs.
				// The edit dialog does it in OnOK, but we don't add the run
				// until after the dialog is done.
				pTrial->SetMultiQs(pTree->GetDocument()->Book().GetConfig());
				pTrial->GetRuns().sort();
				pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
				pTree->GetDocument()->ResetVisibility(venues, pTrial, pRun);
				// Even though we will reset the tree, go ahead and add/select
				// the item into the tree here. That will make sure when the
				// tree is reloaded, that the new item is selected.
				wxDataViewItem item = pTree->InsertRun(pRun, pTree->FindData(pTrial));
				if (item.IsOk())
				{
					pTree->Select(item);
				}
				else
				{
					if (CFilterOptions::Options().IsFilterEnabled())
						wxMessageBox(_("IDS_CREATERUN_FILTERED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
				}
			}
			else
			{
				bOk = false;
				wxMessageBox(_("IDS_CREATERUN_FAILED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
			}
		}
		else
		{
			pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
			pTree->GetDocument()->ResetVisibility(venues, pTrial, pRun);
		}
		// We have to update the tree even when we add above as it may have
		// caused the trial to be reordered.
		if (bOk)
		{
			CUpdateHint hint(UPDATE_TREE_VIEW | UPDATE_POINTS_VIEW);
			pTree->GetDocument()->UpdateAllViews(NULL, &hint);
		}
	}
	return bOk;
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
	, m_Callback(this)
	, m_pDog()
	, m_bSuppressPrompt(false)
{
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
	BIND_OR_CONNECT_ID(wxID_PRINT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnPrint);
	BIND_OR_CONNECT_ID(wxID_PREVIEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnPreview);
	BIND_OR_CONNECT_ID(wxID_DUPLICATE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateDuplicate);
	BIND_OR_CONNECT_ID(wxID_DUPLICATE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnDuplicate);
	BIND_OR_CONNECT_ID(wxID_CUT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateCut);
	BIND_OR_CONNECT_ID(wxID_CUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnCut);
	BIND_OR_CONNECT_ID(wxID_COPY, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateCopy);
	BIND_OR_CONNECT_ID(wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnCopy);
	BIND_OR_CONNECT_ID(wxID_PASTE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdatePaste);
	BIND_OR_CONNECT_ID(wxID_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnPaste);
	BIND_OR_CONNECT_ID(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateDisable);
	BIND_OR_CONNECT_ID(ID_REORDER, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateReorder);
	BIND_OR_CONNECT_ID(ID_REORDER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnReorder);
	BIND_OR_CONNECT_ID(wxID_FIND, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateEnable);
	BIND_OR_CONNECT_ID(wxID_FIND, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnFind);
	BIND_OR_CONNECT_ID(ID_EDIT_FIND_NEXT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateEnable);
	BIND_OR_CONNECT_ID(ID_EDIT_FIND_NEXT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnFindNext);
	BIND_OR_CONNECT_ID(ID_EDIT_FIND_PREVIOUS, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateEnable);
	BIND_OR_CONNECT_ID(ID_EDIT_FIND_PREVIOUS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnFindPrevious);
	BIND_OR_CONNECT_ID(ID_AGILITY_NEW_DOG, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateNewDog);
	BIND_OR_CONNECT_ID(ID_AGILITY_NEW_DOG, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnNewDog);
	BIND_OR_CONNECT_ID(ID_AGILITY_EDIT_DOG, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateEditDog);
	BIND_OR_CONNECT_ID(ID_AGILITY_EDIT_DOG, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnEditDog);
	BIND_OR_CONNECT_ID(ID_AGILITY_DELETE_DOG, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateDelete);
	BIND_OR_CONNECT_ID(ID_AGILITY_DELETE_DOG, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnDelete);
	BIND_OR_CONNECT_ID(ID_AGILITY_NEW_TITLE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateNewTitle);
	BIND_OR_CONNECT_ID(ID_AGILITY_NEW_TITLE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnNewTitle);
	BIND_OR_CONNECT_ID(ID_AGILITY_NEW_TRIAL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateNewTrial);
	BIND_OR_CONNECT_ID(ID_AGILITY_NEW_TRIAL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnNewTrial);
	BIND_OR_CONNECT_ID(ID_AGILITY_EDIT_TRIAL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateEditTrial);
	BIND_OR_CONNECT_ID(ID_AGILITY_EDIT_TRIAL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnEditTrial);
	BIND_OR_CONNECT_ID(ID_AGILITY_DELETE_TRIAL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateDelete);
	BIND_OR_CONNECT_ID(ID_AGILITY_DELETE_TRIAL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnDelete);
	BIND_OR_CONNECT_ID(ID_AGILITY_PRINT_TRIAL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdatePrintTrial);
	BIND_OR_CONNECT_ID(ID_AGILITY_PRINT_TRIAL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnPrintTrial);
	BIND_OR_CONNECT_ID(ID_AGILITY_NEW_RUN, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateNewRun);
	BIND_OR_CONNECT_ID(ID_AGILITY_NEW_RUN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnNewRun);
	BIND_OR_CONNECT_ID(ID_AGILITY_EDIT_RUN, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateEditRun);
	BIND_OR_CONNECT_ID(ID_AGILITY_EDIT_RUN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnEditRun);
	BIND_OR_CONNECT_ID(ID_AGILITY_DELETE_RUN, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateDelete);
	BIND_OR_CONNECT_ID(ID_AGILITY_DELETE_RUN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnDelete);
	BIND_OR_CONNECT_ID(ID_VIEW_CUSTOMIZE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateEnable);
	BIND_OR_CONNECT_ID(ID_VIEW_CUSTOMIZE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewCustomize);
	BIND_OR_CONNECT_ID(ID_VIEW_SORTRUNS, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateEnable);
	BIND_OR_CONNECT_ID(ID_VIEW_SORTRUNS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewSortRuns);
	BIND_OR_CONNECT_ID(ID_VIEW_RUNS_BY_TRIAL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateEnable);
	BIND_OR_CONNECT_ID(ID_VIEW_RUNS_BY_TRIAL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewRunsByTrial);
	BIND_OR_CONNECT_ID(ID_VIEW_TABLE_IN_YPS, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateEnable);
	BIND_OR_CONNECT_ID(ID_VIEW_TABLE_IN_YPS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewTableInYPS);
	BIND_OR_CONNECT_ID(ID_VIEW_RUNTIME_IN_OPS, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateEnable);
	BIND_OR_CONNECT_ID(ID_VIEW_RUNTIME_IN_OPS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewRuntimeInOPS);
	BIND_OR_CONNECT_ID(wxID_PREFERENCES, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateEnable);
	BIND_OR_CONNECT_ID(wxID_PREFERENCES, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnViewPreferences);
	BIND_OR_CONNECT_ID(ID_EXPAND, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateExpand);
	BIND_OR_CONNECT_ID(ID_EXPAND, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnExpand);
	BIND_OR_CONNECT_ID(ID_COLLAPSE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateCollapse);
	BIND_OR_CONNECT_ID(ID_COLLAPSE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnCollapse);
	BIND_OR_CONNECT_ID(ID_EXPAND_ALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateExpandAll);
	BIND_OR_CONNECT_ID(ID_EXPAND_ALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnExpandAll);
	BIND_OR_CONNECT_ID(ID_COLLAPSE_ALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler, CAgilityBookTreeView::OnUpdateCollapseAll);
	BIND_OR_CONNECT_ID(ID_COLLAPSE_ALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler, CAgilityBookTreeView::OnCollapseAll);

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
	if (m_Ctrl && activate)
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


void CAgilityBookTreeView::RefreshItem(wxDataViewItem const& item)
{
	m_Ctrl->GetStore()->ItemChanged(item);
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
		if (m_Ctrl->GetStore()->GetARBBase(inItem) == pBase)
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
		if (eTreeDog == m_Ctrl->GetStore()->Type(inItem))
		{
			if (m_Ctrl->GetStore()->GetDog(inItem) == pDog)
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
		switch (m_Ctrl->GetStore()->Type(inItem))
		{
		default:
			break;
		case eTreeDog:
			{
				wxDataViewItemArray kids;
				unsigned int n = m_Ctrl->GetStore()->GetChildren(inItem, kids);
				for (unsigned int i = 0; !item.IsOk() && i < n; ++i)
				{
					item = FindData(kids[i], pTrial);
				}
			}
			break;
		case eTreeTrial:
			if (m_Ctrl->GetStore()->GetTrial(inItem) == pTrial)
				item = inItem;
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
		switch (m_Ctrl->GetStore()->Type(inItem))
		{
		default:
			break;
		case eTreeDog:
		case eTreeTrial:
			{
				wxDataViewItemArray kids;
				unsigned int n = m_Ctrl->GetStore()->GetChildren(inItem, kids);
				for (unsigned int i = 0; !item.IsOk() && i < n; ++i)
				{
					item = FindData(kids[i], pRun);
				}
			}
			break;
		case eTreeRun:
			if (m_Ctrl->GetStore()->GetRun(inItem) == pRun)
				item = inItem;
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
	assert(parent.IsOk());
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
	assert(parent.IsOk());
	wxDataViewItem run;
	if (pRun && !pRun->IsFiltered() && m_Ctrl)
	{
		run = m_Ctrl->GetStore()->LoadData(pRun, parent);
	}
	return run;
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
				ARBDogRunPtr run = m_Ctrl->GetStore()->GetRun(runs[iRun]);
				if (run && run->GetQ().Qualified())
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


bool CAgilityBookTreeView::DoEdit(
		wxDataViewItem const& item,
		CTreeDataType type)
{
	bool bHandled = false;
	if (m_Ctrl)
	{
		ARBDogPtr pDog = m_Ctrl->GetStore()->GetDog(item);
		ARBDogTrialPtr pTrial = m_Ctrl->GetStore()->GetTrial(item);
		ARBDogRunPtr pRun = m_Ctrl->GetStore()->GetRun(item);
		switch (type)
		{
		default:
			break;
				
		case eTreeDog:
			if (pDog)
			{
				bHandled = true;
				CDlgDog dlg(GetDocument(), pDog, wxGetApp().GetTopWindow(), 0);
				if (wxID_OK == dlg.ShowModal())
				{
					// No need to refresh item here - CDlgDog::OnOk will call the
					// document ResetVisibility which causes a tree load.
					// Note: If the dog dlg gets more intelligent (reset is only
					// needed if changing titles/etc), then we may need this. The
					// problem is that pDogData may be deleted.
					// Need a way to track that pDogData is gone...
				}
			}
			break;

		case eTreeTrial:
			if (pTrial)
			{
				bHandled = true;
				bool bOk = false;
				CDlgTrial dlg(GetDocument(), pTrial, wxGetApp().GetTopWindow());
				if (wxID_OK == dlg.ShowModal())
				{
					bOk = true;
					std::vector<CVenueFilter> venues;
					CFilterOptions::Options().GetFilterVenue(venues);
					pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
					GetDocument()->ResetVisibility(venues, pTrial);
					// We have to update the tree even when we add above as it may have
					// caused the trial to be reordered.
					if (bOk)
					{
						CUpdateHint hint(UPDATE_TREE_VIEW | UPDATE_POINTS_VIEW);
						GetDocument()->UpdateAllViews(NULL, &hint);
					}
				}
			}
			break;

		case eTreeRun:
			if (pRun)
			{
				bHandled = true;
				EditRun(pDog, pTrial, pRun, this);
			}
			break;
		}
	}
	return bHandled;
}


void CAgilityBookTreeView::DoSelectionChange(wxDataViewItem const& item)
{
	unsigned int iHint = 0;
	if (item.IsOk())
	{
		// Set the current dog
		ARBDogPtr pDog = m_Ctrl->GetStore()->GetDog(item);
		if (!m_pDog || !pDog || m_pDog != pDog)
			iHint |= UPDATE_POINTS_VIEW;
		m_pDog = pDog;
	}
	else
		m_pDog.reset();
	if (iHint)
	{
		CUpdateHint hint(iHint);
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

	if (item.IsOk())
	{
		bSkip = false;
		wxMenu* menu = CreatePopup(m_Ctrl->GetStore()->GetMenuID(item));
		if (menu)
		{
			m_Ctrl->PopupMenu(menu, evt.GetPosition());
			delete menu;
		}
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
	// eat the dbl-click when we do this - it's consistent with v1.x
	bool bSkip = true;
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		if (DoEdit(item, m_Ctrl->GetStore()->Type(item)))
			bSkip = false;
	}
	if (bSkip)
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
		if (m_Ctrl)
		{
			wxDataViewItem item = m_Ctrl->GetSelection();
			DoEdit(item, m_Ctrl->GetStore()->Type(item));
		}
		break;
	}
}


void CAgilityBookTreeView::OnPrint(wxCommandEvent& evt)
{
	wxGetApp().GetHtmlPrinter()->PrintText(StringUtil::stringWX(GetPrintDataAsHtmlTable()));
}


void CAgilityBookTreeView::OnPreview(wxCommandEvent& evt)
{
	wxGetApp().GetHtmlPrinter()->PreviewText(StringUtil::stringWX(GetPrintDataAsHtmlTable()));
}


void CAgilityBookTreeView::OnUpdateEnable(wxUpdateUIEvent& evt)
{
	evt.Enable(true);
}


void CAgilityBookTreeView::OnUpdateDisable(wxUpdateUIEvent& evt)
{
	evt.Enable(false);
}




void CAgilityBookTreeView::OnUpdateDuplicate(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		switch (m_Ctrl->GetStore()->Type(item))
		{
		default:
			break;
		case eTreeDog:
		case eTreeTrial:
		case eTreeRun:
			bEnable = true;
			break;
		}
	}
	evt.Enable(bEnable);
}


void CAgilityBookTreeView::OnDuplicate(wxCommandEvent& evt)
{
	bool bModified = false;
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		switch (m_Ctrl->GetStore()->Type(item))
		{
		default:
			break;
		case eTreeDog:
			if (m_Ctrl->GetStore()->GetDog(item))
			{
				ARBDogPtr pDog = m_Ctrl->GetStore()->GetDog(item)->Clone();
				GetDocument()->Book().GetDogs().AddDog(pDog);
				bModified = true;
			}
			break;
		case eTreeTrial:
			if (m_Ctrl->GetStore()->GetTrial(item))
			{
				ARBDogTrialPtr pTrial = m_Ctrl->GetStore()->GetTrial(item)->Clone();
				m_Ctrl->GetStore()->GetDog(item)->GetTrials().AddTrial(pTrial);
				bool bDescending = !CAgilityBookOptions::GetNewestDatesFirst();
				m_Ctrl->GetStore()->GetDog(item)->GetTrials().sort(bDescending);
				bModified = true;
			}
			break;
		case eTreeRun:
			if (m_Ctrl->GetStore()->GetRun(item))
			{
				ARBDogRunPtr pRun = m_Ctrl->GetStore()->GetRun(item)->Clone();
				m_Ctrl->GetStore()->GetTrial(item)->GetRuns().AddRun(pRun);
				m_Ctrl->GetStore()->GetTrial(item)->GetRuns().sort();
				bModified = true;
			}
			break;
		}
	}
	if (bModified)
	{
		GetDocument()->Modify(true);
		CUpdateHint hint(UPDATE_POINTS_VIEW);
		GetDocument()->UpdateAllViews(NULL, &hint);
	}
}


void CAgilityBookTreeView::OnUpdateCut(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		switch (m_Ctrl->GetStore()->Type(item))
		{
		default:
			break;
		case eTreeDog:
		case eTreeTrial:
		case eTreeRun:
			bEnable = true;
			break;
		}
	}
	evt.Enable(bEnable);
}


void CAgilityBookTreeView::OnCut(wxCommandEvent& evt)
{
	OnCopy(evt);
	m_bSuppressPrompt = true;
	OnDelete(evt);
	m_bSuppressPrompt = false;
}


void CAgilityBookTreeView::OnUpdateCopy(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		switch (m_Ctrl->GetStore()->Type(item))
		{
		default:
			break;
		case eTreeDog:
		case eTreeTrial:
		case eTreeRun:
			bEnable = true;
			break;
		}
	}
	evt.Enable(bEnable);
}


void CAgilityBookTreeView::OnCopy(wxCommandEvent& evt)
{
	if (m_Ctrl)
	{
		CClipboardDataWriter clpData;
		if (clpData.isOkay())
		{
			wxBusyCursor wait;
			ElementNodePtr tree;
			eClipFormat format = eFormatNone;
			wxDataViewItem item = m_Ctrl->GetSelection();
			switch (m_Ctrl->GetStore()->Type(item))
			{
			default:
				break;
			case eTreeDog:
				tree = ElementNode::New(CLIPDATA);
				m_Ctrl->GetStore()->GetDog(item)->Save(tree, GetDocument()->Book().GetConfig());
				format = eFormatDog;
				break;
			case eTreeTrial:
				tree = ElementNode::New(CLIPDATA);
				m_Ctrl->GetStore()->GetTrial(item)->Save(tree, GetDocument()->Book().GetConfig());
				format = eFormatTrial;
				break;
			case eTreeRun:
				tree = ElementNode::New(CLIPDATA);
				m_Ctrl->GetStore()->GetRun(item)->Save(tree, NULL, GetDocument()->Book().GetConfig()); // copy/paste: title points don't matter
				format = eFormatRun;
				break;
			}
			if (tree)
			{
				clpData.AddData(format, tree);
				clpData.AddData(GetPrintLine(item));
				clpData.CommitData();
			}
		}
	}
}


void CAgilityBookTreeView::OnUpdatePaste(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		if (CClipboardDataReader::IsFormatAvailable(eFormatDog))
			bEnable = true;
		else if (m_Ctrl->GetStore()->GetDog(item)
		&& CClipboardDataReader::IsFormatAvailable(eFormatTrial))
			bEnable = true;
		else if (m_Ctrl->GetStore()->GetTrial(item)
		&& CClipboardDataReader::IsFormatAvailable(eFormatRun))
			bEnable = true;
	}
	evt.Enable(bEnable);
}


void CAgilityBookTreeView::OnPaste(wxCommandEvent& evt)
{
	if (!m_Ctrl)
		return;

	bool bModified = false;
	wxBusyCursor wait;
	ElementNodePtr tree(ElementNode::New());

	wxDataViewItem item = m_Ctrl->GetSelection();
	ARBDogTrialPtr pTrial = m_Ctrl->GetStore()->GetTrial(item);
	ARBDogPtr pDog = m_Ctrl->GetStore()->GetDog(item);

	CClipboardDataReader clpData;
	if (clpData.GetData(eFormatDog, tree))
	{
		if (CLIPDATA == tree->GetName())
		{
			pDog = ARBDog::New();
			if (pDog)
			{
				CErrorCallback err;
				if (pDog->Load(GetDocument()->Book().GetConfig(), tree->GetNthElementNode(0), ARBAgilityRecordBook::GetCurrentDocVersion(), err))
				{
					bModified = true;
					pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
					std::vector<CVenueFilter> venues;
					CFilterOptions::Options().GetFilterVenue(venues);
					if (!GetDocument()->Book().GetDogs().AddDog(pDog))
					{
						bModified = false;
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
						CUpdateHint hint(UPDATE_POINTS_VIEW);
						GetDocument()->UpdateAllViews(NULL, &hint);
					}
				}
				else if (0 < err.m_ErrMsg.str().length())
					wxMessageBox(StringUtil::stringWX(err.m_ErrMsg.str()), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
			}
		}
	}
	else if (pTrial
	&& clpData.GetData(eFormatRun, tree))
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
					if (pRun->Load(GetDocument()->Book().GetConfig(), pTrial->GetClubs(), element, ARBAgilityRecordBook::GetCurrentDocVersion(), err))
						runs.push_back(pRun);
				}
			}
			if (0 < runs.size())
			{
				size_t nFailed = 0;
				bModified = true;
				std::vector<CVenueFilter> venues;
				CFilterOptions::Options().GetFilterVenue(venues);
				for (std::vector<ARBDogRunPtr>::iterator iter = runs.begin(); iter != runs.end(); ++iter)
				{
					ARBDogRunPtr pRun = *iter;
					if (!pTrial->GetRuns().AddRun(pRun))
					{
						++nFailed;
						wxMessageBox(_("IDS_CREATERUN_FAILED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
					}
					else
						GetDocument()->ResetVisibility(venues, pTrial, pRun);
				}
				if (runs.size() == nFailed)
					bModified = false;
				else
				{
					wxDataViewItem itemParent = FindData(pTrial);
					pTrial->GetRuns().sort();
					pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
					m_Ctrl->Freeze();
					wxDataViewItem itemRun;
					for (std::vector<ARBDogRunPtr>::iterator iter = runs.begin(); iter != runs.end(); ++iter)
					{
						ARBDogRunPtr pRun = *iter;
						itemRun = InsertRun(pRun, itemParent);
					}
					m_Ctrl->Thaw();
					m_Ctrl->Refresh();
					bool bOk = true;
					if (!itemRun.IsOk())
					{
						bOk = false;
						if (CFilterOptions::Options().IsFilterEnabled())
							wxMessageBox(_("IDS_CREATERUN_FILTERED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
					}
					else
					{
						m_Ctrl->Select(itemRun);
					}
					if (bOk)
					{
						pTrial->SetMultiQs(GetDocument()->Book().GetConfig());
						CUpdateHint hint(UPDATE_POINTS_VIEW);
						GetDocument()->UpdateAllViews(NULL, &hint);
					}
				}
			}
			if (!bModified && 0 < err.m_ErrMsg.str().length())
				wxMessageBox(StringUtil::stringWX(err.m_ErrMsg.str()), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
		}
	}
	else if (pDog
	&& clpData.GetData(eFormatTrial, tree))
	{
		if (CLIPDATA == tree->GetName())
		{
			ARBDogTrialPtr pNewTrial(ARBDogTrial::New());
			if (pNewTrial)
			{
				CErrorCallback err;
				if (pNewTrial->Load(GetDocument()->Book().GetConfig(), tree->GetNthElementNode(0), ARBAgilityRecordBook::GetCurrentDocVersion(), err))
				{
					bModified = true;
					std::vector<CVenueFilter> venues;
					CFilterOptions::Options().GetFilterVenue(venues);
					if (!pDog->GetTrials().AddTrial(pNewTrial))
					{
						bModified = false;
						wxMessageBox(_("IDS_CREATETRIAL_FAILED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
					}
					else
					{
						pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
						GetDocument()->ResetVisibility(venues, pNewTrial);
						m_Ctrl->Freeze();
						wxDataViewItem itemTrial = InsertTrial(pNewTrial, FindData(pDog));
						m_Ctrl->Thaw();
						m_Ctrl->Refresh();
						bool bOk = true;
						if (!itemTrial.IsOk())
						{
							bOk = false;
							wxMessageBox(_("IDS_CREATETRIAL_FILTERED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
						}
						else
						{
							m_Ctrl->Select(itemTrial);
						}
						if (bOk)
						{
							pNewTrial->SetMultiQs(GetDocument()->Book().GetConfig());
							CUpdateHint hint(UPDATE_POINTS_VIEW);
							GetDocument()->UpdateAllViews(NULL, &hint);
						}
					}
				}
				else if (0 < err.m_ErrMsg.str().length())
					wxMessageBox(StringUtil::stringWX(err.m_ErrMsg.str()), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
			}
		}
	}
	if (bModified)
	{
		GetDocument()->Modify(true);
	}
}


void CAgilityBookTreeView::OnUpdateDelete(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		switch (m_Ctrl->GetStore()->Type(item))
		{
		default:
			break;
		case eTreeDog:
		case eTreeTrial:
		case eTreeRun:
			bEnable = true;
			break;
		}
	}
	evt.Enable(bEnable);
}


void CAgilityBookTreeView::OnDelete(wxCommandEvent& evt)
{
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		switch (m_Ctrl->GetStore()->Type(item))
		{
		default:
			break;
		case eTreeDog:
			if (m_bSuppressPrompt
			|| wxYES == wxMessageBox(_("IDS_DELETE_DOG_DATA"), wxMessageBoxCaptionStr, wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_QUESTION))
			{
				ARBDogPtr pDog = m_Ctrl->GetStore()->GetDog(item);
				if (GetDocument()->Book().GetDogs().DeleteDog(pDog))
				{
					m_Ctrl->GetStore()->Delete(item);
					GetDocument()->Modify(true);
					CUpdateHint hint(UPDATE_POINTS_VIEW);
					GetDocument()->UpdateAllViews(NULL, &hint);
				}
			}
			break;

		case eTreeTrial:
			if (m_bSuppressPrompt
			|| wxYES == wxMessageBox(_("IDS_DELETE_TRIAL_DATA"), wxMessageBoxCaptionStr, wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_QUESTION))
			{
				ARBDogPtr pDog = m_Ctrl->GetStore()->GetDog(item);
				ARBDogTrialPtr pTrial = m_Ctrl->GetStore()->GetTrial(item);
				if (pDog->GetTrials().DeleteTrial(pTrial))
				{
					m_Ctrl->GetStore()->Delete(item);
					GetDocument()->Modify(true);
					CUpdateHint hint(UPDATE_POINTS_VIEW);
					GetDocument()->UpdateAllViews(NULL, &hint);
				}
			}
			break;

		case eTreeRun:
			if (m_bSuppressPrompt
			|| wxYES == wxMessageBox(_("IDS_DELETE_EVENT_DATA"), wxMessageBoxCaptionStr, wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_QUESTION))
			{
				ARBDogTrialPtr pTrial = m_Ctrl->GetStore()->GetTrial(item);
				ARBDogRunPtr pRun = m_Ctrl->GetStore()->GetRun(item);
				if (pTrial->GetRuns().DeleteRun(pRun))
				{
					m_Ctrl->GetStore()->Delete(item);
					pTrial->SetMultiQs(GetDocument()->Book().GetConfig());
					GetDocument()->Modify(true);
					CUpdateHint hint(UPDATE_POINTS_VIEW);
					GetDocument()->UpdateAllViews(NULL, &hint);
				}
			}
			break;
		}
	}
}


void CAgilityBookTreeView::OnUpdateReorder(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		switch (m_Ctrl->GetStore()->Type(item))
		{
		default:
			break;
		case eTreeDog:
			bEnable = (1 < GetDocument()->Book().GetDogs().size());
			break;
		case eTreeTrial:
		case eTreeRun:
			{
				ARBDogTrialPtr pTrial = m_Ctrl->GetStore()->GetTrial(item);
				bEnable = (pTrial && 1 < pTrial->GetRuns().size());
			}
			break;
		}
	}
	evt.Enable(bEnable);
}

void CAgilityBookTreeView::OnReorder(wxCommandEvent& evt)
{
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		switch (m_Ctrl->GetStore()->Type(item))
		{
		default:
			break;
		case eTreeDog:
			if (1 < GetDocument()->Book().GetDogs().size())
			{
				CDlgReorder dlg(GetDocument(), &(GetDocument()->Book().GetDogs()));
				dlg.ShowModal();
			}
			break;
		case eTreeTrial:
		case eTreeRun:
			{
				ARBDogTrialPtr pTrial = m_Ctrl->GetStore()->GetTrial(item);
				if (pTrial && 1 < pTrial->GetRuns().size())
				{
					CDlgReorder dlg(GetDocument(), pTrial);
					dlg.ShowModal();
				}
			}
			break;
		}
	}
}


void CAgilityBookTreeView::OnFind(wxCommandEvent& evt)
{
	CDlgFind dlg(m_Callback, m_Ctrl);
	dlg.ShowModal();
}


void CAgilityBookTreeView::OnFindNext(wxCommandEvent& evt)
{
	m_Callback.SearchDown(true);
	if (m_Callback.Text().empty())
	{
		CDlgFind dlg(m_Callback, m_Ctrl);
		dlg.ShowModal();
	}
	else
		m_Callback.Search(NULL);
}


void CAgilityBookTreeView::OnFindPrevious(wxCommandEvent& evt)
{
	m_Callback.SearchDown(false);
	if (m_Callback.Text().empty())
	{
		CDlgFind dlg(m_Callback, m_Ctrl);
		dlg.ShowModal();
	}
	else
		m_Callback.Search(NULL);
}


void CAgilityBookTreeView::OnUpdateNewDog(wxUpdateUIEvent& evt)
{
	evt.Enable(true);
}


void CAgilityBookTreeView::OnNewDog(wxCommandEvent& evt)
{
	ARBDogPtr dog(ARBDog::New());
	CDlgDog dlg(GetDocument(), dog);
	if (wxID_OK == dlg.ShowModal())
	{
		if (GetDocument()->Book().GetDogs().AddDog(dog))
			InsertDog(dog, true);
#pragma PRAGMA_TODO("Test")
		// For some reason, the first dog isn't showing up.
		//if (1 == GetDocument()->Book().GetDogs().size())
		//	LoadData(false);
	}
}


void CAgilityBookTreeView::OnUpdateEditDog(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		if (m_Ctrl->GetStore()->GetDog(item))
			bEnable = true;
	}
	evt.Enable(bEnable);
}


void CAgilityBookTreeView::OnEditDog(wxCommandEvent& evt)
{
	if (m_Ctrl)
	{
		DoEdit(m_Ctrl->GetSelection(), eTreeDog);
	}
}


void CAgilityBookTreeView::OnUpdateNewTitle(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		if (m_Ctrl->GetStore()->GetDog(item))
			bEnable = true;
	}
	evt.Enable(bEnable);
}


void CAgilityBookTreeView::OnNewTitle(wxCommandEvent& evt)
{
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		ARBDogPtr pDog = m_Ctrl->GetStore()->GetDog(item);
		if (pDog)
		{
			GetDocument()->AddTitle(pDog);
		}
	}
}


void CAgilityBookTreeView::OnUpdateNewTrial(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		if (m_Ctrl->GetStore()->GetDog(item))
			bEnable = true;
	}
	evt.Enable(bEnable);
}


void CAgilityBookTreeView::OnNewTrial(wxCommandEvent& evt)
{
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		ARBDogPtr pDog = m_Ctrl->GetStore()->GetDog(item);
		if (pDog)
		{
			ARBDogTrialPtr pTrial = ARBDogTrialPtr(ARBDogTrial::New());
			bool bOk = false;
			CDlgTrial dlg(GetDocument(), pTrial, wxGetApp().GetTopWindow());
			if (wxID_OK == dlg.ShowModal())
			{
				bOk = true;
				std::vector<CVenueFilter> venues;
				CFilterOptions::Options().GetFilterVenue(venues);
				if (pDog->GetTrials().AddTrial(pTrial))
				{
					pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
					GetDocument()->ResetVisibility(venues, pTrial);
					// Even though we will reset the tree, go ahead and add/select
					// the item into the tree here. That will make sure when the
					// tree is reloaded, that the new item is selected.
					wxDataViewItem itemTrial = InsertTrial(pTrial, FindData(pDog));
					if (itemTrial.IsOk())
					{
						Select(itemTrial);
					}
					else
					{
						wxMessageBox(_("IDS_CREATETRIAL_FILTERED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
					}
				}
				else
				{
					bOk = false;
					wxMessageBox(_("IDS_CREATETRIAL_FAILED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
				}
				// We have to update the tree even when we add above as it may have
				// caused the trial to be reordered.
				if (bOk)
				{
					CUpdateHint hint(UPDATE_TREE_VIEW | UPDATE_POINTS_VIEW);
					GetDocument()->UpdateAllViews(NULL, &hint);
				}
			}
		}
	}
}


void CAgilityBookTreeView::OnUpdateEditTrial(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		if (m_Ctrl->GetStore()->GetTrial(item))
			bEnable = true;
	}
	evt.Enable(bEnable);
}


void CAgilityBookTreeView::OnEditTrial(wxCommandEvent& evt)
{
	if (m_Ctrl)
	{
		DoEdit(m_Ctrl->GetSelection(), eTreeTrial);
	}
}


void CAgilityBookTreeView::OnUpdatePrintTrial(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		ARBDogTrialPtr pTrial = m_Ctrl->GetStore()->GetTrial(item);
		if (pTrial && 0 < pTrial->GetRuns().size())
			bEnable = true;
	}
	evt.Enable(bEnable);
}


void CAgilityBookTreeView::OnPrintTrial(wxCommandEvent& evt)
{
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		ARBDogTrialPtr pTrial = m_Ctrl->GetStore()->GetTrial(item);
		if (pTrial && 0 < pTrial->GetRuns().size())
		{
			std::vector<RunInfo> runs;
			for (ARBDogRunList::iterator iRun = pTrial->GetRuns().begin(); iRun != pTrial->GetRuns().end(); ++iRun)
			{
				runs.push_back(RunInfo(pTrial, *iRun));
			}
			PrintRuns(&(GetDocument()->Book().GetConfig()), m_Ctrl->GetStore()->GetDog(item), runs);
		}
	}
}


void CAgilityBookTreeView::OnUpdateNewRun(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		ARBDogTrialPtr pTrial = m_Ctrl->GetStore()->GetTrial(item);
		if (pTrial && pTrial->GetClubs().GetPrimaryClub())
			bEnable = true;
	}
	evt.Enable(bEnable);
}


void CAgilityBookTreeView::OnNewRun(wxCommandEvent& evt)
{
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		ARBDogTrialPtr pTrial = m_Ctrl->GetStore()->GetTrial(item);
		if (pTrial && pTrial->GetClubs().GetPrimaryClub())
		{
			EditRun(m_Ctrl->GetStore()->GetDog(item), pTrial, ARBDogRunPtr(), this);
		}
	}
}


void CAgilityBookTreeView::OnUpdateEditRun(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	if (m_Ctrl)
	{
		wxDataViewItem item = m_Ctrl->GetSelection();
		if (m_Ctrl->GetStore()->GetRun(item))
			bEnable = true;
	}
	evt.Enable(bEnable);
}


void CAgilityBookTreeView::OnEditRun(wxCommandEvent& evt)
{
	if (m_Ctrl)
	{
		DoEdit(m_Ctrl->GetSelection(), eTreeRun);
	}
}


void CAgilityBookTreeView::OnViewCustomize(wxCommandEvent& evt)
{
	CDlgAssignColumns dlg(CAgilityBookOptions::eView, m_Ctrl, GetDocument(), IO_TYPE_VIEW_TREE_DOG);
	dlg.ShowModal();
}


void CAgilityBookTreeView::OnViewSortRuns(wxCommandEvent& evt)
{
	CAgilityBookOptions::SetNewestDatesFirst(!CAgilityBookOptions::GetNewestDatesFirst());
	GetDocument()->SortDates();
	LoadData(false);
}


void CAgilityBookTreeView::OnViewRunsByTrial(wxCommandEvent& evt)
{
	CAgilityBookOptions::SetViewRunsByTrial(!CAgilityBookOptions::GetViewRunsByTrial());
}


void CAgilityBookTreeView::OnViewTableInYPS(wxCommandEvent& evt)
{
	CAgilityBookOptions::SetTableInYPS(!CAgilityBookOptions::GetTableInYPS());
}


void CAgilityBookTreeView::OnViewRuntimeInOPS(wxCommandEvent& evt)
{
	CAgilityBookOptions::SetRunTimeInOPS(!CAgilityBookOptions::GetRunTimeInOPS());
}


void CAgilityBookTreeView::OnViewPreferences(wxCommandEvent& evt)
{
	CDlgOptions options(GetDocument(), wxGetApp().GetTopWindow(), CDlgOptions::GetFilterPage());
	options.ShowModal();
}


void CAgilityBookTreeView::OnUpdateExpand(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	wxDataViewItem item = m_Ctrl->GetSelection();
	if (item.IsOk() && m_Ctrl->GetStore()->IsContainer(item))
	{
		if (!m_Ctrl->IsExpanded(item))
			bEnable = true;
	}
	evt.Enable(bEnable);
}


void CAgilityBookTreeView::OnExpand(wxCommandEvent& evt)
{
	wxDataViewItem item = m_Ctrl->GetSelection();
	if (item.IsOk())
	{
		m_Ctrl->Expand(item);
		m_Ctrl->EnsureVisible(item);
	}
}


void CAgilityBookTreeView::OnUpdateCollapse(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	wxDataViewItem item = m_Ctrl->GetSelection();
	if (item.IsOk() && m_Ctrl->GetStore()->IsContainer(item))
	{
		if (m_Ctrl->IsExpanded(item))
			bEnable = true;
	}
	evt.Enable(bEnable);
}


void CAgilityBookTreeView::OnCollapse(wxCommandEvent& evt)
{
	wxDataViewItem item = m_Ctrl->GetSelection();
	if (item.IsOk())
	{
		m_Ctrl->Collapse(item);
		m_Ctrl->EnsureVisible(item);
	}
}


void CAgilityBookTreeView::OnUpdateExpandAll(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	wxDataViewItem item = m_Ctrl->GetSelection();
	if (item.IsOk() && m_Ctrl->GetStore()->IsContainer(item))
		bEnable = true;
	evt.Enable(bEnable);
}


void CAgilityBookTreeView::OnExpandAll(wxCommandEvent& evt)
{
	wxDataViewItem item = m_Ctrl->GetSelection();
	if (item.IsOk())
	{
		m_Ctrl->ExpandAllChildren(item);
		m_Ctrl->EnsureVisible(item);
	}
}


void CAgilityBookTreeView::OnUpdateCollapseAll(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	wxDataViewItem item = m_Ctrl->GetSelection();
	if (item.IsOk() && m_Ctrl->GetStore()->IsContainer(item))
		bEnable = true;
	evt.Enable(bEnable);
}


void CAgilityBookTreeView::OnCollapseAll(wxCommandEvent& evt)
{
	wxDataViewItem item = m_Ctrl->GetSelection();
	if (item.IsOk())
	{
		m_Ctrl->CollapseAllChildren(item);
		m_Ctrl->EnsureVisible(item);
	}
}
