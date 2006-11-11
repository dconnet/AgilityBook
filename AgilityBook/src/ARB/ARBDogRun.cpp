/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-04 DRC Added support for NADAC bonus titling points.
 * @li 2005-10-31 DRC Time+Fault runs didn't compute titling points right.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-10 DRC Only sort runs one way, the UI handles everything else.
 * @li 2005-01-02 DRC Added subnames to events.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-10-04 DRC Only compute MACH points if time and SCT are >0. Also, do
 *                    not allow negative mach points.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-09-07 DRC Time+Fault scoring shouldn't include time faults.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-03-30 DRC Added links.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-10-13 DRC Allow course faults on point runs.
 * @li 2003-09-01 DRC Fixed problem with Score and TitlePts in time+faults.
 */

#include "StdAfx.h"
#include "ARBDogRun.h"
#include <algorithm>

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBDogRun::ARBDogRun()
	: m_pMultiQ()
	, m_Date()
	, m_Division()
	, m_Level()
	, m_Height()
	, m_Event()
	, m_SubName()
	, m_Conditions()
	, m_Judge()
	, m_Handler()
	, m_Partners()
	, m_Scoring()
	, m_Q()
	, m_Place(0)
	, m_InClass(0)
	, m_DogsQd(-1)
	, m_OtherPoints()
	, m_Notes()
	, m_RefRuns()
	, m_Links()
{
}

ARBDogRun::ARBDogRun(ARBDogRun const& rhs)
	: m_pMultiQ()
	, m_Date(rhs.m_Date)
	, m_Division(rhs.m_Division)
	, m_Level(rhs.m_Level)
	, m_Height(rhs.m_Height)
	, m_Event(rhs.m_Event)
	, m_SubName(rhs.m_SubName)
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

ARBDogRun::~ARBDogRun()
{
}

