/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
#include "DlgDog.h"
#include "DlgReorder.h"
#include "DlgRun.h"
#include "DlgTitle.h"
#include "DlgTrial.h"
#include "Element.h"

#define CLIPDATA	"ClipData"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////

static bool EditDog(
	CAgilityBookTreeDataDog* pDogData,
	CAgilityBookTree* pTree,
	bool* bTreeSelectionSet)
{
	bool bAdd = false;
	ARBDog* pDog = NULL;
	if (pDogData)
		pDog = pDogData->GetDog();
	if (pDog)
		pDog->AddRef();
	else
	{
		bAdd = true;
		pDog = new ARBDog;
	}
	bool bOk = false;
	CDlgDog dlg(pTree->GetDocument()->GetConfig(), pDog, pTree);
	if (IDOK == dlg.DoModal())
	{
		bOk = true;
		if (bAdd)
		{
			ARBDog* pNewDog = pTree->GetDocument()->GetDogs().AddDog(pDog);
			if (!pNewDog)
			{
				bOk = false;
			}
			else
			{
				HTREEITEM hItem = pTree->InsertDog(pNewDog);
				pTree->GetTreeCtrl().Select(hItem, TVGN_CARET);
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
			pTree->GetDocument()->UpdateAllViews(NULL, UPDATE_POINTS_VIEW|UPDATE_RUNS_VIEW);
	}
	pDog->Release();
	return bOk;
}

static bool EditTrial(
	CAgilityBookTreeDataDog* pDogData,
	CAgilityBookTreeDataTrial* pTrialData,
	CAgilityBookTree* pTree,
	bool* bTreeSelectionSet)
{
	ASSERT(pDogData && pDogData->GetDog());
	bool bAdd = false;
	ARBDogTrial* pTrial = NULL;
	if (pTrialData)
		pTrial = pTrialData->GetTrial();
	if (pTrial)
		pTrial->AddRef();
	else
	{
		bAdd = true;
		pTrial = new ARBDogTrial;
	}
	bool bOk = false;
	CDlgTrial dlg(pTree->GetDocument(), pTrial, pTree);
	if (IDOK == dlg.DoModal())
	{
		bOk = true;
		std::vector<CVenueFilter> venues;
		CAgilityBookOptions::GetFilterVenue(venues);
		if (bAdd)
		{
			ARBDogTrial* pNewTrial = pDogData->GetDog()->GetTrials().AddTrial(pTrial);
			if (!pNewTrial)
			{
				bOk = false;
				AfxMessageBox(IDS_CREATETRIAL_FAILED, MB_ICONSTOP);
			}
			else
			{
				pDogData->GetDog()->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
				pTree->GetDocument()->ResetVisibility(venues, pNewTrial);
				// Even though we will reset the tree, go ahead and add/select
				// the item into the tree here. That will make sure when the
				// tree is reloaded, that the new item is selected.
				HTREEITEM hItem = pTree->InsertTrial(pNewTrial, pDogData->GetHTreeItem());
				if (NULL == hItem)
				{
					AfxMessageBox(IDS_CREATETRIAL_FILTERED, MB_ICONSTOP);
				}
				else
				{
					pTree->GetTreeCtrl().Select(hItem, TVGN_CARET);
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
	pTrial->Release();
	return bOk;
}

static bool EditRun(
	CAgilityBookTreeDataDog* pDogData,
	CAgilityBookTreeDataTrial* pTrialData,
	CAgilityBookTreeDataRun* pRunData,
	CAgilityBookTree* pTree,
	bool* bTreeSelectionSet)
{
	ASSERT(pDogData && pDogData->GetDog());
	ASSERT(pTrialData && pTrialData->GetTrial());
	bool bAdd = false;
	ARBDogRun* pRun = NULL;
	if (pRunData)
		pRun = pRunData->GetRun();
	if (pRun)
		pRun->AddRef();
	else
	{
		const ARBDogClub* pClub = pTrialData->GetTrial()->GetClubs().GetPrimaryClub();
		if (!pClub)
		{
			AfxMessageBox(IDS_NEED_CLUB, MB_ICONEXCLAMATION);
			return false;
		}
		const ARBConfigVenue* pVenue = pTree->GetDocument()->GetConfig().GetVenues().FindVenue(pClub->GetVenue());
		if (!pVenue)
		{
			CString msg;
			msg.FormatMessage(IDS_VENUE_CONFIG_MISSING, pClub->GetVenue().c_str());
			AfxMessageBox(msg, MB_ICONSTOP);
			return false;
		}
		bAdd = true;
		pRun = new ARBDogRun;
		pRun->SetDate(pTrialData->GetTrial()->GetRuns().GetEndDate());
	}
	bool bOk = false;
	CDlgRun dlg(pTree->GetDocument(), pTrialData->GetTrial(), pRun, pTree);
	if (IDOK == dlg.DoModal())
	{
		bOk = true;
		std::vector<CVenueFilter> venues;
		CAgilityBookOptions::GetFilterVenue(venues);
		if (bAdd)
		{
			ARBDogRun* pNewRun = pTrialData->GetTrial()->GetRuns().AddRun(pRun);
			if (!pNewRun)
			{
				bOk = false;
				AfxMessageBox(IDS_CREATERUN_FAILED, MB_ICONSTOP);
			}
			else
			{
				pTrialData->GetTrial()->GetRuns().sort(!CAgilityBookOptions::GetNewestDatesFirst());
				pTrialData->GetDog()->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
				pTree->GetDocument()->ResetVisibility(venues, pTrialData->GetTrial(), pNewRun);
				// Even though we will reset the tree, go ahead and add/select
				// the item into the tree here. That will make sure when the
				// tree is reloaded, that the new item is selected.
				HTREEITEM hItem = pTree->InsertRun(pTrialData->GetTrial(), pNewRun, pTrialData->GetHTreeItem());
				if (NULL == hItem)
				{
					if (CAgilityBookOptions::IsFilterEnabled())
						AfxMessageBox(IDS_CREATERUN_FILTERED, MB_ICONSTOP);
				}
				else
				{
					pTree->GetTreeCtrl().Select(hItem, TVGN_CARET);
					if (bTreeSelectionSet)
						*bTreeSelectionSet = true;
				}
			}
		}
		else
		{
			pTrialData->GetTrial()->GetRuns().sort(!CAgilityBookOptions::GetNewestDatesFirst());
			pTrialData->GetDog()->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
			pTree->GetDocument()->ResetVisibility(venues, pTrialData->GetTrial(), pRun);
		}
		// We have to update the tree even when we add above as it may have
		// caused the trial to be reordered.
		if (bOk)
			pTree->GetDocument()->UpdateAllViews(NULL, UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW | UPDATE_TREE_VIEW);
	}
	pRun->Release();
	return bOk;
}

static bool ReOrderDogs(
	ARBDogList& dogs,
	CAgilityBookTree* pTree)
{
	bool bOk = false;
	std::vector<ARBBase*> items;
	for (ARBDogList::iterator iter = dogs.begin(); iter != dogs.end(); ++iter)
	{
		ARBDog* pDog = *iter;
		pDog->AddRef();
		items.push_back(pDog);
	}
	CDlgReorder dlg(items);
	if (IDOK == dlg.DoModal())
	{
		bOk = true;
		dogs.clear();
		for (std::vector<ARBBase*>::iterator iter2 = items.begin(); iter2 != items.end(); ++iter2)
		{
			ARBDog* pDog = dynamic_cast<ARBDog*>(*iter2);
			dogs.AddDog(pDog);
		}
		CAgilityBookDoc* pDoc = pTree->GetDocument();
		pDoc->SetModifiedFlag(TRUE);
		pDoc->UpdateAllViews(NULL, UPDATE_TREE_VIEW|UPDATE_RUNS_VIEW);
	}
	for (std::vector<ARBBase*>::iterator iter2 = items.begin(); iter2 != items.end(); ++iter2)
	{
		(*iter2)->Release();
	}
	return bOk;
}

static bool ReOrderTrial(
	ARBDogTrial* pTrial,
	CAgilityBookTree* pTree)
{
	bool bOk = false;
	if (pTrial)
	{
		std::vector<ARBBase*> items;
		for (ARBDogRunList::iterator iter = pTrial->GetRuns().begin(); iter != pTrial->GetRuns().end(); ++iter)
		{
			ARBDogRun* pRun = *iter;
			pRun->AddRef();
			items.push_back(pRun);
		}
		CDlgReorder dlg(items);
		if (IDOK == dlg.DoModal())
		{
			bOk = true;
			pTrial->GetRuns().clear();
			for (std::vector<ARBBase*>::iterator iter2 = items.begin(); iter2 != items.end(); ++iter2)
			{
				ARBDogRun* pRun = dynamic_cast<ARBDogRun*>(*iter2);
				pTrial->GetRuns().AddRun(pRun);
			}
			pTrial->GetRuns().sort(!CAgilityBookOptions::GetNewestDatesFirst());
			CAgilityBookDoc* pDoc = pTree->GetDocument();
			pDoc->SetModifiedFlag(TRUE);
			pDoc->UpdateAllViews(NULL, UPDATE_TREE_VIEW|UPDATE_RUNS_VIEW);
		}
		for (std::vector<ARBBase*>::iterator iter2 = items.begin(); iter2 != items.end(); ++iter2)
		{
			(*iter2)->Release();
		}
	}
	return bOk;
}

static bool AddTitle(
	CAgilityBookTreeDataDog* pDogData,
	CAgilityBookTree* pTree)
{
	ASSERT(pDogData && pDogData->GetDog());
	CDlgTitle dlg(pTree->GetDocument()->GetConfig(), pDogData->GetDog()->GetTitles(), NULL, pTree);
	if (IDOK == dlg.DoModal())
	{
		pTree->GetDocument()->UpdateAllViews(NULL, UPDATE_POINTS_VIEW);
		return true;
	}
	else
		return false;
}

static bool CopyDataToClipboard(UINT clpFmt, const CElement& tree, const CString& txtForm)
{
	if (!AfxGetMainWnd()->OpenClipboard())
		return false;
	if (!EmptyClipboard())
	{
		CloseClipboard();
		return false;
	}
	std::ostringstream out;
	tree.SaveXML(out);
	std::string data = out.str();

	// alloc mem block & copy text in
	HGLOBAL temp = GlobalAlloc(GHND, data.length()+1);
	if (NULL != temp)
	{
		LPTSTR str = (LPTSTR)GlobalLock(temp);
		lstrcpy(str, data.c_str());
		GlobalUnlock((void*)temp);
		// send data to clipbard
		SetClipboardData(clpFmt, temp);

		if (!txtForm.IsEmpty())
		{
			temp = GlobalAlloc(GHND, txtForm.GetLength()+1);
			if (NULL != temp)
			{
				LPTSTR str = (LPTSTR)GlobalLock(temp);
				lstrcpy(str, (LPCTSTR)txtForm);
				GlobalUnlock((void*)temp);
				// send data to clipbard
				SetClipboardData(CF_TEXT, temp);
			}
		}
	}

	CloseClipboard();
	return true;
}

static bool GetDataFromClipboard(UINT clpFmt, CElement& tree)
{
	if (IsClipboardFormatAvailable(clpFmt))
	{
		if (!AfxGetMainWnd()->OpenClipboard())
			return false;
		tree.clear();
		HANDLE hData = GetClipboardData(clpFmt);
		LPTSTR str = (LPTSTR)GlobalLock(hData);
		CString data(str);
		GlobalUnlock(hData);
		CloseClipboard();
		return tree.LoadXMLBuffer((LPCSTR)data, data.GetLength());
	}
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////

bool CAgilityBookTreeData::CanPaste() const
{
	bool bEnable = false;
	if (GetTrial()
	&& IsClipboardFormatAvailable(CAgilityBookOptions::GetClipboardFormat(CAgilityBookOptions::eFormatRun)))
		bEnable = true;
	else if (GetDog()
	&& IsClipboardFormatAvailable(CAgilityBookOptions::GetClipboardFormat(CAgilityBookOptions::eFormatTrial)))
		bEnable = true;
	return bEnable;
}

bool CAgilityBookTreeData::DoPaste(bool* bTreeSelectionSet)
{
	bool bLoaded = false;
	CElement tree;
	ARBDogTrial* pTrial = GetTrial();
	ARBDog* pDog = GetDog();
	if (pTrial
	&& GetDataFromClipboard(CAgilityBookOptions::GetClipboardFormat(CAgilityBookOptions::eFormatRun), tree))
	{
		if (CLIPDATA == tree.GetName())
		{
			ARBDogRun* pRun = new ARBDogRun;
			if (pRun && pRun->Load(m_pTree->GetDocument()->GetConfig(), pTrial->GetClubs(), tree.GetElement(0), ARBAgilityRecordBook::GetCurrentDocVersion()))
			{
				bLoaded = true;
				std::vector<CVenueFilter> venues;
				CAgilityBookOptions::GetFilterVenue(venues);
				ARBDogRun* pNewRun = pTrial->GetRuns().AddRun(pRun);
				if (!pNewRun)
				{
					bLoaded = false;
					AfxMessageBox(IDS_CREATERUN_FAILED, MB_ICONSTOP);
				}
				else
				{
					pTrial->GetRuns().sort(!CAgilityBookOptions::GetNewestDatesFirst());
					pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
					m_pTree->GetDocument()->ResetVisibility(venues, pTrial, pNewRun);
					HTREEITEM hItem = m_pTree->InsertRun(pTrial, pNewRun, GetDataTrial()->GetHTreeItem());
					bool bOk = true;
					if (NULL == hItem)
					{
						bOk = false;
						if (CAgilityBookOptions::IsFilterEnabled())
							AfxMessageBox(IDS_CREATERUN_FILTERED, MB_ICONSTOP);
					}
					else
					{
						m_pTree->GetTreeCtrl().Select(hItem, TVGN_CARET);
						if (bTreeSelectionSet)
							*bTreeSelectionSet = true;
					}
					if (bOk)
					{
						m_pTree->GetDocument()->UpdateAllViews(NULL, UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW | UPDATE_TREE_VIEW);
					}
				}
			}
			pRun->Release();
		}
	}
	else if (pDog
	&& GetDataFromClipboard(CAgilityBookOptions::GetClipboardFormat(CAgilityBookOptions::eFormatTrial), tree))
	{
		if (CLIPDATA == tree.GetName())
		{
			ARBDogTrial* pTrial = new ARBDogTrial;
			if (pTrial && pTrial->Load(m_pTree->GetDocument()->GetConfig(), tree.GetElement(0), ARBAgilityRecordBook::GetCurrentDocVersion()))
			{
				bLoaded = true;
				std::vector<CVenueFilter> venues;
				CAgilityBookOptions::GetFilterVenue(venues);
				ARBDogTrial* pNewTrial = pDog->GetTrials().AddTrial(pTrial);
				if (!pNewTrial)
				{
					bLoaded = false;
					AfxMessageBox(IDS_CREATETRIAL_FAILED, MB_ICONSTOP);
				}
				else
				{
					pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
					m_pTree->GetDocument()->ResetVisibility(venues, pNewTrial);
					HTREEITEM hItem = m_pTree->InsertTrial(pNewTrial, GetDataDog()->GetHTreeItem());
					bool bOk = true;
					if (NULL == hItem)
					{
						bOk = false;
						AfxMessageBox(IDS_CREATETRIAL_FILTERED, MB_ICONSTOP);
					}
					else
					{
						m_pTree->GetTreeCtrl().Select(hItem, TVGN_CARET);
						if (bTreeSelectionSet)
							*bTreeSelectionSet = true;
					}
					if (bOk)
					{
						m_pTree->GetDocument()->UpdateAllViews(NULL, UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW | UPDATE_TREE_VIEW);
					}
				}
			}
			pTrial->Release();
		}
	}
	return bLoaded;
}

////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeDataDog::CAgilityBookTreeDataDog(CAgilityBookTree* pTree, ARBDog* pDog)
	: CAgilityBookTreeData(pTree)
	, m_pDog(pDog)
{
	m_pDog->AddRef();
}

CAgilityBookTreeDataDog::~CAgilityBookTreeDataDog()
{
	m_pDog->Release();
}

bool CAgilityBookTreeDataDog::OnUpdateCmd(UINT id) const
{
	bool bEnable = false;
	switch (id)
	{
	default:
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
		if (m_pTree && 1 < m_pTree->GetDocument()->GetDogs().size())
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

bool CAgilityBookTreeDataDog::OnCmd(UINT id, bool* bTreeSelectionSet)
{
	bool bModified = false;
	switch (id)
	{
	default:
		break;
	case ID_EDIT_PASTE:
		if (DoPaste(bTreeSelectionSet)) // Currently this doesn't do anything. CanPaste said no.
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
		if (IDYES == AfxMessageBox(IDS_DELETE_DOG_DATA, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2))
		{
			if (m_pTree->GetDocument()->GetDogs().DeleteDog(m_pDog))
			{
				CAgilityBookDoc* pDoc = m_pTree->GetDocument();
				// DeleteItem will cause this object to be deleted.
				m_pTree->GetTreeCtrl().DeleteItem(m_hItem);
				bModified = true;
				pDoc->UpdateAllViews(NULL, UPDATE_POINTS_VIEW|UPDATE_RUNS_VIEW);
			}
		}
		break;

	case ID_REORDER:
		if (m_pTree)
			ReOrderDogs(m_pTree->GetDocument()->GetDogs(), m_pTree);
		break;

	case ID_EXPAND:
		m_pTree->GetTreeCtrl().Expand(m_hItem, TVE_EXPAND);
		m_pTree->GetTreeCtrl().EnsureVisible(m_hItem);
		break;

	case ID_EXPAND_ALL:
		ExpandAll(m_pTree->GetTreeCtrl(), m_hItem, TVE_EXPAND);
		m_pTree->GetTreeCtrl().EnsureVisible(m_hItem);
		break;

	case ID_COLLAPSE:
		m_pTree->GetTreeCtrl().Expand(m_hItem, TVE_COLLAPSE);
		m_pTree->GetTreeCtrl().EnsureVisible(m_hItem);
		break;

	case ID_COLLAPSE_ALL:
		ExpandAll(m_pTree->GetTreeCtrl(), m_hItem, TVE_COLLAPSE);
		m_pTree->GetTreeCtrl().EnsureVisible(m_hItem);
		break;
	}
	return bModified;
}

CString CAgilityBookTreeDataDog::OnNeedText() const
{
	return m_pDog->GetCallName().c_str();
}

void CAgilityBookTreeDataDog::Properties()
{
	CAgilityBookDoc* pDoc = m_pTree->GetDocument();
	// This may delete 'this'.
	if (OnCmd(ID_AGILITY_EDIT_DOG, NULL))
		pDoc->SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeDataTrial::CAgilityBookTreeDataTrial(CAgilityBookTree* pTree, ARBDogTrial* pTrial)
	: CAgilityBookTreeData(pTree)
	, m_pTrial(pTrial)
{
	m_pTrial->AddRef();
}

CAgilityBookTreeDataTrial::~CAgilityBookTreeDataTrial()
{
	m_pTrial->Release();
}

const ARBDog* CAgilityBookTreeDataTrial::GetDog() const
{
	const CAgilityBookTreeDataDog* pDog = GetDataDog();
	ASSERT(pDog);
	return pDog->GetDog();
}

ARBDog* CAgilityBookTreeDataTrial::GetDog()
{
	CAgilityBookTreeDataDog* pDog = GetDataDog();
	ASSERT(pDog);
	return pDog->GetDog();
}

const CAgilityBookTreeDataDog* CAgilityBookTreeDataTrial::GetDataDog() const
{
	HTREEITEM hParent = m_pTree->GetTreeCtrl().GetParentItem(m_hItem);
	const CAgilityBookTreeData* pData = reinterpret_cast<const CAgilityBookTreeData*>(m_pTree->GetTreeCtrl().GetItemData(hParent));
	const CAgilityBookTreeDataDog* pDog = dynamic_cast<const CAgilityBookTreeDataDog*>(pData);
	ASSERT(pDog);
	return pDog;
}

CAgilityBookTreeDataDog* CAgilityBookTreeDataTrial::GetDataDog()
{
	HTREEITEM hParent = m_pTree->GetTreeCtrl().GetParentItem(m_hItem);
	CAgilityBookTreeData* pData = reinterpret_cast<CAgilityBookTreeData*>(m_pTree->GetTreeCtrl().GetItemData(hParent));
	CAgilityBookTreeDataDog* pDog = dynamic_cast<CAgilityBookTreeDataDog*>(pData);
	ASSERT(pDog);
	return pDog;
}

bool CAgilityBookTreeDataTrial::OnUpdateCmd(UINT id) const
{
	bool bEnable = false;
	switch (id)
	{
	default:
		break;
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
	}
	return bEnable;
}

bool CAgilityBookTreeDataTrial::OnCmd(UINT id, bool* bTreeSelectionSet)
{
	bool bModified = false;
	switch (id)
	{
	default:
		break;
	case ID_EDIT_COPY:
		{
			CElement tree;
			tree.SetName(CLIPDATA);
			GetTrial()->Save(tree);
			CopyDataToClipboard(CAgilityBookOptions::GetClipboardFormat(CAgilityBookOptions::eFormatTrial), tree, m_pTree->GetPrintLine(GetHTreeItem()));
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
		if (IDYES == AfxMessageBox(IDS_DELETE_TRIAL_DATA, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2))
		{
			if (GetDog()->GetTrials().DeleteTrial(m_pTrial))
			{
				CAgilityBookDoc* pDoc = m_pTree->GetDocument();
				// DeleteItem will cause this object to be deleted.
				m_pTree->GetTreeCtrl().DeleteItem(m_hItem);
				bModified = true;
				pDoc->UpdateAllViews(NULL, UPDATE_POINTS_VIEW|UPDATE_RUNS_VIEW);
			}
		}
		break;
	case ID_REORDER:
		ReOrderTrial(GetTrial(), m_pTree);
		break;
	}
	return bModified;
}

CString CAgilityBookTreeDataTrial::OnNeedText() const
{
	CString trial;
	ARBDate date1 = m_pTrial->GetRuns().GetStartDate();
	ARBDate date2 = m_pTrial->GetRuns().GetEndDate();
	if (date1.IsValid())
	{
		trial += date1.GetString(true, false).c_str();
		if (date1 != date2)
		{
			trial += "-";
			trial += date2.GetString(true, false).c_str();
		}
		trial += " ";
	}
	int i = 0;
	for (ARBDogClubList::const_iterator iter = m_pTrial->GetClubs().begin();
		iter != m_pTrial->GetClubs().end();
		++iter, ++i)
	{
		if (0 < i)
			trial += "/";
		trial += (*iter)->GetVenue().c_str();
	}
	trial += ": ";
	trial += m_pTrial->GetLocation().c_str();
	return trial;
}

void CAgilityBookTreeDataTrial::Properties()
{
	CAgilityBookDoc* pDoc = m_pTree->GetDocument();
	// This may delete 'this'.
	if (OnCmd(ID_AGILITY_EDIT_TRIAL, NULL))
		pDoc->SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeDataRun::CAgilityBookTreeDataRun(CAgilityBookTree* pTree, ARBDogRun* pRun)
	: CAgilityBookTreeData(pTree)
	, m_pRun(pRun)
{
	m_pRun->AddRef();
}

CAgilityBookTreeDataRun::~CAgilityBookTreeDataRun()
{
	m_pRun->Release();
}

const ARBDog* CAgilityBookTreeDataRun::GetDog() const
{
	const CAgilityBookTreeDataDog* pDog = GetDataDog();
	ASSERT(pDog);
	return pDog->GetDog();
}

const ARBDogTrial* CAgilityBookTreeDataRun::GetTrial() const
{
	const CAgilityBookTreeDataTrial* pTrial = GetDataTrial();
	ASSERT(pTrial);
	return pTrial->GetTrial();
}

ARBDog* CAgilityBookTreeDataRun::GetDog()
{
	CAgilityBookTreeDataDog* pDog = GetDataDog();
	ASSERT(pDog);
	return pDog->GetDog();
}

ARBDogTrial* CAgilityBookTreeDataRun::GetTrial()
{
	CAgilityBookTreeDataTrial* pTrial = GetDataTrial();
	ASSERT(pTrial);
	return pTrial->GetTrial();
}

const CAgilityBookTreeDataDog* CAgilityBookTreeDataRun::GetDataDog() const
{
	HTREEITEM hParent = m_pTree->GetTreeCtrl().GetParentItem(m_hItem);
	const CAgilityBookTreeData* pData = reinterpret_cast<const CAgilityBookTreeData*>(m_pTree->GetTreeCtrl().GetItemData(hParent));
	const CAgilityBookTreeDataTrial* pTrial = dynamic_cast<const CAgilityBookTreeDataTrial*>(pData);
	ASSERT(pTrial);
	return pTrial->GetDataDog();
}

const CAgilityBookTreeDataTrial* CAgilityBookTreeDataRun::GetDataTrial() const
{
	HTREEITEM hParent = m_pTree->GetTreeCtrl().GetParentItem(m_hItem);
	const CAgilityBookTreeData* pData = reinterpret_cast<const CAgilityBookTreeData*>(m_pTree->GetTreeCtrl().GetItemData(hParent));
	const CAgilityBookTreeDataTrial* pTrial = dynamic_cast<const CAgilityBookTreeDataTrial*>(pData);
	ASSERT(pTrial);
	return pTrial;
}

CAgilityBookTreeDataDog* CAgilityBookTreeDataRun::GetDataDog()
{
	HTREEITEM hParent = m_pTree->GetTreeCtrl().GetParentItem(m_hItem);
	CAgilityBookTreeData* pData = reinterpret_cast<CAgilityBookTreeData*>(m_pTree->GetTreeCtrl().GetItemData(hParent));
	CAgilityBookTreeDataTrial* pTrial = dynamic_cast<CAgilityBookTreeDataTrial*>(pData);
	ASSERT(pTrial);
	return pTrial->GetDataDog();
}

CAgilityBookTreeDataTrial* CAgilityBookTreeDataRun::GetDataTrial()
{
	HTREEITEM hParent = m_pTree->GetTreeCtrl().GetParentItem(m_hItem);
	CAgilityBookTreeData* pData = reinterpret_cast<CAgilityBookTreeData*>(m_pTree->GetTreeCtrl().GetItemData(hParent));
	CAgilityBookTreeDataTrial* pTrial = dynamic_cast<CAgilityBookTreeDataTrial*>(pData);
	ASSERT(pTrial);
	return pTrial;
}

bool CAgilityBookTreeDataRun::OnUpdateCmd(UINT id) const
{
	bool bEnable = false;
	switch (id)
	{
	default:
		break;
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
	}
	return bEnable;
}

bool CAgilityBookTreeDataRun::OnCmd(UINT id, bool* bTreeSelectionSet)
{
	bool bModified = false;
	switch (id)
	{
	default:
		break;
	case ID_EDIT_COPY:
		{
			CElement tree;
			tree.SetName(CLIPDATA);
			GetRun()->Save(tree);
			CopyDataToClipboard(CAgilityBookOptions::GetClipboardFormat(CAgilityBookOptions::eFormatRun), tree, m_pTree->GetPrintLine(GetHTreeItem()));
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
		if (IDYES == AfxMessageBox(IDS_DELETE_EVENT_DATA, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2))
		{
			if (GetTrial()->GetRuns().DeleteRun(m_pRun))
			{
				CAgilityBookDoc* pDoc = m_pTree->GetDocument();
				// DeleteItem will cause this object to be deleted.
				m_pTree->GetTreeCtrl().DeleteItem(m_hItem);
				bModified = true;
				pDoc->UpdateAllViews(NULL, UPDATE_POINTS_VIEW|UPDATE_RUNS_VIEW);
			}
		}
		break;
	case ID_REORDER:
		ReOrderTrial(GetTrial(), m_pTree);
		break;
	}
	return bModified;
}

CString CAgilityBookTreeDataRun::OnNeedText() const
{
	CString run;
	run += m_pRun->GetDate().GetString(true, true).c_str();
	run += " ";
	run += m_pRun->GetEvent().c_str();
	return run;
}

void CAgilityBookTreeDataRun::Properties()
{
	CAgilityBookDoc* pDoc = m_pTree->GetDocument();
	// This may delete 'this'.
	if (OnCmd(ID_AGILITY_EDIT_RUN, NULL))
		pDoc->SetModifiedFlag();
}
