/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * 2020-10-07 Fix issue were we could save bad data (set a blank Q with a place)
 * 2020-07-31 On Faults[12]00ThenTime, don't allow score to go negative.
 * 2017-12-31 Add support for using raw faults when determining title points.
 * 2017-09-04 Change default DogsInClass to -1 (allows for DNR runs with 0 dogs)
 * 2016-01-06 Add support for named lifetime points.
 * 2015-11-27 Changed generic name to use subname if set.
 * 2015-05-19 Added GetName (generic name without date).
 * 2015-03-15 Fixed Unknown-Q usage.
 * 2014-09-02 Fix gambles with no closing required so titling works.
 * 2014-02-12 Added 'scorePts' to 'Placement'.
 * 2012-09-09 Added 'titlePts','speedPts' to 'Placement'.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-06-11 Still not right - runs with 0 SCT should allow title pts.
 * 2008-03-03 Title point computation on T+F runs was wrong.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-04 Added support for NADAC bonus titling points.
 * 2005-10-31 Time+Fault runs didn't compute titling points right.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-10 Only sort runs one way, the UI handles everything else.
 * 2005-01-02 Added subnames to events.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-10-04 Only compute MACH points if time and SCT are >0. Also, do
 *            not allow negative mach points.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-09-07 Time+Fault scoring shouldn't include time faults.
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-03-30 Added links.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-12-27 Changed FindEvent to take a date.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-10-13 Allow course faults on point runs.
 * 2003-09-01 Fixed problem with Score and TitlePts in time+faults.
 */

