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
 * @note The default date is considered transitory. It is not saved once a
 * run is created. On load, a trial will set the default date to the first
 * run date in case the last run is deleted. This does mean that a trial
 * could end up with a different default date after creating a trial with a
 * run, saving it, reloading, and deleting that run. This is by-design.
 *
 * Revision History
 * 2013-05-25 Implement a default date for a trial.
 * 2012-09-09 Added 'titlePts' to 'Placement'.
 * 2012-02-16 Fixed an issue in co-sanctioned trial detection.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-07-05 Fixed sort predicate so it is really stable.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-09-27 Fixed sorting of trials.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-10 Only sort runs one way, the UI handles everything else.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-12-27 Changed FindEvent to take a date.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-07-16 Allow the code to keep processing after errors are found.
 * 2003-06-23 Added note field to trials.
 */

#include "stdafx.h"
#include "ARB/ARBDogTrial.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBCalendar.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/ARBDate.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class ARBDogTrial_concrete : public ARBDogTrial
	{
	public:
		ARBDogTrial_concrete() {}
		ARBDogTrial_concrete(ARBCalendar const& inCal)
			: ARBDogTrial(inCal)
		{
		}
		ARBDogTrial_concrete(ARBDogTrial const& rhs)
			: ARBDogTrial(rhs)
		{
		}
	};
};


ARBDogTrialPtr ARBDogTrial::New()
{
	return std::make_shared<ARBDogTrial_concrete>();
}


ARBDogTrialPtr ARBDogTrial::New(ARBCalendar const& inCal)
{
	return std::make_shared<ARBDogTrial_concrete>(inCal);
}


ARBDogTrial::ARBDogTrial()
	: m_Location()
	, m_Note()
	, m_DefaultDate()
	, m_Verified(false)
	, m_Clubs()
	, m_Runs()
{
}


ARBDogTrial::ARBDogTrial(ARBCalendar const& inCal)
	: m_Location(inCal.GetLocation())
	, m_Note(inCal.GetNote())
	, m_DefaultDate(inCal.GetStartDate())
	, m_Verified(false)
	, m_Clubs()
	, m_Runs()
{	
	m_Clubs.AddClub(inCal.GetClub(), inCal.GetVenue());
}


ARBDogTrial::ARBDogTrial(ARBDogTrial const& rhs)
	: m_Location(rhs.m_Location)
	, m_Note(rhs.m_Note)
	, m_DefaultDate(rhs.m_DefaultDate)
	, m_Verified(rhs.m_Verified)
	, m_Clubs()
	, m_Runs()
{
	rhs.m_Clubs.Clone(m_Clubs);
	rhs.m_Runs.Clone(m_Runs);
}


ARBDogTrial::~ARBDogTrial()
{
}


ARBDogTrialPtr ARBDogTrial::Clone() const
{
	return std::make_shared<ARBDogTrial_concrete>(*this);
}


ARBDogTrial& ARBDogTrial::operator=(ARBDogTrial const& rhs)
{
	if (this != &rhs)
	{
		m_Location = rhs.m_Location;
		m_Note = rhs.m_Note;
		m_DefaultDate = rhs.m_DefaultDate;
		m_Verified = rhs.m_Verified;
		rhs.m_Clubs.Clone(m_Clubs);
		rhs.m_Runs.Clone(m_Runs);
	}
	return *this;
}


bool ARBDogTrial::operator==(ARBDogTrial const& rhs) const
{
	return m_Location == rhs.m_Location
		&& m_Note == rhs.m_Note
		&& m_DefaultDate == rhs.m_DefaultDate
		&& m_Verified == rhs.m_Verified
		&& m_Clubs == rhs.m_Clubs
		&& m_Runs == rhs.m_Runs;
}


std::wstring ARBDogTrial::GetGenericName() const
{
	std::wostringstream name;
	ARBDogClubPtr pClub;
	if (m_Clubs.GetPrimaryClub(&pClub))
	{
		name << pClub->GetVenue() << L" ";
	}
	name << m_Location;
	return name.str();
}


