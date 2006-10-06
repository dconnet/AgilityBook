/*
 * Copyright � 2004-2006 David Connet. All Rights Reserved.
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
 * @li 2006-07-15 DRC Add option to sort by event instead of division.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
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

#include <map>
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookViewPoints.h"
#include "ARBDate.h"
#include "ARBDog.h"
#include "DlgDog.h"
#include "DlgListViewer.h"
#include "DlgTitle.h"
#include "FilterOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

LifeTimePointInfo::LifeTimePointInfo(
		ARBString const& inDiv,
		ARBString const& inLevel,
		double inPoints,
		double inFiltered)
	: div(inDiv)
	, level(inLevel)
	, points(inPoints)
	, filtered(inFiltered)
{
}

/////////////////////////////////////////////////////////////////////////////

OtherPtInfo::OtherPtInfo(
		ARBDogTrialPtr pTrial,
		ARBDogRunPtr pRun,
		double score)
	: m_pTrial(pTrial)
	, m_pRun(pRun)
	, m_pExisting()
	, m_Score(score)
{
	if (m_pTrial)
	{
		m_Venue = m_pTrial->GetClubs().GetPrimaryClubVenue();
	}
	if (m_pRun)
	{
		m_Div = m_pRun->GetDivision();
		m_Level = m_pRun->GetLevel();
		m_Event = m_pRun->GetEvent();
	}
}

OtherPtInfo::OtherPtInfo(ARBDogExistingPointsPtr pExisting)
	: m_pTrial()
	, m_pRun()
	, m_pExisting(pExisting)
	, m_Venue(pExisting->GetVenue())
	, m_Div(pExisting->GetDivision())
	, m_Level(pExisting->GetLevel())
	, m_Event(pExisting->GetEvent())
	, m_Score(pExisting->GetPoints())
{
}

OtherPtInfo::~OtherPtInfo()
{
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataBase::CPointsDataBase(
		CWnd* pParent,
		CAgilityBookDoc* pDoc)
	: m_pParent(pParent)
	, m_pDoc(pDoc)
{
}

CPointsDataBase::~CPointsDataBase()
{
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataSeparator::CPointsDataSeparator(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		ARBString const& inHtml)
	: CPointsDataBase(pParent, pDoc)
	, m_Html(inHtml)
{
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataText::CPointsDataText(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		LPCTSTR inCol1,
		LPCTSTR inCol2)
	: CPointsDataBase(pParent, pDoc)
	, m_Col1(inCol1)
	, m_Col2(inCol2)
{
	ASSERT(NULL != inCol1);
	ASSERT(NULL != inCol2);
}

ARBString CPointsDataText::OnNeedText(size_t inCol) const
{
	switch (inCol)
	{
	default:
		return _T("");
	case 0:
		return m_Col1;
	case 1:
		return m_Col2;
	}
}

ARBString CPointsDataText::GetHtml(size_t /*nCurLine*/) const
{
	ARBostringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td>") << OnNeedText(0) << _T("</td>") << std::endl
		<< _T("<td>") << OnNeedText(1) << _T("</td>") << std::endl
		<< _T("</tr>") << std::endl;
	return data.str();
}

bool CPointsDataText::IsEqual(CPointsDataBasePtr /*inData*/)
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataDog::CPointsDataDog(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		ARBDogPtr pDog)
	: CPointsDataBase(pParent, pDoc)
	, m_pDog(pDog)
{
}

ARBString CPointsDataDog::OnNeedText(size_t inCol) const
{
	ARBString str;
	if (m_pDog)
	{
		switch (inCol)
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

ARBString CPointsDataDog::GetHtml(size_t nCurLine) const
{
	ARBostringstream data;
	if (m_pDog)
	{
		data << _T("<h1><a href=\"")
			<< ARB_PROTOCOL << nCurLine << _T("\">")
			<< m_pDog->GetCallName()
			<< _T("</a> [")
			<< m_pDog->GetRegisteredName()
			<< _T(']')
			<< _T("</h1>") << std::endl;
	}
	return data.str();
}

void CPointsDataDog::Details() const
{
	// CDlgDog will cause an update msg to occur which will delete us.
	// So we need to cache the document in a stack variable.
	CDlgDog dlg(m_pDoc, m_pDog, m_pParent, 0);
	if (IDOK == dlg.DoModal())
	{
		m_pDoc->SetModifiedFlag();
		m_pDoc->UpdateAllViews(NULL, UPDATE_POINTS_VIEW|UPDATE_RUNS_VIEW);
	}
}

bool CPointsDataDog::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataDogPtr inDog = boost::shared_dynamic_cast<CPointsDataDog, CPointsDataBase>(inData);
	if (inDog)
		return m_pDog->GetCallName() == inDog->m_pDog->GetCallName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataVenue::CPointsDataVenue(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		ARBDogPtr pDog,
		ARBConfigVenuePtr pVenue)
	: CPointsDataBase(pParent, pDoc)
	, m_pDog(pDog)
	, m_pVenue(pVenue)
{
}

ARBString CPointsDataVenue::OnNeedText(size_t inCol) const
{
	ARBString str;
	if (m_pVenue)
	{
		switch (inCol)
		{
		case 0:
			str = m_pVenue->GetName();
			break;
		case 1:
			if (m_pDog)
			{
				ARBDogRegNumPtr pRegNum;
				if (m_pDog->GetRegNums().FindRegNum(m_pVenue->GetName(), &pRegNum))
				{
					str = _T("[") + pRegNum->GetNumber() + _T("]");
				}
			}
			break;
		}
	}
	return str;
}

ARBString CPointsDataVenue::GetHtml(size_t nCurLine) const
{
	ARBostringstream data;
	if (m_pVenue)
	{
		data << _T("<tr>") << std::endl
			<< _T("<td>") << OnNeedText(0) << _T("</td>") << std::endl
			<< _T("<td><a href=\"")
			<< ARB_PROTOCOL << nCurLine << _T("\">")
			<< OnNeedText(1) << _T("</a></td>") << std::endl
			<< _T("</tr>") << std::endl;
	}
	return data.str();
}

void CPointsDataVenue::Details() const
{
	if (m_pDog && m_pVenue)
	{
		ARBDogRegNumPtr pRegNum;
		if (m_pDog->GetRegNums().FindRegNum(m_pVenue->GetName(), &pRegNum))
		{
			// CDlgDog will cause an update msg to occur which will delete us.
			// So we need to cache the document in a stack variable.
			CDlgDog dlg(m_pDoc, m_pDog, m_pParent, 2);
			if (IDOK == dlg.DoModal())
			{
				m_pDoc->SetModifiedFlag();
				m_pDoc->UpdateAllViews(NULL, UPDATE_POINTS_VIEW|UPDATE_RUNS_VIEW);
			}
		}
		else
			MessageBeep(0);
	}
	else
		MessageBeep(0);
}

bool CPointsDataVenue::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataVenuePtr inVenue = boost::shared_dynamic_cast<CPointsDataVenue, CPointsDataBase>(inData);
	if (inVenue)
		return m_pVenue->GetName() == inVenue->m_pVenue->GetName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataTitle::CPointsDataTitle(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		ARBDogPtr pDog,
		ARBDogTitlePtr pTitle)
	: CPointsDataBase(pParent, pDoc)
	, m_pDog(pDog)
	, m_pTitle(pTitle)
{
}

ARBString CPointsDataTitle::OnNeedText(size_t inCol) const
{
	ARBString str;
	if (m_pTitle)
	{
		switch (inCol)
		{
		case 1:
			str = m_pTitle->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints));
			break;
		case 2:
			str = m_pDoc->GetConfig().GetTitleCompleteName(m_pTitle, false);
			if (m_pTitle->GetReceived())
				str += _T("*");
			break;
		}
	}
	return str;
}

