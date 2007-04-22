/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
 * @brief implementation of the CAgilityBookViewRuns class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-06-02 DRC OnNeedText was quietly referencing m_Columns[-1]. Oops.
 * @li 2005-01-25 DRC Remember the sort column between program invocations.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2004-09-07 DRC Time+Fault scoring shouldn't include time faults.
 * @li 2004-06-24 DRC Added a sort header image.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-04-06 DRC Added simple sorting by column.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-12-30 DRC Implemented full column reordering.
 * @li 2003-12-27 DRC Implemented Find/FindNext.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-21 DRC Enabled copy and select all.
 * @li 2003-10-09 DRC Added option to not filter runs by selected trial.
 * @li 2003-08-30 DRC Added the ability to copy entries to the clipboard.
 * @li 2003-08-27 DRC Cleaned up selection synchronization.
 *                    Added creating titles/trials/runs from the Run view.
 * @li 2003-08-25 DRC Mirror the selection in the tree.
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "AgilityBookViewRuns.h"
#if _MSC_VER < 1300
#include "htmlhelp.h"
#endif

#include <algorithm>
#include <afxpriv.h> // wm_commandhelp
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTree.h"
#include "AgilityBookTreeData.h"
#include "ARBTypes.h"
#include "ClipBoard.h"
#include "DlgAssignColumns.h"
#include "DlgFind.h"
#include "Element.h"
#include "FilterOptions.h"
#include "ListData.h"
#include "MainFrm.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewRunsData

class CAgilityBookViewRunsData : public CListData
{
	friend int CALLBACK CompareRuns(LPARAM, LPARAM, LPARAM);
public:
	CAgilityBookViewRunsData(
			CAgilityBookViewRuns* pView,
			ARBDogPtr pDog,
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun)
		: m_pView(pView)
		, m_pDog(pDog)
		, m_pTrial(pTrial)
		, m_pRun(pRun)
	{
	}
	~CAgilityBookViewRunsData()
	{
	}

	ARBDogRunPtr GetRun()			{return m_pRun;}
	ARBString OnNeedText(int iCol) const;
	int OnNeedIcon() const;

protected:
	CAgilityBookViewRuns* m_pView;
	ARBDogPtr m_pDog;
	ARBDogTrialPtr m_pTrial;
	ARBDogRunPtr m_pRun;
};

ARBString CAgilityBookViewRunsData::OnNeedText(int iCol) const
{
	short val;
	ARBostringstream str;
	if (0 < iCol && m_pRun)
	{
		// Col 0 is special: it has the icons. Instead of saving it,
		// we simply ignore it - so iCol is always off by 1.
		switch (m_pView->m_Columns[iCol-1])
		{
		default:
			break;

		case IO_RUNS_REG_NAME:
			str << m_pDog->GetRegisteredName();
			break;
		case IO_RUNS_CALL_NAME:
			str << m_pDog->GetCallName();
			break;
		case IO_RUNS_DATE:
			str << m_pRun->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::eRunList));
			break;
		case IO_RUNS_VENUE:
			{
				int i = 0;
				for (ARBDogClubList::const_iterator iter = m_pTrial->GetClubs().begin();
					iter != m_pTrial->GetClubs().end();
					++iter, ++i)
				{
					if (0 < i)
						str << _T('/');
					str << (*iter)->GetVenue();
				}
			}
			break;
		case IO_RUNS_CLUB:
			{
				int i = 0;
				for (ARBDogClubList::const_iterator iter = m_pTrial->GetClubs().begin();
					iter != m_pTrial->GetClubs().end();
					++iter, ++i)
				{
					if (0 < i)
						str << _T('/');
					str << (*iter)->GetName();
				}
			}
			break;
		case IO_RUNS_LOCATION:
			str << m_pTrial->GetLocation();
			break;
		case IO_RUNS_TRIAL_NOTES:
			str << m_pTrial->GetNote();
			break;
		case IO_RUNS_DIVISION:
			str << m_pRun->GetDivision();
			break;
		case IO_RUNS_LEVEL:
			str << m_pRun->GetLevel();
			break;
		case IO_RUNS_EVENT:
			str << m_pRun->GetEvent();
			break;
		case IO_RUNS_HEIGHT:
			str << m_pRun->GetHeight();
			break;
		case IO_RUNS_JUDGE:
			str << m_pRun->GetJudge();
			break;
		case IO_RUNS_HANDLER:
			str << m_pRun->GetHandler();
			break;
		case IO_RUNS_CONDITIONS:
			str << m_pRun->GetConditions();
			break;
		case IO_RUNS_COURSE_FAULTS:
			str << m_pRun->GetScoring().GetCourseFaults();
			break;
		case IO_RUNS_TIME:
			str << ARBDouble::str(m_pRun->GetScoring().GetTime());
			break;
		case IO_RUNS_YARDS:
			if (ARBDogRunScoring::eTypeByTime == m_pRun->GetScoring().GetType()
			&& 0.0 < m_pRun->GetScoring().GetYards())
			{
				str << ARBDouble::str(m_pRun->GetScoring().GetYards(), 0);
			}
			break;
		case IO_RUNS_MIN_YPS:
			{
				double yps;
				if (m_pRun->GetScoring().GetMinYPS(CAgilityBookOptions::GetTableInYPS(), yps))
				{
					str << ARBDouble::str(yps, 3);
				}
			}
			break;
		case IO_RUNS_YPS:
			{
				double yps;
				if (m_pRun->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), yps))
				{
					str << ARBDouble::str(yps, 3);
				}
			}
			break;
		case IO_RUNS_SCT:
			if (ARBDogRunScoring::eTypeByTime == m_pRun->GetScoring().GetType()
			&& 0.0 < m_pRun->GetScoring().GetSCT())
			{
				str << ARBDouble::str(m_pRun->GetScoring().GetSCT());
			}
			break;
		case IO_RUNS_TOTAL_FAULTS:
			if (ARBDogRunScoring::eTypeByTime == m_pRun->GetScoring().GetType())
			{
				ARBConfigScoringPtr pScoring;
				if (m_pTrial->GetClubs().GetPrimaryClub())
                    m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						m_pTrial->GetClubs().GetPrimaryClubVenue(),
						m_pRun->GetEvent(),
						m_pRun->GetDivision(),
						m_pRun->GetLevel(),
						m_pRun->GetDate(),
						NULL,
						&pScoring);
				double faults = m_pRun->GetScoring().GetCourseFaults() + m_pRun->GetScoring().GetTimeFaults(pScoring);
				str << ARBDouble::str(faults, 0);
			}
			break;
		case IO_RUNS_REQ_OPENING:
			if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetNeedOpenPts();
			}
			break;
		case IO_RUNS_REQ_CLOSING:
			if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetNeedClosePts();
			}
			break;
		case IO_RUNS_OPENING:
			if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetOpenPts();
			}
			break;
		case IO_RUNS_CLOSING:
			if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetClosePts();
			}
			break;
		case IO_RUNS_REQ_POINTS:
			if (ARBDogRunScoring::eTypeByPoints == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetNeedOpenPts();
			}
			break;
		case IO_RUNS_POINTS:
			if (ARBDogRunScoring::eTypeByPoints == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetOpenPts();
			}
			break;
		case IO_RUNS_PLACE:
			val = m_pRun->GetPlace();
			if (0 > val)
				str << _T('?');
			else if (0 == val)
				str << _T('-');
			else
				str << val;
			break;
		case IO_RUNS_IN_CLASS:
			val = m_pRun->GetInClass();
			if (0 >= val)
				str << _T('?');
			else
				str << val;
			break;
		case IO_RUNS_DOGSQD:
			val = m_pRun->GetDogsQd();
			if (0 > val)
				str << _T('?');
			else
				str << m_pRun->GetDogsQd();
			break;
		case IO_RUNS_Q:
			{
				bool bSet = false;
				if (m_pRun->GetQ().Qualified())
				{
					ARBString q;
					if (m_pRun->GetMultiQ())
					{
						q = m_pRun->GetMultiQ()->GetShortName();
					}
					if (ARB_Q::eQ_SuperQ == m_pRun->GetQ())
					{
						bSet = true;
						CString tmp;
						tmp.LoadString(IDS_SQ);
						str << (LPCTSTR)tmp;
						if (!q.empty())
							str << _T('/') << q;
					}
					else
					{
						if (!q.empty())
						{
							bSet = true;
							str << q;
						}
					}
				}
				if (!bSet)
					str << m_pRun->GetQ().str();
			}
			break;
		case IO_RUNS_SCORE:
			if (m_pRun->GetQ().Qualified()
			|| ARB_Q::eQ_NQ == m_pRun->GetQ())
			{
				ARBConfigScoringPtr pScoring;
				if (m_pTrial->GetClubs().GetPrimaryClub())
					m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						m_pTrial->GetClubs().GetPrimaryClubVenue(),
						m_pRun->GetEvent(),
						m_pRun->GetDivision(),
						m_pRun->GetLevel(),
						m_pRun->GetDate(),
						NULL,
						&pScoring);
				if (pScoring)
				{
					str << ARBDouble::str(m_pRun->GetScore(pScoring));
				}
			}
			break;
		case IO_RUNS_TITLE_POINTS:
			{
				double pts = 0;
				if (m_pRun->GetQ().Qualified())
				{
					ARBConfigScoringPtr pScoring;
					if (m_pTrial->GetClubs().GetPrimaryClub())
						m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
							m_pTrial->GetClubs().GetPrimaryClubVenue(),
							m_pRun->GetEvent(),
							m_pRun->GetDivision(),
							m_pRun->GetLevel(),
							m_pRun->GetDate(),
							NULL,
							&pScoring);
					if (pScoring)
					{
						pts = m_pRun->GetTitlePoints(pScoring);
					}
				}
				str << pts;
			}
			break;
		case IO_RUNS_COMMENTS:
			{
				CString str2(m_pRun->GetNote().c_str());
				str2.Replace(_T("\n"), _T(" "));
				str << (LPCTSTR)str2;
			}
			break;
		case IO_RUNS_FAULTS:
			{
				int i = 0;
				for (ARBDogFaultList::const_iterator iter = m_pRun->GetFaults().begin();
					iter != m_pRun->GetFaults().end();
					++i, ++iter)
				{
					if (0 < i)
						str << _T(", ");
					str << (*iter);
				}
			}
			break;
		case IO_RUNS_SPEED:
			{
				ARBConfigScoringPtr pScoring;
				if (m_pTrial->GetClubs().GetPrimaryClub())
                    m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						m_pTrial->GetClubs().GetPrimaryClubVenue(),
						m_pRun->GetEvent(),
						m_pRun->GetDivision(),
						m_pRun->GetLevel(),
						m_pRun->GetDate(),
						NULL,
						&pScoring);
				if (pScoring)
				{
					if (pScoring->HasSpeedPts() && m_pRun->GetQ().Qualified())
					{
						str << m_pRun->GetSpeedPoints(pScoring);
					}
				}
			}
			break;
		}
	}
	return str.str();
}

