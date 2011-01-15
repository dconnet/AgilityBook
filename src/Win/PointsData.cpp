/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Data classes behind the points view items.
 * @author David Connet
 *
 * Revision History
 * @li 2010-12-24 DRC Accumulate speed points by division.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-01-26 DRC Ported to wxWidgets.
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

#include <algorithm>
#include <map>
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookPointsView.h"
#include "ARBDate.h"
#include "ARBDog.h"
#include "DlgDog.h"
#include "DlgListViewer.h"
#include "DlgTitle.h"
#include "FilterOptions.h"
#include <wx/utils.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

static const wxString s_TableHeader(wxT("<table border=\"2\" cellspacing=\"0\" cellpadding=\"2\">"));


static wxString Sanitize(wxString const& inRawData, bool nbsp = false)
{
	wxString data = SanitizeStringForHTML(inRawData);
	if (nbsp && data.empty())
		data = wxT("&nbsp;");
	return data;
}

/////////////////////////////////////////////////////////////////////////////

LifeTimePointInfoPtr LifeTimePointInfo::New(
		wxString const& inSort1,
		wxString const& inSort2,
		double inPoints,
		double inFiltered)
{
	return LifeTimePointInfoPtr(new LifeTimePointInfo(inSort1, inSort2, inPoints, inFiltered));
}


LifeTimePointInfo::LifeTimePointInfo(
		wxString const& inSort1,
		wxString const& inSort2,
		double inPoints,
		double inFiltered)
	: sort1(inSort1)
	, sort2(inSort2)
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

CPointsDataBase::CPointsDataBase(CAgilityBookDoc* pDoc)
	: m_pDoc(pDoc)
{
}


CPointsDataBase::~CPointsDataBase()
{
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataSeparator::CPointsDataSeparator(
		CAgilityBookDoc* pDoc,
		wxString const& inHtml)
	: CPointsDataBase(pDoc)
	, m_Html(inHtml)
{
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataText::CPointsDataText(
		CAgilityBookDoc* pDoc,
		bool bUseInHtml,
		wxChar const* inCol1,
		wxChar const* inCol2)
	: CPointsDataBase(pDoc)
	, m_UseInHtml(bUseInHtml)
	, m_Col1(inCol1)
	, m_Col2(inCol2)
{
	assert(NULL != inCol1);
	assert(NULL != inCol2);
}


wxString CPointsDataText::OnNeedText(int inCol) const
{
	switch (inCol)
	{
	default:
		return wxT("");
	case 0:
		return m_Col1;
	case 1:
		return m_Col2;
	}
}


wxString CPointsDataText::GetHtml(size_t /*nCurLine*/) const
{
	wxString data;
	if (m_UseInHtml)
	{
		data << wxT("<tr>\n")
			<< wxT("<td>") << Sanitize(m_Col2, true) << wxT("</td>\n")
			<< wxT("</tr>\n");
	}
	return data;
}


bool CPointsDataText::IsEqual(CPointsDataBasePtr /*inData*/)
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataDog::CPointsDataDog(
		CAgilityBookDoc* pDoc,
		ARBDogPtr pDog)
	: CPointsDataBase(pDoc)
	, m_pDog(pDog)
{
}


wxString CPointsDataDog::OnNeedText(int inCol) const
{
	wxString str;
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
			str = ARBDate::Today().GetString();
			break;
		}
	}
	return str;
}


wxString CPointsDataDog::GetHtml(size_t nCurLine) const
{
	wxString data;
	if (m_pDog)
	{
		data << wxT("<h1 align=\"center\">") << _("IDS_TITLING_POINTS") << wxT(" ")
			<< Sanitize(ARBDate::Today().GetString())
			<< wxT("</h1>")
			<< wxT("<h1><a href=\"") << ARB_PROTOCOL
#if defined(_MSC_VER) && _MSC_VER < 1400 // VC7 casting warning
			<< static_cast<unsigned int>(nCurLine)
#else
			<< nCurLine
#endif
			<< wxT("\">") << Sanitize(m_pDog->GetCallName());
		if (!m_pDog->GetRegisteredName().empty())
		{
			data << wxT("</a> [")
				<< Sanitize(m_pDog->GetRegisteredName())
				<< wxT("]");
		}
		data << wxT("</h1>\n");

	}
	return data;
}


void CPointsDataDog::Details() const
{
	CDlgDog dlg(m_pDoc, m_pDog, NULL, 0);
	dlg.ShowModal();
}


bool CPointsDataDog::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataDogPtr inDog = std::tr1::dynamic_pointer_cast<CPointsDataDog, CPointsDataBase>(inData);
	if (inDog)
		return m_pDog->GetCallName() == inDog->m_pDog->GetCallName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataVenue::CPointsDataVenue(
		CAgilityBookDoc* pDoc,
		ARBDogPtr pDog,
		ARBConfigVenuePtr pVenue)
	: CPointsDataBase(pDoc)
	, m_pDog(pDog)
	, m_pVenue(pVenue)
{
}


wxString CPointsDataVenue::OnNeedText(int inCol) const
{
	wxString str;
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
					str = wxT("[") + pRegNum->GetNumber() + wxT("]");
				}
			}
			break;
		}
	}
	return str;
}


