/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CAgilityBookRunsView class
 * @author David Connet
 *
 * Revision History
 * @li 2009-07-11 DRC Change how runs are synced with tree to reduce reloading.
 * @li 2009-02-04 DRC Ported to wxWidgets.
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
#include "AgilityBookRunsView.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookMenu.h"
#include "AgilityBookOptions.h"
#include "AgilityBookPanels.h"
#include "AgilityBookTreeData.h"
#include "AgilityBookTreeView.h"
#include "ARBTypes.h"
#include "ClipBoard.h"
#include "DlgAssignColumns.h"
#include "DlgFind.h"
#include "Element.h"
#include "FilterOptions.h"
#include "Globals.h"
#include "ListData.h"
#include "MainFrm.h"
#include "PointsData.h"
#include "Print.h"
#include <algorithm>
#include <wx/config.h>

#include "res/AgilityBook16.xpm"
#include "res/crcd.xpm"


/////////////////////////////////////////////////////////////////////////////
// CAgilityBookRunsViewData

class CAgilityBookRunsViewData : public CListData
{
public:
	CAgilityBookRunsViewData(
			CAgilityBookRunsView* pView,
			ARBDogPtr pDog,
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun)
		: m_pView(pView)
		, m_pDog(pDog)
		, m_pTrial(pTrial)
		, m_pRun(pRun)
	{
	}
	~CAgilityBookRunsViewData()
	{
	}
	CAgilityBookDoc* GetDocument()		{return m_pView->GetDocument();}
	ARBDogPtr GetDog() const			{return m_pDog;}
	ARBDogTrialPtr GetTrial() const		{return m_pTrial;}
	ARBDogRunPtr GetRun() const			{return m_pRun;}
	virtual wxString OnNeedText(long iCol) const;
	virtual void OnNeedListItem(long iCol, wxListItem& info) const;
protected:
	CAgilityBookRunsView* m_pView;
	ARBDogPtr m_pDog;
	ARBDogTrialPtr m_pTrial;
	ARBDogRunPtr m_pRun;
};


