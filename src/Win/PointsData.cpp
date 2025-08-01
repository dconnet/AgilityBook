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
 * 2020-09-30 Fix clicking on FCAT title link.
 * 2020-05-12 Fix formatting of doubles.
 * 2019-05-04 Reworked PointsData usage.
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
#include "DlgDog.h"
#include "DlgListViewer.h"
#include "DlgTitle.h"
#include "FilterOptions.h"

#include "ARB/ARBDog.h"
#include "ARBCommon/ARBDate.h"
#include "ARBCommon/ARBMisc.h"
#include "ARBCommon/StringUtil.h"
#include <wx/utils.h>
#include <algorithm>
#include <map>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

namespace
{
constexpr wchar_t s_TableHeader[] = L"<table border=\"2\" cellspacing=\"0\" cellpadding=\"2\">";
constexpr wchar_t s_refDog[] = L"refdog";
// Ref Formats
constexpr wchar_t s_refEvent[] = L"refevent%zu";
constexpr wchar_t s_refLifetime[] = L"reflifetime%zu";
constexpr wchar_t s_refMultiQ[] = L"refmultiq%zu";
constexpr wchar_t s_refOther[] = L"refother%zu";
constexpr wchar_t s_refSpeedPts[] = L"refspeedpts%zu";
constexpr wchar_t s_refTitle[] = L"reftitle%s";
constexpr wchar_t s_refVenue[] = L"refvenue%s";

wxString Sanitize(wxString const& inRawData, bool nbsp = false)
{
	wxString data = SanitizeStringForHTML(inRawData);
	if (nbsp && data.empty())
		data = L"&nbsp;";
	return data;
}
} // namespace

/////////////////////////////////////////////////////////////////////////////

OtherPtInfo::OtherPtInfo(
	ARB::ARBDogTrialPtr const& inTrial,
	ARB::ARBDogRunPtr const& inRun,
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
	if (m_pRun)
	{
		if (m_pRun->GetClub())
			m_Venue = m_pRun->GetClub()->GetVenue();
		m_Div = m_pRun->GetDivision();
		m_Level = m_pRun->GetLevel();
		m_Event = m_pRun->GetEvent();
	}
}


OtherPtInfo::OtherPtInfo(ARB::ARBDogExistingPointsPtr const& inExisting)
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

/////////////////////////////////////////////////////////////////////////////

CPointsDataTitle::CPointsDataTitle(CAgilityBookDoc* pDoc, ARBDogTitlePtr pTitle)
	: m_refTag(wxString::Format(s_refTitle, pTitle->GetGenericName()))
	, m_pDoc(pDoc)
	, m_pTitle(pTitle)
{
}


void CPointsDataTitle::GetHtml(wxString& data, bool bNoInternalLinks)
{
	data << L"<tr>\n<td>" << Sanitize(m_pTitle->GetDate().GetString(), true) << L"</td>\n<td>";
	if (!bNoInternalLinks)
	{
		data << L"<a href=\"" << ARB_PROTOCOL << m_refTag << "\" name=\"" << m_refTag << L"\">";
	}

	wxString str = m_pDoc->Book().GetConfig().GetTitleCompleteName(m_pTitle, false);
	if (m_pTitle->GetReceived())
		str += L"*";

	data << Sanitize(str);
	if (!bNoInternalLinks)
		data << L"</a>";
	data << L"</td>\n</tr>\n";
}


bool CPointsDataTitle::Details(wxString const& link, ARBDogPtr pDog)
{
	if (link != m_refTag)
		return false;

	CDlgTitle dlg(m_pDoc->Book().GetConfig(), pDog->GetTitles(), m_pTitle);
	if (wxID_OK == dlg.ShowModal())
	{
		m_pDoc->Modify(true);
		std::vector<CVenueFilter> venues;
		CFilterOptions::Options().GetFilterVenue(venues);
		m_pDoc->ResetVisibility(venues, m_pTitle);
		CUpdateHint hint(UPDATE_POINTS_VIEW);
		m_pDoc->UpdateAllViews(nullptr, &hint);
	}
	return true;
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
	wxString const& inRunCount,
	wxString const& inQcount,
	wxString const& inPts,
	wxString const& inSuperQ,
	wxString const& inSpeed,
	CRefTag& id)
	: m_refTag(wxString::Format(s_refEvent, id.GetId()))
	, m_pDoc(pDoc)
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


void CPointsDataEvent::GetHtml(wxString& data, bool bNoInternalLinks)
{
	data << L"<tr>\n<td>" << Sanitize(m_Div->GetName(), true) << L"</td>\n";
	data << L"<td>" << Sanitize(m_Level->GetName(), true) << L"</td>\n";
	data << L"<td>" << Sanitize(m_Event->GetName(), true) << L"</td>\n<td>";
	if (!bNoInternalLinks)
	{
		data << L"<a href=\"" << ARB_PROTOCOL << m_refTag << L"\" name=\"" << m_refTag << L"\">";
	}
	data << Sanitize(m_RunCount, true);
	if (!bNoInternalLinks)
		data << L"</a>";
	data << L"</td>\n<td>" << Sanitize(m_Qcount, true) << L"</td>\n";
	data << L"<td align=\"right\">" << Sanitize(m_Pts, true) << L"</td>\n";
	{ // SuperQ/SpeedPts
		wxString str;
		if (0 < m_SuperQ.length())
			str = m_SuperQ;
		else if (0 < m_Speed.length())
			str = m_Speed;
		data << L"<td>" << Sanitize(str, true) << L"</td>\n";
	}
	{ // Only used for SQ AND Speed
		wxString str;
		if (0 < m_SuperQ.length() && 0 < m_Speed.length())
			str = m_Speed;
		data << L"<td>" << Sanitize(str, true) << L"</td>\n</tr>\n";
	}
}


