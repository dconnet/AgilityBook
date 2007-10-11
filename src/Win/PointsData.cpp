/*
 * Copyright © 2004-2007 David Connet. All Rights Reserved.
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
 * @li 2007-03-14 DRC Change accumulation to work by event, not method.
 *                    This removes the multiple listings in the view.
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
		tstring const& inDiv,
		tstring const& inLevel,
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
		tstring const& inHtml)
	: CPointsDataBase(pParent, pDoc)
	, m_Html(inHtml)
{
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataText::CPointsDataText(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		bool bUseInHtml,
		LPCTSTR inCol1,
		LPCTSTR inCol2)
	: CPointsDataBase(pParent, pDoc)
	, m_UseInHtml(bUseInHtml)
	, m_Col1(inCol1)
	, m_Col2(inCol2)
{
	ASSERT(NULL != inCol1);
	ASSERT(NULL != inCol2);
}


tstring CPointsDataText::OnNeedText(size_t inCol) const
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


tstring CPointsDataText::GetHtml(size_t /*nCurLine*/) const
{
	otstringstream data;
	if (m_UseInHtml)
	{
		data << _T("<tr>") << std::endl
			<< _T("<td>") << SanitizeStringForHTML(m_Col2) << _T("</td>") << std::endl
			<< _T("</tr>") << std::endl;
	}
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


tstring CPointsDataDog::OnNeedText(size_t inCol) const
{
	tstring str;
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
			str = ARBDate::Today().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints));
			break;
		}
	}
	return str;
}


tstring CPointsDataDog::GetHtml(size_t nCurLine) const
{
	otstringstream data;
	if (m_pDog)
	{
		CString title;
		title.LoadString(IDS_TITLING_POINTS);
		data << _T("<h1 align=\"center\">") << (LPCTSTR)title <<_T(" ")
			<< SanitizeStringForHTML(ARBDate::Today().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints)))
			<< _T("</h1>")
			<< _T("<h1><a href=\"") << ARB_PROTOCOL
#if _MSC_VER < 1400 // VC7 casting warning
			<< static_cast<UINT>(nCurLine)
#else
			<< nCurLine
#endif
			<< _T("\">") << SanitizeStringForHTML(m_pDog->GetCallName());
		if (!m_pDog->GetRegisteredName().empty())
		{
			data << _T("</a> [")
				<< SanitizeStringForHTML(m_pDog->GetRegisteredName())
				<< _T("]");
		}
		data << _T("</h1>") << std::endl;

	}
	return data.str();
}