wxString CAgilityBookRunsViewData::OnNeedText(long iCol) const
{
	short val;
	otstringstream str;
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
						str << wxT('/');
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
						str << wxT('/');
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
		case IO_RUNS_OBSTACLES:
			{
				short obstacles = m_pRun->GetScoring().GetObstacles();
				if (0 < obstacles)
					str << obstacles;
			}
			break;
		case IO_RUNS_OPS:
			{
				double ops;
				if (m_pRun->GetScoring().GetObstaclesPS(CAgilityBookOptions::GetTableInYPS(), ops))
				{
					str << ARBDouble::str(ops, 3);
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
                    m_pView->GetDocument()->Book().GetConfig().GetVenues().FindEvent(
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
				str << wxT('?');
			else if (0 == val)
				str << wxT('-');
			else
				str << val;
			break;
		case IO_RUNS_IN_CLASS:
			val = m_pRun->GetInClass();
			if (0 >= val)
				str << wxT('?');
			else
				str << val;
			break;
		case IO_RUNS_DOGSQD:
			val = m_pRun->GetDogsQd();
			if (0 > val)
				str << wxT('?');
			else
				str << m_pRun->GetDogsQd();
			break;
		case IO_RUNS_Q:
			{
				bool bSet = false;
				if (m_pRun->GetQ().Qualified())
				{
					tstring q;
					std::vector<ARBConfigMultiQPtr> multiQs;
					if (0 < m_pRun->GetMultiQs(multiQs))
					{
						for (std::vector<ARBConfigMultiQPtr>::iterator iMultiQ = multiQs.begin(); iMultiQ != multiQs.end(); ++iMultiQ)
						{
							if (!q.empty())
								q += wxT('/');
							q += (*iMultiQ)->GetShortName();
						}
					}
					if (ARB_Q::eQ_SuperQ == m_pRun->GetQ())
					{
						bSet = true;
						if (!q.empty())
							str << q << wxT('/');
						str << _("IDS_SQ");
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
					m_pView->GetDocument()->Book().GetConfig().GetVenues().FindEvent(
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
						m_pView->GetDocument()->Book().GetConfig().GetVenues().FindEvent(
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
				wxString str2(m_pRun->GetNote().c_str());
				str2.Replace(wxT("\n"), wxT(" "));
				str << str2.c_str();
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
						str << wxT(", ");
					str << (*iter);
				}
			}
			break;
		case IO_RUNS_SPEED:
			{
				ARBConfigScoringPtr pScoring;
				if (m_pTrial->GetClubs().GetPrimaryClub())
                    m_pView->GetDocument()->Book().GetConfig().GetVenues().FindEvent(
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
	return str.str().c_str();
}


void CAgilityBookRunsViewData::OnNeedListItem(long iCol, wxListItem& info) const
{
	if (m_pRun)
	{
		info.SetMask(info.GetMask() | wxLIST_MASK_TEXT);
		info.SetText(OnNeedText(iCol));
		if (0 == iCol)
		{
			int iImage = -1;
			if (0 < m_pRun->GetCRCDRawMetaData().length())
				iImage = m_pView->m_imgMap;
			else if (0 < m_pRun->GetCRCD().length())
				iImage = m_pView->m_imgCourse;
			if (0 <= iImage)
			{
				info.SetMask(info.GetMask() | wxLIST_MASK_IMAGE);
				info.SetImage(iImage);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// List sorting

CAgilityBookRunsView::CSortColumn::CSortColumn(std::vector<long>& inColumns)
	: m_Columns(inColumns)
	, m_iCol(1)
{
}


void CAgilityBookRunsView::CSortColumn::Initialize()
{
	long realCol = IO_RUNS_DATE;
	realCol = wxConfig::Get()->Read(wxT("Sorting/Runs"), realCol);
	long neg = 1;
	if (0 > realCol)
	{
		neg = -1;
		realCol *= -1;
	}
	long col = LookupColumn(realCol);
	if (0 > m_iCol)
		col = LookupColumn(IO_RUNS_DATE);
	m_iCol = col * neg;
}


void CAgilityBookRunsView::CSortColumn::SetColumn(long iCol)
{
	m_iCol = iCol;
	if (0 == iCol)
		return;
	long neg = 1;
	long col = iCol;
	if (0 > iCol)
	{
		neg = -1;
		col = iCol * -1;
	}
	long realCol = m_Columns[col-1] * neg;
	wxConfig::Get()->Write(wxT("Sorting/Runs"), realCol);
}


long CAgilityBookRunsView::CSortColumn::LookupColumn(long iCol) const
{
	size_t n = m_Columns.size();
	for (size_t i = 0; i < n; ++i)
	{
		if (m_Columns[i] == iCol)
		{
			return static_cast<long>(i+1);
		}
	}
	return -1;
}


// The wx functions take a 'long'. Which means we can't pass pointers on 64bit.
// So, we use a global. Since this is only used in one place, we don't have
// any threading issues.
static struct
{
	CAgilityBookRunsView* pThis;
	int nCol;
} s_SortInfo;


int wxCALLBACK CompareRuns(long item1, long item2, long sortData)
{
	CAgilityBookRunsViewDataPtr pRun1 = s_SortInfo.pThis->GetItemRunDataByData(item1);
	CAgilityBookRunsViewDataPtr pRun2 = s_SortInfo.pThis->GetItemRunDataByData(item2);

	int nRet = 0;
	int iCol = abs(s_SortInfo.nCol);
	// Col 0 is special: it has the icons. Instead of saving it,
	// we simply ignore it - so iCol is always off by 1.
	switch (s_SortInfo.pThis->m_Columns[iCol - 1])
	{
	default:
		break;

	case IO_RUNS_REG_NAME:
		if (pRun1->GetDog()->GetRegisteredName() < pRun2->GetDog()->GetRegisteredName())
			nRet = -1;
		else if (pRun1->GetDog()->GetRegisteredName() > pRun2->GetDog()->GetRegisteredName())
			nRet = 1;
		break;
	case IO_RUNS_CALL_NAME:
		if (pRun1->GetDog()->GetCallName() < pRun2->GetDog()->GetCallName())
			nRet = -1;
		else if (pRun1->GetDog()->GetCallName() > pRun2->GetDog()->GetCallName())
			nRet = 1;
		break;
	case IO_RUNS_DATE:
		if (pRun1->GetRun()->GetDate() < pRun2->GetRun()->GetDate())
			nRet = -1;
		else if (pRun1->GetRun()->GetDate() > pRun2->GetRun()->GetDate())
			nRet = 1;
		break;
	case IO_RUNS_VENUE:
		{
			tstring str1, str2;
			int i = 0;
			ARBDogClubList::const_iterator iter;
			for (iter = pRun1->GetTrial()->GetClubs().begin();
				iter != pRun1->GetTrial()->GetClubs().end();
				++iter, ++i)
			{
				if (0 < i)
					str1 += wxT("/");
				str1 += (*iter)->GetVenue();
			}
			for (i = 0, iter = pRun2->GetTrial()->GetClubs().begin();
				iter != pRun2->GetTrial()->GetClubs().end();
				++iter, ++i)
			{
				if (0 < i)
					str2 += wxT("/");
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
			tstring str1, str2;
			int i = 0;
			ARBDogClubList::const_iterator iter;
			for (iter = pRun1->GetTrial()->GetClubs().begin();
				iter != pRun1->GetTrial()->GetClubs().end();
				++iter, ++i)
			{
				if (0 < i)
					str1 += wxT("/");
				str1 += (*iter)->GetName();
			}
			for (i = 0, iter = pRun2->GetTrial()->GetClubs().begin();
				iter != pRun2->GetTrial()->GetClubs().end();
				++iter, ++i)
			{
				if (0 < i)
					str2 += wxT("/");
				str2 += (*iter)->GetName();
			}
			if (str1 < str2)
				nRet = -1;
			else if (str1 > str2)
				nRet = 1;
		}
		break;
	case IO_RUNS_LOCATION:
		if (pRun1->GetTrial()->GetLocation() < pRun2->GetTrial()->GetLocation())
			nRet = -1;
		else if (pRun1->GetTrial()->GetLocation() > pRun2->GetTrial()->GetLocation())
			nRet = 1;
		break;
	case IO_RUNS_TRIAL_NOTES:
		if (pRun1->GetTrial()->GetNote() < pRun2->GetTrial()->GetNote())
			nRet = -1;
		else if (pRun1->GetTrial()->GetNote() > pRun2->GetTrial()->GetNote())
			nRet = 1;
		break;
	case IO_RUNS_DIVISION:
		if (pRun1->GetRun()->GetDivision() < pRun2->GetRun()->GetDivision())
			nRet = -1;
		else if (pRun1->GetRun()->GetDivision() > pRun2->GetRun()->GetDivision())
			nRet = 1;
		break;
	case IO_RUNS_LEVEL:
		if (pRun1->GetRun()->GetLevel() < pRun2->GetRun()->GetLevel())
			nRet = -1;
		else if (pRun1->GetRun()->GetLevel() > pRun2->GetRun()->GetLevel())
			nRet = 1;
		break;
	case IO_RUNS_EVENT:
		if (pRun1->GetRun()->GetEvent() < pRun2->GetRun()->GetEvent())
			nRet = -1;
		else if (pRun1->GetRun()->GetEvent() > pRun2->GetRun()->GetEvent())
			nRet = 1;
		break;
	case IO_RUNS_HEIGHT:
		if (pRun1->GetRun()->GetHeight() < pRun2->GetRun()->GetHeight())
			nRet = -1;
		else if (pRun1->GetRun()->GetHeight() > pRun2->GetRun()->GetHeight())
			nRet = 1;
		break;
	case IO_RUNS_JUDGE:
		if (pRun1->GetRun()->GetJudge() < pRun2->GetRun()->GetJudge())
			nRet = -1;
		else if (pRun1->GetRun()->GetJudge() > pRun2->GetRun()->GetJudge())
			nRet = 1;
		break;
	case IO_RUNS_HANDLER:
		if (pRun1->GetRun()->GetHandler() < pRun2->GetRun()->GetHandler())
			nRet = -1;
		else if (pRun1->GetRun()->GetHandler() > pRun2->GetRun()->GetHandler())
			nRet = 1;
		break;
	case IO_RUNS_CONDITIONS:
		if (pRun1->GetRun()->GetConditions() < pRun2->GetRun()->GetConditions())
			nRet = -1;
		else if (pRun1->GetRun()->GetConditions() > pRun2->GetRun()->GetConditions())
			nRet = 1;
		break;
	case IO_RUNS_COURSE_FAULTS:
		if (pRun1->GetRun()->GetScoring().GetCourseFaults() < pRun2->GetRun()->GetScoring().GetCourseFaults())
			nRet = -1;
		else if (pRun1->GetRun()->GetScoring().GetCourseFaults() > pRun2->GetRun()->GetScoring().GetCourseFaults())
			nRet = 1;
		break;
	case IO_RUNS_TIME:
		if (pRun1->GetRun()->GetScoring().GetTime() < pRun2->GetRun()->GetScoring().GetTime())
			nRet = -1;
		else if (pRun1->GetRun()->GetScoring().GetTime() > pRun2->GetRun()->GetScoring().GetTime())
			nRet = 1;
		break;
	case IO_RUNS_YARDS:
		{
			bool bRun1 = (ARBDogRunScoring::eTypeByTime == pRun1->GetRun()->GetScoring().GetType()
				&& 0.0 < pRun1->GetRun()->GetScoring().GetYards());
			bool bRun2 = (ARBDogRunScoring::eTypeByTime == pRun2->GetRun()->GetScoring().GetType()
				&& 0.0 < pRun2->GetRun()->GetScoring().GetYards());
			if (bRun1 && bRun2)
			{
				if (pRun1->GetRun()->GetScoring().GetYards() < pRun2->GetRun()->GetScoring().GetYards())
					nRet = -1;
				else if (pRun1->GetRun()->GetScoring().GetYards() > pRun2->GetRun()->GetScoring().GetYards())
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
			bool bOk1 = pRun1->GetRun()->GetScoring().GetMinYPS(CAgilityBookOptions::GetTableInYPS(), yps1);
			bool bOk2 = pRun2->GetRun()->GetScoring().GetMinYPS(CAgilityBookOptions::GetTableInYPS(), yps2);
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
			bool bOk1 = pRun1->GetRun()->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), yps1);
			bool bOk2 = pRun2->GetRun()->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), yps2);
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
	case IO_RUNS_OBSTACLES:
		{
			short ob1 = pRun1->GetRun()->GetScoring().GetObstacles();
			short ob2 = pRun2->GetRun()->GetScoring().GetObstacles();
			if (ob1 < ob2)
				nRet = -1;
			else if (ob1 > ob2)
				nRet = 1;
		}
		break;
	case IO_RUNS_OPS:
		{
			double ops1, ops2;
			bool bOk1 = pRun1->GetRun()->GetScoring().GetObstaclesPS(CAgilityBookOptions::GetTableInYPS(), ops1);
			bool bOk2 = pRun2->GetRun()->GetScoring().GetObstaclesPS(CAgilityBookOptions::GetTableInYPS(), ops2);
			if (bOk1 && bOk2)
			{
				if (ops1 < ops2)
					nRet = -1;
				else if (ops1 > ops2)
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
			bool bOk1 = ARBDogRunScoring::eTypeByTime == pRun1->GetRun()->GetScoring().GetType()
				&& 0.0 < pRun1->GetRun()->GetScoring().GetSCT();
			bool bOk2 = ARBDogRunScoring::eTypeByTime == pRun2->GetRun()->GetScoring().GetType()
				&& 0.0 < pRun2->GetRun()->GetScoring().GetSCT();
			if (bOk1 && bOk2)
			{
				if (pRun1->GetRun()->GetScoring().GetSCT() < pRun2->GetRun()->GetScoring().GetSCT())
					nRet = -1;
				else if (pRun1->GetRun()->GetScoring().GetSCT() > pRun2->GetRun()->GetScoring().GetSCT())
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
			bool bOk1 = ARBDogRunScoring::eTypeByTime == pRun1->GetRun()->GetScoring().GetType();
			bool bOk2 = ARBDogRunScoring::eTypeByTime == pRun2->GetRun()->GetScoring().GetType();
			if (bOk1 && bOk2)
			{
				ARBConfigScoringPtr pScoring1;
				if (pRun1->GetTrial()->GetClubs().GetPrimaryClub())
					pRun1->GetDocument()->Book().GetConfig().GetVenues().FindEvent(
						pRun1->GetTrial()->GetClubs().GetPrimaryClubVenue(),
						pRun1->GetRun()->GetEvent(),
						pRun1->GetRun()->GetDivision(),
						pRun1->GetRun()->GetLevel(),
						pRun1->GetRun()->GetDate(),
						NULL,
						&pScoring1);
				ARBConfigScoringPtr pScoring2;
				if (pRun2->GetTrial()->GetClubs().GetPrimaryClub())
					pRun2->GetDocument()->Book().GetConfig().GetVenues().FindEvent(
						pRun2->GetTrial()->GetClubs().GetPrimaryClubVenue(),
						pRun2->GetRun()->GetEvent(),
						pRun2->GetRun()->GetDivision(),
						pRun2->GetRun()->GetLevel(),
						pRun2->GetRun()->GetDate(),
						NULL,
						&pScoring2);
				double faults1 = pRun1->GetRun()->GetScoring().GetCourseFaults() + pRun1->GetRun()->GetScoring().GetTimeFaults(pScoring1);
				double faults2 = pRun2->GetRun()->GetScoring().GetCourseFaults() + pRun2->GetRun()->GetScoring().GetTimeFaults(pScoring2);
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
			bool bOk1 = ARBDogRunScoring::eTypeByOpenClose == pRun1->GetRun()->GetScoring().GetType();
			bool bOk2 = ARBDogRunScoring::eTypeByOpenClose == pRun2->GetRun()->GetScoring().GetType();
			if (bOk1 && bOk2)
			{
				if (pRun1->GetRun()->GetScoring().GetNeedOpenPts() < pRun2->GetRun()->GetScoring().GetNeedOpenPts())
					nRet = -1;
				else if (pRun1->GetRun()->GetScoring().GetNeedOpenPts() > pRun2->GetRun()->GetScoring().GetNeedOpenPts())
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
			bool bOk1 = ARBDogRunScoring::eTypeByOpenClose == pRun1->GetRun()->GetScoring().GetType();
			bool bOk2 = ARBDogRunScoring::eTypeByOpenClose == pRun2->GetRun()->GetScoring().GetType();
			if (bOk1 && bOk2)
			{
				if (pRun1->GetRun()->GetScoring().GetNeedClosePts() < pRun2->GetRun()->GetScoring().GetNeedClosePts())
					nRet = -1;
				else if (pRun1->GetRun()->GetScoring().GetNeedClosePts() > pRun2->GetRun()->GetScoring().GetNeedClosePts())
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
			bool bOk1 = ARBDogRunScoring::eTypeByOpenClose == pRun1->GetRun()->GetScoring().GetType();
			bool bOk2 = ARBDogRunScoring::eTypeByOpenClose == pRun2->GetRun()->GetScoring().GetType();
			if (bOk1 && bOk2)
			{
				if (pRun1->GetRun()->GetScoring().GetOpenPts() < pRun2->GetRun()->GetScoring().GetOpenPts())
					nRet = -1;
				else if (pRun1->GetRun()->GetScoring().GetOpenPts() > pRun2->GetRun()->GetScoring().GetOpenPts())
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
			bool bOk1 = ARBDogRunScoring::eTypeByOpenClose == pRun1->GetRun()->GetScoring().GetType();
			bool bOk2 = ARBDogRunScoring::eTypeByOpenClose == pRun2->GetRun()->GetScoring().GetType();
			if (bOk1 && bOk2)
			{
				if (pRun1->GetRun()->GetScoring().GetClosePts() < pRun2->GetRun()->GetScoring().GetClosePts())
					nRet = -1;
				else if (pRun1->GetRun()->GetScoring().GetClosePts() > pRun2->GetRun()->GetScoring().GetClosePts())
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
			bool bOk1 = ARBDogRunScoring::eTypeByPoints == pRun1->GetRun()->GetScoring().GetType();
			bool bOk2 = ARBDogRunScoring::eTypeByPoints == pRun2->GetRun()->GetScoring().GetType();
			if (bOk1 && bOk2)
			{
				if (pRun1->GetRun()->GetScoring().GetNeedOpenPts() < pRun2->GetRun()->GetScoring().GetNeedOpenPts())
					nRet = -1;
				else if (pRun1->GetRun()->GetScoring().GetNeedOpenPts() > pRun2->GetRun()->GetScoring().GetNeedOpenPts())
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
			bool bOk1 = ARBDogRunScoring::eTypeByPoints == pRun1->GetRun()->GetScoring().GetType();
			bool bOk2 = ARBDogRunScoring::eTypeByPoints == pRun2->GetRun()->GetScoring().GetType();
			if (bOk1 && bOk2)
			{
				if (pRun1->GetRun()->GetScoring().GetOpenPts() < pRun2->GetRun()->GetScoring().GetOpenPts())
					nRet = -1;
				else if (pRun1->GetRun()->GetScoring().GetOpenPts() > pRun2->GetRun()->GetScoring().GetOpenPts())
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
			short p1 = pRun1->GetRun()->GetPlace();
			short p2 = pRun2->GetRun()->GetPlace();
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
			short p1 = pRun1->GetRun()->GetInClass();
			short p2 = pRun2->GetRun()->GetInClass();
			if (p1 < p2)
				nRet = -1;
			else if (p1 > p2)
				nRet = 1;
		}
		break;
	case IO_RUNS_DOGSQD:
		{
			short p1 = pRun1->GetRun()->GetDogsQd();
			short p2 = pRun2->GetRun()->GetDogsQd();
			if (p1 < p2)
				nRet = -1;
			else if (p1 > p2)
				nRet = 1;
		}
		break;
	case IO_RUNS_Q:
		if (pRun1->GetRun()->GetQ() < pRun2->GetRun()->GetQ())
			nRet = -1;
		else if (pRun1->GetRun()->GetQ() > pRun2->GetRun()->GetQ())
			nRet = 1;
		break;
	case IO_RUNS_SCORE:
		{
			bool bOk1 = (pRun1->GetRun()->GetQ().Qualified() || ARB_Q::eQ_NQ == pRun1->GetRun()->GetQ());
			bool bOk2 = (pRun2->GetRun()->GetQ().Qualified() || ARB_Q::eQ_NQ == pRun2->GetRun()->GetQ());
			if (bOk1 && bOk2)
			{
				ARBConfigScoringPtr pScoring1;
				if (pRun1->GetTrial()->GetClubs().GetPrimaryClub())
					pRun1->GetDocument()->Book().GetConfig().GetVenues().FindEvent(
						pRun1->GetTrial()->GetClubs().GetPrimaryClubVenue(),
						pRun1->GetRun()->GetEvent(),
						pRun1->GetRun()->GetDivision(),
						pRun1->GetRun()->GetLevel(),
						pRun1->GetRun()->GetDate(),
						NULL,
						&pScoring1);
				ARBConfigScoringPtr pScoring2;
				if (pRun2->GetTrial()->GetClubs().GetPrimaryClub())
					pRun2->GetDocument()->Book().GetConfig().GetVenues().FindEvent(
						pRun2->GetTrial()->GetClubs().GetPrimaryClubVenue(),
						pRun2->GetRun()->GetEvent(),
						pRun2->GetRun()->GetDivision(),
						pRun2->GetRun()->GetLevel(),
						pRun2->GetRun()->GetDate(),
						NULL,
						&pScoring2);
				if (pScoring1 && pScoring2)
				{
					if (pRun1->GetRun()->GetScore(pScoring1) < pRun2->GetRun()->GetScore(pScoring2))
						nRet = -1;
					else if (pRun1->GetRun()->GetScore(pScoring1) > pRun2->GetRun()->GetScore(pScoring2))
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
			if (pRun1->GetRun()->GetQ().Qualified())
			{
				ARBConfigScoringPtr pScoring;
				if (pRun1->GetTrial()->GetClubs().GetPrimaryClub())
					 pRun1->GetDocument()->Book().GetConfig().GetVenues().FindEvent(
						pRun1->GetTrial()->GetClubs().GetPrimaryClubVenue(),
						pRun1->GetRun()->GetEvent(),
						pRun1->GetRun()->GetDivision(),
						pRun1->GetRun()->GetLevel(),
						pRun1->GetRun()->GetDate(),
						NULL,
						&pScoring);
				if (pScoring)
					pts1 = pRun1->GetRun()->GetTitlePoints(pScoring);
			}
			if (pRun2->GetRun()->GetQ().Qualified())
			{
				ARBConfigScoringPtr pScoring;
				if (pRun2->GetTrial()->GetClubs().GetPrimaryClub())
					pRun2->GetDocument()->Book().GetConfig().GetVenues().FindEvent(
						pRun2->GetTrial()->GetClubs().GetPrimaryClubVenue(),
						pRun2->GetRun()->GetEvent(),
						pRun2->GetRun()->GetDivision(),
						pRun2->GetRun()->GetLevel(),
						pRun2->GetRun()->GetDate(),
						NULL,
						&pScoring);
				if (pScoring)
					pts2 = pRun2->GetRun()->GetTitlePoints(pScoring);
			}
			if (pts1 < pts2)
				nRet = -1;
			else if (pts1 > pts2)
				nRet = 1;
		}
		break;
	case IO_RUNS_COMMENTS:
		if (pRun1->GetRun()->GetNote() < pRun2->GetRun()->GetNote())
			nRet = -1;
		else if (pRun1->GetRun()->GetNote() > pRun2->GetRun()->GetNote())
			nRet = 1;
		break;
	case IO_RUNS_FAULTS:
		{
			tstring str1, str2;
			ARBDogFaultList::const_iterator iter;
			int i = 0;
			for (iter = pRun1->GetRun()->GetFaults().begin();
				iter != pRun1->GetRun()->GetFaults().end();
				++i, ++iter)
			{
				if (0 < i)
					str1 += wxT(", ");
				str1 += *iter;
			}
			for (i = 0, iter = pRun2->GetRun()->GetFaults().begin();
				iter != pRun2->GetRun()->GetFaults().end();
				++i, ++iter)
			{
				if (0 < i)
					str2 += wxT(", ");
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
			if (pRun1->GetTrial()->GetClubs().GetPrimaryClub())
				pRun1->GetDocument()->Book().GetConfig().GetVenues().FindEvent(
					pRun1->GetTrial()->GetClubs().GetPrimaryClubVenue(),
					pRun1->GetRun()->GetEvent(),
					pRun1->GetRun()->GetDivision(),
					pRun1->GetRun()->GetLevel(),
					pRun1->GetRun()->GetDate(),
					NULL,
					&pScoring1);
			ARBConfigScoringPtr pScoring2;
			if (pRun2->GetTrial()->GetClubs().GetPrimaryClub())
				pRun2->GetDocument()->Book().GetConfig().GetVenues().FindEvent(
					pRun2->GetTrial()->GetClubs().GetPrimaryClubVenue(),
					pRun2->GetRun()->GetEvent(),
					pRun2->GetRun()->GetDivision(),
					pRun2->GetRun()->GetLevel(),
					pRun2->GetRun()->GetDate(),
					NULL,
					&pScoring2);
			int pts1 = -1;
			int pts2 = -1;
			if (pScoring1 && pScoring2)
			{
				if (pScoring1->HasSpeedPts())
					pts1 = pRun1->GetRun()->GetSpeedPoints(pScoring1);
				if (pScoring2->HasSpeedPts())
					pts2 = pRun2->GetRun()->GetSpeedPoints(pScoring2);
			}
			if (pts1 > pts2)
				nRet = 1;
			else if (pts1 < pts2)
				nRet = -1;
		}
		break;
	}
	if (0 > s_SortInfo.nCol)
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
	long index = m_pView->m_Ctrl->GetFirstSelected();
	if (0 <= index && index < m_pView->m_Ctrl->GetItemCount())
	{
		index += inc;
	}
	else if (0 > index && SearchDown())
		index = 0;
	else if (index >= m_pView->m_Ctrl->GetItemCount() && !SearchDown())
		index = m_pView->m_Ctrl->GetItemCount() - 1;
	wxString search = Text();
	if (!MatchCase())
		search.MakeLower();
	for (; !bFound && 0 <= index && index < m_pView->m_Ctrl->GetItemCount(); index += inc)
	{
		std::set<tstring> strings;
		if (SearchAll())
		{
			CAgilityBookRunsViewDataPtr pData = m_pView->GetItemRunData(index);
			if (pData)
				pData->GetRun()->GetSearchStrings(strings);
		}
		else
		{
			long nColumns = m_pView->m_Ctrl->GetColumnCount();
			for (long i = 0; i < nColumns; ++i)
			{
				wxListItem info;
				info.SetId(index);
				info.SetMask(wxLIST_MASK_TEXT);
				info.SetColumn(i);
				m_pView->m_Ctrl->GetItem(info);
				strings.insert(info.GetText().c_str());
			}
		}
		for (std::set<tstring>::iterator iter = strings.begin(); iter != strings.end(); ++iter)
		{
			wxString str((*iter).c_str());
			if (!MatchCase())
				str.MakeLower();
			if (0 <= str.Find(search))
			{
				m_pView->m_Ctrl->SetSelection(index, true);
				bFound = true;
			}
		}
	}
	if (!bFound)
	{
		wxString msg = wxString::Format(_("IDS_CANNOT_FIND"), m_strSearch.c_str());
		wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
	}
	return bFound;
}

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(CAgilityBookRunsView, CAgilityBookBaseExtraView)


BEGIN_EVENT_TABLE(CAgilityBookRunsView, CAgilityBookBaseExtraView)
	EVT_CONTEXT_MENU(CAgilityBookRunsView::OnViewContextMenu)
	EVT_UPDATE_UI(wxID_DUPLICATE, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(wxID_DUPLICATE, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(wxID_CUT, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(wxID_CUT, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(wxID_COPY, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(wxID_COPY, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(wxID_PASTE, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(wxID_PASTE, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(wxID_SELECTALL, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(wxID_SELECTALL, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(wxID_FIND, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(wxID_FIND, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(ID_EDIT_FIND_NEXT, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(ID_EDIT_FIND_NEXT, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(ID_EDIT_FIND_PREVIOUS, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(ID_EDIT_FIND_PREVIOUS, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_NEW_TITLE, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_NEW_TITLE, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_NEW_TRIAL, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_NEW_TRIAL, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_EDIT_RUN, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_EDIT_RUN, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_NEW_RUN, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_NEW_RUN, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_DELETE_RUN, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_DELETE_RUN, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_PRINT_RUNS, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_PRINT_RUNS, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_CUSTOMIZE, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_CUSTOMIZE, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_SORTRUNS, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_SORTRUNS, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_RUNS_BY_TRIAL, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_RUNS_BY_TRIAL, CAgilityBookRunsView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_TABLE_IN_YPS, CAgilityBookRunsView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_TABLE_IN_YPS, CAgilityBookRunsView::OnViewCmd)
	EVT_MENU(wxID_PRINT, CAgilityBookRunsView::OnPrint)
	EVT_MENU(wxID_PREVIEW, CAgilityBookRunsView::OnPreview)
END_EVENT_TABLE()


#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4355)
#endif
CAgilityBookRunsView::CAgilityBookRunsView(
		CTabView* pTabView,
		wxDocument* doc)
	: CAgilityBookBaseExtraView(pTabView, doc)
	, m_Ctrl(NULL)
	, m_imgCourse(-1)
	, m_imgMap(-1)
	, m_bSuppressSelect(false)
	, m_Columns()
	, m_Callback(this)
	, m_SortColumn(m_Columns)
{
}
#ifdef _MSC_VER
#pragma warning (pop)
#endif


CAgilityBookRunsView::~CAgilityBookRunsView()
{
}


bool CAgilityBookRunsView::Create(
		CBasePanel* parentView,
		wxWindow* parentCtrl,
		wxDocument* doc,
		long flags,
		wxSizer* sizer,
		int proportion,
		int sizerFlags,
		int border)
{
	m_Ctrl = new CReportListCtrl(parentCtrl, false);
	m_Ctrl->Connect(wxEVT_COMMAND_SET_FOCUS, wxFocusEventHandler(CAgilityBookRunsView::OnCtrlSetFocus), NULL, this);
	m_Ctrl->Connect(wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler(CAgilityBookRunsView::OnCtrlColumnClick), NULL, this);
	m_Ctrl->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CAgilityBookRunsView::OnCtrlItemSelected), NULL, this);
	m_Ctrl->Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(CAgilityBookRunsView::OnCtrlItemActivated), NULL, this);
	m_Ctrl->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(CAgilityBookRunsView::OnCtrlKeyDown), NULL, this);
	m_imgCourse = m_Ctrl->AddIcon(wxIcon(AgilityBook16_xpm));
	m_imgMap = m_Ctrl->AddIcon(wxIcon(crcd_xpm));
	return CAgilityBookBaseExtraView::Create(parentView, parentCtrl, doc, flags, sizer, proportion, sizerFlags, border);
}


bool CAgilityBookRunsView::PrevPane()
{
	if (m_parentPanel)
		return m_parentPanel->ActivateView(0);
	return false;
}


bool CAgilityBookRunsView::NextPane()
{
	if (m_parentPanel)
		return m_parentPanel->ActivateView(0);
	return false;
}


void CAgilityBookRunsView::DetachView()
{
	// The control is actually owned by the panel, the view is not.
	m_Ctrl = NULL;
	m_imgCourse = -1;
	m_imgMap = -1;
}


bool CAgilityBookRunsView::IsFiltered() const
{
	return CFilterOptions::Options().IsFilterEnabled();
}


bool CAgilityBookRunsView::GetMessage(wxString& msg) const
{
	if (!m_Ctrl)
		return false;
	int nQs = 0;
	for (int index = 0; index < m_Ctrl->GetItemCount(); ++index)
	{
		CAgilityBookRunsViewDataPtr pData = GetItemRunData(index);
		if (pData && pData->GetRun() && pData->GetRun()->GetQ().Qualified())
			++nQs;
	}
	msg = wxString::Format(_("IDS_NUM_RUNS_QS"), m_Ctrl->GetItemCount(), nQs);
	return true;
}


bool CAgilityBookRunsView::GetMessage2(wxString& msg) const
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


bool CAgilityBookRunsView::AllowStatusContext(int field) const
{
	return STATUS_DOG == field || STATUS_FILTERED == field;
}


bool CAgilityBookRunsView::OnCreate(
		wxDocument* doc,
		long flags)
{
	SetupColumns();
	return true;
}


void CAgilityBookRunsView::DoActivateView(
		bool activate,
		wxView* activeView,
		wxView* deactiveView)
{
	if (m_Ctrl && activate && wxWindow::DoFindFocus() != m_Ctrl)
		m_Ctrl->SetFocus();
}


void CAgilityBookRunsView::OnDraw(wxDC* dc)
{
}


void CAgilityBookRunsView::OnUpdate(
		wxView* sender,
		wxObject* inHint)
{
	if (!m_Ctrl)
		return;
	CUpdateHint* hint = NULL;
	if (inHint)
		hint = reinterpret_cast<CUpdateHint*>(inHint);
	bool bLoad = false;
	if (hint)
	{
		if (hint->IsEqual(UPDATE_LANG_CHANGE) || hint->IsEqual(UPDATE_CUSTOMIZE))
		{
			SetupColumns();
			bLoad = true;
		}
		else if (hint->IsSet(UPDATE_RUNS_VIEW)
		|| hint->IsEqual(UPDATE_CONFIG)
		|| hint->IsEqual(UPDATE_OPTIONS))
		{
			bLoad = true;
		}
		else if (hint->IsSet(UPDATE_RUNS_SELECTION_VIEW))
		{
			ARBDogRunPtr pRun = tr1::dynamic_pointer_cast<ARBDogRun, ARBBase>(hint->GetObj());
			if (pRun)
			{
				for (int i = 0; i < m_Ctrl->GetItemCount(); ++i)
				{
					CAgilityBookRunsViewDataPtr pData = GetItemRunData(i);
					if (pData && pData->GetRun() == pRun)
					{
						m_Ctrl->SetSelection(i, true);
						break;
					}
				}
			}
		}
	}
	else
		bLoad = true;
	if (bLoad)
		LoadData();
}


bool CAgilityBookRunsView::IsTrial(ARBDogTrialPtr pTrial) const
{
	if (pTrial)
	{
		// Same number of runs... maybe?
		if (pTrial->GetRuns().size() == static_cast<size_t>(m_Ctrl->GetItemCount()))
		{
			// Get the first run in the list and see if that's in the trial.
			CAgilityBookRunsViewDataPtr pData = GetItemRunData(0);
			if (pData && pData->GetRun())
			{
				for (ARBDogRunList::const_iterator iter = pTrial->GetRuns().begin();
					iter != pTrial->GetRuns().end();
					++iter)
				{
					if (*iter == pData->GetRun())
						return true;
				}
			}
		}
	}
	return false;
}


void CAgilityBookRunsView::GetPrintLine(
		long item,
		std::vector<wxString>& line) const
{
	if (m_Ctrl)
		m_Ctrl->GetPrintLine(item, line);
}


CAgilityBookRunsViewDataPtr CAgilityBookRunsView::GetItemRunData(long index) const
{
	if (!m_Ctrl)
		return CAgilityBookRunsViewDataPtr();
	return tr1::dynamic_pointer_cast<CAgilityBookRunsViewData, CListData>(m_Ctrl->GetData(index));
}


CAgilityBookRunsViewDataPtr CAgilityBookRunsView::GetItemRunDataByData(long data) const
{
	if (!m_Ctrl)
		return CAgilityBookRunsViewDataPtr();
	return tr1::dynamic_pointer_cast<CAgilityBookRunsViewData, CListData>(m_Ctrl->GetDataByData(data));
}


void CAgilityBookRunsView::SetupColumns()
{
	if (!m_Ctrl)
		return;
	int nColumnCount = m_Ctrl->GetColumnCount();
	for (int i = 0; i < nColumnCount; ++i)
		m_Ctrl->DeleteColumn(0);
	if (CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_RUNS_LIST, m_Columns))
	{
		for (size_t iCol = 0; iCol <= m_Columns.size(); ++iCol)
		{
			wxString str(wxT(""));
			int fmt = 0;
			if (0 < iCol)
			{
				str = CDlgAssignColumns::GetNameFromColumnID(m_Columns[iCol-1]);
				fmt = CDlgAssignColumns::GetFormatFromColumnID(m_Columns[iCol-1]);
			}
			m_Ctrl->InsertColumn(static_cast<long>(iCol), str, fmt);
		}
		m_SortColumn.Initialize();
	}
}


void CAgilityBookRunsView::LoadData()
{
	if (!m_Ctrl)
		return;

	m_bSuppressSelect = true;

	wxBusyCursor wait;

	// Mirror the selection in the tree here.
	ARBDogRunPtr pCurRun = GetDocument()->GetCurrentRun();

	// Reduce flicker.
	m_Ctrl->Freeze();

	// Clear everything.
	m_Ctrl->DeleteAllItems();

	// Add items.
	std::vector<CVenueFilter> venues;
	CFilterOptions::Options().GetFilterVenue(venues);
	std::list<ARBDogTrialPtr> trials;
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
		for (std::list<ARBDogTrialPtr>::iterator iter = trials.begin(); iter != trials.end(); ++iter)
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

				long index = m_Ctrl->InsertItem(CAgilityBookRunsViewDataPtr(new CAgilityBookRunsViewData(this, pDog, pTrial, pRun)));
				// Compare by ptr, not value.
				if (pCurRun && pCurRun == pRun)
				{
					m_Ctrl->Select(index, true);
				}
			}
		}
	}
	int nColumnCount = m_Ctrl->GetColumnCount();
	for (int i = 0; i < nColumnCount; ++i)
		m_Ctrl->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);

	if (m_Ctrl->IsShownOnScreen())
		UpdateMessages();

	s_SortInfo.pThis = this;
	s_SortInfo.nCol = m_SortColumn.GetColumn();
	m_Ctrl->SortItems(CompareRuns, 0);
	if (0 == m_SortColumn.GetColumn())
		m_Ctrl->SetColumnSort(abs(m_SortColumn.GetColumn()), 0);
	else
		m_Ctrl->SetColumnSort(abs(m_SortColumn.GetColumn()), m_SortColumn.GetColumn());
	// Now make sure the selected item is visible.
	if (0 <= m_Ctrl->GetFirstSelected())
		m_Ctrl->Focus(m_Ctrl->GetFirstSelected());

	// Cleanup.
	m_Ctrl->Thaw();
	m_Ctrl->Refresh();

	m_bSuppressSelect = false;
}


void CAgilityBookRunsView::OnCtrlSetFocus(wxFocusEvent& evt)
{
	// We need this as clicking directly in the control does not change
	// the active view - we have to do it. Conversely, setting the active
	// view does not change the focused window, so we have to do that too.
	if (GetDocumentManager()->GetCurrentView() != this)
	{
		m_parentPanel->OnActivateView(this);
		Activate(true);
	}
}


void CAgilityBookRunsView::OnCtrlColumnClick(wxListEvent& evt)
{
	if (0 != evt.GetColumn() && m_Ctrl)
	{
		m_Ctrl->SetColumnSort(abs(m_SortColumn.GetColumn()), 0);
		int nBackwards = 1;
		if (m_SortColumn.GetColumn() == evt.GetColumn())
			nBackwards = -1;
		m_SortColumn.SetColumn(evt.GetColumn() * nBackwards);
		s_SortInfo.pThis = this;
		s_SortInfo.nCol = m_SortColumn.GetColumn();
		m_Ctrl->SortItems(CompareRuns, 0);
		m_Ctrl->SetColumnSort(abs(m_SortColumn.GetColumn()), m_SortColumn.GetColumn());
	}
}


void CAgilityBookRunsView::OnCtrlItemSelected(wxListEvent& evt)
{
	if (m_Ctrl && !m_bSuppressSelect && 1 == m_Ctrl->GetSelectedItemCount())
	{
		CAgilityBookRunsViewDataPtr pData = GetItemRunDataByData(evt.GetData());
		if (pData)
		{
			CAgilityBookTreeData* pTreeData = GetDocument()->GetTreeView()->FindData(pData->GetRun());
			if (pTreeData)
			{
				GetDocument()->GetTreeView()->SuppressSelect(true);
				GetDocument()->GetTreeView()->SelectItem(pTreeData->GetId());
				GetDocument()->GetTreeView()->SuppressSelect(false);
			}
		}
	}
	evt.Skip();
}


void CAgilityBookRunsView::OnCtrlItemActivated(wxListEvent& evt)
{
	OnCmd(ID_AGILITY_EDIT_RUN);
	evt.Skip();
}


void CAgilityBookRunsView::OnCtrlKeyDown(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
	case WXK_RETURN:
	case WXK_NUMPAD_ENTER:
		OnCmd(ID_AGILITY_EDIT_RUN);
		break;
	}
	evt.Skip();
}


void CAgilityBookRunsView::OnViewContextMenu(wxContextMenuEvent& evt)
{
	if (!m_Ctrl)
		return;
	wxPoint point;
	if (GetMenuPosition(point, *m_Ctrl, evt))
	{
		wxMenu* menu = CreatePopup(IdMenuRun);
		m_Ctrl->PopupMenu(menu, point);
		delete menu;
	}
	else
		evt.Skip();
}


void CAgilityBookRunsView::OnViewUpdateCmd(wxUpdateUIEvent& evt)
{
	switch (evt.GetId())
	{
	case wxID_CUT:
		evt.Enable(1 == m_Ctrl->GetSelectedItemCount());
		break;
	case wxID_COPY:
		evt.Enable(0 < m_Ctrl->GetSelectedItemCount());
		break;
	case wxID_SELECTALL:
		evt.Enable(m_Ctrl->CanSelectAll());
		break;
	case wxID_FIND:
	case ID_EDIT_FIND_NEXT:
	case ID_EDIT_FIND_PREVIOUS:
		evt.Enable(true);
		break;
	case ID_AGILITY_NEW_TITLE:
		evt.Enable(1 == m_Ctrl->GetSelectedItemCount());
		break;
	case ID_AGILITY_NEW_TRIAL:
		evt.Enable(1 == m_Ctrl->GetSelectedItemCount());
		break;
	case ID_AGILITY_EDIT_RUN:
		evt.Enable(1 == m_Ctrl->GetSelectedItemCount());
		break;
	case ID_AGILITY_NEW_RUN:
		evt.Enable(1 == m_Ctrl->GetSelectedItemCount());
		break;
	case ID_AGILITY_DELETE_RUN:
		evt.Enable(1 == m_Ctrl->GetSelectedItemCount());
		break;
	case ID_AGILITY_PRINT_RUNS:
		evt.Enable(0 < m_Ctrl->GetSelectedItemCount());
		break;
	case ID_VIEW_CUSTOMIZE:
		evt.Enable(true);
		break;
	case ID_VIEW_SORTRUNS:
		evt.Enable(true);
		break;
	case ID_VIEW_RUNS_BY_TRIAL:
		evt.Enable(true);
		break;
	case ID_VIEW_TABLE_IN_YPS:
		evt.Enable(true);
		break;
	}
}


bool CAgilityBookRunsView::OnCmd(int id)
{
	if (!m_Ctrl)
		return false;
	bool bHandled = true;
	switch (id)
	{
	default:
		bHandled = false;
		break;

	case wxID_CUT:
		if (1 == m_Ctrl->GetSelectedItemCount())
		{
			OnCmd(wxID_COPY);
			OnCmd(ID_AGILITY_DELETE_RUN);
		}
		break;

	case wxID_COPY:
		{
			std::vector<long> indices;
			if (0 < m_Ctrl->GetSelection(indices))
			{
				CClipboardDataWriter clpData;
				if (!clpData.isOkay())
					return true;

				wxString data;
				wxString html;
				CClipboardDataTable table(data, html);

				// Take care of the header, but only if more than one line is selected.
				if (1 < indices.size()
				|| indices.size() == static_cast<size_t>(m_Ctrl->GetItemCount()))
				{
					std::vector<wxString> line;
					m_Ctrl->GetPrintLine(-1, line);
					table.StartLine();
					for (int i = 0; i < static_cast<int>(line.size()); ++i)
					{
						table.Cell(i, line[i]);
					}
					table.EndLine();
				}

				ElementNodePtr tree(ElementNode::New(CLIPDATA));

				// Now all the data.
				for (std::vector<long>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
				{
					CAgilityBookRunsViewDataPtr pData = GetItemRunData(*iter);
					if (pData)
						pData->GetRun()->Save(tree);
					std::vector<wxString> line;
					m_Ctrl->GetPrintLine((*iter), line);
					table.StartLine();
					for (int i = 0; i < static_cast<int>(line.size()); ++i)
					{
						table.Cell(i, line[i]);
					}
					table.EndLine();
				}

				clpData.AddData(eFormatRun, tree);
				clpData.AddData(table);
				clpData.CommitData();
			}
		}
		break;

	case wxID_SELECTALL:
		m_Ctrl->SelectAll();
		break;

	case wxID_FIND:
		{
			CDlgFind dlg(m_Callback, m_Ctrl);
			dlg.ShowModal();
		}
		break;

	case ID_EDIT_FIND_NEXT:
		{
			m_Callback.SearchDown(true);
			if (m_Callback.Text().IsEmpty())
				OnCmd(wxID_FIND);
			else
				m_Callback.Search(NULL);
		}
		break;

	case ID_EDIT_FIND_PREVIOUS:
		{
			m_Callback.SearchDown(false);
			if (m_Callback.Text().IsEmpty())
				OnCmd(wxID_FIND);
			else
				m_Callback.Search(NULL);
		}
		break;

	case ID_AGILITY_NEW_TITLE:
		{
			CAgilityBookRunsViewDataPtr pData = GetItemRunData(m_Ctrl->GetSelection(true));
			if (pData)
				GetDocument()->AddTitle(pData->GetRun());
		}
		break;

	case ID_AGILITY_NEW_TRIAL:
		{
			CAgilityBookRunsViewDataPtr pData = GetItemRunData(m_Ctrl->GetSelection(true));
			if (pData)
				GetDocument()->AddTrial(pData->GetRun());
		}
		break;

	case ID_AGILITY_EDIT_RUN:
		{
			CAgilityBookRunsViewDataPtr pData = GetItemRunData(m_Ctrl->GetSelection(true));
			if (pData)
				GetDocument()->EditRun(pData->GetRun());
		}
		break;

	case ID_AGILITY_NEW_RUN:
		{
			CAgilityBookRunsViewDataPtr pData = GetItemRunData(m_Ctrl->GetSelection(true));
			if (pData)
				GetDocument()->AddRun(pData->GetRun());
		}
		break;

	case ID_AGILITY_DELETE_RUN:
		{
			CAgilityBookRunsViewDataPtr pData = GetItemRunData(m_Ctrl->GetSelection(true));
			if (pData)
				GetDocument()->DeleteRun(pData->GetRun());
		}
		break;

	case ID_AGILITY_PRINT_RUNS:
		{
			std::vector<long> indices;
			if (0 < m_Ctrl->GetSelection(indices))
			{
				ARBDogPtr dog;
				std::vector<RunInfo> runs;
				for (std::vector<long>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
				{
					CAgilityBookRunsViewDataPtr pData = GetItemRunData(*iter);
					if (pData)
					{
						dog = pData->GetDog();
						runs.push_back(RunInfo(pData->GetTrial(), pData->GetRun()));
					}
				}
				PrintRuns(&(GetDocument()->Book().GetConfig()), dog, runs);
			}
		}
		break;

	case ID_VIEW_CUSTOMIZE:
		{
			CDlgAssignColumns dlg(CAgilityBookOptions::eView, m_Ctrl, GetDocument(), IO_TYPE_VIEW_RUNS_LIST);
			dlg.ShowModal();
		}
		break;

	case ID_VIEW_SORTRUNS:
		{
			CAgilityBookOptions::SetNewestDatesFirst(!CAgilityBookOptions::GetNewestDatesFirst());
			GetDocument()->SortDates();
			// Note, sorting the trials doesn't affect this view.
			CUpdateHint hint(UPDATE_TREE_VIEW);
			GetDocument()->UpdateAllViews(NULL, &hint);
		}
		break;

	case ID_VIEW_RUNS_BY_TRIAL:
		CAgilityBookOptions::SetViewRunsByTrial(!CAgilityBookOptions::GetViewRunsByTrial());
		LoadData();
		break;

	case ID_VIEW_TABLE_IN_YPS:
		CAgilityBookOptions::SetTableInYPS(!CAgilityBookOptions::GetTableInYPS());
		wxGetApp().GetTopWindow()->Refresh();
		break;
	}
	return bHandled;
}


void CAgilityBookRunsView::OnViewCmd(wxCommandEvent& evt)
{
	OnCmd(evt.GetId());
}


void CAgilityBookRunsView::OnPrint(wxCommandEvent& evt)
{
	wxGetApp().GetHtmlPrinter()->PrintText(m_Ctrl->GetPrintDataAsHtmlTable());
}


void CAgilityBookRunsView::OnPreview(wxCommandEvent& evt)
{
	wxGetApp().GetHtmlPrinter()->PreviewText(m_Ctrl->GetPrintDataAsHtmlTable());
}
