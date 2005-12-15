/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @brief implementation of the CAgilityBookViewPoints class
 * @author David Connet
 *
 * Revision History
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-10-18 DRC Remember last selected item when reloading data.
 * @li 2005-10-14 DRC Added a context menu.
 * @li 2005-09-15 DRC Added code to filter multi-Qs by date (forgot it - oops!)
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-05-04 DRC Added subtotaling by division to lifetime points.
 * @li 2005-03-14 DRC Show a summary of lifetime points in the list viewer.
 * @li 2005-01-11 DRC Mark the document dirty when creating a title.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2004-12-03 DRC Show all lifetime points when filtering.
 * @li 2004-10-20 DRC Do not accumulate NA titling runs.
 * @li 2004-08-25 DRC Existing other pts were included in all other points.
 * @li 2004-08-12 DRC Allow creating a new title.
 * @li 2004-06-23 DRC Fixed a problem when getting the Q/NQ ratio when a filter
 *                    was in place. Trials that were hidden also removed any
 *                    runs that might have been applicable to the ratio.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 *                    Fix filtering on co-sanctioned trials.
 * @li 2004-05-27 DRC Lifetime point accumulation did not display the points
 *                    for existing runs.
 * @li 2004-05-20 DRC Add Dogs name and current date to report. Oops, just
 *                    realized the only thing that shouldn't be filtered on
 *                    runs is the Q-status (resets the last 2 changes).
 * @li 2004-05-16 DRC Do filter levels.
 * @li 2004-05-03 DRC Do not filter runs, only venues and titles.
 *                    Added percent qualifying.
 * @li 2004-02-02 DRC Added ExistingPoints accumulation.
 * @li 2004-01-05 DRC Header didn't generate properly if there were no titles.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-12-09 DRC Fixed problem tallying QQs when a 3rd run is present.
 * @li 2003-11-22 DRC Added more dog information into the report.
 * @li 2003-11-21 DRC Enabled copy and select all.
 * @li 2003-10-28 DRC Added '*' to Title if it has been received.
 * @li 2003-10-14 DRC Fixed problem inserting other point lines.
 * @li 2003-10-13 DRC Don't tally runs that have no titling points.
 * @li 2003-08-28 DRC Completed Other Points tallying
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 * @li 2003-06-11 DRC Accumulate points based on level, not sublevel, name.
 */

#include "stdafx.h"
#include <map>
#include <set>
#include <vector>
#include "AgilityBook.h"
#include "AgilityBookViewPoints.h"
#if _MSC_VER < 1300
#include "htmlhelp.h"
#endif

#include <afxpriv.h> // wm_commandhelp
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTreeData.h"
#include "ARBDogClub.h"
#include "ARBDogTrial.h"
#include "ARBTypes.h"
#include "MainFrm.h"
#include "PointsData.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Columns:
// 1: Venue
// 2: Reg-Number
// 1: <blank>
// 2: Date
// 3: Title
// 1: <blank>
// 2: Division
// 3: Level
// 4: Event
// 5: n Runs, n Judges, n Partners
// 6: n Qs (x %), (n clean), n Judges, n Partners
// 7: Points
// 8: SuperQs/SpeedPts(per run), total MQs/speedpts
// 9: SpeedPts (if SuperQs too) [possible, but an event like this doesn't exist]
//
// 1: Other Points
// 2: Name
// 3: Points total
#define MAX_COLUMNS		9

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewPoints

IMPLEMENT_DYNCREATE(CAgilityBookViewPoints, CListView2)

BEGIN_MESSAGE_MAP(CAgilityBookViewPoints, CListView2)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CAgilityBookViewPoints)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnLvnDeleteitem)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnLvnGetdispinfo)
	ON_UPDATE_COMMAND_UI(ID_DETAILS, OnUpdateDetails)
	ON_COMMAND(ID_DETAILS, OnDetails)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_NEW_TITLE, OnUpdateAgilityNewTitle)
	ON_COMMAND(ID_AGILITY_NEW_TITLE, OnAgilityNewTitle)
	ON_COMMAND(ID_VIEW_HIDDEN, OnViewHiddenTitles)
	ON_UPDATE_COMMAND_UI(ID_COPY_TITLES_LIST, OnUpdateCopyTitles)
	ON_COMMAND(ID_COPY_TITLES_LIST, OnCopyTitles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CAgilityBookViewPoints construction/destruction

CAgilityBookViewPoints::CAgilityBookViewPoints()
{
}

CAgilityBookViewPoints::~CAgilityBookViewPoints()
{
}

BOOL CAgilityBookViewPoints::PreCreateWindow(CREATESTRUCT& cs)
{
	// This is actually set in TabView.cpp.
	cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER;
	return CListView2::PreCreateWindow(cs);
}

LRESULT CAgilityBookViewPoints::OnCommandHelp(WPARAM, LPARAM)
{
	AfxGetApp()->WinHelp(HID_BASE_RESOURCE+IDR_POINTS, HH_HELP_CONTEXT);
	return 1;
}

int CAgilityBookViewPoints::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView2::OnCreate(lpCreateStruct) == -1)
		return -1;
	GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	LV_COLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM;
	for (int i = 0; i < MAX_COLUMNS; ++i)
	{
		col.fmt = LVCFMT_LEFT;
		if (1 == i)
			col.pszText = _T("Titling Points");
		else
			col.pszText = _T("");
		col.iSubItem = i;
		InsertColumn(i, &col);
	}

	return 0;
}