#include "stdafx.h"
#include "ARB/ARBDogRun.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBCommon;
namespace ARB
{

namespace
{
class ARBDogRun_concrete : public ARBDogRun
{
public:
	ARBDogRun_concrete()
	{
	}
	ARBDogRun_concrete(ARBDogRun const& rhs)
		: ARBDogRun(rhs)
	{
	}
};
}; // namespace


ARBDogRunPtr ARBDogRun::New()
{
	return std::make_shared<ARBDogRun_concrete>();
}


ARBDogRun::ARBDogRun()
	: m_pMultiQs()
	, m_Date()
	, m_Club()
	, m_Division()
	, m_Level()
	, m_Height()
	, m_Event()
	, m_SubName()
	, m_isAtHome(false)
	, m_Conditions()
	, m_Judge()
	, m_Handler()
	, m_Partners()
	, m_Scoring()
	, m_Q()
	, m_Place(0)
	, m_InClass(-1)
	, m_DogsQd(-1)
	, m_OtherPoints()
	, m_Notes()
	, m_RefRuns()
	, m_Links()
{
}


ARBDogRun::ARBDogRun(ARBDogRun const& rhs)
	: m_pMultiQs()
	, m_Date(rhs.m_Date)
	, m_Club(rhs.m_Club)
	, m_Division(rhs.m_Division)
	, m_Level(rhs.m_Level)
	, m_Height(rhs.m_Height)
	, m_Event(rhs.m_Event)
	, m_SubName(rhs.m_SubName)
	, m_isAtHome(rhs.m_isAtHome)
	, m_Conditions(rhs.m_Conditions)
	, m_Judge(rhs.m_Judge)
	, m_Handler(rhs.m_Handler)
	, m_Partners()
	, m_Scoring(rhs.m_Scoring)
	, m_Q(rhs.m_Q)
	, m_Place(rhs.m_Place)
	, m_InClass(rhs.m_InClass)
	, m_DogsQd(rhs.m_DogsQd)
	, m_OtherPoints()
	, m_Notes(rhs.m_Notes)
	, m_RefRuns()
	, m_Links(rhs.m_Links)
{
	rhs.m_Partners.Clone(m_Partners);
	rhs.m_OtherPoints.Clone(m_OtherPoints);
	rhs.m_RefRuns.Clone(m_RefRuns);
}


ARBDogRun::ARBDogRun(ARBDogRun&& rhs)
	: m_pMultiQs()
	, m_Date(std::move(rhs.m_Date))
	, m_Club(std::move(rhs.m_Club))
	, m_Division(std::move(rhs.m_Division))
	, m_Level(std::move(rhs.m_Level))
	, m_Height(std::move(rhs.m_Height))
	, m_Event(std::move(rhs.m_Event))
	, m_SubName(std::move(rhs.m_SubName))
	, m_isAtHome(std::move(rhs.m_isAtHome))
	, m_Conditions(std::move(rhs.m_Conditions))
	, m_Judge(std::move(rhs.m_Judge))
	, m_Handler(std::move(rhs.m_Handler))
	, m_Partners(std::move(rhs.m_Partners))
	, m_Scoring(std::move(rhs.m_Scoring))
	, m_Q(std::move(rhs.m_Q))
	, m_Place(std::move(rhs.m_Place))
	, m_InClass(std::move(rhs.m_InClass))
	, m_DogsQd(std::move(rhs.m_DogsQd))
	, m_OtherPoints(std::move(rhs.m_OtherPoints))
	, m_Notes(std::move(rhs.m_Notes))
	, m_RefRuns(std::move(rhs.m_RefRuns))
	, m_Links(std::move(rhs.m_Links))
{
}


ARBDogRun::~ARBDogRun()
{
}


ARBDogRunPtr ARBDogRun::Clone() const
{
	return std::make_shared<ARBDogRun_concrete>(*this);
}


ARBDogRun& ARBDogRun::operator=(ARBDogRun const& rhs)
{
	if (this != &rhs)
	{
		m_pMultiQs.clear();
		m_Date = rhs.m_Date;
		m_Club = rhs.m_Club;
		m_Division = rhs.m_Division;
		m_Level = rhs.m_Level;
		m_Height = rhs.m_Height;
		m_Event = rhs.m_Event;
		m_SubName = rhs.m_SubName;
		m_isAtHome = rhs.m_isAtHome;
		m_Conditions = rhs.m_Conditions;
		m_Judge = rhs.m_Judge;
		m_Handler = rhs.m_Handler;
		rhs.m_Partners.Clone(m_Partners);
		m_Scoring = rhs.m_Scoring;
		m_Q = rhs.m_Q;
		m_Place = rhs.m_Place;
		m_InClass = rhs.m_InClass;
		m_DogsQd = rhs.m_DogsQd;
		rhs.m_OtherPoints.Clone(m_OtherPoints);
		m_Notes = rhs.m_Notes;
		rhs.m_RefRuns.Clone(m_RefRuns);
		m_Links = rhs.m_Links;
	}
	return *this;
}


ARBDogRun& ARBDogRun::operator=(ARBDogRun&& rhs)
{
	if (this != &rhs)
	{
		m_pMultiQs.clear();
		m_Date = std::move(rhs.m_Date);
		m_Club = std::move(rhs.m_Club);
		m_Division = std::move(rhs.m_Division);
		m_Level = std::move(rhs.m_Level);
		m_Height = std::move(rhs.m_Height);
		m_Event = std::move(rhs.m_Event);
		m_SubName = std::move(rhs.m_SubName);
		m_isAtHome = std::move(rhs.m_isAtHome);
		m_Conditions = std::move(rhs.m_Conditions);
		m_Judge = std::move(rhs.m_Judge);
		m_Handler = std::move(rhs.m_Handler);
		m_Partners = std::move(rhs.m_Partners);
		m_Scoring = std::move(rhs.m_Scoring);
		m_Q = std::move(rhs.m_Q);
		m_Place = std::move(rhs.m_Place);
		m_InClass = std::move(rhs.m_InClass);
		m_DogsQd = std::move(rhs.m_DogsQd);
		m_OtherPoints = std::move(rhs.m_OtherPoints);
		m_Notes = std::move(rhs.m_Notes);
		m_RefRuns = std::move(rhs.m_RefRuns);
		m_Links = std::move(rhs.m_Links);
	}
	return *this;
}


bool ARBDogRun::operator==(ARBDogRun const& rhs) const
{
	// clang-format off
	return m_Date == rhs.m_Date
		&& m_Club == rhs.m_Club
		&& m_Division == rhs.m_Division
		&& m_Level == rhs.m_Level
		&& m_Height == rhs.m_Height
		&& m_Event == rhs.m_Event
		&& m_SubName == rhs.m_SubName
		&& m_isAtHome == rhs.m_isAtHome
		&& m_Conditions == rhs.m_Conditions
		&& m_Judge == rhs.m_Judge
		&& m_Handler == rhs.m_Handler
		&& m_Partners == rhs.m_Partners
		&& m_Scoring == rhs.m_Scoring
		&& m_Q == rhs.m_Q
		&& m_Place == rhs.m_Place
		&& m_InClass == rhs.m_InClass
		&& m_DogsQd == rhs.m_DogsQd
		&& m_OtherPoints == rhs.m_OtherPoints
		&& m_Notes == rhs.m_Notes
		&& m_RefRuns == rhs.m_RefRuns
		&& m_Links == rhs.m_Links;
	// clang-format on
}


wxString ARBDogRun::GetName() const
{
	wxString name = m_Division + L" " + m_Level + L" " + m_Event;
	if (0 < m_SubName.length())
	{
		name += L" " + m_SubName;
	}
	return name;
}


wxString ARBDogRun::GetGenericName() const
{
	wxString name = m_Date.GetString(ARBDateFormat::DashYMD) + L" ";
	if (0 < m_SubName.length())
		name = m_Division + L" " + m_Level + L" " + m_SubName;
	else
		name += GetName();
	return name;
}


size_t ARBDogRun::GetSearchStrings(std::set<wxString>& ioStrings) const
{
	size_t nItems = 0;

	ioStrings.insert(m_Date.GetString(ARBDateFormat::SlashMDY));
	++nItems;

	if (0 < m_Club->GetName().length())
	{
		ioStrings.insert(m_Club->GetName());
		++nItems;
	}

	if (0 < m_Division.length())
	{
		ioStrings.insert(m_Division);
		++nItems;
	}

	if (0 < m_Level.length())
	{
		ioStrings.insert(m_Level);
		++nItems;
	}

	if (0 < m_Height.length())
	{
		ioStrings.insert(m_Height);
		++nItems;
	}

	if (0 < m_Event.length())
	{
		ioStrings.insert(m_Event);
		++nItems;
	}

	if (0 < m_SubName.length())
	{
		ioStrings.insert(m_SubName);
		++nItems;
	}

	if (0 < m_Conditions.length())
	{
		ioStrings.insert(m_Conditions);
		++nItems;
	}

	if (0 < m_Judge.length())
	{
		ioStrings.insert(m_Judge);
		++nItems;
	}

	if (0 < m_Handler.length())
	{
		ioStrings.insert(m_Handler);
		++nItems;
	}

	nItems += m_Partners.GetSearchStrings(ioStrings);

	nItems += m_OtherPoints.GetSearchStrings(ioStrings);

	nItems += m_Notes.GetSearchStrings(ioStrings);

	nItems += m_RefRuns.GetSearchStrings(ioStrings);

	return nItems;
}


bool ARBDogRun::Load(
	ARBConfig const& inConfig,
	ARBDogClubList const& inClubs,
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_RUN)
		return false;
	switch (inTree->GetAttrib(ATTRIB_RUN_DATE, m_Date))
	{
	case ARBAttribLookup::Found:
		break;
	case ARBAttribLookup::NotFound:
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_DATE));
		return false;
	case ARBAttribLookup::Invalid:
	{
		wxString attrib;
		inTree->GetAttrib(ATTRIB_RUN_DATE, attrib);
		wxString msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_RUN, ATTRIB_RUN_DATE, msg));
		return false;
	}
	}

	{
		short idx = 0;
		if (inVersion >= ARBVersion(15, 0))
			inTree->GetAttrib(ATTRIB_RUN_CLUB, idx);
		if (0 > idx || static_cast<size_t>(idx) >= inClubs.size())
		{
			ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_RUN, ATTRIB_RUN_CLUB));
			return false;
		}
		m_Club = inClubs[idx];
	}

	if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_RUN_DIVISION, m_Division) || 0 == m_Division.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_DIVISION));
		return false;
	}

	if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_RUN_LEVEL, m_Level) || 0 == m_Level.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_LEVEL));
		return false;
	}

	// Height is no longer a required attribute (doc ver 8.1)
	inTree->GetAttrib(ATTRIB_RUN_HEIGHT, m_Height);

	if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_RUN_EVENT, m_Event) || 0 == m_Event.length())
	{
		bool bReallyError = true;
		if (inVersion < ARBVersion(15, 0))
		{
			// Fix a data corruption bug from the v3.3.3 release.
			if (m_Division == L"FCAT" && m_Level == L"FCAT")
			{
				m_Event = L"FCAT";
				bReallyError = false;
			}
		}
		if (bReallyError)
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_EVENT));
			return false;
		}
	}

	inTree->GetAttrib(ATTRIB_RUN_SUBNAME, m_SubName);
	inTree->GetAttrib(ATTRIB_RUN_ATHOME, m_isAtHome);

	// This will get the first scoring style to match. So the order of
	// the clubs is critical as we'll search the venues by club order.
	ARBConfigEventPtr pEvent;
	ARBConfigScoringPtr pScoring;
	if (!inClubs.FindEvent(inConfig, m_Event, m_Division, m_Level, m_Date, ioCallback, &pEvent, &pScoring))
		return false;

	for (int i = 0; i < inTree->GetElementCount(); ++i)
	{
		ElementNodePtr element = inTree->GetElementNode(i);
		if (!element)
			continue;
		wxString const& name = element->GetName();
		if (name == TREE_CONDITIONS)
		{
			m_Conditions = element->GetValue();
		}
		else if (name == TREE_JUDGE)
		{
			m_Judge = element->GetValue();
		}
		else if (name == TREE_HANDLER)
		{
			m_Handler = element->GetValue();
		}
		else if (name == TREE_PARTNER)
		{
			// Ignore any errors...
			m_Partners.Load(inConfig, element, inVersion, ioCallback);
		}
		else if (
			name == TREE_BY_TIME || name == TREE_BY_OPENCLOSE || name == TREE_BY_POINTS || name == TREE_BY_SPEED
			|| name == TREE_BY_PASS)
		{
			// Ignore any errors...
			m_Scoring.Load(inConfig.GetVersion(), pEvent, pScoring, element, inVersion, ioCallback);
		}
		else if (name == TREE_PLACEMENT)
		{
			// If we have a placement element, ignore any missing errors.
			// When we save, we save if place>0 OR attribQ.
			wxString attrib;
			if (ARBAttribLookup::Found == element->GetAttrib(ATTRIB_PLACEMENT_Q, attrib) || 0 == attrib.length())
			{
				m_Q.Load(attrib, inVersion, ioCallback);
			}
			element->GetAttrib(ATTRIB_PLACEMENT_PLACE, m_Place);
			element->GetAttrib(ATTRIB_PLACEMENT_INCLASS, m_InClass);
			element->GetAttrib(ATTRIB_PLACEMENT_DOGSQD, m_DogsQd);
			for (int idx = 0; idx < element->GetElementCount(); ++idx)
			{
				ElementNodePtr subElement = element->GetElementNode(idx);
				if (!subElement)
					continue;
				if (subElement->GetName() == TREE_PLACEMENT_OTHERPOINTS)
				{
					m_OtherPoints.Load(inConfig, subElement, inVersion, ioCallback);
				}
			}
		}
		else if (name == TREE_NOTES)
		{
			// Ignore any errors...
			m_Notes.Load(inConfig, element, inVersion, ioCallback);
		}
		else if (name == TREE_REF_RUN)
		{
			// Ignore any errors...
			m_RefRuns.Load(inConfig, element, inVersion, ioCallback);
		}
		else if (name == TREE_RUN_LINK)
		{
			m_Links.insert(element->GetValue());
		}
	}

	// Fix improperly recorded data for FCAT.
	if (inVersion < ARBVersion(15, 1) && m_Division == L"FCAT" && m_Level == L"FCAT")
	{
		m_Height.clear();
		m_Judge.clear();
		m_Handler.clear();
		m_InClass = -1;
		m_DogsQd = -1;
	}
	return true;
}