wxString CPointsDataVenue::GetHtml(size_t nCurLine) const
{
	wxString data;
	if (m_pVenue)
	{
		data << wxT("<h2>");
		if (m_pVenue->GetURL().empty())
		{
			data << Sanitize(m_pVenue->GetName());
		}
		else
		{
			data << wxT("<a href=\"")
				<< m_pVenue->GetURL()
				<< wxT("\">")
				<< Sanitize(m_pVenue->GetName())
				<< wxT("</a>");
		}
		if (m_pDog)
		{
			ARBDogRegNumPtr pRegNum;
			if (m_pDog->GetRegNums().FindRegNum(m_pVenue->GetName(), &pRegNum))
			{
				data << wxT(" [<a href=\"") << ARB_PROTOCOL
#if defined(_MSC_VER) && _MSC_VER < 1400 // VC7 casting warning
					<< static_cast<unsigned int>(nCurLine)
#else
					<< nCurLine
#endif
					<< wxT("\">") << Sanitize(pRegNum->GetNumber())
					<< wxT("</a>]\n");
			}
		}
		data << wxT("</h2>\n");
	}
	return data;
}


void CPointsDataVenue::Details() const
{
	if (m_pDog && m_pVenue)
	{
		ARBDogRegNumPtr pRegNum;
		if (m_pDog->GetRegNums().FindRegNum(m_pVenue->GetName(), &pRegNum))
		{
			CDlgDog dlg(m_pDoc, m_pDog, NULL, 2);
			dlg.ShowModal();
		}
		else
			wxBell();
	}
	else
		wxBell();
}


bool CPointsDataVenue::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataVenuePtr inVenue = std::tr1::dynamic_pointer_cast<CPointsDataVenue, CPointsDataBase>(inData);
	if (inVenue)
		return m_pVenue->GetName() == inVenue->m_pVenue->GetName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataTitle::CPointsDataTitle(
		CAgilityBookDoc* pDoc,
		ARBDogPtr pDog,
		ARBDogTitlePtr pTitle)
	: CPointsDataBase(pDoc)
	, m_pDog(pDog)
	, m_pTitle(pTitle)
{
}


wxString CPointsDataTitle::OnNeedText(int inCol) const
{
	wxString str;
	if (m_pTitle)
	{
		switch (inCol)
		{
		case 1:
			str = m_pTitle->GetDate().GetString();
			break;
		case 2:
			str = m_pDoc->Book().GetConfig().GetTitleCompleteName(m_pTitle, false);
			if (m_pTitle->GetReceived())
				str += wxT("*");
			break;
		}
	}
	return str;
}


wxString CPointsDataTitle::GetHtml(size_t nCurLine) const
{
	wxString data;
	if (m_pTitle)
	{
		data << wxT("<tr>\n")
			<< wxT("<td>") << Sanitize(OnNeedText(1), true) << wxT("</td>\n")
			<< wxT("<td><a href=\"") << ARB_PROTOCOL
#if defined(_MSC_VER) && _MSC_VER < 1400 // VC7 casting warning
			<< static_cast<unsigned int>(nCurLine)
#else
			<< nCurLine
#endif
			<< wxT("\">") << Sanitize(OnNeedText(2)) << wxT("</a></td>\n")
			<< wxT("</tr>\n");
	}
	return data;
}


void CPointsDataTitle::Details() const
{
	CDlgTitle dlg(m_pDoc->Book().GetConfig(), m_pDog->GetTitles(), m_pTitle);
	if (wxID_OK == dlg.ShowModal())
	{
		m_pDoc->Modify(true);
		std::vector<CVenueFilter> venues;
		CFilterOptions::Options().GetFilterVenue(venues);
		m_pDoc->ResetVisibility(venues, m_pTitle);
		CUpdateHint hint(UPDATE_POINTS_VIEW);
		m_pDoc->UpdateAllViews(NULL, &hint);
	}
}


bool CPointsDataTitle::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataTitlePtr inTitle = std::tr1::dynamic_pointer_cast<CPointsDataTitle, CPointsDataBase>(inData);
	if (inTitle)
		return m_pTitle->GetVenue() == inTitle->m_pTitle->GetVenue()
			&& m_pTitle->GetRawName() == inTitle->m_pTitle->GetRawName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataEvent::CPointsDataEvent(
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
		wxString const& inRunCount,
		wxString const& inQcount,
		wxString const& inPts,
		wxString const& inSuperQ,
		wxString const& inSpeed)
	: CPointsDataBase(pDoc)
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


wxString CPointsDataEvent::OnNeedText(int inCol) const
{
	wxString str;
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
		if (0 < m_SuperQ.length() && 0 < m_Speed.length())
			str = m_Speed;
		break;
	}
	return str;
}


wxString CPointsDataEvent::GetHtml(size_t nCurLine) const
{
	wxString data;
	data << wxT("<tr>\n")
		<< wxT("<td>") << Sanitize(OnNeedText(1), true) << wxT("</td>\n")
		<< wxT("<td>") << Sanitize(OnNeedText(2), true) << wxT("</td>\n")
		<< wxT("<td>") << Sanitize(OnNeedText(3), true) << wxT("</td>\n")
		<< wxT("<td><a href=\"") << ARB_PROTOCOL
#if defined(_MSC_VER) && _MSC_VER < 1400 // VC7 casting warning
		<< static_cast<unsigned int>(nCurLine)
#else
		<< nCurLine
#endif
		<< wxT("\">") << Sanitize(OnNeedText(4)) << wxT("</a></td>\n")
		<< wxT("<td>") << Sanitize(OnNeedText(5), true) << wxT("</td>\n")
		<< wxT("<td align=\"right\">") << Sanitize(OnNeedText(6), true) << wxT("</td>\n")
		<< wxT("<td>") << Sanitize(OnNeedText(7), true) << wxT("</td>\n")
		<< wxT("<td>") << Sanitize(OnNeedText(8), true) << wxT("</td>\n")
		<< wxT("</tr>\n");
	return data;
}


