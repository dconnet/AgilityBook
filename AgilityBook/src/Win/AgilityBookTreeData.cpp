/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CAgilityBookTreeData classes
 * @author David Connet
 *
 * Revision History
 * @li 2012-09-29 DRC Strip the Runs View.
 * @li 2012-09-09 DRC Added 'titlePts' to 'Placement'.
 * @li 2012-03-03 DRC Fixed new unearned titles from showing up in view.
 * @li 2011-10-14 DRC Modify how reorder dialog is invoked.
 * @li 2010-12-30 DRC After copying/deleting runs, update multi-Q status.
 * @li 2010-12-05 DRC DOB can be invalid (on import). Don't show in tree.
 * @li 2010-01-02 DRC Fix initialization of date for a new run.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-08-14 DRC Fixed crash (on Mac) when editing dog.
 * @li 2009-02-02 DRC Ported to wxWidgets.
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
#include "AgilityBookTreeData.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookMenu.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTreeView.h"
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
#include "Globals.h"
#include "PointsData.h"
#include "Print.h"
#include "StringUtil.h"
#include "TabView.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


#pragma PRAGMA_TODO("Fix me")
#if 0
static bool EditDog(
		CAgilityBookTreeDataDog* pDogData,
		CAgilityBookTreeView* pTree,
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
	CDlgDog dlg(pTree->GetDocument(), pDog, wxGetApp().GetTopWindow(), nPage);
	if (wxID_OK == dlg.ShowModal())
	{
		pDogData = NULL; // Tree may have reloaded.
		bOk = true;
		if (bAdd)
		{
			if (!pTree->GetDocument()->Book().GetDogs().AddDog(pDog))
			{
				bOk = false;
			}
			else
			{
				wxTreeItemId hItem = pTree->InsertDog(pDog);
				pTree->SelectItem(hItem);
				if (bTreeSelectionSet)
					*bTreeSelectionSet = true;
			}
		}
		// No need to refresh item here - CDlgDog::OnOk will call the document
		// ResetVisibility which causes a tree load. Note: If the dog dlg gets
		// more intelligent (reset is only needed if changing titles/etc), then
		// we may need this. The problem is that pDogData may be deleted.
		// Need a way to track that pDogData is gone...
		//else
		//	pTree->RefreshItem(pDogData->GetId());
	}
	return bOk;
}


