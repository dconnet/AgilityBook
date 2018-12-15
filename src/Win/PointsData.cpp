/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Data classes behind the points view items.
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2017-08-20 Add CPointsDataHeader, Fix MultiQ sorting.
 * 2016-04-29 Separate lifetime points from title (run) points.
 * 2014-04-23 Add anchors to all hrefs.
 * 2011-10-27 Add the MultiQ name to the Points view, span columns.
 * 2011-08-13 Don't copy internal url links to the clipboard.
 * 2010-12-24 Accumulate speed points by division.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-01-26 Ported to wxWidgets.
 * 2007-03-14 Change accumulation to work by event, not method.
 *            This removes the multiple listings in the view.
 * 2006-07-15 Add option to sort by event instead of division.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-10-18 Remember last selected item when reloading data.
 * 2005-10-14 Added a context menu.
 * 2005-05-04 Added subtotaling by division to lifetime points.
 * 2005-03-14 Show a summary of lifetime points in the list viewer.
 * 2005-01-10 Allow titles to be optionally entered multiple times.
 * 2005-01-02 Show existing points in the list viewer.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-12-03 Show all lifetime points when filtering.
 * 2004-08-06 Created
 */

#include "stdafx.h"
#include "PointsData.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookPointsView.h"
#include "DlgDog.h"
#include "DlgListViewer.h"
#include "DlgTitle.h"
#include "FilterOptions.h"

#include "ARB/ARBDog.h"
#include "ARBCommon/ARBDate.h"
#include "ARBCommon/ARBMisc.h"
#include "fmt/printf.h"
#include <algorithm>
#include <map>
#include <wx/utils.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

static wchar_t const* const s_TableHeader = L"<table border=\"2\" cellspacing=\"0\" cellpadding=\"2\">";


static std::wstring Sanitize(std::wstring const& inRawData, bool nbsp = false)
{
	std::wstring data = SanitizeStringForHTML(inRawData);
	if (nbsp && data.empty())
		data = L"&nbsp;";
	return data;
}

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class LifeTimePointInfo_concrete : public LifeTimePointInfo
	{
	public:
		LifeTimePointInfo_concrete(
				std::wstring const& inSort1,
				std::wstring const& inSort2,
				double inPoints,
				double inFiltered)
			: LifeTimePointInfo(inSort1, inSort2, inPoints, inFiltered)
		{
		}
	};
};


LifeTimePointInfoPtr LifeTimePointInfo::New(
		std::wstring const& inSort1,
		std::wstring const& inSort2,
		double inPoints,
		double inFiltered)
{
	return std::make_shared<LifeTimePointInfo_concrete>(inSort1, inSort2, inPoints, inFiltered);
}


LifeTimePointInfo::LifeTimePointInfo(
		std::wstring const& inSort1,
		std::wstring const& inSort2,
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
		ARBDogTrialPtr const& inTrial,
		ARBDogRunPtr const& inRun,
		double points,
		bool bScore,
		double score)
	: m_pTrial(inTrial)
	, m_pRun(inRun)
	, m_pExisting()
	, m_Points(points)
	, m_bScore(bScore)
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


OtherPtInfo::OtherPtInfo(ARBDogExistingPointsPtr const& inExisting)
	: m_pTrial()
	, m_pRun()
	, m_pExisting(inExisting)
	, m_Venue(inExisting->GetVenue())
	, m_Div(inExisting->GetDivision())
	, m_Level(inExisting->GetLevel())
	, m_Event(inExisting->GetEvent())
	, m_Points(inExisting->GetPoints())
	, m_bScore(false)
	, m_Score(0.0)
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
		std::wstring const& inHtml)
	: CPointsDataBase(pDoc)
	, m_Html(inHtml)
{
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataBasePtr CPointsDataText::CreateListBlankLine(CAgilityBookDoc* pDoc)
{
	// Would normally use make_shared, but VS2015 is complaining it can't access
	// the constructor.
	CPointsDataBasePtr obj;
	obj.reset(new CPointsDataText(pDoc, false));
	return obj;
}


CPointsDataBasePtr CPointsDataText::CreateListOtherPoints(CAgilityBookDoc* pDoc)
{
	std::wstring str(_("IDS_OTHERPOINTS"));
	CPointsDataBasePtr obj;
	obj.reset(new CPointsDataText(pDoc, false, str.c_str()));
	return obj;
}


CPointsDataBasePtr CPointsDataText::CreateDataLine(
		CAgilityBookDoc* pDoc,
		std::wstring const& str)
{
	CPointsDataBasePtr obj;
	obj.reset(new CPointsDataText(pDoc, true, L"", str.c_str()));
	return obj;
}


CPointsDataText::CPointsDataText(
		CAgilityBookDoc* pDoc,
		bool bUseInHtml,
		wchar_t const* inCol1,
		wchar_t const* inCol2)
	: CPointsDataBase(pDoc)
	, m_UseInHtml(bUseInHtml)
	, m_Col1(inCol1)
	, m_Col2(inCol2)
{
	assert(!!inCol1);
	assert(!!inCol2);
#ifdef _DEBUG
	if (m_UseInHtml)
		assert(m_Col1.empty());
#endif
}


std::wstring CPointsDataText::OnNeedText(int inCol) const
{
	switch (inCol)
	{
	default:
		return std::wstring();
	case 0:
		return m_Col1;
	case 1:
		return m_Col2;
	}
}


std::wstring CPointsDataText::GetHtml(
		size_t /*nCurLine*/,
		bool /*bNoInternalLinks*/) const
{
	if (m_UseInHtml)
		return fmt::format(L"<tr>\n<td>{}</td>\n</tr>\n", Sanitize(m_Col2, true));
	else
		return std::wstring();
}


bool CPointsDataText::IsEqual(CPointsDataBasePtr const& /*inData*/)
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataHeader::CPointsDataHeader(CAgilityBookDoc* pDoc)
	: CPointsDataBase(pDoc)
	, m_today(ARBDate::Today())
{
}


std::wstring CPointsDataHeader::OnNeedText(int inCol) const
{
	std::wstring str;
	switch (inCol)
	{
	case 1: // Caption
		str = _("IDS_TITLING_POINTS");
		break;
	case 2: // Current date
		str = m_today.GetString();
		break;
	}
	return str;
}


std::wstring CPointsDataHeader::GetHtml(
		size_t /*nCurLine*/,
		bool /*bNoInternalLinks*/) const
{
	return fmt::format(L"<head><title>{} {}</title></head>", _("IDS_TITLING_POINTS").wx_str(), m_today.GetString());
}


bool CPointsDataHeader::IsEqual(CPointsDataBasePtr const& /*inData*/)
{
	return ARBDate::Today() == m_today;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataDog::CPointsDataDog(
		CAgilityBookDoc* pDoc,
		ARBDogPtr const& inDog)
	: CPointsDataBase(pDoc)
	, m_pDog(inDog)
{
}


std::wstring CPointsDataDog::OnNeedText(int inCol) const
{
	std::wstring str;
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
		}
	}
	return str;
}


