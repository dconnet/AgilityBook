/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
#include "AgilityBook.h"
#include "AgilityBookViewPoints.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBDogClub.h"
#include "ARBDogTrial.h"
#include "MainFrm.h"

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
// 8: SuperQs/MachPts(per run), total QQs/machpts
// 9: MachPts (if SuperQs too) [possible, but an event like this doesn't exist]
//
// 1: Other Points
// 2: Name
// 3: Points total
#define MAX_COLUMNS		9

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewPoints

IMPLEMENT_DYNCREATE(CAgilityBookViewPoints, CListView2)

BEGIN_MESSAGE_MAP(CAgilityBookViewPoints, CListView2)
	//{{AFX_MSG_MAP(CAgilityBookViewPoints)
	ON_WM_CREATE()
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
	cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOCOLUMNHEADER;
	return CListView2::PreCreateWindow(cs);
}

int CAgilityBookViewPoints::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView2::OnCreate(lpCreateStruct) == -1)
		return -1;
	GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	LV_COLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM;
	for (int i = 0; i < MAX_COLUMNS; ++i)
	{
		col.fmt = LVCFMT_LEFT;
		col.pszText = "";
		col.iSubItem = i;
		GetListCtrl().InsertColumn(i, &col);
	}

	return 0;
}

void CAgilityBookViewPoints::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CListView2::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if (pActivateView)
	{
		CString msg;
		if (GetMessage(msg))
			((CMainFrame*)AfxGetMainWnd())->SetStatusText(msg, IsFiltered());
		if (GetMessage2(msg))
			((CMainFrame*)AfxGetMainWnd())->SetStatusText2(msg);
	}
}

