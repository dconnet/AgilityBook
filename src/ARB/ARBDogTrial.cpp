/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-10 DRC Only sort runs one way, the UI handles everything else.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-26 DRC Changed version number to a complex value.
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

ARBDogTrial::ARBDogTrial(ARBCalendar const& inCal)
	: m_Location(inCal.GetLocation())
	, m_Note(inCal.GetNote())
	, m_Verified(false)
	, m_Clubs()
	, m_Runs()
{	
	m_Clubs.AddClub(inCal.GetClub(), inCal.GetVenue());
}

ARBDogTrial::ARBDogTrial(ARBDogTrial const& rhs)
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

ARBDogTrial& ARBDogTrial::operator=(ARBDogTrial const& rhs)
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

bool ARBDogTrial::operator==(ARBDogTrial const& rhs) const
{
	return m_Location == rhs.m_Location
		&& m_Note == rhs.m_Note
		&& m_Verified == rhs.m_Verified
		&& m_Clubs == rhs.m_Clubs
		&& m_Runs == rhs.m_Runs;
}

bool ARBDogTrial::operator!=(ARBDogTrial const& rhs) const
{
	return !operator==(rhs);
}

std::string ARBDogTrial::GetGenericName() const
{
	std::string name = m_Location;
	return name;
}

size_t ARBDogTrial::GetSearchStrings(std::set<std::string>& ioStrings) const
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
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_TRIAL_VERIFIED, m_Verified))
	{
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_TRIAL, ATTRIB_TRIAL_VERIFIED, VALID_VALUES_BOOL));
		return false;
	}
	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		Element const& element = inTree.GetElement(i);
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
			m_Clubs.Load(inConfig, element, inVersion, ioCallback);
		}
		// Clubs should all be loaded first. We're not going to validate that
		// though. If it's not true, someone has been messing with the file
		// directly and the ARBRunList::Load will fail.
		else if (element.GetName() == TREE_RUN)
		{
			// Ignore any errors...
			m_Runs.Load(inConfig, m_Clubs, element, inVersion, ioCallback);
		}
	}
	m_Runs.sort();
	return true;
}

bool ARBDogTrial::Save(Element& ioTree) const
{
	Element& trial = ioTree.AddElement(TREE_TRIAL);
	if (m_Verified) // Default is no
		trial.AddAttrib(ATTRIB_TRIAL_VERIFIED, m_Verified);
	if (0 < m_Location.length())
	{
		Element& element = trial.AddElement(TREE_LOCATION);
		element.SetValue(m_Location);
	}
	if (0 < m_Note.length())
	{
		Element& element = trial.AddElement(TREE_NOTE);
		element.SetValue(m_Note);
	}
	if (!m_Clubs.Save(trial))
		return false;
	if (!m_Runs.Save(trial))
		return false;
	return true;
}

bool ARBDogTrial::HasMultiQ(
		ARBDate const& inDate,
		ARBConfig const& inConfig,
		ARBDogRun const* inRun,
		ARBVectorBase<ARBConfigMultiQ>* outMultiQs) const
{
	if (outMultiQs)
		outMultiQs->clear();
	ARBDogClub* pClub;
	if (!GetClubs().GetPrimaryClub(&pClub))
		return false;
	// First, get all the qualifying runs for the given date.
	std::vector<ARBDogRun const*> runs;
	for (ARBDogRunList::const_iterator iterRun = m_Runs.begin(); iterRun != m_Runs.end(); ++iterRun)
	{
		ARBDogRun* pRun = (*iterRun);
		if (pRun->GetDate() == inDate
		&& pRun->GetQ().Qualified())
		{
			runs.push_back(pRun);
		}
	}
	size_t nMatches = 0;
	// Now, see if any combo of these runs matches a multiQ config.
	if (1 < runs.size())
	{
		bool bGotIt = NULL == inRun ? true : false;
		ARBConfigVenue* pVenue;
		if (inConfig.GetVenues().FindVenue(pClub->GetVenue(), &pVenue))
		{
			for (ARBConfigMultiQList::iterator iterM = pVenue->GetMultiQs().begin();
				iterM != pVenue->GetMultiQs().end();
				++iterM)
			{
				ARBConfigMultiQ* pMultiQ = *iterM;
				std::vector<ARBDogRun const*> matchedRuns;
				if (pMultiQ->Match(runs, matchedRuns))
				{
					++nMatches;
					if (!bGotIt)
					{
						// Comparing by ptr is fine...
						if (matchedRuns.end() != std::find(matchedRuns.begin(), matchedRuns.end(), inRun))
							bGotIt = true;
					}
					if (outMultiQs)
					{
						pMultiQ->AddRef();
						outMultiQs->push_back(pMultiQ);
					}
				}
			}
			if (!bGotIt)
				nMatches = 0;
		}
		if (pVenue)
			pVenue->Release();
	}
	if (pClub)
		pClub->Release();
	return 0 < nMatches;
}