ARBDogRun& ARBDogRun::operator=(ARBDogRun const& rhs)
{
	if (this != &rhs)
	{
		m_Date = rhs.m_Date;
		m_Division = rhs.m_Division;
		m_Level = rhs.m_Level;
		m_Height = rhs.m_Height;
		m_Event = rhs.m_Event;
		m_SubName = rhs.m_SubName;
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

bool ARBDogRun::operator==(ARBDogRun const& rhs) const
{
	return m_Date == rhs.m_Date
		&& m_Division == rhs.m_Division
		&& m_Level == rhs.m_Level
		&& m_Height == rhs.m_Height
		&& m_Event == rhs.m_Event
		&& m_SubName == rhs.m_SubName
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
}

ARBString ARBDogRun::GetGenericName() const
{
	ARBString name = m_Date.GetString(ARBDate::eDashYMD) + _T(' ');
	name += m_Division + _T(' ') + m_Level + _T(' ') + m_Event;
	if (0 < m_SubName.length())
	{
		name += _T(' ') + m_SubName;
	}
	return name;
}

size_t ARBDogRun::GetSearchStrings(std::set<ARBString>& ioStrings) const
{
	size_t nItems = 0;

	ioStrings.insert(m_Date.GetString(ARBDate::eSlashMDY));
	++nItems;

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
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	switch (inTree.GetAttrib(ATTRIB_RUN_DATE, m_Date))
	{
	case Element::eNotFound:
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_DATE));
		return false;
	case Element::eInvalidValue:
		{
			ARBString attrib;
			inTree.GetAttrib(ATTRIB_RUN_DATE, attrib);
			ARBString msg(INVALID_DATE);
			msg += attrib;
			ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_RUN, ATTRIB_RUN_DATE, msg.c_str()));
			return false;
		}
	}

	if (Element::eFound != inTree.GetAttrib(ATTRIB_RUN_DIVISION, m_Division)
	|| 0 == m_Division.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_DIVISION));
		return false;
	}

	if (Element::eFound != inTree.GetAttrib(ATTRIB_RUN_LEVEL, m_Level)
	|| 0 == m_Level.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_LEVEL));
		return false;
	}

	// Height is no longer a required attribute (doc ver 8.1)
	inTree.GetAttrib(ATTRIB_RUN_HEIGHT, m_Height);

	if (Element::eFound != inTree.GetAttrib(ATTRIB_RUN_EVENT, m_Event)
	|| 0 == m_Event.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_EVENT));
		return false;
	}

	inTree.GetAttrib(ATTRIB_RUN_SUBNAME, m_SubName);

	// This will get the first scoring style to match. So the order of
	// the clubs is critical as we'll search the venues by club order.
	ARBConfigScoringPtr pEvent;
	if (!inClubs.FindEvent(inConfig, m_Event, m_Division, m_Level, m_Date, ioCallback, &pEvent))
		return false;

	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		Element const& element = inTree.GetElement(i);
		ARBString const& name = element.GetName();
		if (name == TREE_CONDITIONS)
		{
			m_Conditions = element.GetValue();
		}
		else if (name == TREE_JUDGE)
		{
			m_Judge = element.GetValue();
		}
		else if (name == TREE_HANDLER)
		{
			m_Handler = element.GetValue();
		}
		else if (name == TREE_PARTNER)
		{
			// Ignore any errors...
			m_Partners.Load(inConfig, element, inVersion, ioCallback);
		}
		else if (name == TREE_BY_TIME
		|| name == TREE_BY_OPENCLOSE
		|| name == TREE_BY_POINTS)
		{
			// Ignore any errors...
			m_Scoring.Load(pEvent, element, inVersion, ioCallback);
		}
		else if (name == TREE_PLACEMENT)
		{
			ARBString attrib;
			if (Element::eFound != element.GetAttrib(ATTRIB_PLACEMENT_Q, attrib)
			|| 0 == attrib.length())
			{
				ioCallback.LogMessage(ErrorMissingAttribute(TREE_PLACEMENT, ATTRIB_PLACEMENT_Q));
				return false;
			}
			if (!m_Q.Load(attrib, inVersion, ioCallback))
			{
				ARBString msg(VALID_VALUES);
				msg += ARB_Q::GetValidTypes();
				ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_PLACEMENT, ATTRIB_PLACEMENT_Q, msg.c_str()));
				return false;
			}
			element.GetAttrib(ATTRIB_PLACEMENT_PLACE, m_Place);
			element.GetAttrib(ATTRIB_PLACEMENT_INCLASS, m_InClass);
			element.GetAttrib(ATTRIB_PLACEMENT_DOGSQD, m_DogsQd);
			for (int idx = 0; idx < element.GetElementCount(); ++idx)
			{
				Element const& subElement = element.GetElement(idx);
				if (subElement.GetName() == TREE_PLACEMENT_OTHERPOINTS)
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
			m_Links.insert(element.GetValue());
		}
	}
	return true;
}

bool ARBDogRun::Save(Element& ioTree) const
{
	Element& run = ioTree.AddElement(TREE_RUN);
	run.AddAttrib(ATTRIB_RUN_DATE, m_Date);
	run.AddAttrib(ATTRIB_RUN_DIVISION, m_Division);
	run.AddAttrib(ATTRIB_RUN_LEVEL, m_Level);
	run.AddAttrib(ATTRIB_RUN_HEIGHT, m_Height);
	run.AddAttrib(ATTRIB_RUN_EVENT, m_Event);
	if (0 < m_SubName.length())
		run.AddAttrib(ATTRIB_RUN_SUBNAME, m_SubName);
	if (0 < m_Conditions.length())
	{
		Element& element = run.AddElement(TREE_CONDITIONS);
		element.SetValue(m_Conditions);
	}
	run.AddElement(TREE_JUDGE).SetValue(m_Judge);
	if (0 < m_Handler.length())
		run.AddElement(TREE_HANDLER).SetValue(m_Handler);
	if (!m_Partners.Save(run))
		return false;
	if (!m_Scoring.Save(run))
		return false;
	if (0 < m_Place || ARB_Q::eQ_NA != m_Q)
	{
		Element& element = run.AddElement(TREE_PLACEMENT);
		m_Q.Save(element, ATTRIB_PLACEMENT_Q);
		element.AddAttrib(ATTRIB_PLACEMENT_PLACE, m_Place);
		if (0 < m_InClass)
			element.AddAttrib(ATTRIB_PLACEMENT_INCLASS, m_InClass);
		if (0 <= m_DogsQd)
			element.AddAttrib(ATTRIB_PLACEMENT_DOGSQD, m_DogsQd);
		if (!m_OtherPoints.Save(element))
			return false;
	}
	if (!m_Notes.Save(run))
		return false;
	if (!m_RefRuns.Save(run))
		return false;
	for (ARBDogRunLinks::const_iterator iterLink = m_Links.begin();
		iterLink != m_Links.end();
		++iterLink)
	{
		Element& element = run.AddElement(TREE_RUN_LINK);
		element.SetValue(*iterLink);
	}
	return true;
}

