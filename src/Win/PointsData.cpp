/*
 * Copyright © 2004-2005 David Connet. All Rights Reserved.
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
 * @brief Data classes behind the points view items.
 * @author David Connet
 *
 * Revision History
 * @li 2005-01-02 DRC Show existing points in the list viewer.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-12-03 DRC Show all lifetime points when filtering.
 * @li 2004-08-06 DRC Created
 */

#include "stdafx.h"
#include "PointsData.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookViewPoints.h"
#include "ARBDate.h"
#include "ARBDog.h"
#include "DlgDog.h"
#include "DlgListViewer.h"
#include "DlgTitle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

OtherPtInfo::OtherPtInfo(ARBDogTrial const* pTrial, ARBDogRun const* pRun, int score)
	: m_pTrial(pTrial)
	, m_pRun(pRun)
	, m_pExisting(NULL)
	, m_Score(score)
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

OtherPtInfo::OtherPtInfo(ARBDogExistingPoints const* pExisting)
	: m_pTrial(NULL)
	, m_pRun(NULL)
	, m_pExisting(pExisting)
	, m_Venue(pExisting->GetVenue())
	, m_Div(pExisting->GetDivision())
	, m_Level(pExisting->GetLevel())
	, m_Event(pExisting->GetEvent())
	, m_Score(pExisting->GetPoints())
{
}

/////////////////////////////////////////////////////////////////////////////

PointsDataBase::PointsDataBase(CAgilityBookViewPoints* pView)
	: m_RefCount(1)
	, m_pView(pView)
{
}

PointsDataBase::~PointsDataBase()
{
}

void PointsDataBase::AddRef()
{
	++m_RefCount;
}

void PointsDataBase::Release()
{
	if (0 == --m_RefCount)
		delete this;
}

/////////////////////////////////////////////////////////////////////////////

PointsDataDog::PointsDataDog(CAgilityBookViewPoints* pView, ARBDog* pDog)
	: PointsDataBase(pView)
	, m_pDog(pDog)
{
	if (m_pDog)
		m_pDog->AddRef();
}

PointsDataDog::~PointsDataDog()
{
	if (m_pDog)
		m_pDog->Release();
}

std::string PointsDataDog::OnNeedText(size_t index) const
{
	std::string str;
	if (m_pDog)
	{
		switch (index)
		{
		case 0: // Call name
			str = m_pDog->GetCallName();
			break;
		case 1: // Full name
			str = m_pDog->GetRegisteredName();
			break;
		case 7: // Current date
			{
				ARBDate today(ARBDate::Today());
				str = today.GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints));
			}
			break;
		}
	}
	return str;
}

void PointsDataDog::OnDblClick() const
{
	// CDlgDog will cause an update msg to occur which will delete us.
	// So we need to cache the document in a stack variable.
	CAgilityBookDoc* pDoc = m_pView->GetDocument();
	CDlgDog dlg(pDoc, m_pDog, m_pView, 0);
	if (IDOK == dlg.DoModal())
	{
		pDoc->SetModifiedFlag();
		pDoc->UpdateAllViews(NULL, UPDATE_POINTS_VIEW|UPDATE_RUNS_VIEW);
	}
}

/////////////////////////////////////////////////////////////////////////////

PointsDataVenue::PointsDataVenue(CAgilityBookViewPoints* pView, ARBDog* pDog, ARBConfigVenue* pVenue)
	: PointsDataBase(pView)
	, m_pDog(pDog)
	, m_pVenue(pVenue)
{
	if (m_pDog)
		m_pDog->AddRef();
	if (m_pVenue)
		m_pVenue->AddRef();
}

PointsDataVenue::~PointsDataVenue()
{
	if (m_pDog)
		m_pDog->Release();
	if (m_pVenue)
		m_pVenue->Release();
}

std::string PointsDataVenue::OnNeedText(size_t index) const
{
	std::string str;
	if (m_pVenue)
	{
		switch (index)
		{
		case 0:
			str = m_pVenue->GetName();
			break;
		case 1:
			if (m_pDog)
			{
				ARBDogRegNum const* pRegNum = m_pDog->GetRegNums().FindRegNum(m_pVenue->GetName());
				if (pRegNum)
				{
					str = "[" + pRegNum->GetNumber() + "]";
				}
			}
			break;
		}
	}
	return str;
}

void PointsDataVenue::OnDblClick() const
{
	if (m_pDog && m_pVenue)
	{
		ARBDogRegNum const* pRegNum = m_pDog->GetRegNums().FindRegNum(m_pVenue->GetName());
		if (pRegNum)
		{
			// CDlgDog will cause an update msg to occur which will delete us.
			// So we need to cache the document in a stack variable.
			CAgilityBookDoc* pDoc = m_pView->GetDocument();
			CDlgDog dlg(pDoc, m_pDog, m_pView, 2);
			if (IDOK == dlg.DoModal())
			{
				pDoc->SetModifiedFlag();
				pDoc->UpdateAllViews(NULL, UPDATE_POINTS_VIEW|UPDATE_RUNS_VIEW);
			}
		}
		else
			MessageBeep(0);
	}
	else
		MessageBeep(0);
}