void CPointsDataEvent::Details() const
{
	wxString str;
	str << _("IDS_RUNS") << wxT(": ")
		<< m_Div->GetName()
		<< wxT("/")
		<< m_Level->GetName()
		<< wxT("/")
		<< m_Event->GetName();
	RunInfoData data(m_Dog, m_Venue, m_Div, m_Level, m_Event);
	CDlgListViewer dlg(m_pDoc, str, m_Dog ? &data : NULL, m_Matching);
	dlg.ShowModal();
}


bool CPointsDataEvent::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataEventPtr inEvent = std::tr1::dynamic_pointer_cast<CPointsDataEvent, CPointsDataBase>(inData);
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
		CAgilityBookDoc* pDoc,
		bool bLifetime,
		ARBConfigVenuePtr inVenue)
	: CPointsDataBase(pDoc)
	, m_bLifetime(bLifetime)
	, m_Venue(inVenue)
	, m_Lifetime(0.0)
	, m_Filtered(0.0)
{
}


void CPointsDataLifetime::AddLifetimeInfo(
		wxString const& inDiv,
		wxString const& inLevel,
		double inLifetime,
		double inFiltered)
{
	m_Data.push_back(LifeTimePointInfo::New(inDiv, inLevel, inLifetime, inFiltered));
	m_Lifetime += inLifetime;
	m_Filtered += inFiltered;
}


wxString CPointsDataLifetime::OnNeedText(int inCol) const
{
	wxString str;
	switch (inCol)
	{
	case 1:
		if (m_bLifetime)
		{
			wxString lifetime;
			if (m_Venue->HasLifetimeName())
				lifetime = m_Venue->GetLifetimeName();
			else
				lifetime = _("IDS_TITLEPOINT_LIFETIME");
			str = wxString::Format(_("IDS_LIFETIME_POINTS"), lifetime.c_str());
		}
		else
			str = _("IDS_PLACEMENT_POINTS");
		break;
	case 2:
		str << _("IDS_TOTAL") << wxT(": ");
		if (0 < m_Filtered)
			str << m_Lifetime - m_Filtered << wxT(" (") << m_Lifetime << wxT(")");
		else
			str << m_Lifetime;
		break;
	}
	return str;
}


wxString CPointsDataLifetime::GetHtml(size_t nCurLine) const
{
	wxString data;
	data << wxT("<tr><td>&nbsp;</td></tr>\n")
		<< wxT("<tr>\n")
		<< wxT("<td>") << Sanitize(OnNeedText(1), true) << wxT("</td>\n")
		<< wxT("<td align=\"right\"><a href=\"") << ARB_PROTOCOL
#if defined(_MSC_VER) && _MSC_VER < 1400 // VC7 casting warning
		<< static_cast<unsigned int>(nCurLine)
#else
		<< nCurLine
#endif
		<< wxT("\">") << Sanitize(OnNeedText(2)) << wxT("</a></td>\n")
		<< wxT("</tr>\n");
	return data;
}


void CPointsDataLifetime::Details() const
{
	wxString caption(m_Venue->GetName());
	wxString str;
	if (m_bLifetime)
	{
		wxString lifetime;
		if (m_Venue->HasLifetimeName())
			lifetime = m_Venue->GetLifetimeName();
		else
			lifetime = _("IDS_TITLEPOINT_LIFETIME");
		str = wxString::Format(_("IDS_LIFETIME_POINTS"), lifetime.c_str());
	}
	else
		str = _("IDS_PLACEMENT_POINTS");
	caption += wxT(" ") + str;
	CDlgListViewer dlg(m_pDoc, caption, m_Data);
	dlg.ShowModal();
}


bool CPointsDataLifetime::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataLifetimePtr inLife = std::tr1::dynamic_pointer_cast<CPointsDataLifetime, CPointsDataBase>(inData);
	if (inLife)
		return m_Venue == inLife->m_Venue;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataLifetimeByName::CPointsDataLifetimeByName(
		CAgilityBookDoc* pDoc,
		bool bLifetime,
		ARBConfigVenuePtr inVenue,
		wxString const& inName)
	: CPointsDataLifetime(pDoc, bLifetime, inVenue)
	, m_Name(inName)
{
}


void CPointsDataLifetimeByName::AddLifetimeInfo(
		wxString const& inSort1,
		wxString const& inSort2,
		double inLifetime,
		double inFiltered)
{
	if (inSort1 == m_Name)
	{
		m_Data.push_back(LifeTimePointInfo::New(inSort1, inSort2, inLifetime, inFiltered));
		m_Lifetime += inLifetime;
		m_Filtered += inFiltered;
	}
}


wxString CPointsDataLifetimeByName::OnNeedText(int inCol) const
{
	wxString str;
	switch (inCol)
	{
	case 2:
		str << m_Name << wxT(": ");
		if (0 < m_Filtered)
			str << m_Lifetime - m_Filtered << wxT(" (") << m_Lifetime << wxT(")");
		else
			str << m_Lifetime;
		break;
	}
	return str;
}