bool ARBDogRun::Save(ElementNodePtr const& ioTree, ARBDogTrial const* pTrial, ARBConfig const& inConfig) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr run = ioTree->AddElementNode(TREE_RUN);
	run->AddAttrib(ATTRIB_RUN_DATE, m_Date);
	{
		// Note: If we're copy/pasting, pTrial is null.
		size_t index = 0;
		if (pTrial && pTrial->GetClubs().FindClubIndex(m_Club, index))
		{
			// Only save if index is not zero.
			if (0 < index)
				run->AddAttrib(ATTRIB_RUN_CLUB, static_cast<short>(index));
		}
	}
	run->AddAttrib(ATTRIB_RUN_DIVISION, m_Division);
	run->AddAttrib(ATTRIB_RUN_LEVEL, m_Level);
	run->AddAttrib(ATTRIB_RUN_HEIGHT, m_Height);
	run->AddAttrib(ATTRIB_RUN_EVENT, m_Event);
	if (0 < m_SubName.length())
		run->AddAttrib(ATTRIB_RUN_SUBNAME, m_SubName);
	if (m_isAtHome)
		run->AddAttrib(ATTRIB_RUN_ATHOME, m_isAtHome);
	if (0 < m_Conditions.length())
	{
		ElementNodePtr element = run->AddElementNode(TREE_CONDITIONS);
		element->SetValue(m_Conditions);
	}
	if (0 < m_Judge.length())
		run->AddElementNode(TREE_JUDGE)->SetValue(m_Judge);
	if (0 < m_Handler.length())
		run->AddElementNode(TREE_HANDLER)->SetValue(m_Handler);
	if (!m_Partners.Save(run))
		return false;
	if (!m_Scoring.Save(run))
		return false;

	if (0 < m_Place || Q::UNK != m_Q)
	{
		ElementNodePtr element = run->AddElementNode(TREE_PLACEMENT);
		m_Q.Save(element, ATTRIB_PLACEMENT_Q);
		element->AddAttrib(ATTRIB_PLACEMENT_PLACE, m_Place);
		if (0 <= m_InClass)
			element->AddAttrib(ATTRIB_PLACEMENT_INCLASS, m_InClass);
		if (0 <= m_DogsQd)
			element->AddAttrib(ATTRIB_PLACEMENT_DOGSQD, m_DogsQd);

		// This data is exclusively for other programs to use. ARB does not.
		if (pTrial)
		{
			ARBConfigScoringPtr pScoring;
			if (GetClub())
				inConfig.GetVenues().FindEvent(
					GetClub()->GetVenue(),
					GetEvent(),
					GetDivision(),
					GetLevel(),
					GetDate(),
					nullptr,
					&pScoring);
			if (pScoring)
			{
				element->AddAttrib(ATTRIB_PLACEMENT_SCORE_POINTS, GetScore(pScoring));
				if (m_Q.Qualified())
				{
					element->AddAttrib(ATTRIB_PLACEMENT_TITLE_POINTS, GetTitlePoints(pScoring));
					if (pScoring->HasSpeedPts())
						element->AddAttrib(ATTRIB_PLACEMENT_SPEED_POINTS, GetSpeedPoints(pScoring));
				}
			}
		}

		if (!m_OtherPoints.Save(element))
			return false;
	}

	if (!m_Notes.Save(run))
		return false;
	if (!m_RefRuns.Save(run))
		return false;
	for (ARBDogRunLinks::const_iterator iterLink = m_Links.begin(); iterLink != m_Links.end(); ++iterLink)
	{
		if (0 < (*iterLink).length())
		{
			ElementNodePtr element = run->AddElementNode(TREE_RUN_LINK);
			element->SetValue(*iterLink);
		}
	}
	return true;
}


