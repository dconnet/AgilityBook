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
 * @brief implementation of the CAgilityBookViewRuns class
 * @author David Connet
 *
 * Revision History
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
#include "MainFrm.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewRunsData

class CAgilityBookViewRunsData
{
	friend int CALLBACK CompareRuns(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3);
public:
	CAgilityBookViewRunsData(CAgilityBookViewRuns* pView, ARBDog* pDog, ARBDogTrial* pTrial, ARBDogRun* pRun)
		: m_RefCount(1)
		, m_pView(pView)
		, m_pDog(pDog)
		, m_pTrial(pTrial)
		, m_pRun(pRun)
	{
		if (m_pDog)
			m_pDog->AddRef();
		if (m_pTrial)
			m_pTrial->AddRef();
		if (m_pRun)
			m_pRun->AddRef();
	}

	void AddRef();
	void Release();

	ARBDogTrial* GetTrial()		{return m_pTrial;}
	ARBDogRun* GetRun()			{return m_pRun;}
	CString OnNeedText(int iCol) const;
	int OnNeedIcon() const;

protected:
	~CAgilityBookViewRunsData()
	{
		if (m_pDog)
			m_pDog->Release();
		if (m_pTrial)
			m_pTrial->Release();
		if (m_pRun)
			m_pRun->Release();
	}
	UINT m_RefCount;
	CAgilityBookViewRuns* m_pView;
	ARBDog* m_pDog;
	ARBDogTrial* m_pTrial;
	ARBDogRun* m_pRun;
};

void CAgilityBookViewRunsData::AddRef()
{
	++m_RefCount;
}

void CAgilityBookViewRunsData::Release()
{
	--m_RefCount;
	if (0 == m_RefCount)
		delete this;
}