short ARBDogTrial::GetSpeedPoints(
		ARBConfig const& inConfig,
		std::string const& inDiv,
		std::string const& inLevel) const
{
	if (!GetClubs().GetPrimaryClub())
		return 0;
	short speed = 0;
	for (ARBDogRunList::const_iterator iterRun = m_Runs.begin(); iterRun != m_Runs.end(); ++iterRun)
	{
		ARBDogRun* pRun = (*iterRun);
		if (pRun->GetDivision() == inDiv
		&& pRun->GetLevel() == inLevel)
		{
			ARBConfigScoring* pScoring;
			if (inConfig.GetVenues().FindEvent(
				GetClubs().GetPrimaryClubVenue(),
				pRun->GetEvent(),
				pRun->GetDivision(),
				pRun->GetLevel(),
				pRun->GetDate(),
				&pScoring))
			{
				speed = speed + pRun->GetSpeedPoints(pScoring);
				pScoring->Release();
			}
		}
	}
	return speed;
}

bool ARBDogTrial::HasVenue(std::string const& inVenue) const
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

int ARBDogTrialList::NumTrialsInVenue(std::string const& inVenue) const
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
		std::string const& inOldVenue,
		std::string const& inNewVenue)
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

int ARBDogTrialList::DeleteVenue(std::string const& inVenue)
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

int ARBDogTrialList::NumOtherPointsInUse(std::string const& inOther) const
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
		std::string const& inOldOther,
		std::string const& inNewOther)
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

int ARBDogTrialList::DeleteOtherPoints(std::string const& inOther)
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
		std::string const& inVenue,
		std::string const& inDiv) const
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
				ARBConfigVenue* pVenue;
				if (inConfig.GetVenues().FindVenue((*iterClub)->GetVenue(), &pVenue))
				{
					if (pVenue->GetDivisions().FindDivision(inDiv))
						++nDivCount;
					pVenue->Release();
				}
			}
			if (1 < nDivCount)
				++count;
		}
	}
	return count;
}

int ARBDogTrialList::NumRunsInDivision(
		ARBConfigVenue const* inVenue,
		std::string const& inDiv) const
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
		ARBConfigVenue const* inVenue,
		std::string const& inOldDiv,
		std::string const& inNewDiv)
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
		std::string const& inVenue,
		std::string const& inDiv)
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
				ARBConfigVenue* pVenue;
				if (inConfig.GetVenues().FindVenue((*iterClub)->GetVenue(), &pVenue))
				{
					if (pVenue->GetDivisions().FindDivision(div))
						++nDivCount;
					pVenue->Release();
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
		std::string const& inVenue,
		std::string const& inDiv,
		std::string const& inLevel) const
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
		std::string const& inVenue,
		std::string const& inDiv,
		std::string const& inOldLevel,
		std::string const& inNewLevel)
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
		std::string const& inVenue,
		std::string const& inDiv,
		std::string const& inLevel)
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
		std::string const& inVenue,
		std::string const& inEvent) const
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
		std::string const& inVenue,
		std::string const& inOldEvent,
		std::string const& inNewEvent)
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
		std::string const& inVenue,
		std::string const& inEvent)
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

bool ARBDogTrialList::AddTrial(ARBDogTrial* inTrial)
{
	bool bAdded = false;
	if (inTrial)
	{
		bAdded = true;
		inTrial->AddRef();
		push_back(inTrial);
	}
	return bAdded;
}

bool ARBDogTrialList::DeleteTrial(ARBDogTrial const* inTrial)
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