wxString CPointsDataLifetimeByName::GetHtml(size_t nCurLine) const
{
	wxString data;
	data << wxT("<tr>\n")
		<< wxT("<td>&nbsp;</td>\n")
		<< wxT("<td align=\"right\">") << Sanitize(OnNeedText(2), true) << wxT("</td>\n")
		<< wxT("</tr>\n");
	return data;
}


bool CPointsDataLifetimeByName::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataLifetimeByNamePtr inLife = std::tr1::dynamic_pointer_cast<CPointsDataLifetimeByName, CPointsDataBase>(inData);
	if (inLife)
		return CPointsDataLifetimeByName::IsEqual(inData)
			&& m_Name == inLife->m_Name;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataMultiQs::CPointsDataMultiQs(
		CAgilityBookDoc* pDoc,
		ARBDogPtr inDog,
		ARBConfigVenuePtr inVenue,
		ARBConfigMultiQPtr inMultiQ,
		std::set<MultiQdata> const& inMQs)
	: CPointsDataBase(pDoc)
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


wxString CPointsDataMultiQs::OnNeedText(int inCol) const
{
	wxString str;
	if (7 == inCol)
	{
		str << m_ExistingDblQs + m_MQs.size() << wxT(" ") << m_MultiQ->GetShortName();
	}
	return str;
}


wxString CPointsDataMultiQs::GetHtml(size_t nCurLine) const
{
	wxString data;
	data << wxT("<tr>\n")
		<< wxT("<td colspan=\"6\"/>\n")
		<< wxT("<td><a href=\"") << ARB_PROTOCOL
#if defined(_MSC_VER) && _MSC_VER < 1400 // VC7 casting warning
		<< static_cast<unsigned int>(nCurLine)
#else
		<< nCurLine
#endif
		<< wxT("\">")
		<< Sanitize(OnNeedText(7)) << wxT("</a></td>\n")
		<< wxT("</tr>\n");
	return data;
}


void CPointsDataMultiQs::Details() const
{
	MultiQInfoData data(m_Dog, m_Venue, m_MultiQ);
	CDlgListViewer dlg(m_pDoc, m_MultiQ->GetName(), &data, m_MQs);
	dlg.ShowModal();
}


bool CPointsDataMultiQs::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataMultiQsPtr inMulti = std::tr1::dynamic_pointer_cast<CPointsDataMultiQs, CPointsDataBase>(inData);
	if (inMulti)
		return m_Venue->GetName() == inMulti->m_Venue->GetName()
			&& m_MultiQ->GetName() == inMulti->m_MultiQ->GetName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataSpeedPts::CPointsDataSpeedPts(
		CAgilityBookDoc* pDoc,
		ARBConfigVenuePtr inVenue,
		ARBConfigDivisionPtr inDiv,
		int inPts)
	: CPointsDataBase(pDoc)
	, m_Venue(inVenue)
	, m_Div(inDiv)
	, m_Pts(inPts)
{
}


wxString CPointsDataSpeedPts::OnNeedText(int inCol) const
{
	wxString str;
	switch (inCol)
	{
	case 1:
		str = m_Div->GetName();
		break;
	case 7:
		str = wxString::Format(_("IDS_POINTS_SPEED"), m_Pts);
		break;
	}
	return str;
}


wxString CPointsDataSpeedPts::GetHtml(size_t nCurLine) const
{
	wxString data;
	data << wxT("<tr>\n")
		<< wxT("<td>") << Sanitize(OnNeedText(1), true) << wxT("</td>\n")
		<< wxT("<td colspan=\"5\"/>\n")
		<< wxT("<td>") << Sanitize(OnNeedText(7), true) << wxT("</td>\n")
		<< wxT("</tr>\n");
	return data;
}