ARBString CPointsDataTitle::GetHtml(size_t nCurLine) const
{
	ARBostringstream data;
	if (m_pTitle)
	{
		data << _T("<tr>") << std::endl
			<< _T("<td/>") << std::endl
			<< _T("<td>") << OnNeedText(1) << _T("</td>") << std::endl
			<< _T("<td><a href=\"")
			<< ARB_PROTOCOL << nCurLine << _T("\">")
			<< OnNeedText(2) << _T("</a></td>") << std::endl
			<< _T("</tr>") << std::endl;
	}
	return data.str();
}

void CPointsDataTitle::Details() const
{
	CDlgTitle dlg(m_pDoc->GetConfig(), m_pDog->GetTitles(), m_pTitle, m_pParent);
	if (IDOK == dlg.DoModal())
	{
		m_pDoc->SetModifiedFlag();
		std::vector<CVenueFilter> venues;
		CFilterOptions::Options().GetFilterVenue(venues);
		m_pDoc->ResetVisibility(venues, m_pTitle);
		m_pDoc->UpdateAllViews(NULL, UPDATE_POINTS_VIEW);
	}
}

bool CPointsDataTitle::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataTitlePtr inTitle = boost::shared_dynamic_cast<CPointsDataTitle, CPointsDataBase>(inData);
	if (inTitle)
		return m_pTitle->GetVenue() == inTitle->m_pTitle->GetVenue()
			&& m_pTitle->GetRawName() == inTitle->m_pTitle->GetRawName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataEvent::CPointsDataEvent(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		ARBDogPtr inDog,
		std::list<RunInfo>& inMatching,
		ARBConfigVenuePtr inVenue,
		ARBConfigDivisionPtr inDiv,
		int inDivIdx,
		ARBConfigLevelPtr inLevel,
		int inLevelIdx,
		ARBConfigEventPtr inEvent,
		int inEventIdx,
		ARBString const& inRunCount,
		ARBString const& inQcount,
		ARBString const& inPts,
		ARBString const& inSuperQ,
		ARBString const& inSpeed)
	: CPointsDataBase(pParent, pDoc)
	, m_Dog(inDog)
	, m_Matching(inMatching)
	, m_Venue(inVenue)
	, m_Div(inDiv)
	, m_DivIdx(inDivIdx)
	, m_Level(inLevel)
	, m_LevelIdx(inLevelIdx)
	, m_Event(inEvent)
	, m_EventIdx(inEventIdx)
	, m_RunCount(inRunCount)
	, m_Qcount(inQcount)
	, m_Pts(inPts)
	, m_SuperQ(inSuperQ)
	, m_Speed(inSpeed)
{
}