std::wstring CPointsDataDog::GetHtml(
		size_t nCurLine,
		bool bNoInternalLinks) const
{
	fmt::wmemory_buffer data;
	if (m_pDog)
	{
		fmt::format_to(data, L"<h1 align=\"center\">{} {}</h1><h1>",
			_("IDS_TITLING_POINTS").wx_str(),
			Sanitize(ARBDate::Today().GetString()));
		if (!bNoInternalLinks)
		{
			fmt::format_to(data, L"<a href=\"{0}{1}\" name=\"ref{1}\">", ARB_PROTOCOL, nCurLine);
		}
		fmt::format_to(data, L"{}", Sanitize(m_pDog->GetCallName()));
		if (!bNoInternalLinks)
			fmt::format_to(data, L"</a>");
		if (!m_pDog->GetRegisteredName().empty())
		{
			fmt::format_to(data, L" [{}]", Sanitize(m_pDog->GetRegisteredName()));
		}
		fmt::format_to(data, L"</h1>\n");

	}
	return fmt::to_string(data);
}


void CPointsDataDog::Details() const
{
	CDlgDog dlg(m_pDoc, m_pDog, nullptr, 0);
	dlg.ShowModal();
}


bool CPointsDataDog::IsEqual(CPointsDataBasePtr const& inData)
{
	CPointsDataDogPtr inDog = std::dynamic_pointer_cast<CPointsDataDog, CPointsDataBase>(inData);
	if (inDog)
		return m_pDog->GetCallName() == inDog->m_pDog->GetCallName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataVenue::CPointsDataVenue(
		CAgilityBookDoc* pDoc,
		ARBDogPtr const& inDog,
		ARBConfigVenuePtr const& inVenue)
	: CPointsDataBase(pDoc)
	, m_pDog(inDog)
	, m_pVenue(inVenue)
{
}


std::wstring CPointsDataVenue::OnNeedText(int inCol) const
{
	std::wstring str;
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
					str = L"[" + pRegNum->GetNumber() + L"]";
				}
			}
			break;
		}
	}
	return str;
}


std::wstring CPointsDataVenue::GetHtml(
		size_t nCurLine,
		bool bNoInternalLinks) const
{
	fmt::wmemory_buffer data;
	if (m_pVenue)
	{
		fmt::format_to(data, L"<h2>");
		if (m_pVenue->GetURL().empty())
		{
			fmt::format_to(data, L"{}", Sanitize(m_pVenue->GetName()));
		}
		else
		{
			fmt::format_to(data, L"<a href=\"{}\">{}</a>",
				m_pVenue->GetURL(),
				Sanitize(m_pVenue->GetName()));
		}
		if (m_pDog)
		{
			ARBDogRegNumPtr pRegNum;
			if (m_pDog->GetRegNums().FindRegNum(m_pVenue->GetName(), &pRegNum))
			{
				fmt::format_to(data, L" [");
				if (!bNoInternalLinks)
				{
					fmt::format_to(data, L"<a href=\"{0}{1}\" name=\"ref{1}\">", ARB_PROTOCOL, nCurLine);
				}
				fmt::format_to(data, L"{}", Sanitize(pRegNum->GetNumber()));
				if (!bNoInternalLinks)
					fmt::format_to(data, L"</a>");
				fmt::format_to(data, L"]\n");
			}
		}
		fmt::format_to(data, L"</h2>\n");
	}
	return fmt::to_string(data);
}


void CPointsDataVenue::Details() const
{
	if (m_pDog && m_pVenue)
	{
		ARBDogRegNumPtr pRegNum;
		if (m_pDog->GetRegNums().FindRegNum(m_pVenue->GetName(), &pRegNum))
		{
			CDlgDog dlg(m_pDoc, m_pDog, nullptr, 2);
			dlg.ShowModal();
		}
		else
			wxBell();
	}
	else
		wxBell();
}


bool CPointsDataVenue::IsEqual(CPointsDataBasePtr const& inData)
{
	CPointsDataVenuePtr inVenue = std::dynamic_pointer_cast<CPointsDataVenue, CPointsDataBase>(inData);
	if (inVenue)
		return m_pVenue->GetName() == inVenue->m_pVenue->GetName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataTitle::CPointsDataTitle(
		CAgilityBookDoc* pDoc,
		ARBDogPtr const& inDog,
		ARBDogTitlePtr const& inTitle)
	: CPointsDataBase(pDoc)
	, m_pDog(inDog)
	, m_pTitle(inTitle)
{
}


std::wstring CPointsDataTitle::OnNeedText(int inCol) const
{
	std::wstring str;
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
				str += L"*";
			break;
		}
	}
	return str;
}


std::wstring CPointsDataTitle::GetHtml(
		size_t nCurLine,
		bool bNoInternalLinks) const
{
	fmt::wmemory_buffer data;
	if (m_pTitle)
	{
		fmt::format_to(data, L"<tr>\n<td>{}</td>\n<td>",
			Sanitize(OnNeedText(1), true));
		if (!bNoInternalLinks)
		{
			fmt::format_to(data, L"<a href=\"{0}{1}\" name=\"ref{1}\">", ARB_PROTOCOL, nCurLine);
		}
		fmt::format_to(data, L"{}", Sanitize(OnNeedText(2)));
		if (!bNoInternalLinks)
			fmt::format_to(data, L"</a>");
		fmt::format_to(data, L"</td>\n</tr>\n");
	}
	return fmt::to_string(data);
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
		m_pDoc->UpdateAllViews(nullptr, &hint);
	}
}