void CAgilityBookViewPoints::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
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
	return (CAgilityBookDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Printing

void CAgilityBookViewPoints::GetPrintLine(int nItem, CStringArray& line)
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

// Entering this function, we know the trial is visible.
// We don't know if the individual runs are.
int CAgilityBookViewPoints::DoEvents(
	ARBDog const* inDog,
	std::vector<CVenueFilter> const& venues,
	int index,
	std::list<ARBDogTrial const*> const& trials,
	ARBConfigVenue const* inVenue,
	ARBConfigDivision const* inDiv,
	ARBConfigLevel const* inLevel,
	LifeTimePointList& inLifetime)
{
	int nAdded = 0;
	int machPts = 0;
	bool bHasMachPts = false;
	std::set<ARBDate> QQs;
	bool bHasDoubleQs = false;
	for (ARBConfigEventList::const_iterator iterEvent = inVenue->GetEvents().begin();
		iterEvent != inVenue->GetEvents().end();
		++iterEvent)
	{
		ARBConfigEvent const* pEvent = (*iterEvent);
		bool bHasPoints = inDog->GetExistingPoints().HasPoints(inVenue, inDiv, inLevel, pEvent, false);

		// Don't tally runs that have no titling points.
		std::vector<ARBConfigScoring const*> scoringItems;
		if (0 == pEvent->FindAllEvents(inDiv->GetName(), inLevel->GetName(), true, scoringItems))
			continue;
		// Iterate across each scoring method separately. This means it is
		// possible to have multiple lines show up for a given event. But if
		// that happens, it means the events were scored differently.
		for (std::vector<ARBConfigScoring const*>::iterator iterScoring = scoringItems.begin();
			iterScoring != scoringItems.end();
			++iterScoring)
		{
			ARBConfigScoring const* pScoringMethod = *iterScoring;
			int SQs = 0;
			int machPtsEvent = 0;
			list<ARBDogRun const*> matching;
			set<std::string> judges;
			set<std::string> judgesQ;
			set<std::string> partners;
			set<std::string> partnersQ;
			for (list<ARBDogTrial const*>::const_iterator iterTrial = trials.begin();
				iterTrial != trials.end();
				++iterTrial)
			{
				ARBDogTrial const* pTrial = (*iterTrial);
				if (pScoringMethod->HasDoubleQ())
				{
					for (ARBDate date = pTrial->GetRuns().GetStartDate();
						date <= pTrial->GetRuns().GetEndDate();
						++date)
					{
						if (pTrial->HasQQ(date, GetDocument()->GetConfig(), inDiv->GetName(), inLevel->GetName()))
						{
							int nVisible = 0;
							// But first, make sure all the runs are visible.
							for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
								iterRun != pTrial->GetRuns().end();
								++iterRun)
							{
								ARBDogRun const* pRun = (*iterRun);
								// This extra test only looks at runs that are
								// QQing. Otherwise a 3rd NA run throws things off.
								ARBConfigScoring const* pScoring = GetDocument()->GetConfig().GetVenues().FindEvent(
									pTrial->GetClubs().GetPrimaryClub()->GetVenue(),
									pRun->GetEvent(),
									pRun->GetDivision(),
									pRun->GetLevel(),
									pRun->GetDate());
								if (pScoring && pScoring->HasDoubleQ()
								&& date == pRun->GetDate()
								&& !pRun->IsFiltered(ARBBase::eIgnoreQ)
								&& CAgilityBookOptions::IsRunVisible(venues, inVenue, pTrial, pRun))
								{
									++nVisible;
								}
							}
							if (2 == nVisible)
								QQs.insert(date);
						}
					}
				}
				for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
					iterRun != pTrial->GetRuns().end();
					++iterRun)
				{
					ARBDogRun const* pRun = (*iterRun);
					if (!pRun->IsFiltered(ARBBase::eIgnoreQ)
					&& CAgilityBookOptions::IsRunVisible(venues, inVenue, pTrial, pRun))
					{
						if (pRun->GetDivision() != inDiv->GetName()
						|| (pRun->GetLevel() != inLevel->GetName() && !inLevel->GetSubLevels().FindSubLevel(pRun->GetLevel()))
						|| pRun->GetEvent() != pEvent->GetName())
							continue;
						ARBConfigScoring const* pScoring = pEvent->FindEvent(inDiv->GetName(), inLevel->GetName(), pRun->GetDate());
						ASSERT(pScoring);
						if (!pScoring) continue; // Shouldn't need it...
						if (*pScoring != *pScoringMethod)
							continue;
						matching.push_back(pRun);
						judges.insert(pRun->GetJudge());
						if (pRun->GetQ().Qualified())
							judgesQ.insert(pRun->GetJudge());
						if (pScoringMethod->HasSuperQ() && ARB_Q::eQ_SuperQ == pRun->GetQ())
							++SQs;
						if (pScoringMethod->HasMachPts())
						{
							int pts = pRun->GetMachPoints(pScoringMethod);
							machPts += pts;
							machPtsEvent += pts;
						}
						// Only tally partners for pairs. In USDAA DAM, pairs is
						// actually a 3-dog relay.
						if (pEvent->HasPartner() && 1 == pRun->GetPartners().size())
						{
							for (ARBDogRunPartnerList::const_iterator iterPartner = pRun->GetPartners().begin();
								iterPartner != pRun->GetPartners().end();
								++iterPartner)
							{
								string p = (*iterPartner)->GetDog();
								p += (*iterPartner)->GetRegNum();
								partners.insert(p);
								if (pRun->GetQ().Qualified())
									partnersQ.insert(p);
							}
						}
					}
				}
			}
			CString str, str2;
			if (bHasPoints || 0 < matching.size())
			{
				GetListCtrl().InsertItem(index+nAdded, "");
				int nextCol = 1;
				GetListCtrl().SetItemText(index+nAdded, nextCol++, inDiv->GetName().c_str());
				GetListCtrl().SetItemText(index+nAdded, nextCol++, inLevel->GetName().c_str());
				GetListCtrl().SetItemText(index+nAdded, nextCol++, pEvent->GetName().c_str());
				int nCleanQ, nNotCleanQ;
				int pts = TallyPoints(matching, pScoringMethod, nCleanQ, nNotCleanQ, inLifetime);
				int nExistingPts = inDog->GetExistingPoints().ExistingPoints(
					ARBDogExistingPoints::eRuns,
					inVenue, inDiv, inLevel, pEvent);
				pts += nExistingPts;
				int nExistingSQ = 0;
				if (pScoringMethod->HasSuperQ())
					nExistingSQ += inDog->GetExistingPoints().ExistingPoints(
						ARBDogExistingPoints::eSQ,
						inVenue, inDiv, inLevel, pEvent);
				if (inDog->GetExistingPoints().HasPoints(inVenue, inDiv, inLevel, pEvent, true))
				{
					inLifetime.push_back(LifeTimePoint(pEvent->GetName(), nExistingPts + nExistingSQ));
				}
				str.FormatMessage(IDS_POINTS_RUNS_JUDGES,
					matching.size(),
					judges.size());
				if (pEvent->HasPartner() && 0 < partners.size())
				{
					CString str2;
					str2.FormatMessage(IDS_POINTS_PARTNERS, partners.size());
					str += str2;
				}
				GetListCtrl().SetItemText(index+nAdded, nextCol++, str);
				double percentQs = (static_cast<double>(nCleanQ + nNotCleanQ) / static_cast<double>(matching.size())) * 100;
				str.FormatMessage(IDS_POINTS_QS,
					nCleanQ + nNotCleanQ,
					static_cast<int>(percentQs));
				if (0 < nCleanQ)
				{
					str2.FormatMessage(IDS_POINTS_CLEAN, nCleanQ);
					str += str2;
				}
				if (0 < judgesQ.size())
				{
					str2.FormatMessage(IDS_POINTS_JUDGES, judgesQ.size());
					str += str2;
				}
				if (pEvent->HasPartner() && 0 < partnersQ.size())
				{
					str2.FormatMessage(IDS_POINTS_PARTNERS, partnersQ.size());
					str += str2;
				}
				GetListCtrl().SetItemText(index+nAdded, nextCol++, str);

				str.Format("%d", pts + nExistingSQ);
				GetListCtrl().SetItemText(index+nAdded, nextCol++, str);
				if (pScoringMethod->HasSuperQ())
				{
					SQs += nExistingSQ;
					str.FormatMessage(IDS_POINTS_SQS, SQs);
					GetListCtrl().SetItemText(index+nAdded, nextCol++, str);
				}
				if (pScoringMethod->HasDoubleQ())
					bHasDoubleQs = true;
				if (pScoringMethod->HasMachPts())
				{
					bHasMachPts = true;
					if (0 < machPtsEvent)
					{
						str.FormatMessage(IDS_POINTS_MACH_SUBTOTAL, machPtsEvent);
						GetListCtrl().SetItemText(index+nAdded, nextCol++, str);
					}
				}
				ASSERT(nextCol <= MAX_COLUMNS);
				++nAdded;
			}
		}
	}

	// Information that is tallied after all a venue's events.
	if (bHasDoubleQs)
	{
		machPts += inDog->GetExistingPoints().ExistingPoints(
			ARBDogExistingPoints::eMach,
			inVenue, inDiv, inLevel, NULL);
		int dblQs = static_cast<int>(QQs.size());
		dblQs += inDog->GetExistingPoints().ExistingPoints(
			ARBDogExistingPoints::eQQ,
			inVenue, inDiv, inLevel, NULL);

		GetListCtrl().InsertItem(index+nAdded, "");
		int nextCol = 1;
		GetListCtrl().SetItemText(index+nAdded, nextCol++, "");
		GetListCtrl().SetItemText(index+nAdded, nextCol++, "");
		GetListCtrl().SetItemText(index+nAdded, nextCol++, "");
		GetListCtrl().SetItemText(index+nAdded, nextCol++, "");
		GetListCtrl().SetItemText(index+nAdded, nextCol++, "");
		GetListCtrl().SetItemText(index+nAdded, nextCol++, "");
		CString str;
		str.FormatMessage(IDS_POINTS_QQS, dblQs);
		GetListCtrl().SetItemText(index+nAdded, nextCol++, str);
		ASSERT(nextCol <= MAX_COLUMNS);
		++nAdded;
	}
	if (bHasMachPts)
	{
		GetListCtrl().InsertItem(index+nAdded, "");
		int nextCol = 1;
		GetListCtrl().SetItemText(index+nAdded, nextCol++, "");
		GetListCtrl().SetItemText(index+nAdded, nextCol++, "");
		GetListCtrl().SetItemText(index+nAdded, nextCol++, "");
		GetListCtrl().SetItemText(index+nAdded, nextCol++, "");
		GetListCtrl().SetItemText(index+nAdded, nextCol++, "");
		GetListCtrl().SetItemText(index+nAdded, nextCol++, "");
		CString str;
		str.FormatMessage(IDS_POINTS_MACH, machPts);
		GetListCtrl().SetItemText(index+nAdded, nextCol++, str);
		ASSERT(nextCol <= MAX_COLUMNS);
		++nAdded;
	}
	return nAdded;
}