int ARBDogRun::NumOtherPointsInUse(wxString const& inOther) const
{
	int count = 0;
	for (ARBDogRunOtherPointsList::const_iterator iter = m_OtherPoints.begin(); iter != m_OtherPoints.end(); ++iter)
	{
		if ((*iter)->GetName() == inOther)
			++count;
	}
	return count;
}


int ARBDogRun::RenameOtherPoints(wxString const& inOldName, wxString const& inNewName)
{
	int count = 0;
	for (ARBDogRunOtherPointsList::iterator iter = m_OtherPoints.begin(); iter != m_OtherPoints.end(); ++iter)
	{
		if ((*iter)->GetName() == inOldName)
		{
			++count;
			(*iter)->SetName(inNewName);
		}
	}
	return count;
}


int ARBDogRun::DeleteOtherPoints(wxString const& inName)
{
	wxString name(inName);
	int count = 0;
	for (ARBDogRunOtherPointsList::iterator iter = m_OtherPoints.begin(); iter != m_OtherPoints.end();)
	{
		if ((*iter)->GetName() == name)
		{
			++count;
			iter = m_OtherPoints.erase(iter);
		}
		else
			++iter;
	}
	return count;
}


short ARBDogRun::GetSpeedPoints(ARBConfigScoringPtr const& inScoring) const
{
	short pts = 0;
	if (inScoring && inScoring->HasSpeedPts())
	{
		if (GetQ().Qualified())
		{
			double time = m_Scoring.GetTime();
			double sct = m_Scoring.GetSCT();
			if (0.0 < time && 0.0 < sct)
			{
				double diff = sct - time;
				pts = static_cast<short>(diff);
				if (0 > pts)
					pts = 0;
				if (0 < GetPlace())
				{
					double mult = 0.0;
					if (inScoring->GetPlaceInfo().GetPlaceInfo(GetPlace(), mult))
					{
						// Compute the multiplier for the given place.
						pts = static_cast<short>(pts * mult);
					}
					else if (inScoring->GetPlaceInfo().GetPlaceInfo(0, mult))
					{
						// If the specified place wasn't found, see if there is
						// a special '0' place. This acts as an 'everything else'
						pts = static_cast<short>(pts * mult);
					}
				}
			}
		}
	}
	return pts;
}


