/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * @li 2003-07-16 DRC Allow the code to keep processing after errors are found.
 * @li 2003-06-23 DRC Added note field to trials.
 */

#include "StdAfx.h"
#include "ARBDogTrial.h"
#include <algorithm>

#include "ARBAgilityRecordBook.h"
#include "ARBCalendar.h"
#include "ARBConfig.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBDogTrial::ARBDogTrial()
	: m_Location()
	, m_Note()
	, m_Verified(false)
	, m_Clubs()
	, m_Runs()
{
}

ARBDogTrial::ARBDogTrial(const ARBCalendar& inCal)
	: m_Location(inCal.GetLocation())
	, m_Note()
	, m_Verified(false)
	, m_Clubs()
	, m_Runs()
{	
	m_Clubs.AddClub(inCal.GetClub(), inCal.GetVenue());
}

ARBDogTrial::ARBDogTrial(const ARBDogTrial& rhs)
	: m_Location(rhs.m_Location)
	, m_Note(rhs.m_Note)
	, m_Verified(rhs.m_Verified)
	, m_Clubs(rhs.m_Clubs)
	, m_Runs(rhs.m_Runs)
{
}

ARBDogTrial::~ARBDogTrial()
{
}

ARBDogTrial& ARBDogTrial::operator=(const ARBDogTrial& rhs)
{
	if (this != &rhs)
	{
		m_Location = rhs.m_Location;
		m_Note = rhs.m_Note;
		m_Verified = rhs.m_Verified;
		m_Clubs = rhs.m_Clubs;
		m_Runs = rhs.m_Runs;
	}
	return *this;
}

bool ARBDogTrial::operator==(const ARBDogTrial& rhs) const
{
	return m_Location == rhs.m_Location
		&& m_Note == rhs.m_Note
		&& m_Verified == rhs.m_Verified
		&& m_Clubs == rhs.m_Clubs
		&& m_Runs == rhs.m_Runs;
}

bool ARBDogTrial::operator!=(const ARBDogTrial& rhs) const
{
	return !operator==(rhs);
}

std::string ARBDogTrial::GetGenericName() const
{
	std::string name = m_Location;
	return name;
}

bool ARBDogTrial::Load(
	const ARBConfig& inConfig,
	const CElement& inTree,
	int inVersion)
{
	if (CElement::eInvalidValue == inTree.GetAttrib(ATTRIB_TRIAL_VERIFIED, m_Verified))
	{
		ErrorInvalidAttributeValue(TREE_TRIAL, ATTRIB_TRIAL_VERIFIED, VALID_VALUES_BOOL);
		return false;
	}
	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		const CElement& element = inTree.GetElement(i);
		if (element.GetName() == TREE_LOCATION)
		{
			m_Location = element.GetValue();
		}
		else if (element.GetName() == TREE_NOTE)
		{
			m_Note = element.GetValue();
		}
		else if (element.GetName() == TREE_CLUB)
		{
			// Ignore any errors...
			m_Clubs.Load(inConfig, element, inVersion);
		}
		// Clubs should all be loaded first. We're not going to validate that
		// though. If it's not true, someone has been messing with the file
		// directly and the ARBRunList::Load will fail.
		else if (element.GetName() == TREE_RUN)
		{
			// Ignore any errors...
			m_Runs.Load(inConfig, m_Clubs, element, inVersion);
		}
	}
	m_Runs.sort(true);
	return true;
}

bool ARBDogTrial::Save(CElement& ioTree) const
{
	CElement& trial = ioTree.AddElement(TREE_TRIAL);
	trial.AddAttrib(ATTRIB_TRIAL_VERIFIED, m_Verified);
	if (0 < m_Location.length())
	{
		CElement& element = trial.AddElement(TREE_LOCATION);
		element.SetValue(m_Location);
	}
	if (0 < m_Note.length())
	{
		CElement& element = trial.AddElement(TREE_NOTE);
		element.SetValue(m_Note);
	}
	if (!m_Clubs.Save(trial))
		return false;
	if (!m_Runs.Save(trial))
		return false;
	return true;
}

// This level is the true level (ie: sublevel), not the config level.
int ARBDogTrial::NumQs(
	const ARBDate& inDate,
	const ARBConfig& inConfig,
	const std::string& inDiv,
	const std::string& inLevel) const
{
	if (!GetClubs().GetPrimaryClub())
		return 0;
	int nQs = 0;
	for (ARBDogRunList::const_iterator iterRun = m_Runs.begin(); iterRun != m_Runs.end(); ++iterRun)
	{
		ARBDogRun* pRun = (*iterRun);
		if (pRun->GetDate() == inDate
		&& pRun->GetDivision() == inDiv
		&& pRun->GetLevel() == inLevel
		&& pRun->GetQ().Qualified())
		{
			const ARBConfigScoring* pScoring = inConfig.GetVenues().FindEvent(
				GetClubs().GetPrimaryClub()->GetVenue(),
				pRun->GetEvent(),
				pRun->GetDivision(),
				pRun->GetLevel());
			if (pScoring && pScoring->HasDoubleQ())
				++nQs;
		}
	}
	return nQs;
}