size_t ARBDogTrial::GetSearchStrings(std::set<std::wstring>& ioStrings) const
{
	size_t nItems = 0;

	if (0 < m_Location.length())
	{
		ioStrings.insert(m_Location);
		++nItems;
	}

	if (0 < m_Note.length())
	{
		ioStrings.insert(m_Note);
		++nItems;
	}

	nItems += m_Clubs.GetSearchStrings(ioStrings);

	return nItems;
}


bool ARBDogTrial::Load(
		ARBConfig const& inConfig,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_TRIAL)
		return false;
	inTree->GetAttrib(ATTRIB_TRIAL_DEFAULT_DATE, m_DefaultDate);
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_TRIAL_VERIFIED, m_Verified))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_TRIAL, ATTRIB_TRIAL_VERIFIED, Localization()->ValidValuesBool().c_str()));
		return false;
	}
	for (int i = 0; i < inTree->GetElementCount(); ++i)
	{
		ElementNodePtr element = inTree->GetElementNode(i);
		if (!element)
			continue;
		if (element->GetName() == TREE_LOCATION)
		{
			m_Location = element->GetValue();
		}
		else if (element->GetName() == TREE_NOTE)
		{
			m_Note = element->GetValue();
		}
		else if (element->GetName() == TREE_CLUB)
		{
			// Ignore any errors...
			m_Clubs.Load(inConfig, element, inVersion, ioCallback);
		}
		// Clubs should all be loaded first. We're not going to validate that
		// though. If it's not true, someone has been messing with the file
		// directly and the ARBRunList::Load will fail.
		else if (element->GetName() == TREE_RUN)
		{
			// Ignore any errors...
			m_Runs.Load(inConfig, m_Clubs, element, inVersion, ioCallback);
		}
	}
	SetMultiQs(inConfig);
	m_Runs.sort();
	// Default date is only saved when there are no runs. Set this in case
	// all the runs are deleted.
	if (0 < m_Runs.size())
		m_DefaultDate = m_Runs.GetStartDate();
	return true;
}


bool ARBDogTrial::Save(
		ElementNodePtr ioTree,
		ARBConfig const& inConfig) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr trial = ioTree->AddElementNode(TREE_TRIAL);
	if (m_DefaultDate.IsValid() && 0 == m_Runs.size())
		trial->AddAttrib(ATTRIB_TRIAL_DEFAULT_DATE, m_DefaultDate);
	if (m_Verified) // Default is no
		trial->AddAttrib(ATTRIB_TRIAL_VERIFIED, m_Verified);
	if (0 < m_Location.length())
	{
		ElementNodePtr element = trial->AddElementNode(TREE_LOCATION);
		element->SetValue(m_Location);
	}
	if (0 < m_Note.length())
	{
		ElementNodePtr element = trial->AddElementNode(TREE_NOTE);
		element->SetValue(m_Note);
	}
	if (!m_Clubs.Save(trial))
		return false;
	if (!m_Runs.Save(trial, this, inConfig))
		return false;
	return true;
}