double ARBDogRun::GetTitlePoints(ARBConfigScoringPtr const& inScoring, bool* outClean) const
{
	double pts = 0.0;
	if (outClean)
		*outClean = false;
	double bonusTitlePts = inScoring->HasBonusTitlePts() ? m_Scoring.GetBonusTitlePts() : 0.0;
	bool isTourney = false;
	if (GetClub())
		isTourney = (GetClub()->GetVenue() == L"USDAA" && GetLevel() == L"Tournament");

	switch (m_Scoring.GetType())
	{
	case ARBScoringType::Unknown:
		break;

	case ARBScoringType::ByTime:
	case ARBScoringType::BySpeed:
	{
		double score = m_Scoring.GetCourseFaults() + m_Scoring.GetTimeFaults(inScoring);
		if (ARBDouble::equal(score, 0))
		{
			if (outClean)
				*outClean = true;
		}
		if (ARBScoringStyle::TimePlusFaults == inScoring->GetScoringStyle())
		{
			if (!(inScoring && inScoring->QsMustBeClean() && score > 0.0))
			{
				// If SCT is 0, don't compute anything.
				if (0.0 < m_Scoring.GetSCT())
				{
					score += m_Scoring.GetTime();
					// Adjust the 'score' to the number of "faults" (total over SCT)
					// This allows DOCNA's Challenge to work - it's T+F with 12 time faults allowed
					score -= m_Scoring.GetSCT();
					// If negative, run was faster than SCT
					if (0.0 > score)
						score = 0.0;
				}
				bool bCompute = true;
				if (inScoring->ComputeTitlingPointsRawFaults())
				{
					score = m_Scoring.GetCourseFaults() + m_Scoring.GetTimeFaults(inScoring);

					// If using raw faults for determining title points, this implies that
					// the run must be under SCT.
					if (m_Scoring.GetTime() + score > m_Scoring.GetSCT())
						bCompute = false;
				}
				if (bCompute)
					pts = inScoring->GetTitlePoints().GetTitlePoints(
							  score,
							  m_Scoring.GetTime(),
							  m_Scoring.GetSCT(),
							  GetPlace(),
							  GetInClass(),
							  GetDate(),
							  isTourney,
							  m_isAtHome)
						  + bonusTitlePts;
			}
		}
		else
		{
			pts = inScoring->GetTitlePoints().GetTitlePoints(
					  score,
					  m_Scoring.GetTime(),
					  m_Scoring.GetSCT(),
					  GetPlace(),
					  GetInClass(),
					  GetDate(),
					  isTourney,
					  m_isAtHome)
				  + bonusTitlePts;
		}
	}
	break;

	case ARBScoringType::ByOpenClose:
		if ((m_Scoring.GetNeedOpenPts() <= m_Scoring.GetOpenPts()
			 && m_Scoring.GetNeedClosePts() <= m_Scoring.GetClosePts())
			// Allows for USDAA tournament gambles
			|| (0 == m_Scoring.GetNeedClosePts()
				&& m_Scoring.GetNeedOpenPts() <= m_Scoring.GetOpenPts() + m_Scoring.GetClosePts()))
		{
			double timeFaults = 0.0;
			if (inScoring->ComputeTimeFaultsUnder() || inScoring->ComputeTimeFaultsOver())
			{
				timeFaults = m_Scoring.GetTimeFaults(inScoring);
				if (0.0 < timeFaults && inScoring->SubtractTimeFaultsFromScore())
				{
					// If time faults are being subtracted from the score,
					// recompute if we have enough points. If so, just pretend
					// there are no time faults.
					if (static_cast<double>(m_Scoring.GetNeedOpenPts() + m_Scoring.GetNeedClosePts())
						<= GetScore(inScoring))
						timeFaults = 0.0;
				}
			}
			if (outClean)
				*outClean = true;
			pts = inScoring->GetTitlePoints().GetTitlePoints(
					  timeFaults,
					  m_Scoring.GetTime(),
					  m_Scoring.GetSCT(),
					  GetPlace(),
					  GetInClass(),
					  GetDate(),
					  isTourney,
					  m_isAtHome)
				  + bonusTitlePts;
		}
		break;

	case ARBScoringType::ByPoints:
		if (m_Scoring.GetNeedOpenPts() <= m_Scoring.GetOpenPts())
		{
			double timeFaults = 0.0;
			if (inScoring->ComputeTimeFaultsUnder() || inScoring->ComputeTimeFaultsOver())
			{
				timeFaults = m_Scoring.GetTimeFaults(inScoring);
				if (0.0 < timeFaults && inScoring->SubtractTimeFaultsFromScore())
				{
					// If time faults are being subtracted from the score,
					// recompute if we have enough points. If so, just pretend
					// there are no time faults.
					if (static_cast<double>(m_Scoring.GetNeedOpenPts()) <= GetScore(inScoring))
						timeFaults = 0.0;
				}
			}
			if (outClean)
				*outClean = true;
			pts = inScoring->GetTitlePoints().GetTitlePoints(
					  timeFaults,
					  m_Scoring.GetTime(),
					  m_Scoring.GetSCT(),
					  GetPlace(),
					  GetInClass(),
					  GetDate(),
					  isTourney,
					  m_isAtHome)
				  + bonusTitlePts;
		}
		break;

	case ARBScoringType::ByPass:
		if (m_Q.Qualified())
		{
			pts = inScoring->GetTitlePoints().GetTitlePoints(
				0.0,
				m_Scoring.GetTime(),
				m_Scoring.GetSCT(),
				GetPlace(),
				GetInClass(),
				GetDate(),
				isTourney,
				m_isAtHome);
		}
		break;
	}

	return pts;
}