void CPointsDataDog::Details() const
{
	// CDlgDog will cause an update msg to occur which will delete us.
	// So we need to cache the document in a stack variable.
	CAgilityBookDoc* pDoc = m_pDoc;
	CDlgDog dlg(pDoc, m_pDog, m_pParent, 0);
	if (IDOK == dlg.DoModal())
	{
		pDoc->SetModifiedFlag();
		pDoc->UpdateAllViews(NULL, UPDATE_POINTS_VIEW|UPDATE_RUNS_VIEW);
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


tstring CPointsDataVenue::OnNeedText(size_t inCol) const
{
	tstring str;
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


tstring CPointsDataVenue::GetHtml(size_t nCurLine) const
{
	otstringstream data;
	if (m_pVenue)
	{
		data << _T("<h2>");
		if (m_pVenue->GetURL().empty())
		{
			data << SanitizeStringForHTML(m_pVenue->GetName());
		}
		else
		{
			data << _T("<a href=\"")
				<< m_pVenue->GetURL()
				<< _T("\">")
				<< SanitizeStringForHTML(m_pVenue->GetName())
				<< _T("</a>");
		}
		if (m_pDog)
		{
			ARBDogRegNumPtr pRegNum;
			if (m_pDog->GetRegNums().FindRegNum(m_pVenue->GetName(), &pRegNum))
			{
				data << _T(" [<a href=\"") << ARB_PROTOCOL
#if _MSC_VER < 1400 // VC7 casting warning
					<< static_cast<UINT>(nCurLine)
#else
					<< nCurLine
#endif
					<< _T("\">") << SanitizeStringForHTML(pRegNum->GetNumber())
					<< _T("</a>]") << std::endl;
			}
		}
		data << _T("</h2>") << std::endl;
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
			CAgilityBookDoc* pDoc = m_pDoc;
			CDlgDog dlg(pDoc, m_pDog, m_pParent, 2);
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


tstring CPointsDataTitle::OnNeedText(size_t inCol) const
{
	tstring str;
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


tstring CPointsDataTitle::GetHtml(size_t nCurLine) const
{
	otstringstream data;
	if (m_pTitle)
	{
		data << _T("<tr>") << std::endl
			<< _T("<td>") << SanitizeStringForHTML(OnNeedText(1)) << _T("</td>") << std::endl
			<< _T("<td><a href=\"") << ARB_PROTOCOL
#if _MSC_VER < 1400 // VC7 casting warning
			<< static_cast<UINT>(nCurLine)
#else
			<< nCurLine
#endif
			<< _T("\">") << SanitizeStringForHTML(OnNeedText(2)) << _T("</a></td>") << std::endl
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
		tstring const& inRunCount,
		tstring const& inQcount,
		tstring const& inPts,
		tstring const& inSuperQ,
		tstring const& inSpeed)
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


tstring CPointsDataEvent::OnNeedText(size_t inCol) const
{
	tstring str;
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


tstring CPointsDataEvent::GetHtml(size_t nCurLine) const
{
	otstringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td>") << SanitizeStringForHTML(OnNeedText(1)) << _T("</td>") << std::endl
		<< _T("<td>") << SanitizeStringForHTML(OnNeedText(2)) << _T("</td>") << std::endl
		<< _T("<td>") << SanitizeStringForHTML(OnNeedText(3)) << _T("</td>") << std::endl
		<< _T("<td><a href=\"") << ARB_PROTOCOL
#if _MSC_VER < 1400 // VC7 casting warning
		<< static_cast<UINT>(nCurLine)
#else
		<< nCurLine
#endif
		<< _T("\">") << SanitizeStringForHTML(OnNeedText(4)) << _T("</a></td>") << std::endl
		<< _T("<td>") << SanitizeStringForHTML(OnNeedText(5)) << _T("</td>") << std::endl
		<< _T("<td align=\"right\">") << SanitizeStringForHTML(OnNeedText(6)) << _T("</td>") << std::endl
		<< _T("<td>") << SanitizeStringForHTML(OnNeedText(7)) << _T("</td>") << std::endl
		<< _T("<td>") << SanitizeStringForHTML(OnNeedText(8)) << _T("</td>") << std::endl
		<< _T("</tr>") << std::endl;
	return data.str();
}


void CPointsDataEvent::Details() const
{
	CString runs;
	runs.LoadString(IDS_RUNS);
	otstringstream str;
	str << (LPCTSTR)runs << _T(": ")
		<< m_Div->GetName()
		<< _T("/")
		<< m_Level->GetName()
		<< _T("/")
		<< m_Event->GetName();
	RunInfoData data(m_Dog, m_Venue, m_Div, m_Level, m_Event);
	CDlgListViewer dlg(m_pDoc, str.str().c_str(), m_Dog ? &data : NULL, m_Matching, m_pParent);
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
		bool bLifetime,
		ARBConfigVenuePtr inVenue)
	: CPointsDataBase(pParent, pDoc)
	, m_bLifetime(bLifetime)
	, m_Venue(inVenue)
	, m_Lifetime(0.0)
	, m_Filtered(0.0)
{
}


void CPointsDataLifetime::AddLifetimeInfo(
		tstring const& inDiv,
		tstring const& inLevel,
		double inLifetime,
		double inFiltered)
{
	m_Data.push_back(LifeTimePointInfo(inDiv, inLevel, inLifetime, inFiltered));
	m_Lifetime += inLifetime;
	m_Filtered += inFiltered;
}


tstring CPointsDataLifetime::OnNeedText(size_t inCol) const
{
	tstring str;
	switch (inCol)
	{
	case 1:
		{
			CString str2;
			if (m_bLifetime)
			{
				CString lifetime;
				if (m_Venue->HasLifetimeName())
					lifetime = m_Venue->GetLifetimeName().c_str();
				else
					lifetime.LoadString(IDS_TITLEPOINT_LIFETIME);
				str2.FormatMessage(IDS_LIFETIME_POINTS, (LPCTSTR)lifetime);
			}
			else
				str2.LoadString(IDS_PLACEMENT_POINTS);
			str = (LPCTSTR)str2;
		}
		break;
	case 2:
		{
			CString total;
			total.LoadString(IDS_TOTAL);
			otstringstream str2;
			str2 << (LPCTSTR)total << _T(": ");
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


tstring CPointsDataLifetime::GetHtml(size_t nCurLine) const
{
	otstringstream data;
	data << _T("<tr><td>&nbsp;</td</tr>") << std::endl
		<< _T("<tr>") << std::endl
		<< _T("<td>") << SanitizeStringForHTML(OnNeedText(1)) << _T("</td>") << std::endl
		<< _T("<td align=\"right\"><a href=\"") << ARB_PROTOCOL
#if _MSC_VER < 1400 // VC7 casting warning
		<< static_cast<UINT>(nCurLine)
#else
		<< nCurLine
#endif
		<< _T("\">") << SanitizeStringForHTML(OnNeedText(2)) << _T("</a></td>") << std::endl
		<< _T("</tr>") << std::endl;
	return data.str();
}


void CPointsDataLifetime::Details() const
{
	CString caption(m_Venue->GetName().c_str());
	CString str;
	if (m_bLifetime)
	{
		CString lifetime;
		if (m_Venue->HasLifetimeName())
			lifetime = m_Venue->GetLifetimeName().c_str();
		else
			lifetime.LoadString(IDS_TITLEPOINT_LIFETIME);
		str.FormatMessage(IDS_LIFETIME_POINTS, (LPCTSTR)lifetime);
	}
	else
		str.LoadString(IDS_PLACEMENT_POINTS);
	caption += _T(" ") + str;
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
		bool bLifetime,
		ARBConfigVenuePtr inVenue,
		tstring const& inDiv)
	: CPointsDataLifetime(pParent, pDoc, bLifetime, inVenue)
	, m_Div(inDiv)
{
}


void CPointsDataLifetimeDiv::AddLifetimeInfo(
		tstring const& inDiv,
		tstring const& inLevel,
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


tstring CPointsDataLifetimeDiv::OnNeedText(size_t inCol) const
{
	tstring str;
	switch (inCol)
	{
	case 2:
		{
			otstringstream str2;
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


tstring CPointsDataLifetimeDiv::GetHtml(size_t nCurLine) const
{
	otstringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td/>") << std::endl
		<< _T("<td align=\"right\">") << SanitizeStringForHTML(OnNeedText(2)) << _T("</td>") << std::endl
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


tstring CPointsDataMultiQs::OnNeedText(size_t inCol) const
{
	tstring str;
	if (7 == inCol)
	{
		otstringstream str2;
		str2 << m_ExistingDblQs + m_MQs.size() << ' ' << m_MultiQ->GetShortName();
		str = str2.str();
	}
	return str;
}


tstring CPointsDataMultiQs::GetHtml(size_t nCurLine) const
{
	otstringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td colspan=\"6\"/>") << std::endl
		<< _T("<td><a href=\"") << ARB_PROTOCOL
#if _MSC_VER < 1400 // VC7 casting warning
		<< static_cast<UINT>(nCurLine)
#else
		<< nCurLine
#endif
		<< _T("\">")
		<< SanitizeStringForHTML(OnNeedText(7)) << _T("</a></td>") << std::endl
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


tstring CPointsDataSpeedPts::OnNeedText(size_t inCol) const
{
	tstring str;
	if (7 == inCol)
	{
		CString str2;
		str2.FormatMessage(IDS_POINTS_SPEED, m_Pts);
		str = (LPCTSTR)str2;
	}
	return str;
}


tstring CPointsDataSpeedPts::GetHtml(size_t nCurLine) const
{
	otstringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td colspan=\"6\"/>") << std::endl
		<< _T("<td>") << SanitizeStringForHTML(OnNeedText(7)) << _T("</td>") << std::endl
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
		tstring const& inName,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataOtherPoints(pParent, pDoc, inRunList)
	, m_Name(inName)
{
}


tstring CPointsDataOtherPointsTallyAll::OnNeedText(size_t inCol) const
{
	tstring str;
	switch (inCol)
	{
	case 1:
		str = m_Name;
		break;
	case 2:
		{
			otstringstream str2;
			str2<< m_Score;
			str = str2.str();
		}
		break;
	}
	return str;
}


tstring CPointsDataOtherPointsTallyAll::GetHtml(size_t nCurLine) const
{
	otstringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td>") << SanitizeStringForHTML(OnNeedText(1)) << _T("</td>") << std::endl
		<< _T("<td align=\"right\"><a href=\"") << ARB_PROTOCOL
#if _MSC_VER < 1400 // VC7 casting warning
		<< static_cast<UINT>(nCurLine)
#else
		<< nCurLine
#endif
		<< _T("\">")
		<< SanitizeStringForHTML(OnNeedText(2)) << _T("</a></td>") << std::endl
		<< _T("</tr>") << std::endl;
	return data.str();
}


void CPointsDataOtherPointsTallyAll::Details() const
{
	CString str;
	str.LoadString(IDS_OTHERPOINTS);
	CDlgListViewer dlg(m_pDoc, str, m_RunList, m_pParent);
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
		tstring const& inEvent,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataOtherPoints(pParent, pDoc, inRunList)
	, m_Event(inEvent)
{
}


tstring CPointsDataOtherPointsTallyAllByEvent::OnNeedText(size_t inCol) const
{
	tstring str;
	switch (inCol)
	{
	case 2: // Event
		str = m_RunList.begin()->m_Event;
		break;
	case 3:
		{
			otstringstream str2;
			str2 << m_Score;
			str = str2.str();
		}
		break;
	}
	return str;
}


tstring CPointsDataOtherPointsTallyAllByEvent::GetHtml(size_t nCurLine) const
{
	otstringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td/>") << std::endl
		<< _T("<td>") << SanitizeStringForHTML(OnNeedText(2)) << _T("</td>") << std::endl
		<< _T("<td align=\"right\"><a href=\"") << ARB_PROTOCOL
#if _MSC_VER < 1400 // VC7 casting warning
		<< static_cast<UINT>(nCurLine)
#else
		<< nCurLine
#endif
		<< _T("\">")
		<< SanitizeStringForHTML(OnNeedText(3)) << _T("</a></td>") << std::endl
		<< _T("</tr>") << std::endl;
	return data.str();
}


void CPointsDataOtherPointsTallyAllByEvent::Details() const
{
	CString str;
	str.LoadString(IDS_OTHERPOINTS);
	CDlgListViewer dlg(m_pDoc, str, m_RunList, m_pParent);
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
		tstring const& inLevel,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataOtherPoints(pParent, pDoc, inRunList)
	, m_Level(inLevel)
{
}


tstring CPointsDataOtherPointsTallyLevel::OnNeedText(size_t inCol) const
{
	tstring str;
	switch (inCol)
	{
	case 2: // Level
		str = m_RunList.begin()->m_Level;
		break;
	case 3:
		{
			otstringstream str2;
			str2 << m_Score;
			str = str2.str();
		}
		break;
	}
	return str;
}


tstring CPointsDataOtherPointsTallyLevel::GetHtml(size_t nCurLine) const
{
	otstringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td/>") << std::endl
		<< _T("<td>") << SanitizeStringForHTML(OnNeedText(2)) << _T("</td>") << std::endl
		<< _T("<td align=\"right\"><a href=\"") << ARB_PROTOCOL
#if _MSC_VER < 1400 // VC7 casting warning
		<< static_cast<UINT>(nCurLine)
#else
		<< nCurLine
#endif
		<< _T("\">")
		<< SanitizeStringForHTML(OnNeedText(3)) << _T("</a></td>") << std::endl
		<< _T("</tr>") << std::endl;
	return data.str();
}


void CPointsDataOtherPointsTallyLevel::Details() const
{
	CString str;
	str.LoadString(IDS_OTHERPOINTS);
	CDlgListViewer dlg(m_pDoc, str, m_RunList, m_pParent);
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
		tstring const& inLevel,
		tstring const& inEvent,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataOtherPoints(pParent, pDoc, inRunList)
	, m_Level(inLevel)
	, m_Event(inEvent)
{
}


tstring CPointsDataOtherPointsTallyLevelByEvent::OnNeedText(size_t inCol) const
{
	tstring str;
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
			otstringstream str2;
			str2 << m_Score;
			str = str2.str();
		}
		break;
	}
	return str;
}


tstring CPointsDataOtherPointsTallyLevelByEvent::GetHtml(size_t nCurLine) const
{
	otstringstream data;
	data << _T("<tr>") << std::endl
		<< _T("<td/>") << std::endl
		<< _T("<td>") << SanitizeStringForHTML(OnNeedText(2)) << _T("</td>") << std::endl
		<< _T("<td>") << SanitizeStringForHTML(OnNeedText(3)) << _T("</td>") << std::endl
		<< _T("<td align=\"right\"><a href=\"") << ARB_PROTOCOL
#if _MSC_VER < 1400 // VC7 casting warning
		<< static_cast<UINT>(nCurLine)
#else
		<< nCurLine
#endif
		<< _T("\">")
		<< SanitizeStringForHTML(OnNeedText(4)) << _T("</a></td>") << std::endl
		<< _T("</tr>") << std::endl;
	return data.str();
}


void CPointsDataOtherPointsTallyLevelByEvent::Details() const
{
	CString str;
	str.LoadString(IDS_OTHERPOINTS);
	CDlgListViewer dlg(m_pDoc, str, m_RunList, m_pParent);
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
	m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pParent, pDoc, false)));
	m_Lines.push_back(CPointsDataBasePtr(new CPointsDataVenue(pParent, pDoc, inDog, pVenue)));
}


void CPointsDataItems::LoadData(
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		ARBDogPtr inDog)
{
	clear();

	if (!inDog)
		return;

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
					CString str;
					str.LoadString(IDS_TITLES);
					tstring data(_T("<h3>"));
					data += (LPCTSTR)str;
					data += _T("</h3><table border=\"2\" cellspacing=\"1\" cellpadding=\"1\">");
					m_Lines.push_back(CPointsDataBasePtr(new CPointsDataSeparator(pParent, pDoc, data)));
				}
				m_Lines.push_back(CPointsDataBasePtr(new CPointsDataTitle(pParent, pDoc, inDog, pTitle)));
			}
		}
		if (bHeaderInserted)
			m_Lines.push_back(CPointsDataBasePtr(new CPointsDataSeparator(pParent, pDoc, _T("</table>"))));

		bool bRunsInserted = false;
		LifeTimePointsList lifetime;
		LifeTimePointsList placement;

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
			bRunsInserted = true;
			CString str;
			str.LoadString(IDS_RUNS);
			tstring data(_T("<h3>"));
			data += (LPCTSTR)str;
			data += _T("</h3><table border=\"2\" cellspacing=\"1\" cellpadding=\"1\">");
			m_Lines.push_back(CPointsDataBasePtr(new CPointsDataSeparator(pParent, pDoc, data)));
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
						bool hasSQs = false;
						int SQs = 0;
						int speedPtsEvent = 0;
						int nCleanQ = 0;
						int nNotCleanQ = 0;
						double points = 0.0;
						double nExistingPts = 0;
						int nExistingSQ = 0;
						std::list<RunInfo> allmatching;
						std::set<tstring> judges;
						std::set<tstring> judgesQ;
						std::set<tstring> partners;
						std::set<tstring> partnersQ;
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
							std::list<RunInfo> matching;
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
											int pts2 = pRun->GetSpeedPoints(pScoringMethod);
											speedPts += pts2;
											speedPtsEvent += pts2;
										}
										// Only tally partners for pairs. In USDAA DAM, pairs is
										// actually a 3-dog relay.
										if (pEvent->HasPartner() && 1 == pRun->GetPartners().size())
										{
											for (ARBDogRunPartnerList::const_iterator iterPartner = pRun->GetPartners().begin();
												iterPartner != pRun->GetPartners().end();
												++iterPartner)
											{
												tstring p = (*iterPartner)->GetDog();
												p += (*iterPartner)->GetRegNum();
												partners.insert(p);
												if (pRun->GetQ().Qualified())
													partnersQ.insert(p);
											}
										}
									}
									// Tally lifetime points, regardless of visibility.
									if ((0 < pScoringMethod->GetLifetimePoints().size()
									|| 0 < pScoringMethod->GetPlacements().size())
									&& pRun->GetQ().Qualified())
									{
										double nLifetime, nPlacement;
										pRun->GetTitlePoints(pScoringMethod, NULL, &nLifetime, &nPlacement);
										if (0 < nLifetime)
										{
											pts.ptLifetime.push_back(LifeTimePoint(pRun->GetEvent(), nLifetime, !bRunVisible));
										}
										if (0 < nPlacement)
										{
											pts.ptPlacement.push_back(LifeTimePoint(pRun->GetEvent(), nPlacement, !bRunVisible));
										}
									}
								}
							}
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
								pts.ptLifetime.push_back(LifeTimePoint(pEvent->GetName(), nExistingPts + nExistingSQ, false));
							}
							if (bHasExistingPoints || 0 < matching.size())
							{
								for (std::list<RunInfo>::const_iterator iterRun = matching.begin();
									iterRun != matching.end();
									++iterRun)
								{
									ARBDogRunPtr pRun = iterRun->second;
									if (pRun->GetQ().Qualified())
									{
										bool bClean = false;
										points += pRun->GetTitlePoints(pScoringMethod, &bClean);
										if (bClean)
											++nCleanQ;
										else
											++nNotCleanQ;
									}
								}
								points += nExistingPts;
								if (pScoringMethod->HasSuperQ())
									hasSQs = true;
								if (pScoringMethod->HasSpeedPts())
									bHasSpeedPts = true;
								allmatching.insert(allmatching.end(), matching.begin(), matching.end());
							}
						}
						//TODO: Add ability to accumulate existing placement points
						// Now we deal with the visible runs.
						if (0 < allmatching.size())
						{
							CString strRunCount;
							strRunCount.FormatMessage(IDS_POINTS_RUNS_JUDGES,
								allmatching.size(),
								judges.size());
							if (pEvent->HasPartner() && 0 < partners.size())
							{
								CString str2;
								str2.FormatMessage(IDS_POINTS_PARTNERS, partners.size());
								strRunCount += str2;
							}
							double percentQs = 0.0;
							if (0 < allmatching.size())
								percentQs = (static_cast<double>(nCleanQ + nNotCleanQ) / static_cast<double>(allmatching.size())) * 100;
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
							otstringstream strPts;
							CString strSuperQ;
							strPts << points + nExistingSQ;
							if (hasSQs)
							{
								SQs += nExistingSQ;
								strSuperQ.FormatMessage(IDS_POINTS_SQS, SQs);
							}
							CString strSpeed;
							if (bHasSpeedPts && 0 < speedPtsEvent)
							{
								strSpeed.FormatMessage(IDS_POINTS_SPEED_SUBTOTAL, speedPtsEvent);
							}
							items.push_back(new CPointsDataEvent(pParent, pDoc,
								!ARBDouble::equal(0.0, nExistingPts + nExistingSQ) ? inDog : ARBDogPtr(),
								allmatching,
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
					if (bHasSpeedPts)
					{
						speedPts += static_cast<int>(inDog->GetExistingPoints().ExistingPoints(
							ARBDogExistingPoints::eSpeed,
							pVenue, ARBConfigMultiQPtr(), pDiv, pLevel, ARBConfigEventPtr(), dateFrom, dateTo));
					}
					if (0 < pts.ptLifetime.size())
						lifetime.push_back(pts);
					if (0 < pts.ptPlacement.size())
						placement.push_back(pts);
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
			CPointsDataLifetime* pData = new CPointsDataLifetime(pParent, pDoc, true, pVenue);
			double pts = 0;
			double ptFiltered = 0;
			typedef std::map<tstring, CPointsDataLifetimeDiv*> DivLifetime;
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
					pDivData = new CPointsDataLifetimeDiv(pParent, pDoc, true, pVenue, iter->pDiv->GetName());
					divs.insert(DivLifetime::value_type(iter->pDiv->GetName(), pDivData));
				}

				double pts2 = 0.0;
				double ptFiltered2 = 0;
				for (LifeTimePointList::iterator iter2 = (*iter).ptLifetime.begin();
					iter2 != (*iter).ptLifetime.end();
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
		if (0 < placement.size())
		{
			CPointsDataLifetime* pData = new CPointsDataLifetime(pParent, pDoc, false, pVenue);
			double pts = 0;
			double ptFiltered = 0;
			typedef std::map<tstring, CPointsDataLifetimeDiv*> DivLifetime;
			DivLifetime divs;
			for (LifeTimePointsList::iterator iter = placement.begin();
				iter != placement.end();
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
					pDivData = new CPointsDataLifetimeDiv(pParent, pDoc, false, pVenue, iter->pDiv->GetName());
					divs.insert(DivLifetime::value_type(iter->pDiv->GetName(), pDivData));
				}

				double pts2 = 0.0;
				double ptFiltered2 = 0;
				for (LifeTimePointList::iterator iter2 = (*iter).ptPlacement.begin();
					iter2 != (*iter).ptPlacement.end();
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
		if (bRunsInserted)
			m_Lines.push_back(CPointsDataBasePtr(new CPointsDataSeparator(pParent, pDoc, _T("</table>"))));
	}

	// After all the venues, we do 'other points'.
	ARBConfigOtherPointsList const& other = pDoc->GetConfig().GetOtherPoints();
	if (0 < other.size())
	{
		CString str;
		str.LoadString(IDS_OTHERPOINTS);
		tstring table;
		table = _T("<h2>");
		table += (LPCTSTR)str;
		table += _T("</h2><table border=\"2\" cellspacing=\"1\" cellpadding=\"1\">");

		m_Lines.push_back(CPointsDataBasePtr(new CPointsDataSeparator(pParent, pDoc, table)));
		m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pParent, pDoc, false)));
		m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pParent, pDoc, false, str)));
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
				m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pParent, pDoc, true, _T(""), pOther->GetName().c_str())));
				{
					std::set<tstring> tally;
					std::list<OtherPtInfo>::iterator iter;
					for (iter = runs.begin(); iter != runs.end(); ++iter)
					{
						tally.insert((*iter).m_Event);
					}
					for (std::set<tstring>::iterator iterTally = tally.begin();
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
				m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pParent, pDoc, true, _T(""), pOther->GetName().c_str())));
				{
					std::set<tstring> tally;
					std::list<OtherPtInfo>::iterator iter;
					for (iter = runs.begin(); iter != runs.end(); ++iter)
					{
						tally.insert((*iter).m_Level);
					}
					for (std::set<tstring>::iterator iterTally = tally.begin();
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
				m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pParent, pDoc, true, _T(""), pOther->GetName().c_str())));
				{
					typedef std::pair<tstring, tstring> LevelEvent;
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
