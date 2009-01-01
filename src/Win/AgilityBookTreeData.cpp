/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CAgilityBookTreeData classes
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-10 DRC Only sort runs one way, the UI handles everything else.
 * @li 2004-10-21 DRC When an item is inserted in the tree, the text callback
 *                    can actually occur BEFORE we set the HTREEITEM.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-08-11 DRC Added verified-trial info to listing
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-06-06 DRC Added duplicate/cut/paste support for dogs.
 * @li 2004-05-09 DRC After adding a title, display dog property dlg.
 * @li 2004-04-15 DRC Added Duplicate menu item.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-12-30 DRC Implemented customized text in tree.
 * @li 2003-09-04 DRC When pasting, set document modified flag. Clean up some
 *                    warning messages when creating/pasting items and a filter
 *                    is set. (It was saying they weren't added - they were,
 *                    they just weren't visible.) Also, after adding/editing
 *                    trials/runs, make sure the items get sorted properly.
 * @li 2003-09-03 DRC When pasting trials/runs, select the new item.
 * @li 2003-08-30 DRC Copy a trial/run in the tree to the clipboard in dual
 *                    form. The internal form allows copying of the entry.
 *                    The text form copies what is printed.
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 * @li 2003-07-24 DRC Added reorder support on all items. Made dogs user-sorted.
 */

#include "stdafx.h"
#include <sstream>
#include "AgilityBook.h"
#include "AgilityBookTreeData.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTree.h"
#include "ARBDog.h"
#include "ARBDogRun.h"
#include "ARBDogTrial.h"
#include "ClipBoard.h"
#include "DlgAssignColumns.h"
#include "DlgDog.h"
#include "DlgOptions.h"
#include "DlgReorder.h"
#include "DlgRun.h"
#include "DlgTitle.h"
#include "DlgTrial.h"
#include "Element.h"
#include "FilterOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////

static bool EditDog(
		CAgilityBookTreeDataDog* pDogData,
		CAgilityBookTree* pTree,
		bool* bTreeSelectionSet,
		int nPage = 0)
{
	bool bAdd = false;
	ARBDogPtr pDog;
	if (pDogData)
		pDog = pDogData->GetDog();
	if (!pDog)
	{
		bAdd = true;
		pDog = ARBDogPtr(ARBDog::New());
	}
	bool bOk = false;
	CDlgDog dlg(pTree->GetDocument(), pDog, pTree, nPage);
	if (IDOK == dlg.DoModal())
	{
		bOk = true;
		if (bAdd)
		{
			if (!pTree->GetDocument()->Book().GetDogs().AddDog(pDog))
			{
				bOk = false;
			}
			else
			{
				HTREEITEM hItem = pTree->InsertDog(pDog);
				pTree->GetTreeCtrl().Select(hItem, TVGN_CARET);
				pTree->GetTreeCtrl().EnsureVisible(hItem);
				if (bTreeSelectionSet)
					*bTreeSelectionSet = true;
			}
		}
		else
			pTree->Invalidate();
		// No need to update the tree view here. We don't sort the dogs so
		// adding a new dog is all set and editing an existing one doesn't
		// change any ordering.
		if (bOk)
			pTree->GetDocument()->UpdateAllViews(NULL, UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW);
	}
	return bOk;
}


static bool EditTrial(
		CAgilityBookTreeDataDog* pDogData,
		CAgilityBookTreeDataTrial* pTrialData,
		CAgilityBookTree* pTree,
		bool* bTreeSelectionSet)
{
	assert(pDogData && pDogData->GetDog());
	bool bAdd = false;
	ARBDogTrialPtr pTrial;
	if (pTrialData)
		pTrial = pTrialData->GetTrial();
	if (!pTrial)
	{
		bAdd = true;
		pTrial = ARBDogTrialPtr(ARBDogTrial::New());
	}
	bool bOk = false;
	CDlgTrial dlg(pTree->GetDocument(), pTrial, pTree);
	if (IDOK == dlg.DoModal())
	{
		bOk = true;
		std::vector<CVenueFilter> venues;
		CFilterOptions::Options().GetFilterVenue(venues);
		if (bAdd)
		{
			if (!pDogData->GetDog()->GetTrials().AddTrial(pTrial))
			{
				bOk = false;
				AfxMessageBox(IDS_CREATETRIAL_FAILED, MB_ICONSTOP);
			}
			else
			{
				pDogData->GetDog()->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
				pTree->GetDocument()->ResetVisibility(venues, pTrial);
				// Even though we will reset the tree, go ahead and add/select
				// the item into the tree here. That will make sure when the
				// tree is reloaded, that the new item is selected.
				HTREEITEM hItem = pTree->InsertTrial(pTrial, pDogData->GetHTreeItem());
				if (NULL == hItem)
				{
					AfxMessageBox(IDS_CREATETRIAL_FILTERED, MB_ICONSTOP);
				}
				else
				{
					pTree->GetTreeCtrl().Select(hItem, TVGN_CARET);
					pTree->GetTreeCtrl().EnsureVisible(hItem);
					if (bTreeSelectionSet)
						*bTreeSelectionSet = true;
				}
			}
		}
		else
		{
			pDogData->GetDog()->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
			pTree->GetDocument()->ResetVisibility(venues, pTrial);
			pTree->Invalidate();
			if (dlg.RunsWereDeleted())
			{
				if (bTreeSelectionSet)
					*bTreeSelectionSet = true;
			}
		}
		// We have to update the tree even when we add above as it may have
		// caused the trial to be reordered.
		if (bOk)
			pTree->GetDocument()->UpdateAllViews(NULL, UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW | UPDATE_TREE_VIEW);
	}
	return bOk;
}


static bool EditRun(
		CAgilityBookTreeDataDog* pDogData,
		CAgilityBookTreeDataTrial* pTrialData,
		CAgilityBookTreeDataRun* pRunData,
		CAgilityBookTree* pTree,
		bool* bTreeSelectionSet)
{
	assert(pDogData && pDogData->GetDog());
	assert(pTrialData && pTrialData->GetTrial());
	bool bAdd = false;
	ARBDogRunPtr pRun;
	if (pRunData)
		pRun = pRunData->GetRun();
	if (!pRun)
	{
		ARBDogClubPtr pClub;
		if (!pTrialData->GetTrial()->GetClubs().GetPrimaryClub(&pClub))
		{
			AfxMessageBox(IDS_NEED_CLUB, MB_ICONEXCLAMATION);
			return false;
		}
		if (!pTree->GetDocument()->Book().GetConfig().GetVenues().FindVenue(pClub->GetVenue()))
		{
			CString msg;
			msg.FormatMessage(IDS_VENUE_CONFIG_MISSING, pClub->GetVenue().c_str());
			AfxMessageBox(msg, MB_ICONSTOP);
			return false;
		}
		bAdd = true;
		pRun = ARBDogRunPtr(ARBDogRun::New());
		pRun->SetDate(pTrialData->GetTrial()->GetRuns().GetEndDate());
	}
	bool bOk = false;
	CDlgRun dlg(pTree->GetDocument(), pTrialData->GetTrial(), pRun, pTree);
	if (IDOK == dlg.DoModal())
	{
		bOk = true;
		std::vector<CVenueFilter> venues;
		CFilterOptions::Options().GetFilterVenue(venues);
		if (bAdd)
		{
			if (!pTrialData->GetTrial()->GetRuns().AddRun(pRun))
			{
				bOk = false;
				AfxMessageBox(IDS_CREATERUN_FAILED, MB_ICONSTOP);
			}
			else
			{
				// When adding a new trial, we need to reset the multiQs.
				// The edit dialog does it in OnOK, but we don't add the run
				// until after the dialog is done.
				pTrialData->GetTrial()->SetMultiQs(pTree->GetDocument()->Book().GetConfig());
				pTrialData->GetTrial()->GetRuns().sort();
				pTrialData->GetDog()->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
				pTree->GetDocument()->ResetVisibility(venues, pTrialData->GetTrial(), pRun);
				// Even though we will reset the tree, go ahead and add/select
				// the item into the tree here. That will make sure when the
				// tree is reloaded, that the new item is selected.
				HTREEITEM hItem = pTree->InsertRun(pTrialData->GetTrial(), pRun, pTrialData->GetHTreeItem());
				if (NULL == hItem)
				{
					if (CFilterOptions::Options().IsFilterEnabled())
						AfxMessageBox(IDS_CREATERUN_FILTERED, MB_ICONSTOP);
				}
				else
				{
					pTree->GetTreeCtrl().Select(hItem, TVGN_CARET);
					pTree->GetTreeCtrl().EnsureVisible(hItem);
					if (bTreeSelectionSet)
						*bTreeSelectionSet = true;
				}
			}
		}
		else
		{
			pTrialData->GetDog()->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
			pTree->GetDocument()->ResetVisibility(venues, pTrialData->GetTrial(), pRun);
		}
		// We have to update the tree even when we add above as it may have
		// caused the trial to be reordered.
		if (bOk)
			pTree->GetDocument()->UpdateAllViews(NULL, UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW | UPDATE_TREE_VIEW);
	}
	return bOk;
}


static bool ReOrderDogs(
		ARBDogList& dogs,
		CAgilityBookTree* pTree)
{
	bool bOk = false;
	std::vector<ARBBasePtr> items;
	for (ARBDogList::iterator iter = dogs.begin(); iter != dogs.end(); ++iter)
	{
		ARBDogPtr pDog = *iter;
		items.push_back(pDog);
	}
	CDlgReorder dlg(items);
	if (IDOK == dlg.DoModal())
	{
		bOk = true;
		dogs.clear();
		for (std::vector<ARBBasePtr>::iterator iter2 = items.begin(); iter2 != items.end(); ++iter2)
		{
			ARBDogPtr pDog = tr1::dynamic_pointer_cast<ARBDog, ARBBase>(*iter2);
			dogs.AddDog(pDog);
		}
		CAgilityBookDoc* pDoc = pTree->GetDocument();
		pDoc->SetModifiedFlag(TRUE);
		pDoc->UpdateAllViews(NULL, UPDATE_TREE_VIEW | UPDATE_RUNS_VIEW);
	}
	return bOk;
}


static bool ReOrderTrial(
		ARBDogTrialPtr pTrial,
		CAgilityBookTree* pTree)
{
	bool bOk = false;
	if (pTrial)
	{
		ARBDogRunPtr pTmpRun;
		std::vector<ARBBasePtr> items;
		for (ARBDogRunList::iterator iter = pTrial->GetRuns().begin(); iter != pTrial->GetRuns().end(); ++iter)
		{
			ARBDogRunPtr pRun = *iter;
			if (!pTmpRun)
				pTmpRun = pRun;
			items.push_back(pRun);
		}
		CDlgReorder dlg(items);
		if (IDOK == dlg.DoModal())
		{
			bOk = true;
			pTrial->GetRuns().clear();
			for (std::vector<ARBBasePtr>::iterator iter2 = items.begin(); iter2 != items.end(); ++iter2)
			{
				ARBDogRunPtr pRun = tr1::dynamic_pointer_cast<ARBDogRun, ARBBase>(*iter2);
				pTrial->GetRuns().AddRun(pRun);
			}
			pTrial->GetRuns().sort();
			CAgilityBookDoc* pDoc = pTree->GetDocument();
			pDoc->SetModifiedFlag(TRUE);
			pDoc->UpdateAllViews(NULL, UPDATE_TREE_VIEW | UPDATE_RUNS_VIEW);
		}
	}
	return bOk;
}


static bool AddTitle(
		CAgilityBookTreeDataDog* pDogData,
		CAgilityBookTree* pTree)
{
	assert(pDogData && pDogData->GetDog());
	CDlgTitle dlg(pTree->GetDocument()->Book().GetConfig(), pDogData->GetDog()->GetTitles(), ARBDogTitlePtr(), pTree);
	if (IDOK == dlg.DoModal())
	{
		pTree->GetDocument()->UpdateAllViews(NULL, UPDATE_POINTS_VIEW);
		if (CAgilityBookOptions::AutoShowPropertiesOnNewTitle())
			EditDog(pDogData, pTree, NULL, 1);
		return true;
	}
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////

bool CAgilityBookTreeData::CanPaste() const
{
	bool bEnable = false;
	if (CClipboardDataReader::IsFormatAvailable(eFormatDog))
		bEnable = true;
	else if (GetTrial()
	&& CClipboardDataReader::IsFormatAvailable(eFormatRun))
		bEnable = true;
	else if (GetDog()
	&& CClipboardDataReader::IsFormatAvailable(eFormatTrial))
		bEnable = true;
	return bEnable;
}


bool CAgilityBookTreeData::DoPaste(bool* bTreeSelectionSet)
{
	CWaitCursor wait;
	bool bLoaded = false;
	ElementNodePtr tree(ElementNode::New());
	CClipboardDataReader clpData;
	ARBDogTrialPtr pTrial = GetTrial();
	ARBDogPtr pDog = GetDog();
	if (m_pTree->PasteDog(bLoaded))
	{
		// Done.
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
					if (pRun->Load(m_pTree->GetDocument()->Book().GetConfig(), pTrial->GetClubs(), element, ARBAgilityRecordBook::GetCurrentDocVersion(), err))
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
					if (!pTrial->GetRuns().AddRun(pRun))
					{
						++nFailed;
						AfxMessageBox(IDS_CREATERUN_FAILED, MB_ICONSTOP);
					}
					else
						m_pTree->GetDocument()->ResetVisibility(venues, pTrial, pRun);
				}
				if (runs.size() == nFailed)
					bLoaded = false;
				else
				{
					pTrial->GetRuns().sort();
					pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
					m_pTree->SetRedraw(FALSE);
					HTREEITEM hItem = NULL;
					for (std::vector<ARBDogRunPtr>::iterator iter = runs.begin(); iter != runs.end(); ++iter)
					{
						ARBDogRunPtr pRun = *iter;
						hItem = m_pTree->InsertRun(pTrial, pRun, GetDataTrial()->GetHTreeItem());
					}
					m_pTree->SetRedraw(TRUE);
					m_pTree->Invalidate();
					bool bOk = true;
					if (NULL == hItem)
					{
						bOk = false;
						if (CFilterOptions::Options().IsFilterEnabled())
							AfxMessageBox(IDS_CREATERUN_FILTERED, MB_ICONSTOP);
					}
					else
					{
						m_pTree->GetTreeCtrl().Select(hItem, TVGN_CARET);
						m_pTree->GetTreeCtrl().EnsureVisible(hItem);
						if (bTreeSelectionSet)
							*bTreeSelectionSet = true;
					}
					if (bOk)
					{
						m_pTree->GetDocument()->UpdateAllViews(NULL, UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW | UPDATE_TREE_VIEW);
					}
				}
			}
			if (!bLoaded && 0 < err.m_ErrMsg.length())
				AfxMessageBox(err.m_ErrMsg.c_str(), MB_ICONWARNING);
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
				if (pNewTrial->Load(m_pTree->GetDocument()->Book().GetConfig(), tree->GetNthElementNode(0), ARBAgilityRecordBook::GetCurrentDocVersion(), err))
				{
					bLoaded = true;
					std::vector<CVenueFilter> venues;
					CFilterOptions::Options().GetFilterVenue(venues);
					if (!pDog->GetTrials().AddTrial(pNewTrial))
					{
						bLoaded = false;
						AfxMessageBox(IDS_CREATETRIAL_FAILED, MB_ICONSTOP);
					}
					else
					{
						pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
						m_pTree->GetDocument()->ResetVisibility(venues, pNewTrial);
						m_pTree->SetRedraw(FALSE);
						HTREEITEM hItem = m_pTree->InsertTrial(pNewTrial, GetDataDog()->GetHTreeItem());
						m_pTree->SetRedraw(TRUE);
						m_pTree->Invalidate();
						bool bOk = true;
						if (NULL == hItem)
						{
							bOk = false;
							AfxMessageBox(IDS_CREATETRIAL_FILTERED, MB_ICONSTOP);
						}
						else
						{
							m_pTree->GetTreeCtrl().Select(hItem, TVGN_CARET);
							m_pTree->GetTreeCtrl().EnsureVisible(hItem);
							if (bTreeSelectionSet)
								*bTreeSelectionSet = true;
						}
						if (bOk)
						{
							m_pTree->GetDocument()->UpdateAllViews(NULL, UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW | UPDATE_TREE_VIEW);
						}
					}
				}
				else if (0 < err.m_ErrMsg.length())
					AfxMessageBox(err.m_ErrMsg.c_str(), MB_ICONWARNING);
			}
		}
	}
	return bLoaded;
}