/////////////////////////////////////////////////////////////////////////////

PointsDataTitle::PointsDataTitle(CAgilityBookViewPoints* pView, ARBDog* pDog, ARBDogTitle* pTitle)
	: PointsDataBase(pView)
	, m_pDog(pDog)
	, m_pTitle(pTitle)
{
	if (m_pDog)
		m_pDog->AddRef();
	if (m_pTitle)
		m_pTitle->AddRef();
}

PointsDataTitle::~PointsDataTitle()
{
	if (m_pDog)
		m_pDog->Release();
	if (m_pTitle)
		m_pTitle->Release();
}

std::string PointsDataTitle::OnNeedText(size_t index) const
{
	std::string str;
	if (m_pTitle)
	{
		switch (index)
		{
		case 1:
			str = m_pTitle->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints));
			break;
		case 2:
			str = m_pView->GetDocument()->GetConfig().GetTitleCompleteName(m_pTitle->GetVenue(), m_pTitle->GetName(), false);
			if (m_pTitle->GetReceived())
				str += "*";
			break;
		}
	}
	return str;
}

void PointsDataTitle::OnDblClick() const
{
	CAgilityBookDoc* pDoc = m_pView->GetDocument();
	CDlgTitle dlg(pDoc->GetConfig(), m_pDog->GetTitles(), m_pTitle, m_pView);
	if (IDOK == dlg.DoModal())
	{
		pDoc->SetModifiedFlag();
		std::vector<CVenueFilter> venues;
		CAgilityBookOptions::GetFilterVenue(venues);
		pDoc->ResetVisibility(venues, m_pTitle);
		pDoc->UpdateAllViews(NULL, UPDATE_POINTS_VIEW);
	}
}

/////////////////////////////////////////////////////////////////////////////

PointsDataEvent::PointsDataEvent(CAgilityBookViewPoints* pView,
		ARBDog const* inDog,
		std::list<RunInfo>& inMatching,
		ARBConfigVenue const* inVenue,
		ARBConfigDivision const* inDiv,
		ARBConfigLevel const* inLevel,
		ARBConfigEvent const* inEvent,
		std::string const& inRunCount,
		std::string const& inQcount,
		std::string const& inPts,
		std::string const& inSuperQ,
		std::string const& inSpeed)
	: PointsDataBase(pView)
	, m_Dog(inDog)
	, m_Matching(inMatching)
	, m_Venue(inVenue)
	, m_Div(inDiv)
	, m_Level(inLevel)
	, m_Event(inEvent)
	, m_RunCount(inRunCount)
	, m_Qcount(inQcount)
	, m_Pts(inPts)
	, m_SuperQ(inSuperQ)
	, m_Speed(inSpeed)
{
}

std::string PointsDataEvent::OnNeedText(size_t index) const
{
	std::string str;
	switch (index)
	{
	case 1: // Division
		str = m_Div->GetName();
		break;
	case 2: // Level
		str = m_Level->GetName();
		break;
	case 3: // Event
		str = m_Event->GetName();
		break;
	case 4: // Runs, judges, partners
		str = m_RunCount;
		break;
	case 5: // Q counts
		str = m_Qcount;
		break;
	case 6: // Points
		str = m_Pts;
		break;
	case 7: // SuperQ/SpeedPts
		if (0 < m_SuperQ.length())
			str = m_SuperQ;
		else if (0 < m_Speed.length())
			str = m_Speed;
	case 8: // Only used for SQ AND Speed
		if (0 < m_SuperQ.length() &&  0 < m_Speed.length())
			str = m_Speed;
		break;
	}
	return str;
}