void ARBDogTrial::SetMultiQs(ARBConfig const& inConfig)
{
	// First clear all settings.
	ARBDate date;
	for (date = m_Runs.GetStartDate();
		date <= m_Runs.GetEndDate();
		++date)
	{
		for (ARBDogRunList::iterator iterRun = m_Runs.begin(); iterRun != m_Runs.end(); ++iterRun)
		{
			(*iterRun)->ClearMultiQs();
		}
	}

	// Now get some needed info...
	ARBDogClubPtr pClub;
	if (!GetClubs().GetPrimaryClub(&pClub))
		return;
	ARBConfigVenuePtr pVenue;
	if (!inConfig.GetVenues().FindVenue(pClub->GetVenue(), &pVenue))
		return;
	if (0 == pVenue->GetMultiQs().size())
		return;

	// Then for each day in the trial, look for multiQs.
	for (date = m_Runs.GetStartDate();
		date <= m_Runs.GetEndDate();
		++date)
	{
		std::vector<ARBDogRunPtr> runs;
		for (ARBDogRunList::iterator iterRun = m_Runs.begin(); iterRun != m_Runs.end(); ++iterRun)
		{
			ARBDogRunPtr pRun = (*iterRun);
			if (pRun->GetDate() == date
			&& pRun->GetQ().Qualified())
			{
				runs.push_back(pRun);
			}
		}
		// Now, see if any combo of these runs matches a multiQ config.
		if (1 < runs.size())
		{
			for (ARBConfigMultiQList::iterator iterM = pVenue->GetMultiQs().begin();
				iterM != pVenue->GetMultiQs().end();
				++iterM)
			{
				ARBConfigMultiQPtr pMultiQ = *iterM;
				std::vector<ARBDogRunPtr> matchedRuns;
				if (pMultiQ->Match(runs, matchedRuns))
				{
					for (std::vector<ARBDogRunPtr>::iterator iter = matchedRuns.begin();
						iter != matchedRuns.end();
						++iter)
					{
						(*iter)->AddMultiQ(pMultiQ);
					}
				}
			}
		}
	}
}


short ARBDogTrial::GetSpeedPoints(
		ARBConfig const& inConfig,
		std::wstring const& inDiv,
		std::wstring const& inLevel) const
{
	if (!GetClubs().GetPrimaryClub())
		return 0;
	short speed = 0;
	for (ARBDogRunList::const_iterator iterRun = m_Runs.begin(); iterRun != m_Runs.end(); ++iterRun)
	{
		ARBDogRunPtr pRun = (*iterRun);
		if (pRun->GetDivision() == inDiv
		&& pRun->GetLevel() == inLevel)
		{
			ARBConfigScoringPtr pScoring;
			if (inConfig.GetVenues().FindEvent(
				GetClubs().GetPrimaryClubVenue(),
				pRun->GetEvent(),
				pRun->GetDivision(),
				pRun->GetLevel(),
				pRun->GetDate(),
				nullptr,
				&pScoring))
			{
				speed = speed + pRun->GetSpeedPoints(pScoring);
			}
		}
	}
	return speed;
}


bool ARBDogTrial::HasVenue(std::wstring const& inVenue) const
{
	for (ARBDogClubList::const_iterator iter = m_Clubs.begin(); iter != m_Clubs.end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue)
			return true;
	}
	return false;
}


ARBDate ARBDogTrial::GetStartDate() const
{
	ARBDate date(m_DefaultDate);
	if (0 < m_Runs.size())
		date = m_Runs.GetStartDate();
	return date;
}