void CAgilityBookViewPoints::OnRclick(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	// Send WM_CONTEXTMENU to self (done according to Q222905)
	SendMessage(WM_CONTEXTMENU, reinterpret_cast<WPARAM>(m_hWnd), GetMessagePos());
	*pResult = 1;
}

void CAgilityBookViewPoints::OnContextMenu(
		CWnd* pWnd,
		CPoint point)
{
	// Point is (-1,-1) on the context menu button.
	if (0 > point.x || 0 > point.y)
	{
		// Adjust the menu position so it's on the selected item.
		CRect rect;
		int index = GetSelection();
		if (0 <= index)
			GetListCtrl().GetItemRect(index, &rect, FALSE);
		else
			GetListCtrl().GetClientRect(&rect);
		point.x = rect.left + rect.Width() / 3;
		point.y = rect.top + rect.Height() / 2;
		ClientToScreen(&point);
	}
	UINT idMenu = IDR_POINTS;
	if (0 != idMenu)
	{
		CMenu menu;
		menu.LoadMenu(idMenu);
		CMenu* pMenu = menu.GetSubMenu(0);
		ASSERT(pMenu != NULL);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	}
}

void CAgilityBookViewPoints::OnLvnDeleteitem(
		NMHDR *pNMHDR,
		LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	PointsDataBase *pData = reinterpret_cast<PointsDataBase*>(pNMLV->lParam);
	if (pData)
		pData->Release();
	pNMLV->lParam = 0;
	*pResult = 0;
}

void CAgilityBookViewPoints::OnNMDblclk(
		NMHDR *pNMHDR,
		LRESULT *pResult)
{
	PointsDataBase* pData = GetItemData(GetSelection(true));
	if (pData)
		pData->Details();
	else
		MessageBeep(0);
	*pResult = 0;
}

void CAgilityBookViewPoints::OnKeydown(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	LV_KEYDOWN* pLVKeyDown = reinterpret_cast<LV_KEYDOWN*>(pNMHDR);
	switch (pLVKeyDown->wVKey)
	{
	default:
		break;
	case VK_SPACE:
	case VK_RETURN:
		{
			PointsDataBase* pData = GetItemData(GetSelection(true));
			if (pData)
				pData->Details();
			else
				MessageBeep(0);
		}
		break;
	}
	*pResult = 0;
}

void CAgilityBookViewPoints::OnLvnGetdispinfo(
		NMHDR *pNMHDR,
		LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		PointsDataBase *pData = reinterpret_cast<PointsDataBase*>(pDispInfo->item.lParam);
		if (pData)
		{
			ARBString str = pData->OnNeedText(pDispInfo->item.iSubItem);
			::lstrcpyn(pDispInfo->item.pszText, str.c_str(), pDispInfo->item.cchTextMax);
			pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
		}
		else
			pDispInfo->item.pszText[0] = '\0';
	}
	*pResult = 0;
}

