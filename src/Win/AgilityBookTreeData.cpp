/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CAgilityBookTreeData classes
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2018-09-15 Refactored how tree/list handle common actions.
 * 2015-11-27 Use subname for event, if set.
 * 2012-09-09 Added 'titlePts' to 'Placement'.
 * 2012-03-03 Fixed new unearned titles from showing up in view.
 * 2011-10-14 Modify how reorder dialog is invoked.
 * 2010-12-30 After copying/deleting runs, update multi-Q status.
 * 2010-12-05 DOB can be invalid (on import). Don't show in tree.
 * 2010-01-02 Fix initialization of date for a new run.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-14 Fixed crash (on Mac) when editing dog.
 * 2009-02-02 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-10 Only sort runs one way, the UI handles everything else.
 * 2004-10-21 When an item is inserted in the tree, the text callback
 *            can actually occur BEFORE we set the HTREEITEM.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-08-11 Added verified-trial info to listing
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-06-06 Added duplicate/cut/paste support for dogs.
 * 2004-05-09 After adding a title, display dog property dlg.
 * 2004-04-15 Added Duplicate menu item.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-12-30 Implemented customized text in tree.
 * 2003-09-04 When pasting, set document modified flag. Clean up some
 *            warning messages when creating/pasting items and a filter
 *            is set. (It was saying they weren't added - they were,
 *            they just weren't visible.) Also, after adding/editing
 *            trials/runs, make sure the items get sorted properly.
 * 2003-09-03 When pasting trials/runs, select the new item.
 * 2003-08-30 Copy a trial/run in the tree to the clipboard in dual
 *            form. The internal form allows copying of the entry.
 *            The text form copies what is printed.
 * 2003-08-24 Optimized filtering by adding boolean into ARBBase to
 *            prevent constant re-evaluation.
 * 2003-07-24 Added reorder support on all items. Made dogs user-sorted.
 */

#include "stdafx.h"
#include "AgilityBookTreeData.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookMenu.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTreeView.h"
#include "ClipBoard.h"
#include "DlgAssignColumns.h"
#include "DlgOptions.h"
#include "FilterOptions.h"
#include "PointsData.h"
#include "Print.h"
#include "TabView.h"

#include "ARB/ARBDog.h"
#include "ARB/ARBDogRun.h"
#include "ARB/ARBDogTrial.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

////////////////////////////////////////////////////////////////////////////

std::vector<long> const& CAgilityBookTreeData::GetDogColumns() const
{
	return m_pTree->m_Columns[0];
}


std::vector<long> const& CAgilityBookTreeData::GetTrialColumns() const
{
	return m_pTree->m_Columns[1];
}


std::vector<long> const& CAgilityBookTreeData::GetRunColumns() const
{
	return m_pTree->m_Columns[2];
}

////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeDataDog::CAgilityBookTreeDataDog(CAgilityBookTreeView* pTree, ARBDogPtr const& inDog)
	: CAgilityBookTreeData(pTree)
	, m_pDog(inDog)
{
}


CAgilityBookTreeDataDog::~CAgilityBookTreeDataDog()
{
}


std::wstring CAgilityBookTreeDataDog::OnNeedText() const
{
	fmt::wmemory_buffer str;
	for (size_t idx = 0; idx < GetDogColumns().size(); ++idx)
	{
		if (0 < idx)
			fmt::format_to(str, L" ");
		switch (GetDogColumns()[idx])
		{
		case IO_TREE_DOG_REGNAME:
			fmt::format_to(str, L"{}", m_pDog->GetRegisteredName());
			break;
		case IO_TREE_DOG_CALLNAME:
			fmt::format_to(str, L"{}", m_pDog->GetCallName());
			break;
		case IO_TREE_DOG_BREED:
			fmt::format_to(str, L"{}", m_pDog->GetBreed());
			break;
		case IO_TREE_DOG_DOB:
			if (m_pDog->GetDOB().IsValid())
				fmt::format_to(str, L"{}", m_pDog->GetDOB().GetString());
			break;
		case IO_TREE_DOG_AGE:
			if (m_pDog->GetDOB().IsValid())
			{
				wxDateTime dob;
				m_pDog->GetDOB().GetDate(dob);
				wxDateTime current = wxDateTime::Now();
				if (m_pDog->GetDeceased().IsValid())
					m_pDog->GetDeceased().GetDate(current);
				wxTimeSpan age = current - dob;
				fmt::format_to(str, _("IDS_YEARS").wx_str(), ARBDouble::ToString(age.GetDays() / 365.0, 1));
			}
			break;
		}
	}
	return fmt::to_string(str);
}


int CAgilityBookTreeDataDog::OnNeedIcon() const
{
	return m_pTree->GetImageList().IndexDog();
}


void CAgilityBookTreeDataDog::Properties()
{
	m_pTree->GetDocument()->EditDog(GetDog());
}


bool CAgilityBookTreeDataDog::DoCopy()
{
	bool bCopied = false;
	if (GetDog())
	{
		CClipboardDataWriter clpData;
		if (clpData.isOkay())
		{
			wxBusyCursor wait;
			ElementNodePtr tree(ElementNode::New(CLIPDATA));
			GetDog()->Save(tree, m_pTree->GetDocument()->Book().GetConfig());
			clpData.AddData(ARBClipFormat::Dog, tree);
			clpData.AddData(m_pTree->GetPrintLine(GetId()));
			clpData.CommitData();
			bCopied = true;
		}
	}
	return bCopied;
}


bool CAgilityBookTreeDataDog::DoDuplicate()
{
	if (!GetDog())
		return false;
	CAgilityBookDoc* pDoc = m_pTree->GetDocument();
	ARBDogPtr pDog = GetDog()->Clone();
	pDoc->Book().GetDogs().AddDog(pDog);
	CUpdateHint hint(UPDATE_TREE_VIEW | UPDATE_RUNS_VIEW | UPDATE_POINTS_VIEW);
	pDoc->UpdateAllViews(nullptr, &hint);
	pDoc->Modify(true);
	return true;
}


bool CAgilityBookTreeDataDog::DoDelete(bool bSilent)
{
	bool bModified = false;
	if (bSilent
		|| wxYES
			   == wxMessageBox(
				   _("IDS_DELETE_DOG_DATA"),
				   wxMessageBoxCaptionStr,
				   wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_QUESTION))
	{
		CAgilityBookDoc* pDoc = m_pTree->GetDocument();
		if (GetId().IsOk() && pDoc->Book().GetDogs().DeleteDog(m_pDog))
		{
			CAgilityBookOptions::CleanLastItems(m_pDog->GetCallName());
			// Delete() will cause this object to be deleted.
			m_pTree->Delete(GetId());
			bModified = true;
			CUpdateHint hint(UPDATE_POINTS_VIEW | UPDATE_RUNS_VIEW);
			pDoc->UpdateAllViews(nullptr, &hint);
		}
		if (bModified)
			pDoc->Modify(true);
	}
	return bModified;
}

/////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeDataTrial::CAgilityBookTreeDataTrial(CAgilityBookTreeView* pTree, ARBDogTrialPtr const& inTrial)
	: CAgilityBookTreeData(pTree)
	, m_pTrial(inTrial)
	, m_idxIcon(-1)
{
	m_idxIcon = m_pTree->GetImageList().IndexTrial();
	if (m_pTrial)
	{
		ARBDogClubPtr pClub = m_pTrial->GetClubs().GetMainClub();
		if (pClub)
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
	if (!GetId().IsOk())
		return nullptr;
	wxTreeItemId hParent = m_pTree->GetItemParent(GetId());
	CAgilityBookTreeData const* pData = m_pTree->GetTreeItem(hParent);
	CAgilityBookTreeDataDog const* pDog = dynamic_cast<CAgilityBookTreeDataDog const*>(pData);
	assert(pDog);
	return pDog;
}


CAgilityBookTreeDataDog* CAgilityBookTreeDataTrial::GetDataDog()
{
	if (!GetId().IsOk())
		return nullptr;
	wxTreeItemId hParent = m_pTree->GetItemParent(GetId());
	CAgilityBookTreeData* pData = m_pTree->GetTreeItem(hParent);
	CAgilityBookTreeDataDog* pDog = dynamic_cast<CAgilityBookTreeDataDog*>(pData);
	assert(pDog);
	return pDog;
}


std::wstring CAgilityBookTreeDataTrial::OnNeedText() const
{
	fmt::wmemory_buffer str;
	bool bNeedSpace = false;
	for (size_t idx = 0; idx < GetTrialColumns().size(); ++idx)
	{
		switch (GetTrialColumns()[idx])
		{
		case IO_TREE_TRIAL_START:
			if (m_pTrial->GetStartDate().IsValid())
			{
				if (bNeedSpace)
				{
					if (IO_TREE_TRIAL_END == GetTrialColumns()[idx - 1])
						fmt::format_to(str, L"-");
					else
						fmt::format_to(str, L" ");
				}
				fmt::format_to(str, L"{}", m_pTrial->GetStartDate().GetString());
				bNeedSpace = true;
			}
			break;
		case IO_TREE_TRIAL_END:
			if (m_pTrial->GetEndDate().IsValid())
			{
				if (bNeedSpace)
				{
					if (IO_TREE_TRIAL_START == GetTrialColumns()[idx - 1])
						fmt::format_to(str, L"-");
					else
						fmt::format_to(str, L" ");
				}
				fmt::format_to(str, L"{}", m_pTrial->GetEndDate().GetString());
				bNeedSpace = true;
			}
			break;
		case IO_TREE_TRIAL_VERIFIED:
			if (bNeedSpace)
				fmt::format_to(str, L" ");
			if (m_pTrial->IsVerified())
				fmt::format_to(str, L"*");
			else
				fmt::format_to(str, L"  "); // 2 spaces due to font (variable spacing)
			bNeedSpace = true;
			break;
		case IO_TREE_TRIAL_CLUB:
		{
			if (bNeedSpace && 0 < m_pTrial->GetClubs().size())
				fmt::format_to(str, L" ");
			fmt::format_to(str, L"{}", m_pTrial->GetClubs().GetClubList(true));
			bNeedSpace = true;
		}
		break;
		case IO_TREE_TRIAL_VENUE:
		{
			if (bNeedSpace && 0 < m_pTrial->GetClubs().size())
				fmt::format_to(str, L" ");
			fmt::format_to(str, L"{}", m_pTrial->GetClubs().GetClubList(false));
			bNeedSpace = true;
		}
		break;
		case IO_TREE_TRIAL_LOCATION:
			if (!m_pTrial->GetLocation().empty())
			{
				if (bNeedSpace)
					fmt::format_to(str, L" ");
				fmt::format_to(str, L"{}", m_pTrial->GetLocation());
				bNeedSpace = true;
			}
			break;
		case IO_TREE_TRIAL_NOTES:
			if (!m_pTrial->GetNote().empty())
			{
				if (bNeedSpace)
					fmt::format_to(str, L" ");
				fmt::format_to(str, L"{}", StringUtil::Replace(m_pTrial->GetNote(), L"\n", L" "));
				bNeedSpace = true;
			}
			break;
		}
	}
	return fmt::to_string(str);
}


int CAgilityBookTreeDataTrial::OnNeedIcon() const
{
	return m_idxIcon;
}


void CAgilityBookTreeDataTrial::Properties()
{
	if (GetTrial())
		m_pTree->GetDocument()->EditTrial(GetDog(), GetTrial());
}


bool CAgilityBookTreeDataTrial::DoCopy()
{
	bool bCopied = false;
	if (GetTrial())
	{
		CClipboardDataWriter clpData;
		if (clpData.isOkay())
		{
			wxBusyCursor wait;
			ElementNodePtr tree(ElementNode::New(CLIPDATA));
			GetTrial()->Save(tree, m_pTree->GetDocument()->Book().GetConfig());
			clpData.AddData(ARBClipFormat::Trial, tree);
			clpData.AddData(m_pTree->GetPrintLine(GetId()));
			clpData.CommitData();
			bCopied = true;
		}
	}
	return bCopied;
}


bool CAgilityBookTreeDataTrial::DoDuplicate()
{
	if (!GetTrial())
		return false;
	CAgilityBookDoc* pDoc = m_pTree->GetDocument();
	ARBDogTrialPtr pTrial = GetTrial()->Clone();
	GetDog()->GetTrials().AddTrial(pTrial, !CAgilityBookOptions::GetNewestDatesFirst());
	CUpdateHint hint(UPDATE_TREE_VIEW | UPDATE_RUNS_VIEW | UPDATE_POINTS_VIEW);
	pDoc->UpdateAllViews(nullptr, &hint);
	pDoc->Modify(true);
	return true;
}


bool CAgilityBookTreeDataTrial::DoDelete(bool bSilent)
{
	if (!m_pTrial)
		return false;

	wxTreeItemId id = GetId();
	if (!m_pTree->GetDocument()->DeleteTrial(GetDog(), GetTrial(), bSilent))
		return false;

	// Delete() will cause this object to be deleted.
	m_pTree->Delete(id);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeDataRun::CAgilityBookTreeDataRun(CAgilityBookTreeView* pTree, ARBDogRunPtr const& inRun)
	: CAgilityBookTreeData(pTree)
	, m_pRun(inRun)
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
	if (!GetId().IsOk())
		return nullptr;
	wxTreeItemId hParent = m_pTree->GetItemParent(GetId());
	CAgilityBookTreeData const* pData = m_pTree->GetTreeItem(hParent);
	CAgilityBookTreeDataTrial const* pTrial = dynamic_cast<CAgilityBookTreeDataTrial const*>(pData);
	assert(pTrial);
	return pTrial->GetDataDog();
}


CAgilityBookTreeDataTrial const* CAgilityBookTreeDataRun::GetDataTrial() const
{
	if (!GetId().IsOk())
		return nullptr;
	wxTreeItemId hParent = m_pTree->GetItemParent(GetId());
	CAgilityBookTreeData const* pData = m_pTree->GetTreeItem(hParent);
	CAgilityBookTreeDataTrial const* pTrial = dynamic_cast<CAgilityBookTreeDataTrial const*>(pData);
	assert(pTrial);
	return pTrial;
}


CAgilityBookTreeDataDog* CAgilityBookTreeDataRun::GetDataDog()
{
	if (!GetId().IsOk())
		return nullptr;
	wxTreeItemId hParent = m_pTree->GetItemParent(GetId());
	CAgilityBookTreeData* pData = m_pTree->GetTreeItem(hParent);
	CAgilityBookTreeDataTrial* pTrial = dynamic_cast<CAgilityBookTreeDataTrial*>(pData);
	assert(pTrial);
	return pTrial->GetDataDog();
}


CAgilityBookTreeDataTrial* CAgilityBookTreeDataRun::GetDataTrial()
{
	if (!GetId().IsOk())
		return nullptr;
	wxTreeItemId hParent = m_pTree->GetItemParent(GetId());
	CAgilityBookTreeData* pData = m_pTree->GetTreeItem(hParent);
	CAgilityBookTreeDataTrial* pTrial = dynamic_cast<CAgilityBookTreeDataTrial*>(pData);
	assert(pTrial);
	return pTrial;
}


std::wstring CAgilityBookTreeDataRun::OnNeedText() const
{
	fmt::wmemory_buffer str;
	for (size_t idx = 0; idx < GetRunColumns().size(); ++idx)
	{
		if (0 < idx)
			fmt::format_to(str, L" ");
		switch (GetRunColumns()[idx])
		{
		case IO_TREE_RUN_DATE:
			fmt::format_to(str, L"{}", m_pRun->GetDate().GetString());
			break;
		case IO_TREE_RUN_Q:
		{
			std::wstring q;
			if (m_pRun->GetQ().Qualified())
			{
				std::vector<ARBConfigMultiQPtr> multiQs;
				if (0 < m_pRun->GetMultiQs(multiQs))
				{
					for (std::vector<ARBConfigMultiQPtr>::iterator iMultiQ = multiQs.begin(); iMultiQ != multiQs.end();
						 ++iMultiQ)
					{
						if (!q.empty())
							q += L"/";
						q += (*iMultiQ)->GetShortName();
					}
				}
				if (Q::SuperQ == m_pRun->GetQ())
				{
					std::wstring tmp(StringUtil::stringW(_("IDS_SQ")));
					if (!q.empty())
						q += L"/";
					q += tmp;
				}
			}
			if (q.empty())
				q = m_pRun->GetQ().str();
			fmt::format_to(str, L"{}", q);
		}
		break;
		case IO_TREE_RUN_EVENT:
			if (m_pRun->GetSubName().empty())
				fmt::format_to(str, L"{}", m_pRun->GetEvent());
			else
				fmt::format_to(str, L"{}", m_pRun->GetSubName());
			break;
		case IO_TREE_RUN_DIVISION:
			fmt::format_to(str, L"{}", m_pRun->GetDivision());
			break;
		case IO_TREE_RUN_LEVEL:
			fmt::format_to(str, L"{}", m_pRun->GetLevel());
			break;
		case IO_TREE_RUN_HEIGHT:
			fmt::format_to(str, L"{}", m_pRun->GetHeight());
			break;
		}
	}
	return fmt::to_string(str);
}


int CAgilityBookTreeDataRun::OnNeedIcon() const
{
	return m_pTree->GetImageList().IndexRun();
}


void CAgilityBookTreeDataRun::Properties()
{
	if (GetRun())
		m_pTree->GetDocument()->EditRun(GetDog(), GetTrial(), GetRun());
}


bool CAgilityBookTreeDataRun::DoCopy()
{
	bool bCopied = false;
	if (GetRun())
	{
		CClipboardDataWriter clpData;
		if (clpData.isOkay())
		{
			// We need the trial or runs in multi-club trials don't copy correctly.
			ARBDogTrial const* pTrial = nullptr;
			ARBDogTrialPtr trial = GetTrial();
			if (trial && trial->GetClubs().size() > 1)
				pTrial = trial.get();
			wxBusyCursor wait;
			ElementNodePtr tree(ElementNode::New(CLIPDATA));
			GetRun()->Save(
				tree,
				pTrial,
				m_pTree->GetDocument()->Book().GetConfig()); // copy/paste: title points don't matter
			clpData.AddData(ARBClipFormat::Run, tree);
			clpData.AddData(m_pTree->GetPrintLine(GetId()));
			clpData.CommitData();
			bCopied = true;
		}
	}
	return bCopied;
}


bool CAgilityBookTreeDataRun::DoDuplicate()
{
	if (!GetRun())
		return false;
	CAgilityBookDoc* pDoc = m_pTree->GetDocument();
	ARBDogRunPtr pRun = GetRun()->Clone();
	GetTrial()->GetRuns().AddRun(pRun);
	GetTrial()->GetRuns().sort();
	CUpdateHint hint(UPDATE_TREE_VIEW | UPDATE_RUNS_VIEW | UPDATE_POINTS_VIEW);
	pDoc->UpdateAllViews(nullptr, &hint);
	pDoc->Modify(true);
	return true;
}


bool CAgilityBookTreeDataRun::DoDelete(bool bSilent)
{
	if (!GetRun())
		return false;
	return m_pTree->GetDocument()->DeleteRuns({GetRun()}, bSilent);
}