double ARBDogRun::GetLifetimePoints(ARBConfigScoringPtr const& inScoring, wxString const& inLifetimeName) const
{
	double pts = 0.0;
	double bonusTitlePts = inScoring->HasBonusTitlePts() ? m_Scoring.GetBonusTitlePts() : 0.0;
	switch (m_Scoring.GetType())
	{
	case ARBScoringType::Unknown:
		break;
	case ARBScoringType::ByTime:
	case ARBScoringType::BySpeed:
	{
		double score = m_Scoring.GetCourseFaults() + m_Scoring.GetTimeFaults(inScoring);
		if (ARBScoringStyle::TimePlusFaults == inScoring->GetScoringStyle())
		{
			if (!(inScoring && inScoring->QsMustBeClean() && score > 0.0))
			{
				// If SCT is 0, don't compute anything.
				if (0.0 < m_Scoring.GetSCT())
				{
					score += m_Scoring.GetTime();
					// Adjust the 'score' to the number of "faults" (total over SCT)
					// This allows DOCNA's Challenge to work - it's T+F with 12 time faults allowed
					score -= m_Scoring.GetSCT();
					// If negative, run was faster than SCT
					if (0.0 > score)
						score = 0.0;
				}
				pts = inScoring->GetLifetimePoints().GetLifetimePoints(inLifetimeName, score, GetSpeedPoints(inScoring))
					  + bonusTitlePts;
			}
		}
		else
		{
			pts = inScoring->GetLifetimePoints().GetLifetimePoints(inLifetimeName, score, GetSpeedPoints(inScoring))
				  + bonusTitlePts;
		}
	}
	break;
	case ARBScoringType::ByOpenClose:
		if ((m_Scoring.GetNeedOpenPts() <= m_Scoring.GetOpenPts()
			 && m_Scoring.GetNeedClosePts() <= m_Scoring.GetClosePts())
			// Allows for USDAA tournament gambles
			|| (0 == m_Scoring.GetNeedClosePts()
				&& m_Scoring.GetNeedOpenPts() <= m_Scoring.GetOpenPts() + m_Scoring.GetClosePts()))
		{
			double timeFaults = 0.0;
			if (inScoring->ComputeTimeFaultsUnder() || inScoring->ComputeTimeFaultsOver())
			{
				timeFaults = m_Scoring.GetTimeFaults(inScoring);
				if (0.0 < timeFaults && inScoring->SubtractTimeFaultsFromScore())
				{
					// If time faults are being subtracted from the score,
					// recompute if we have enough points. If so, just pretend
					// there are no time faults.
					if (static_cast<double>(m_Scoring.GetNeedOpenPts() + m_Scoring.GetNeedClosePts())
						<= GetScore(inScoring))
						timeFaults = 0.0;
				}
			}
			pts = inScoring->GetLifetimePoints().GetLifetimePoints(
					  inLifetimeName,
					  timeFaults,
					  GetSpeedPoints(inScoring))
				  + bonusTitlePts;
		}
		break;
	case ARBScoringType::ByPoints:
		if (m_Scoring.GetNeedOpenPts() <= m_Scoring.GetOpenPts())
		{
			double timeFaults = 0.0;
			if (inScoring->ComputeTimeFaultsUnder() || inScoring->ComputeTimeFaultsOver())
			{
				timeFaults = m_Scoring.GetTimeFaults(inScoring);
				if (0.0 < timeFaults && inScoring->SubtractTimeFaultsFromScore())
				{
					// If time faults are being subtracted from the score,
					// recompute if we have enough points. If so, just pretend
					// there are no time faults.
					if (static_cast<double>(m_Scoring.GetNeedOpenPts()) <= GetScore(inScoring))
						timeFaults = 0.0;
				}
			}
			pts = inScoring->GetLifetimePoints().GetLifetimePoints(
					  inLifetimeName,
					  timeFaults,
					  GetSpeedPoints(inScoring))
				  + bonusTitlePts;
		}
		break;
	case ARBScoringType::ByPass:
		if (m_Q.Qualified())
		{
			pts = inScoring->GetLifetimePoints().GetLifetimePoints(inLifetimeName, 0.0, GetSpeedPoints(inScoring))
				  + bonusTitlePts;
		}
		break;
	}
	return pts;
}


