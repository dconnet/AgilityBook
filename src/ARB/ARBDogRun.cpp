/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
	: m_Date()
	, m_Division()
	, m_Level()
	, m_Height()
	, m_Event()
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
{
}

ARBDogRun::ARBDogRun(const ARBDogRun& rhs)
	: m_Date(rhs.m_Date)
	, m_Division(rhs.m_Division)
	, m_Level(rhs.m_Level)
	, m_Height(rhs.m_Height)
	, m_Event(rhs.m_Event)
	, m_Conditions(rhs.m_Conditions)
	, m_Judge(rhs.m_Judge)
	, m_Handler(rhs.m_Handler)
	, m_Partners(rhs.m_Partners)
	, m_Scoring(rhs.m_Scoring)
	, m_Q(rhs.m_Q)
	, m_Place(rhs.m_Place)
	, m_InClass(rhs.m_InClass)
	, m_DogsQd(rhs.m_DogsQd)
	, m_OtherPoints(rhs.m_OtherPoints)
	, m_Notes(rhs.m_Notes)
	, m_RefRuns(rhs.m_RefRuns)
{
}

ARBDogRun::~ARBDogRun()
{
}

ARBDogRun& ARBDogRun::operator=(const ARBDogRun& rhs)
{
	if (this != &rhs)
	{
		m_Date = rhs.m_Date;
		m_Division = rhs.m_Division;
		m_Level = rhs.m_Level;
		m_Height = rhs.m_Height;
		m_Event = rhs.m_Event;
		m_Conditions = rhs.m_Conditions;
		m_Judge = rhs.m_Judge;
		m_Handler = rhs.m_Handler;
		m_Partners = rhs.m_Partners;
		m_Scoring = rhs.m_Scoring;
		m_Q = rhs.m_Q;
		m_Place = rhs.m_Place;
		m_InClass = rhs.m_InClass;
		m_DogsQd = rhs.m_DogsQd;
		m_OtherPoints = rhs.m_OtherPoints;
		m_Notes = rhs.m_Notes;
		m_RefRuns = rhs.m_RefRuns;
	}
	return *this;
}

bool ARBDogRun::operator==(const ARBDogRun& rhs) const
{
	return m_Date == rhs.m_Date
		&& m_Division == rhs.m_Division
		&& m_Level == rhs.m_Level
		&& m_Height == rhs.m_Height
		&& m_Event == rhs.m_Event
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
		&& m_RefRuns == rhs.m_RefRuns;
}

bool ARBDogRun::operator!=(const ARBDogRun& rhs) const
{
	return !operator==(rhs);
}

std::string ARBDogRun::GetGenericName() const
{
	std::string name = m_Date.GetString(false, false);
	name += " ";
	name += m_Division;
	name += " ";
	name += m_Level;
	name += " ";
	name += m_Event;
	return name;
}