CString CAgilityBookViewRunsData::OnNeedText(int iCol) const
{
	short val;
	CString str;
	if (m_pRun)
	{
		// Col 0 is special: it has the icons. Instead of saving it,
		// we simply ignore it - so iCol is always off by 1.
		switch (m_pView->m_Columns[iCol-1])
		{
		default:
			break;

		case IO_RUNS_REG_NAME:
			str = m_pDog->GetRegisteredName().c_str();
			break;
		case IO_RUNS_CALL_NAME:
			str = m_pDog->GetCallName().c_str();
			break;
		case IO_RUNS_DATE:
			str = m_pRun->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::eRunList)).c_str();
			break;
		case IO_RUNS_VENUE:
			{
				str.Empty();
				int i = 0;
				for (ARBDogClubList::const_iterator iter = m_pTrial->GetClubs().begin();
					iter != m_pTrial->GetClubs().end();
					++iter, ++i)
				{
					if (0 < i)
						str += "/";
					str += (*iter)->GetVenue().c_str();
				}
			}
			break;
		case IO_RUNS_CLUB:
			{
				str.Empty();
				int i = 0;
				for (ARBDogClubList::const_iterator iter = m_pTrial->GetClubs().begin();
					iter != m_pTrial->GetClubs().end();
					++iter, ++i)
				{
					if (0 < i)
						str += "/";
					str += (*iter)->GetName().c_str();
				}
			}
			break;
		case IO_RUNS_LOCATION:
			str = m_pTrial->GetLocation().c_str();
			break;
		case IO_RUNS_TRIAL_NOTES:
			str = m_pTrial->GetNote().c_str();
			break;
		case IO_RUNS_DIVISION:
			str = m_pRun->GetDivision().c_str();
			break;
		case IO_RUNS_LEVEL:
			str = m_pRun->GetLevel().c_str();
			break;
		case IO_RUNS_EVENT:
			str = m_pRun->GetEvent().c_str();
			break;
		case IO_RUNS_HEIGHT:
			str = m_pRun->GetHeight().c_str();
			break;
		case IO_RUNS_JUDGE:
			str = m_pRun->GetJudge().c_str();
			break;
		case IO_RUNS_HANDLER:
			str = m_pRun->GetHandler().c_str();
			break;
		case IO_RUNS_CONDITIONS:
			str = m_pRun->GetConditions().c_str();
			break;
		case IO_RUNS_COURSE_FAULTS:
			str.Format("%hd", m_pRun->GetScoring().GetCourseFaults());
			break;
		case IO_RUNS_TIME:
			str = m_pRun->GetScoring().GetTime().str().c_str();
			break;
		case IO_RUNS_YARDS:
			if (ARBDogRunScoring::eTypeByTime == m_pRun->GetScoring().GetType()
			&& 0.0 < m_pRun->GetScoring().GetYards())
			{
				str.Format("%g", m_pRun->GetScoring().GetYards());
			}
			break;
		case IO_RUNS_YPS:
			{
				double yps;
				if (m_pRun->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), yps))
				{
					str.Format("%.3f", yps);
				}
			}
			break;
		case IO_RUNS_SCT:
			if (ARBDogRunScoring::eTypeByTime == m_pRun->GetScoring().GetType()
			&& 0.0 < m_pRun->GetScoring().GetSCT())
			{
				str = m_pRun->GetScoring().GetSCT().str().c_str();
			}
			break;
		case IO_RUNS_TOTAL_FAULTS:
			if (ARBDogRunScoring::eTypeByTime == m_pRun->GetScoring().GetType())
			{
				ARBConfigScoring const* pScoring = NULL;
				if (m_pTrial->GetClubs().GetPrimaryClub())
                    pScoring = m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						m_pTrial->GetClubs().GetPrimaryClub()->GetVenue(),
						m_pRun->GetEvent(),
						m_pRun->GetDivision(),
						m_pRun->GetLevel(),
						m_pRun->GetDate());
				double faults = m_pRun->GetScoring().GetCourseFaults() + m_pRun->GetScoring().GetTimeFaults(pScoring);
				str.Format("%g", faults);
			}
			break;
		case IO_RUNS_REQ_OPENING:
			if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
			{
				str.Format("%hd", m_pRun->GetScoring().GetNeedOpenPts());
			}
			break;
		case IO_RUNS_REQ_CLOSING:
			if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
			{
				str.Format("%hd", m_pRun->GetScoring().GetNeedClosePts());
			}
			break;
		case IO_RUNS_OPENING:
			if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
			{
				str.Format("%hd", m_pRun->GetScoring().GetOpenPts());
			}
			break;
		case IO_RUNS_CLOSING:
			if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
			{
				str.Format("%hd", m_pRun->GetScoring().GetClosePts());
			}
			break;
		case IO_RUNS_REQ_POINTS:
			if (ARBDogRunScoring::eTypeByPoints == m_pRun->GetScoring().GetType())
			{
				str.Format("%hd", m_pRun->GetScoring().GetNeedOpenPts());
			}
			break;
		case IO_RUNS_POINTS:
			if (ARBDogRunScoring::eTypeByPoints == m_pRun->GetScoring().GetType())
			{
				str.Format("%hd", m_pRun->GetScoring().GetOpenPts());
			}
			break;
		case IO_RUNS_PLACE:
			val = m_pRun->GetPlace();
			if (0 > val)
				str = "?";
			else if (0 == val)
				str = "-";
			else
				str.Format("%hd", val);
			break;
		case IO_RUNS_IN_CLASS:
			val = m_pRun->GetInClass();
			if (0 >= val)
				str = "?";
			else
				str.Format("%hd", val);
			break;
		case IO_RUNS_DOGSQD:
			val = m_pRun->GetDogsQd();
			if (0 > val)
				str = "?";
			else
				str.Format("%hd", m_pRun->GetDogsQd());
			break;
		case IO_RUNS_Q:
			str = m_pRun->GetQ().str().c_str();
			if (m_pRun->GetQ().Qualified())
			{
				if (m_pTrial->HasQQ(
					m_pRun->GetDate(),
					m_pView->GetDocument()->GetConfig(),
					m_pRun->GetDivision(),
					m_pRun->GetLevel()))
				{
					str.LoadString(IDS_QQ);
				}
				if (ARB_Q::eQ_SuperQ == m_pRun->GetQ())
					str.LoadString(IDS_SQ);
			}
			break;
		case IO_RUNS_SCORE:
			if (m_pRun->GetQ().Qualified()
			|| ARB_Q::eQ_NQ == m_pRun->GetQ())
			{
				ARBConfigScoring const* pScoring = NULL;
				if (m_pTrial->GetClubs().GetPrimaryClub())
					pScoring = m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						m_pTrial->GetClubs().GetPrimaryClub()->GetVenue(),
						m_pRun->GetEvent(),
						m_pRun->GetDivision(),
						m_pRun->GetLevel(),
						m_pRun->GetDate());
				if (pScoring)
					str = m_pRun->GetScore(pScoring).str().c_str();
			}
			break;
		case IO_RUNS_TITLE_POINTS:
			{
				short pts = 0;
				if (m_pRun->GetQ().Qualified())
				{
					ARBConfigScoring const* pScoring = NULL;
					if (m_pTrial->GetClubs().GetPrimaryClub())
						pScoring = m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
							m_pTrial->GetClubs().GetPrimaryClub()->GetVenue(),
							m_pRun->GetEvent(),
							m_pRun->GetDivision(),
							m_pRun->GetLevel(),
							m_pRun->GetDate());
					if (pScoring)
						pts = m_pRun->GetTitlePoints(pScoring);
				}
				str.Format("%hd", pts);
			}
			break;
		case IO_RUNS_COMMENTS:
			str = m_pRun->GetNote().c_str();
			str.Replace("\n", " ");
			break;
		case IO_RUNS_FAULTS:
			{
				str.Empty();
				int i = 0;
				for (ARBDogFaultList::const_iterator iter = m_pRun->GetFaults().begin();
					iter != m_pRun->GetFaults().end();
					++i, ++iter)
				{
					if (0 < i)
						str += ", ";
					str += (*iter).c_str();
				}
			}
			break;
		case IO_RUNS_SPEED:
			{
				ARBConfigScoring const* pScoring = NULL;
				if (m_pTrial->GetClubs().GetPrimaryClub())
                    pScoring = m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						m_pTrial->GetClubs().GetPrimaryClub()->GetVenue(),
						m_pRun->GetEvent(),
						m_pRun->GetDivision(),
						m_pRun->GetLevel(),
						m_pRun->GetDate());
				if (pScoring && pScoring->HasSpeedPts())
				{
					int pts = m_pRun->GetSpeedPoints(pScoring);
					str.Format("%d", pts);
				}
			}
			break;
		}
	}
	return str;
}