bool CPointsDataEvent::Details(wxString const& link)
{
	if (link != m_refTag)
		return false;

	wxString str;
	str << _("IDS_RUNS") << L": " << m_Div->GetName() << L"/" << m_Level->GetName() << L"/" << m_Event->GetName();
	RunInfoData data(m_Dog, m_Venue, m_Div, m_Level, m_Event);
	CDlgListViewer dlg(m_pDoc, str, m_Dog ? &data : nullptr, m_Matching);
	dlg.ShowModal();
	return true;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataSpeedPts::CPointsDataSpeedPts(
	CAgilityBookDoc* pDoc,
	ARBConfigVenuePtr const& inVenue,
	ARBConfigDivisionPtr const& inDiv,
	int inPts,
	CRefTag& id)
	: m_refTag(wxString::Format(s_refSpeedPts, id.GetId()))
	, m_pDoc(pDoc)
	, m_Venue(inVenue)
	, m_Div(inDiv)
	, m_Pts(inPts)
{
}


void CPointsDataSpeedPts::GetHtml(wxString& data, bool bNoInternalLinks)
{
	data << L"<tr>\n<td colspan=\"3\">" << Sanitize(m_Div->GetName(), true) << "</td>\n<td colspan=\"3\"></td>\n<td>"
		 << Sanitize(wxString::Format(_("IDS_POINTS_SPEED"), m_Pts), true) << "</td>\n</tr>\n";
}


bool CPointsDataSpeedPts::Details(wxString const& link)
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataMultiQs::CPointsDataMultiQs(
	CAgilityBookDoc* pDoc,
	ARBDogPtr const& inDog,
	ARBConfigVenuePtr const& inVenue,
	ARBConfigMultiQPtr const& inMultiQ,
	std::set<MultiQdata> const& inMQs,
	CRefTag& id)
	: m_refTag(wxString::Format(s_refMultiQ, id.GetId()))
	, m_pDoc(pDoc)
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
		ARBExistingPointType::MQ,
		m_Venue,
		m_MultiQ,
		ARBConfigDivisionPtr(),
		ARBConfigLevelPtr(),
		ARBConfigEventPtr(),
		dateFrom,
		dateTo);
}


void CPointsDataMultiQs::GetHtml(wxString& data, bool bNoInternalLinks)
{
	data << L"<tr>\n<td colspan=\"3\">" << Sanitize(m_MultiQ->GetName(), true)
		 << L"</td>\n<td colspan=\"3\"></td>\n<td>";
	if (!bNoInternalLinks)
	{
		data << L"<a href=\"" << ARB_PROTOCOL << m_refTag << L"\" name=\"" << m_refTag << L"\">";
	}
	data << ARBDouble::ToString(m_ExistingDblQs + static_cast<double>(m_MQs.size())) << L" "
		 << Sanitize(m_MultiQ->GetShortName());
	if (!bNoInternalLinks)
		data << L"</a>";
	data << L"</td>\n</tr>\n";
}


bool CPointsDataMultiQs::Details(wxString const& link)
{
	if (link != m_refTag)
		return false;

	MultiQInfoData data(m_Dog, m_Venue, m_MultiQ);
	CDlgListViewer dlg(m_pDoc, m_MultiQ->GetName(), &data, m_MQs);
	dlg.ShowModal();
	return true;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataLifetime::CPointsDataLifetime(CAgilityBookDoc* pDoc, ARB::ARBConfigVenuePtr const& inVenue, CRefTag& id)
	: m_refTag(wxString::Format(s_refLifetime, id.GetId()))
	, m_pDoc(pDoc)
	, m_LifetimeName()
	, m_Venue(inVenue)
	, m_Lifetime(0.0)
	, m_Filtered(0.0)
{
}


CPointsDataLifetime::CPointsDataLifetime(
	CAgilityBookDoc* pDoc,
	ARB::ARBConfigLifetimeNamePtr const& inLifetimeName,
	ARB::ARBConfigVenuePtr const& inVenue,
	CRefTag& id)
	: m_refTag(wxString::Format(s_refLifetime, id.GetId()))
	, m_pDoc(pDoc)
	, m_LifetimeName(inLifetimeName)
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
	m_Data.push_back(std::make_shared<LifeTimePointInfo>(inDiv, inLevel, inLifetime, inFiltered));
	m_Lifetime += inLifetime;
	m_Filtered += inFiltered;
}


void CPointsDataLifetime::GetHtml(wxString& data, bool bNoInternalLinks)
{
	data << L"<tr>\n<td>";
	if (m_LifetimeName)
	{
		wxString lifetime = m_LifetimeName->GetName();
		if (lifetime.empty())
			lifetime = _("IDS_TITLEPOINT_LIFETIME_NAME");
		data += wxString::Format(_("IDS_LIFETIME_POINTS"), Sanitize(lifetime, true));
	}
	else
		data << _("IDS_PLACEMENT_POINTS");
	data << L"</td>\n<td align=\"right\">";
	if (!bNoInternalLinks)
	{
		data << L"<a href=\"" << ARB_PROTOCOL << m_refTag << L"\" name=\"" << m_refTag << L"\">";
	}
	data << _("IDS_TOTAL") << L": ";
	if (0 < m_Filtered)
		data << ARBDouble::ToString(m_Lifetime - m_Filtered) << L" (" << ARBDouble::ToString(m_Lifetime) << L")";
	else
		data << ARBDouble::ToString(m_Lifetime);
	if (!bNoInternalLinks)
		data << L"</a>";
	data << L"</td>\n</tr>\n";
}


bool CPointsDataLifetime::Details(wxString const& link)
{
	if (link != m_refTag)
		return false;

	wxString caption(m_Venue->GetName());
	wxString str;
	if (m_LifetimeName)
	{
		wxString lifetime = m_LifetimeName->GetName();
		if (lifetime.empty())
			lifetime = _("IDS_TITLEPOINT_LIFETIME_NAME");
		str = wxString::Format(_("IDS_LIFETIME_POINTS"), lifetime);
	}
	else
		str = _("IDS_PLACEMENT_POINTS");
	caption += L" " + str;
	CDlgListViewer dlg(m_pDoc, caption, m_Data);
	dlg.ShowModal();
	return true;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataLifetimeByName::CPointsDataLifetimeByName(
	CAgilityBookDoc* pDoc,
	ARB::ARBConfigVenuePtr const& inVenue,
	wxString const& inName,
	CRefTag& id)
	: CPointsDataLifetime(pDoc, inVenue, id)
	, m_Name(inName)
{
}

CPointsDataLifetimeByName::CPointsDataLifetimeByName(
	CAgilityBookDoc* pDoc,
	ARB::ARBConfigLifetimeNamePtr const& inLifetimeName,
	ARB::ARBConfigVenuePtr const& inVenue,
	wxString const& inName,
	CRefTag& id)
	: CPointsDataLifetime(pDoc, inLifetimeName, inVenue, id)
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
		m_Data.push_back(std::make_shared<LifeTimePointInfo>(inSort1, inSort2, inLifetime, inFiltered));
		m_Lifetime += inLifetime;
		m_Filtered += inFiltered;
	}
}


