/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-06-11 DRC Still not right - runs with 0 SCT should allow title pts.
 * @li 2008-03-03 DRC Title point computation on T+F runs was wrong.
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

#include "stdafx.h"
#include "ARBDogRun.h"
#include <algorithm>

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "ARBLocalization.h"
#include "Element.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

ARBDogRunPtr ARBDogRun::New()
{
	return ARBDogRunPtr(new ARBDogRun());
}


ARBDogRun::ARBDogRun()
	: m_pMultiQs()
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
	: m_pMultiQs()
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


ARBDogRunPtr ARBDogRun::Clone() const
{
	return ARBDogRunPtr(new ARBDogRun(*this));
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


wxString ARBDogRun::GetGenericName() const
{
	wxString name = m_Date.GetString(ARBDate::eDashYMD) + wxT(" ");
	name += m_Division + wxT(" ") + m_Level + wxT(" ") + m_Event;
	if (0 < m_SubName.length())
	{
		name += wxT(" ") + m_SubName;
	}
	return name;
}


size_t ARBDogRun::GetSearchStrings(std::set<wxString>& ioStrings) const
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
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_RUN)
		return false;
	switch (inTree->GetAttrib(ATTRIB_RUN_DATE, m_Date))
	{
	case ElementNode::eNotFound:
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_DATE));
		return false;
	case ElementNode::eInvalidValue:
		{
			wxString attrib;
			inTree->GetAttrib(ATTRIB_RUN_DATE, attrib);
			wxString msg(Localization()->InvalidDate());
			msg += attrib;
			ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_RUN, ATTRIB_RUN_DATE, msg));
			return false;
		}
	}

	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_RUN_DIVISION, m_Division)
	|| 0 == m_Division.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_DIVISION));
		return false;
	}

	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_RUN_LEVEL, m_Level)
	|| 0 == m_Level.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_LEVEL));
		return false;
	}

	// Height is no longer a required attribute (doc ver 8.1)
	inTree->GetAttrib(ATTRIB_RUN_HEIGHT, m_Height);

	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_RUN_EVENT, m_Event)
	|| 0 == m_Event.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_EVENT));
		return false;
	}

	inTree->GetAttrib(ATTRIB_RUN_SUBNAME, m_SubName);

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
		else if (name == TREE_BY_TIME
		|| name == TREE_BY_OPENCLOSE
		|| name == TREE_BY_POINTS)
		{
			// Ignore any errors...
			m_Scoring.Load(inConfig.GetVersion(), pEvent, pScoring, element, inVersion, ioCallback);
		}
		else if (name == TREE_PLACEMENT)
		{
			wxString attrib;
			if (ElementNode::eFound != element->GetAttrib(ATTRIB_PLACEMENT_Q, attrib)
			|| 0 == attrib.length())
			{
				ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_PLACEMENT, ATTRIB_PLACEMENT_Q));
				return false;
			}
			if (!m_Q.Load(attrib, inVersion, ioCallback))
			{
				wxString msg(Localization()->ValidValues());
				msg += ARB_Q::GetValidTypes();
				ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_PLACEMENT, ATTRIB_PLACEMENT_Q, msg));
				return false;
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
	return true;
}


bool ARBDogRun::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr run = ioTree->AddElementNode(TREE_RUN);
	run->AddAttrib(ATTRIB_RUN_DATE, m_Date);
	run->AddAttrib(ATTRIB_RUN_DIVISION, m_Division);
	run->AddAttrib(ATTRIB_RUN_LEVEL, m_Level);
	run->AddAttrib(ATTRIB_RUN_HEIGHT, m_Height);
	run->AddAttrib(ATTRIB_RUN_EVENT, m_Event);
	if (0 < m_SubName.length())
		run->AddAttrib(ATTRIB_RUN_SUBNAME, m_SubName);
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
	if (0 < m_Place || ARB_Q::eQ_NA != m_Q)
	{
		ElementNodePtr element = run->AddElementNode(TREE_PLACEMENT);
		m_Q.Save(element, ATTRIB_PLACEMENT_Q);
		element->AddAttrib(ATTRIB_PLACEMENT_PLACE, m_Place);
		if (0 < m_InClass)
			element->AddAttrib(ATTRIB_PLACEMENT_INCLASS, m_InClass);
		if (0 <= m_DogsQd)
			element->AddAttrib(ATTRIB_PLACEMENT_DOGSQD, m_DogsQd);
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


int ARBDogRun::RenameOtherPoints(
		wxString const& inOldName,
		wxString const& inNewName)
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
		double pts2;
		if (inScoring->GetPlacements().GetPlaceInfo(GetPlace(), pts2))
			*outPlacement = pts2;
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
			if (ARBDouble::equal(score, 0))
			{
				if (outClean)
					*outClean = true;
			}
			if (ARBConfigScoring::eTimePlusFaults == inScoring->GetScoringStyle())
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
					pts = inScoring->GetTitlePoints().GetTitlePoints(score) + bonusPts;
					if (outLifeTime)
						*outLifeTime = inScoring->GetLifetimePoints().GetLifetimePoints(score) + bonusPts;
				}
			}
			else
			{
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


size_t ARBDogRun::GetMultiQs(std::vector<ARBConfigMultiQPtr>& outMultiQs) const
{
	outMultiQs.clear();
	for (std::set<std::tr1::weak_ptr<ARBConfigMultiQ> >::const_iterator i = m_pMultiQs.begin();
		i != m_pMultiQs.end();
		++i)
	{
		outMultiQs.push_back((*i).lock());
	}
	return outMultiQs.size();
}


void ARBDogRun::ClearMultiQs()
{
	m_pMultiQs.clear();
}


void ARBDogRun::AddMultiQ(ARBConfigMultiQPtr inMultiQ)
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
		ElementNodePtr inTree,
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