size_t CAgilityBookViewPoints::FindMatchingRuns(
	std::list<ARBDogRun const*> const& runs,
	std::string const& div,
	std::string const& level,
	std::string const& event,
	std::list<ARBDogRun const*>& matching)
{
	matching.clear();
	for (list<ARBDogRun const*>::const_iterator iterRun = runs.begin();
		iterRun != runs.end();
		++iterRun)
	{
		ARBDogRun const* pRun = (*iterRun);
		if (pRun->GetDivision() == div && pRun->GetLevel() == level && pRun->GetEvent() == event)
			matching.push_back(pRun);
	}
	return matching.size();
}

int CAgilityBookViewPoints::TallyPoints(
	std::list<ARBDogRun const*> const& runs,
	ARBConfigScoring const* pScoringMethod,
	int& nCleanQ,
	int& nNotCleanQ,
	LifeTimePointList& inLifetime)
{
	nCleanQ = 0;
	nNotCleanQ = 0;
	int score = 0;
	for (list<ARBDogRun const*>::const_iterator iterRun = runs.begin();
		iterRun != runs.end();
		++iterRun)
	{
		ARBDogRun const* pRun = (*iterRun);
		if (pRun->GetQ().Qualified())
		{
			bool bClean = false;
			short nLifetime;
			score += pRun->GetTitlePoints(pScoringMethod, &bClean, &nLifetime);
			if (0 < nLifetime)
				inLifetime.push_back(LifeTimePoint(pRun->GetEvent(), nLifetime));
			if (bClean)
				++nCleanQ;
			else
				++nNotCleanQ;
		}
	}
	return score;
}