int CAgilityBookViewRunsData::OnNeedIcon() const
{
	int iImage = -1;
	if (m_pRun)
	{
		if (0 < m_pRun->GetCRCDMetaFile().length())
			iImage = m_pView->m_imgMap;
		else if (0 < m_pRun->GetCRCD().length())
			iImage = m_pView->m_imgCourse;
	}
	return iImage;
}

/////////////////////////////////////////////////////////////////////////////
// List sorting

struct SORT_RUN_INFO
{
	CAgilityBookViewRuns *pThis;
	int nCol;
};

int CALLBACK CompareRuns(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3)
{
	SORT_RUN_INFO* sortInfo = reinterpret_cast<SORT_RUN_INFO*>(lParam3);
	if (!sortInfo)
		return 0;
	CAgilityBookViewRunsData* pRun1 = reinterpret_cast<CAgilityBookViewRunsData*>(lParam1);
	CAgilityBookViewRunsData* pRun2 = reinterpret_cast<CAgilityBookViewRunsData*>(lParam2);
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
			std::string str1, str2;
			int i = 0;
			ARBDogClubList::const_iterator iter;
			for (iter = pRun1->m_pTrial->GetClubs().begin();
				iter != pRun1->m_pTrial->GetClubs().end();
				++iter, ++i)
			{
				if (0 < i)
					str1 += "/";
				str1 += (*iter)->GetVenue();
			}
			for (i = 0, iter = pRun2->m_pTrial->GetClubs().begin();
				iter != pRun2->m_pTrial->GetClubs().end();
				++iter, ++i)
			{
				if (0 < i)
					str2 += "/";
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
			std::string str1, str2;
			int i = 0;
			ARBDogClubList::const_iterator iter;
			for (iter = pRun1->m_pTrial->GetClubs().begin();
				iter != pRun1->m_pTrial->GetClubs().end();
				++iter, ++i)
			{
				if (0 < i)
					str1 += "/";
				str1 += (*iter)->GetName();
			}
			for (i = 0, iter = pRun2->m_pTrial->GetClubs().begin();
				iter != pRun2->m_pTrial->GetClubs().end();
				++iter, ++i)
			{
				if (0 < i)
					str2 += "/";
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
				ARBConfigScoring const* pScoring1 = NULL;
				if (pRun1->m_pTrial->GetClubs().GetPrimaryClub())
					pScoring1 = pRun1->m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						pRun1->m_pTrial->GetClubs().GetPrimaryClub()->GetVenue(),
						pRun1->m_pRun->GetEvent(),
						pRun1->m_pRun->GetDivision(),
						pRun1->m_pRun->GetLevel(),
						pRun1->m_pRun->GetDate());
				ARBConfigScoring const* pScoring2 = NULL;
				if (pRun2->m_pTrial->GetClubs().GetPrimaryClub())
					pScoring2 = pRun2->m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						pRun2->m_pTrial->GetClubs().GetPrimaryClub()->GetVenue(),
						pRun2->m_pRun->GetEvent(),
						pRun2->m_pRun->GetDivision(),
						pRun2->m_pRun->GetLevel(),
						pRun2->m_pRun->GetDate());
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
				ARBConfigScoring const* pScoring1 = NULL;
				if (pRun1->m_pTrial->GetClubs().GetPrimaryClub())
					pScoring1 = pRun1->m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						pRun1->m_pTrial->GetClubs().GetPrimaryClub()->GetVenue(),
						pRun1->m_pRun->GetEvent(),
						pRun1->m_pRun->GetDivision(),
						pRun1->m_pRun->GetLevel(),
						pRun1->m_pRun->GetDate());
				ARBConfigScoring const* pScoring2 = NULL;
				if (pRun2->m_pTrial->GetClubs().GetPrimaryClub())
					pScoring2 = pRun2->m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						pRun2->m_pTrial->GetClubs().GetPrimaryClub()->GetVenue(),
						pRun2->m_pRun->GetEvent(),
						pRun2->m_pRun->GetDivision(),
						pRun2->m_pRun->GetLevel(),
						pRun2->m_pRun->GetDate());
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
			short pts1 = 0;
			short pts2 = 0;
			if (pRun1->m_pRun->GetQ().Qualified())
			{
				ARBConfigScoring const* pScoring = NULL;
				if (pRun1->m_pTrial->GetClubs().GetPrimaryClub())
					 pScoring = pRun1->m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						pRun1->m_pTrial->GetClubs().GetPrimaryClub()->GetVenue(),
						pRun1->m_pRun->GetEvent(),
						pRun1->m_pRun->GetDivision(),
						pRun1->m_pRun->GetLevel(),
						pRun1->m_pRun->GetDate());
				if (pScoring)
					pts1 = pRun1->m_pRun->GetTitlePoints(pScoring);
			}
			if (pRun2->m_pRun->GetQ().Qualified())
			{
				ARBConfigScoring const* pScoring = NULL;
				if (pRun2->m_pTrial->GetClubs().GetPrimaryClub())
					pScoring = pRun2->m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
						pRun2->m_pTrial->GetClubs().GetPrimaryClub()->GetVenue(),
						pRun2->m_pRun->GetEvent(),
						pRun2->m_pRun->GetDivision(),
						pRun2->m_pRun->GetLevel(),
						pRun2->m_pRun->GetDate());
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
			std::string str1, str2;
			ARBDogFaultList::const_iterator iter;
			int i = 0;
			for (iter = pRun1->m_pRun->GetFaults().begin();
				iter != pRun1->m_pRun->GetFaults().end();
				++i, ++iter)
			{
				if (0 < i)
					str1 += ", ";
				str1 += *iter;
			}
			for (i = 0, iter = pRun2->m_pRun->GetFaults().begin();
				iter != pRun2->m_pRun->GetFaults().end();
				++i, ++iter)
			{
				if (0 < i)
					str2 += ", ";
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
			ARBConfigScoring const* pScoring1 = NULL;
			if (pRun1->m_pTrial->GetClubs().GetPrimaryClub())
				pScoring1 = pRun1->m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
					pRun1->m_pTrial->GetClubs().GetPrimaryClub()->GetVenue(),
					pRun1->m_pRun->GetEvent(),
					pRun1->m_pRun->GetDivision(),
					pRun1->m_pRun->GetLevel(),
					pRun1->m_pRun->GetDate());
			ARBConfigScoring const* pScoring2 = NULL;
			if (pRun2->m_pTrial->GetClubs().GetPrimaryClub())
				pScoring2 = pRun2->m_pView->GetDocument()->GetConfig().GetVenues().FindEvent(
					pRun2->m_pTrial->GetClubs().GetPrimaryClub()->GetVenue(),
					pRun2->m_pRun->GetEvent(),
					pRun2->m_pRun->GetDivision(),
					pRun2->m_pRun->GetLevel(),
					pRun2->m_pRun->GetDate());
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
		std::set<std::string> strings;
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
		for (std::set<std::string>::iterator iter = strings.begin(); iter != strings.end(); ++iter)
		{
			CString str((*iter).c_str());
			if (!MatchCase())
				str.MakeLower();
			if (0 <= str.Find(search))
			{
				m_pView->SetSelection(index, true);
				bFound = true;
			}
		}
	}
	if (!bFound)
	{
		CString msg;
		msg.Format("Cannot find \"%s\"", (LPCTSTR)m_strSearch);
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
	ON_WM_INITMENUPOPUP()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CAgilityBookViewRuns construction/destruction

#pragma warning (push)
#pragma warning ( disable : 4355 )
CAgilityBookViewRuns::CAgilityBookViewRuns()
	: m_bSuppressSelect(false)
	, m_Callback(this)
	, m_SortColumn(0)
{
	m_ImageList.Create(16, 16, ILC_MASK, 2, 0);
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

void CAgilityBookViewRuns::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
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

void CAgilityBookViewRuns::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
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
	return (CAgilityBookDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Printing

void CAgilityBookViewRuns::GetPrintLine(int nItem, CStringArray& line)
{
	CListView2::GetPrintLine(nItem, line);
}

/////////////////////////////////////////////////////////////////////////////

bool CAgilityBookViewRuns::IsFiltered() const
{
	return CAgilityBookOptions::IsFilterEnabled();
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
	CAgilityBookViewRunsData *pData = NULL;
	if (0 <= index && index < GetListCtrl().GetItemCount())
		pData = reinterpret_cast<CAgilityBookViewRunsData*>(GetListCtrl().GetItemData(index));
	return pData;
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
			CString str("");
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
	}
}

void CAgilityBookViewRuns::LoadData()
{
	m_bSuppressSelect = true;
	
	CWaitCursor wait;

	// Mirror the selection in the tree here.
	ARBDogRun* pCurRun = GetDocument()->GetCurrentRun();

	// Reduce flicker.
	GetListCtrl().SetRedraw(FALSE);

	// Clear everything.
	GetListCtrl().DeleteAllItems();

	// Add items.
	std::vector<CVenueFilter> venues;
	CAgilityBookOptions::GetFilterVenue(venues);
	list<ARBDogTrial*> trials;
	ARBDog* pDog = GetDocument()->GetCurrentDog();
	ARBDogTrial* pCurTrial = GetDocument()->GetCurrentTrial();
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
				ARBDogTrial* pTrial = (*iter);
				if (!pTrial->IsFiltered())
					trials.push_back(pTrial);
			}
		}
	}
	int nQs = 0;
	if (0 < trials.size())
	{
		int i = 0;
		for (list<ARBDogTrial*>::iterator iter = trials.begin(); iter != trials.end(); ++iter)
		{
			ARBDogTrial* pTrial = (*iter);
			for (ARBDogRunList::iterator iterRun = pTrial->GetRuns().begin();
			iterRun != pTrial->GetRuns().end();
			++i, ++iterRun)
			{
				ARBDogRun* pRun = (*iterRun);
				if (pRun->IsFiltered())
					continue;
				if (pRun->GetQ().Qualified())
					++nQs;
				CAgilityBookViewRunsData* pData = new CAgilityBookViewRunsData(this, pDog, pTrial, pRun);
				LV_ITEM item;
				item.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
				item.pszText = LPSTR_TEXTCALLBACK;
				item.iImage = I_IMAGECALLBACK;
				item.iItem = i;
				item.iSubItem = 0;
				item.lParam = reinterpret_cast<LPARAM>(pData);
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
			((CMainFrame*)AfxGetMainWnd())->SetStatusText(msg, IsFiltered());
		if (GetMessage2(msg))
			((CMainFrame*)AfxGetMainWnd())->SetStatusText2(msg);
	}

	SORT_RUN_INFO info;
	info.pThis = this;
	for (i = 0; i < static_cast<int>(m_Columns.size()); ++i)
	{
		if (m_Columns[i] == IO_RUNS_DATE)
		{
			m_SortColumn = i + 1;
			break;
		}
	}
	info.nCol = m_SortColumn;
	GetListCtrl().SortItems(CompareRuns, reinterpret_cast<LPARAM>(&info));
	if (0 == m_SortColumn)
		HeaderSort(0, CHeaderCtrl2::eNoSort);
	else
		HeaderSort(abs(m_SortColumn), CHeaderCtrl2::eAscending);

	// Cleanup.
	GetListCtrl().SetRedraw(TRUE);
	GetListCtrl().Invalidate();

	m_bSuppressSelect = false;
}

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewRuns message handlers

void CAgilityBookViewRuns::OnRclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Send WM_CONTEXTMENU to self (done according to Q222905)
	SendMessage(WM_CONTEXTMENU, (WPARAM)m_hWnd, GetMessagePos());
	*pResult = 1;
}