short ARBDogTrial::GetMachPoints(
	const ARBConfig& inConfig,
	const std::string& inDiv,
	const std::string& inLevel) const
{
	if (!GetClubs().GetPrimaryClub())
		return 0;
	short mach = 0;
	for (ARBDogRunList::const_iterator iterRun = m_Runs.begin(); iterRun != m_Runs.end(); ++iterRun)
	{
		ARBDogRun* pRun = (*iterRun);
		if (pRun->GetDivision() == inDiv
		&& pRun->GetLevel() == inLevel)
		{
			const ARBConfigScoring* pScoring = inConfig.GetVenues().FindEvent(
				GetClubs().GetPrimaryClub()->GetVenue(),
				pRun->GetEvent(),
				pRun->GetDivision(),
				pRun->GetLevel());
			mach = mach + pRun->GetMachPoints(pScoring);
		}
	}
	return mach;
}

bool ARBDogTrial::HasVenue(const std::string inVenue) const
{
	for (ARBDogClubList::const_iterator iter = m_Clubs.begin(); iter != m_Clubs.end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue)
			return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////

class SortTrials
{
public:
	SortTrials(bool bDescending) : m_bDescending(bDescending) {}
	bool operator()(ARBDogTrial* one, ARBDogTrial* two) const
	{
		bool bSorted = !m_bDescending;
		if (0 < one->GetRuns().size()
		&& 0 < two->GetRuns().size())
		{
			if ((*(one->GetRuns().begin()))->GetDate() <
			(*(two->GetRuns().begin()))->GetDate())
				bSorted = m_bDescending;
		}
		return bSorted;
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

int ARBDogTrialList::NumTrialsInVenue(const std::string& inVenue) const
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
	const std::string& inOldVenue,
	const std::string& inNewVenue)
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

int ARBDogTrialList::DeleteVenue(const std::string& inVenue)
{
	std::string venue(inVenue);
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

int ARBDogTrialList::NumOtherPointsInUse(const std::string& inOther) const
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
	const std::string& inOldOther,
	const std::string& inNewOther)
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

int ARBDogTrialList::DeleteOtherPoints(const std::string& inOther)
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
	const ARBConfig& inConfig,
	const std::string& inVenue,
	const std::string& inDiv)
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetClubs().FindVenue(inVenue)
		&& 1 < (*iter)->GetClubs().size())
		{
			int nDivCount = 0;
			for (ARBDogClubList::const_iterator iterClub = (*iter)->GetClubs().begin(); iterClub != (*iter)->GetClubs().end(); ++iterClub)
			{
				const ARBConfigVenue* pVenue = inConfig.GetVenues().FindVenue((*iterClub)->GetVenue());
				if (pVenue->GetDivisions().FindDivision(inDiv))
					++nDivCount;
			}
			if (1 < nDivCount)
				++count;
		}
	}
	return count;
}

int ARBDogTrialList::NumRunsInDivision(
	const ARBConfigVenue* inVenue,
	const std::string& inDiv) const
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
	const ARBConfigVenue* inVenue,
	const std::string& inOldDiv,
	const std::string& inNewDiv)
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
	const ARBConfig& inConfig,
	const std::string& inVenue,
	const std::string& inDiv)
{
	std::string venue(inVenue);
	std::string div(inDiv);
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
				const ARBConfigVenue* pVenue = inConfig.GetVenues().FindVenue((*iterClub)->GetVenue());
				if (pVenue->GetDivisions().FindDivision(div))
					++nDivCount;
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
	const std::string& inVenue,
	const std::string& inDiv,
	const std::string& inLevel) const
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
	const std::string& inVenue,
	const std::string& inDiv,
	const std::string& inOldLevel,
	const std::string& inNewLevel)
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
	const std::string& inVenue,
	const std::string& inDiv,
	const std::string& inLevel)
{
	std::string venue(inVenue);
	std::string div(inDiv);
	std::string level(inLevel);
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
	const std::string& inVenue,
	const std::string& inEvent) const
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
	const std::string& inVenue,
	const std::string& inOldEvent,
	const std::string& inNewEvent)
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
	const std::string& inVenue,
	const std::string& inEvent)
{
	std::string venue(inVenue);
	std::string event(inEvent);
	int count = 0;
	for (iterator iter = begin(); iter != end(); )
	{
		if ((*iter)->GetClubs().FindVenue(venue))
		{
			for (ARBDogRunList::iterator iterRun = (*iter)->GetRuns().begin(); iterRun != (*iter)->GetRuns().end(); )
			{
				if ((*iterRun)->GetEvent() == event)
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

ARBDogTrial* ARBDogTrialList::AddTrial(ARBDogTrial* inTrial)
{
	if (inTrial)
	{
		inTrial->AddRef();
		push_back(inTrial);
	}
	return inTrial;
}

bool ARBDogTrialList::DeleteTrial(const ARBDogTrial* inTrial)
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