int CAgilityBookViewRunsData::OnNeedIcon() const
{
	int iImage = -1;
	if (m_pRun)
	{
		if (0 < m_pRun->GetCRCDRawMetaData().length())
			iImage = m_pView->m_imgMap;
		else if (0 < m_pRun->GetCRCD().length())
			iImage = m_pView->m_imgCourse;
	}
	return iImage;
}

/////////////////////////////////////////////////////////////////////////////
// List sorting

CAgilityBookViewRuns::CSortColumn::CSortColumn(std::vector<int>& inColumns)
	: m_Columns(inColumns)
	, m_iCol(1)
{
}

void CAgilityBookViewRuns::CSortColumn::Initialize()
{
	int realCol = IO_RUNS_DATE;
	realCol = AfxGetApp()->GetProfileInt(_T("Sorting"), _T("Runs"), realCol);
	int neg = 1;
	if (0 > realCol)
	{
		neg = -1;
		realCol *= -1;
	}
	int col = LookupColumn(realCol);
	if (0 > m_iCol)
		col = LookupColumn(IO_RUNS_DATE);
	m_iCol = col * neg;
}

void CAgilityBookViewRuns::CSortColumn::SetColumn(int iCol)
{
	m_iCol = iCol;
	if (0 == iCol)
		return;
	int neg = 1;
	int col = iCol;
	if (0 > iCol)
	{
		neg = -1;
		col = iCol * -1;
	}
	int realCol = m_Columns[col-1] * neg;
	AfxGetApp()->WriteProfileInt(_T("Sorting"), _T("Runs"), realCol);
}

int CAgilityBookViewRuns::CSortColumn::LookupColumn(int iCol) const
{
	size_t n = m_Columns.size();
	for (size_t i = 0; i < n; ++i)
	{
		if (m_Columns[i] == iCol)
		{
			return static_cast<int>(i+1);
		}
	}
	return -1;
}

struct SORT_RUN_INFO
{
	CAgilityBookViewRuns* pThis;
	int nCol;
};