ARBString CPointsDataEvent::OnNeedText(size_t inCol) const
{
	ARBString str;
	switch (inCol)
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

ARBString CPointsDataEvent::GetHtml(size_t nCurLine) const
{
	ARBostringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td/>") << std::endl;
	data << _T("<td>") << OnNeedText(1) << _T("</td>") << std::endl;
	data << _T("<td>") << OnNeedText(2) << _T("</td>") << std::endl;
	data << _T("<td>") << OnNeedText(3) << _T("</td>") << std::endl;
	data << _T("<td><a href=\"")
		<< ARB_PROTOCOL << nCurLine << _T("\">")
		<< OnNeedText(4) << _T("</a></td>") << std::endl;
	data << _T("<td>") << OnNeedText(5) << _T("</td>") << std::endl;
	data << _T("<td>") << OnNeedText(6) << _T("</td>") << std::endl;
	data << _T("<td>") << OnNeedText(7) << _T("</td>") << std::endl;
	data << _T("<td>") << OnNeedText(8) << _T("</td>") << std::endl;
	data << _T("</tr>") << std::endl;
	return data.str();
}

void CPointsDataEvent::Details() const
{
	ARBString str(_T("Runs: "));
	str += m_Div->GetName();
	str += _T("/");
	str += m_Level->GetName();
	str += _T("/");
	str += m_Event->GetName();
	RunInfoData data(m_Dog, m_Venue, m_Div, m_Level, m_Event);
	CDlgListViewer dlg(m_pDoc, str.c_str(), m_Dog ? &data : NULL, m_Matching, m_pParent);
	dlg.DoModal();
}

bool CPointsDataEvent::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataEventPtr inEvent = boost::shared_dynamic_cast<CPointsDataEvent, CPointsDataBase>(inData);
	if (inEvent)
		return m_Venue->GetName() == inEvent->m_Venue->GetName()
			&& m_Div->GetName() == inEvent->m_Div->GetName()
			&& m_Level->GetName() == inEvent->m_Level->GetName()
			&& m_Event->GetName() == inEvent->m_Event->GetName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataLifetime::CPointsDataLifetime(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		ARBString const& inVenue)
	: CPointsDataBase(pParent, pDoc)
	, m_Venue(inVenue.c_str())
	, m_Lifetime(0.0)
	, m_Filtered(0.0)
{
}

void CPointsDataLifetime::AddLifetimeInfo(
		ARBString const& inDiv,
		ARBString const& inLevel,
		double inLifetime,
		double inFiltered)
{
	m_Data.push_back(LifeTimePointInfo(inDiv, inLevel, inLifetime, inFiltered));
	m_Lifetime += inLifetime;
	m_Filtered += inFiltered;
}

ARBString CPointsDataLifetime::OnNeedText(size_t inCol) const
{
	ARBString str;
	switch (inCol)
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
			ARBostringstream str2;
			str2 << _T("Total: ");
			if (0 < m_Filtered)
				str2 << m_Lifetime - m_Filtered << _T(" (") << m_Lifetime << ')';
			else
				str2 << m_Lifetime;
			str = str2.str();
		}
		break;
	}
	return str;
}

ARBString CPointsDataLifetime::GetHtml(size_t nCurLine) const
{
	ARBostringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td/>") << std::endl
		<< _T("<td>") << OnNeedText(1) << _T("</td>") << std::endl
		<< _T("<td><a href=\"")
		<< ARB_PROTOCOL << nCurLine << _T("\">")
		<< OnNeedText(2) << _T("</a></td>") << std::endl
		<< _T("</tr>") << std::endl;
	return data.str();
}

void CPointsDataLifetime::Details() const
{
	CString caption(m_Venue);
	caption += _T(" Lifetime Points");
	CDlgListViewer dlg(m_pDoc, caption, m_Data, m_pParent);
	dlg.DoModal();
}

bool CPointsDataLifetime::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataLifetimePtr inLife = boost::shared_dynamic_cast<CPointsDataLifetime, CPointsDataBase>(inData);
	if (inLife)
		return m_Venue == inLife->m_Venue;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataLifetimeDiv::CPointsDataLifetimeDiv(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		ARBString const& inVenue,
		ARBString const& inDiv)
	: CPointsDataLifetime(pParent, pDoc, inVenue)
	, m_Div(inDiv)
{
}

void CPointsDataLifetimeDiv::AddLifetimeInfo(
		ARBString const& inDiv,
		ARBString const& inLevel,
		double inLifetime,
		double inFiltered)
{
	if (inDiv == m_Div)
	{
		m_Data.push_back(LifeTimePointInfo(inDiv, inLevel, inLifetime, inFiltered));
		m_Lifetime += inLifetime;
		m_Filtered += inFiltered;
	}
}

ARBString CPointsDataLifetimeDiv::OnNeedText(size_t inCol) const
{
	ARBString str;
	switch (inCol)
	{
	case 2:
		{
			ARBostringstream str2;
			str2 << m_Div << _T(": ");
			if (0 < m_Filtered)
				str2 << m_Lifetime - m_Filtered << _T(" (") << m_Lifetime << ')';
			else
				str2 << m_Lifetime;
			str = str2.str();
		}
		break;
	}
	return str;
}

ARBString CPointsDataLifetimeDiv::GetHtml(size_t nCurLine) const
{
	ARBostringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td colspan=\"2\"/>") << std::endl
		<< _T("<td>") << OnNeedText(2) << _T("</td>") << std::endl
		<< _T("</tr>") << std::endl;
	return data.str();
}

bool CPointsDataLifetimeDiv::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataLifetimeDivPtr inLife = boost::shared_dynamic_cast<CPointsDataLifetimeDiv, CPointsDataBase>(inData);
	if (inLife)
		return CPointsDataLifetimeDiv::IsEqual(inData)
			&& m_Div == inLife->m_Div;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataMultiQs::CPointsDataMultiQs(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		ARBDogPtr inDog,
		ARBConfigVenuePtr inVenue,
		ARBConfigMultiQPtr inMultiQ,
		std::set<MultiQdata> const& inMQs)
	: CPointsDataBase(pParent, pDoc)
	, m_Dog(inDog)
	, m_Venue(inVenue)
	, m_MultiQ(inMultiQ)
	, m_MQs(inMQs)
	, m_ExistingDblQs(0)
{
	ARBDate dateFrom, dateTo;
	if (!CFilterOptions::Options().GetViewAllDates())
	{
		if (CFilterOptions::Options().GetStartFilterDateSet())
			dateFrom = CFilterOptions::Options().GetStartFilterDate();
		if (CFilterOptions::Options().GetEndFilterDateSet())
			dateTo = CFilterOptions::Options().GetEndFilterDate();
	}
	m_ExistingDblQs = m_Dog->GetExistingPoints().ExistingPoints(
		ARBDogExistingPoints::eMQ, m_Venue, m_MultiQ,
		ARBConfigDivisionPtr(), ARBConfigLevelPtr(), ARBConfigEventPtr(), dateFrom, dateTo);
}