ARBDate ARBDogTrial::GetEndDate() const
{
	ARBDate date(m_DefaultDate);
	if (0 < m_Runs.size())
		date = m_Runs.GetEndDate();
	return date;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBDogTrialList::Load(
		ARBConfig const& inConfig,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBDogTrialPtr thing(ARBDogTrial::New());
	if (!thing->Load(inConfig, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


class SortTrials
{
public:
	SortTrials(bool bDescending) : m_bDescending(bDescending) {}
	bool operator()(ARBDogTrialPtr const& one, ARBDogTrialPtr const& two) const
	{
		int iCompare = 0;
		ARBDate d1 = one->GetStartDate();
		ARBDate d2 = two->GetStartDate();
		if (d1.IsValid() && d2.IsValid())
		{
			if (d1 < d2)
				iCompare = -1;
			else if (d1 > d2)
				iCompare = 1;
		}
		else if (d1.IsValid())
			iCompare = 1;
		else if (d2.IsValid())
			iCompare = -1;
		if (0 == iCompare)
		{
			iCompare = StringUtil::Compare(one->GetGenericName(), two->GetGenericName());
			if (0 == iCompare)
			{
				ARBDogClubPtr club1;
				one->GetClubs().GetPrimaryClub(&club1);
				std::wstring name1, venue1;
				if (club1)
				{
					name1 = club1->GetName();
					venue1 = club1->GetVenue();
				}
				ARBDogClubPtr club2;
				two->GetClubs().GetPrimaryClub(&club2);
				std::wstring name2, venue2;
				if (club2)
				{
					name2 = club2->GetName();
					venue2 = club2->GetVenue();
				}
				iCompare = StringUtil::Compare(name1, name2);
				if (0 == iCompare)
					iCompare = StringUtil::Compare(venue1, venue2);
			}
		}
		return m_bDescending ? iCompare < 0 : iCompare > 0;
	}
private:
	bool m_bDescending;
};


void ARBDogTrialList::sort(bool inDescending)
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortTrials(inDescending));
}


int ARBDogTrialList::NumTrialsInVenue(std::wstring const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		for (ARBDogClubList::const_iterator iterClub = (*iter)->GetClubs().begin(); iterClub != (*iter)->GetClubs().end(); ++iterClub)
		{
			if ((*iterClub)->GetVenue() == inVenue)
			{
				++count;
				// We're counting the number of trials, not clubs.
				break;
			}
		}
	}
	return count;
}


int ARBDogTrialList::RenameVenue(
		std::wstring const& inOldVenue,
		std::wstring const& inNewVenue)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		for (ARBDogClubList::iterator iterClub = (*iter)->GetClubs().begin(); iterClub != (*iter)->GetClubs().end(); ++iterClub)
		{
			if ((*iterClub)->GetVenue() == inOldVenue)
			{
				(*iterClub)->SetVenue(inNewVenue);
				++count;
			}
		}
	}
	return count;
}


int ARBDogTrialList::DeleteVenue(std::wstring const& inVenue)
{
	std::wstring venue(inVenue);
	int count = 0;
	for (iterator iter = begin(); iter != end(); )
	{
		for (ARBDogClubList::iterator iterClub = (*iter)->GetClubs().begin(); iterClub != (*iter)->GetClubs().end(); )
		{
			if ((*iterClub)->GetVenue() == venue)
			{
				iterClub = (*iter)->GetClubs().erase(iterClub);
				++count;
			}
			else
				++iterClub;
		}
		if (0 == (*iter)->GetClubs().size())
			iter = erase(iter);
		else
			++iter;
	}
	return count;
}


int ARBDogTrialList::NumOtherPointsInUse(std::wstring const& inOther) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		for (ARBDogRunList::const_iterator iterRun = (*iter)->GetRuns().begin(); iterRun != (*iter)->GetRuns().end(); ++iterRun)
		{
			count += (*iterRun)->NumOtherPointsInUse(inOther);
		}
	}
	return count;
}


int ARBDogTrialList::RenameOtherPoints(
		std::wstring const& inOldOther,
		std::wstring const& inNewOther)
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		for (ARBDogRunList::const_iterator iterRun = (*iter)->GetRuns().begin(); iterRun != (*iter)->GetRuns().end(); ++iterRun)
		{
			count += (*iterRun)->RenameOtherPoints(inOldOther, inNewOther);
		}
	}
	return count;
}


int ARBDogTrialList::DeleteOtherPoints(std::wstring const& inOther)
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		for (ARBDogRunList::const_iterator iterRun = (*iter)->GetRuns().begin(); iterRun != (*iter)->GetRuns().end(); ++iterRun)
		{
			count += (*iterRun)->DeleteOtherPoints(inOther);
		}
	}
	return count;
}