int CALLBACK CompareRuns(
		LPARAM lParam1,
		LPARAM lParam2,
		LPARAM lParam3)
{
	SORT_RUN_INFO* sortInfo = reinterpret_cast<SORT_RUN_INFO*>(lParam3);
	if (!sortInfo)
		return 0;
	CListData* pRawRun1 = reinterpret_cast<CListData*>(lParam1);
	CListData* pRawRun2 = reinterpret_cast<CListData*>(lParam2);
	CAgilityBookViewRunsData* pRun1 = dynamic_cast<CAgilityBookViewRunsData*>(pRawRun1);
	CAgilityBookViewRunsData* pRun2 = dynamic_cast<CAgilityBookViewRunsData*>(pRawRun2);
	int nRet = 0;
	int iCol = abs(sortInfo->nCol);
	// Col 0 is special: it has the icons. Instead of saving it,
	// we simply ignore it - so iCol is always off by 1.
	switch (sortInfo->pThis->m_Columns[iCol - 1])
	{
	default:
		break;

	case IO_RUNS_REG_NAME:
		if (pRun1->m_pDog->GetRegisteredName() < pRun2->m_pDog->GetRegisteredName())
			nRet = -1;
		else if (pRun1->m_pDog->GetRegisteredName() > pRun2->m_pDog->GetRegisteredName())
			nRet = 1;
		break;
	case IO_RUNS_CALL_NAME:
		if (pRun1->m_pDog->GetCallName() < pRun2->m_pDog->GetCallName())
			nRet = -1;
		else if (pRun1->m_pDog->GetCallName() > pRun2->m_pDog->GetCallName())
			nRet = 1;
		break;
	case IO_RUNS_DATE:
		if (pRun1->m_pRun->GetDate() < pRun2->m_pRun->GetDate())
			nRet = -1;
		else if (pRun1->m_pRun->GetDate() > pRun2->m_pRun->GetDate())
			nRet = 1;
		break;
	case IO_RUNS_VENUE:
		{
			ARBString str1, str2;
			int i = 0;
			ARBDogClubList::const_iterator iter;
			for (iter = pRun1->m_pTrial->GetClubs().begin();
				iter != pRun1->m_pTrial->GetClubs().end();
				++iter, ++i)
			{
				if (0 < i)
					str1 += _T("/");
				str1 += (*iter)->GetVenue();
			}
			for (i = 0, iter = pRun2->m_pTrial->GetClubs().begin();
				iter != pRun2->m_pTrial->GetClubs().end();
				++iter, ++i)
			{
				if (0 < i)
					str2 += _T("/");
				str2 += (*iter)->GetVenue();
			}
			if (str1 < str2)
				nRet = -1;
			else if (str1 > str2)
				nRet = 1;
		}
		break;
	case IO_RUNS_CLUB:
		{
			ARBString str1, str2;
			int i = 0;
			ARBDogClubList::const_iterator iter;
			for (iter = pRun1->m_pTrial->GetClubs().begin();
				iter != pRun1->m_pTrial->GetClubs().end();
				++iter, ++i)
			{
				if (0 < i)
					str1 += _T("/");
				str1 += (*iter)->GetName();
			}
			for (i = 0, iter = pRun2->m_pTrial->GetClubs().begin();
				iter != pRun2->m_pTrial->GetClubs().end();
				++iter, ++i)
			{
				if (0 < i)
					str2 += _T("/");
				str2 += (*iter)->GetName();
			}
			if (str1 < str2)
				nRet = -1;
			else if (str1 > str2)
				nRet = 1;
		}
		break;
	case IO_RUNS_LOCATION:
		if (pRun1->m_pTrial->GetLocation() < pRun2->m_pTrial->GetLocation())
			nRet = -1;
		else if (pRun1->m_pTrial->GetLocation() > pRun2->m_pTrial->GetLocation())
			nRet = 1;
		break;
	case IO_RUNS_TRIAL_NOTES:
		if (pRun1->m_pTrial->GetNote() < pRun2->m_pTrial->GetNote())
			nRet = -1;
		else if (pRun1->m_pTrial->GetNote() > pRun2->m_pTrial->GetNote())
			nRet = 1;
		break;
	case IO_RUNS_DIVISION:
		if (pRun1->m_pRun->GetDivision() < pRun2->m_pRun->GetDivision())
			nRet = -1;
		else if (pRun1->m_pRun->GetDivision() > pRun2->m_pRun->GetDivision())
			nRet = 1;
		break;
	case IO_RUNS_LEVEL:
		if (pRun1->m_pRun->GetLevel() < pRun2->m_pRun->GetLevel())
			nRet = -1;
		else if (pRun1->m_pRun->GetLevel() > pRun2->m_pRun->GetLevel())
			nRet = 1;
		break;
	case IO_RUNS_EVENT:
		if (pRun1->m_pRun->GetEvent() < pRun2->m_pRun->GetEvent())
			nRet = -1;
		else if (pRun1->m_pRun->GetEvent() > pRun2->m_pRun->GetEvent())
			nRet = 1;
		break;
	case IO_RUNS_HEIGHT:
		if (pRun1->m_pRun->GetHeight() < pRun2->m_pRun->GetHeight())
			nRet = -1;
		else if (pRun1->m_pRun->GetHeight() > pRun2->m_pRun->GetHeight())
			nRet = 1;
		break;
	case IO_RUNS_JUDGE:
		if (pRun1->m_pRun->GetJudge() < pRun2->m_pRun->GetJudge())
			nRet = -1;
		else if (pRun1->m_pRun->GetJudge() > pRun2->m_pRun->GetJudge())
			nRet = 1;
		break;
	case IO_RUNS_HANDLER:
		if (pRun1->m_pRun->GetHandler() < pRun2->m_pRun->GetHandler())
			nRet = -1;
		else if (pRun1->m_pRun->GetHandler() > pRun2->m_pRun->GetHandler())
			nRet = 1;
		break;
	case IO_RUNS_CONDITIONS:
		if (pRun1->m_pRun->GetConditions() < pRun2->m_pRun->GetConditions())
			nRet = -1;
		else if (pRun1->m_pRun->GetConditions() > pRun2->m_pRun->GetConditions())
			nRet = 1;
		break;
	case IO_RUNS_COURSE_FAULTS:
		if (pRun1->m_pRun->GetScoring().GetCourseFaults() < pRun2->m_pRun->GetScoring().GetCourseFaults())
			nRet = -1;
		else if (pRun1->m_pRun->GetScoring().GetCourseFaults() > pRun2->m_pRun->GetScoring().GetCourseFaults())
			nRet = 1;
		break;
	case IO_RUNS_TIME:
		if (pRun1->m_pRun->GetScoring().GetTime() < pRun2->m_pRun->GetScoring().GetTime())
			nRet = -1;
		else if (pRun1->m_pRun->GetScoring().GetTime() > pRun2->m_pRun->GetScoring().GetTime())
			nRet = 1;
		break;
	case IO_RUNS_YARDS:
		{
			bool bRun1 = (ARBDogRunScoring::eTypeByTime == pRun1->m_pRun->GetScoring().GetType()
				&& 0.0 < pRun1->m_pRun->GetScoring().GetYards());
			bool bRun2 = (ARBDogRunScoring::eTypeByTime == pRun2->m_pRun->GetScoring().GetType()
				&& 0.0 < pRun2->m_pRun->GetScoring().GetYards());
			if (bRun1 && bRun2)
			{
				if (pRun1->m_pRun->GetScoring().GetYards() < pRun2->m_pRun->GetScoring().GetYards())
					nRet = -1;
				else if (pRun1->m_pRun->GetScoring().GetYards() > pRun2->m_pRun->GetScoring().GetYards())
					nRet = 1;
			}
			else if (bRun1)
				nRet = 1;
			else if (bRun2)
				nRet = -1;
		}
		break;
	case IO_RUNS_MIN_YPS:
		{
			double yps1, yps2;
			bool bOk1 = pRun1->m_pRun->GetScoring().GetMinYPS(CAgilityBookOptions::GetTableInYPS(), yps1);
			bool bOk2 = pRun2->m_pRun->GetScoring().GetMinYPS(CAgilityBookOptions::GetTableInYPS(), yps2);
			if (bOk1 && bOk2)
			{
				if (yps1 < yps2)
					nRet = -1;
				else if (yps1 > yps2)
					nRet = 1;
			}
			else if (bOk1)
				nRet = 1;
			else if (bOk2)
				nRet = -1;
		}
		break;
	case IO_RUNS_YPS:
		{
			double yps1, yps2;
			bool bOk1 = pRun1->m_pRun->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), yps1);
			bool bOk2 = pRun2->m_pRun->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), yps2);
			if (bOk1 && bOk2)
			{
				if (yps1 < yps2)
					nRet = -1;
				else if (yps1 > yps2)
					nRet = 1;
			}
			else if (bOk1)
				nRet = 1;
			else if (bOk2)
				nRet = -1;
		}
		break;
	case IO_RUNS_SCT:
		{
			bool bOk1 = ARBDogRunScoring::eTypeByTime == pRun1->m_pRun->GetScoring().GetType()
				&& 0.0 < pRun1->m_pRun->GetScoring().GetSCT();
			bool bOk2 = ARBDogRunScoring::eTypeByTime == pRun2->m_pRun->GetScoring().GetType()
				&& 0.0 < pRun2->m_pRun->GetScoring().GetSCT();
			if (bOk1 && bOk2)
			{
				if (pRun1->m_pRun->GetScoring().GetSCT() < pRun2->m_pRun->GetScoring().GetSCT())
					nRet = -1;
				else if (pRun1->m_pRun->GetScoring().GetSCT() > pRun2->m_pRun->GetScoring().GetSCT())
					nRet = 1;
			}
			else if (bOk1)
				nRet = 1;
			else if (bOk2)
				nRet = -1;
		}
		break;
	case IO_RUNS_TOTAL_FAULTS:
		{
			bool bOk1 = ARBDogRunScoring::eTypeByTime == pRun1->m_pRun->GetScoring().GetType();
			bool bOk2 = ARBDogRunScoring::eTypeByTime == pRun2->m_pRun->GetScoring().GetType();
			if (bOk1 && bOk2)
			{
				ARBConfigScoringPtr pScoring1;
				if (pRun1->m_pTrial->GetClubs().GetPrimaryClub())
					pRun1->m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						pRun1->m_pTrial->GetClubs().GetPrimaryClubVenue(),
						pRun1->m_pRun->GetEvent(),
						pRun1->m_pRun->GetDivision(),
						pRun1->m_pRun->GetLevel(),
						pRun1->m_pRun->GetDate(),
						NULL,
						&pScoring1);
				ARBConfigScoringPtr pScoring2;
				if (pRun2->m_pTrial->GetClubs().GetPrimaryClub())
					pRun2->m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						pRun2->m_pTrial->GetClubs().GetPrimaryClubVenue(),
						pRun2->m_pRun->GetEvent(),
						pRun2->m_pRun->GetDivision(),
						pRun2->m_pRun->GetLevel(),
						pRun2->m_pRun->GetDate(),
						NULL,
						&pScoring2);
				double faults1 = pRun1->m_pRun->GetScoring().GetCourseFaults() + pRun1->m_pRun->GetScoring().GetTimeFaults(pScoring1);
				double faults2 = pRun2->m_pRun->GetScoring().GetCourseFaults() + pRun2->m_pRun->GetScoring().GetTimeFaults(pScoring2);
				if (faults1 < faults2)
					nRet = -1;
				else if (faults1 > faults2)
					nRet = 1;
			}
			else if (bOk1)
				nRet = 1;
			else if (bOk2)
				nRet = -1;
		}
		break;
	case IO_RUNS_REQ_OPENING:
		{
			bool bOk1 = ARBDogRunScoring::eTypeByOpenClose == pRun1->m_pRun->GetScoring().GetType();
			bool bOk2 = ARBDogRunScoring::eTypeByOpenClose == pRun2->m_pRun->GetScoring().GetType();
			if (bOk1 && bOk2)
			{
				if (pRun1->m_pRun->GetScoring().GetNeedOpenPts() < pRun2->m_pRun->GetScoring().GetNeedOpenPts())
					nRet = -1;
				else if (pRun1->m_pRun->GetScoring().GetNeedOpenPts() > pRun2->m_pRun->GetScoring().GetNeedOpenPts())
					nRet = 1;
			}
			else if (bOk1)
				nRet = 1;
			else if (bOk2)
				nRet = -1;
		}
		break;
	case IO_RUNS_REQ_CLOSING:
		{
			bool bOk1 = ARBDogRunScoring::eTypeByOpenClose == pRun1->m_pRun->GetScoring().GetType();
			bool bOk2 = ARBDogRunScoring::eTypeByOpenClose == pRun2->m_pRun->GetScoring().GetType();
			if (bOk1 && bOk2)
			{
				if (pRun1->m_pRun->GetScoring().GetNeedClosePts() < pRun2->m_pRun->GetScoring().GetNeedClosePts())
					nRet = -1;
				else if (pRun1->m_pRun->GetScoring().GetNeedClosePts() > pRun2->m_pRun->GetScoring().GetNeedClosePts())
					nRet = 1;
			}
			else if (bOk1)
				nRet = 1;
			else if (bOk2)
				nRet = -1;
		}
		break;
	case IO_RUNS_OPENING:
		{
			bool bOk1 = ARBDogRunScoring::eTypeByOpenClose == pRun1->m_pRun->GetScoring().GetType();
			bool bOk2 = ARBDogRunScoring::eTypeByOpenClose == pRun2->m_pRun->GetScoring().GetType();
			if (bOk1 && bOk2)
			{
				if (pRun1->m_pRun->GetScoring().GetOpenPts() < pRun2->m_pRun->GetScoring().GetOpenPts())
					nRet = -1;
				else if (pRun1->m_pRun->GetScoring().GetOpenPts() > pRun2->m_pRun->GetScoring().GetOpenPts())
					nRet = 1;
			}
			else if (bOk1)
				nRet = 1;
			else if (bOk2)
				nRet = -1;
		}
		break;
	case IO_RUNS_CLOSING:
		{
			bool bOk1 = ARBDogRunScoring::eTypeByOpenClose == pRun1->m_pRun->GetScoring().GetType();
			bool bOk2 = ARBDogRunScoring::eTypeByOpenClose == pRun2->m_pRun->GetScoring().GetType();
			if (bOk1 && bOk2)
			{
				if (pRun1->m_pRun->GetScoring().GetClosePts() < pRun2->m_pRun->GetScoring().GetClosePts())
					nRet = -1;
				else if (pRun1->m_pRun->GetScoring().GetClosePts() > pRun2->m_pRun->GetScoring().GetClosePts())
					nRet = 1;
			}
			else if (bOk1)
				nRet = 1;
			else if (bOk2)
				nRet = -1;
		}
		break;
	case IO_RUNS_REQ_POINTS:
		{
			bool bOk1 = ARBDogRunScoring::eTypeByPoints == pRun1->m_pRun->GetScoring().GetType();
			bool bOk2 = ARBDogRunScoring::eTypeByPoints == pRun2->m_pRun->GetScoring().GetType();
			if (bOk1 && bOk2)
			{
				if (pRun1->m_pRun->GetScoring().GetNeedOpenPts() < pRun2->m_pRun->GetScoring().GetNeedOpenPts())
					nRet = -1;
				else if (pRun1->m_pRun->GetScoring().GetNeedOpenPts() > pRun2->m_pRun->GetScoring().GetNeedOpenPts())
					nRet = 1;
			}
			else if (bOk1)
				nRet = 1;
			else if (bOk2)
				nRet = -1;
		}
		break;
	case IO_RUNS_POINTS:
		{
			bool bOk1 = ARBDogRunScoring::eTypeByPoints == pRun1->m_pRun->GetScoring().GetType();
			bool bOk2 = ARBDogRunScoring::eTypeByPoints == pRun2->m_pRun->GetScoring().GetType();
			if (bOk1 && bOk2)
			{
				if (pRun1->m_pRun->GetScoring().GetOpenPts() < pRun2->m_pRun->GetScoring().GetOpenPts())
					nRet = -1;
				else if (pRun1->m_pRun->GetScoring().GetOpenPts() > pRun2->m_pRun->GetScoring().GetOpenPts())
					nRet = 1;
			}
			else if (bOk1)
				nRet = 1;
			else if (bOk2)
				nRet = -1;
		}
		break;
	case IO_RUNS_PLACE:
		{
			short p1 = pRun1->m_pRun->GetPlace();
			short p2 = pRun2->m_pRun->GetPlace();
			if (p1 > p2)
			{
				if (p2 <= 0)
					nRet = -1;
				else
					nRet = 1;
			}
			else if (p1 < p2)
			{
				if (p1 <= 0)
					nRet = 1;
				else
					nRet = -1;
			}
		}
		break;
	case IO_RUNS_IN_CLASS:
		{
			short p1 = pRun1->m_pRun->GetInClass();
			short p2 = pRun2->m_pRun->GetInClass();
			if (p1 < p2)
				nRet = -1;
			else if (p1 > p2)
				nRet = 1;
		}
		break;
	case IO_RUNS_DOGSQD:
		{
			short p1 = pRun1->m_pRun->GetDogsQd();
			short p2 = pRun2->m_pRun->GetDogsQd();
			if (p1 < p2)
				nRet = -1;
			else if (p1 > p2)
				nRet = 1;
		}
		break;
	case IO_RUNS_Q:
		if (pRun1->m_pRun->GetQ() < pRun2->m_pRun->GetQ())
			nRet = -1;
		else if (pRun1->m_pRun->GetQ() > pRun2->m_pRun->GetQ())
			nRet = 1;
		break;
	case IO_RUNS_SCORE:
		{
			bool bOk1 = (pRun1->m_pRun->GetQ().Qualified() || ARB_Q::eQ_NQ == pRun1->m_pRun->GetQ());
			bool bOk2 = (pRun2->m_pRun->GetQ().Qualified() || ARB_Q::eQ_NQ == pRun2->m_pRun->GetQ());
			if (bOk1 && bOk2)
			{
				ARBConfigScoringPtr pScoring1;
				if (pRun1->m_pTrial->GetClubs().GetPrimaryClub())
					pRun1->m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						pRun1->m_pTrial->GetClubs().GetPrimaryClubVenue(),
						pRun1->m_pRun->GetEvent(),
						pRun1->m_pRun->GetDivision(),
						pRun1->m_pRun->GetLevel(),
						pRun1->m_pRun->GetDate(),
						NULL,
						&pScoring1);
				ARBConfigScoringPtr pScoring2;
				if (pRun2->m_pTrial->GetClubs().GetPrimaryClub())
					pRun2->m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						pRun2->m_pTrial->GetClubs().GetPrimaryClubVenue(),
						pRun2->m_pRun->GetEvent(),
						pRun2->m_pRun->GetDivision(),
						pRun2->m_pRun->GetLevel(),
						pRun2->m_pRun->GetDate(),
						NULL,
						&pScoring2);
				if (pScoring1 && pScoring2)
				{
					if (pRun1->m_pRun->GetScore(pScoring1) < pRun2->m_pRun->GetScore(pScoring2))
						nRet = -1;
					else if (pRun1->m_pRun->GetScore(pScoring1) > pRun2->m_pRun->GetScore(pScoring2))
						nRet = 1;
				}
				else if (pScoring1)
					nRet = -1;
				else if (pScoring2)
					nRet = 1;
			}
			else if (bOk1)
				nRet = 1;
			else if (bOk2)
				nRet = -1;
		}
		break;
	case IO_RUNS_TITLE_POINTS:
		{
			double pts1 = 0;
			double pts2 = 0;
			if (pRun1->m_pRun->GetQ().Qualified())
			{
				ARBConfigScoringPtr pScoring;
				if (pRun1->m_pTrial->GetClubs().GetPrimaryClub())
					 pRun1->m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						pRun1->m_pTrial->GetClubs().GetPrimaryClubVenue(),
						pRun1->m_pRun->GetEvent(),
						pRun1->m_pRun->GetDivision(),
						pRun1->m_pRun->GetLevel(),
						pRun1->m_pRun->GetDate(),
						NULL,
						&pScoring);
				if (pScoring)
					pts1 = pRun1->m_pRun->GetTitlePoints(pScoring);
			}
			if (pRun2->m_pRun->GetQ().Qualified())
			{
				ARBConfigScoringPtr pScoring;
				if (pRun2->m_pTrial->GetClubs().GetPrimaryClub())
					pRun2->m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						pRun2->m_pTrial->GetClubs().GetPrimaryClubVenue(),
						pRun2->m_pRun->GetEvent(),
						pRun2->m_pRun->GetDivision(),
						pRun2->m_pRun->GetLevel(),
						pRun2->m_pRun->GetDate(),
						NULL,
						&pScoring);
				if (pScoring)
					pts2 = pRun2->m_pRun->GetTitlePoints(pScoring);
			}
			if (pts1 < pts2)
				nRet = -1;
			else if (pts1 > pts2)
				nRet = 1;
		}
		break;
	case IO_RUNS_COMMENTS:
		if (pRun1->m_pRun->GetNote() < pRun2->m_pRun->GetNote())
			nRet = -1;
		else if (pRun1->m_pRun->GetNote() > pRun2->m_pRun->GetNote())
			nRet = 1;
		break;
	case IO_RUNS_FAULTS:
		{
			ARBString str1, str2;
			ARBDogFaultList::const_iterator iter;
			int i = 0;
			for (iter = pRun1->m_pRun->GetFaults().begin();
				iter != pRun1->m_pRun->GetFaults().end();
				++i, ++iter)
			{
				if (0 < i)
					str1 += _T(", ");
				str1 += *iter;
			}
			for (i = 0, iter = pRun2->m_pRun->GetFaults().begin();
				iter != pRun2->m_pRun->GetFaults().end();
				++i, ++iter)
			{
				if (0 < i)
					str2 += _T(", ");
				str2 += *iter;
			}
			if (str1 < str2)
				nRet = -1;
			else if (str1 > str2)
				nRet = 1;
		}
		break;
	case IO_RUNS_SPEED:
		{
			ARBConfigScoringPtr pScoring1;
			if (pRun1->m_pTrial->GetClubs().GetPrimaryClub())
				pRun1->m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
					pRun1->m_pTrial->GetClubs().GetPrimaryClubVenue(),
					pRun1->m_pRun->GetEvent(),
					pRun1->m_pRun->GetDivision(),
					pRun1->m_pRun->GetLevel(),
					pRun1->m_pRun->GetDate(),
					NULL,
					&pScoring1);
			ARBConfigScoringPtr pScoring2;
			if (pRun2->m_pTrial->GetClubs().GetPrimaryClub())
				pRun2->m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
					pRun2->m_pTrial->GetClubs().GetPrimaryClubVenue(),
					pRun2->m_pRun->GetEvent(),
					pRun2->m_pRun->GetDivision(),
					pRun2->m_pRun->GetLevel(),
					pRun2->m_pRun->GetDate(),
					NULL,
					&pScoring2);
			int pts1 = -1;
			int pts2 = -1;
			if (pScoring1 && pScoring2)
			{
				if (pScoring1->HasSpeedPts())
					pts1 = pRun1->m_pRun->GetSpeedPoints(pScoring1);
				if (pScoring2->HasSpeedPts())
					pts2 = pRun2->m_pRun->GetSpeedPoints(pScoring2);
			}
			if (pts1 > pts2)
				nRet = 1;
			else if (pts1 < pts2)
				nRet = -1;
		}
		break;
	}
	if (0 > sortInfo->nCol)
		nRet *= -1;
	return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// Find