std::vector<int> const& CAgilityBookTreeData::GetDogColumns() const
{
	return m_pTree->m_Columns[0];
}


std::vector<int> const& CAgilityBookTreeData::GetTrialColumns() const
{
	return m_pTree->m_Columns[1];
}


std::vector<int> const& CAgilityBookTreeData::GetRunColumns() const
{
	return m_pTree->m_Columns[2];
}

////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeDataDog::CAgilityBookTreeDataDog(
		CAgilityBookTree* pTree,
		ARBDogPtr pDog)
	: CAgilityBookTreeData(pTree)
	, m_pDog(pDog)
{
}


CAgilityBookTreeDataDog::~CAgilityBookTreeDataDog()
{
}


bool CAgilityBookTreeDataDog::OnUpdateCmd(UINT id) const
{
	bool bEnable = false;
	switch (id)
	{
	default:
		break;
	case ID_EDIT_DUPLICATE:
	case ID_EDIT_CUT:
	case ID_EDIT_COPY:
		bEnable = true;
		break;
	case ID_EDIT_PASTE:
		bEnable = CanPaste();
		break;
	case ID_AGILITY_EDIT_DOG:
	case ID_AGILITY_NEW_DOG:
	case ID_AGILITY_NEW_TITLE:
	case ID_AGILITY_NEW_TRIAL:
	case ID_AGILITY_DELETE_DOG:
		bEnable = true;
		break;
	case ID_REORDER:
		if (m_pTree && 1 < m_pTree->GetDocument()->Book().GetDogs().size())
			bEnable = true;
		break;
	case ID_EXPAND:
		if (m_hItem && m_pTree && m_pTree->GetTreeCtrl().ItemHasChildren(m_hItem))
		{
			if (!(TVIS_EXPANDED & m_pTree->GetTreeCtrl().GetItemState(m_hItem, TVIS_EXPANDED)))
				bEnable = true;
		}
		break;
	case ID_COLLAPSE:
		if (m_hItem && m_pTree && m_pTree->GetTreeCtrl().ItemHasChildren(m_hItem))
		{
			if ((TVIS_EXPANDED & m_pTree->GetTreeCtrl().GetItemState(m_hItem, TVIS_EXPANDED)))
				bEnable = true;
		}
		break;
	case ID_EXPAND_ALL:
	case ID_COLLAPSE_ALL:
		if (m_hItem && m_pTree && m_pTree->GetTreeCtrl().ItemHasChildren(m_hItem))
			bEnable = true;
		break;
	}
	return bEnable;
}