void CAgilityBookViewRuns::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CListView2::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	CCmdUI cmdUI;
	// (This may have changed for VC7+, but as of MFC4.2 it was required)
	// Hack to make this code work!!!!
	cmdUI.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (UINT n = 0; n < cmdUI.m_nIndexMax; ++n)
	{
		cmdUI.m_nIndex = n;
		cmdUI.m_nID = pPopupMenu->GetMenuItemID(cmdUI.m_nIndex);
		cmdUI.m_pMenu = pPopupMenu;
		CCmdTarget* pTarget = this;
		// Undocumented MFC cmd calls the ON_UPDATE_COMMAND_UI funcs.
		cmdUI.DoUpdate(pTarget, FALSE);
	}
}

void CAgilityBookViewRuns::OnContextMenu(CWnd* pWnd, CPoint point)
{
	int index = GetSelection();
	if (0 > index)
	{
		MessageBeep(0);
		return;
	}
	CAgilityBookViewRunsData* pData = GetItemData(index);
	if (!pData)
		return;
	// Point is (-1,-1) on the context menu button.
	if (0 > point.x || 0 > point.y)
	{
		// Adjust the menu position so it's on the selected item.
		CRect rect;
		GetListCtrl().GetItemRect(index, &rect, FALSE);
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
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
}

void CAgilityBookViewRuns::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (0 != pNMListView->iSubItem)
	{
		HeaderSort(abs(m_SortColumn), CHeaderCtrl2::eNoSort);
		int nBackwards = 1;
		if (m_SortColumn == pNMListView->iSubItem)
			nBackwards = -1;
		m_SortColumn = pNMListView->iSubItem * nBackwards;
		SORT_RUN_INFO info;
		info.pThis = this;
		info.nCol = m_SortColumn;
		GetListCtrl().SortItems(CompareRuns, reinterpret_cast<LPARAM>(&info));
		HeaderSort(abs(m_SortColumn),
			nBackwards > 0 ? CHeaderCtrl2::eAscending : CHeaderCtrl2::eDescending);
	}
	*pResult = 0;
}