static bool EditTrial(
		CAgilityBookTreeDataDog* pDogData,
		CAgilityBookTreeDataTrial* pTrialData,
		CAgilityBookTreeView* pTree,
		bool* bTreeSelectionSet)
{
	assert(pDogData && pDogData->GetDog());
	bool bAdd = false;
	ARBDogPtr pDog = pDogData->GetDog();
	ARBDogTrialPtr pTrial;
	if (pTrialData)
		pTrial = pTrialData->GetTrial();
	if (!pTrial)
	{
		bAdd = true;
		pTrial = ARBDogTrialPtr(ARBDogTrial::New());
	}
	bool bOk = false;
	CDlgTrial dlg(pTree->GetDocument(), pTrial, wxGetApp().GetTopWindow());
	if (wxID_OK == dlg.ShowModal())
	{
		bOk = true;
		std::vector<CVenueFilter> venues;
		CFilterOptions::Options().GetFilterVenue(venues);
		if (bAdd)
		{
			if (!pDog->GetTrials().AddTrial(pTrial))
			{
				bOk = false;
				wxMessageBox(_("IDS_CREATETRIAL_FAILED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
			}
			else
			{
				pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
				pTree->GetDocument()->ResetVisibility(venues, pTrial);
				// Even though we will reset the tree, go ahead and add/select
				// the item into the tree here. That will make sure when the
				// tree is reloaded, that the new item is selected.
				wxTreeItemId hItem = pTree->InsertTrial(pTrial, pDogData->GetId());
				if (!hItem.IsOk())
				{
					wxMessageBox(_("IDS_CREATETRIAL_FILTERED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
				}
				else
				{
					pTree->SelectItem(hItem);
					if (bTreeSelectionSet)
						*bTreeSelectionSet = true;
				}
			}
		}
		else
		{
			pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
			pTree->GetDocument()->ResetVisibility(venues, pTrial);
			pTree->RefreshItem(pTrialData->GetId());
			if (dlg.RunsWereDeleted())
			{
				if (bTreeSelectionSet)
					*bTreeSelectionSet = true;
			}
		}
		// We have to update the tree even when we add above as it may have
		// caused the trial to be reordered.
		if (bOk)
		{
			CUpdateHint hint(UPDATE_POINTS_VIEW | UPDATE_TREE_VIEW);
			pTree->GetDocument()->UpdateAllViews(NULL, &hint);
		}
	}
	return bOk;
}


static bool EditRun(
		CAgilityBookTreeDataDog* pDogData,
		CAgilityBookTreeDataTrial* pTrialData,
		CAgilityBookTreeDataRun* pRunData,
		CAgilityBookTreeView* pTree,
		bool* bTreeSelectionSet)
{
	assert(pDogData && pDogData->GetDog());
	assert(pTrialData && pTrialData->GetTrial());
	bool bOk = false;
	bool bAdd = false;
	ARBDogRunPtr pRun;
	if (pRunData)
		pRun = pRunData->GetRun();
	if (!pRun)
	{
		ARBDogClubPtr pClub;
		if (!pTrialData->GetTrial()->GetClubs().GetPrimaryClub(&pClub))
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
		ARBDate date = pTrialData->GetTrial()->GetRuns().GetEndDate();
		// If this is the first run, the date won't be set.
		if (!date.IsValid())
			date.SetToday();
		pRun->SetDate(date);
	}
	CDlgRun dlg(pTree->GetDocument(), pTrialData->GetTrial(), pRun);
	if (wxID_OK == dlg.ShowModal())
	{
		bOk = true;
		std::vector<CVenueFilter> venues;
		CFilterOptions::Options().GetFilterVenue(venues);
		if (bAdd)
		{
			if (!pTrialData->GetTrial()->GetRuns().AddRun(pRun))
			{
				bOk = false;
				wxMessageBox(_("IDS_CREATERUN_FAILED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
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
				wxTreeItemId hItem = pTree->InsertRun(pTrialData->GetTrial(), pRun, pTrialData->GetId());
				if (!hItem.IsOk())
				{
					if (CFilterOptions::Options().IsFilterEnabled())
						wxMessageBox(_("IDS_CREATERUN_FILTERED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
				}
				else
				{
					pTree->SelectItem(hItem);
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
		{
			CUpdateHint hint(UPDATE_POINTS_VIEW | UPDATE_TREE_VIEW);
			pTree->GetDocument()->UpdateAllViews(NULL, &hint);
		}
	}
	return bOk;
}


static void UpdateFutureTrials(
		ARBConfig const& config,
		ARBDogPtr pDog,
		ARBDogTitlePtr title)
{
	// Find all trials in the future in the same venue.
	std::vector<ARBDogTrialPtr> trials;
	for (ARBDogTrialList::iterator i = pDog->GetTrials().begin();
		i != pDog->GetTrials().end();
		++i)
	{
		if ((*i)->HasVenue(title->GetVenue())
		&& 0 < (*i)->GetRuns().size())
		{
			ARBDate d1 = (*((*i)->GetRuns().begin()))->GetDate();
			if (d1 > ARBDate::Today())
			{
				trials.push_back(*i);
			}
		}
	}
	if (0 < trials.size())
	{
		/*
		Interesting problem. Level order can be implied by the actual order in
		the config. But what does moveup mean? I could be moving from (USDAA)
		Ch to Pf.
		- Need user option to suppress moveup if in top level
		- Need to add info to a title for what events it affects (or determining
		  if a moveup can happen can't be done)
		- more than one event may be a move (USDAA AD implies everything goes up
		Oh. Even more complicated. Earning NovA means NovB moveOVER.
		*/
#pragma PRAGMA_TODO("Add dialog to update future trials")
		/*
		Thought 2. Maybe we just fly an info dialog here 'You are entered in
		N upcoming trials (list). Please check for moveups.'
		*/
	}
}


static bool AddTitle(
		CAgilityBookTreeDataDog* pDogData,
		CAgilityBookTreeView* pTree)
{
	assert(pDogData && pDogData->GetDog());
	CDlgTitle dlg(pTree->GetDocument()->Book().GetConfig(), pDogData->GetDog()->GetTitles(), ARBDogTitlePtr());
	if (wxID_OK == dlg.ShowModal())
	{
		std::vector<CVenueFilter> venues;
		CFilterOptions::Options().GetFilterVenue(venues);
		pTree->GetDocument()->ResetVisibility(venues, dlg.GetNewTitle());
		UpdateFutureTrials(pTree->GetDocument()->Book().GetConfig(), pDogData->GetDog(), dlg.GetNewTitle());
		CUpdateHint hint(UPDATE_POINTS_VIEW);
		pTree->GetDocument()->UpdateAllViews(NULL, &hint);
		if (CAgilityBookOptions::AutoShowPropertiesOnNewTitle())
			EditDog(pDogData, pTree, NULL, 1);
		return true;
	}
	else
		return false;
}
#endif

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
	bool bLoaded = false;
	wxBusyCursor wait;
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
						wxMessageBox(_("IDS_CREATERUN_FAILED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
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
					m_pTree->Freeze();
					wxTreeItemId hItem;
					for (std::vector<ARBDogRunPtr>::iterator iter = runs.begin(); iter != runs.end(); ++iter)
					{
						ARBDogRunPtr pRun = *iter;
#pragma PRAGMA_TODO("Fix me")
#if 0
						hItem = m_pTree->InsertRun(pTrial, pRun, GetDataTrial()->GetId());
#endif
					}
					m_pTree->Thaw();
					m_pTree->Refresh();
					bool bOk = true;
					if (!hItem.IsOk())
					{
						bOk = false;
						if (CFilterOptions::Options().IsFilterEnabled())
							wxMessageBox(_("IDS_CREATERUN_FILTERED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
					}
					else
					{
#pragma PRAGMA_TODO("Fix me")
#if 0
						m_pTree->SelectItem(hItem);
						if (bTreeSelectionSet)
							*bTreeSelectionSet = true;
#endif
					}
					if (bOk)
					{
						pTrial->SetMultiQs(m_pTree->GetDocument()->Book().GetConfig());
						CUpdateHint hint(UPDATE_POINTS_VIEW | UPDATE_TREE_VIEW);
						m_pTree->GetDocument()->UpdateAllViews(NULL, &hint);
					}
				}
			}
			if (!bLoaded && 0 < err.m_ErrMsg.str().length())
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
				if (pNewTrial->Load(m_pTree->GetDocument()->Book().GetConfig(), tree->GetNthElementNode(0), ARBAgilityRecordBook::GetCurrentDocVersion(), err))
				{
					bLoaded = true;
					std::vector<CVenueFilter> venues;
					CFilterOptions::Options().GetFilterVenue(venues);
					if (!pDog->GetTrials().AddTrial(pNewTrial))
					{
						bLoaded = false;
						wxMessageBox(_("IDS_CREATETRIAL_FAILED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
					}
					else
					{
						pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
						m_pTree->GetDocument()->ResetVisibility(venues, pNewTrial);
						m_pTree->Freeze();
#pragma PRAGMA_TODO("Fix me")
						//wxTreeItemId hItem = m_pTree->InsertTrial(pNewTrial, GetDataDog()->GetId());
						m_pTree->Thaw();
						m_pTree->Refresh();
						bool bOk = true;
#pragma PRAGMA_TODO("Fix me")
#if 0
						if (!hItem.IsOk())
						{
							bOk = false;
							wxMessageBox(_("IDS_CREATETRIAL_FILTERED"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_STOP);
						}
						else
						{
							m_pTree->SelectItem(hItem);
							if (bTreeSelectionSet)
								*bTreeSelectionSet = true;
						}
#endif
						if (bOk)
						{
							pNewTrial->SetMultiQs(m_pTree->GetDocument()->Book().GetConfig());
							CUpdateHint hint(UPDATE_POINTS_VIEW | UPDATE_TREE_VIEW);
							m_pTree->GetDocument()->UpdateAllViews(NULL, &hint);
						}
					}
				}
				else if (0 < err.m_ErrMsg.str().length())
					wxMessageBox(StringUtil::stringWX(err.m_ErrMsg.str()), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
			}
		}
	}
	return bLoaded;
}

////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeDataDog::CAgilityBookTreeDataDog(
		CAgilityBookTreeView* pTree,
		ARBDogPtr pDog)
	: CAgilityBookTreeData(pTree)
	, m_pDog(pDog)
{
}


CAgilityBookTreeDataDog::~CAgilityBookTreeDataDog()
{
}


bool CAgilityBookTreeDataDog::OnUpdateCmd(int id, bool& ioEnable) const
{
	bool bHandled = true;
	bool bEnable = false;
	switch (id)
	{
	default:
		bHandled = false;
		break;
	case wxID_DUPLICATE:
	case wxID_CUT:
	case wxID_COPY:
		bEnable = true;
		break;
	case wxID_PASTE:
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
#pragma PRAGMA_TODO("Fix me")
#if 0
	case ID_EXPAND:
		if (GetId().IsOk() && m_pTree && m_pTree->ItemHasChildren(GetId()))
		{
			if (!m_pTree->IsExpanded(GetId()))
				bEnable = true;
		}
		break;
	case ID_COLLAPSE:
		if (GetId().IsOk() && m_pTree && m_pTree->ItemHasChildren(GetId()))
		{
			if (m_pTree->IsExpanded(GetId()))
				bEnable = true;
		}
		break;
	case ID_EXPAND_ALL:
	case ID_COLLAPSE_ALL:
		if (GetId().IsOk() && m_pTree && m_pTree->ItemHasChildren(GetId()))
			bEnable = true;
		break;
#endif
	}
	ioEnable = bEnable;
	return bHandled;
}


bool CAgilityBookTreeDataDog::OnCmd(
		int id,
		bool& bModified,
		bool* bTreeSelectionSet)
{
	static bool bPrompt = true;
	bool bHandled = true;
#pragma PRAGMA_TODO("Fix me")
#if 0
	switch (id)
	{
	default:
		bHandled = false;
		break;
	case wxID_PREFERENCES:
		{
			CDlgOptions options(m_pTree->GetDocument(), wxGetApp().GetTopWindow(), CDlgOptions::GetFilterPage());
			options.ShowModal();
		}
		break;
	case wxID_DUPLICATE:
		if (GetDog())
		{
			ARBDogPtr pDog = GetDog()->Clone();
			m_pTree->GetDocument()->Book().GetDogs().AddDog(pDog);
			bModified = true;
			CUpdateHint hint(UPDATE_TREE_VIEW | UPDATE_POINTS_VIEW);
			m_pTree->GetDocument()->UpdateAllViews(NULL, &hint);
		}
		break;
	case wxID_CUT:
		OnCmd(wxID_COPY, bModified, bTreeSelectionSet);
		bPrompt = false;
		OnCmd(ID_AGILITY_DELETE_DOG, bModified, bTreeSelectionSet);
		bPrompt = true;
		bModified = true;
		break;
	case wxID_COPY:
		{
			CClipboardDataWriter clpData;
			if (clpData.isOkay())
			{
				wxBusyCursor wait;
				ElementNodePtr tree(ElementNode::New(CLIPDATA));
				GetDog()->Save(tree, m_pTree->GetDocument()->Book().GetConfig());
				clpData.AddData(eFormatDog, tree);
				clpData.AddData(m_pTree->GetPrintLine(GetId()));
				clpData.CommitData();
			}
		}
		break;
	case wxID_PASTE:
		if (DoPaste(bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_EDIT_DOG:
		EditDog(this, m_pTree, bTreeSelectionSet);
		break;
	case ID_AGILITY_NEW_DOG:
		EditDog(NULL, m_pTree, bTreeSelectionSet);
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
		|| wxYES == wxMessageBox(_("IDS_DELETE_DOG_DATA"), wxMessageBoxCaptionStr, wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_QUESTION))
		{
			if (GetId().IsOk() && m_pTree->GetDocument()->Book().GetDogs().DeleteDog(m_pDog))
			{
				CAgilityBookDoc* pDoc = m_pTree->GetDocument();
				// Delete() will cause this object to be deleted.
				m_pTree->Delete(GetId());
				bModified = true;
				CUpdateHint hint(UPDATE_POINTS_VIEW);
				pDoc->UpdateAllViews(NULL, &hint);
			}
		}
		break;

	case ID_REORDER:
		if (m_pTree)
		{
			CDlgReorder dlg(m_pTree->GetDocument(), &(m_pTree->GetDocument()->Book().GetDogs()));
			dlg.ShowModal();
		}
		break;

	case ID_EXPAND:
		if (GetId().IsOk())
		{
			m_pTree->Expand(GetId());
			m_pTree->EnsureVisible(GetId());
		}
		break;

	case ID_EXPAND_ALL:
		if (GetId().IsOk())
		{
			m_pTree->ExpandAllChildren(GetId());
			m_pTree->EnsureVisible(GetId());
		}
		break;

	case ID_COLLAPSE:
		if (GetId().IsOk())
		{
			m_pTree->Collapse(GetId());
			m_pTree->EnsureVisible(GetId());
		}
		break;

	case ID_COLLAPSE_ALL:
		if (GetId().IsOk())
		{
			m_pTree->CollapseAllChildren(GetId());
			m_pTree->EnsureVisible(GetId());
		}
		break;
	}
#endif
	return bHandled;
}


std::wstring CAgilityBookTreeDataDog::OnNeedText() const
{
	return std::wstring();
}
int CAgilityBookTreeDataDog::OnNeedIcon() const
{
#pragma PRAGMA_TODO("Fix me")
	//return m_pTree->GetImageList().Dog();
	return -1;
}


void CAgilityBookTreeDataDog::Properties()
{
	CAgilityBookDoc* pDoc = m_pTree->GetDocument();
	// This may delete 'this'.
	bool bModified = false;
	if (OnCmd(ID_AGILITY_EDIT_DOG, bModified, NULL))
	{
		if (bModified)
			pDoc->Modify(true);
	}
}

/////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeDataTrial::CAgilityBookTreeDataTrial(
		CAgilityBookTreeView* pTree,
		ARBDogTrialPtr pTrial)
	: CAgilityBookTreeData(pTree)
	, m_pTrial(pTrial)
	, m_idxIcon(-1)
{
#pragma PRAGMA_TODO("Fix me")
//	m_idxIcon = m_pTree->GetImageList().Trial();
	if (m_pTrial)
	{
		ARBDogClubPtr pClub;
		if (m_pTrial->GetClubs().GetPrimaryClub(&pClub))
		{
			ARBConfigVenuePtr pVenue;
			if (pTree->GetDocument()->Book().GetConfig().GetVenues().FindVenue(pClub->GetVenue(), &pVenue))
			{
#pragma PRAGMA_TODO("Fix me")
#if 0
				short idx = pVenue->GetIcon();
				if (0 <= idx && idx < m_pTree->GetImageList().GetImageCount())
					m_idxIcon = idx;
#endif
			}
		}
	}
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
#pragma PRAGMA_TODO("Fix me")
//	if (!GetId().IsOk())
		return NULL;
#if 0
	wxTreeItemId hParent = m_pTree->GetItemParent(GetId());
	CAgilityBookTreeData const* pData = m_pTree->GetTreeItem(hParent);
	CAgilityBookTreeDataDog const* pDog = dynamic_cast<CAgilityBookTreeDataDog const*>(pData);
	assert(pDog);
	return pDog;
#endif
}


CAgilityBookTreeDataDog* CAgilityBookTreeDataTrial::GetDataDog()
{
#pragma PRAGMA_TODO("Fix me")
//	if (!GetId().IsOk())
		return NULL;
#if 0
	wxTreeItemId hParent = m_pTree->GetItemParent(GetId());
	CAgilityBookTreeData* pData = m_pTree->GetTreeItem(hParent);
	CAgilityBookTreeDataDog* pDog = dynamic_cast<CAgilityBookTreeDataDog*>(pData);
	assert(pDog);
	return pDog;
#endif
}


bool CAgilityBookTreeDataTrial::OnUpdateCmd(int id, bool& ioEnable) const
{
	bool bHandled = true;
	bool bEnable = false;
#pragma PRAGMA_TODO("Fix me")
#if 0
	switch (id)
	{
	default:
		bHandled = false;
		break;
	case wxID_DUPLICATE:
	case wxID_CUT:
	case wxID_COPY:
		bEnable = true;
		break;
	case wxID_PASTE:
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
#endif
	ioEnable = bEnable;
	return bHandled;
}


bool CAgilityBookTreeDataTrial::OnCmd(
		int id,
		bool& bModified,
		bool* bTreeSelectionSet)
{
	static bool bPrompt = true;
	bool bHandled = true;
#pragma PRAGMA_TODO("Fix me")
#if 0
	switch (id)
	{
	default:
		bHandled = false;
		break;
	case wxID_PREFERENCES:
		{
			CDlgOptions options(m_pTree->GetDocument(), wxGetApp().GetTopWindow(), CDlgOptions::GetFilterPage());
			options.ShowModal();
		}
		break;
	case wxID_DUPLICATE:
		if (GetTrial())
		{
			ARBDogTrialPtr pTrial = GetTrial()->Clone();
			GetDog()->GetTrials().AddTrial(pTrial);
			bool bDescending = !CAgilityBookOptions::GetNewestDatesFirst();
			GetDog()->GetTrials().sort(bDescending);
			bModified = true;
			CUpdateHint hint(UPDATE_TREE_VIEW | UPDATE_POINTS_VIEW);
			m_pTree->GetDocument()->UpdateAllViews(NULL, &hint);
		}
		break;
	case wxID_CUT:
		OnCmd(wxID_COPY, bModified, bTreeSelectionSet);
		bPrompt = false;
		OnCmd(ID_AGILITY_DELETE_TRIAL, bModified, bTreeSelectionSet);
		bPrompt = true;
		bModified = true;
		break;
	case wxID_COPY:
		{
			CClipboardDataWriter clpData;
			if (clpData.isOkay())
			{
				wxBusyCursor wait;
				ElementNodePtr tree(ElementNode::New(CLIPDATA));
				GetTrial()->Save(tree, m_pTree->GetDocument()->Book().GetConfig());
				clpData.AddData(eFormatTrial, tree);
				clpData.AddData(m_pTree->GetPrintLine(GetId()));
				clpData.CommitData();
			}
		}
		break;
	case wxID_PASTE:
		if (DoPaste(bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_EDIT_TRIAL:
		if (EditTrial(GetDataDog(), this, m_pTree, bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_NEW_DOG:
		EditDog(NULL, m_pTree, bTreeSelectionSet);
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
		|| wxYES == wxMessageBox(_("IDS_DELETE_TRIAL_DATA"), wxMessageBoxCaptionStr, wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_QUESTION))
		{
			if (GetId() && GetDog()->GetTrials().DeleteTrial(m_pTrial))
			{
				CAgilityBookDoc* pDoc = m_pTree->GetDocument();
				// Delete() will cause this object to be deleted.
				m_pTree->Delete(GetId());
				bModified = true;
				CUpdateHint hint(UPDATE_POINTS_VIEW);
				pDoc->UpdateAllViews(NULL, &hint);
			}
		}
		break;
	case ID_REORDER:
		if (GetTrial())
		{
			CDlgReorder dlg(m_pTree->GetDocument(), GetTrial());
			dlg.ShowModal();
		}
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
#endif
	return bHandled;
}


std::wstring CAgilityBookTreeDataTrial::OnNeedText() const
{
	return std::wstring();
}
int CAgilityBookTreeDataTrial::OnNeedIcon() const
{
	return m_idxIcon;
}


void CAgilityBookTreeDataTrial::Properties()
{
	CAgilityBookDoc* pDoc = m_pTree->GetDocument();
	// This may delete 'this'.
	bool bModified = false;
	if (OnCmd(ID_AGILITY_EDIT_TRIAL, bModified, NULL))
	{
		if (bModified)
			pDoc->Modify(true);
	}
}

/////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeDataRun::CAgilityBookTreeDataRun(
		CAgilityBookTreeView* pTree,
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
#pragma PRAGMA_TODO("Fix me")
//	if (!GetId().IsOk())
		return NULL;
#if 0
	wxTreeItemId hParent = m_pTree->GetItemParent(GetId());
	CAgilityBookTreeData const* pData = m_pTree->GetTreeItem(hParent);
	CAgilityBookTreeDataTrial const* pTrial = dynamic_cast<CAgilityBookTreeDataTrial const*>(pData);
	assert(pTrial);
	return pTrial->GetDataDog();
#endif
}


CAgilityBookTreeDataTrial const* CAgilityBookTreeDataRun::GetDataTrial() const
{
#pragma PRAGMA_TODO("Fix me")
//	if (!GetId().IsOk())
		return NULL;
#if 0
	wxTreeItemId hParent = m_pTree->GetItemParent(GetId());
	CAgilityBookTreeData const* pData = m_pTree->GetTreeItem(hParent);
	CAgilityBookTreeDataTrial const* pTrial = dynamic_cast<CAgilityBookTreeDataTrial const*>(pData);
	assert(pTrial);
	return pTrial;
#endif
}


CAgilityBookTreeDataDog* CAgilityBookTreeDataRun::GetDataDog()
{
#pragma PRAGMA_TODO("Fix me")
//	if (!GetId().IsOk())
		return NULL;
#if 0
	wxTreeItemId hParent = m_pTree->GetItemParent(GetId());
	CAgilityBookTreeData* pData = m_pTree->GetTreeItem(hParent);
	CAgilityBookTreeDataTrial* pTrial = dynamic_cast<CAgilityBookTreeDataTrial*>(pData);
	assert(pTrial);
	return pTrial->GetDataDog();
#endif
}


CAgilityBookTreeDataTrial* CAgilityBookTreeDataRun::GetDataTrial()
{
#pragma PRAGMA_TODO("Fix me")
//	if (!GetId().IsOk())
		return NULL;
#if 0
	wxTreeItemId hParent = m_pTree->GetItemParent(GetId());
	CAgilityBookTreeData* pData = m_pTree->GetTreeItem(hParent);
	CAgilityBookTreeDataTrial* pTrial = dynamic_cast<CAgilityBookTreeDataTrial*>(pData);
	assert(pTrial);
	return pTrial;
#endif
}


bool CAgilityBookTreeDataRun::OnUpdateCmd(int id, bool& ioEnable) const
{
	bool bHandled = true;
	bool bEnable = false;
#pragma PRAGMA_TODO("Fix me")
#if 0
	switch (id)
	{
	default:
		bHandled = false;
		break;
	case wxID_DUPLICATE:
	case wxID_CUT:
	case wxID_COPY:
		bEnable = true;
		break;
	case wxID_PASTE:
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
#endif
	ioEnable = bEnable;
	return bHandled;
}


bool CAgilityBookTreeDataRun::OnCmd(
		int id,
		bool& bModified,
		bool* bTreeSelectionSet)
{
	static bool bPrompt = true;
	bool bHandled = true;
#pragma PRAGMA_TODO("Fix me")
#if 0
	switch (id)
	{
	default:
		bHandled = false;
		break;
	case wxID_PREFERENCES:
		{
			CDlgOptions options(m_pTree->GetDocument(), wxGetApp().GetTopWindow(), CDlgOptions::GetFilterPage());
			options.ShowModal();
		}
		break;
	case wxID_DUPLICATE:
		if (GetRun())
		{
			ARBDogRunPtr pRun = GetRun()->Clone();
			GetTrial()->GetRuns().AddRun(pRun);
			GetTrial()->GetRuns().sort();
			bModified = true;
			CUpdateHint hint(UPDATE_TREE_VIEW | UPDATE_POINTS_VIEW);
			m_pTree->GetDocument()->UpdateAllViews(NULL, &hint);
		}
		break;
	case wxID_CUT:
		OnCmd(wxID_COPY, bModified, bTreeSelectionSet);
		bPrompt = false;
		OnCmd(ID_AGILITY_DELETE_RUN, bModified, bTreeSelectionSet);
		bPrompt = true;
		bModified = true;
		break;
	case wxID_COPY:
		{
			CClipboardDataWriter clpData;
			if (clpData.isOkay())
			{
				wxBusyCursor wait;
				ElementNodePtr tree(ElementNode::New(CLIPDATA));
				GetRun()->Save(tree, NULL, m_pTree->GetDocument()->Book().GetConfig()); // copy/paste: title points don't matter
				clpData.AddData(eFormatRun, tree);
				clpData.AddData(m_pTree->GetPrintLine(GetId()));
				clpData.CommitData();
			}
		}
		break;
	case wxID_PASTE:
		if (DoPaste(bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_EDIT_RUN:
		if (EditRun(GetDataDog(), GetDataTrial(), this, m_pTree, bTreeSelectionSet))
			bModified = true;
		break;
	case ID_AGILITY_NEW_DOG:
		EditDog(NULL, m_pTree, bTreeSelectionSet);
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
		|| wxYES == wxMessageBox(_("IDS_DELETE_EVENT_DATA"), wxMessageBoxCaptionStr, wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_QUESTION))
		{
			if (GetId().IsOk() && GetTrial()->GetRuns().DeleteRun(m_pRun))
			{
				CAgilityBookDoc* pDoc = m_pTree->GetDocument();
				ARBDogTrialPtr pTrial = GetTrial();
				// Delete() will cause this object to be deleted.
				m_pTree->Delete(GetId());
				bModified = true;
				pTrial->SetMultiQs(pDoc->Book().GetConfig());
				CUpdateHint hint(UPDATE_POINTS_VIEW);
				pDoc->UpdateAllViews(NULL, &hint);
			}
		}
		break;
	case ID_REORDER:
		if (GetTrial())
		{
			CDlgReorder dlg(m_pTree->GetDocument(), GetTrial());
			dlg.ShowModal();
		}
		break;
	case ID_AGILITY_PRINT_RUNS:
		{
			std::vector<RunInfo> runs;
			runs.push_back(RunInfo(GetTrial(), m_pRun));
			PrintRuns(&(m_pTree->GetDocument()->Book().GetConfig()), GetDog(), runs);
		}
		break;
	}
#endif
	return bHandled;
}


std::wstring CAgilityBookTreeDataRun::OnNeedText() const
{
	return std::wstring();
}
int CAgilityBookTreeDataRun::OnNeedIcon() const
{
#pragma PRAGMA_TODO("Fix me")
	//return m_pTree->GetImageList().Run();
	return -1;
}


void CAgilityBookTreeDataRun::Properties()
{
	CAgilityBookDoc* pDoc = m_pTree->GetDocument();
	// This may delete 'this'.
	bool bModified = false;
	if (OnCmd(ID_AGILITY_EDIT_RUN, bModified, NULL))
	{
		if (bModified)
			pDoc->Modify(true);
	}
}