bool CFindRuns::Search(CDlgFind* pDlg) const
{
	bool bFound = false;
	int inc = 1;
	if (!SearchDown())
		inc = -1;
	int index = m_pView->GetSelection();
	if (0 <= index && index < m_pView->GetListCtrl().GetItemCount())
	{
		index += inc;
	}
	else if (0 > index && SearchDown())
		index = 0;
	else if (index >= m_pView->GetListCtrl().GetItemCount() && !SearchDown())
		index = m_pView->GetListCtrl().GetItemCount() - 1;
	CString search = Text();
	if (!MatchCase())
		search.MakeLower();
	for (; !bFound && 0 <= index && index < m_pView->GetListCtrl().GetItemCount(); index += inc)
	{
		std::set<ARBString> strings;
		if (SearchAll())
		{
			CAgilityBookViewRunsData* pData = m_pView->GetItemData(index);
			if (pData)
				pData->GetRun()->GetSearchStrings(strings);
		}
		else
		{
			int nColumns = m_pView->HeaderItemCount();
			for (int i = 0; i < nColumns; ++i)
			{
				strings.insert((LPCTSTR)m_pView->GetListCtrl().GetItemText(index, i));
			}
		}
		for (std::set<ARBString>::iterator iter = strings.begin(); iter != strings.end(); ++iter)
		{
			CString str((*iter).c_str());
			if (!MatchCase())
				str.MakeLower();
			if (0 <= str.Find(search))
			{
				m_pView->SetSelection(index, true);
				m_pView->GetListCtrl().EnsureVisible(index, FALSE);
				bFound = true;
			}
		}
	}
	if (!bFound)
	{
		CString msg;
		msg.FormatMessage(IDS_CANNOT_FIND, (LPCTSTR)m_strSearch);
		AfxMessageBox(msg, MB_ICONINFORMATION);
	}
	return bFound;
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewRuns

IMPLEMENT_DYNCREATE(CAgilityBookViewRuns, CListView2)

BEGIN_MESSAGE_MAP(CAgilityBookViewRuns, CListView2)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	//{{AFX_MSG_MAP(CAgilityBookViewRuns)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_FIND_NEXT, OnEditFindNext)
	ON_COMMAND(ID_EDIT_FIND_PREVIOUS, OnEditFindPrevious)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_EDIT_RUN, OnUpdateAgilityEditRun)
	ON_COMMAND(ID_AGILITY_EDIT_RUN, OnAgilityEditRun)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_NEW_TITLE, OnUpdateAgilityNewTitle)
	ON_COMMAND(ID_AGILITY_NEW_TITLE, OnAgilityNewTitle)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_NEW_TRIAL, OnUpdateAgilityNewTrial)
	ON_COMMAND(ID_AGILITY_NEW_TRIAL, OnAgilityNewTrial)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_NEW_RUN, OnUpdateAgilityNewRun)
	ON_COMMAND(ID_AGILITY_NEW_RUN, OnAgilityNewRun)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_AGILITY_DELETE_RUN, OnUpdateAgilityDeleteRun)
	ON_COMMAND(ID_AGILITY_DELETE_RUN, OnAgilityDeleteRun)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, OnViewCustomize)
	ON_COMMAND(ID_VIEW_SORTRUNS, OnViewSortruns)
	ON_COMMAND(ID_VIEW_RUNS_BY_TRIAL, OnViewRunsByTrial)
	ON_COMMAND(ID_VIEW_TABLE_IN_YPS, OnViewTableInYPS)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CAgilityBookViewRuns construction/destruction