ARBString CPointsDataMultiQs::OnNeedText(size_t inCol) const
{
	ARBString str;
	if (7 == inCol)
	{
		ARBostringstream str2;
		str2 << m_ExistingDblQs + m_MQs.size() << ' ' << m_MultiQ->GetShortName();
		str = str2.str();
	}
	return str;
}

ARBString CPointsDataMultiQs::GetHtml(size_t nCurLine) const
{
	ARBostringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td colspan=\"7\"/>") << std::endl
		<< _T("<td><a href=\"")
		<< ARB_PROTOCOL << nCurLine << _T("\">")
		<< OnNeedText(7) << _T("</a></td>") << std::endl
		<< _T("</tr>") << std::endl;
	return data.str();
}

void CPointsDataMultiQs::Details() const
{
	MultiQInfoData data(m_Dog, m_Venue, m_MultiQ);
	CDlgListViewer dlg(m_pDoc, m_MultiQ->GetName().c_str(), &data, m_MQs, m_pParent);
	dlg.DoModal();
}

bool CPointsDataMultiQs::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataMultiQsPtr inMulti = boost::shared_dynamic_cast<CPointsDataMultiQs, CPointsDataBase>(inData);
	if (inMulti)
		return m_Venue->GetName() == inMulti->m_Venue->GetName()
			&& m_MultiQ->GetName() == inMulti->m_MultiQ->GetName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataSpeedPts::CPointsDataSpeedPts(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		ARBConfigVenuePtr inVenue,
		int inPts)
	: CPointsDataBase(pParent, pDoc)
	, m_Venue(inVenue)
	, m_Pts(inPts)
{
}

ARBString CPointsDataSpeedPts::OnNeedText(size_t inCol) const
{
	ARBString str;
	if (7 == inCol)
	{
		CString str2;
		str2.FormatMessage(IDS_POINTS_SPEED, m_Pts);
		str = (LPCTSTR)str2;
	}
	return str;
}

ARBString CPointsDataSpeedPts::GetHtml(size_t nCurLine) const
{
	ARBostringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td colspan=\"7\"/>") << std::endl
		<< _T("<td>") << OnNeedText(7) << _T("</td>") << std::endl
		<< _T("</tr>") << std::endl;
	return data.str();
}

