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
 * @brief implementation of the CAgilityBookViewPoints class
 * @author David Connet
 *
 * Revision History
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
// 1: Date
// 2: Title
//
// 1: Venue
// 2: Division
// 3: Level
// 4: Event
// 5: n Runs, n Judges, n Partners
// 6: n Qs (n clean), n Judges, n Partners
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
	CString msg;
	if (pActivateView && GetMessage(msg))
		((CMainFrame*)AfxGetMainWnd())->SetStatusText(msg, IsFiltered());
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
	msg = "          ";
	return true;
}

// Entering this function, we know the trial is visible.
// We don't know if the individual runs are.
int CAgilityBookViewPoints::DoEvents(
	const std::vector<CVenueFilter>& venues,
	int index,
	const std::list<const ARBDogTrial*>& trials,
	const ARBConfigVenue* pVenue,
	const ARBConfigDivision* inDiv,
	const ARBConfigLevel* inLevel)
{
	int nAdded = 0;
	int machPts = 0;
	bool bHasMachPts = false;
	std::set<ARBDate> QQs;
	bool bHasDoubleQs = false;
	for (ARBConfigEventList::const_iterator iterEvent = pVenue->GetEvents().begin();
	iterEvent != pVenue->GetEvents().end();
	++iterEvent)
	{
		const ARBConfigEvent* pEvent = (*iterEvent);
		const ARBConfigScoring* pScoringMethod = pEvent->FindEvent(inDiv->GetName(), inLevel->GetName());
		// Don't tally runs that have no titling points.
		if (!pScoringMethod || 0 == pScoringMethod->GetTitlePoints().size())
			continue;
		int SQs = 0;
		int machPtsEvent = 0;
		list<const ARBDogRun*> matching;
		set<std::string> judges;
		set<std::string> judgesQ;
		set<std::string> partners;
		set<std::string> partnersQ;
		for (list<const ARBDogTrial*>::const_iterator iterTrial = trials.begin();
		iterTrial != trials.end();
		++iterTrial)
		{
			const ARBDogTrial* pTrial = (*iterTrial);
			if (pScoringMethod->HasDoubleQ())
			{
				for (ARBDate date = pTrial->GetRuns().GetStartDate();
				date <= pTrial->GetRuns().GetEndDate();
				++date)
				{
					if (2 == pTrial->NumQs(date, GetDocument()->GetConfig(), inDiv->GetName(), inLevel->GetName()))
					{
						int nVisible = 0;
						// But first, make sure all the runs are visible.
						for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
						iterRun != pTrial->GetRuns().end();
						++iterRun)
						{
							const ARBDogRun* pRun = (*iterRun);
							if (date == pRun->GetDate()
							&& !pRun->IsFiltered())
								++nVisible;
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
				const ARBDogRun* pRun = (*iterRun);
				if (!pRun->IsFiltered())
				{
					if (pRun->GetDivision() == inDiv->GetName()
					&& (pRun->GetLevel() == inLevel->GetName() || inLevel->GetSubLevels().FindSubLevel(pRun->GetLevel()))
					&& pRun->GetEvent() == pEvent->GetName())
					{
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
		}
		CString str, str2;
		if (0 < matching.size())
		{
			GetListCtrl().InsertItem(index+nAdded, "");
			int nextCol = 1;
			GetListCtrl().SetItemText(index+nAdded, nextCol++, inDiv->GetName().c_str());
			GetListCtrl().SetItemText(index+nAdded, nextCol++, inLevel->GetName().c_str());
			GetListCtrl().SetItemText(index+nAdded, nextCol++, pEvent->GetName().c_str());
			int nCleanQ, nNotCleanQ;
			int pts = TallyPoints(matching, pScoringMethod, nCleanQ, nNotCleanQ);
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
			str.FormatMessage(IDS_POINTS_QS, nCleanQ + nNotCleanQ);
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
			str.Format("%d", pts);
			GetListCtrl().SetItemText(index+nAdded, nextCol++, str);
			if (pScoringMethod->HasSuperQ())
			{
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
			++nAdded;
		}
	}
	if (bHasDoubleQs)
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
		str.FormatMessage(IDS_POINTS_QQS, QQs.size());
		GetListCtrl().SetItemText(index+nAdded, nextCol++, str);
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
		++nAdded;
	}
	ASSERT(nAdded < MAX_COLUMNS);
	return nAdded;
}

size_t CAgilityBookViewPoints::FindMatchingRuns(
	const std::list<const ARBDogRun*>& runs,
	const std::string& div,
	const std::string& level,
	const std::string& event,
	std::list<const ARBDogRun*>& matching)
{
	matching.clear();
	for (list<const ARBDogRun*>::const_iterator iterRun = runs.begin(); iterRun != runs.end(); ++iterRun)
	{
		const ARBDogRun* pRun = (*iterRun);
		if (pRun->GetDivision() == div && pRun->GetLevel() == level && pRun->GetEvent() == event)
			matching.push_back(pRun);
	}
	return matching.size();
}

int CAgilityBookViewPoints::TallyPoints(
	const std::list<const ARBDogRun*>& runs,
	const ARBConfigScoring* pScoringMethod,
	int& nCleanQ,
	int& nNotCleanQ)
{
	nCleanQ = 0;
	nNotCleanQ = 0;
	int score = 0;
	for (list<const ARBDogRun*>::const_iterator iterRun = runs.begin(); iterRun != runs.end(); ++iterRun)
	{
		const ARBDogRun* pRun = (*iterRun);
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

/**
 * Used to accumulate run info into a flat list.
 */
class OtherPtInfo
{
public:
	OtherPtInfo(const ARBDogTrial* pTrial, const ARBDogRun* pRun, int score)
		: m_pTrial(pTrial)
		, m_pRun(pRun)
		, m_Score(score)
	{
	}
	const ARBDogTrial* m_pTrial;
	const ARBDogRun* m_pRun;
	int m_Score;
};

void CAgilityBookViewPoints::LoadData()
{
	CWaitCursor wait;
	// Remember what's selected.
//	CAgilityBookViewData* pCurData = GetCurrentViewItem();
//	if (pCurData)
//		pCurData->AddRef();

	// Reduce flicker
	GetListCtrl().SetRedraw(FALSE);

	// Clear everything.
	GetListCtrl().DeleteAllItems();

	// Find all visible items and sort them out by venue.
	const ARBDog* pDog = GetDocument()->GetCurrentDog();
	if (pDog)
	{
		std::vector<CVenueFilter> venues;
		CAgilityBookOptions::GetFilterVenue(venues);
		int i = 0;
		// For each venue...
		for (ARBConfigVenueList::const_iterator iterVenue = GetDocument()->GetConfig().GetVenues().begin();
		iterVenue != GetDocument()->GetConfig().GetVenues().end();
		++iterVenue)
		{
			const ARBConfigVenue* pVenue = (*iterVenue);
			if (!CAgilityBookOptions::IsVenueVisible(venues, pVenue->GetName()))
				continue;

			// First, titles.
			bool bHeaderInserted = false;
			for (ARBDogTitleList::const_iterator iterTitle = pDog->GetTitles().begin();
			iterTitle != pDog->GetTitles().end();
			++iterTitle)
			{
				const ARBDogTitle* pTitle = (*iterTitle);
				if (pTitle->GetVenue() == pVenue->GetName()
				&& !pTitle->IsFiltered())
				{
					if (!bHeaderInserted)
					{
						bHeaderInserted = true;
						if (i > 0)
							GetListCtrl().InsertItem(i++, "");
						GetListCtrl().InsertItem(i, pVenue->GetName().c_str());
						const ARBDogRegNum* pRegNum = pDog->GetRegNums().FindRegNum(pVenue->GetName());
						if (pRegNum)
						{
							CString str;
							str.Format("[%s]", pRegNum->GetNumber().c_str());
							GetListCtrl().SetItemText(i, 1, str);
						}
						++i;
					}
					GetListCtrl().InsertItem(i, "");
					GetListCtrl().SetItemText(i, 1, pTitle->GetDate().GetString(false, false).c_str());
					CString str = pTitle->GetName().c_str();
					if (pTitle->GetReceived())
						str += "*";
					GetListCtrl().SetItemText(i, 2, str);
					++i;
				}
			}

			// Then the runs.
			list<const ARBDogTrial*> trialsInVenue;
			for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
			iterTrial != pDog->GetTrials().end();
			++iterTrial)
			{
				const ARBDogTrial* pTrial = (*iterTrial);
				if (pTrial->HasVenue(pVenue->GetName())
				&& !pTrial->IsFiltered())
				{
					trialsInVenue.push_back(pTrial);
				}
			}
			if (0 < trialsInVenue.size())
			{
				if (!bHeaderInserted)
					GetListCtrl().InsertItem(i++, pVenue->GetName().c_str());
				for (ARBConfigDivisionList::const_iterator iterDiv = pVenue->GetDivisions().begin();
				iterDiv != pVenue->GetDivisions().end();
				++iterDiv)
				{
					const ARBConfigDivision* pDiv = (*iterDiv);
					for (ARBConfigLevelList::const_iterator iterLevel = pDiv->GetLevels().begin();
					iterLevel != pDiv->GetLevels().end();
					++iterLevel)
					{
						const ARBConfigLevel* pLevel = (*iterLevel);
						i += DoEvents(venues, i, trialsInVenue, pVenue, pDiv, pLevel);
					}
				}
			}
		}

		// After all that, we do 'other points'.
		const ARBConfigOtherPointsList& other = GetDocument()->GetConfig().GetOtherPoints();
		if (0 < other.size())
		{
			CString str;
			if (i > 0)
				GetListCtrl().InsertItem(i++, "");
			str.LoadString(IDS_OTHERPOINTS);
			GetListCtrl().InsertItem(i++, str);
			for (ARBConfigOtherPointsList::const_iterator iterOther = other.begin(); iterOther != other.end(); ++iterOther)
			{
				// First, just generate a list of runs with the needed info.
				std::list<OtherPtInfo> runs;

				ARBConfigOtherPoints* pOther = (*iterOther);
				for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
				iterTrial != pDog->GetTrials().end();
				++iterTrial)
				{
					const ARBDogTrial* pTrial = (*iterTrial);
					if (!pTrial->IsFiltered())
					{
						for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
						iterRun != pTrial->GetRuns().end();
						++iterRun)
						{
							const ARBDogRun* pRun = (*iterRun);
							if (!pRun->IsFiltered())
							{
								for (ARBDogRunOtherPointsList::const_iterator iterOtherPts = pRun->GetOtherPoints().begin();
								iterOtherPts != pRun->GetOtherPoints().end();
								++iterOtherPts)
								{
									const ARBDogRunOtherPoints* pOtherPts = (*iterOtherPts);
									if (pOtherPts->GetName() == pOther->GetName())
									{
										runs.push_back(OtherPtInfo(pTrial, pRun, pOtherPts->GetPoints()));
									}
								}
							}
						}
					}
				}

				if (0 == runs.size())
					continue;

				GetListCtrl().InsertItem(i, "");
				GetListCtrl().SetItemText(i, 1, pOther->GetName().c_str());
				switch (pOther->GetTally())
				{
				default:
				case ARBConfigOtherPoints::eTallyAll:
					{
						int score = 0;
						for (std::list<OtherPtInfo>::iterator iter = runs.begin(); iter != runs.end(); ++iter)
						{
							score += (*iter).m_Score;
						}
						CString str;
						str.Format("%d", score);
						GetListCtrl().SetItemText(i, 2, str);
					}
					break;

				case ARBConfigOtherPoints::eTallyAllByEvent:
					{
						std::set<std::string> tally;
						std::list<OtherPtInfo>::iterator iter;
						for (iter = runs.begin(); iter != runs.end(); ++iter)
						{
							tally.insert((*iter).m_pRun->GetEvent());
						}
						for (std::set<std::string>::iterator iterTally = tally.begin(); iterTally != tally.end(); ++iterTally)
						{
							int score = 0;
							for (iter = runs.begin(); iter != runs.end(); ++iter)
							{
								if ((*iter).m_pRun->GetEvent() == (*iterTally))
									score += (*iter).m_Score;
							}
							++i;
							GetListCtrl().InsertItem(i, "");
							GetListCtrl().SetItemText(i, 2, (*iterTally).c_str());
							CString str;
							str.Format("%d", score);
							GetListCtrl().SetItemText(i, 3, str);
						}
					}
					break;

				case ARBConfigOtherPoints::eTallyLevel:
					{
						std::set<std::string> tally;
						std::list<OtherPtInfo>::iterator iter;
						for (iter = runs.begin(); iter != runs.end(); ++iter)
						{
							tally.insert((*iter).m_pRun->GetLevel());
						}
						for (std::set<std::string>::iterator iterTally = tally.begin(); iterTally != tally.end(); ++iterTally)
						{
							int score = 0;
							for (iter = runs.begin(); iter != runs.end(); ++iter)
							{
								if ((*iter).m_pRun->GetLevel() == (*iterTally))
									score += (*iter).m_Score;
							}
							++i;
							GetListCtrl().InsertItem(i, "");
							GetListCtrl().SetItemText(i, 2, (*iterTally).c_str());
							CString str;
							str.Format("%d", score);
							GetListCtrl().SetItemText(i, 3, str);
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
							tally.insert(LevelEvent((*iter).m_pRun->GetLevel(), (*iter).m_pRun->GetEvent()));
						}
						for (std::set<LevelEvent>::iterator iterTally = tally.begin(); iterTally != tally.end(); ++iterTally)
						{
							int score = 0;
							for (iter = runs.begin(); iter != runs.end(); ++iter)
							{
								if ((*iter).m_pRun->GetLevel() == (*iterTally).first
								&& (*iter).m_pRun->GetEvent() == (*iterTally).second)
									score += (*iter).m_Score;
							}
							++i;
							GetListCtrl().InsertItem(i, "");
							GetListCtrl().SetItemText(i, 2, (*iterTally).first.c_str());
							GetListCtrl().SetItemText(i, 3, (*iterTally).second.c_str());
							CString str;
							str.Format("%d", score);
							GetListCtrl().SetItemText(i, 4, str);
						}
					}
					break;
				}
				++i;
			}
		}
	}

	int nColumnCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumnCount; ++i)
		GetListCtrl().SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);

	CString msg;
	if (GetMessage(msg) && IsWindowVisible())
		((CMainFrame*)AfxGetMainWnd())->SetStatusText(msg, IsFiltered());

	// Cleanup.
//	if (pCurData)
//		pCurData->Release();
	GetListCtrl().SetRedraw(TRUE);
	GetListCtrl().Invalidate();
}

// CAgilityBookViewPoints message handlers