#pragma warning (push)
#pragma warning ( disable : 4355 )
CAgilityBookViewRuns::CAgilityBookViewRuns()
	: m_bSuppressSelect(false)
	, m_Callback(this)
	, m_SortColumn(m_Columns)
{
	SetAutoDelete(true);
	m_ImageList.Create(16, 16, ILC_MASK | ILC_COLOR32, 2, 0);
	CWinApp* app = AfxGetApp();
	m_imgCourse = m_ImageList.Add(app->LoadIcon(IDI_AGILITYBOOKDOC));
	m_imgMap = m_ImageList.Add(app->LoadIcon(IDI_CRCD));
}
#pragma warning (pop)

CAgilityBookViewRuns::~CAgilityBookViewRuns()
{
}

BOOL CAgilityBookViewRuns::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS;
	return CListView2::PreCreateWindow(cs);
}

LRESULT CAgilityBookViewRuns::OnCommandHelp(WPARAM, LPARAM)
{
	AfxGetApp()->WinHelp(HID_BASE_RESOURCE+IDR_RUN, HH_HELP_CONTEXT);
	return 1;
}

int CAgilityBookViewRuns::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView2::OnCreate(lpCreateStruct) == -1)
		return -1;
	GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	GetListCtrl().SetImageList(&m_ImageList, LVSIL_SMALL);
	return 0;
}