double ARBDogRun::GetPlacementPoints(ARBConfigScoringPtr const& inScoring) const
{
	double pts = 0.0;
	if (!inScoring->GetPlacements().GetPlaceInfo(GetPlace(), pts))
		pts = 0.0;
	return pts;
}


double ARBDogRun::GetScore(ARBConfigScoringPtr const& inScoring) const
{
	double pts = 0.0;
	switch (m_Scoring.GetType())
	{
	case ARBScoringType::Unknown:
		break;
	case ARBScoringType::ByTime:
	case ARBScoringType::BySpeed:
		pts = m_Scoring.GetCourseFaults() + m_Scoring.GetTimeFaults(inScoring);
		switch (inScoring->GetScoringStyle())
		{
		case ARBScoringStyle::Unknown:
		case ARBScoringStyle::FaultsThenTime:
		case ARBScoringStyle::OCScoreThenTime:
		case ARBScoringStyle::ScoreThenTime:
		case ARBScoringStyle::TimeNoPlaces:
		case ARBScoringStyle::PassFail:
			break;
		case ARBScoringStyle::TimePlusFaults:
			pts += m_Scoring.GetTime();
			break;
		case ARBScoringStyle::Faults100ThenTime:
			pts = 100 - pts;
			if (0 > pts)
				pts = 0;
			break;
		case ARBScoringStyle::Faults200ThenTime:
			pts = 200 - pts;
			if (0 > pts)
				pts = 0;
			break;
		}
		break;
	case ARBScoringType::ByOpenClose:
		pts = m_Scoring.GetOpenPts() + m_Scoring.GetClosePts() - m_Scoring.GetCourseFaults();
		if (inScoring->SubtractTimeFaultsFromScore())
			pts -= m_Scoring.GetTimeFaults(inScoring);
		break;
	case ARBScoringType::ByPoints:
		pts = m_Scoring.GetOpenPts() - m_Scoring.GetCourseFaults();
		if (inScoring->SubtractTimeFaultsFromScore())
			pts -= m_Scoring.GetTimeFaults(inScoring);
		break;
	case ARBScoringType::ByPass:
		break;
	}
	return pts;
}