bool CPointsDataSpeedPts::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataSpeedPtsPtr inPts = boost::shared_dynamic_cast<CPointsDataSpeedPts, CPointsDataBase>(inData);
	if (inPts)
		return m_Venue->GetName() == inPts->m_Venue->GetName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPoints::CPointsDataOtherPoints(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataBase(pParent, pDoc)
	, m_Score(0.0)
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

CPointsDataOtherPointsTallyAll::CPointsDataOtherPointsTallyAll(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		ARBString const& inName,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataOtherPoints(pParent, pDoc, inRunList)
	, m_Name(inName)
{
}

ARBString CPointsDataOtherPointsTallyAll::OnNeedText(size_t inCol) const
{
	ARBString str;
	switch (inCol)
	{
	case 1:
		str = m_Name;
		break;
	case 2:
		{
			ARBostringstream str2;
			str2<< m_Score;
			str = str2.str();
		}
		break;
	}
	return str;
}

ARBString CPointsDataOtherPointsTallyAll::GetHtml(size_t nCurLine) const
{
	ARBostringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td/>") << std::endl
		<< _T("<td>") << OnNeedText(1) << _T("</td>") << std::endl
		<< _T("<td><a href=\"")
		<< ARB_PROTOCOL << nCurLine << _T("\">")
		<< OnNeedText(2) << _T("</a></td>") << std::endl
		<< _T("</tr>") << std::endl;
	return data.str();
}

void CPointsDataOtherPointsTallyAll::Details() const
{
	CDlgListViewer dlg(m_pDoc, _T("Other Points"), m_RunList, m_pParent);
	dlg.DoModal();
}

bool CPointsDataOtherPointsTallyAll::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataOtherPointsTallyAllPtr inPts = boost::shared_dynamic_cast<CPointsDataOtherPointsTallyAll, CPointsDataBase>(inData);
	if (inPts)
		return m_Name == inPts->m_Name;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPointsTallyAllByEvent::CPointsDataOtherPointsTallyAllByEvent(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		ARBString const& inEvent,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataOtherPoints(pParent, pDoc, inRunList)
	, m_Event(inEvent)
{
}

ARBString CPointsDataOtherPointsTallyAllByEvent::OnNeedText(size_t inCol) const
{
	ARBString str;
	switch (inCol)
	{
	case 2: // Event
		str = m_RunList.begin()->m_Event;
		break;
	case 3:
		{
			ARBostringstream str2;
			str2 << m_Score;
			str = str2.str();
		}
		break;
	}
	return str;
}

ARBString CPointsDataOtherPointsTallyAllByEvent::GetHtml(size_t nCurLine) const
{
	ARBostringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td colspan=\"2\"/>") << std::endl
		<< _T("<td>") << OnNeedText(2) << _T("</td>") << std::endl
		<< _T("<td><a href=\"")
		<< ARB_PROTOCOL << nCurLine << _T("\">")
		<< OnNeedText(3) << _T("</a></td>") << std::endl
		<< _T("</tr>") << std::endl;
	return data.str();
}

void CPointsDataOtherPointsTallyAllByEvent::Details() const
{
	CDlgListViewer dlg(m_pDoc, _T("Other Points"), m_RunList, m_pParent);
	dlg.DoModal();
}

bool CPointsDataOtherPointsTallyAllByEvent::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataOtherPointsTallyAllByEventPtr inPts = boost::shared_dynamic_cast<CPointsDataOtherPointsTallyAllByEvent, CPointsDataBase>(inData);
	if (inPts)
		return m_Event == inPts->m_Event;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPointsTallyLevel::CPointsDataOtherPointsTallyLevel(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		ARBString const& inLevel,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataOtherPoints(pParent, pDoc, inRunList)
	, m_Level(inLevel)
{
}

ARBString CPointsDataOtherPointsTallyLevel::OnNeedText(size_t inCol) const
{
	ARBString str;
	switch (inCol)
	{
	case 2: // Level
		str = m_RunList.begin()->m_Level;
		break;
	case 3:
		{
			ARBostringstream str2;
			str2 << m_Score;
			str = str2.str();
		}
		break;
	}
	return str;
}

ARBString CPointsDataOtherPointsTallyLevel::GetHtml(size_t nCurLine) const
{
	ARBostringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td colspan=\"2\"/>") << std::endl
		<< _T("<td>") << OnNeedText(2) << _T("</td>") << std::endl
		<< _T("<td><a href=\"")
		<< ARB_PROTOCOL << nCurLine << _T("\">")
		<< OnNeedText(3) << _T("</a></td>") << std::endl
		<< _T("</tr>") << std::endl;
	return data.str();
}

void CPointsDataOtherPointsTallyLevel::Details() const
{
	CDlgListViewer dlg(m_pDoc, _T("Other Points"), m_RunList, m_pParent);
	dlg.DoModal();
}

bool CPointsDataOtherPointsTallyLevel::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataOtherPointsTallyLevelPtr inPts = boost::shared_dynamic_cast<CPointsDataOtherPointsTallyLevel, CPointsDataBase>(inData);
	if (inPts)
		return m_Level == inPts->m_Level;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPointsTallyLevelByEvent::CPointsDataOtherPointsTallyLevelByEvent(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		ARBString const& inLevel,
		ARBString const& inEvent,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataOtherPoints(pParent, pDoc, inRunList)
	, m_Level(inLevel)
	, m_Event(inEvent)
{
}

ARBString CPointsDataOtherPointsTallyLevelByEvent::OnNeedText(size_t inCol) const
{
	ARBString str;
	switch (inCol)
	{
	case 2: // Level
		str = m_RunList.begin()->m_Level;
		break;
	case 3: // Event
		str = m_RunList.begin()->m_Event;
		break;
	case 4:
		{
			ARBostringstream str2;
			str2 << m_Score;
			str = str2.str();
		}
		break;
	}
	return str;
}

ARBString CPointsDataOtherPointsTallyLevelByEvent::GetHtml(size_t nCurLine) const
{
	ARBostringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td colspan=\"2\"/>") << std::endl
		<< _T("<td>") << OnNeedText(2) << _T("</td>") << std::endl
		<< _T("<td>") << OnNeedText(3) << _T("</td>") << std::endl
		<< _T("<td><a href=\"")
		<< ARB_PROTOCOL << nCurLine << _T("\">")
		<< OnNeedText(4) << _T("</a></td>") << std::endl
		<< _T("</tr>") << std::endl;
	return data.str();
}

void CPointsDataOtherPointsTallyLevelByEvent::Details() const
{
	CDlgListViewer dlg(m_pDoc, _T("Other Points"), m_RunList, m_pParent);
	dlg.DoModal();
}

bool CPointsDataOtherPointsTallyLevelByEvent::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataOtherPointsTallyLevelByEventPtr inPts = boost::shared_dynamic_cast<CPointsDataOtherPointsTallyLevelByEvent, CPointsDataBase>(inData);
	if (inPts)
		return m_Level == inPts->m_Level
			&& m_Event == inPts->m_Event;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataItems::CPointsDataItems()
{
}

CPointsDataItems::~CPointsDataItems()
{
}

class SortPointItems
{
public:
	SortPointItems()
	{
		CAgilityBookOptions::GetPointsViewSort(m_Order[0], m_Order[1], m_Order[2]);
	}
	bool operator()(CPointsDataEvent* one, CPointsDataEvent* two) const
	{
		for (int i = 0; i < 3; ++i)
		{
			switch (m_Order[i])
			{
			default:
				ASSERT(0);
				return false;

 			case CAgilityBookOptions::ePointsViewSortDivision:
				if (one->m_DivIdx != two->m_DivIdx)
					return one->m_DivIdx < two->m_DivIdx;
				break;

			case CAgilityBookOptions::ePointsViewSortLevel:
				if (one->m_LevelIdx != two->m_LevelIdx)
					return one->m_LevelIdx < two->m_LevelIdx;
				break;

			case CAgilityBookOptions::ePointsViewSortEvent:
				if (one->m_EventIdx != two->m_EventIdx)
					return one->m_EventIdx < two->m_EventIdx;
				break;
			}
		}
		return false;
	}
private:
	CAgilityBookOptions::PointsViewSort m_Order[3];
};

void CPointsDataItems::InsertVenueHeader(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		ARBDogPtr inDog,
		ARBConfigVenuePtr pVenue)
{
	m_Lines.push_back(CPointsDataBasePtr(new CPointsDataSeparator(pParent, pDoc, _T("<p><table border=\"2\" cellspacing=\"1\" cellpadding=\"1\">"))));
	m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pParent, pDoc)));
	m_Lines.push_back(CPointsDataBasePtr(new CPointsDataVenue(pParent, pDoc, inDog, pVenue)));
}