bool ARBDogRun::Load(
	const ARBConfig& inConfig,
	const ARBDogClubList& inClubs,
	const CElement& inTree,
	int inVersion)
{
	switch (inTree.GetAttrib(ATTRIB_RUN_DATE, m_Date))
	{
	case CElement::eNotFound:
		ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_DATE);
		return false;
	case CElement::eInvalidValue:
		{
			std::string attrib;
			inTree.GetAttrib(ATTRIB_RUN_DATE, attrib);
			std::string msg(INVALID_DATE);
			msg += attrib;
			ErrorInvalidAttributeValue(TREE_RUN, ATTRIB_RUN_DATE, msg.c_str());
			return false;
		}
	}

	if (CElement::eFound != inTree.GetAttrib(ATTRIB_RUN_DIVISION, m_Division)
	|| 0 == m_Division.length())
	{
		ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_DIVISION);
		return false;
	}

	if (CElement::eFound != inTree.GetAttrib(ATTRIB_RUN_LEVEL, m_Level)
	|| 0 == m_Level.length())
	{
		ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_LEVEL);
		return false;
	}

	if (CElement::eFound != inTree.GetAttrib(ATTRIB_RUN_HEIGHT, m_Height)
	|| 0 == m_Height.length())
	{
		ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_HEIGHT);
		return false;
	}

	if (CElement::eFound != inTree.GetAttrib(ATTRIB_RUN_EVENT, m_Event)
	|| 0 == m_Event.length())
	{
		ErrorMissingAttribute(TREE_RUN, ATTRIB_RUN_EVENT);
		return false;
	}

	// This will get the first scoring style to match. So the order of
	// the clubs is critical as we'll search the venues by club order.
	const ARBConfigScoring* pEvent = inClubs.FindEvent(&inConfig, m_Event, m_Division, m_Level);
	if (!pEvent)
		return false;

	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		const CElement& element = inTree.GetElement(i);
		const std::string& name = element.GetName();
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
			m_Partners.Load(inConfig, element, inVersion);
		}
		else if (name == TREE_BY_TIME
		|| name == TREE_BY_OPENCLOSE
		|| name == TREE_BY_POINTS)
		{
			// Ignore any errors...
			m_Scoring.Load(pEvent, element, inVersion);
		}
		else if (name == TREE_PLACEMENT)
		{
			std::string attrib;
			if (CElement::eFound != element.GetAttrib(ATTRIB_PLACEMENT_Q, attrib)
			|| 0 == attrib.length())
			{
				ErrorMissingAttribute(TREE_PLACEMENT, ATTRIB_PLACEMENT_Q);
				return false;
			}
			if (!m_Q.Load(attrib, inVersion))
			{
				std::string msg(VALID_VALUES);
				msg += ARB_Q::GetValidTypes();
				ErrorInvalidAttributeValue(TREE_PLACEMENT, ATTRIB_PLACEMENT_Q, msg.c_str());
				return false;
			}
			element.GetAttrib(ATTRIB_PLACEMENT_PLACE, m_Place);
			element.GetAttrib(ATTRIB_PLACEMENT_INCLASS, m_InClass);
			element.GetAttrib(ATTRIB_PLACEMENT_DOGSQD, m_DogsQd);
			for (int idx = 0; idx < element.GetElementCount(); ++idx)
			{
				const CElement& subElement = element.GetElement(idx);
				if (subElement.GetName() == TREE_PLACEMENT_OTHERPOINTS)
				{
					m_OtherPoints.Load(inConfig, subElement, inVersion);
				}
			}
		}
		else if (name == TREE_NOTES)
		{
			// Ignore any errors...
			m_Notes.Load(inConfig, element, inVersion);
		}
		else if (name == TREE_REF_RUN)
		{
			// Ignore any errors...
			m_RefRuns.Load(inConfig, element, inVersion);
		}
	}
	return true;
}