size_t ARBDogRun::GetMultiQs(std::vector<ARBConfigMultiQPtr>& outMultiQs) const
{
	outMultiQs.clear();
	for (std::set<ARBConfigMultiQPtr>::const_iterator i = m_pMultiQs.begin(); i != m_pMultiQs.end(); ++i)
	{
		outMultiQs.push_back(*i);
	}
	return outMultiQs.size();
}


void ARBDogRun::ClearMultiQs()
{
	m_pMultiQs.clear();
}


void ARBDogRun::AddMultiQ(ARBConfigMultiQPtr const& inMultiQ)
{
	m_pMultiQs.insert(inMultiQ);
}


size_t ARBDogRun::GetLinks(std::set<wxString>& outLinks) const
{
	outLinks.clear();
	outLinks = m_Links;
	return outLinks.size();
}


bool ARBDogRun::HasLink(wxString const& inLink) const
{
	return m_Links.find(inLink) != m_Links.end();
}


void ARBDogRun::AddLink(wxString const& inLink)
{
	m_Links.insert(inLink);
}


void ARBDogRun::RemoveLink(wxString const& inLink)
{
	ARBDogRunLinks::iterator iter = m_Links.find(inLink);
	if (iter != m_Links.end())
		m_Links.erase(iter);
}

/////////////////////////////////////////////////////////////////////////////

bool ARBDogRunList::Load(
	ARBConfig const& inConfig,
	ARBDogClubList const& inClubs,
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	ARBDogRunPtr thing(ARBDogRun::New());
	if (!thing->Load(inConfig, inClubs, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


bool ARBDogRunList::Save(ElementNodePtr const& ioTree, ARBDogTrial const* pTrial, ARBConfig const& inConfig) const
{
	assert(pTrial);
	for (ARBVector<ARBDogRunPtr>::const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (!(*iter)->Save(ioTree, pTrial, inConfig))
			return false;
	}
	return true;
}


void ARBDogRunList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), [](ARBDogRunPtr const& one, ARBDogRunPtr const& two) {
		return one->GetDate() < two->GetDate();
	});
}


ARBDate ARBDogRunList::GetStartDate() const
{
	ARBDate date;
	if (0 < size())
	{
		ARBDate date1 = (*begin())->GetDate();
		ARBDate date2 = (*(end() - 1))->GetDate();
		date = date1 < date2 ? date1 : date2;
	}
	return date;
}


ARBDate ARBDogRunList::GetEndDate() const
{
	ARBDate date;
	if (0 < size())
	{
		ARBDate date1 = (*begin())->GetDate();
		ARBDate date2 = (*(end() - 1))->GetDate();
		date = date1 > date2 ? date1 : date2;
	}
	return date;
}


bool ARBDogRunList::AddRun(ARBDogRunPtr const& inRun)
{
	bool bAdded = false;
	if (inRun)
	{
		bAdded = true;
		push_back(inRun);
	}
	return bAdded;
}


bool ARBDogRunList::DeleteRun(ARBDogRunPtr const& inRun)
{
	if (inRun)
	{
		for (iterator iter = begin(); iter != end(); ++iter)
		{
			if ((*iter) && *(*iter) == *inRun)
			{
				erase(iter);
				return true;
			}
		}
	}
	return false;
}

} // namespace ARB
} // namespace dconSoft