void CPointsDataLifetimeByName::GetHtml(wxString& data, bool bNoInternalLinks)
{
	data << L"<tr>\n<td>&nbsp;</td>\n<td align=\"right\">";
	data << Sanitize(m_Name, true) << L": ";
	if (0 < m_Filtered)
		data << ARBDouble::ToString(m_Lifetime - m_Filtered) << L" (" << ARBDouble::ToString(m_Lifetime) << L")";
	else
		data << ARBDouble::ToString(m_Lifetime);
	data << L"</td>\n</tr>\n";
}

/////////////////////////////////////////////////////////////////////////////

class SortPointItems
{
public:
	SortPointItems()
	{
		CAgilityBookOptions::GetPointsViewSort(m_Order[0], m_Order[1], m_Order[2]);
	}
	bool operator()(CPointsDataEventPtr const one, CPointsDataEventPtr const two) const
	{
		for (int i = 0; i < 3; ++i)
		{
			switch (m_Order[i])
			{
			case ARBPointsViewSort::Unknown:
				assert(0);
				return false;

			case ARBPointsViewSort::Division:
				if (one->m_DivIdx != two->m_DivIdx)
					return one->m_DivIdx < two->m_DivIdx;
				break;

			case ARBPointsViewSort::Level:
				if (one->m_LevelIdx != two->m_LevelIdx)
					return one->m_LevelIdx < two->m_LevelIdx;
				break;

			case ARBPointsViewSort::Event:
				if (one->m_EventIdx != two->m_EventIdx)
					return one->m_EventIdx < two->m_EventIdx;
				break;
			}
		}
		return false;
	}

private:
	ARBPointsViewSort m_Order[3];
};

/////////////////////////////////////////////////////////////////////////////