void PointsDataEvent::OnDblClick() const
{
	std::string str("Runs: ");
	str += m_Div->GetName();
	str += "/";
	str += m_Level->GetName();
	str += "/";
	str += m_Event->GetName();
	RunInfoData data(m_Dog, m_Venue, m_Div, m_Level, m_Event);
	CDlgListViewer dlg(m_pView->GetDocument(), str.c_str(), m_Dog ? &data : NULL, m_Matching, m_pView);
	dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////

PointsDataLifetime::PointsDataLifetime(CAgilityBookViewPoints* pView,
	int inLifetime,
	int inFiltered)
	: PointsDataBase(pView)
	, m_Lifetime(inLifetime)
	, m_Filtered(inFiltered)
{
}

std::string PointsDataLifetime::OnNeedText(size_t index) const
{
	std::string str;
	switch (index)
	{
	case 1:
		{
			CString str2;
			str2.LoadString(IDS_LIFETIME_POINTS);
			str = (LPCTSTR)str2;
		}
		break;
	case 2:
		{
			CString str2;
			if (0 < m_Filtered)
				str2.Format("%d (%d)", m_Lifetime - m_Filtered, m_Lifetime);
			else
				str2.Format("%d", m_Lifetime);
			str = (LPCTSTR)str2;
		}
		break;
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

PointsDataDoubleQs::PointsDataDoubleQs(CAgilityBookViewPoints* pView, int inExistingDblQs, std::set<DoubleQdata> const& inQQs)
	: PointsDataBase(pView)
	, m_ExistingDblQs(inExistingDblQs)
	, m_QQs(inQQs)
{
}

std::string PointsDataDoubleQs::OnNeedText(size_t index) const
{
	std::string str;
	if (7 == index)
	{
		CString str2;
		str2.FormatMessage(IDS_POINTS_QQS, m_ExistingDblQs + m_QQs.size());
		str = (LPCTSTR)str2;
	}
	return str;
}

void PointsDataDoubleQs::OnDblClick() const
{
	CDlgListViewer dlg(m_pView->GetDocument(), "Double Qs", m_QQs, m_pView);
	dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////

PointsDataSpeedPts::PointsDataSpeedPts(CAgilityBookViewPoints* pView, int inPts)
	: PointsDataBase(pView)
	, m_Pts(inPts)
{
}

std::string PointsDataSpeedPts::OnNeedText(size_t index) const
{
	std::string str;
	if (7 == index)
	{
		CString str2;
		str2.FormatMessage(IDS_POINTS_SPEED, m_Pts);
		str = (LPCTSTR)str2;
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

PointsDataOtherPoints::PointsDataOtherPoints(CAgilityBookViewPoints* pView, std::list<OtherPtInfo> const& inRunList)
	: PointsDataBase(pView)
	, m_Score(0)
	, m_RunList(inRunList)
{
	for (std::list<OtherPtInfo>::iterator iter = m_RunList.begin();
		iter != m_RunList.end();
		++iter)
	{
		m_Score += (*iter).m_Score;
	}
}

/////////////////////////////////////////////////////////////////////////////

PointsDataOtherPointsTallyAll::PointsDataOtherPointsTallyAll(CAgilityBookViewPoints* pView, std::string const& inName, std::list<OtherPtInfo> const& inRunList)
	: PointsDataOtherPoints(pView, inRunList)
	, m_Name(inName)
{
}

std::string PointsDataOtherPointsTallyAll::OnNeedText(size_t index) const
{
	std::string str;
	switch (index)
	{
	case 1:
		str = m_Name;
		break;
	case 2:
		{
			CString str2;
			str2.Format("%d", m_Score);
			str = (LPCTSTR)str2;
		}
		break;
	}
	return str;
}

void PointsDataOtherPointsTallyAll::OnDblClick() const
{
	CDlgListViewer dlg(m_pView->GetDocument(), "Other Points", m_RunList, m_pView);
	dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////

PointsDataOtherPointsTallyAllByEvent::PointsDataOtherPointsTallyAllByEvent(CAgilityBookViewPoints* pView, std::list<OtherPtInfo> const& inRunList)
	: PointsDataOtherPoints(pView, inRunList)
{
}

std::string PointsDataOtherPointsTallyAllByEvent::OnNeedText(size_t index) const
{
	std::string str;
	switch (index)
	{
	case 2: // Event
		str = m_RunList.begin()->m_Event;
		break;
	case 3:
		{
			CString str2;
			str2.Format("%d", m_Score);
			str = (LPCTSTR)str2;
		}
		break;
	}
	return str;
}

void PointsDataOtherPointsTallyAllByEvent::OnDblClick() const
{
	CDlgListViewer dlg(m_pView->GetDocument(), "Other Points", m_RunList, m_pView);
	dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////

PointsDataOtherPointsTallyLevel::PointsDataOtherPointsTallyLevel(CAgilityBookViewPoints* pView, std::list<OtherPtInfo> const& inRunList)
	: PointsDataOtherPoints(pView, inRunList)
{
}

std::string PointsDataOtherPointsTallyLevel::OnNeedText(size_t index) const
{
	std::string str;
	switch (index)
	{
	case 2: // Level
		str = m_RunList.begin()->m_Level;
		break;
	case 3:
		{
			CString str2;
			str2.Format("%d", m_Score);
			str = (LPCTSTR)str2;
		}
		break;
	}
	return str;
}

void PointsDataOtherPointsTallyLevel::OnDblClick() const
{
	CDlgListViewer dlg(m_pView->GetDocument(), "Other Points", m_RunList, m_pView);
	dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////

PointsDataOtherPointsTallyLevelByEvent::PointsDataOtherPointsTallyLevelByEvent(CAgilityBookViewPoints* pView, std::list<OtherPtInfo> const& inRunList)
	: PointsDataOtherPoints(pView, inRunList)
{
}

std::string PointsDataOtherPointsTallyLevelByEvent::OnNeedText(size_t index) const
{
	std::string str;
	switch (index)
	{
	case 2: // Level
		str = m_RunList.begin()->m_Level;
		break;
	case 3: // Event
		str = m_RunList.begin()->m_Event;
		break;
	case 4:
		{
			CString str2;
			str2.Format("%d", m_Score);
			str = (LPCTSTR)str2;
		}
		break;
	}
	return str;
}

void PointsDataOtherPointsTallyLevelByEvent::OnDblClick() const
{
	CDlgListViewer dlg(m_pView->GetDocument(), "Other Points", m_RunList, m_pView);
	dlg.DoModal();
}