int ARBDogTrialList::NumMultiHostedTrialsInDivision(
		ARBConfig const& inConfig,
		std::wstring const& inVenue,
		std::wstring const& inDiv) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetClubs().FindVenue(inVenue)
		&& 1 < (*iter)->GetClubs().size())
		{
			std::wstring venue1 = (*iter)->GetClubs()[1]->GetVenue();
			int nDivCount = 0;
			for (ARBDogClubList::const_iterator iterClub = (*iter)->GetClubs().begin(); iterClub != (*iter)->GetClubs().end(); ++iterClub)
			{
				// If all the clubs are in the same venue, it really isn't a
				// multi-hosted trial. (This is really for asca/nadac)
				if ((*iterClub)->GetVenue() == venue1)
					continue;
				ARBConfigVenuePtr pVenue;
				if (inConfig.GetVenues().FindVenue((*iterClub)->GetVenue(), &pVenue))
				{
					if (pVenue->GetDivisions().FindDivision(inDiv))
						++nDivCount;
				}
			}
			if (0 < nDivCount)
				++count;
		}
	}
	return count;
}


int ARBDogTrialList::NumRunsInDivision(
		ARBConfigVenuePtr inVenue,
		std::wstring const& inDiv) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetClubs().FindVenue(inVenue->GetName()))
		{
			for (ARBDogRunList::const_iterator iterRun = (*iter)->GetRuns().begin(); iterRun != (*iter)->GetRuns().end(); ++iterRun)
			{
				if ((*iterRun)->GetDivision() == inDiv)
					++count;
			}
		}
	}
	return count;
}


int ARBDogTrialList::RenameDivision(
		ARBConfigVenuePtr inVenue,
		std::wstring const& inOldDiv,
		std::wstring const& inNewDiv)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetClubs().FindVenue(inVenue->GetName()))
		{
			for (ARBDogRunList::iterator iterRun = (*iter)->GetRuns().begin(); iterRun != (*iter)->GetRuns().end(); ++iterRun)
			{
				if ((*iterRun)->GetDivision() == inOldDiv)
				{
					++count;
					(*iterRun)->SetDivision(inNewDiv);
				}
			}
		}
	}
	return count;
}


int ARBDogTrialList::DeleteDivision(
		ARBConfig const& inConfig,
		std::wstring const& inVenue,
		std::wstring const& inDiv)
{
	std::wstring venue(inVenue);
	std::wstring div(inDiv);
	int count = 0;
	for (iterator iter = begin(); iter != end(); )
	{
		if ((*iter)->GetClubs().FindVenue(venue))
		{
			int nDivCount = 0;
			// First, should we delete the run? If there are multiple hosting
			// clubs we need to look for the division name within each.
			for (ARBDogClubList::const_iterator iterClub = (*iter)->GetClubs().begin(); iterClub != (*iter)->GetClubs().end(); ++iterClub)
			{
				ARBConfigVenuePtr pVenue;
				if (inConfig.GetVenues().FindVenue((*iterClub)->GetVenue(), &pVenue))
				{
					if (pVenue->GetDivisions().FindDivision(div))
						++nDivCount;
				}
			}
			// If more than one hosting club has this division, no need to go
			// across the runs - we're not going to delete anything!
			if (1 == nDivCount)
			{
				for (ARBDogRunList::iterator iterRun = (*iter)->GetRuns().begin(); iterRun != (*iter)->GetRuns().end(); )
				{
					if ((*iterRun)->GetDivision() == div)
					{
						++count;
						iterRun = (*iter)->GetRuns().erase(iterRun);
					}
					else
						++iterRun;
				}
			}
		}
		if (0 == (*iter)->GetRuns().size())
			iter = erase(iter);
		else
			++iter;
	}
	return count;
}


int ARBDogTrialList::NumLevelsInUse(
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inLevel) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetClubs().FindVenue(inVenue))
		{
			for (ARBDogRunList::iterator iterRun = (*iter)->GetRuns().begin(); iterRun != (*iter)->GetRuns().end(); ++iterRun)
			{
				if ((*iterRun)->GetDivision() == inDiv
				&& (*iterRun)->GetLevel() == inLevel)
					++count;
			}
		}
	}
	return count;
}