void CAgilityBookViewRuns::OnInitialUpdate()
{
	SetupColumns();
	CListView2::OnInitialUpdate();
}

void CAgilityBookViewRuns::OnActivateView(
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

void CAgilityBookViewRuns::OnUpdate(
		CView* pSender,
		LPARAM lHint,
		CObject* pHint)
{
	if (0 == lHint || (UPDATE_RUNS_VIEW & lHint) || (UPDATE_OPTIONS & lHint) || (UPDATE_CONFIG & lHint))
		LoadData();
}

#ifdef _DEBUG
// CAgilityBookViewRuns diagnostics
void CAgilityBookViewRuns::AssertValid() const
{
	CListView2::AssertValid();
}

void CAgilityBookViewRuns::Dump(CDumpContext& dc) const
{
	CListView2::Dump(dc);
}

CAgilityBookDoc* CAgilityBookViewRuns::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAgilityBookDoc)));
	return reinterpret_cast<CAgilityBookDoc*>(m_pDocument);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Printing

void CAgilityBookViewRuns::GetPrintLine(
		int nItem,
		CStringArray& line)
{
	CListView2::GetPrintLine(nItem, line);
}

/////////////////////////////////////////////////////////////////////////////

bool CAgilityBookViewRuns::IsFiltered() const
{
	return CFilterOptions::Options().IsFilterEnabled();
}