void CPointsDataItems::InsertVenueFooter(
		CWnd* pParent,
		CAgilityBookDoc* pDoc)
{
	m_Lines.push_back(CPointsDataBasePtr(new CPointsDataSeparator(pParent, pDoc, _T("</table></p>"))));
}

void CPointsDataItems::LoadData(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		ARBDogPtr inDog)
{
	clear();

	// Find all visible items and sort them out by venue.
	std::vector<CVenueFilter> venues;
	CFilterOptions::Options().GetFilterVenue(venues);

	// Put general info about the dog in...
	m_Lines.push_back(CPointsDataBasePtr(new CPointsDataDog(pParent, pDoc, inDog)));

	// For each venue...
	for (ARBConfigVenueList::const_iterator iterVenue = pDoc->GetConfig().GetVenues().begin();
		iterVenue != pDoc->GetConfig().GetVenues().end();
		++iterVenue)
	{
		ARBConfigVenuePtr pVenue = (*iterVenue);
		if (!CFilterOptions::Options().IsVenueVisible(venues, pVenue->GetName()))
			continue;

		// First, titles.
		bool bHeaderInserted = false;
		for (ARBDogTitleList::const_iterator iterTitle = inDog->GetTitles().begin();
			iterTitle != inDog->GetTitles().end();
			++iterTitle)
		{
			ARBDogTitlePtr pTitle = (*iterTitle);
			if (pTitle->GetVenue() == pVenue->GetName()
			&& !pTitle->IsFiltered())
			{
				if (!bHeaderInserted)
				{
					bHeaderInserted = true;
					InsertVenueHeader(pParent, pDoc, inDog, pVenue);
				}
				m_Lines.push_back(CPointsDataBasePtr(new CPointsDataTitle(pParent, pDoc, inDog, pTitle)));
			}
		}

		LifeTimePointsList lifetime;

		// Then the runs.
		std::list<ARBDogTrialPtr> trialsInVenue;
		for (ARBDogTrialList::const_iterator iterTrial = inDog->GetTrials().begin();
			iterTrial != inDog->GetTrials().end();
			++iterTrial)
		{
			ARBDogTrialPtr pTrial = (*iterTrial);
			// Don't bother subtracting "hidden" trials. Doing so
			// will skew the qualifying percentage.
			if (pTrial->HasVenue(pVenue->GetName()))
				trialsInVenue.push_back(pTrial);
		}
		if (inDog->GetExistingPoints().HasPoints(pVenue->GetName())
		|| 0 < trialsInVenue.size())
		{
			// Ok, we have some trials in the venue to process...
			if (!bHeaderInserted)
			{
				bHeaderInserted = true;
				InsertVenueHeader(pParent, pDoc, inDog, pVenue);
			}
			int speedPts = 0;
			bool bHasSpeedPts = false;
			// Show events sorted out by division/level.
			std::vector<CPointsDataEvent*> items;
			int idxDiv = 0;
			for (ARBConfigDivisionList::const_iterator iterDiv = pVenue->GetDivisions().begin();
				iterDiv != pVenue->GetDivisions().end();
				++idxDiv, ++iterDiv)
			{
				ARBConfigDivisionPtr pDiv = (*iterDiv);
				int idxLevel = 0;
				for (ARBConfigLevelList::const_iterator iterLevel = pDiv->GetLevels().begin();
					iterLevel != pDiv->GetLevels().end();
					++idxLevel, ++iterLevel)
				{
					ARBConfigLevelPtr pLevel = (*iterLevel);
					ARBDate dateFrom, dateTo;
					if (!CFilterOptions::Options().GetViewAllDates())
					{
						if (CFilterOptions::Options().GetStartFilterDateSet())
							dateFrom = CFilterOptions::Options().GetStartFilterDate();
						if (CFilterOptions::Options().GetEndFilterDateSet())
							dateTo = CFilterOptions::Options().GetEndFilterDate();
					}
					LifeTimePoints pts;
					pts.pDiv = pDiv;
					pts.pLevel = pLevel;
					// We know the venue is visible,
					// we don't know if the trial or individual runs are.
					int idxEvent = 0;
					for (ARBConfigEventList::const_iterator iterEvent = pVenue->GetEvents().begin();
						iterEvent != pVenue->GetEvents().end();
						++idxEvent, ++iterEvent)
					{
						ARBConfigEventPtr pEvent = (*iterEvent);

						// Don't tally runs that have no titling points.
						ARBVector<ARBConfigScoringPtr> scoringItems;
						if (0 == pEvent->FindAllEvents(pDiv->GetName(), pLevel->GetName(), ARBDate(), true, scoringItems))
							continue;
						// Iterate across each scoring method separately. This means it is
						// possible to have multiple lines show up for a given event. But if
						// that happens, it means the events were scored differently.
						for (ARBVector<ARBConfigScoringPtr>::iterator iterScoring = scoringItems.begin();
							iterScoring != scoringItems.end();
							++iterScoring)
						{
							ARBConfigScoringPtr pScoringMethod = *iterScoring;
							ARBDate dateFrom2 = pScoringMethod->GetValidFrom();
							ARBDate dateTo2 = pScoringMethod->GetValidTo();
							if (!dateFrom2.IsValid() || dateFrom > dateFrom2)
								dateFrom2 = dateFrom;
							if (!dateTo2.IsValid() || dateTo > dateTo2)
								dateTo2 = dateTo;
							bool bHasExistingPoints = inDog->GetExistingPoints().HasPoints(pVenue, pDiv, pLevel, pEvent, dateFrom2, dateTo2, false);
							bool bHasExistingLifetimePoints = inDog->GetExistingPoints().HasPoints(pVenue, pDiv, pLevel, pEvent, dateFrom2, dateTo2, true);
							if (!CFilterOptions::Options().IsVenueLevelVisible(venues, pVenue->GetName(), pDiv->GetName(), pLevel->GetName()))
							{
								bHasExistingPoints = false;
								bHasExistingLifetimePoints = false;
							}
							int SQs = 0;
							int speedPtsEvent = 0;
							std::list<RunInfo> matching;
							std::set<ARBString> judges;
							std::set<ARBString> judgesQ;
							std::set<ARBString> partners;
							std::set<ARBString> partnersQ;
							for (std::list<ARBDogTrialPtr>::const_iterator iterTrial = trialsInVenue.begin();
								iterTrial != trialsInVenue.end();
								++iterTrial)
							{
								ARBDogTrialPtr pTrial = (*iterTrial);
								for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
									iterRun != pTrial->GetRuns().end();
									++iterRun)
								{
									ARBDogRunPtr pRun = (*iterRun);
									if (pRun->GetDivision() != pDiv->GetName()
									|| (pRun->GetLevel() != pLevel->GetName() && !pLevel->GetSubLevels().FindSubLevel(pRun->GetLevel()))
									|| pRun->GetEvent() != pEvent->GetName())
										continue;
									ARBConfigScoringPtr pScoring;
									pEvent->FindEvent(pDiv->GetName(), pLevel->GetName(), pRun->GetDate(), &pScoring);
									ASSERT(pScoring);
									if (!pScoring) continue; // Shouldn't need it...
									if (*pScoring != *pScoringMethod)
										continue;
									bool bRunVisible = (!pRun->IsFiltered(ARBBase::eIgnoreQ)
									&& CFilterOptions::Options().IsRunVisible(venues, pVenue, pTrial, pRun));
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
								nExistingPts = inDog->GetExistingPoints().ExistingPoints(
									ARBDogExistingPoints::eRuns,
									pVenue, ARBConfigMultiQPtr(), pDiv, pLevel, pEvent, dateFrom2, dateTo2);
								if (pScoringMethod->HasSuperQ())
									nExistingSQ += static_cast<int>(inDog->GetExistingPoints().ExistingPoints(
										ARBDogExistingPoints::eSQ,
										pVenue, ARBConfigMultiQPtr(), pDiv, pLevel, pEvent, dateFrom2, dateTo2));
							}
							// Now add the existing lifetime points
							if (bHasExistingLifetimePoints && !ARBDouble::equal(0.0, nExistingPts + nExistingSQ))
							{
								pts.ptList.push_back(LifeTimePoint(pEvent->GetName(), nExistingPts + nExistingSQ, false));
							}
							// Now we deal with the visible runs.
							if (bHasExistingPoints || 0 < matching.size())
							{
								int nCleanQ = 0;
								int nNotCleanQ = 0;
								double pts = 0.0;
								for (std::list<RunInfo>::const_iterator iterRun = matching.begin();
									iterRun != matching.end();
									++iterRun)
								{
									ARBDogRunPtr pRun = iterRun->second;
									if (pRun->GetQ().Qualified())
									{
										bool bClean = false;
										pts += pRun->GetTitlePoints(pScoringMethod, &bClean);
										if (bClean)
											++nCleanQ;
										else
											++nNotCleanQ;
									}
								}
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
								double percentQs = 0.0;
								if (0 < matching.size())
									percentQs = (static_cast<double>(nCleanQ + nNotCleanQ) / static_cast<double>(matching.size())) * 100;
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
								items.push_back(new CPointsDataEvent(pParent, pDoc,
									!ARBDouble::equal(0.0, nExistingPts + nExistingSQ) ? inDog : ARBDogPtr(),
									matching,
									pVenue,
									pDiv, idxDiv,
									pLevel, idxLevel,
									pEvent, idxEvent,
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
						speedPts += static_cast<int>(inDog->GetExistingPoints().ExistingPoints(
							ARBDogExistingPoints::eSpeed,
							pVenue, ARBConfigMultiQPtr(), pDiv, pLevel, ARBConfigEventPtr(), dateFrom, dateTo));
					}
					if (0 < pts.ptList.size())
						lifetime.push_back(pts);
				}
			}
			if (1 < items.size())
				std::stable_sort(items.begin(), items.end(), SortPointItems());
			for (std::vector<CPointsDataEvent*>::iterator i = items.begin();
				i != items.end();
				++i)
			{
				m_Lines.push_back(CPointsDataBasePtr(*i));
			}
			items.clear();

			// Information that is tallied after all a venue's events.
			if (bHasSpeedPts)
			{
				m_Lines.push_back(CPointsDataBasePtr(new CPointsDataSpeedPts(pParent, pDoc, pVenue, speedPts)));
			}

			// If the venue has multiQs, tally them now.
			if (0 < pVenue->GetMultiQs().size())
			{
				std::map<ARBConfigMultiQPtr, std::set<MultiQdata> > MQs;
				for (std::list<ARBDogTrialPtr>::const_iterator iterTrial = trialsInVenue.begin();
					iterTrial != trialsInVenue.end();
					++iterTrial)
				{
					ARBDogTrialPtr pTrial = (*iterTrial);
					for (ARBDogRunList::const_iterator iterR = pTrial->GetRuns().begin();
						iterR != pTrial->GetRuns().end();
						++iterR)
					{
						ARBDogRunPtr pRun = *iterR;
						if (pRun->GetMultiQ()
						&& !pRun->IsFiltered(ARBBase::eIgnoreQ)
						&& CFilterOptions::Options().IsRunVisible(venues, pVenue, pTrial, pRun))
						{
							MQs[pRun->GetMultiQ()].insert(MultiQdata(pRun->GetDate(), pTrial));
						}
					}
				}
				for (std::map<ARBConfigMultiQPtr, std::set<MultiQdata> >::iterator iter = MQs.begin();
					iter != MQs.end();
					++iter)
				{
					m_Lines.push_back(CPointsDataBasePtr(new CPointsDataMultiQs(pParent, pDoc, inDog, pVenue, (*iter).first, (*iter).second)));
				}
			}
		}

		// Next comes lifetime points.
		if (0 < lifetime.size())
		{
			CPointsDataLifetime* pData = new CPointsDataLifetime(pParent, pDoc, pVenue->GetName());
			double pts = 0;
			double ptFiltered = 0;
			typedef std::map<ARBString, CPointsDataLifetimeDiv*> DivLifetime;
			DivLifetime divs;
			for (LifeTimePointsList::iterator iter = lifetime.begin();
				iter != lifetime.end();
				++iter)
			{
				CPointsDataLifetimeDiv* pDivData = NULL;
				DivLifetime::iterator it = divs.find(iter->pDiv->GetName());
				if (divs.end() != it)
				{
					pDivData = it->second;
				}
				else
				{
					pDivData = new CPointsDataLifetimeDiv(pParent, pDoc, pVenue->GetName(), iter->pDiv->GetName());
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
			m_Lines.push_back(CPointsDataBasePtr(pData));
			if (1 < divs.size())
			{
				for (DivLifetime::iterator it = divs.begin();
					it != divs.end();
					++it)
				{
					m_Lines.push_back(CPointsDataBasePtr(it->second));
				}
			}
			else if (1 == divs.size())
			{
				delete divs.begin()->second;
			}
		}
		if (bHeaderInserted)
			InsertVenueFooter(pParent, pDoc);
	}

	// After all the venues, we do 'other points'.
	ARBConfigOtherPointsList const& other = pDoc->GetConfig().GetOtherPoints();
	if (0 < other.size())
	{
		m_Lines.push_back(CPointsDataBasePtr(new CPointsDataSeparator(pParent, pDoc, _T("<p><table border=\"2\" cellspacing=\"1\" cellpadding=\"1\">"))));
		CString str;
		m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pParent, pDoc)));
		str.LoadString(IDS_OTHERPOINTS);
		m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pParent, pDoc, str)));
		for (ARBConfigOtherPointsList::const_iterator iterOther = other.begin();
			iterOther != other.end();
			++iterOther)
		{
			// First, just generate a list of runs with the needed info.
			std::list<OtherPtInfo> runs;

			ARBConfigOtherPointsPtr pOther = (*iterOther);
			for (ARBDogTrialList::const_iterator iterTrial = inDog->GetTrials().begin();
				iterTrial != inDog->GetTrials().end();
				++iterTrial)
			{
				ARBDogTrialPtr pTrial = (*iterTrial);
				if (!pTrial->IsFiltered())
				{
					for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
						iterRun != pTrial->GetRuns().end();
						++iterRun)
					{
						ARBDogRunPtr pRun = (*iterRun);
						if (!pRun->IsFiltered(ARBBase::eIgnoreQ))
						{
							for (ARBDogRunOtherPointsList::const_iterator iterOtherPts = pRun->GetOtherPoints().begin();
								iterOtherPts != pRun->GetOtherPoints().end();
								++iterOtherPts)
							{
								ARBDogRunOtherPointsPtr pOtherPts = (*iterOtherPts);
								if (pOtherPts->GetName() == pOther->GetName())
								{
									runs.push_back(OtherPtInfo(pTrial, pRun, pOtherPts->GetPoints()));
								}
							}
						}
					}
				}
			}

			for (ARBDogExistingPointsList::const_iterator iterExisting = inDog->GetExistingPoints().begin();
				iterExisting != inDog->GetExistingPoints().end();
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
				m_Lines.push_back(CPointsDataBasePtr(new CPointsDataOtherPointsTallyAll(pParent, pDoc, pOther->GetName(), runs)));
				break;

			case ARBConfigOtherPoints::eTallyAllByEvent:
				m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pParent, pDoc, _T(""), pOther->GetName().c_str())));
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
						m_Lines.push_back(CPointsDataBasePtr(new CPointsDataOtherPointsTallyAllByEvent(pParent, pDoc, (*iterTally), validRuns)));
					}
				}
				break;

			case ARBConfigOtherPoints::eTallyLevel:
				m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pParent, pDoc, _T(""), pOther->GetName().c_str())));
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
						m_Lines.push_back(CPointsDataBasePtr(new CPointsDataOtherPointsTallyLevel(pParent, pDoc, (*iterTally), validRuns)));
					}
				}
				break;

			case ARBConfigOtherPoints::eTallyLevelByEvent:
				m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pParent, pDoc, _T(""), pOther->GetName().c_str())));
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
						m_Lines.push_back(CPointsDataBasePtr(new CPointsDataOtherPointsTallyLevelByEvent(pParent, pDoc, (*iterTally).first, (*iterTally).second, validRuns)));
					}
				}
				break;
			}
		}
		m_Lines.push_back(CPointsDataBasePtr(new CPointsDataSeparator(pParent, pDoc, _T("</table></p>"))));
	}
}

void CPointsDataItems::clear()
{
	m_Lines.clear();
}

size_t CPointsDataItems::NumLines() const
{
	return m_Lines.size();
}

CPointsDataBasePtr CPointsDataItems::GetLine(size_t nLine) const
{
	if (nLine < m_Lines.size())
		return m_Lines[nLine];
	else
		return CPointsDataBasePtr();
}