void CAgilityBookViewRuns::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CAgilityBookViewRunsData *pData = reinterpret_cast<CAgilityBookViewRunsData*>(pDispInfo->item.lParam);
		CString str = pData->OnNeedText(pDispInfo->item.iSubItem);
		::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
		pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
	}
	if (pDispInfo->item.mask & LVIF_IMAGE)
	{
		CAgilityBookViewRunsData *pData = reinterpret_cast<CAgilityBookViewRunsData*>(pDispInfo->item.lParam);
		pDispInfo->item.iImage = pData->OnNeedIcon();
	}
	*pResult = 0;
}

void CAgilityBookViewRuns::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// I only care about the item being selected.
	if (0 <= pNMListView->iItem
	&& (LVIF_STATE & pNMListView->uChanged)
	&& !(LVIS_SELECTED & pNMListView->uOldState)
	&& (LVIS_SELECTED & pNMListView->uNewState))
	{
		if (!m_bSuppressSelect && 1 == GetListCtrl().GetSelectedCount())
		{
			CAgilityBookViewRunsData *pData = reinterpret_cast<CAgilityBookViewRunsData*>(pNMListView->lParam);
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

void CAgilityBookViewRuns::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CAgilityBookViewRunsData *pData = reinterpret_cast<CAgilityBookViewRunsData*>(pNMListView->lParam);
	if (pData)
		pData->Release();
	pNMListView->lParam = 0;
	*pResult = 0;
}

void CAgilityBookViewRuns::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
	if (pData)
		GetDocument()->EditRun(pData->GetRun());
	*pResult = 0;
}