bool CAgilityBookViewRuns::GetMessage(CString& msg) const
{
	int nQs = 0;
	for (int index = 0; index < GetListCtrl().GetItemCount(); ++index)
	{
		CAgilityBookViewRunsData* pData = GetItemData(index);
		if (pData && pData->GetRun() && pData->GetRun()->GetQ().Qualified())
			++nQs;
	}
	msg.FormatMessage(IDS_NUM_RUNS_QS, GetListCtrl().GetItemCount(), nQs);
	return true;
}

bool CAgilityBookViewRuns::GetMessage2(CString& msg) const
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

CAgilityBookViewRunsData* CAgilityBookViewRuns::GetItemData(int index) const
{
	return dynamic_cast<CAgilityBookViewRunsData*>(GetData(index));
}

void CAgilityBookViewRuns::SetupColumns()
{
	int nColumnCount = HeaderItemCount();
	for (int i = 0; i < nColumnCount; ++i)
		DeleteColumn(0);
	if (CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eViewRuns, IO_TYPE_VIEW_RUNS_LIST, m_Columns))
	{
		LV_COLUMN col;
		col.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM;
		for (size_t iCol = 0; iCol <= m_Columns.size(); ++iCol)
		{
			CString str(_T(""));
			if (0 < iCol)
			{
				str = CDlgAssignColumns::GetNameFromColumnID(m_Columns[iCol-1]);
				col.fmt = CDlgAssignColumns::GetFormatFromColumnID(m_Columns[iCol-1]);
			}
			col.pszText = str.GetBuffer(0);
			col.iSubItem = static_cast<int>(iCol);
			InsertColumn(static_cast<int>(iCol), &col);
			str.ReleaseBuffer();
		}
		m_SortColumn.Initialize();
	}
}

void CAgilityBookViewRuns::LoadData()
{
	m_bSuppressSelect = true;
	
	CWaitCursor wait;

	// Mirror the selection in the tree here.
	ARBDogRunPtr pCurRun = GetDocument()->GetCurrentRun();

	// Reduce flicker.
	GetListCtrl().SetRedraw(FALSE);

	// Clear everything.
	GetListCtrl().DeleteAllItems();

	// Add items.
	std::vector<CVenueFilter> venues;
	CFilterOptions::Options().GetFilterVenue(venues);
	list<ARBDogTrialPtr> trials;
	ARBDogPtr pDog = GetDocument()->GetCurrentDog();
	ARBDogTrialPtr pCurTrial = GetDocument()->GetCurrentTrial();
	if (pCurTrial && CAgilityBookOptions::GetViewRunsByTrial())
		trials.push_back(pCurTrial);
	else
	{
		if (pDog)
		{
			for (ARBDogTrialList::iterator iter = pDog->GetTrials().begin();
			iter != pDog->GetTrials().end();
			++iter)
			{
				ARBDogTrialPtr pTrial = (*iter);
				if (!pTrial->IsFiltered())
					trials.push_back(pTrial);
			}
		}
	}
	int nQs = 0;
	if (0 < trials.size())
	{
		int i = 0;
		for (list<ARBDogTrialPtr>::iterator iter = trials.begin(); iter != trials.end(); ++iter)
		{
			ARBDogTrialPtr pTrial = (*iter);
			for (ARBDogRunList::iterator iterRun = pTrial->GetRuns().begin();
			iterRun != pTrial->GetRuns().end();
			++i, ++iterRun)
			{
				ARBDogRunPtr pRun = (*iterRun);
				if (pRun->IsFiltered())
					continue;
				if (pRun->GetQ().Qualified())
					++nQs;
				LV_ITEM item;
				item.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
				item.pszText = LPSTR_TEXTCALLBACK;
				item.iImage = I_IMAGECALLBACK;
				item.iItem = i;
				item.iSubItem = 0;
				item.lParam = reinterpret_cast<LPARAM>(
					static_cast<CListData*>(
						new CAgilityBookViewRunsData(this, pDog, pTrial, pRun)));
				int index = GetListCtrl().InsertItem(&item);
				// Compare by ptr, not value.
				if (pCurRun && pCurRun == pRun)
				{
					SetSelection(index, true);
				}
			}
		}
	}
	int i;
	int nColumnCount = HeaderItemCount();
	for (i = 0; i < nColumnCount; ++i)
		GetListCtrl().SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);

	CString msg;
	if (IsWindowVisible())
	{
		if (GetMessage(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText(msg, IsFiltered());
		if (GetMessage2(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText2(msg);
	}

	SORT_RUN_INFO info;
	info.pThis = this;
	info.nCol = m_SortColumn.GetColumn();
	GetListCtrl().SortItems(CompareRuns, reinterpret_cast<LPARAM>(&info));
	if (0 == m_SortColumn.GetColumn())
		HeaderSort(0, CHeaderCtrl2::eNoSort);
	else
		HeaderSort(abs(m_SortColumn.GetColumn()),
			info.nCol > 0 ? CHeaderCtrl2::eAscending : CHeaderCtrl2::eDescending);
	// Now make sure the selected item is visible.
	GetListCtrl().EnsureVisible(GetSelection(), FALSE);

	// Cleanup.
	GetListCtrl().SetRedraw(TRUE);
	GetListCtrl().Invalidate();

	m_bSuppressSelect = false;
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewRuns message handlers

void CAgilityBookViewRuns::OnRclick(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	// Send WM_CONTEXTMENU to self (done according to Q222905)
	SendMessage(WM_CONTEXTMENU, reinterpret_cast<WPARAM>(m_hWnd), GetMessagePos());
	*pResult = 1;
}

void CAgilityBookViewRuns::OnContextMenu(
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
	UINT idMenu = IDR_RUN;
	if (0 != idMenu)
	{
		CMenu menu;
		menu.LoadMenu(idMenu);
		CMenu* pMenu = menu.GetSubMenu(0);
		ASSERT(pMenu != NULL);
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	}
}

void CAgilityBookViewRuns::OnColumnclick(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	if (0 != pNMListView->iSubItem)
	{
		HeaderSort(abs(m_SortColumn.GetColumn()), CHeaderCtrl2::eNoSort);
		int nBackwards = 1;
		if (m_SortColumn.GetColumn() == pNMListView->iSubItem)
			nBackwards = -1;
		m_SortColumn.SetColumn(pNMListView->iSubItem * nBackwards);
		SORT_RUN_INFO info;
		info.pThis = this;
		info.nCol = m_SortColumn.GetColumn();
		GetListCtrl().SortItems(CompareRuns, reinterpret_cast<LPARAM>(&info));
		HeaderSort(abs(m_SortColumn.GetColumn()),
			nBackwards > 0 ? CHeaderCtrl2::eAscending : CHeaderCtrl2::eDescending);
	}
	*pResult = 0;
}

void CAgilityBookViewRuns::OnGetdispinfo(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
	CListData* pRawData = reinterpret_cast<CListData*>(pDispInfo->item.lParam);
	CAgilityBookViewRunsData* pData = dynamic_cast<CAgilityBookViewRunsData*>(pRawData);
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		ARBString str = pData->OnNeedText(pDispInfo->item.iSubItem);
		::lstrcpyn(pDispInfo->item.pszText, str.c_str(), pDispInfo->item.cchTextMax);
		pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
	}
	if (pDispInfo->item.mask & LVIF_IMAGE)
	{
		pDispInfo->item.iImage = pData->OnNeedIcon();
	}
	*pResult = 0;
}

void CAgilityBookViewRuns::OnItemchanged(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	// I only care about the item being selected.
	if (0 <= pNMListView->iItem
	&& (LVIF_STATE & pNMListView->uChanged)
	&& !(LVIS_SELECTED & pNMListView->uOldState)
	&& (LVIS_SELECTED & pNMListView->uNewState))
	{
		if (!m_bSuppressSelect && 1 == GetListCtrl().GetSelectedCount())
		{
			CListData* pRawData = reinterpret_cast<CListData*>(pNMListView->lParam);
			CAgilityBookViewRunsData* pData = dynamic_cast<CAgilityBookViewRunsData*>(pRawData);
			if (pData)
			{
				CAgilityBookTreeData* pTreeData = GetDocument()->GetTreeView()->FindData(TVI_ROOT, pData->GetRun());
				if (pTreeData)
				{
					GetDocument()->GetTreeView()->SuppressSelect(true);
					GetDocument()->GetTreeView()->GetTreeCtrl().SelectItem(pTreeData->GetHTreeItem());
					GetDocument()->GetTreeView()->SuppressSelect(false);
				}
			}
		}
	}
	*pResult = 0;
}

void CAgilityBookViewRuns::OnDblclk(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection(true));
	if (pData)
		GetDocument()->EditRun(pData->GetRun());
	else
		MessageBeep(0);
	*pResult = 0;
}

void CAgilityBookViewRuns::OnKeydown(
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
			CAgilityBookViewRunsData* pData = GetItemData(GetSelection(true));
			if (pData)
				GetDocument()->EditRun(pData->GetRun());
		}
		break;
	}
	*pResult = 0;
}