bool CPointsDataTitle::IsEqual(CPointsDataBasePtr const& inData)
{
	CPointsDataTitlePtr inTitle = std::dynamic_pointer_cast<CPointsDataTitle, CPointsDataBase>(inData);
	if (inTitle)
		return m_pTitle->GetVenue() == inTitle->m_pTitle->GetVenue()
			&& m_pTitle->GetRawName() == inTitle->m_pTitle->GetRawName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataEvent::CPointsDataEvent(
		CAgilityBookDoc* pDoc,
		ARBDogPtr const& inDog,
		std::list<RunInfo>& inMatching,
		ARBConfigVenuePtr const& inVenue,
		ARBConfigDivisionPtr const& inDiv,
		int inDivIdx,
		ARBConfigLevelPtr const& inLevel,
		int inLevelIdx,
		ARBConfigEventPtr const& inEvent,
		int inEventIdx,
		std::wstring const& inRunCount,
		std::wstring const& inQcount,
		std::wstring const& inPts,
		std::wstring const& inSuperQ,
		std::wstring const& inSpeed)
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


std::wstring CPointsDataEvent::OnNeedText(int inCol) const
{
	std::wstring str;
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


std::wstring CPointsDataEvent::GetHtml(
		size_t nCurLine,
		bool bNoInternalLinks) const
{
	fmt::wmemory_buffer data;
	fmt::format_to(data, L"<tr>\n<td>{}</td>\n", Sanitize(OnNeedText(1), true));
	fmt::format_to(data, L"<td>{}</td>\n", Sanitize(OnNeedText(2), true));
	fmt::format_to(data, L"<td>{}</td><td>\n", Sanitize(OnNeedText(3), true));
	if (!bNoInternalLinks)
	{
		fmt::format_to(data, L"<a href=\"{0}{1}\" name=\"ref{1}\">", ARB_PROTOCOL, nCurLine);
	}
	fmt::format_to(data, L"{}", Sanitize(OnNeedText(4)));
	if (!bNoInternalLinks)
		fmt::format_to(data, L"</a>");
	fmt::format_to(data, L"</td>\n<td>{}</td>\n", Sanitize(OnNeedText(5), true));
	fmt::format_to(data, L"<td align=\"right\">{}</td>\n", Sanitize(OnNeedText(6), true));
	fmt::format_to(data, L"<td>{}</td>\n", Sanitize(OnNeedText(7), true));
	fmt::format_to(data, L"<td>{}</td>\n</tr>\n", Sanitize(OnNeedText(8), true));
	return fmt::to_string(data);
}


void CPointsDataEvent::Details() const
{
	std::wstring str = fmt::format(L"{}: {}/{}/{}",
		_("IDS_RUNS").wx_str(),
		m_Div->GetName(),
		m_Level->GetName(),
		m_Event->GetName());
	RunInfoData data(m_Dog, m_Venue, m_Div, m_Level, m_Event);
	CDlgListViewer dlg(m_pDoc, str, m_Dog ? &data : nullptr, m_Matching);
	dlg.ShowModal();
}


bool CPointsDataEvent::IsEqual(CPointsDataBasePtr const& inData)
{
	CPointsDataEventPtr inEvent = std::dynamic_pointer_cast<CPointsDataEvent, CPointsDataBase>(inData);
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
		ARBConfigVenuePtr const& inVenue)
	: CPointsDataBase(pDoc)
	, m_LifetimeName()
	, m_Venue(inVenue)
	, m_Lifetime(0.0)
	, m_Filtered(0.0)
{
}


CPointsDataLifetime::CPointsDataLifetime(
		CAgilityBookDoc* pDoc,
		ARBConfigLifetimeNamePtr const& inLifetimeName,
		ARBConfigVenuePtr const& inVenue)
	: CPointsDataBase(pDoc)
	, m_LifetimeName(inLifetimeName)
	, m_Venue(inVenue)
	, m_Lifetime(0.0)
	, m_Filtered(0.0)
{
}


void CPointsDataLifetime::AddLifetimeInfo(
		std::wstring const& inDiv,
		std::wstring const& inLevel,
		double inLifetime,
		double inFiltered)
{
	m_Data.push_back(LifeTimePointInfo::New(inDiv, inLevel, inLifetime, inFiltered));
	m_Lifetime += inLifetime;
	m_Filtered += inFiltered;
}


std::wstring CPointsDataLifetime::OnNeedText(int inCol) const
{
	fmt::wmemory_buffer str;
	switch (inCol)
	{
	case 1:
		if (m_LifetimeName)
		{
			std::wstring lifetime = m_LifetimeName->GetName();
			if (lifetime.empty())
				lifetime = _("IDS_TITLEPOINT_LIFETIME_NAME");
			fmt::format_to(str, L"{}", fmt::sprintf(_("IDS_LIFETIME_POINTS").wx_str(), lifetime));
		}
		else
			fmt::format_to(str, L"{}", StringUtil::stringW(_("IDS_PLACEMENT_POINTS")));
		break;
	case 2:
		fmt::format_to(str, L"{}: ", StringUtil::stringW(_("IDS_TOTAL")));
		if (0 < m_Filtered)
			fmt::format_to(str, L"{} ({})", m_Lifetime - m_Filtered, m_Lifetime);
		else
			fmt::format_to(str, L"{}", m_Lifetime);
		break;
	}
	return fmt::to_string(str);
}


std::wstring CPointsDataLifetime::GetHtml(
		size_t nCurLine,
		bool bNoInternalLinks) const
{
	fmt::wmemory_buffer data;
	fmt::format_to(data, L"<tr><td>&nbsp;</td></tr>\n<tr>\n<td>{}</td>\n<td align=\"right\">", Sanitize(OnNeedText(1), true));
	if (!bNoInternalLinks)
	{
		fmt::format_to(data, L"<a href=\"{0}{1}\" name=\"ref{1}\">", ARB_PROTOCOL, nCurLine);
	}
	fmt::format_to(data, L"{}", Sanitize(OnNeedText(2)));
	if (!bNoInternalLinks)
		fmt::format_to(data, L"</a>");
	fmt::format_to(data, L"</td>\n</tr>\n");
	return fmt::to_string(data);
}


void CPointsDataLifetime::Details() const
{
	std::wstring caption(m_Venue->GetName());
	std::wstring str;
	if (m_LifetimeName)
	{
		std::wstring lifetime = m_LifetimeName->GetName();
		if (lifetime.empty())
			lifetime = _("IDS_TITLEPOINT_LIFETIME_NAME");
		str = StringUtil::stringW(wxString::Format(_("IDS_LIFETIME_POINTS"), lifetime.c_str()));
	}
	else
		str = StringUtil::stringW(_("IDS_PLACEMENT_POINTS"));
	caption += L" " + str;
	CDlgListViewer dlg(m_pDoc, caption, m_Data);
	dlg.ShowModal();
}


bool CPointsDataLifetime::IsEqual(CPointsDataBasePtr const& inData)
{
	CPointsDataLifetimePtr inLife = std::dynamic_pointer_cast<CPointsDataLifetime, CPointsDataBase>(inData);
	if (inLife)
		return m_Venue == inLife->m_Venue;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataLifetimeByName::CPointsDataLifetimeByName(
		CAgilityBookDoc* pDoc,
		ARBConfigVenuePtr const& inVenue,
		std::wstring const& inName)
	: CPointsDataLifetime(pDoc, inVenue)
	, m_Name(inName)
{
}

CPointsDataLifetimeByName::CPointsDataLifetimeByName(
		CAgilityBookDoc* pDoc,
		ARBConfigLifetimeNamePtr const& inLifetimeName,
		ARBConfigVenuePtr const& inVenue,
		std::wstring const& inName)
	: CPointsDataLifetime(pDoc, inLifetimeName, inVenue)
	, m_Name(inName)
{
}


void CPointsDataLifetimeByName::AddLifetimeInfo(
		std::wstring const& inSort1,
		std::wstring const& inSort2,
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


std::wstring CPointsDataLifetimeByName::OnNeedText(int inCol) const
{
	fmt::wmemory_buffer str;
	switch (inCol)
	{
	case 2:
		fmt::format_to(str, L"{}: ", m_Name);
		if (0 < m_Filtered)
			fmt::format_to(str, L"{} ({})", m_Lifetime - m_Filtered, m_Lifetime);
		else
			fmt::format_to(str, L"{}", m_Lifetime);
		break;
	}
	return fmt::to_string(str);
}


std::wstring CPointsDataLifetimeByName::GetHtml(
		size_t nCurLine,
		bool bNoInternalLinks) const
{
	return fmt::format(L"<tr>\n<td>&nbsp;</td>\n<td align=\"right\">{}</td>\n</tr>\n", Sanitize(OnNeedText(2), true));
}


bool CPointsDataLifetimeByName::IsEqual(CPointsDataBasePtr const& inData)
{
	CPointsDataLifetimeByNamePtr inLife = std::dynamic_pointer_cast<CPointsDataLifetimeByName, CPointsDataBase>(inData);
	if (inLife)
		return CPointsDataLifetimeByName::IsEqual(inData)
			&& m_Name == inLife->m_Name;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataMultiQs::CPointsDataMultiQs(
		CAgilityBookDoc* pDoc,
		ARBDogPtr const& inDog,
		ARBConfigVenuePtr const& inVenue,
		ARBConfigMultiQPtr const& inMultiQ,
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


std::wstring CPointsDataMultiQs::OnNeedText(int inCol) const
{
	std::wstring str;
	switch (inCol)
	{
	case 1:
		str = m_MultiQ->GetName();
		break;
	case 7:
		str = fmt::format(L"{} {}", m_ExistingDblQs + m_MQs.size(), m_MultiQ->GetShortName());
		break;
	}
	return str;
}


std::wstring CPointsDataMultiQs::GetHtml(
		size_t nCurLine,
		bool bNoInternalLinks) const
{
	fmt::wmemory_buffer data;
	fmt::format_to(data, L"<tr>\n<td colspan=\"2\">{}</td>\n<td colspan=\"4\"/>\n<td>", Sanitize(OnNeedText(1), true));
	if (!bNoInternalLinks)
	{
		fmt::format_to(data, L"<a href=\"{0}{1}\" name=\"ref{1}\">", ARB_PROTOCOL, nCurLine);
	}
	fmt::format_to(data, L"{}", Sanitize(OnNeedText(7)));
	if (!bNoInternalLinks)
		fmt::format_to(data, L"</a>");
	fmt::format_to(data, L"</td>\n</tr>\n");
	return fmt::to_string(data);
}


void CPointsDataMultiQs::Details() const
{
	MultiQInfoData data(m_Dog, m_Venue, m_MultiQ);
	CDlgListViewer dlg(m_pDoc, m_MultiQ->GetName(), &data, m_MQs);
	dlg.ShowModal();
}


bool CPointsDataMultiQs::IsEqual(CPointsDataBasePtr const& inData)
{
	CPointsDataMultiQsPtr inMulti = std::dynamic_pointer_cast<CPointsDataMultiQs, CPointsDataBase>(inData);
	if (inMulti)
		return m_Venue->GetName() == inMulti->m_Venue->GetName()
			&& m_MultiQ->GetName() == inMulti->m_MultiQ->GetName();
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataSpeedPts::CPointsDataSpeedPts(
		CAgilityBookDoc* pDoc,
		ARBConfigVenuePtr const& inVenue,
		ARBConfigDivisionPtr const& inDiv,
		int inPts)
	: CPointsDataBase(pDoc)
	, m_Venue(inVenue)
	, m_Div(inDiv)
	, m_Pts(inPts)
{
}


std::wstring CPointsDataSpeedPts::OnNeedText(int inCol) const
{
	std::wstring str;
	switch (inCol)
	{
	case 1:
		str = m_Div->GetName();
		break;
	case 7:
		str = StringUtil::stringW(wxString::Format(_("IDS_POINTS_SPEED"), m_Pts));
		break;
	}
	return str;
}


std::wstring CPointsDataSpeedPts::GetHtml(
		size_t nCurLine,
		bool bNoInternalLinks) const
{
	return fmt::format(L"<tr>\n<td colspan=\"2\">{}</td>\n<td colspan=\"4\"/>\n<td>{}</td>\n</tr>\n",
		Sanitize(OnNeedText(1), true),
		Sanitize(OnNeedText(7), true));
}


bool CPointsDataSpeedPts::IsEqual(CPointsDataBasePtr const& inData)
{
	CPointsDataSpeedPtsPtr inPts = std::dynamic_pointer_cast<CPointsDataSpeedPts, CPointsDataBase>(inData);
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
	, m_Points(0.0)
	, m_RunList(inRunList)
{
	for (std::list<OtherPtInfo>::iterator iter = m_RunList.begin();
		iter != m_RunList.end();
		++iter)
	{
		m_Points += (*iter).m_Points;
	}
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPointsTallyAll::CPointsDataOtherPointsTallyAll(
		CAgilityBookDoc* pDoc,
		std::wstring const& inName,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataOtherPoints(pDoc, inRunList)
	, m_Name(inName)
{
}


std::wstring CPointsDataOtherPointsTallyAll::OnNeedText(int inCol) const
{
	std::wstring str;
	switch (inCol)
	{
	case 1:
		str = m_Name;
		break;
	case 2:
		str = fmt::format(L"{}", m_Points);
		break;
	}
	return str;
}


std::wstring CPointsDataOtherPointsTallyAll::GetHtml(
		size_t nCurLine,
		bool bNoInternalLinks) const
{
	fmt::wmemory_buffer data;
	fmt::format_to(data, L"<tr>\n<td>{}</td>\n<td align=\"right\">", Sanitize(OnNeedText(1), true));
	if (!bNoInternalLinks)
	{
		fmt::format_to(data, L"<a href=\"{0}{1}\" name=\"ref{1}\">", ARB_PROTOCOL, nCurLine);
	}
	fmt::format_to(data, L"{}", Sanitize(OnNeedText(2)));
	if (!bNoInternalLinks)
		fmt::format_to(data, L"</a>");
	fmt::format_to(data, L"</td>\n</tr>\n");
	return fmt::to_string(data);
}


void CPointsDataOtherPointsTallyAll::Details() const
{
	CDlgListViewer dlg(m_pDoc, StringUtil::stringW(_("IDS_OTHERPOINTS")), m_RunList);
	dlg.ShowModal();
}


bool CPointsDataOtherPointsTallyAll::IsEqual(CPointsDataBasePtr const& inData)
{
	CPointsDataOtherPointsTallyAllPtr inPts = std::dynamic_pointer_cast<CPointsDataOtherPointsTallyAll, CPointsDataBase>(inData);
	if (inPts)
		return m_Name == inPts->m_Name;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPointsTallyAllByEvent::CPointsDataOtherPointsTallyAllByEvent(
		CAgilityBookDoc* pDoc,
		std::wstring const& inEvent,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataOtherPoints(pDoc, inRunList)
	, m_Event(inEvent)
{
}


std::wstring CPointsDataOtherPointsTallyAllByEvent::OnNeedText(int inCol) const
{
	std::wstring str;
	switch (inCol)
	{
	case 2: // Event
		str = m_RunList.begin()->m_Event;
		break;
	case 3:
		str = fmt::format(L"{}", m_Points);
		break;
	}
	return str;
}


std::wstring CPointsDataOtherPointsTallyAllByEvent::GetHtml(
		size_t nCurLine,
		bool bNoInternalLinks) const
{
	fmt::wmemory_buffer data;
	fmt::format_to(data, L"<tr>\n<td>&nbsp;</td>\n<td>{}</td>\n<td align=\"right\">", Sanitize(OnNeedText(2), true));
	if (!bNoInternalLinks)
	{
		fmt::format_to(data, L"<a href=\"{0}{1}\" name=\"ref{1}\">", ARB_PROTOCOL, nCurLine);
	}
	fmt::format_to(data, L"{}", Sanitize(OnNeedText(3)));
	if (!bNoInternalLinks)
		fmt::format_to(data, L"</a>");
	fmt::format_to(data, L"</td>\n</tr>\n");
	return fmt::to_string(data);
}


void CPointsDataOtherPointsTallyAllByEvent::Details() const
{
	CDlgListViewer dlg(m_pDoc, StringUtil::stringW(_("IDS_OTHERPOINTS")), m_RunList);
	dlg.ShowModal();
}


bool CPointsDataOtherPointsTallyAllByEvent::IsEqual(CPointsDataBasePtr const& inData)
{
	CPointsDataOtherPointsTallyAllByEventPtr inPts = std::dynamic_pointer_cast<CPointsDataOtherPointsTallyAllByEvent, CPointsDataBase>(inData);
	if (inPts)
		return m_Event == inPts->m_Event;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPointsTallyLevel::CPointsDataOtherPointsTallyLevel(
		CAgilityBookDoc* pDoc,
		std::wstring const& inLevel,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataOtherPoints(pDoc, inRunList)
	, m_Level(inLevel)
{
}


std::wstring CPointsDataOtherPointsTallyLevel::OnNeedText(int inCol) const
{
	std::wstring str;
	switch (inCol)
	{
	case 2: // Level
		str = m_RunList.begin()->m_Level;
		break;
	case 3:
		str = fmt::format(L"{}", m_Points);
		break;
	}
	return str;
}


std::wstring CPointsDataOtherPointsTallyLevel::GetHtml(
		size_t nCurLine,
		bool bNoInternalLinks) const
{
	fmt::wmemory_buffer data;
	fmt::format_to(data, L"<tr>\n<td>&nbsp;</td>\n<td>{}</td>\n<td align=\"right\">", Sanitize(OnNeedText(2), true));
	if (!bNoInternalLinks)
	{
		fmt::format_to(data, L"<a href=\"{0}{1}\" name=\"ref{1}\">", ARB_PROTOCOL, nCurLine);
	}
	fmt::format_to(data, L"{}", Sanitize(OnNeedText(3)));
	if (!bNoInternalLinks)
		fmt::format_to(data, L"</a>");
	fmt::format_to(data, L"</td>\n</tr>\n");
	return fmt::to_string(data);
}


void CPointsDataOtherPointsTallyLevel::Details() const
{
	CDlgListViewer dlg(m_pDoc, StringUtil::stringW(_("IDS_OTHERPOINTS")), m_RunList);
	dlg.ShowModal();
}


bool CPointsDataOtherPointsTallyLevel::IsEqual(CPointsDataBasePtr const& inData)
{
	CPointsDataOtherPointsTallyLevelPtr inPts = std::dynamic_pointer_cast<CPointsDataOtherPointsTallyLevel, CPointsDataBase>(inData);
	if (inPts)
		return m_Level == inPts->m_Level;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPointsTallyLevelByEvent::CPointsDataOtherPointsTallyLevelByEvent(
		CAgilityBookDoc* pDoc,
		std::wstring const& inLevel,
		std::wstring const& inEvent,
		std::list<OtherPtInfo> const& inRunList)
	: CPointsDataOtherPoints(pDoc, inRunList)
	, m_Level(inLevel)
	, m_Event(inEvent)
{
}


std::wstring CPointsDataOtherPointsTallyLevelByEvent::OnNeedText(int inCol) const
{
	std::wstring str;
	switch (inCol)
	{
	case 2: // Level
		str = m_RunList.begin()->m_Level;
		break;
	case 3: // Event
		str = m_RunList.begin()->m_Event;
		break;
	case 4:
		str = fmt::format(L"{}", m_Points);
		break;
	}
	return str;
}


std::wstring CPointsDataOtherPointsTallyLevelByEvent::GetHtml(
		size_t nCurLine,
		bool bNoInternalLinks) const
{
	fmt::wmemory_buffer data;
	fmt::format_to(data, L"<tr>\n<td>&nbsp;</td>\n<td>{}</td>\n<td></td>\n<td align=\"right\">",
		Sanitize(OnNeedText(2), true),
		Sanitize(OnNeedText(3), true));
	if (!bNoInternalLinks)
	{
		fmt::format_to(data, L"<a href=\"{0}{1}\" name=\"ref{1}\">", ARB_PROTOCOL, nCurLine);
	}
	fmt::format_to(data, L"{}", Sanitize(OnNeedText(4)));
	if (!bNoInternalLinks)
		fmt::format_to(data, L"</a>");
	fmt::format_to(data, L"</td>\n</tr>\n");
	return fmt::to_string(data);
}


void CPointsDataOtherPointsTallyLevelByEvent::Details() const
{
	CDlgListViewer dlg(m_pDoc, StringUtil::stringW(_("IDS_OTHERPOINTS")), m_RunList);
	dlg.ShowModal();
}


bool CPointsDataOtherPointsTallyLevelByEvent::IsEqual(CPointsDataBasePtr const& inData)
{
	CPointsDataOtherPointsTallyLevelByEventPtr inPts = std::dynamic_pointer_cast<CPointsDataOtherPointsTallyLevelByEvent, CPointsDataBase>(inData);
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
	bool operator()(CPointsDataEvent const* const one, CPointsDataEvent const* const two) const
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
		ARBDogPtr const& inDog,
		ARBConfigVenuePtr const& inVenue)
{
	m_Lines.push_back(CPointsDataText::CreateListBlankLine(pDoc));
	m_Lines.push_back(std::make_shared<CPointsDataVenue>(pDoc, inDog, inVenue));
}


void CPointsDataItems::LoadData(
		CAgilityBookDoc* pDoc,
		ARBDogPtr const& inDog)
{
	clear();

	if (!inDog)
		return;

	// Find all visible items and sort them out by venue.
	std::vector<CVenueFilter> venues;
	CFilterOptions::Options().GetFilterVenue(venues);

	// Put header in...
	m_Lines.push_back(std::make_shared<CPointsDataHeader>(pDoc));

	// Put general info about the dog in...
	m_Lines.push_back(std::make_shared<CPointsDataDog>(pDoc, inDog));

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
					std::wstring data(L"<h3>");
					data += _("IDS_TITLES");
					data += L"</h3>";
					data += s_TableHeader;
					m_Lines.push_back(std::make_shared<CPointsDataSeparator>(pDoc, data));
				}
				m_Lines.push_back(std::make_shared<CPointsDataTitle>(pDoc, inDog, pTitle));
			}
		}
		if (bHeaderInserted)
			m_Lines.push_back(std::make_shared<CPointsDataSeparator>(pDoc, L"</table>"));

		bool bRunsInserted = false;
		LifeTimePointsList lifetime;
		PlacementPointsList placement;

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
			std::wstring data(L"<h3>");
			data += _("IDS_RUNS");
			data += L"</h3>";
			data += s_TableHeader;
			m_Lines.push_back(std::make_shared<CPointsDataSeparator>(pDoc, data));
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
					LifeTimePoints ptsLifetime;
					PlacementPoints ptsPlacement;
					ptsLifetime.pDiv = ptsPlacement.pDiv = pDiv;
					ptsLifetime.pLevel = ptsPlacement.pLevel = pLevel;
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
						std::set<std::wstring> judges;
						std::set<std::wstring> judgesQ;
						std::set<std::wstring> partners;
						std::set<std::wstring> partnersQ;
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
										matching.push_back(RunInfo(inDog, pTrial, pRun));
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
												std::wstring p = (*iterPartner)->GetDog();
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
										for (ARBConfigLifetimeNameList::iterator iterN = pVenue->GetLifetimeNames().begin();
											iterN != pVenue->GetLifetimeNames().end();
											++iterN)
										{
											double nLifetime = pRun->GetLifetimePoints(pScoringMethod, (*iterN)->GetName());
											if (0 < nLifetime)
											{
												ptsLifetime.ptLifetime[(*iterN)].push_back(LifeTimePoint(pRun->GetEvent(), nLifetime, !bRunVisible));
											}
										}
										double nPlacement = pRun->GetPlacementPoints(pScoringMethod);
										if (0 < nPlacement)
										{
											ptsPlacement.ptPlacement.push_back(LifeTimePoint(pRun->GetEvent(), nPlacement, !bRunVisible));
										}
									}
								}
							}
							// Accumulate existing points
							if (bHasExistingPoints || bHasExistingLifetimePoints || 0 < matching.size())
							{
								nExistingPts = inDog->GetExistingPoints().ExistingPoints(
									ARBDogExistingPoints::eTitle,
									pVenue, ARBConfigMultiQPtr(), pDiv, pLevel, pEvent, dateFrom2, dateTo2);
								if (pScoringMethod->HasSuperQ())
									nExistingSQ += static_cast<int>(inDog->GetExistingPoints().ExistingPoints(
										ARBDogExistingPoints::eSQ,
										pVenue, ARBConfigMultiQPtr(), pDiv, pLevel, pEvent, dateFrom2, dateTo2));
								// Now add the existing lifetime points
								for (ARBConfigLifetimeNameList::const_iterator iterLifetime = pVenue->GetLifetimeNames().begin();
									iterLifetime != pVenue->GetLifetimeNames().end();
									++iterLifetime)
								{
									double nExistingLifetimePts = inDog->GetExistingPoints().ExistingLifetimePoints(
										*iterLifetime, pVenue, pDiv, pLevel, pEvent, dateFrom2, dateTo2);
									if (0.0 < nExistingLifetimePts)
										ptsLifetime.ptLifetime[(*iterLifetime)].push_back(LifeTimePoint(pEvent->GetName(), nExistingLifetimePts, false));
								}
							}
							if (bHasExistingPoints || 0 < matching.size())
							{
								for (std::list<RunInfo>::const_iterator iterRun = matching.begin();
									iterRun != matching.end();
									++iterRun)
								{
									ARBDogRunPtr pRun = get<TUPLE_RUN>(*iterRun);
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
								static_cast<int>(allmatching.size()),
								static_cast<int>(judges.size()));
							if (pEvent->HasPartner() && 0 < partners.size())
							{
								strRunCount += wxString::Format(_("IDS_POINTS_PARTNERS"),
									static_cast<int>(partners.size()));
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
								strQcount += wxString::Format(_("IDS_POINTS_JUDGES"),
									static_cast<int>(judgesQ.size()));
							}
							if (pEvent->HasPartner() && 0 < partnersQ.size())
							{
								strQcount += wxString::Format(_("IDS_POINTS_PARTNERS"),
									static_cast<int>(partnersQ.size()));
							}
							std::wstring strPts;
							std::wstring strSuperQ;
							strPts = fmt::format(L"{}{}", points, nExistingSQ);
							if (hasSQs)
							{
								SQs += nExistingSQ;
								strSuperQ = wxString::Format(_("IDS_POINTS_SQS"), SQs);
							}
							std::wstring strSpeed;
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
								StringUtil::stringW(strRunCount),
								StringUtil::stringW(strQcount),
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
					if (0 < ptsLifetime.ptLifetime.size())
						lifetime.push_back(ptsLifetime);
					if (0 < ptsPlacement.ptPlacement.size())
						placement.push_back(ptsPlacement);
				} // level loop
				if (bHasSpeedPts)
				{
					speedPtsData.push_back(std::make_shared<CPointsDataSpeedPts>(pDoc, pVenue, pDiv, speedPts));
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
				// List multiQs in configuration order.
				for (auto iMulti = pVenue->GetMultiQs().begin(); iMulti != pVenue->GetMultiQs().end(); ++iMulti)
				{
					auto iterMQ = MQs.find((*iMulti));
					if (iterMQ != MQs.end())
					{
						m_Lines.push_back(std::make_shared<CPointsDataMultiQs>(pDoc, inDog, pVenue, (*iterMQ).first, (*iterMQ).second));
					}
				}
			}
		}

		// Next comes lifetime points.
		if (0 < lifetime.size())
		{
			for (ARBConfigLifetimeNameList::iterator iterL = pVenue->GetLifetimeNames().begin();
				iterL != pVenue->GetLifetimeNames().end();
				++iterL)
			{
				CPointsDataLifetime* pData = new CPointsDataLifetime(pDoc, (*iterL), pVenue);
				typedef std::map<std::wstring, CPointsDataLifetimeByName*> NamedLifetime;
				NamedLifetime subgroups;
				if (CAgilityBookOptions::GetViewLifetimePointsByEvent())
				{
					// Gather event names
					std::set<std::wstring> names;
					for (LifeTimePointsList::iterator iter = lifetime.begin();
					iter != lifetime.end();
						++iter)
					{
						std::map<ARBConfigLifetimeNamePtr, LifeTimePointList>::iterator iterName = (*iter).ptLifetime.find((*iterL));
						if ((*iter).ptLifetime.end() != iterName)
						{
							for (LifeTimePointList::iterator iter2 = iterName->second.begin();
								iter2 != iterName->second.end();
								++iter2)
							{
								names.insert(iter2->eventName);
							}
						}
					}
					for (std::set<std::wstring>::iterator iName = names.begin(); iName != names.end(); ++iName)
					{
						double pts2 = 0.0;
						double ptFiltered2 = 0;
						for (LifeTimePointsList::iterator iter = lifetime.begin();
						iter != lifetime.end();
							++iter)
						{
							std::map<ARBConfigLifetimeNamePtr, LifeTimePointList>::iterator iterName = (*iter).ptLifetime.find((*iterL));
							if ((*iter).ptLifetime.end() != iterName)
							{
								for (LifeTimePointList::iterator iter2 = iterName->second.begin();
								iter2 != iterName->second.end();
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
						}
						pData->AddLifetimeInfo(*iName, std::wstring(), pts2, ptFiltered2);
					}
				}
				for (LifeTimePointsList::iterator iter = lifetime.begin();
				iter != lifetime.end();
					++iter)
				{
					CPointsDataLifetimeByName* pNameData = nullptr;
					NamedLifetime::iterator it = subgroups.find(iter->pDiv->GetName());
					if (subgroups.end() != it)
					{
						pNameData = it->second;
					}
					else
					{
						pNameData = new CPointsDataLifetimeByName(pDoc, (*iterL), pVenue, iter->pDiv->GetName());
						subgroups.insert(NamedLifetime::value_type(iter->pDiv->GetName(), pNameData));
					}

					double pts2 = 0.0;
					double ptFiltered2 = 0;
					std::map<ARBConfigLifetimeNamePtr, LifeTimePointList>::iterator iterName = (*iter).ptLifetime.find((*iterL));
					if ((*iter).ptLifetime.end() != iterName)
					{
						for (LifeTimePointList::iterator iter2 = iterName->second.begin();
						iter2 != iterName->second.end();
							++iter2)
						{
							pts2 += (*iter2).points;
							if ((*iter2).bFiltered)
								ptFiltered2 += (*iter2).points;
						}
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
		}
		if (0 < placement.size())
		{
			CPointsDataLifetime* pData = new CPointsDataLifetime(pDoc, pVenue);
			typedef std::map<std::wstring, CPointsDataLifetimeByName*> NamedLifetime;
			NamedLifetime subgroups;
			if (CAgilityBookOptions::GetViewLifetimePointsByEvent())
			{
				// Gather event names
				std::set<std::wstring> names;
				for (PlacementPointsList::iterator iter = placement.begin();
					iter != placement.end();
					++iter)
				{
					for (LifeTimePointList::iterator iter2 = (*iter).ptPlacement.begin();
						iter2 != (*iter).ptPlacement.end();
						++iter2)
					{
						names.insert(iter2->eventName);
					}
				}
				for (std::set<std::wstring>::iterator iName = names.begin(); iName != names.end(); ++iName)
				{
					double pts2 = 0.0;
					double ptFiltered2 = 0;
					for (PlacementPointsList::iterator iter = placement.begin();
						iter != placement.end();
						++iter)
					{
						for (LifeTimePointList::iterator iter2 = (*iter).ptPlacement.begin();
							iter2 != (*iter).ptPlacement.end();
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
					pData->AddLifetimeInfo(*iName, std::wstring(), pts2, ptFiltered2);
				}
			}
			for (PlacementPointsList::iterator iter = placement.begin();
				iter != placement.end();
				++iter)
			{
				CPointsDataLifetimeByName* pNameData = nullptr;
				NamedLifetime::iterator it = subgroups.find(iter->pDiv->GetName());
				if (subgroups.end() != it)
				{
					pNameData = it->second;
				}
				else
				{
					pNameData = new CPointsDataLifetimeByName(pDoc, pVenue, iter->pDiv->GetName());
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
			m_Lines.push_back(std::make_shared<CPointsDataSeparator>(pDoc, L"</table>"));
	}

	// After all the venues, we do 'other points'.
	ARBConfigOtherPointsList const& other = pDoc->Book().GetConfig().GetOtherPoints();
	if (0 < other.size())
	{
		std::wstring str(_("IDS_OTHERPOINTS"));
		std::wstring table;
		table = L"<h2>";
		table += str;
		table += L"</h2>";
		table += s_TableHeader;

		m_Lines.push_back(std::make_shared<CPointsDataSeparator>(pDoc, table));
		m_Lines.push_back(CPointsDataText::CreateListBlankLine(pDoc));
		m_Lines.push_back(CPointsDataText::CreateListOtherPoints(pDoc));
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
									bool bScore = false;
									double score = 0.0;
									ARBConfigScoringPtr pScoring;
									if (pTrial->GetClubs().GetPrimaryClub())
									{
										pDoc->Book().GetConfig().GetVenues().FindEvent(
											pTrial->GetClubs().GetPrimaryClubVenue(),
											pRun->GetEvent(),
											pRun->GetDivision(),
											pRun->GetLevel(),
											pRun->GetDate(),
											nullptr,
											&pScoring);
									}
									if (pScoring)
									{
										bScore = true;
										score = pRun->GetScore(pScoring);
									}
									runs.push_back(OtherPtInfo(pTrial, pRun, pOtherPts->GetPoints(), bScore, score));
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
				&& (*iterExisting)->GetTypeName() == pOther->GetName())
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
				m_Lines.push_back(std::make_shared<CPointsDataOtherPointsTallyAll>(pDoc, pOther->GetName(), runs));
				break;

			case ARBConfigOtherPoints::eTallyAllByEvent:
				m_Lines.push_back(CPointsDataText::CreateDataLine(pDoc, pOther->GetName()));
				{
					std::set<std::wstring> tally;
					std::list<OtherPtInfo>::iterator iter;
					for (iter = runs.begin(); iter != runs.end(); ++iter)
					{
						tally.insert((*iter).m_Event);
					}
					for (std::set<std::wstring>::iterator iterTally = tally.begin();
						iterTally != tally.end();
						++iterTally)
					{
						std::list<OtherPtInfo> validRuns;
						for (iter = runs.begin(); iter != runs.end(); ++iter)
						{
							if ((*iter).m_Event == (*iterTally))
								validRuns.push_back(*iter);
						}
						m_Lines.push_back(std::make_shared<CPointsDataOtherPointsTallyAllByEvent>(pDoc, (*iterTally), validRuns));
					}
				}
				break;

			case ARBConfigOtherPoints::eTallyLevel:
				m_Lines.push_back(CPointsDataText::CreateDataLine(pDoc, pOther->GetName()));
				{
					std::set<std::wstring> tally;
					std::list<OtherPtInfo>::iterator iter;
					for (iter = runs.begin(); iter != runs.end(); ++iter)
					{
						tally.insert((*iter).m_Level);
					}
					for (std::set<std::wstring>::iterator iterTally = tally.begin();
						iterTally != tally.end();
						++iterTally)
					{
						std::list<OtherPtInfo> validRuns;
						for (iter = runs.begin(); iter != runs.end(); ++iter)
						{
							if ((*iter).m_Level == (*iterTally))
								validRuns.push_back(*iter);
						}
						m_Lines.push_back(std::make_shared<CPointsDataOtherPointsTallyLevel>(pDoc, (*iterTally), validRuns));
					}
				}
				break;

			case ARBConfigOtherPoints::eTallyLevelByEvent:
				m_Lines.push_back(CPointsDataText::CreateDataLine(pDoc, pOther->GetName()));
				{
					typedef std::pair<std::wstring, std::wstring> LevelEvent;
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
						m_Lines.push_back(std::make_shared<CPointsDataOtherPointsTallyLevelByEvent>(pDoc, (*iterTally).first, (*iterTally).second, validRuns));
					}
				}
				break;
			}
		}
		m_Lines.push_back(std::make_shared<CPointsDataSeparator>(pDoc, L"</table></html>"));
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