void CAgilityBookViewRuns::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;
	switch (pLVKeyDown->wVKey)
	{
	default:
		break;
	case VK_SPACE:
	case VK_RETURN:
		{
			CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
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
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
	if (pData)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewRuns::OnAgilityEditRun() 
{
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
	if (pData)
		GetDocument()->EditRun(pData->GetRun());
}

void CAgilityBookViewRuns::OnUpdateAgilityNewTitle(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
	if (pData)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewRuns::OnAgilityNewTitle()
{
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
	if (pData)
		GetDocument()->AddTitle(pData->GetRun());
}

void CAgilityBookViewRuns::OnUpdateAgilityNewTrial(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
	if (pData)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewRuns::OnAgilityNewTrial()
{
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
	if (pData)
		GetDocument()->AddTrial(pData->GetRun());
}

void CAgilityBookViewRuns::OnUpdateAgilityNewRun(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
	if (pData)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewRuns::OnAgilityNewRun()
{
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
	if (pData)
		GetDocument()->AddRun(pData->GetRun());
}

void CAgilityBookViewRuns::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	OnUpdateEditCopy(pCmdUI);
}

void CAgilityBookViewRuns::OnEditCut()
{
	OnEditCopy();
	OnAgilityDeleteRun();
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
			data += "\r\n";
		}

		Element tree;
		tree.SetName(CLIPDATA);

		// Now all the data.
		for (std::vector<int>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
		{
			CAgilityBookViewRunsData* pData = GetItemData(*iter);
			if (pData)
				pData->GetRun()->Save(tree);
			CStringArray line;
			GetPrintLine((*iter), line);
			for (int i = 0; i < line.GetSize(); ++i)
			{
				if (0 < i)
					data += '\t';
				data += line[i];
			}
			data += "\r\n";
		}

		CopyDataToClipboard(CAgilityBookOptions::GetClipboardFormat(CAgilityBookOptions::eFormatRun), tree, data);
	}
}

void CAgilityBookViewRuns::OnUpdateAgilityDeleteRun(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
	if (pData)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewRuns::OnAgilityDeleteRun() 
{
	CAgilityBookViewRunsData* pData = GetItemData(GetSelection());
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
