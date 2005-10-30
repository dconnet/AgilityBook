/*
 * Copyright � 2004-2005 David Connet. All Rights Reserved.
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
 * @li 2005-10-18 DRC Remember last selected item when reloading data.
 * @li 2005-10-14 DRC Added a context menu.
 * @li 2005-05-04 DRC Added subtotaling by division to lifetime points.
 * @li 2005-03-14 DRC Show a summary of lifetime points in the list viewer.
 * @li 2005-01-10 DRC Allow titles to be optionally entered multiple times.
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

LifeTimePointInfo::LifeTimePointInfo(
		ARBString const& inDiv,
		ARBString const& inLevel,
		int inPoints,
		int inFiltered)
	: div(inDiv)
	, level(inLevel)
	, points(inPoints)
	, filtered(inFiltered)
{
}

/////////////////////////////////////////////////////////////////////////////

OtherPtInfo::OtherPtInfo(
		ARBDogTrial const* pTrial,
		ARBDogRun const* pRun,
		int score)
	: m_pTrial(pTrial)
	, m_pRun(pRun)
	, m_pExisting(NULL)
	, m_Score(score)
{
	if (pTrial)
		m_Venue = pTrial->GetClubs().GetPrimaryClubVenue();
	if (pRun)
	{
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

PointsDataDog::PointsDataDog(
		CAgilityBookViewPoints* pView,
		ARBDog* pDog)
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

ARBString PointsDataDog::OnNeedText(size_t index) const
{
	ARBString str;
	if (m_pDog)
	{
		switch (index)
		{
		case 1: // Call name
			str = m_pDog->GetCallName();
			break;
		case 2: // Full name
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

void PointsDataDog::Details() const
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

bool PointsDataDog::IsEqual(PointsDataBase const* inData)
{
	PointsDataDog const* inDog = dynamic_cast<PointsDataDog const*>(inData);
	if (inDog)
		return m_pDog->GetCallName() == inDog->m_pDog->GetCallName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

PointsDataVenue::PointsDataVenue(
		CAgilityBookViewPoints* pView,
		ARBDog* pDog,
		ARBConfigVenue* pVenue)
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

ARBString PointsDataVenue::OnNeedText(size_t index) const
{
	ARBString str;
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
				ARBDogRegNum* pRegNum;
				if (m_pDog->GetRegNums().FindRegNum(m_pVenue->GetName(), &pRegNum))
				{
					str = _T("[") + pRegNum->GetNumber() + _T("]");
					pRegNum->Release();
				}
			}
			break;
		}
	}
	return str;
}

void PointsDataVenue::Details() const
{
	if (m_pDog && m_pVenue)
	{
		ARBDogRegNum* pRegNum;
		if (m_pDog->GetRegNums().FindRegNum(m_pVenue->GetName(), &pRegNum))
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
			pRegNum->Release();
		}
		else
			MessageBeep(0);
	}
	else
		MessageBeep(0);
}

bool PointsDataVenue::IsEqual(PointsDataBase const* inData)
{
	PointsDataVenue const* inVenue = dynamic_cast<PointsDataVenue const*>(inData);
	if (inVenue)
		return m_pVenue->GetName() == inVenue->m_pVenue->GetName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

PointsDataTitle::PointsDataTitle(
		CAgilityBookViewPoints* pView,
		ARBDog* pDog,
		ARBDogTitle* pTitle)
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

ARBString PointsDataTitle::OnNeedText(size_t index) const
{
	ARBString str;
	if (m_pTitle)
	{
		switch (index)
		{
		case 1:
			str = m_pTitle->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints));
			break;
		case 2:
			str = m_pView->GetDocument()->GetConfig().GetTitleCompleteName(m_pTitle, false);
			if (m_pTitle->GetReceived())
				str += _T("*");
			break;
		}
	}
	return str;
}

void PointsDataTitle::Details() const
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

bool PointsDataTitle::IsEqual(PointsDataBase const* inData)
{
	PointsDataTitle const* inTitle = dynamic_cast<PointsDataTitle const*>(inData);
	if (inTitle)
		return m_pTitle->GetVenue() == inTitle->m_pTitle->GetVenue()
			&& m_pTitle->GetRawName() == inTitle->m_pTitle->GetRawName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

PointsDataEvent::PointsDataEvent(
		CAgilityBookViewPoints* pView,
		ARBDog const* inDog,
		std::list<RunInfo>& inMatching,
		ARBConfigVenue const* inVenue,
		ARBConfigDivision const* inDiv,
		ARBConfigLevel const* inLevel,
		ARBConfigEvent const* inEvent,
		ARBString const& inRunCount,
		ARBString const& inQcount,
		ARBString const& inPts,
		ARBString const& inSuperQ,
		ARBString const& inSpeed)
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

ARBString PointsDataEvent::OnNeedText(size_t index) const
{
	ARBString str;
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

void PointsDataEvent::Details() const
{
	ARBString str(_T("Runs: "));
	str += m_Div->GetName();
	str += _T("/");
	str += m_Level->GetName();
	str += _T("/");
	str += m_Event->GetName();
	RunInfoData data(m_Dog, m_Venue, m_Div, m_Level, m_Event);
	CDlgListViewer dlg(m_pView->GetDocument(), str.c_str(), m_Dog ? &data : NULL, m_Matching, m_pView);
	dlg.DoModal();
}

bool PointsDataEvent::IsEqual(PointsDataBase const* inData)
{
	PointsDataEvent const* inEvent = dynamic_cast<PointsDataEvent const*>(inData);
	if (inEvent)
		return m_Venue->GetName() == inEvent->m_Venue->GetName()
			&& m_Div->GetName() == inEvent->m_Div->GetName()
			&& m_Level->GetName() == inEvent->m_Level->GetName()
			&& m_Event->GetName() == inEvent->m_Event->GetName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

PointsDataLifetime::PointsDataLifetime(
		CAgilityBookViewPoints* pView,
		ARBString const& inVenue)
	: PointsDataBase(pView)
	, m_Venue(inVenue.c_str())
	, m_Lifetime(0)
	, m_Filtered(0)
{
}

void PointsDataLifetime::AddLifetimeInfo(
		ARBString const& inDiv,
		ARBString const& inLevel,
		int inLifetime,
		int inFiltered)
{
	m_Data.push_back(LifeTimePointInfo(inDiv, inLevel, inLifetime, inFiltered));
	m_Lifetime += inLifetime;
	m_Filtered += inFiltered;
}

ARBString PointsDataLifetime::OnNeedText(size_t index) const
{
	ARBString str;
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
				str2.Format(_T("Total: %d (%d)"), m_Lifetime - m_Filtered, m_Lifetime);
			else
				str2.Format(_T("Total: %d"), m_Lifetime);
			str = (LPCTSTR)str2;
		}
		break;
	}
	return str;
}

void PointsDataLifetime::Details() const
{
	CString caption(m_Venue);
	caption += _T(" Lifetime Points");
	CDlgListViewer dlg(m_pView->GetDocument(), caption, m_Data, m_pView);
	dlg.DoModal();
}

bool PointsDataLifetime::IsEqual(PointsDataBase const* inData)
{
	PointsDataLifetime const* inLife = dynamic_cast<PointsDataLifetime const*>(inData);
	if (inLife)
		return m_Venue == inLife->m_Venue;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

PointsDataLifetimeDiv::PointsDataLifetimeDiv(
		CAgilityBookViewPoints* pView,
		ARBString const& inVenue,
		ARBString const& inDiv)
	: PointsDataLifetime(pView, inVenue)
	, m_Div(inDiv)
{
}

void PointsDataLifetimeDiv::AddLifetimeInfo(
		ARBString const& inDiv,
		ARBString const& inLevel,
		int inLifetime,
		int inFiltered)
{
	if (inDiv == m_Div)
	{
		m_Data.push_back(LifeTimePointInfo(inDiv, inLevel, inLifetime, inFiltered));
		m_Lifetime += inLifetime;
		m_Filtered += inFiltered;
	}
}

ARBString PointsDataLifetimeDiv::OnNeedText(size_t index) const
{
	ARBString str;
	switch (index)
	{
	case 2:
		{
			CString str2;
			if (0 < m_Filtered)
				str2.Format(_T("%s: %d (%d)"), m_Div.c_str(), m_Lifetime - m_Filtered, m_Lifetime);
			else
				str2.Format(_T("%s: %d"), m_Div.c_str(), m_Lifetime);
			str = (LPCTSTR)str2;
		}
		break;
	}
	return str;
}

bool PointsDataLifetimeDiv::IsEqual(PointsDataBase const* inData)
{
	PointsDataLifetimeDiv const* inLife = dynamic_cast<PointsDataLifetimeDiv const*>(inData);
	if (inLife)
		return PointsDataLifetimeDiv::IsEqual(inData)
			&& m_Div == inLife->m_Div;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

PointsDataMultiQs::PointsDataMultiQs(
		CAgilityBookViewPoints* pView,
		ARBDog* inDog,
		ARBConfigVenue* inVenue,
		ARBConfigMultiQ const* inMultiQ,
		std::set<MultiQdata> const& inMQs)
	: PointsDataBase(pView)
	, m_Dog(inDog)
	, m_Venue(inVenue)
	, m_MultiQ(inMultiQ)
	, m_MQs(inMQs)
	, m_ExistingDblQs(0)
{
	m_ExistingDblQs = m_Dog->GetExistingPoints().ExistingPoints(
		ARBDogExistingPoints::eMQ,
		m_Venue, m_MultiQ, NULL, NULL, NULL);
}

ARBString PointsDataMultiQs::OnNeedText(size_t index) const
{
	ARBString str;
	if (7 == index)
	{
		CString str2;
		str2.FormatMessage(_T("%1!d! %2"), m_ExistingDblQs + m_MQs.size(), m_MultiQ->GetShortName().c_str());
		str = (LPCTSTR)str2;
	}
	return str;
}

void PointsDataMultiQs::Details() const
{
	MultiQInfoData data(m_Dog, m_Venue, m_MultiQ);
	CDlgListViewer dlg(m_pView->GetDocument(), m_MultiQ->GetName().c_str(), &data, m_MQs, m_pView);
	dlg.DoModal();
}

bool PointsDataMultiQs::IsEqual(PointsDataBase const* inData)
{
	PointsDataMultiQs const* inMulti = dynamic_cast<PointsDataMultiQs const*>(inData);
	if (inMulti)
		return m_Venue->GetName() == inMulti->m_Venue->GetName()
			&& m_MultiQ->GetName() == inMulti->m_MultiQ->GetName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

PointsDataSpeedPts::PointsDataSpeedPts(
		CAgilityBookViewPoints* pView,
		ARBConfigVenue* inVenue,
		int inPts)
	: PointsDataBase(pView)
	, m_Venue(inVenue)
	, m_Pts(inPts)
{
}

ARBString PointsDataSpeedPts::OnNeedText(size_t index) const
{
	ARBString str;
	if (7 == index)
	{
		CString str2;
		str2.FormatMessage(IDS_POINTS_SPEED, m_Pts);
		str = (LPCTSTR)str2;
	}
	return str;
}

bool PointsDataSpeedPts::IsEqual(PointsDataBase const* inData)
{
	PointsDataSpeedPts const* inPts = dynamic_cast<PointsDataSpeedPts const*>(inData);
	if (inPts)
		return m_Venue->GetName() == inPts->m_Venue->GetName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

PointsDataOtherPoints::PointsDataOtherPoints(
		CAgilityBookViewPoints* pView,
		std::list<OtherPtInfo> const& inRunList)
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

PointsDataOtherPointsTallyAll::PointsDataOtherPointsTallyAll(
		CAgilityBookViewPoints* pView,
		ARBString const& inName,
		std::list<OtherPtInfo> const& inRunList)
	: PointsDataOtherPoints(pView, inRunList)
	, m_Name(inName)
{
}

ARBString PointsDataOtherPointsTallyAll::OnNeedText(size_t index) const
{
	ARBString str;
	switch (index)
	{
	case 1:
		str = m_Name;
		break;
	case 2:
		{
			CString str2;
			str2.Format(_T("%d"), m_Score);
			str = (LPCTSTR)str2;
		}
		break;
	}
	return str;
}

void PointsDataOtherPointsTallyAll::Details() const
{
	CDlgListViewer dlg(m_pView->GetDocument(), _T("Other Points"), m_RunList, m_pView);
	dlg.DoModal();
}

bool PointsDataOtherPointsTallyAll::IsEqual(PointsDataBase const* inData)
{
	PointsDataOtherPointsTallyAll const* inPts = dynamic_cast<PointsDataOtherPointsTallyAll const*>(inData);
	if (inPts)
		return m_Name == inPts->m_Name;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

PointsDataOtherPointsTallyAllByEvent::PointsDataOtherPointsTallyAllByEvent(
		CAgilityBookViewPoints* pView,
		ARBString const& inEvent,
		std::list<OtherPtInfo> const& inRunList)
	: PointsDataOtherPoints(pView, inRunList)
	, m_Event(inEvent)
{
}

ARBString PointsDataOtherPointsTallyAllByEvent::OnNeedText(size_t index) const
{
	ARBString str;
	switch (index)
	{
	case 2: // Event
		str = m_RunList.begin()->m_Event;
		break;
	case 3:
		{
			CString str2;
			str2.Format(_T("%d"), m_Score);
			str = (LPCTSTR)str2;
		}
		break;
	}
	return str;
}

void PointsDataOtherPointsTallyAllByEvent::Details() const
{
	CDlgListViewer dlg(m_pView->GetDocument(), _T("Other Points"), m_RunList, m_pView);
	dlg.DoModal();
}

bool PointsDataOtherPointsTallyAllByEvent::IsEqual(PointsDataBase const* inData)
{
	PointsDataOtherPointsTallyAllByEvent const* inPts = dynamic_cast<PointsDataOtherPointsTallyAllByEvent const*>(inData);
	if (inPts)
		return m_Event == inPts->m_Event;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

PointsDataOtherPointsTallyLevel::PointsDataOtherPointsTallyLevel(
		CAgilityBookViewPoints* pView,
		ARBString const& inLevel,
		std::list<OtherPtInfo> const& inRunList)
	: PointsDataOtherPoints(pView, inRunList)
	, m_Level(inLevel)
{
}

ARBString PointsDataOtherPointsTallyLevel::OnNeedText(size_t index) const
{
	ARBString str;
	switch (index)
	{
	case 2: // Level
		str = m_RunList.begin()->m_Level;
		break;
	case 3:
		{
			CString str2;
			str2.Format(_T("%d"), m_Score);
			str = (LPCTSTR)str2;
		}
		break;
	}
	return str;
}

void PointsDataOtherPointsTallyLevel::Details() const
{
	CDlgListViewer dlg(m_pView->GetDocument(), _T("Other Points"), m_RunList, m_pView);
	dlg.DoModal();
}

bool PointsDataOtherPointsTallyLevel::IsEqual(PointsDataBase const* inData)
{
	PointsDataOtherPointsTallyLevel const* inPts = dynamic_cast<PointsDataOtherPointsTallyLevel const*>(inData);
	if (inPts)
		return m_Level == inPts->m_Level;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

PointsDataOtherPointsTallyLevelByEvent::PointsDataOtherPointsTallyLevelByEvent(
		CAgilityBookViewPoints* pView,
		ARBString const& inLevel,
		ARBString const& inEvent,
		std::list<OtherPtInfo> const& inRunList)
	: PointsDataOtherPoints(pView, inRunList)
	, m_Level(inLevel)
	, m_Event(inEvent)
{
}

ARBString PointsDataOtherPointsTallyLevelByEvent::OnNeedText(size_t index) const
{
	ARBString str;
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
			str2.Format(_T("%d"), m_Score);
			str = (LPCTSTR)str2;
		}
		break;
	}
	return str;
}

void PointsDataOtherPointsTallyLevelByEvent::Details() const
{
	CDlgListViewer dlg(m_pView->GetDocument(), _T("Other Points"), m_RunList, m_pView);
	dlg.DoModal();
}

bool PointsDataOtherPointsTallyLevelByEvent::IsEqual(PointsDataBase const* inData)
{
	PointsDataOtherPointsTallyLevelByEvent const* inPts = dynamic_cast<PointsDataOtherPointsTallyLevelByEvent const*>(inData);
	if (inPts)
		return m_Level == inPts->m_Level
			&& m_Event == inPts->m_Event;
	else
		return false;
}