bool ARBDogRun::Save(CElement& ioTree) const
{
	CElement& run = ioTree.AddElement(TREE_RUN);
	run.AddAttrib(ATTRIB_RUN_DATE, m_Date);
	run.AddAttrib(ATTRIB_RUN_DIVISION, m_Division);
	run.AddAttrib(ATTRIB_RUN_LEVEL, m_Level);
	run.AddAttrib(ATTRIB_RUN_HEIGHT, m_Height);
	run.AddAttrib(ATTRIB_RUN_EVENT, m_Event);
	if (0 < m_Conditions.length())
	{
		CElement& element = run.AddElement(TREE_CONDITIONS);
		element.SetValue(m_Conditions);
	}
	run.AddElement(TREE_JUDGE).SetValue(m_Judge);
	run.AddElement(TREE_HANDLER).SetValue(m_Handler);
	if (!m_Partners.Save(run))
		return false;
	if (!m_Scoring.Save(run))
		return false;
	if (0 < m_Place || ARB_Q::eQ_NA != m_Q)
	{
		CElement& element = run.AddElement(TREE_PLACEMENT);
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
	return true;
}

int ARBDogRun::NumOtherPointsInUse(const std::string& inOther) const
{
	int count = 0;
	for (ARBDogRunOtherPointsList::const_iterator iter = m_OtherPoints.begin(); iter != m_OtherPoints.end(); ++iter)
	{
		if ((*iter)->GetName() == inOther)
			++count;
	}
	return count;
}

int ARBDogRun::RenameOtherPoints(const std::string& inOldName, const std::string& inNewName)
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

int ARBDogRun::DeleteOtherPoints(const std::string& inName)
{
	std::string name(inName);
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

short ARBDogRun::GetMachPoints(const ARBConfigScoring* inScoring) const
{
	short pts = 0;
	if (inScoring && inScoring->HasMachPts())
	{
		if (GetQ().Qualified())
		{
			ARBDouble time = GetScoring().GetTime();
			ARBDouble sct = GetScoring().GetSCT();
			ARBDouble diff = sct - time;
			pts = static_cast<short>(diff);
			if (1 == GetPlace())
				pts *= 2;
			else if (2 == GetPlace())
				pts = static_cast<short>(pts * 1.5);
		}
	}
	return pts;
}

short ARBDogRun::GetTitlePoints(
	const ARBConfigScoring* inScoring,
	bool* outClean) const
{
	short pts = 0;
	if (outClean)
		*outClean = false;
	switch (m_Scoring.GetType())
	{
	default:
		break;
	case ARBDogRunScoring::eTypeByTime:
		{
			double faults = m_Scoring.GetCourseFaults() + m_Scoring.GetTimeFaults();
			if (ARBConfigScoring::eTimePlusFaults == inScoring->GetScoringStyle())
			{
				if (0 == m_Scoring.GetSCT() || faults <= m_Scoring.GetSCT())
				{
					if (outClean)
						*outClean = true;
					pts = inScoring->GetTitlePoints().GetTitlePoints(0);
				}
			}
			else
			{
				if (0 == faults)
				{
					if (outClean)
						*outClean = true;
				}
				pts = inScoring->GetTitlePoints().GetTitlePoints(faults);
			}
		}
		break;
	case ARBDogRunScoring::eTypeByOpenClose:
		if (m_Scoring.GetNeedOpenPts() <= m_Scoring.GetOpenPts()
		&& m_Scoring.GetNeedClosePts() <= m_Scoring.GetClosePts())
		{
			if (outClean)
				*outClean = true;
			pts = inScoring->GetTitlePoints().GetTitlePoints(0);
		}
		break;
	case ARBDogRunScoring::eTypeByPoints:
		if (m_Scoring.GetNeedOpenPts() <= m_Scoring.GetOpenPts())
		{
			if (outClean)
				*outClean = true;
			pts = inScoring->GetTitlePoints().GetTitlePoints(0);
		}
		break;
	}
	return pts;
}

ARBDouble ARBDogRun::GetScore(const ARBConfigScoring* inScoring) const
{
	ARBDouble pts = 0.0;
	switch (m_Scoring.GetType())
	{
	default:
		break;
	case ARBDogRunScoring::eTypeByTime:
		pts = m_Scoring.GetCourseFaults() + m_Scoring.GetTimeFaults();
		switch (inScoring->GetScoringStyle())
		{
		default: break;
		case ARBConfigScoring::eTimePlusFaults: pts += GetScoring().GetTime(); break;
		case ARBConfigScoring::eFaults100ThenTime: pts = 100 - pts; break;
		case ARBConfigScoring::eFaults200ThenTime: pts = 200 - pts; break;
		}
		break;
	case ARBDogRunScoring::eTypeByOpenClose:
		pts = m_Scoring.GetOpenPts() + m_Scoring.GetClosePts() - m_Scoring.GetCourseFaults();
		break;
	case ARBDogRunScoring::eTypeByPoints:
		pts = m_Scoring.GetOpenPts() - m_Scoring.GetCourseFaults();
		break;
	}
	return pts;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBDogRunList::Load(
	const ARBConfig& inConfig,
	const ARBDogClubList& inClubs,
	const CElement& inTree,
	int inVersion)
{
	ARBDogRun* thing = new ARBDogRun;
	if (!thing->Load(inConfig, inClubs, inTree, inVersion))
	{
		thing->Release();
		return false;
	}
	push_back(thing);
	return true;
}

class SortRun
{
public:
	SortRun(bool bDescending) : m_bDescending(bDescending) {}
	bool operator()(ARBDogRun* one, ARBDogRun* two) const
	{
		if (one->GetDate() < two->GetDate())
			return m_bDescending;
		else
			return !m_bDescending;
	}
private:
	bool m_bDescending;
};

void ARBDogRunList::sort(bool inDescending)
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortRun(inDescending));
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

ARBDogRun* ARBDogRunList::AddRun(ARBDogRun* inRun)
{
	if (inRun)
	{
		inRun->AddRef();
		push_back(inRun);
	}
	return inRun;
}

bool ARBDogRunList::DeleteRun(const ARBDogRun* inRun)
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