bool CPointsDataSpeedPts::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataSpeedPtsPtr inPts = std::tr1::dynamic_pointer_cast<CPointsDataSpeedPts, CPointsDataBase>(inData);
	if (inPts)
		return m_Venue->GetName() == inPts->m_Venue->GetName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPoints::CPointsDataOtherPoints(
		CAgilityBookDoc* pDoc,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataBase(pDoc)
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
		CAgilityBookDoc* pDoc,
		wxString const& inName,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataOtherPoints(pDoc, inRunList)
	, m_Name(inName)
{
}


wxString CPointsDataOtherPointsTallyAll::OnNeedText(int inCol) const
{
	wxString str;
	switch (inCol)
	{
	case 1:
		str = m_Name;
		break;
	case 2:
		str << m_Score;
		break;
	}
	return str;
}


wxString CPointsDataOtherPointsTallyAll::GetHtml(size_t nCurLine) const
{
	wxString data;
	data << wxT("<tr>\n")
		<< wxT("<td>") << Sanitize(OnNeedText(1), true) << wxT("</td>\n")
		<< wxT("<td align=\"right\"><a href=\"") << ARB_PROTOCOL
#if defined(_MSC_VER) && _MSC_VER < 1400 // VC7 casting warning
		<< static_cast<unsigned int>(nCurLine)
#else
		<< nCurLine
#endif
		<< wxT("\">")
		<< Sanitize(OnNeedText(2)) << wxT("</a></td>\n")
		<< wxT("</tr>\n");
	return data;
}


void CPointsDataOtherPointsTallyAll::Details() const
{
	CDlgListViewer dlg(m_pDoc, _("IDS_OTHERPOINTS"), m_RunList);
	dlg.ShowModal();
}


bool CPointsDataOtherPointsTallyAll::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataOtherPointsTallyAllPtr inPts = std::tr1::dynamic_pointer_cast<CPointsDataOtherPointsTallyAll, CPointsDataBase>(inData);
	if (inPts)
		return m_Name == inPts->m_Name;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPointsTallyAllByEvent::CPointsDataOtherPointsTallyAllByEvent(
		CAgilityBookDoc* pDoc,
		wxString const& inEvent,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataOtherPoints(pDoc, inRunList)
	, m_Event(inEvent)
{
}


wxString CPointsDataOtherPointsTallyAllByEvent::OnNeedText(int inCol) const
{
	wxString str;
	switch (inCol)
	{
	case 2: // Event
		str = m_RunList.begin()->m_Event;
		break;
	case 3:
		str << m_Score;
		break;
	}
	return str;
}


wxString CPointsDataOtherPointsTallyAllByEvent::GetHtml(size_t nCurLine) const
{
	wxString data;
	data << wxT("<tr>\n")
		<< wxT("<td>&nbsp;</td>\n")
		<< wxT("<td>") << Sanitize(OnNeedText(2), true) << wxT("</td>\n")
		<< wxT("<td align=\"right\"><a href=\"") << ARB_PROTOCOL
#if defined(_MSC_VER) && _MSC_VER < 1400 // VC7 casting warning
		<< static_cast<unsigned int>(nCurLine)
#else
		<< nCurLine
#endif
		<< wxT("\">")
		<< Sanitize(OnNeedText(3)) << wxT("</a></td>\n")
		<< wxT("</tr>\n");
	return data;
}


void CPointsDataOtherPointsTallyAllByEvent::Details() const
{
	CDlgListViewer dlg(m_pDoc, _("IDS_OTHERPOINTS"), m_RunList);
	dlg.ShowModal();
}


bool CPointsDataOtherPointsTallyAllByEvent::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataOtherPointsTallyAllByEventPtr inPts = std::tr1::dynamic_pointer_cast<CPointsDataOtherPointsTallyAllByEvent, CPointsDataBase>(inData);
	if (inPts)
		return m_Event == inPts->m_Event;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPointsTallyLevel::CPointsDataOtherPointsTallyLevel(
		CAgilityBookDoc* pDoc,
		wxString const& inLevel,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataOtherPoints(pDoc, inRunList)
	, m_Level(inLevel)
{
}


wxString CPointsDataOtherPointsTallyLevel::OnNeedText(int inCol) const
{
	wxString str;
	switch (inCol)
	{
	case 2: // Level
		str = m_RunList.begin()->m_Level;
		break;
	case 3:
		str << m_Score;
		break;
	}
	return str;
}


wxString CPointsDataOtherPointsTallyLevel::GetHtml(size_t nCurLine) const
{
	wxString data;
	data << wxT("<tr>\n")
		<< wxT("<td>&nbsp;</td>\n")
		<< wxT("<td>") << Sanitize(OnNeedText(2), true) << wxT("</td>\n")
		<< wxT("<td align=\"right\"><a href=\"") << ARB_PROTOCOL
#if defined(_MSC_VER) && _MSC_VER < 1400 // VC7 casting warning
		<< static_cast<unsigned int>(nCurLine)
#else
		<< nCurLine
#endif
		<< wxT("\">")
		<< Sanitize(OnNeedText(3)) << wxT("</a></td>\n")
		<< wxT("</tr>\n");
	return data;
}


void CPointsDataOtherPointsTallyLevel::Details() const
{
	CDlgListViewer dlg(m_pDoc, _("IDS_OTHERPOINTS"), m_RunList);
	dlg.ShowModal();
}


bool CPointsDataOtherPointsTallyLevel::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataOtherPointsTallyLevelPtr inPts = std::tr1::dynamic_pointer_cast<CPointsDataOtherPointsTallyLevel, CPointsDataBase>(inData);
	if (inPts)
		return m_Level == inPts->m_Level;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPointsTallyLevelByEvent::CPointsDataOtherPointsTallyLevelByEvent(
		CAgilityBookDoc* pDoc,
		wxString const& inLevel,
		wxString const& inEvent,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataOtherPoints(pDoc, inRunList)
	, m_Level(inLevel)
	, m_Event(inEvent)
{
}


wxString CPointsDataOtherPointsTallyLevelByEvent::OnNeedText(int inCol) const
{
	wxString str;
	switch (inCol)
	{
	case 2: // Level
		str = m_RunList.begin()->m_Level;
		break;
	case 3: // Event
		str = m_RunList.begin()->m_Event;
		break;
	case 4:
		str << m_Score;
		break;
	}
	return str;
}


wxString CPointsDataOtherPointsTallyLevelByEvent::GetHtml(size_t nCurLine) const
{
	wxString data;
	data << wxT("<tr>\n")
		<< wxT("<td>&nbsp;</td>\n")
		<< wxT("<td>") << Sanitize(OnNeedText(2), true) << wxT("</td>\n")
		<< wxT("<td>") << Sanitize(OnNeedText(3), true) << wxT("</td>\n")
		<< wxT("<td align=\"right\"><a href=\"") << ARB_PROTOCOL
#if defined(_MSC_VER) && _MSC_VER < 1400 // VC7 casting warning
		<< static_cast<unsigned int>(nCurLine)
#else
		<< nCurLine
#endif
		<< wxT("\">")
		<< Sanitize(OnNeedText(4)) << wxT("</a></td>\n")
		<< wxT("</tr>\n");
	return data;
}


void CPointsDataOtherPointsTallyLevelByEvent::Details() const
{
	CDlgListViewer dlg(m_pDoc, _("IDS_OTHERPOINTS"), m_RunList);
	dlg.ShowModal();
}


bool CPointsDataOtherPointsTallyLevelByEvent::IsEqual(CPointsDataBasePtr inData)
{
	CPointsDataOtherPointsTallyLevelByEventPtr inPts = std::tr1::dynamic_pointer_cast<CPointsDataOtherPointsTallyLevelByEvent, CPointsDataBase>(inData);
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
				assert(0);
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
		CAgilityBookDoc* pDoc,
		ARBDogPtr inDog,
		ARBConfigVenuePtr pVenue)
{
	m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pDoc, false)));
	m_Lines.push_back(CPointsDataBasePtr(new CPointsDataVenue(pDoc, inDog, pVenue)));
}


void CPointsDataItems::LoadData(
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
	m_Lines.push_back(CPointsDataBasePtr(new CPointsDataDog(pDoc, inDog)));

	// For each venue...
	for (ARBConfigVenueList::const_iterator iterVenue = pDoc->Book().GetConfig().GetVenues().begin();
		iterVenue != pDoc->Book().GetConfig().GetVenues().end();
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
					InsertVenueHeader(pDoc, inDog, pVenue);
					wxString data(wxT("<h3>"));
					data += _("IDS_TITLES");
					data += wxT("</h3>");
					data += s_TableHeader;
					m_Lines.push_back(CPointsDataBasePtr(new CPointsDataSeparator(pDoc, data)));
				}
				m_Lines.push_back(CPointsDataBasePtr(new CPointsDataTitle(pDoc, inDog, pTitle)));
			}
		}
		if (bHeaderInserted)
			m_Lines.push_back(CPointsDataBasePtr(new CPointsDataSeparator(pDoc, wxT("</table>"))));

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
				InsertVenueHeader(pDoc, inDog, pVenue);
			}
			bRunsInserted = true;
			wxString data(wxT("<h3>"));
			data += _("IDS_RUNS");
			data += wxT("</h3>");
			data += s_TableHeader;
			m_Lines.push_back(CPointsDataBasePtr(new CPointsDataSeparator(pDoc, data)));
			std::vector<CPointsDataBasePtr> speedPtsData;
			// Show events sorted out by division/level.
			std::vector<CPointsDataEvent*> items;
			int idxDiv = 0;
			for (ARBConfigDivisionList::const_iterator iterDiv = pVenue->GetDivisions().begin();
				iterDiv != pVenue->GetDivisions().end();
				++idxDiv, ++iterDiv)
			{
				bool bHasSpeedPts = false;
				int speedPts = 0;
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
						std::set<wxString> judges;
						std::set<wxString> judgesQ;
						std::set<wxString> partners;
						std::set<wxString> partnersQ;
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
									assert(pScoring);
									if (!pScoring) continue; // Shouldn't need it...
									if (*pScoring != *pScoringMethod)
										continue;
									bool bRunVisible = (!pRun->IsFiltered(ARBBase::eIgnoreQ)
									&& CFilterOptions::Options().IsRunVisible(venues, pVenue, pTrial, pRun));
									if (bRunVisible)
									{
										// Don't tally NA runs for titling events.
										if (!pRun->GetQ().AllowTally())
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
												wxString p = (*iterPartner)->GetDog();
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
						if (0 < points || 0 < allmatching.size())
						{
							wxString strRunCount = wxString::Format(_("IDS_POINTS_RUNS_JUDGES"),
								allmatching.size(),
								judges.size());
							if (pEvent->HasPartner() && 0 < partners.size())
							{
								strRunCount += wxString::Format(_("IDS_POINTS_PARTNERS"), partners.size());
							}
							double percentQs = 0.0;
							if (0 < allmatching.size())
								percentQs = (static_cast<double>(nCleanQ + nNotCleanQ) / static_cast<double>(allmatching.size())) * 100;
							wxString strQcount = wxString::Format(_("IDS_POINTS_QS"),
								nCleanQ + nNotCleanQ,
								static_cast<int>(percentQs));
							if (0 < nCleanQ)
							{
								strQcount += wxString::Format(_("IDS_POINTS_CLEAN"), nCleanQ);
							}
							if (0 < judgesQ.size())
							{
								strQcount += wxString::Format(_("IDS_POINTS_JUDGES"), judgesQ.size());
							}
							if (pEvent->HasPartner() && 0 < partnersQ.size())
							{
								strQcount += wxString::Format(_("IDS_POINTS_PARTNERS"), partnersQ.size());
							}
							wxString strPts;
							wxString strSuperQ;
							strPts << points + nExistingSQ;
							if (hasSQs)
							{
								SQs += nExistingSQ;
								strSuperQ = wxString::Format(_("IDS_POINTS_SQS"), SQs);
							}
							wxString strSpeed;
							if (bHasSpeedPts && 0 < speedPtsEvent)
							{
								strSpeed = wxString::Format(_("IDS_POINTS_SPEED_SUBTOTAL"), speedPtsEvent);
							}
							items.push_back(new CPointsDataEvent(pDoc,
								!ARBDouble::equal(0.0, nExistingPts + nExistingSQ) ? inDog : ARBDogPtr(),
								allmatching,
								pVenue,
								pDiv, idxDiv,
								pLevel, idxLevel,
								pEvent, idxEvent,
								strRunCount,
								strQcount,
								strPts,
								strSuperQ,
								strSpeed));
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
				} // level loop
				if (bHasSpeedPts)
				{
					speedPtsData.push_back(CPointsDataBasePtr(new CPointsDataSpeedPts(pDoc, pVenue, pDiv, speedPts)));
				}
			} // division loop
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
			if (!speedPtsData.empty())
			{
				m_Lines.insert(m_Lines.end(), speedPtsData.begin(), speedPtsData.end());
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
						std::vector<ARBConfigMultiQPtr> multiQs;
						if (0 < pRun->GetMultiQs(multiQs)
						&& !pRun->IsFiltered(ARBBase::eIgnoreQ)
						&& CFilterOptions::Options().IsRunVisible(venues, pVenue, pTrial, pRun))
						{
							for (std::vector<ARBConfigMultiQPtr>::iterator iMultiQ = multiQs.begin(); iMultiQ != multiQs.end(); ++iMultiQ)
							{
								MQs[*iMultiQ].insert(MultiQdata(pRun->GetDate(), pTrial));
							}
						}
					}
				}
				for (std::map<ARBConfigMultiQPtr, std::set<MultiQdata> >::iterator iter = MQs.begin();
					iter != MQs.end();
					++iter)
				{
					m_Lines.push_back(CPointsDataBasePtr(new CPointsDataMultiQs(pDoc, inDog, pVenue, (*iter).first, (*iter).second)));
				}
			}
		}

		// Next comes lifetime points.
		if (0 < lifetime.size())
		{
			CPointsDataLifetime* pData = new CPointsDataLifetime(pDoc, true, pVenue);
			typedef std::map<wxString, CPointsDataLifetimeByName*> NamedLifetime;
			NamedLifetime subgroups;
			if (CAgilityBookOptions::GetViewLifetimePointsByEvent())
			{
				// Gather event names
				std::set<wxString> names;
				for (LifeTimePointsList::iterator iter = lifetime.begin();
					iter != lifetime.end();
					++iter)
				{
					for (LifeTimePointList::iterator iter2 = (*iter).ptLifetime.begin();
						iter2 != (*iter).ptLifetime.end();
						++iter2)
					{
						names.insert(iter2->eventName);
					}
				}
				for (std::set<wxString>::iterator iName = names.begin(); iName != names.end(); ++iName)
				{
					double pts2 = 0.0;
					double ptFiltered2 = 0;
					for (LifeTimePointsList::iterator iter = lifetime.begin();
						iter != lifetime.end();
						++iter)
					{
						for (LifeTimePointList::iterator iter2 = (*iter).ptLifetime.begin();
							iter2 != (*iter).ptLifetime.end();
							++iter2)
						{
							if (iter2->eventName == *iName)
							{
								pts2 += (*iter2).points;
								if ((*iter2).bFiltered)
									ptFiltered2 += (*iter2).points;
							}
						}
					}
					pData->AddLifetimeInfo(*iName, wxString(), pts2, ptFiltered2);
				}
			}
			for (LifeTimePointsList::iterator iter = lifetime.begin();
				iter != lifetime.end();
				++iter)
			{
				CPointsDataLifetimeByName* pNameData = NULL;
				NamedLifetime::iterator it = subgroups.find(iter->pDiv->GetName());
				if (subgroups.end() != it)
				{
					pNameData = it->second;
				}
				else
				{
					pNameData = new CPointsDataLifetimeByName(pDoc, true, pVenue, iter->pDiv->GetName());
					subgroups.insert(NamedLifetime::value_type(iter->pDiv->GetName(), pNameData));
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

				if (!CAgilityBookOptions::GetViewLifetimePointsByEvent())
					pData->AddLifetimeInfo(iter->pDiv->GetName(), iter->pLevel->GetName(), pts2, ptFiltered2);
				pNameData->AddLifetimeInfo(iter->pDiv->GetName(), iter->pLevel->GetName(), pts2, ptFiltered2);
			}

			m_Lines.push_back(CPointsDataBasePtr(pData));
			if (1 < subgroups.size())
			{
				for (NamedLifetime::iterator it = subgroups.begin();
					it != subgroups.end();
					++it)
				{
					m_Lines.push_back(CPointsDataBasePtr(it->second));
				}
			}
			else if (1 == subgroups.size())
			{
				delete subgroups.begin()->second;
			}
		}
		if (0 < placement.size())
		{
			CPointsDataLifetime* pData = new CPointsDataLifetime(pDoc, false, pVenue);
			typedef std::map<wxString, CPointsDataLifetimeByName*> NamedLifetime;
			NamedLifetime subgroups;
			if (CAgilityBookOptions::GetViewLifetimePointsByEvent())
			{
				// Gather event names
				std::set<wxString> names;
				for (LifeTimePointsList::iterator iter = lifetime.begin();
					iter != lifetime.end();
					++iter)
				{
					for (LifeTimePointList::iterator iter2 = (*iter).ptLifetime.begin();
						iter2 != (*iter).ptLifetime.end();
						++iter2)
					{
						names.insert(iter2->eventName);
					}
				}
				for (std::set<wxString>::iterator iName = names.begin(); iName != names.end(); ++iName)
				{
					double pts2 = 0.0;
					double ptFiltered2 = 0;
					for (LifeTimePointsList::iterator iter = lifetime.begin();
						iter != lifetime.end();
						++iter)
					{
						for (LifeTimePointList::iterator iter2 = (*iter).ptLifetime.begin();
							iter2 != (*iter).ptLifetime.end();
							++iter2)
						{
							if (iter2->eventName == *iName)
							{
								pts2 += (*iter2).points;
								if ((*iter2).bFiltered)
									ptFiltered2 += (*iter2).points;
							}
						}
					}
					pData->AddLifetimeInfo(*iName, wxString(), pts2, ptFiltered2);
				}
			}
			for (LifeTimePointsList::iterator iter = placement.begin();
				iter != placement.end();
				++iter)
			{
				CPointsDataLifetimeByName* pNameData = NULL;
				NamedLifetime::iterator it = subgroups.find(iter->pDiv->GetName());
				if (subgroups.end() != it)
				{
					pNameData = it->second;
				}
				else
				{
					pNameData = new CPointsDataLifetimeByName(pDoc, false, pVenue, iter->pDiv->GetName());
					subgroups.insert(NamedLifetime::value_type(iter->pDiv->GetName(), pNameData));
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

				if (!CAgilityBookOptions::GetViewLifetimePointsByEvent())
					pData->AddLifetimeInfo(iter->pDiv->GetName(), iter->pLevel->GetName(), pts2, ptFiltered2);
				pNameData->AddLifetimeInfo(iter->pDiv->GetName(), iter->pLevel->GetName(), pts2, ptFiltered2);
			}
			m_Lines.push_back(CPointsDataBasePtr(pData));
			if (1 < subgroups.size())
			{
				for (NamedLifetime::iterator it = subgroups.begin();
					it != subgroups.end();
					++it)
				{
					m_Lines.push_back(CPointsDataBasePtr(it->second));
				}
			}
			else if (1 == subgroups.size())
			{
				delete subgroups.begin()->second;
			}
		}
		if (bRunsInserted)
			m_Lines.push_back(CPointsDataBasePtr(new CPointsDataSeparator(pDoc, wxT("</table>"))));
	}

	// After all the venues, we do 'other points'.
	ARBConfigOtherPointsList const& other = pDoc->Book().GetConfig().GetOtherPoints();
	if (0 < other.size())
	{
		wxString str(_("IDS_OTHERPOINTS"));
		wxString table;
		table = wxT("<h2>");
		table += str;
		table += wxT("</h2>");
		table += s_TableHeader;

		m_Lines.push_back(CPointsDataBasePtr(new CPointsDataSeparator(pDoc, table)));
		m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pDoc, false)));
		m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pDoc, false, str)));
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
				m_Lines.push_back(CPointsDataBasePtr(new CPointsDataOtherPointsTallyAll(pDoc, pOther->GetName(), runs)));
				break;

			case ARBConfigOtherPoints::eTallyAllByEvent:
				m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pDoc, true, wxT(""), pOther->GetName())));
				{
					std::set<wxString> tally;
					std::list<OtherPtInfo>::iterator iter;
					for (iter = runs.begin(); iter != runs.end(); ++iter)
					{
						tally.insert((*iter).m_Event);
					}
					for (std::set<wxString>::iterator iterTally = tally.begin();
						iterTally != tally.end();
						++iterTally)
					{
						std::list<OtherPtInfo> validRuns;
						for (iter = runs.begin(); iter != runs.end(); ++iter)
						{
							if ((*iter).m_Event == (*iterTally))
								validRuns.push_back(*iter);
						}
						m_Lines.push_back(CPointsDataBasePtr(new CPointsDataOtherPointsTallyAllByEvent(pDoc, (*iterTally), validRuns)));
					}
				}
				break;

			case ARBConfigOtherPoints::eTallyLevel:
				m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pDoc, true, wxT(""), pOther->GetName())));
				{
					std::set<wxString> tally;
					std::list<OtherPtInfo>::iterator iter;
					for (iter = runs.begin(); iter != runs.end(); ++iter)
					{
						tally.insert((*iter).m_Level);
					}
					for (std::set<wxString>::iterator iterTally = tally.begin();
						iterTally != tally.end();
						++iterTally)
					{
						std::list<OtherPtInfo> validRuns;
						for (iter = runs.begin(); iter != runs.end(); ++iter)
						{
							if ((*iter).m_Level == (*iterTally))
								validRuns.push_back(*iter);
						}
						m_Lines.push_back(CPointsDataBasePtr(new CPointsDataOtherPointsTallyLevel(pDoc, (*iterTally), validRuns)));
					}
				}
				break;

			case ARBConfigOtherPoints::eTallyLevelByEvent:
				m_Lines.push_back(CPointsDataBasePtr(new CPointsDataText(pDoc, true, wxT(""), pOther->GetName())));
				{
					typedef std::pair<wxString, wxString> LevelEvent;
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
						m_Lines.push_back(CPointsDataBasePtr(new CPointsDataOtherPointsTallyLevelByEvent(pDoc, (*iterTally).first, (*iterTally).second, validRuns)));
					}
				}
				break;
			}
		}
		m_Lines.push_back(CPointsDataBasePtr(new CPointsDataSeparator(pDoc, wxT("</table></html>"))));
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