bool CAgilityBookTreeDataDog::OnCmd(
		UINT id,
		bool* bTreeSelectionSet)
{
	static bool bPrompt = true;
	bool bModified = false;
	switch (id)
	{
	default:
		break;
	case ID_VIEW_OPTIONS:
		{
			CDlgOptions options(m_pTree->GetDocument(), AfxGetMainWnd(), CDlgOptions::GetFilterPage());
			options.DoModal();
		}
		break;
	case ID_EDIT_DUPLICATE:
		if (GetDog())
		{
			ARBDogPtr pDog = GetDog()->Clone();
			m_pTree->GetDocument()->Book().GetDogs().AddDog(pDog);
			bModified = true;
			m_pTree->GetDocument()->UpdateAllViews(NULL, UPDATE_TREE_VIEW | UPDATE_RUNS_VIEW | UPDATE_POINTS_VIEW);
		}
		break;
	case ID_EDIT_CUT:
		OnCmd(ID_EDIT_COPY, bTreeSelectionSet);
		bPrompt = false;
		OnCmd(ID_AGILITY_DELETE_DOG, bTreeSelectionSet);
		bPrompt = true;
		bModified = true;
		break;
	case ID_EDIT_COPY:
		{
			CClipboardDataWriter clpData;
			if (clpData.isOkay())
			{
				CWaitCursor wait;
				ElementNodePtr tree(ElementNode::New(CLIPDATA));
				GetDog()->Save(tree);
				clpData.SetData(eFormatDog, tree);
				clpData.SetData(m_pTree->GetPrintLine(GetHTreeItem()));
			}
		}
		break;
	case ID_EDIT_PASTE:
		if (DoPaste(bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_EDIT_DOG:
		if (EditDog(this, m_pTree, bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_NEW_DOG:
		if (EditDog(NULL, m_pTree, bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_NEW_TRIAL:
		if (EditTrial(this, NULL, m_pTree, bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_NEW_TITLE:
		if (AddTitle(this, m_pTree))
			bModified = true;
		break;
	case ID_AGILITY_DELETE_DOG:
		if (!bPrompt
		|| IDYES == AfxMessageBox(IDS_DELETE_DOG_DATA, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2))
		{
			if (m_hItem && m_pTree->GetDocument()->Book().GetDogs().DeleteDog(m_pDog))
			{
				CAgilityBookDoc* pDoc = m_pTree->GetDocument();
				// DeleteItem will cause this object to be deleted.
				m_pTree->GetTreeCtrl().DeleteItem(m_hItem);
				bModified = true;
				pDoc->UpdateAllViews(NULL, UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW);
			}
		}
		break;

	case ID_REORDER:
		if (m_pTree)
			ReOrderDogs(m_pTree->GetDocument()->Book().GetDogs(), m_pTree);
		break;

	case ID_EXPAND:
		if (m_hItem)
		{
			m_pTree->GetTreeCtrl().Expand(m_hItem, TVE_EXPAND);
			m_pTree->GetTreeCtrl().EnsureVisible(m_hItem);
		}
		break;

	case ID_EXPAND_ALL:
		if (m_hItem)
		{
			ExpandAll(m_pTree->GetTreeCtrl(), m_hItem, TVE_EXPAND);
			m_pTree->GetTreeCtrl().EnsureVisible(m_hItem);
		}
		break;

	case ID_COLLAPSE:
		if (m_hItem)
		{
			m_pTree->GetTreeCtrl().Expand(m_hItem, TVE_COLLAPSE);
			m_pTree->GetTreeCtrl().EnsureVisible(m_hItem);
		}
		break;

	case ID_COLLAPSE_ALL:
		if (m_hItem)
		{
			ExpandAll(m_pTree->GetTreeCtrl(), m_hItem, TVE_COLLAPSE);
			m_pTree->GetTreeCtrl().EnsureVisible(m_hItem);
		}
		break;
	}
	return bModified;
}


tstring CAgilityBookTreeDataDog::OnNeedText() const
{
	tstring str;
	// We can actually be called for text BEFORE we call SetHTreeItem.
	if (m_hItem)
	{
		for (size_t idx = 0; idx < GetDogColumns().size(); ++idx)
		{
			if (0 < idx)
				str += ' ';
			switch (GetDogColumns()[idx])
			{
			case IO_TREE_DOG_REGNAME:
				str += m_pDog->GetRegisteredName();
				break;
			case IO_TREE_DOG_CALLNAME:
				str += m_pDog->GetCallName();
				break;
			case IO_TREE_DOG_BREED:
				str += m_pDog->GetBreed();
				break;
			case IO_TREE_DOG_DOB:
				str += m_pDog->GetDOB().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::eRunTree));
				break;
			case IO_TREE_DOG_AGE:
				{
					CTime dob = m_pDog->GetDOB().GetDate();
					CTime current = CTime::GetCurrentTime();
					if (m_pDog->GetDeceased().IsValid())
						current = m_pDog->GetDeceased().GetDate();
					CTimeSpan age = current - dob;
					CString yrs;
					yrs.FormatMessage(IDS_YEARS, ARBDouble::str(age.GetDays()/365.0, 1).c_str());
					str += (LPCTSTR)yrs;
				}
				break;
			}
		}
	}
	return str;
}


int CAgilityBookTreeDataDog::OnNeedIcon() const
{
	return m_pTree->GetImageList().Dog();
}


void CAgilityBookTreeDataDog::Properties()
{
	CAgilityBookDoc* pDoc = m_pTree->GetDocument();
	// This may delete 'this'.
	if (OnCmd(ID_AGILITY_EDIT_DOG, NULL))
		pDoc->SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeDataTrial::CAgilityBookTreeDataTrial(
		CAgilityBookTree* pTree,
		ARBDogTrialPtr pTrial)
	: CAgilityBookTreeData(pTree)
	, m_pTrial(pTrial)
	, m_idxIcon(-1)
{
	m_idxIcon = m_pTree->GetImageList().Trial();
	if (m_pTrial)
	{
		ARBDogClubPtr pClub;
		if (m_pTrial->GetClubs().GetPrimaryClub(&pClub))
		{
			ARBConfigVenuePtr pVenue;
			if (pTree->GetDocument()->Book().GetConfig().GetVenues().FindVenue(pClub->GetVenue(), &pVenue))
			{
				short idx = pVenue->GetIcon();
				if (0 <= idx && idx < m_pTree->GetImageList().GetImageCount())
					m_idxIcon = idx;
			}
		}
	}
}


CAgilityBookTreeDataTrial::~CAgilityBookTreeDataTrial()
{
}


ARBDogPtr CAgilityBookTreeDataTrial::GetDog() const
{
	CAgilityBookTreeDataDog const* pDog = GetDataDog();
	assert(pDog);
	return pDog->GetDog();
}


ARBDogPtr CAgilityBookTreeDataTrial::GetDog()
{
	CAgilityBookTreeDataDog* pDog = GetDataDog();
	assert(pDog);
	return pDog->GetDog();
}


CAgilityBookTreeDataDog const* CAgilityBookTreeDataTrial::GetDataDog() const
{
	if (!m_hItem)
		return NULL;
	HTREEITEM hParent = m_pTree->GetTreeCtrl().GetParentItem(m_hItem);
	CAgilityBookTreeData const* pData = reinterpret_cast<CAgilityBookTreeData const*>(m_pTree->GetTreeCtrl().GetItemData(hParent));
	CAgilityBookTreeDataDog const* pDog = dynamic_cast<CAgilityBookTreeDataDog const*>(pData);
	assert(pDog);
	return pDog;
}


CAgilityBookTreeDataDog* CAgilityBookTreeDataTrial::GetDataDog()
{
	if (!m_hItem)
		return NULL;
	HTREEITEM hParent = m_pTree->GetTreeCtrl().GetParentItem(m_hItem);
	CAgilityBookTreeData* pData = reinterpret_cast<CAgilityBookTreeData*>(m_pTree->GetTreeCtrl().GetItemData(hParent));
	CAgilityBookTreeDataDog* pDog = dynamic_cast<CAgilityBookTreeDataDog*>(pData);
	assert(pDog);
	return pDog;
}


bool CAgilityBookTreeDataTrial::OnUpdateCmd(UINT id) const
{
	bool bEnable = false;
	switch (id)
	{
	default:
		break;
	case ID_EDIT_DUPLICATE:
	case ID_EDIT_CUT:
	case ID_EDIT_COPY:
		bEnable = true;
		break;
	case ID_EDIT_PASTE:
		bEnable = CanPaste();
		break;
	case ID_AGILITY_EDIT_TRIAL:
	case ID_AGILITY_NEW_DOG:
	case ID_AGILITY_NEW_TRIAL:
		bEnable = true;
		break;
	case ID_AGILITY_NEW_RUN:
		if (GetTrial() && GetTrial()->GetClubs().GetPrimaryClub())
			bEnable = true;
		break;
	case ID_AGILITY_NEW_TITLE:
	case ID_AGILITY_DELETE_TRIAL:
		bEnable = true;
		break;
	case ID_REORDER:
		if (GetTrial() && 1 < GetTrial()->GetRuns().size())
			bEnable = true;
		break;
	case ID_AGILITY_PRINT_TRIAL:
		if (GetTrial() && 0 < GetTrial()->GetRuns().size())
			bEnable = true;
		break;
	}
	return bEnable;
}


bool CAgilityBookTreeDataTrial::OnCmd(
		UINT id,
		bool* bTreeSelectionSet)
{
	static bool bPrompt = true;
	bool bModified = false;
	switch (id)
	{
	default:
		break;
	case ID_VIEW_OPTIONS:
		{
			CDlgOptions options(m_pTree->GetDocument(), AfxGetMainWnd(), CDlgOptions::GetFilterPage());
			options.DoModal();
		}
		break;
	case ID_EDIT_DUPLICATE:
		if (GetTrial())
		{
			ARBDogTrialPtr pTrial = GetTrial()->Clone();
			GetDog()->GetTrials().AddTrial(pTrial);
			bool bDescending = !CAgilityBookOptions::GetNewestDatesFirst();
			GetDog()->GetTrials().sort(bDescending);
			bModified = true;
			m_pTree->GetDocument()->UpdateAllViews(NULL, UPDATE_TREE_VIEW | UPDATE_RUNS_VIEW | UPDATE_POINTS_VIEW);
		}
		break;
	case ID_EDIT_CUT:
		OnCmd(ID_EDIT_COPY, bTreeSelectionSet);
		bPrompt = false;
		OnCmd(ID_AGILITY_DELETE_TRIAL, bTreeSelectionSet);
		bPrompt = true;
		bModified = true;
		break;
	case ID_EDIT_COPY:
		{
			CClipboardDataWriter clpData;
			if (clpData.isOkay())
			{
				CWaitCursor wait;
				ElementNodePtr tree(ElementNode::New(CLIPDATA));
				GetTrial()->Save(tree);
				clpData.SetData(eFormatTrial, tree);
				clpData.SetData(m_pTree->GetPrintLine(GetHTreeItem()));
			}
		}
		break;
	case ID_EDIT_PASTE:
		if (DoPaste(bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_EDIT_TRIAL:
		if (EditTrial(GetDataDog(), this, m_pTree, bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_NEW_DOG:
		if (EditDog(NULL, m_pTree, bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_NEW_TRIAL:
		if (EditTrial(GetDataDog(), NULL, m_pTree, bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_NEW_RUN:
		if (GetTrial() && GetTrial()->GetClubs().GetPrimaryClub())
			if (EditRun(GetDataDog(), this, NULL, m_pTree, bTreeSelectionSet))
				bModified = true;
		break;
	case ID_AGILITY_NEW_TITLE:
		if (AddTitle(GetDataDog(), m_pTree))
			bModified = true;
		break;
	case ID_AGILITY_DELETE_TRIAL:
		if (!bPrompt
		|| IDYES == AfxMessageBox(IDS_DELETE_TRIAL_DATA, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2))
		{
			if (m_hItem && GetDog()->GetTrials().DeleteTrial(m_pTrial))
			{
				CAgilityBookDoc* pDoc = m_pTree->GetDocument();
				// DeleteItem will cause this object to be deleted.
				m_pTree->GetTreeCtrl().DeleteItem(m_hItem);
				bModified = true;
				pDoc->UpdateAllViews(NULL, UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW);
			}
		}
		break;
	case ID_REORDER:
		ReOrderTrial(GetTrial(), m_pTree);
		break;
	case ID_AGILITY_PRINT_TRIAL:
		if (GetTrial() && 0 < GetTrial()->GetRuns().size())
		{
			std::vector<RunInfo> runs;
			for (ARBDogRunList::iterator iRun = GetTrial()->GetRuns().begin(); iRun != GetTrial()->GetRuns().end(); ++iRun)
			{
				runs.push_back(RunInfo(GetTrial(), *iRun));
			}
			PrintRuns(&(m_pTree->GetDocument()->Book().GetConfig()), GetDog(), runs);
		}
		break;
	}
	return bModified;
}


tstring CAgilityBookTreeDataTrial::OnNeedText() const
{
	tstring str;
	// We can actually be called for text BEFORE we call SetHTreeItem.
	if (m_hItem)
	{
		bool bNeedSpace = false;
		for (size_t idx = 0; idx < GetTrialColumns().size(); ++idx)
		{
			switch (GetTrialColumns()[idx])
			{
			case IO_TREE_TRIAL_START:
				if (m_pTrial->GetRuns().GetStartDate().IsValid())
				{
					if (bNeedSpace)
					{
						if (IO_TREE_TRIAL_END == GetTrialColumns()[idx-1])
							str += '-';
						else
							str += ' ';
					}
					str += m_pTrial->GetRuns().GetStartDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::eRunTree));
					bNeedSpace = true;
				}
				break;
			case IO_TREE_TRIAL_END:
				if (m_pTrial->GetRuns().GetEndDate().IsValid())
				{
					if (bNeedSpace)
					{
						if (IO_TREE_TRIAL_START == GetTrialColumns()[idx-1])
							str += '-';
						else
							str += ' ';
					}
					str += m_pTrial->GetRuns().GetEndDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::eRunTree));
					bNeedSpace = true;
				}
				break;
			case IO_TREE_TRIAL_CLUB:
				{
					if (bNeedSpace && 0 < m_pTrial->GetClubs().size())
						str += ' ';
					int i = 0;
					for (ARBDogClubList::const_iterator iter = m_pTrial->GetClubs().begin();
						iter != m_pTrial->GetClubs().end();
						++iter, ++i)
					{
						if (0 < i)
							str += _T("/");
						str += (*iter)->GetName();
						bNeedSpace = true;
					}
				}
				break;
			case IO_TREE_TRIAL_VENUE:
				{
					if (bNeedSpace && 0 < m_pTrial->GetClubs().size())
						str += ' ';
					int i = 0;
					for (ARBDogClubList::const_iterator iter = m_pTrial->GetClubs().begin();
						iter != m_pTrial->GetClubs().end();
						++iter, ++i)
					{
						if (0 < i)
							str += _T("/");
						str += (*iter)->GetVenue();
						bNeedSpace = true;
					}
				}
				break;
			case IO_TREE_TRIAL_LOCATION:
				if (!m_pTrial->GetLocation().empty())
				{
					if (bNeedSpace)
						str += ' ';
					str += m_pTrial->GetLocation();
					bNeedSpace = true;
				}
				break;
			case IO_TREE_TRIAL_NOTES:
				if (!m_pTrial->GetNote().empty())
				{
					if (bNeedSpace)
						str += ' ';
					str += m_pTrial->GetNote();
					str = tstringUtil::Replace(str, _T("\n"), _T(" "));
					bNeedSpace = true;
				}
				break;
			}
		}
	}
	return str;
}


int CAgilityBookTreeDataTrial::OnNeedIcon() const
{
	return m_idxIcon;
}


void CAgilityBookTreeDataTrial::Properties()
{
	CAgilityBookDoc* pDoc = m_pTree->GetDocument();
	// This may delete 'this'.
	if (OnCmd(ID_AGILITY_EDIT_TRIAL, NULL))
		pDoc->SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeDataRun::CAgilityBookTreeDataRun(
		CAgilityBookTree* pTree,
		ARBDogRunPtr pRun)
	: CAgilityBookTreeData(pTree)
	, m_pRun(pRun)
{
}


CAgilityBookTreeDataRun::~CAgilityBookTreeDataRun()
{
}


ARBDogPtr CAgilityBookTreeDataRun::GetDog() const
{
	CAgilityBookTreeDataDog const* pDog = GetDataDog();
	assert(pDog);
	return pDog->GetDog();
}


ARBDogTrialPtr CAgilityBookTreeDataRun::GetTrial() const
{
	CAgilityBookTreeDataTrial const* pTrial = GetDataTrial();
	assert(pTrial);
	return pTrial->GetTrial();
}


ARBDogPtr CAgilityBookTreeDataRun::GetDog()
{
	CAgilityBookTreeDataDog* pDog = GetDataDog();
	assert(pDog);
	return pDog->GetDog();
}


ARBDogTrialPtr CAgilityBookTreeDataRun::GetTrial()
{
	CAgilityBookTreeDataTrial* pTrial = GetDataTrial();
	assert(pTrial);
	return pTrial->GetTrial();
}


CAgilityBookTreeDataDog const* CAgilityBookTreeDataRun::GetDataDog() const
{
	if (!m_hItem)
		return NULL;
	HTREEITEM hParent = m_pTree->GetTreeCtrl().GetParentItem(m_hItem);
	CAgilityBookTreeData const* pData = reinterpret_cast<CAgilityBookTreeData const*>(m_pTree->GetTreeCtrl().GetItemData(hParent));
	CAgilityBookTreeDataTrial const* pTrial = dynamic_cast<CAgilityBookTreeDataTrial const*>(pData);
	assert(pTrial);
	return pTrial->GetDataDog();
}


CAgilityBookTreeDataTrial const* CAgilityBookTreeDataRun::GetDataTrial() const
{
	if (!m_hItem)
		return NULL;
	HTREEITEM hParent = m_pTree->GetTreeCtrl().GetParentItem(m_hItem);
	CAgilityBookTreeData const* pData = reinterpret_cast<CAgilityBookTreeData const*>(m_pTree->GetTreeCtrl().GetItemData(hParent));
	CAgilityBookTreeDataTrial const* pTrial = dynamic_cast<CAgilityBookTreeDataTrial const*>(pData);
	assert(pTrial);
	return pTrial;
}


CAgilityBookTreeDataDog* CAgilityBookTreeDataRun::GetDataDog()
{
	if (!m_hItem)
		return NULL;
	HTREEITEM hParent = m_pTree->GetTreeCtrl().GetParentItem(m_hItem);
	CAgilityBookTreeData* pData = reinterpret_cast<CAgilityBookTreeData*>(m_pTree->GetTreeCtrl().GetItemData(hParent));
	CAgilityBookTreeDataTrial* pTrial = dynamic_cast<CAgilityBookTreeDataTrial*>(pData);
	assert(pTrial);
	return pTrial->GetDataDog();
}


CAgilityBookTreeDataTrial* CAgilityBookTreeDataRun::GetDataTrial()
{
	if (!m_hItem)
		return NULL;
	HTREEITEM hParent = m_pTree->GetTreeCtrl().GetParentItem(m_hItem);
	CAgilityBookTreeData* pData = reinterpret_cast<CAgilityBookTreeData*>(m_pTree->GetTreeCtrl().GetItemData(hParent));
	CAgilityBookTreeDataTrial* pTrial = dynamic_cast<CAgilityBookTreeDataTrial*>(pData);
	assert(pTrial);
	return pTrial;
}


bool CAgilityBookTreeDataRun::OnUpdateCmd(UINT id) const
{
	bool bEnable = false;
	switch (id)
	{
	default:
		break;
	case ID_EDIT_DUPLICATE:
	case ID_EDIT_CUT:
	case ID_EDIT_COPY:
		bEnable = true;
		break;
	case ID_EDIT_PASTE:
		bEnable = CanPaste();
		break;
	case ID_AGILITY_EDIT_RUN:
	case ID_AGILITY_NEW_DOG:
	case ID_AGILITY_NEW_TRIAL:
		bEnable = true;
		break;
	case ID_AGILITY_NEW_RUN:
		if (GetTrial() && GetTrial()->GetClubs().GetPrimaryClub())
			bEnable = true;
		break;
	case ID_AGILITY_NEW_TITLE:
	case ID_AGILITY_DELETE_RUN:
		bEnable = true;
		break;
	case ID_REORDER:
		if (GetTrial() && 1 < GetTrial()->GetRuns().size())
			bEnable = true;
		break;
	case ID_AGILITY_PRINT_RUNS:
		bEnable = true;
		break;
	}
	return bEnable;
}


bool CAgilityBookTreeDataRun::OnCmd(
		UINT id,
		bool* bTreeSelectionSet)
{
	static bool bPrompt = true;
	bool bModified = false;
	switch (id)
	{
	default:
		break;
	case ID_VIEW_OPTIONS:
		{
			CDlgOptions options(m_pTree->GetDocument(), AfxGetMainWnd(), CDlgOptions::GetFilterPage());
			options.DoModal();
		}
		break;
	case ID_EDIT_DUPLICATE:
		if (GetRun())
		{
			ARBDogRunPtr pRun = GetRun()->Clone();
			GetTrial()->GetRuns().AddRun(pRun);
			GetTrial()->GetRuns().sort();
			bModified = true;
			m_pTree->GetDocument()->UpdateAllViews(NULL, UPDATE_TREE_VIEW | UPDATE_RUNS_VIEW | UPDATE_POINTS_VIEW);
		}
		break;
	case ID_EDIT_CUT:
		OnCmd(ID_EDIT_COPY, bTreeSelectionSet);
		bPrompt = false;
		OnCmd(ID_AGILITY_DELETE_RUN, bTreeSelectionSet);
		bPrompt = true;
		bModified = true;
		break;
	case ID_EDIT_COPY:
		{
			CClipboardDataWriter clpData;
			if (clpData.isOkay())
			{
				CWaitCursor wait;
				ElementNodePtr tree(ElementNode::New(CLIPDATA));
				GetRun()->Save(tree);
				clpData.SetData(eFormatRun, tree);
				clpData.SetData(m_pTree->GetPrintLine(GetHTreeItem()));
			}
		}
		break;
	case ID_EDIT_PASTE:
		if (DoPaste(bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_EDIT_RUN:
		if (EditRun(GetDataDog(), GetDataTrial(), this, m_pTree, bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_NEW_DOG:
		if (EditDog(NULL, m_pTree, bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_NEW_TRIAL:
		if (EditTrial(GetDataDog(), NULL, m_pTree, bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_NEW_RUN:
		if (GetTrial() && GetTrial()->GetClubs().GetPrimaryClub())
			if (EditRun(GetDataDog(), GetDataTrial(), NULL, m_pTree, bTreeSelectionSet))
				bModified = true;
		break;
	case ID_AGILITY_NEW_TITLE:
		if (AddTitle(GetDataDog(), m_pTree))
			bModified = true;
		break;
	case ID_AGILITY_DELETE_RUN:
		if (!bPrompt
		|| IDYES == AfxMessageBox(IDS_DELETE_EVENT_DATA, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2))
		{
			if (m_hItem && GetTrial()->GetRuns().DeleteRun(m_pRun))
			{
				CAgilityBookDoc* pDoc = m_pTree->GetDocument();
				// DeleteItem will cause this object to be deleted.
				m_pTree->GetTreeCtrl().DeleteItem(m_hItem);
				bModified = true;
				pDoc->UpdateAllViews(NULL, UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW);
			}
		}
		break;
	case ID_REORDER:
		ReOrderTrial(GetTrial(), m_pTree);
		break;
	case ID_AGILITY_PRINT_RUNS:
		{
			std::vector<RunInfo> runs;
			runs.push_back(RunInfo(GetTrial(), m_pRun));
			PrintRuns(&(m_pTree->GetDocument()->Book().GetConfig()), GetDog(), runs);
		}
		break;
	}
	return bModified;
}


tstring CAgilityBookTreeDataRun::OnNeedText() const
{
	tstring str;
	// We can actually be called for text BEFORE we call SetHTreeItem.
	if (m_hItem)
	{
		for (size_t idx = 0; idx < GetRunColumns().size(); ++idx)
		{
			if (0 < idx)
				str += ' ';
			switch (GetRunColumns()[idx])
			{
			case IO_TREE_RUN_DATE:
				str += m_pRun->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::eRunTree));
				break;
			case IO_TREE_RUN_Q:
				{
					tstring q;
					if (m_pRun->GetQ().Qualified())
					{
						std::vector<ARBConfigMultiQPtr> multiQs;
						if (0 < m_pRun->GetMultiQs(multiQs))
						{
							for (std::vector<ARBConfigMultiQPtr>::iterator iMultiQ = multiQs.begin(); iMultiQ != multiQs.end(); ++iMultiQ)
							{
								if (!q.empty())
									q += _T('/');
								q += (*iMultiQ)->GetShortName();
							}
						}
						if (ARB_Q::eQ_SuperQ == m_pRun->GetQ())
						{
							CString tmp;
							tmp.LoadString(IDS_SQ);
							if (!q.empty())
								q += _T('/');
							q += (LPCTSTR)tmp;
						}
					}
					if (q.empty())
						q = m_pRun->GetQ().str();
					str += q;
				}
				break;
			case IO_TREE_RUN_EVENT:
				str += m_pRun->GetEvent();
				break;
			case IO_TREE_RUN_DIVISION:
				str += m_pRun->GetDivision();
				break;
			case IO_TREE_RUN_LEVEL:
				str += m_pRun->GetLevel();
				break;
			case IO_TREE_RUN_HEIGHT:
				str += m_pRun->GetHeight();
				break;
			}
		}
	}
	return str;
}


int CAgilityBookTreeDataRun::OnNeedIcon() const
{
	return m_pTree->GetImageList().Run();
}


void CAgilityBookTreeDataRun::Properties()
{
	CAgilityBookDoc* pDoc = m_pTree->GetDocument();
	// This may delete 'this'.
	if (OnCmd(ID_AGILITY_EDIT_RUN, NULL))
		pDoc->SetModifiedFlag();
}