int ARBDogRun::NumOtherPointsInUse(ARBString const& inOther) const
{
	int count = 0;
	for (ARBDogRunOtherPointsList::const_iterator iter = m_OtherPoints.begin(); iter != m_OtherPoints.end(); ++iter)
	{
		if ((*iter)->GetName() == inOther)
			++count;
	}
	return count;
}

int ARBDogRun::RenameOtherPoints(
		ARBString const& inOldName,
		ARBString const& inNewName)
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

int ARBDogRun::DeleteOtherPoints(ARBString const& inName)
{
	ARBString name(inName);
	int count = 0;
	for (ARBDogRunOtherPointsList::iterator iter = m_OtherPoints.begin(); iter != m_OtherPoints.end(); )
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

short ARBDogRun::GetSpeedPoints(ARBConfigScoringPtr inScoring) const
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
				double mult;
				if (0 < GetPlace())
				{
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

double ARBDogRun::GetTitlePoints(
		ARBConfigScoringPtr inScoring,
		bool* outClean,
		double* outLifeTime,
		double* outPlacement) const
{
	double pts = 0.0;
	if (outClean)
		*outClean = false;
	if (outLifeTime)
		*outLifeTime = 0.0;
	if (outPlacement)
	{
		double pts;
		if (inScoring->GetPlacements().GetPlaceInfo(GetPlace(), pts))
			*outPlacement = pts;
		else
			*outPlacement = 0.0;
	}
	short bonusPts = inScoring->HasBonusPts() ? m_Scoring.GetBonusPts() : 0;
	switch (m_Scoring.GetType())
	{
	default:
		break;
	case ARBDogRunScoring::eTypeByTime:
		{
			double score = m_Scoring.GetCourseFaults() + m_Scoring.GetTimeFaults(inScoring);
			if (ARBConfigScoring::eTimePlusFaults == inScoring->GetScoringStyle())
			{
				bool bGood = true;
				if (inScoring && inScoring->QsMustBeClean() && score > 0.0)
					bGood = false;
				score += m_Scoring.GetTime();
				if (bGood
				&& (ARBDouble::equal(m_Scoring.GetSCT(), 0) || score <= m_Scoring.GetSCT()))
				{
					if (outClean)
						*outClean = true;
					pts = inScoring->GetTitlePoints().GetTitlePoints(0) + bonusPts;
					if (outLifeTime)
						*outLifeTime = inScoring->GetLifetimePoints().GetLifetimePoints(0) + bonusPts;
				}
			}
			else
			{
				if (ARBDouble::equal(score, 0))
				{
					if (outClean)
						*outClean = true;
				}
				pts = inScoring->GetTitlePoints().GetTitlePoints(score) + bonusPts;
				if (outLifeTime)
					*outLifeTime = inScoring->GetLifetimePoints().GetLifetimePoints(score) + bonusPts;
			}
		}
		break;
	case ARBDogRunScoring::eTypeByOpenClose:
		if (m_Scoring.GetNeedOpenPts() <= m_Scoring.GetOpenPts()
		&& m_Scoring.GetNeedClosePts() <= m_Scoring.GetClosePts())
		{
			double timeFaults = 0.0;
			if (inScoring->ComputeTimeFaultsUnder()
			|| inScoring->ComputeTimeFaultsOver())
			{
				timeFaults = m_Scoring.GetTimeFaults(inScoring);
				if (0.0 < timeFaults && inScoring->SubtractTimeFaultsFromScore())
				{
					// If time faults are being subtracted from the score,
					// recompute if we have enough points. If so, just pretend
					// there are no time faults.
					if (static_cast<double>(m_Scoring.GetNeedOpenPts() + m_Scoring.GetNeedClosePts()) <= GetScore(inScoring))
						timeFaults = 0.0;
				}
			}
			if (outClean)
				*outClean = true;
			pts = inScoring->GetTitlePoints().GetTitlePoints(timeFaults) + bonusPts;
			if (outLifeTime)
				*outLifeTime = inScoring->GetLifetimePoints().GetLifetimePoints(timeFaults) + bonusPts;
		}
		break;
	case ARBDogRunScoring::eTypeByPoints:
		if (m_Scoring.GetNeedOpenPts() <= m_Scoring.GetOpenPts())
		{
			double timeFaults = 0.0;
			if (inScoring->ComputeTimeFaultsUnder()
			|| inScoring->ComputeTimeFaultsOver())
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
			pts = inScoring->GetTitlePoints().GetTitlePoints(timeFaults) + bonusPts;
			if (outLifeTime)
				*outLifeTime = inScoring->GetLifetimePoints().GetLifetimePoints(timeFaults) + bonusPts;
		}
		break;
	}
	return pts;
}

double ARBDogRun::GetScore(ARBConfigScoringPtr inScoring) const
{
	double pts = 0.0;
	switch (m_Scoring.GetType())
	{
	default:
		break;
	case ARBDogRunScoring::eTypeByTime:
		pts = m_Scoring.GetCourseFaults() + m_Scoring.GetTimeFaults(inScoring);
		switch (inScoring->GetScoringStyle())
		{
		default: break;
		case ARBConfigScoring::eTimePlusFaults: pts += m_Scoring.GetTime(); break;
		case ARBConfigScoring::eFaults100ThenTime: pts = 100 - pts; break;
		case ARBConfigScoring::eFaults200ThenTime: pts = 200 - pts; break;
		}
		break;
	case ARBDogRunScoring::eTypeByOpenClose:
		pts = m_Scoring.GetOpenPts() + m_Scoring.GetClosePts() - m_Scoring.GetCourseFaults();
		if (inScoring->SubtractTimeFaultsFromScore())
			pts -= m_Scoring.GetTimeFaults(inScoring);
		break;
	case ARBDogRunScoring::eTypeByPoints:
		pts = m_Scoring.GetOpenPts() - m_Scoring.GetCourseFaults();
		if (inScoring->SubtractTimeFaultsFromScore())
			pts -= m_Scoring.GetTimeFaults(inScoring);
		break;
	}
	return pts;
}

size_t ARBDogRun::GetLinks(std::set<ARBString>& outLinks) const
{
	outLinks.clear();
	outLinks = m_Links;
	return outLinks.size();
}

bool ARBDogRun::HasLink(ARBString const& inLink) const
{
	return m_Links.find(inLink) != m_Links.end();
}

void ARBDogRun::AddLink(ARBString const& inLink)
{
	m_Links.insert(inLink);
}

void ARBDogRun::RemoveLink(ARBString const& inLink)
{
	ARBDogRunLinks::iterator iter = m_Links.find(inLink);
	if (iter != m_Links.end())
			m_Links.erase(iter);
}

/////////////////////////////////////////////////////////////////////////////

bool ARBDogRunList::Load(
		ARBConfig const& inConfig,
		ARBDogClubList const& inClubs,
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBDogRunPtr thing(ARBDogRun::New());
	if (!thing->Load(inConfig, inClubs, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}

class SortRun
{
public:
	SortRun() {}
	bool operator()(ARBDogRunPtr one, ARBDogRunPtr two) const
	{
		return one->GetDate() < two->GetDate();
	}
};

void ARBDogRunList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortRun());
}

ARBDate ARBDogRunList::GetStartDate() const
{
	ARBDate date;
	if (0 < size())
	{
		ARBDate date1 = (*begin())->GetDate();
		ARBDate date2 = (*(end()-1))->GetDate();
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
		ARBDate date2 = (*(end()-1))->GetDate();
		date = date1 > date2 ? date1 : date2;
	}
	return date;
}

bool ARBDogRunList::AddRun(ARBDogRunPtr inRun)
{
	bool bAdded = false;
	if (inRun)
	{
		bAdded = true;
		push_back(inRun);
	}
	return bAdded;
}

bool ARBDogRunList::DeleteRun(ARBDogRunPtr inRun)
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