int ARBDogTrialList::RenameLevel(
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inOldLevel,
		std::wstring const& inNewLevel)
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetClubs().FindVenue(inVenue))
		{
			for (ARBDogRunList::iterator iterRun = (*iter)->GetRuns().begin(); iterRun != (*iter)->GetRuns().end(); ++iterRun)
			{
				if ((*iterRun)->GetDivision() == inDiv
				&& (*iterRun)->GetLevel() == inOldLevel)
				{
					++count;
					(*iterRun)->SetLevel(inNewLevel);
				}
			}
		}
	}
	return count;
}


int ARBDogTrialList::DeleteLevel(
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inLevel)
{
	std::wstring venue(inVenue);
	std::wstring div(inDiv);
	std::wstring level(inLevel);
	int count = 0;
	for (iterator iter = begin(); iter != end(); )
	{
		if ((*iter)->GetClubs().FindVenue(venue))
		{
			for (ARBDogRunList::iterator iterRun = (*iter)->GetRuns().begin(); iterRun != (*iter)->GetRuns().end(); )
			{
				if ((*iterRun)->GetDivision() == div
				&& (*iterRun)->GetLevel() == level)
				{
					++count;
					iterRun = (*iter)->GetRuns().erase(iterRun);
				}
				else
					++iterRun;
			}
		}
		if (0 == (*iter)->GetRuns().size())
			iter = erase(iter);
		else
			++iter;
	}
	return count;
}


int ARBDogTrialList::NumEventsInUse(
		std::wstring const& inVenue,
		std::wstring const& inEvent) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetClubs().FindVenue(inVenue))
		{
			for (ARBDogRunList::iterator iterRun = (*iter)->GetRuns().begin(); iterRun != (*iter)->GetRuns().end(); ++iterRun)
			{
				if ((*iterRun)->GetEvent() == inEvent)
					++count;
			}
		}
	}
	return count;
}


int ARBDogTrialList::RenameEvent(
		std::wstring const& inVenue,
		std::wstring const& inOldEvent,
		std::wstring const& inNewEvent)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetClubs().FindVenue(inVenue))
		{
			for (ARBDogRunList::iterator iterRun = (*iter)->GetRuns().begin(); iterRun != (*iter)->GetRuns().end(); ++iterRun)
			{
				if ((*iterRun)->GetEvent() == inOldEvent)
				{
					++count;
					(*iterRun)->SetEvent(inNewEvent);
				}
			}
		}
	}
	return count;
}


int ARBDogTrialList::DeleteEvent(
		std::wstring const& inVenue,
		std::wstring const& inEvent)
{
	std::wstring venue(inVenue);
	std::wstring pEvent(inEvent);
	int count = 0;
	for (iterator iter = begin(); iter != end(); )
	{
		if ((*iter)->GetClubs().FindVenue(venue))
		{
			for (ARBDogRunList::iterator iterRun = (*iter)->GetRuns().begin(); iterRun != (*iter)->GetRuns().end(); )
			{
				if ((*iterRun)->GetEvent() == pEvent)
				{
					++count;
					iterRun = (*iter)->GetRuns().erase(iterRun);
				}
				else
					++iterRun;
			}
		}
		if (0 == (*iter)->GetRuns().size())
			iter = erase(iter);
		else
			++iter;
	}
	return count;
}


bool ARBDogTrialList::AddTrial(ARBDogTrialPtr inTrial)
{
	bool bAdded = false;
	if (inTrial)
	{
		bAdded = true;
		push_back(inTrial);
	}
	return bAdded;
}


bool ARBDogTrialList::AddTrial(
		ARBDogTrialPtr inTrial,
		bool inDescending)
{
	bool bAdded = AddTrial(inTrial);
	if (bAdded)
		sort(inDescending);
	return bAdded;
}


bool ARBDogTrialList::DeleteTrial(ARBDogTrialPtr inTrial)
{
	if (inTrial)
	{
		for (iterator iter = begin(); iter != end(); ++iter)
		{
			if ((*iter) && *(*iter) == *inTrial)
			{
				erase(iter);
				return true;
			}
		}
	}
	return false;
}