void CAgilityBookViewPoints::OnActivateView(
		BOOL bActivate,
		CView* pActivateView,
		CView* pDeactiveView) 
{
	CListView2::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if (pActivateView)
	{
		CString msg;
		if (GetMessage(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText(msg, IsFiltered());
		if (GetMessage2(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText2(msg);
	}
}

void CAgilityBookViewPoints::OnUpdate(
		CView* pSender,
		LPARAM lHint,
		CObject* pHint)
{
	if (0 == lHint
	|| (UPDATE_POINTS_VIEW & lHint)
	|| (UPDATE_OPTIONS & lHint)
	|| (UPDATE_CONFIG & lHint))
		LoadData();
}

#ifdef _DEBUG
// CAgilityBookViewPoints diagnostics
void CAgilityBookViewPoints::AssertValid() const
{
	CListView2::AssertValid();
}

void CAgilityBookViewPoints::Dump(CDumpContext& dc) const
{
	CListView2::Dump(dc);
}

CAgilityBookDoc* CAgilityBookViewPoints::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAgilityBookDoc)));
	return reinterpret_cast<CAgilityBookDoc*>(m_pDocument);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Printing

void CAgilityBookViewPoints::GetPrintLine(
		int nItem,
		CStringArray& line)
{
	CListView2::GetPrintLine(nItem, line);
}

/////////////////////////////////////////////////////////////////////////////

bool CAgilityBookViewPoints::IsFiltered() const
{
	return CAgilityBookOptions::IsFilterEnabled();
}

bool CAgilityBookViewPoints::GetMessage(CString& msg) const
{
	msg.LoadString(IDS_INDICATOR_BLANK);
	return true;
}

bool CAgilityBookViewPoints::GetMessage2(CString& msg) const
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

PointsDataBase* CAgilityBookViewPoints::GetItemData(int index) const
{
	PointsDataBase *pData = NULL;
	if (0 <= index && index < GetListCtrl().GetItemCount())
		pData = reinterpret_cast<PointsDataBase*>(GetListCtrl().GetItemData(index));
	return pData;
}

size_t CAgilityBookViewPoints::FindMatchingRuns(
		std::list<RunInfo> const& runs,
		ARBString const& div,
		ARBString const& level,
		ARBString const& event,
		std::list<RunInfo>& matching)
{
	matching.clear();
	for (list<RunInfo>::const_iterator iterRun = runs.begin();
		iterRun != runs.end();
		++iterRun)
	{
		ARBDogRun const* pRun = iterRun->second;
		if (pRun->GetDivision() == div && pRun->GetLevel() == level && pRun->GetEvent() == event)
			matching.push_back(*iterRun);
	}
	return matching.size();
}

double CAgilityBookViewPoints::TallyPoints(
		std::list<RunInfo> const& runs,
		ARBConfigScoring const* pScoringMethod,
		int& nCleanQ,
		int& nNotCleanQ)
{
	nCleanQ = 0;
	nNotCleanQ = 0;
	double score = 0.0;
	for (list<RunInfo>::const_iterator iterRun = runs.begin();
		iterRun != runs.end();
		++iterRun)
	{
		ARBDogRun const* pRun = iterRun->second;
		if (pRun->GetQ().Qualified())
		{
			bool bClean = false;
			score += pRun->GetTitlePoints(pScoringMethod, &bClean);
			if (bClean)
				++nCleanQ;
			else
				++nNotCleanQ;
		}
	}
	return score;
}

void CAgilityBookViewPoints::InsertData(int& ioIndex, PointsDataBase* inData)
{
	if (inData)
	{
		LVITEM item;
		item.iItem = ioIndex;
		item.iSubItem = 0;
		item.mask = LVIF_TEXT | LVIF_PARAM;
		item.pszText = LPSTR_TEXTCALLBACK;
		item.lParam = reinterpret_cast<LPARAM>(inData);
		GetListCtrl().InsertItem(&item);
		++ioIndex;
	}
}

void CAgilityBookViewPoints::LoadData()
{
	CWaitCursor wait;

	// Reduce flicker
	GetListCtrl().SetRedraw(FALSE);

	// Get the current item.
	PointsDataBase* pCurData = GetItemData(GetSelection(true));
	if (pCurData)
		pCurData->AddRef();

	// Clear everything.
	GetListCtrl().DeleteAllItems();

	// Find all visible items and sort them out by venue.
	ARBDog* pDog = GetDocument()->GetCurrentDog();
	if (pDog)
	{
		std::vector<CVenueFilter> venues;
		CAgilityBookOptions::GetFilterVenue(venues);
		int idxInsertItem = 0;

		// Put general info about the dog in...
		InsertData(idxInsertItem, new PointsDataDog(this, pDog));

		// For each venue...
		for (ARBConfigVenueList::const_iterator iterVenue = GetDocument()->GetConfig().GetVenues().begin();
			iterVenue != GetDocument()->GetConfig().GetVenues().end();
			++iterVenue)
		{
			ARBConfigVenue* pVenue = (*iterVenue);
			if (!CAgilityBookOptions::IsVenueVisible(venues, pVenue->GetName()))
				continue;

			// First, titles.
			bool bHeaderInserted = false;
			for (ARBDogTitleList::const_iterator iterTitle = pDog->GetTitles().begin();
				iterTitle != pDog->GetTitles().end();
				++iterTitle)
			{
				ARBDogTitle* pTitle = (*iterTitle);
				if (pTitle->GetVenue() == pVenue->GetName()
				&& !pTitle->IsFiltered())
				{
					if (!bHeaderInserted)
					{
						bHeaderInserted = true;
						GetListCtrl().InsertItem(idxInsertItem++, _T(""));
						InsertData(idxInsertItem, new PointsDataVenue(this, pDog, pVenue));
					}
					InsertData(idxInsertItem, new PointsDataTitle(this, pDog, pTitle));
				}
			}

			LifeTimePointsList lifetime;

			// Then the runs.
			list<ARBDogTrial const*> trialsInVenue;
			for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
				iterTrial != pDog->GetTrials().end();
				++iterTrial)
			{
				ARBDogTrial const* pTrial = (*iterTrial);
				// Don't bother subtracting "hidden" trials. Doing so
				// will skew the qualifying percentage.
				if (pTrial->HasVenue(pVenue->GetName()))
					trialsInVenue.push_back(pTrial);
			}
			if (pDog->GetExistingPoints().HasPoints(pVenue->GetName())
			|| 0 < trialsInVenue.size())
			{
				// Ok, we have some trials in the venue to process...
				if (!bHeaderInserted)
				{
					bHeaderInserted = true;
					GetListCtrl().InsertItem(idxInsertItem++, _T(""));
					InsertData(idxInsertItem, new PointsDataVenue(this, pDog, pVenue));
				}
				int speedPts = 0;
				bool bHasSpeedPts = false;
				// Show events sorted out by division/level.
				for (ARBConfigDivisionList::const_iterator iterDiv = pVenue->GetDivisions().begin();
					iterDiv != pVenue->GetDivisions().end();
					++iterDiv)
				{
					ARBConfigDivision* pDiv = (*iterDiv);
					for (ARBConfigLevelList::const_iterator iterLevel = pDiv->GetLevels().begin();
						iterLevel != pDiv->GetLevels().end();
						++iterLevel)
					{
						ARBConfigLevel* pLevel = (*iterLevel);
						LifeTimePoints pts;
						pts.pDiv = pDiv;
						pts.pLevel = pLevel;
						// We know the venue is visible,
						// we don't know if the trial or individual runs are.
						for (ARBConfigEventList::const_iterator iterEvent = pVenue->GetEvents().begin();
							iterEvent != pVenue->GetEvents().end();
							++iterEvent)
						{
							ARBConfigEvent* pEvent = (*iterEvent);
							bool bHasExistingPoints = pDog->GetExistingPoints().HasPoints(pVenue, pDiv, pLevel, pEvent, false);
							bool bHasExistingLifetimePoints = pDog->GetExistingPoints().HasPoints(pVenue, pDiv, pLevel, pEvent, true);

							// Don't tally runs that have no titling points.
							ARBVector<ARBConfigScoring> scoringItems;
							if (0 == pEvent->FindAllEvents(pDiv->GetName(), pLevel->GetName(), ARBDate(), true, scoringItems))
								continue;
							// Iterate across each scoring method separately. This means it is
							// possible to have multiple lines show up for a given event. But if
							// that happens, it means the events were scored differently.
							for (ARBVector<ARBConfigScoring>::iterator iterScoring = scoringItems.begin();
								iterScoring != scoringItems.end();
								++iterScoring)
							{
								ARBConfigScoring* pScoringMethod = *iterScoring;
								int SQs = 0;
								int speedPtsEvent = 0;
								list<RunInfo> matching;
								set<ARBString> judges;
								set<ARBString> judgesQ;
								set<ARBString> partners;
								set<ARBString> partnersQ;
								for (list<ARBDogTrial const*>::const_iterator iterTrial = trialsInVenue.begin();
									iterTrial != trialsInVenue.end();
									++iterTrial)
								{
									ARBDogTrial const* pTrial = (*iterTrial);
									for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
										iterRun != pTrial->GetRuns().end();
										++iterRun)
									{
										ARBDogRun const* pRun = (*iterRun);
										if (pRun->GetDivision() != pDiv->GetName()
										|| (pRun->GetLevel() != pLevel->GetName() && !pLevel->GetSubLevels().FindSubLevel(pRun->GetLevel()))
										|| pRun->GetEvent() != pEvent->GetName())
											continue;
										ARBConfigScoring* pScoring;
										pEvent->FindEvent(pDiv->GetName(), pLevel->GetName(), pRun->GetDate(), &pScoring);
										ASSERT(pScoring);
										if (!pScoring) continue; // Shouldn't need it...
										if (*pScoring != *pScoringMethod)
										{
											pScoring->Release();
											continue;
										}
										pScoring->Release();
										bool bRunVisible = (!pRun->IsFiltered(ARBBase::eIgnoreQ)
										&& CAgilityBookOptions::IsRunVisible(venues, pVenue, pTrial, pRun));
										if (bRunVisible)
										{
											// Don't tally NA runs for titling events.
											if (pRun->GetQ() == ARB_Q::eQ_NA)
												continue;
											matching.push_back(RunInfo(pTrial, pRun));
											judges.insert(pRun->GetJudge());
											if (pRun->GetQ().Qualified())
												judgesQ.insert(pRun->GetJudge());
											if (pScoringMethod->HasSuperQ() && ARB_Q::eQ_SuperQ == pRun->GetQ())
												++SQs;
											if (pScoringMethod->HasSpeedPts())
											{
												int pts = pRun->GetSpeedPoints(pScoringMethod);
												speedPts += pts;
												speedPtsEvent += pts;
											}
											// Only tally partners for pairs. In USDAA DAM, pairs is
											// actually a 3-dog relay.
											if (pEvent->HasPartner() && 1 == pRun->GetPartners().size())
											{
												for (ARBDogRunPartnerList::const_iterator iterPartner = pRun->GetPartners().begin();
													iterPartner != pRun->GetPartners().end();
													++iterPartner)
												{
													ARBString p = (*iterPartner)->GetDog();
													p += (*iterPartner)->GetRegNum();
													partners.insert(p);
													if (pRun->GetQ().Qualified())
														partnersQ.insert(p);
												}
											}
										}
										// Tally lifetime points, regardless of visibility.
										if (0 < pScoringMethod->GetLifetimePoints().size()
										&& pRun->GetQ().Qualified())
										{
											double nLifetime;
											pRun->GetTitlePoints(pScoringMethod, NULL, &nLifetime);
											if (0 < nLifetime)
											{
												pts.ptList.push_back(LifeTimePoint(pRun->GetEvent(), nLifetime, !bRunVisible));
											}
										}
									}
								}
								double nExistingPts = 0;
								int nExistingSQ = 0;
								// Accumulate existing points - used for both lifetime and
								// normal runs.
								if (bHasExistingPoints || bHasExistingLifetimePoints || 0 < matching.size())
								{
									nExistingPts = pDog->GetExistingPoints().ExistingPoints(
										ARBDogExistingPoints::eRuns,
										pVenue, NULL, pDiv, pLevel, pEvent);
									if (pScoringMethod->HasSuperQ())
										nExistingSQ += static_cast<int>(pDog->GetExistingPoints().ExistingPoints(
											ARBDogExistingPoints::eSQ,
											pVenue, NULL, pDiv, pLevel, pEvent));
								}
								// Now add the existing lifetime points
								if (bHasExistingLifetimePoints && 0.0 < nExistingPts + nExistingSQ)
								{
									pts.ptList.push_back(LifeTimePoint(pEvent->GetName(), nExistingPts + nExistingSQ, false));
								}
								// Now we deal with the visible runs.
								if (bHasExistingPoints || 0 < matching.size())
								{
									int nCleanQ, nNotCleanQ;
									double pts = TallyPoints(matching, pScoringMethod, nCleanQ, nNotCleanQ);
									pts += nExistingPts;
									CString strRunCount;
									strRunCount.FormatMessage(IDS_POINTS_RUNS_JUDGES,
										matching.size(),
										judges.size());
									if (pEvent->HasPartner() && 0 < partners.size())
									{
										CString str2;
										str2.FormatMessage(IDS_POINTS_PARTNERS, partners.size());
										strRunCount += str2;
									}
									double percentQs = (static_cast<double>(nCleanQ + nNotCleanQ) / static_cast<double>(matching.size())) * 100;
									CString strQcount;
									strQcount.FormatMessage(IDS_POINTS_QS,
										nCleanQ + nNotCleanQ,
										static_cast<int>(percentQs));
									if (0 < nCleanQ)
									{
										CString str2;
										str2.FormatMessage(IDS_POINTS_CLEAN, nCleanQ);
										strQcount += str2;
									}
									if (0 < judgesQ.size())
									{
										CString str2;
										str2.FormatMessage(IDS_POINTS_JUDGES, judgesQ.size());
										strQcount += str2;
									}
									if (pEvent->HasPartner() && 0 < partnersQ.size())
									{
										CString str2;
										str2.FormatMessage(IDS_POINTS_PARTNERS, partnersQ.size());
										strQcount += str2;
									}
									ARBostringstream strPts;
									CString strSuperQ;
									strPts << pts + nExistingSQ;
									if (pScoringMethod->HasSuperQ())
									{
										SQs += nExistingSQ;
										strSuperQ.FormatMessage(IDS_POINTS_SQS, SQs);
									}
									CString strSpeed;
									if (pScoringMethod->HasSpeedPts())
									{
										bHasSpeedPts = true;
										if (0 < speedPtsEvent)
										{
											strSpeed.FormatMessage(IDS_POINTS_SPEED_SUBTOTAL, speedPtsEvent);
										}
									}
									InsertData(idxInsertItem, new PointsDataEvent(this,
										0 < nExistingPts + nExistingSQ ? pDog : NULL,
										matching,
										pVenue, pDiv, pLevel, pEvent,
										(LPCTSTR)strRunCount,
										(LPCTSTR)strQcount,
										strPts.str().c_str(),
										(LPCTSTR)strSuperQ,
										(LPCTSTR)strSpeed));
								}
							}
						}
						if (bHasSpeedPts)
						{
							speedPts += static_cast<int>(pDog->GetExistingPoints().ExistingPoints(
								ARBDogExistingPoints::eSpeed,
								pVenue, NULL, pDiv, pLevel, NULL));
						}
						if (0 < pts.ptList.size())
							lifetime.push_back(pts);
					}
				}

				// Information that is tallied after all a venue's events.
				if (bHasSpeedPts)
				{
					InsertData(idxInsertItem, new PointsDataSpeedPts(this, pVenue, speedPts));
				}

				// If the venue has multiQs, tally them now.
				if (0 < pVenue->GetMultiQs().size())
				{
					std::map<ARBConfigMultiQ*, std::set<MultiQdata> > MQs;
					for (list<ARBDogTrial const*>::const_iterator iterTrial = trialsInVenue.begin();
						iterTrial != trialsInVenue.end();
						++iterTrial)
					{
						ARBDogTrial const* pTrial = (*iterTrial);
						for (ARBDogRunList::const_iterator iterR = pTrial->GetRuns().begin();
							iterR != pTrial->GetRuns().end();
							++iterR)
						{
							ARBDogRun const* pRun = *iterR;
							if (pRun->GetMultiQ()
							&& !pRun->IsFiltered(ARBBase::eIgnoreQ)
							&& CAgilityBookOptions::IsRunVisible(venues, pVenue, pTrial, pRun))
							{
								MQs[pRun->GetMultiQ()].insert(MultiQdata(pRun->GetDate(), pTrial));
							}
						}
					}
					for (std::map<ARBConfigMultiQ*, std::set<MultiQdata> >::iterator iter = MQs.begin();
						iter != MQs.end();
						++iter)
					{
						InsertData(idxInsertItem,
							new PointsDataMultiQs(this,
								pDog, pVenue, (*iter).first, (*iter).second));
					}
				}
			}
			// Next comes lifetime points.
			if (0 < lifetime.size())
			{
				PointsDataLifetime* pData = new PointsDataLifetime(this, pVenue->GetName());
				double pts = 0;
				double ptFiltered = 0;
				typedef std::map<ARBString, PointsDataLifetimeDiv*> DivLifetime;
				DivLifetime divs;
				for (LifeTimePointsList::iterator iter = lifetime.begin();
					iter != lifetime.end();
					++iter)
				{
					PointsDataLifetimeDiv* pDivData = NULL;
					DivLifetime::iterator it = divs.find(iter->pDiv->GetName());
					if (divs.end() != it)
					{
						pDivData = it->second;
					}
					else
					{
						pDivData = new PointsDataLifetimeDiv(this, pVenue->GetName(), iter->pDiv->GetName());
						divs.insert(DivLifetime::value_type(iter->pDiv->GetName(), pDivData));
					}

					double pts2 = 0.0;
					double ptFiltered2 = 0;
					for (LifeTimePointList::iterator iter2 = (*iter).ptList.begin();
						iter2 != (*iter).ptList.end();
						++iter2)
					{
						pts2 += (*iter2).points;
						if ((*iter2).bFiltered)
							ptFiltered2 += (*iter2).points;
					}

					pData->AddLifetimeInfo(iter->pDiv->GetName(), iter->pLevel->GetName(), pts2, ptFiltered2);
					pDivData->AddLifetimeInfo(iter->pDiv->GetName(), iter->pLevel->GetName(), pts2, ptFiltered2);
					pts += pts2;
					ptFiltered += ptFiltered2;
				}
				InsertData(idxInsertItem, pData);
				if (1 < divs.size())
				{
					for (DivLifetime::iterator it = divs.begin();
						it != divs.end();
						++it)
					{
						InsertData(idxInsertItem, it->second);
					}
				}
				else if (1 == divs.size())
					divs.begin()->second->Release();
			}
		}

		// After all the venues, we do 'other points'.
		ARBConfigOtherPointsList const& other = GetDocument()->GetConfig().GetOtherPoints();
		if (0 < other.size())
		{
			CString str;
			GetListCtrl().InsertItem(idxInsertItem++, _T(""));
			str.LoadString(IDS_OTHERPOINTS);
			GetListCtrl().InsertItem(idxInsertItem++, str);
			for (ARBConfigOtherPointsList::const_iterator iterOther = other.begin();
				iterOther != other.end();
				++iterOther)
			{
				// First, just generate a list of runs with the needed info.
				std::list<OtherPtInfo> runs;

				ARBConfigOtherPoints* pOther = (*iterOther);
				for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
					iterTrial != pDog->GetTrials().end();
					++iterTrial)
				{
					ARBDogTrial const* pTrial = (*iterTrial);
					if (!pTrial->IsFiltered())
					{
						for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
							iterRun != pTrial->GetRuns().end();
							++iterRun)
						{
							ARBDogRun const* pRun = (*iterRun);
							if (!pRun->IsFiltered(ARBBase::eIgnoreQ))
							{
								for (ARBDogRunOtherPointsList::const_iterator iterOtherPts = pRun->GetOtherPoints().begin();
									iterOtherPts != pRun->GetOtherPoints().end();
									++iterOtherPts)
								{
									ARBDogRunOtherPoints const* pOtherPts = (*iterOtherPts);
									if (pOtherPts->GetName() == pOther->GetName())
									{
										runs.push_back(OtherPtInfo(pTrial, pRun, pOtherPts->GetPoints()));
									}
								}
							}
						}
					}
				}

				for (ARBDogExistingPointsList::const_iterator iterExisting = pDog->GetExistingPoints().begin();
					iterExisting != pDog->GetExistingPoints().end();
					++iterExisting)
				{
					if (ARBDogExistingPoints::eOtherPoints == (*iterExisting)->GetType()
					&& (*iterExisting)->GetOtherPoints() == pOther->GetName())
					{
						runs.push_back(OtherPtInfo(*iterExisting));
					}
				}

				if (0 == runs.size())
					continue;

				switch (pOther->GetTally())
				{
				default:
				case ARBConfigOtherPoints::eTallyAll:
					InsertData(idxInsertItem, new PointsDataOtherPointsTallyAll(this, pOther->GetName(), runs));
					break;

				case ARBConfigOtherPoints::eTallyAllByEvent:
					GetListCtrl().InsertItem(idxInsertItem, _T(""));
					GetListCtrl().SetItemText(idxInsertItem++, 1, pOther->GetName().c_str());
					{
						std::set<ARBString> tally;
						std::list<OtherPtInfo>::iterator iter;
						for (iter = runs.begin(); iter != runs.end(); ++iter)
						{
							tally.insert((*iter).m_Event);
						}
						for (std::set<ARBString>::iterator iterTally = tally.begin();
							iterTally != tally.end();
							++iterTally)
						{
							std::list<OtherPtInfo> validRuns;
							for (iter = runs.begin(); iter != runs.end(); ++iter)
							{
								if ((*iter).m_Event == (*iterTally))
									validRuns.push_back(*iter);
							}
							InsertData(idxInsertItem, new PointsDataOtherPointsTallyAllByEvent(this, (*iterTally), validRuns));
						}
					}
					break;

				case ARBConfigOtherPoints::eTallyLevel:
					GetListCtrl().InsertItem(idxInsertItem, _T(""));
					GetListCtrl().SetItemText(idxInsertItem++, 1, pOther->GetName().c_str());
					{
						std::set<ARBString> tally;
						std::list<OtherPtInfo>::iterator iter;
						for (iter = runs.begin(); iter != runs.end(); ++iter)
						{
							tally.insert((*iter).m_Level);
						}
						for (std::set<ARBString>::iterator iterTally = tally.begin();
							iterTally != tally.end();
							++iterTally)
						{
							std::list<OtherPtInfo> validRuns;
							for (iter = runs.begin(); iter != runs.end(); ++iter)
							{
								if ((*iter).m_Level == (*iterTally))
									validRuns.push_back(*iter);
							}
							InsertData(idxInsertItem, new PointsDataOtherPointsTallyLevel(this, (*iterTally), validRuns));
						}
					}
					break;

				case ARBConfigOtherPoints::eTallyLevelByEvent:
					GetListCtrl().InsertItem(idxInsertItem, _T(""));
					GetListCtrl().SetItemText(idxInsertItem++, 1, pOther->GetName().c_str());
					{
						typedef std::pair<ARBString, ARBString> LevelEvent;
						std::set<LevelEvent> tally;
						std::list<OtherPtInfo>::iterator iter;
						for (iter = runs.begin(); iter != runs.end(); ++iter)
						{
							tally.insert(LevelEvent((*iter).m_Level, (*iter).m_Event));
						}
						for (std::set<LevelEvent>::iterator iterTally = tally.begin();
							iterTally != tally.end();
							++iterTally)
						{
							std::list<OtherPtInfo> validRuns;
							for (iter = runs.begin(); iter != runs.end(); ++iter)
							{
								if ((*iter).m_Level == (*iterTally).first
								&& (*iter).m_Event == (*iterTally).second)
									validRuns.push_back(*iter);
							}
							InsertData(idxInsertItem, new PointsDataOtherPointsTallyLevelByEvent(this, (*iterTally).first, (*iterTally).second, validRuns));
						}
					}
					break;
				}
			}
		}
	}

	int nColumnCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumnCount; ++i)
		GetListCtrl().SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);

	CString msg;
	if (IsWindowVisible())
	{
		if (GetMessage(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText(msg, IsFiltered());
		if (GetMessage2(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText2(msg);
	}

	if (pCurData)
	{
		int n = GetListCtrl().GetItemCount();
		for (int i = 0; i < n; ++i)
		{
			PointsDataBase* pBase = GetItemData(i);
			if (pBase)
			{
				if (pBase->IsEqual(pCurData))
				{
					SetSelection(i);
					GetListCtrl().EnsureVisible(i, FALSE);
					break;
				}
			}
		}
		pCurData->Release();
	}

	GetListCtrl().SetRedraw(TRUE);
	GetListCtrl().Invalidate();
}

// CAgilityBookViewPoints message handlers

void CAgilityBookViewPoints::OnUpdateDetails(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	PointsDataBase* pData = GetItemData(GetSelection(true));
	if (pData && pData->HasDetails())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewPoints::OnDetails()
{
	PointsDataBase* pData = GetItemData(GetSelection(true));
	if (pData)
		pData->Details();
}

void CAgilityBookViewPoints::OnUpdateAgilityNewTitle(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	ARBDog* pDog = GetDocument()->GetCurrentDog();
	if (pDog)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewPoints::OnAgilityNewTitle()
{
	ARBDog* pDog = GetDocument()->GetCurrentDog();
	if (pDog)
	{
		// Convenience! No duplicated code!
		CAgilityBookTreeDataDog data(GetDocument()->GetTreeView(), pDog);
		if (data.OnCmd(ID_AGILITY_NEW_TITLE, NULL))
			GetDocument()->SetModifiedFlag();
	}
}

void CAgilityBookViewPoints::OnViewHiddenTitles()
{
	CAgilityBookOptions::SetViewHiddenTitles(!CAgilityBookOptions::GetViewHiddenTitles());
	std::vector<CVenueFilter> venues;
	CAgilityBookOptions::GetFilterVenue(venues);
	for (ARBDogList::iterator iterDogs = GetDocument()->GetDogs().begin(); iterDogs != GetDocument()->GetDogs().end(); ++iterDogs)
		for (ARBDogTitleList::iterator iterTitle = (*iterDogs)->GetTitles().begin(); iterTitle != (*iterDogs)->GetTitles().end(); ++iterTitle)
			GetDocument()->ResetVisibility(venues, *iterTitle);
	LoadData();
}

void CAgilityBookViewPoints::OnUpdateCopyTitles(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	ARBDog* pDog = GetDocument()->GetCurrentDog();
	if (pDog && 0 < pDog->GetTitles().size())
	{
		int count = 0;
		for (ARBDogTitleList::const_iterator iter = pDog->GetTitles().begin();
			iter != pDog->GetTitles().end();
			++iter)
		{
			if ((*iter)->GetDate().IsValid()
			&& !(*iter)->IsHidden())
				++count;
		}
		if (0 < count)
			bEnable = TRUE;
	}
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewPoints::OnCopyTitles()
{
	ARBDog* pDog = GetDocument()->GetCurrentDog();
	if (pDog && 0 < pDog->GetTitles().size())
	{
		std::vector<CVenueFilter> venues;
		CAgilityBookOptions::GetFilterVenue(venues);

		ARBString preTitles, postTitles;
		for (ARBConfigVenueList::const_iterator iVenue = GetDocument()->GetConfig().GetVenues().begin();
			iVenue != GetDocument()->GetConfig().GetVenues().end();
			++iVenue)
		{
			if (!CAgilityBookOptions::IsVenueVisible(venues, (*iVenue)->GetName()))
				continue;
			ARBString preTitles2, postTitles2;
			for (ARBConfigTitleList::const_iterator iTitle = (*iVenue)->GetTitles().begin();
				iTitle != (*iVenue)->GetTitles().end();
				++iTitle)
			{
				ARBDogTitle* pTitle;
				if (pDog->GetTitles().FindTitle((*iVenue)->GetName(), (*iTitle)->GetName(), &pTitle))
				{
					if (pTitle->GetDate().IsValid()
					&& !pTitle->IsHidden())
					{
						if ((*iTitle)->GetPrefix())
						{
							if (!preTitles2.empty())
								preTitles2 += ' ';
							preTitles2 += (*iTitle)->GetName();
						}
						else
						{
							if (!postTitles2.empty())
								postTitles2 += ' ';
							postTitles2 += (*iTitle)->GetName();
						}
					}
					pTitle->Release();
				}
			}
			if (!preTitles2.empty())
			{
				if (!preTitles.empty())
					preTitles += ' ';
				preTitles += preTitles2;
			}
			if (!postTitles2.empty())
			{
				if (!postTitles.empty())
					postTitles += _T("; ");
				postTitles += postTitles2;
			}
		}
		if (!preTitles.empty() || !postTitles.empty())
		{
			CString data(preTitles.c_str());
			data += ' ';
			data += pDog->GetCallName().c_str();
			data += _T(": ");
			data += postTitles.c_str();

			// Now, copy to the clipboard
			if (AfxGetMainWnd()->OpenClipboard())
			{
				EmptyClipboard();

				// alloc mem block & copy text in
				HGLOBAL temp = GlobalAlloc(GHND, data.GetLength()+1);
				if (NULL != temp)
				{
					LPTSTR str = reinterpret_cast<LPTSTR>(GlobalLock(temp));
					lstrcpy(str, (LPCTSTR)data);
					GlobalUnlock(temp);
					// send data to clipbard
					SetClipboardData(CF_TEXT, temp);
				}

				CloseClipboard();
			}
		}
		else
			AfxMessageBox(_T("No titles to copy."), MB_ICONINFORMATION);
	}
}