void CAgilityBookViewRuns::OnEditFind()
{
	CDlgFind dlg(m_Callback, this);
	dlg.DoModal();
}

void CAgilityBookViewRuns::OnEditFindNext()
{
	m_Callback.SearchDown(true);
	if (m_Callback.Text().IsEmpty())
		OnEditFind();
	else
		m_Callback.Search(NULL);
}

void CAgilityBookViewRuns::OnEditFindPrevious()
{
	m_Callback.SearchDown(false);
	if (m_Callback.Text().IsEmpty())
		OnEditFind();
	else
		m_Callback.Search(NULL);
}

void CAgilityBookViewRuns::OnUpdateAgilityEditRun(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection(true));
	if (pData)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewRuns::OnAgilityEditRun() 
{
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection(true));
	if (pData)
		GetDocument()->EditRun(pData->GetRun());
}

void CAgilityBookViewRuns::OnUpdateAgilityNewTitle(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection(true));
	if (pData)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewRuns::OnAgilityNewTitle()
{
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection(true));
	if (pData)
		GetDocument()->AddTitle(pData->GetRun());
}

void CAgilityBookViewRuns::OnUpdateAgilityNewTrial(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection(true));
	if (pData)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewRuns::OnAgilityNewTrial()
{
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection(true));
	if (pData)
		GetDocument()->AddTrial(pData->GetRun());
}

void CAgilityBookViewRuns::OnUpdateAgilityNewRun(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection(true));
	if (pData)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewRuns::OnAgilityNewRun()
{
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection(true));
	if (pData)
		GetDocument()->AddRun(pData->GetRun());
}

void CAgilityBookViewRuns::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	if (1 == GetListCtrl().GetSelectedCount())
		OnUpdateEditCopy(pCmdUI);
	else
		pCmdUI->Enable(FALSE);
}

void CAgilityBookViewRuns::OnEditCut()
{
	if (1 == GetListCtrl().GetSelectedCount())
	{
		OnEditCopy();
		OnAgilityDeleteRun();
	}
	else
		MessageBeep(0);
}

void CAgilityBookViewRuns::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	if (0 < GetListCtrl().GetSelectedCount())
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewRuns::OnEditCopy()
{
	std::vector<int> indices;
	if (0 < GetSelection(indices))
	{
		CClipboardDataWriter clpData;
		if (!clpData.isOkay())
			return;

		CString data;
		CStringArray line;

		// Take care of the header, but only if more than one line is selected.
		if (1 < indices.size()
		|| indices.size() == static_cast<size_t>(GetListCtrl().GetItemCount()))
		{
			GetPrintLine(-1, line);
			for (int i = 0; i < line.GetSize(); ++i)
			{
				if (0 < i)
					data += '\t';
				data += line[i];
			}
			data += _T("\r\n");
		}

		Element tree(CLIPDATA);

		// Now all the data.
		for (std::vector<int>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
		{
			CAgilityBookViewRunsData* pData = GetItemData(*iter);
			if (pData)
				pData->GetRun()->Save(tree);
			CStringArray line2;
			GetPrintLine((*iter), line2);
			for (int i = 0; i < line2.GetSize(); ++i)
			{
				if (0 < i)
					data += '\t';
				data += line2[i];
			}
			data += _T("\r\n");
		}

		clpData.SetData(eFormatRun, tree);
		clpData.SetData(data);
	}
}

void CAgilityBookViewRuns::OnUpdateAgilityDeleteRun(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection(true));
	if (pData)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewRuns::OnAgilityDeleteRun() 
{
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection(true));
	if (pData)
		GetDocument()->DeleteRun(pData->GetRun());
}

void CAgilityBookViewRuns::OnViewCustomize()
{
	CDlgAssignColumns dlg(CAgilityBookOptions::eViewRuns);
	if (IDOK == dlg.DoModal())
	{
		SetupColumns();
		LoadData();
	}
}

void CAgilityBookViewRuns::OnViewSortruns() 
{
	CAgilityBookOptions::SetNewestDatesFirst(!CAgilityBookOptions::GetNewestDatesFirst());
	GetDocument()->SortDates();
	// Note, sorting the trials doesn't affect this view.
	GetDocument()->UpdateAllViews(NULL, UPDATE_TREE_VIEW);
}

void CAgilityBookViewRuns::OnViewRunsByTrial()
{
	CAgilityBookOptions::SetViewRunsByTrial(!CAgilityBookOptions::GetViewRunsByTrial());
	LoadData();
}

void CAgilityBookViewRuns::OnViewTableInYPS()
{
	CAgilityBookOptions::SetTableInYPS(!CAgilityBookOptions::GetTableInYPS());
	AfxGetMainWnd()->Invalidate();
}