CPointsDataVenue::CPointsDataVenue(
	std::vector<CVenueFilter> const& venues,
	CAgilityBookDoc* pDoc,
	ARBDogPtr const& inDog,
	ARBConfigVenuePtr const inVenue,
	CRefTag& id)
	: m_refTag(wxString::Format(s_refVenue, inVenue->GetName()))
	, m_pDoc(pDoc)
	, m_pDog(inDog)
	, m_pVenue(inVenue)
{
	// First, titles.
	for (ARBDogTitleList::const_iterator iterTitle = inDog->GetTitles().begin(); iterTitle != inDog->GetTitles().end();
		 ++iterTitle)
	{
		ARBDogTitlePtr pTitle = (*iterTitle);
		if (pTitle->GetVenue() == m_pVenue->GetName() && !pTitle->IsFiltered())
		{
			m_titles.push_back(std::make_shared<CPointsDataTitle>(m_pDoc, pTitle));
		}
	}

	bool bRunsInserted = false;
	LifeTimePointsList lifetime;
	PlacementPointsList placement;

	// Then the runs.
	std::list<ARBDogTrialPtr> trialsInVenue;
	for (ARBDogTrialList::const_iterator iterTrial = inDog->GetTrials().begin(); iterTrial != inDog->GetTrials().end();
		 ++iterTrial)
	{
		ARBDogTrialPtr pTrial = (*iterTrial);
		// Don't bother subtracting "hidden" trials. Doing so
		// will skew the qualifying percentage.
		if (pTrial->HasVenue(m_pVenue->GetName()))
			trialsInVenue.push_back(pTrial);
	}
	if (inDog->GetExistingPoints().HasPoints(m_pVenue->GetName()) || 0 < trialsInVenue.size())
	{
		bRunsInserted = true;
		// Show events sorted out by division/level.
		int idxDiv = 0;
		for (ARBConfigDivisionList::const_iterator iterDiv = m_pVenue->GetDivisions().begin();
			 iterDiv != m_pVenue->GetDivisions().end();
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
				for (ARBConfigEventList::const_iterator iterEvent = m_pVenue->GetEvents().begin();
					 iterEvent != m_pVenue->GetEvents().end();
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
						bool bHasExistingPoints
							= inDog->GetExistingPoints()
								  .HasPoints(m_pVenue, pDiv, pLevel, pEvent, dateFrom2, dateTo2, false);
						bool bHasExistingLifetimePoints
							= inDog->GetExistingPoints()
								  .HasPoints(m_pVenue, pDiv, pLevel, pEvent, dateFrom2, dateTo2, true);
						if (!CFilterOptions::Options()
								 .IsVenueLevelVisible(venues, m_pVenue->GetName(), pDiv->GetName(), pLevel->GetName()))
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
									|| (pRun->GetLevel() != pLevel->GetName()
										&& !pLevel->GetSubLevels().FindSubLevel(pRun->GetLevel()))
									|| pRun->GetEvent() != pEvent->GetName())
									continue;
								ARBConfigScoringPtr pScoring;
								pEvent->FindEvent(pDiv->GetName(), pLevel->GetName(), pRun->GetDate(), &pScoring);
								assert(pScoring);
								if (!pScoring)
									continue; // Shouldn't need it... Actually, we do - if a trial's venues are changed,
											  // this can cause FindEvent to fail.
								if (*pScoring != *pScoringMethod)
									continue;
								bool bRunVisible
									= (!pRun->IsFiltered(ARBFilterType::IgnoreQ)
									   && CFilterOptions::Options().IsRunVisible(venues, m_pVenue, pTrial, pRun));
								if (bRunVisible)
								{
									// Don't tally NA runs for titling events.
									if (!pRun->GetQ().AllowTally())
										continue;
									matching.push_back(RunInfo(inDog, pTrial, pRun));
									if (!pRun->GetJudge().empty())
									{
										judges.insert(pRun->GetJudge());
										if (pRun->GetQ().Qualified())
											judgesQ.insert(pRun->GetJudge());
									}
									if (pScoringMethod->HasSuperQ() && Q::SuperQ == pRun->GetQ())
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
										for (ARBDogRunPartnerList::const_iterator iterPartner
											 = pRun->GetPartners().begin();
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
									for (ARBConfigLifetimeNameList::iterator iterN
										 = m_pVenue->GetLifetimeNames().begin();
										 iterN != m_pVenue->GetLifetimeNames().end();
										 ++iterN)
									{
										double nLifetime = pRun->GetLifetimePoints(pScoringMethod, (*iterN)->GetName());
										if (0 < nLifetime)
										{
											ptsLifetime.ptLifetime[(*iterN)].push_back(
												LifeTimePoint(pRun->GetEvent(), nLifetime, !bRunVisible));
										}
									}
									double nPlacement = pRun->GetPlacementPoints(pScoringMethod);
									if (0 < nPlacement)
									{
										ptsPlacement.ptPlacement.push_back(
											LifeTimePoint(pRun->GetEvent(), nPlacement, !bRunVisible));
									}
								}
							}
						}
						// Accumulate existing points
						if (bHasExistingPoints || bHasExistingLifetimePoints || 0 < matching.size())
						{
							nExistingPts = inDog->GetExistingPoints().ExistingPoints(
								ARBExistingPointType::Title,
								m_pVenue,
								ARBConfigMultiQPtr(),
								pDiv,
								pLevel,
								pEvent,
								dateFrom2,
								dateTo2);
							if (pScoringMethod->HasSuperQ())
								nExistingSQ += static_cast<int>(inDog->GetExistingPoints().ExistingPoints(
									ARBExistingPointType::SQ,
									m_pVenue,
									ARBConfigMultiQPtr(),
									pDiv,
									pLevel,
									pEvent,
									dateFrom2,
									dateTo2));
							// Now add the existing lifetime points
							for (ARBConfigLifetimeNameList::const_iterator iterLifetime
								 = m_pVenue->GetLifetimeNames().begin();
								 iterLifetime != m_pVenue->GetLifetimeNames().end();
								 ++iterLifetime)
							{
								double nExistingLifetimePts = inDog->GetExistingPoints().ExistingLifetimePoints(
									*iterLifetime,
									m_pVenue,
									pDiv,
									pLevel,
									pEvent,
									dateFrom2,
									dateTo2);
								if (0.0 < nExistingLifetimePts)
									ptsLifetime.ptLifetime[(*iterLifetime)].push_back(
										LifeTimePoint(pEvent->GetName(), nExistingLifetimePts, false));
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
					// TODO: Add ability to accumulate existing placement points
					// Now we deal with the visible runs.
					if (0 < points || 0 < allmatching.size())
					{
						wxString strRunCount;
						if (0 < judges.size())
							strRunCount
								<< wxString::Format(_("IDS_POINTS_RUNS_JUDGES"), allmatching.size(), judges.size());
						else
							strRunCount << wxString::Format(_("IDS_POINTS_RUNS"), allmatching.size());
						if (pEvent->HasPartner() && 0 < partners.size())
						{
							strRunCount << wxString::Format(_("IDS_POINTS_PARTNERS"), partners.size());
						}
						double percentQs = 0.0;
						if (0 < allmatching.size())
							percentQs
								= (static_cast<double>(nCleanQ + nNotCleanQ) / static_cast<double>(allmatching.size()))
								  * 100;
						wxString strQcount;
						strQcount
							<< wxString::Format(_("IDS_POINTS_QS"), nCleanQ + nNotCleanQ, static_cast<int>(percentQs));
						if (0 < nCleanQ)
						{
							strQcount << wxString::Format(_("IDS_POINTS_CLEAN"), nCleanQ);
						}
						if (0 < judgesQ.size())
						{
							strQcount << wxString::Format(_("IDS_POINTS_JUDGES"), judgesQ.size());
						}
						if (pEvent->HasPartner() && 0 < partnersQ.size())
						{
							strQcount << wxString::Format(_("IDS_POINTS_PARTNERS"), partnersQ.size());
						}
						wxString strPts;
						strPts << ARBDouble::ToString(points + nExistingSQ);
						wxString strSuperQ;
						if (hasSQs)
						{
							SQs += nExistingSQ;
							strSuperQ << wxString::Format(_("IDS_POINTS_SQS"), SQs);
						}
						wxString strSpeed;
						if (bHasSpeedPts && 0 < speedPtsEvent)
						{
							strSpeed << wxString::Format(_("IDS_POINTS_SPEED_SUBTOTAL"), speedPtsEvent);
						}
						m_events.push_back(
							std::make_shared<CPointsDataEvent>(
								pDoc,
								!ARBDouble::equal(0.0, nExistingPts + nExistingSQ) ? inDog : ARBDogPtr(),
								allmatching,
								m_pVenue,
								pDiv,
								idxDiv,
								pLevel,
								idxLevel,
								pEvent,
								idxEvent,
								strRunCount,
								strQcount,
								strPts,
								strSuperQ,
								strSpeed,
								id));
					}
				}
				if (bHasSpeedPts)
				{
					speedPts += static_cast<int>(inDog->GetExistingPoints().ExistingPoints(
						ARBExistingPointType::Speed,
						m_pVenue,
						ARBConfigMultiQPtr(),
						pDiv,
						pLevel,
						ARBConfigEventPtr(),
						dateFrom,
						dateTo));
				}
				if (0 < ptsLifetime.ptLifetime.size())
					lifetime.push_back(ptsLifetime);
				if (0 < ptsPlacement.ptPlacement.size())
					placement.push_back(ptsPlacement);
			} // level loop
			if (bHasSpeedPts)
			{
				m_speedPts.push_back(std::make_shared<CPointsDataSpeedPts>(pDoc, m_pVenue, pDiv, speedPts, id));
			}
		} // division loop
		if (1 < m_events.size())
			std::stable_sort(m_events.begin(), m_events.end(), SortPointItems());

		// If the venue has multiQs, tally them now.
		if (0 < m_pVenue->GetMultiQs().size())
		{
			std::map<ARBConfigMultiQPtr, std::set<MultiQdata>> MQs;
			for (std::list<ARBDogTrialPtr>::const_iterator iterTrial = trialsInVenue.begin();
				 iterTrial != trialsInVenue.end();
				 ++iterTrial)
			{
				ARBDogTrialPtr pTrial = (*iterTrial);
				for (ARBDogRunList::const_iterator iterR = pTrial->GetRuns().begin(); iterR != pTrial->GetRuns().end();
					 ++iterR)
				{
					ARBDogRunPtr pRun = *iterR;
					std::vector<ARBConfigMultiQPtr> multiQs;
					if (0 < pRun->GetMultiQs(multiQs) && !pRun->IsFiltered(ARBFilterType::IgnoreQ)
						&& CFilterOptions::Options().IsRunVisible(venues, m_pVenue, pTrial, pRun))
					{
						for (std::vector<ARBConfigMultiQPtr>::iterator iMultiQ = multiQs.begin();
							 iMultiQ != multiQs.end();
							 ++iMultiQ)
						{
							wxString name;
							if (pRun->GetClub())
								name = pRun->GetClub()->GetName();
							MQs[*iMultiQ].insert(MultiQdata(pRun->GetDate(), pTrial, name));
						}
					}
				}
			}
			// List multiQs in configuration order.
			for (auto iMulti = m_pVenue->GetMultiQs().begin(); iMulti != m_pVenue->GetMultiQs().end(); ++iMulti)
			{
				auto iterMQ = MQs.find((*iMulti));
				if (iterMQ != MQs.end())
				{
					m_multiQs.push_back(
						std::make_shared<
							CPointsDataMultiQs>(pDoc, inDog, m_pVenue, (*iterMQ).first, (*iterMQ).second, id));
				}
			}
		}
	}

	// Next comes lifetime points.
	if (0 < lifetime.size())
	{
		for (ARBConfigLifetimeNameList::iterator iterL = m_pVenue->GetLifetimeNames().begin();
			 iterL != m_pVenue->GetLifetimeNames().end();
			 ++iterL)
		{
			CPointsDataLifetimePtr pData = std::make_shared<CPointsDataLifetime>(pDoc, (*iterL), m_pVenue, id);
			typedef std::map<wxString, CPointsDataLifetimeByNamePtr> NamedLifetime;
			NamedLifetime subgroups;
			if (CAgilityBookOptions::GetViewLifetimePointsByEvent())
			{
				// Gather event names
				std::set<wxString> names;
				for (LifeTimePointsList::iterator iter = lifetime.begin(); iter != lifetime.end(); ++iter)
				{
					std::map<ARBConfigLifetimeNamePtr, LifeTimePointList>::iterator iterName
						= (*iter).ptLifetime.find((*iterL));
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
				for (std::set<wxString>::iterator iName = names.begin(); iName != names.end(); ++iName)
				{
					double pts2 = 0.0;
					double ptFiltered2 = 0;
					for (LifeTimePointsList::iterator iter = lifetime.begin(); iter != lifetime.end(); ++iter)
					{
						std::map<ARBConfigLifetimeNamePtr, LifeTimePointList>::iterator iterName
							= (*iter).ptLifetime.find((*iterL));
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
					pData->AddLifetimeInfo(*iName, wxString(), pts2, ptFiltered2);
				}
			}
			for (LifeTimePointsList::iterator iter = lifetime.begin(); iter != lifetime.end(); ++iter)
			{
				CPointsDataLifetimeByNamePtr pNameData;
				NamedLifetime::iterator it = subgroups.find(iter->pDiv->GetName());
				if (subgroups.end() != it)
				{
					pNameData = it->second;
				}
				else
				{
					pNameData = std::make_shared<CPointsDataLifetimeByName>(
						pDoc,
						(*iterL),
						m_pVenue,
						iter->pDiv->GetName(),
						id);
					subgroups.insert(NamedLifetime::value_type(iter->pDiv->GetName(), pNameData));
				}

				double pts2 = 0.0;
				double ptFiltered2 = 0;
				std::map<ARBConfigLifetimeNamePtr, LifeTimePointList>::iterator iterName
					= (*iter).ptLifetime.find((*iterL));
				if ((*iter).ptLifetime.end() != iterName)
				{
					for (LifeTimePointList::iterator iter2 = iterName->second.begin(); iter2 != iterName->second.end();
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

			m_lifetime.push_back(pData);
			if (1 < subgroups.size())
			{
				for (NamedLifetime::iterator it = subgroups.begin(); it != subgroups.end(); ++it)
				{
					m_lifetime.push_back(it->second);
				}
			}
			else if (1 == subgroups.size())
			{
				subgroups.begin()->second.reset();
			}
		}
	}

	if (0 < placement.size())
	{
		CPointsDataLifetimePtr pData = std::make_shared<CPointsDataLifetime>(pDoc, m_pVenue, id);
		typedef std::map<wxString, CPointsDataLifetimeByNamePtr> NamedLifetime;
		NamedLifetime subgroups;
		if (CAgilityBookOptions::GetViewLifetimePointsByEvent())
		{
			// Gather event names
			std::set<wxString> names;
			for (PlacementPointsList::iterator iter = placement.begin(); iter != placement.end(); ++iter)
			{
				for (LifeTimePointList::iterator iter2 = (*iter).ptPlacement.begin();
					 iter2 != (*iter).ptPlacement.end();
					 ++iter2)
				{
					names.insert(iter2->eventName);
				}
			}
			for (std::set<wxString>::iterator iName = names.begin(); iName != names.end(); ++iName)
			{
				double pts2 = 0.0;
				double ptFiltered2 = 0;
				for (PlacementPointsList::iterator iter = placement.begin(); iter != placement.end(); ++iter)
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
				pData->AddLifetimeInfo(*iName, wxString(), pts2, ptFiltered2);
			}
		}
		for (PlacementPointsList::iterator iter = placement.begin(); iter != placement.end(); ++iter)
		{
			CPointsDataLifetimeByNamePtr pNameData;
			NamedLifetime::iterator it = subgroups.find(iter->pDiv->GetName());
			if (subgroups.end() != it)
			{
				pNameData = it->second;
			}
			else
			{
				pNameData = std::make_shared<CPointsDataLifetimeByName>(pDoc, m_pVenue, iter->pDiv->GetName(), id);
				subgroups.insert(NamedLifetime::value_type(iter->pDiv->GetName(), pNameData));
			}

			double pts2 = 0.0;
			double ptFiltered2 = 0;
			for (LifeTimePointList::iterator iter2 = (*iter).ptPlacement.begin(); iter2 != (*iter).ptPlacement.end();
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
		m_lifetime.push_back(pData);
		if (1 < subgroups.size())
		{
			for (NamedLifetime::iterator it = subgroups.begin(); it != subgroups.end(); ++it)
			{
				m_lifetime.push_back(it->second);
			}
		}
		else if (1 == subgroups.size())
		{
			subgroups.begin()->second.reset();
		}
	}
}


bool CPointsDataVenue::HasData() const
{
	return !m_titles.empty() || !m_events.empty() || !m_speedPts.empty() || !m_multiQs.empty() || !m_lifetime.empty();
}


void CPointsDataVenue::GetHtml(wxString& data, bool bNoInternalLinks)
{
	if (!HasData())
		return;

	// Venue header
	if (m_pVenue)
	{
		data << L"<h2>";
		if (m_pVenue->GetURL().empty())
		{
			data << Sanitize(m_pVenue->GetName());
		}
		else
		{
			data << L"<a href=\"" << m_pVenue->GetURL() << L"\">" << Sanitize(m_pVenue->GetName()) << L"</a>";
		}
		if (m_pDog)
		{
			ARBDogRegNumPtr pRegNum;
			if (m_pDog->GetRegNums().FindRegNum(m_pVenue->GetName(), &pRegNum))
			{
				data << L" [";
				if (!bNoInternalLinks)
				{
					data << L"<a href=\"" << ARB_PROTOCOL << m_refTag << L"\" name=\"" << m_refTag << L"\">";
				}
				data << Sanitize(pRegNum->GetNumber());
				if (!bNoInternalLinks)
					data << L"</a>";
				data << L"]\n";
			}
		}
		data << L"</h2>\n";
	}

	if (!m_titles.empty())
	{
		data << L"<h3>" << _("IDS_TITLES") << L"</h3>" << s_TableHeader;

		for (auto ptr : m_titles)
		{
			ptr->GetHtml(data, bNoInternalLinks);
		}
		data << L"</table>";
	}

	if (!m_events.empty() || !m_speedPts.empty() || !m_multiQs.empty() || !m_lifetime.empty())
	{
		data << L"<h3>" << _("IDS_RUNS") << L"</h3>" << s_TableHeader;
		for (auto ptr : m_events)
		{
			ptr->GetHtml(data, bNoInternalLinks);
		}
		for (auto ptr : m_speedPts)
		{
			ptr->GetHtml(data, bNoInternalLinks);
		}
		for (auto ptr : m_multiQs)
		{
			ptr->GetHtml(data, bNoInternalLinks);
		}
		if (!m_lifetime.empty())
			data << L"<tr><td>&nbsp;</td></tr>\n";
		for (auto ptr : m_lifetime)
		{
			ptr->GetHtml(data, bNoInternalLinks);
		}
		data << L"</table>";
	}
}


bool CPointsDataVenue::Details(wxString const& link)
{
	if (link == m_refTag)
	{
		if (m_pDog && m_pVenue)
		{
			ARBDogRegNumPtr pRegNum;
			if (m_pDog->GetRegNums().FindRegNum(m_pVenue->GetName(), &pRegNum))
			{
				CDlgDog dlg(m_pDoc, m_pDog, nullptr, 2);
				dlg.ShowModal();
			}
		}
		else
			wxBell();
		return true;
	}
	for (auto ptr : m_titles)
	{
		if (ptr->Details(link, m_pDog))
			return true;
	}
	for (auto ptr : m_events)
	{
		if (ptr->Details(link))
			return true;
	}
	for (auto ptr : m_speedPts)
	{
		if (ptr->Details(link))
			return true;
	}
	for (auto ptr : m_multiQs)
	{
		if (ptr->Details(link))
			return true;
	}
	for (auto ptr : m_lifetime)
	{
		if (ptr->Details(link))
			return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPoints::CPointsDataOtherPoints(
	CAgilityBookDoc* pDoc,
	ARBConfigOtherPointsPtr pOther,
	std::list<OtherPtInfo> const& inRunList,
	CRefTag& id)
	: m_refTag(wxString::Format(s_refOther, id.GetId()))
	, m_pDoc(pDoc)
	, m_pOther(pOther)
	, m_RunList(inRunList)
	, m_Points(0.0)
{
	for (std::list<OtherPtInfo>::iterator iter = m_RunList.begin(); iter != m_RunList.end(); ++iter)
	{
		m_Points += (*iter).m_Points;
	}
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPointsTallyAll::CPointsDataOtherPointsTallyAll(
	CAgilityBookDoc* pDoc,
	ARBConfigOtherPointsPtr pOther,
	std::list<OtherPtInfo> const& inRunList,
	CRefTag& id)
	: CPointsDataOtherPoints(pDoc, pOther, inRunList, id)
{
}


void CPointsDataOtherPointsTallyAll::GetHtml(wxString& data, bool bNoInternalLinks)
{
	data << L"<tr>\n<td>" << Sanitize(m_pOther->GetName(), true) << L"</td>\n<td align=\"right\">";
	if (!bNoInternalLinks)
	{
		data << L"<a href=\"" << ARB_PROTOCOL << m_refTag << L"\" name=\"" << m_refTag << L"\">";
	}
	data << ARBDouble::ToString(m_Points);
	if (!bNoInternalLinks)
		data << L"</a>";
	data << L"</td>\n</tr>\n";
}


bool CPointsDataOtherPointsTallyAll::Details(wxString const& link)
{
	if (link != m_refTag)
		return false;

	CDlgListViewer dlg(m_pDoc, _("IDS_OTHERPOINTS"), m_RunList);
	dlg.ShowModal();
	return true;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPointsTallyAllByEvent::CPointsDataOtherPointsTallyAllByEvent(
	CAgilityBookDoc* pDoc,
	ARBConfigOtherPointsPtr pOther,
	wxString const& inEvent,
	std::list<OtherPtInfo> const& inRunList,
	CRefTag& id)
	: CPointsDataOtherPoints(pDoc, pOther, inRunList, id)
	, m_Event(inEvent)
{
}


void CPointsDataOtherPointsTallyAllByEvent::GetHtml(wxString& data, bool bNoInternalLinks)
{
	data << L"<tr>\n<td>&nbsp;</td>\n<td>" << Sanitize(m_RunList.begin()->m_Event, true)
		 << L"</td>\n<td align=\"right\">";
	if (!bNoInternalLinks)
	{
		data << L"<a href=\"" << ARB_PROTOCOL << m_refTag << L"\" name=\"" << m_refTag << L"\">";
	}
	data << ARBDouble::ToString(m_Points);
	if (!bNoInternalLinks)
		data << L"</a>";
	data << L"</td>\n</tr>\n";
}


bool CPointsDataOtherPointsTallyAllByEvent::Details(wxString const& link)
{
	if (link != m_refTag)
		return false;

	CDlgListViewer dlg(m_pDoc, _("IDS_OTHERPOINTS"), m_RunList);
	dlg.ShowModal();
	return true;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPointsTallyLevel::CPointsDataOtherPointsTallyLevel(
	CAgilityBookDoc* pDoc,
	ARBConfigOtherPointsPtr pOther,
	wxString const& inLevel,
	std::list<OtherPtInfo> const& inRunList,
	CRefTag& id)
	: CPointsDataOtherPoints(pDoc, pOther, inRunList, id)
	, m_Level(inLevel)
{
}


void CPointsDataOtherPointsTallyLevel::GetHtml(wxString& data, bool bNoInternalLinks)
{
	data << L"<tr>\n<td>&nbsp;</td>\n<td>" << Sanitize(m_RunList.begin()->m_Level, true)
		 << L"</td>\n<td align=\"right\">";
	if (!bNoInternalLinks)
	{
		data << L"<a href=\"" << ARB_PROTOCOL << m_refTag << L"\" name=\"" << m_refTag << L"\">";
	}
	data << ARBDouble::ToString(m_Points);
	if (!bNoInternalLinks)
		data << L"</a>";
	data << L"</td>\n</tr>\n";
}


bool CPointsDataOtherPointsTallyLevel::Details(wxString const& link)
{
	if (link != m_refTag)
		return false;

	CDlgListViewer dlg(m_pDoc, _("IDS_OTHERPOINTS"), m_RunList);
	dlg.ShowModal();
	return true;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataOtherPointsTallyLevelByEvent::CPointsDataOtherPointsTallyLevelByEvent(
	CAgilityBookDoc* pDoc,
	ARBConfigOtherPointsPtr pOther,
	wxString const& inLevel,
	wxString const& inEvent,
	std::list<OtherPtInfo> const& inRunList,
	CRefTag& id)
	: CPointsDataOtherPoints(pDoc, pOther, inRunList, id)
	, m_Level(inLevel)
	, m_Event(inEvent)
{
}


void CPointsDataOtherPointsTallyLevelByEvent::GetHtml(wxString& data, bool bNoInternalLinks)
{
	data << L"<tr>\n<td>&nbsp;</td>\n<td>" << Sanitize(m_RunList.begin()->m_Level, true) << L"</td>\n<td>"
		 << Sanitize(m_RunList.begin()->m_Event, true) << L"</td>\n<td align=\"right\">";
	if (!bNoInternalLinks)
	{
		data << L"<a href=\"" << ARB_PROTOCOL << m_refTag << L"\" name=\"" << m_refTag << L"\">";
	}
	data << ARBDouble::ToString(m_Points);
	if (!bNoInternalLinks)
		data << L"</a>";
	data << L"</td>\n</tr>\n";
}


bool CPointsDataOtherPointsTallyLevelByEvent::Details(wxString const& link)
{
	if (link != m_refTag)
		return false;

	CDlgListViewer dlg(m_pDoc, _("IDS_OTHERPOINTS"), m_RunList);
	dlg.ShowModal();
	return true;
}

/////////////////////////////////////////////////////////////////////////////

CPointsDataItems::CPointsDataItems(CAgilityBookDoc* pDoc)
	: m_pDoc(pDoc)
	, m_pDog()
{
	assert(m_pDoc);
}


void CPointsDataItems::clear()
{
	m_pDog.reset();
	m_venues.clear();
	m_otherPts.clear();
}


void CPointsDataItems::LoadData(ARBDogPtr const& inDog)
{
	clear();

	if (!inDog)
		return;

	m_pDog = inDog;

	// Find all visible items and sort them out by venue.
	std::vector<CVenueFilter> venues;
	CFilterOptions::Options().GetFilterVenue(venues);
	CRefTag id;

	// For each venue...
	for (ARBConfigVenueList::const_iterator iterVenue = m_pDoc->Book().GetConfig().GetVenues().begin();
		 iterVenue != m_pDoc->Book().GetConfig().GetVenues().end();
		 ++iterVenue)
	{
		ARBConfigVenuePtr pVenue = (*iterVenue);
		if (!CFilterOptions::Options().IsVenueVisible(venues, pVenue->GetName()))
			continue;

		CPointsDataVenuePtr pVenueData = std::make_shared<CPointsDataVenue>(venues, m_pDoc, inDog, pVenue, id);
		if (pVenueData->HasData())
			m_venues.push_back(pVenueData);
	}

	// After all the venues, we do 'other points'.
	ARBConfigOtherPointsList const& other = m_pDoc->Book().GetConfig().GetOtherPoints();
	if (0 < other.size())
	{
		for (ARBConfigOtherPointsList::const_iterator iterOther = other.begin(); iterOther != other.end(); ++iterOther)
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
						if (!pRun->IsFiltered(ARBFilterType::IgnoreQ))
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
									if (pRun->GetClub())
									{
										m_pDoc->Book().GetConfig().GetVenues().FindEvent(
											pRun->GetClub()->GetVenue(),
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
				if (ARBExistingPointType::OtherPoints == (*iterExisting)->GetType()
					&& (*iterExisting)->GetTypeName() == pOther->GetName())
				{
					runs.push_back(OtherPtInfo(*iterExisting));
				}
			}

			if (0 == runs.size())
				continue;

			switch (pOther->GetTally())
			{
			case ARBOtherPointsTally::All:
				m_otherPts.push_back(std::make_shared<CPointsDataOtherPointsTallyAll>(m_pDoc, pOther, runs, id));
				break;

			case ARBOtherPointsTally::AllByEvent:
			{
				std::set<wxString> tally;
				std::list<OtherPtInfo>::iterator iter;
				for (iter = runs.begin(); iter != runs.end(); ++iter)
				{
					tally.insert((*iter).m_Event);
				}
				for (std::set<wxString>::iterator iterTally = tally.begin(); iterTally != tally.end(); ++iterTally)
				{
					std::list<OtherPtInfo> validRuns;
					for (iter = runs.begin(); iter != runs.end(); ++iter)
					{
						if ((*iter).m_Event == (*iterTally))
							validRuns.push_back(*iter);
					}
					m_otherPts.push_back(
						std::make_shared<CPointsDataOtherPointsTallyAllByEvent>(
							m_pDoc,
							pOther,
							(*iterTally),
							validRuns,
							id));
				}
			}
			break;

			case ARBOtherPointsTally::Level:
			{
				std::set<wxString> tally;
				std::list<OtherPtInfo>::iterator iter;
				for (iter = runs.begin(); iter != runs.end(); ++iter)
				{
					tally.insert((*iter).m_Level);
				}
				for (std::set<wxString>::iterator iterTally = tally.begin(); iterTally != tally.end(); ++iterTally)
				{
					std::list<OtherPtInfo> validRuns;
					for (iter = runs.begin(); iter != runs.end(); ++iter)
					{
						if ((*iter).m_Level == (*iterTally))
							validRuns.push_back(*iter);
					}
					m_otherPts.push_back(
						std::make_shared<CPointsDataOtherPointsTallyLevel>(
							m_pDoc,
							pOther,
							(*iterTally),
							validRuns,
							id));
				}
			}
			break;

			case ARBOtherPointsTally::LevelByEvent:
			{
				typedef std::pair<wxString, wxString> LevelEvent;
				std::set<LevelEvent> tally;
				std::list<OtherPtInfo>::iterator iter;
				for (iter = runs.begin(); iter != runs.end(); ++iter)
				{
					tally.insert(LevelEvent((*iter).m_Level, (*iter).m_Event));
				}
				for (std::set<LevelEvent>::iterator iterTally = tally.begin(); iterTally != tally.end(); ++iterTally)
				{
					std::list<OtherPtInfo> validRuns;
					for (iter = runs.begin(); iter != runs.end(); ++iter)
					{
						if ((*iter).m_Level == (*iterTally).first && (*iter).m_Event == (*iterTally).second)
							validRuns.push_back(*iter);
					}
					m_otherPts.push_back(
						std::make_shared<CPointsDataOtherPointsTallyLevelByEvent>(
							m_pDoc,
							pOther,
							(*iterTally).first,
							(*iterTally).second,
							validRuns,
							id));
				}
			}
			break;
			}
		}
	}
}


wxString CPointsDataItems::GetHtml(bool bFragment, bool bNoInternalLinks)
{
	ARBDate today(ARBDate::Today());
	wxString data;

	// Put header in...
	if (!bFragment)
	{
		data << L"<html><head><title>" << _("IDS_TITLING_POINTS") << L" " << today.GetString()
			 << L"</title></head><body>\n";
	}

	// Put general info about the dog in...
	data << L"<h1 align=\"center\">" << _("IDS_TITLING_POINTS") << L" " << Sanitize(today.GetString()) << L"</h1>";
	if (m_pDog)
	{
		data << L"<h1>";
		if (!bNoInternalLinks)
			data << L"<a href=\"" << ARB_PROTOCOL << s_refDog << L"\" name=\"" << s_refDog << L"\">";
		data << Sanitize(m_pDog->GetCallName());
		if (!bNoInternalLinks)
			data << L"</a>";
		if (!m_pDog->GetRegisteredName().empty())
			data << L" [" << Sanitize(m_pDog->GetRegisteredName()) << "]";
		data << L"</h1>\n";
	}

	// Venues
	for (auto venue : m_venues)
	{
		venue->GetHtml(data, bNoInternalLinks);
	}

	// Other Points
	if (!m_otherPts.empty())
	{
		data << L"<h2>" << _("IDS_OTHERPOINTS") << L"</h2>" << s_TableHeader;
		ARBOtherPointsTally last = static_cast<ARBOtherPointsTally>(-1);
		for (auto other : m_otherPts)
		{
			if (other->GetOther()->GetTally() != last)
			{
				last = other->GetOther()->GetTally();
				if (ARBOtherPointsTally::All != last)
				{
					data << L"<tr>\n<td>" << Sanitize(other->GetOther()->GetName(), true) << L"</td>\n</tr>\n";
				}
			}
			other->GetHtml(data, bNoInternalLinks);
		}
	}

	if (!bFragment)
		data << L"</body></html>";
	data << L"\n";

	return data;
}


bool CPointsDataItems::Details(wxString const& link)
{
	if (link == s_refDog)
	{
		CDlgDog dlg(m_pDoc, m_pDog, nullptr, 0);
		dlg.ShowModal();
		return true;
	}
	for (auto venue : m_venues)
	{
		if (venue->Details(link))
			return true;
	}
	for (auto other : m_otherPts)
	{
		if (other->Details(link))
			return true;
	}
	return false;
}

} // namespace dconSoft