/**
 * Used to accumulate run info into a flat list.
 */
class OtherPtInfo
{
public:
	OtherPtInfo(std::string const& venue,
			std::string const& div,
			std::string const& level,
			std::string const& event,
			int score)
		: m_Venue(venue)
		, m_Div(div)
		, m_Level(level)
		, m_Event(event)
		, m_Score(score)
	{
	}
	OtherPtInfo(ARBDogTrial const* pTrial, ARBDogRun const* pRun, int score)
		: m_Score(score)
	{
		ARBDogClub const* pClub = NULL;
		if (pTrial)
			pClub = pTrial->GetClubs().GetPrimaryClub();
		if (pClub)
		{
			m_Venue = pClub->GetVenue();
			m_Div = pRun->GetDivision();
			m_Level = pRun->GetLevel();
			m_Event = pRun->GetEvent();
		}
	}
	std::string m_Venue;
	std::string m_Div;
	std::string m_Level;
	std::string m_Event;
	int m_Score;
};

void CAgilityBookViewPoints::LoadData()
{
	CWaitCursor wait;

	// Reduce flicker
	GetListCtrl().SetRedraw(FALSE);

	// Clear everything.
	GetListCtrl().DeleteAllItems();

	// Find all visible items and sort them out by venue.
	ARBDog const* pDog = GetDocument()->GetCurrentDog();
	if (pDog)
	{
		std::vector<CVenueFilter> venues;
		CAgilityBookOptions::GetFilterVenue(venues);
		int idxInsertItem = 0;

		// Put general info about the dog in...
		ARBDate today(ARBDate::Today());
		GetListCtrl().InsertItem(idxInsertItem, pDog->GetCallName().c_str());
		GetListCtrl().SetItemText(idxInsertItem, 1, pDog->GetRegisteredName().c_str());
		// (MAX_COLUMNS-1) is a theorectical column - no events exist that will populate it.
		GetListCtrl().SetItemText(idxInsertItem, MAX_COLUMNS-2,
			today.GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints)).c_str());
		++idxInsertItem;

		// For each venue...
		for (ARBConfigVenueList::const_iterator iterVenue = GetDocument()->GetConfig().GetVenues().begin();
			iterVenue != GetDocument()->GetConfig().GetVenues().end();
			++iterVenue)
		{
			ARBConfigVenue const* pVenue = (*iterVenue);
			if (!CAgilityBookOptions::IsVenueVisible(venues, pVenue->GetName()))
				continue;

			LifeTimePointsList lifetime;

			// First, titles.
			bool bHeaderInserted = false;
			for (ARBDogTitleList::const_iterator iterTitle = pDog->GetTitles().begin();
				iterTitle != pDog->GetTitles().end();
				++iterTitle)
			{
				ARBDogTitle const* pTitle = (*iterTitle);
				if (pTitle->GetVenue() == pVenue->GetName()
				&& !pTitle->IsFiltered())
				{
					if (!bHeaderInserted)
					{
						bHeaderInserted = true;
						GetListCtrl().InsertItem(idxInsertItem++, "");
						GetListCtrl().InsertItem(idxInsertItem, pVenue->GetName().c_str());
						ARBDogRegNum const* pRegNum = pDog->GetRegNums().FindRegNum(pVenue->GetName());
						if (pRegNum)
						{
							CString str;
							str.Format("[%s]", pRegNum->GetNumber().c_str());
							GetListCtrl().SetItemText(idxInsertItem, 1, str);
						}
						++idxInsertItem;
					}
					GetListCtrl().InsertItem(idxInsertItem, "");
					GetListCtrl().SetItemText(idxInsertItem, 1, pTitle->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints)).c_str());
					CString str = GetDocument()->GetConfig().GetTitleCompleteName(pTitle->GetVenue(), pTitle->GetName(), false).c_str();
					if (pTitle->GetReceived())
						str += "*";
					GetListCtrl().SetItemText(idxInsertItem, 2, str);
					++idxInsertItem;
				}
			}

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
				if (!bHeaderInserted)
				{
					bHeaderInserted = true;
					GetListCtrl().InsertItem(idxInsertItem++, "");
					GetListCtrl().InsertItem(idxInsertItem, pVenue->GetName().c_str());
					ARBDogRegNum const* pRegNum = pDog->GetRegNums().FindRegNum(pVenue->GetName());
					if (pRegNum)
					{
						CString str;
						str.Format("[%s]", pRegNum->GetNumber().c_str());
						GetListCtrl().SetItemText(idxInsertItem, 1, str);
					}
					++idxInsertItem;
				}
				for (ARBConfigDivisionList::const_iterator iterDiv = pVenue->GetDivisions().begin();
					iterDiv != pVenue->GetDivisions().end();
					++iterDiv)
				{
					ARBConfigDivision const* pDiv = (*iterDiv);
					for (ARBConfigLevelList::const_iterator iterLevel = pDiv->GetLevels().begin();
						iterLevel != pDiv->GetLevels().end();
						++iterLevel)
					{
						ARBConfigLevel const* pLevel = (*iterLevel);
						LifeTimePoints pts;
						pts.pDiv = pDiv;
						pts.pLevel = pLevel;
						idxInsertItem += DoEvents(pDog, venues, idxInsertItem, trialsInVenue, pVenue, pDiv, pLevel, pts.ptList);
						if (0 < pts.ptList.size())
							lifetime.push_back(pts);
					}
				}
			}
			if (0 < lifetime.size())
			{
				int pts = 0;
				for (LifeTimePointsList::iterator iter = lifetime.begin();
					iter != lifetime.end();
					++iter)
				{
					for (LifeTimePointList::iterator iter2 = (*iter).ptList.begin();
						iter2 != (*iter).ptList.end();
						++iter2)
					{
						pts += (*iter2).points;
					}
				}
				GetListCtrl().InsertItem(idxInsertItem, "");
				int nextCol = 1;
				CString str;
				str.LoadString(IDS_LIFETIME_POINTS);
				GetListCtrl().SetItemText(idxInsertItem, nextCol++, str);
				str.Format("%d", pts);
				GetListCtrl().SetItemText(idxInsertItem, nextCol++, str);
				++idxInsertItem;
			}
		}

		// After all that, we do 'other points'.
		ARBConfigOtherPointsList const& other = GetDocument()->GetConfig().GetOtherPoints();
		if (0 < other.size())
		{
			CString str;
			GetListCtrl().InsertItem(idxInsertItem++, "");
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
					if (ARBDogExistingPoints::eOtherPoints == (*iterExisting)->GetType())
					{
						runs.push_back(OtherPtInfo(
							(*iterExisting)->GetVenue(),
							(*iterExisting)->GetDivision(),
							(*iterExisting)->GetLevel(),
							(*iterExisting)->GetEvent(),
							(*iterExisting)->GetPoints()));
					}
				}

				if (0 == runs.size())
					continue;

				GetListCtrl().InsertItem(idxInsertItem, "");
				GetListCtrl().SetItemText(idxInsertItem, 1, pOther->GetName().c_str());
				switch (pOther->GetTally())
				{
				default:
				case ARBConfigOtherPoints::eTallyAll:
					{
						int score = 0;
						for (std::list<OtherPtInfo>::iterator iter = runs.begin();
							iter != runs.end();
							++iter)
						{
							score += (*iter).m_Score;
						}
						CString str;
						str.Format("%d", score);
						GetListCtrl().SetItemText(idxInsertItem, 2, str);
					}
					break;

				case ARBConfigOtherPoints::eTallyAllByEvent:
					{
						std::set<std::string> tally;
						std::list<OtherPtInfo>::iterator iter;
						for (iter = runs.begin(); iter != runs.end(); ++iter)
						{
							tally.insert((*iter).m_Event);
						}
						for (std::set<std::string>::iterator iterTally = tally.begin();
							iterTally != tally.end();
							++iterTally)
						{
							int score = 0;
							for (iter = runs.begin(); iter != runs.end(); ++iter)
							{
								if ((*iter).m_Event == (*iterTally))
									score += (*iter).m_Score;
							}
							++idxInsertItem;
							GetListCtrl().InsertItem(idxInsertItem, "");
							GetListCtrl().SetItemText(idxInsertItem, 2, (*iterTally).c_str());
							CString str;
							str.Format("%d", score);
							GetListCtrl().SetItemText(idxInsertItem, 3, str);
						}
					}
					break;

				case ARBConfigOtherPoints::eTallyLevel:
					{
						std::set<std::string> tally;
						std::list<OtherPtInfo>::iterator iter;
						for (iter = runs.begin(); iter != runs.end(); ++iter)
						{
							tally.insert((*iter).m_Level);
						}
						for (std::set<std::string>::iterator iterTally = tally.begin();
							iterTally != tally.end();
							++iterTally)
						{
							int score = 0;
							for (iter = runs.begin(); iter != runs.end(); ++iter)
							{
								if ((*iter).m_Level == (*iterTally))
									score += (*iter).m_Score;
							}
							++idxInsertItem;
							GetListCtrl().InsertItem(idxInsertItem, "");
							GetListCtrl().SetItemText(idxInsertItem, 2, (*iterTally).c_str());
							CString str;
							str.Format("%d", score);
							GetListCtrl().SetItemText(idxInsertItem, 3, str);
						}
					}
					break;

				case ARBConfigOtherPoints::eTallyLevelByEvent:
					{
						typedef std::pair<std::string, std::string> LevelEvent;
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
							int score = 0;
							for (iter = runs.begin(); iter != runs.end(); ++iter)
							{
								if ((*iter).m_Level == (*iterTally).first
								&& (*iter).m_Event == (*iterTally).second)
									score += (*iter).m_Score;
							}
							++idxInsertItem;
							GetListCtrl().InsertItem(idxInsertItem, "");
							GetListCtrl().SetItemText(idxInsertItem, 2, (*iterTally).first.c_str());
							GetListCtrl().SetItemText(idxInsertItem, 3, (*iterTally).second.c_str());
							CString str;
							str.Format("%d", score);
							GetListCtrl().SetItemText(idxInsertItem, 4, str);
						}
					}
					break;
				}
				++idxInsertItem;
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
			((CMainFrame*)AfxGetMainWnd())->SetStatusText(msg, IsFiltered());
		if (GetMessage2(msg))
			((CMainFrame*)AfxGetMainWnd())->SetStatusText2(msg);
	}

	GetListCtrl().SetRedraw(TRUE);
	GetListCtrl().Invalidate();
}

// CAgilityBookViewPoints message handlers
