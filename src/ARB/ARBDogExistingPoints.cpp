/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * @li 2003-02-02 DRC Created
 */

#include "StdAfx.h"
#include "ARBDogExistingPoints.h"
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

// static
std::string ARBDogExistingPoints::GetPointTypeName(ARBDogExistingPoints::PointType inType)
{
	std::string str;
	switch (inType)
	{
	case eOtherPoints:
		str = "OtherPoints";
		break;
	case eRuns:
		str = "Run";
		break;
	case eMach:
		str = "MACH";
		break;
	case eQQ:
		str = "Double Q";
		break;
	case eSQ:
		str = "SuperQ";
		break;
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

ARBDogExistingPoints::ARBDogExistingPoints()
	: m_Date()
	, m_Comment()
	, m_Type(eRuns)
	, m_Other()
	, m_Venue()
	, m_Div()
	, m_Level()
	, m_Event()
	, m_Points(0)
{
}

ARBDogExistingPoints::ARBDogExistingPoints(const ARBDogExistingPoints& rhs)
	: m_Date(rhs.m_Date)
	, m_Comment(rhs.m_Comment)
	, m_Type(rhs.m_Type)
	, m_Other(rhs.m_Other)
	, m_Venue(rhs.m_Venue)
	, m_Div(rhs.m_Div)
	, m_Level(rhs.m_Level)
	, m_Event(rhs.m_Event)
	, m_Points(rhs.m_Points)
{
}

ARBDogExistingPoints::~ARBDogExistingPoints()
{
}

ARBDogExistingPoints& ARBDogExistingPoints::operator=(const ARBDogExistingPoints& rhs)
{
	if (this != &rhs)
	{
		m_Date = rhs.m_Date;
		m_Comment = rhs.m_Comment;
		m_Type = rhs.m_Type;
		m_Other = rhs.m_Other;
		m_Venue = rhs.m_Venue;
		m_Div = rhs.m_Div;
		m_Level = rhs.m_Level;
		m_Event = rhs.m_Event;
		m_Points = rhs.m_Points;
	}
	return *this;
}

bool ARBDogExistingPoints::operator==(const ARBDogExistingPoints& rhs) const
{
	return m_Date == rhs.m_Date
		&& m_Comment == rhs.m_Comment
		&& m_Type == rhs.m_Type
		&& m_Other == rhs.m_Other
		&& m_Venue == rhs.m_Venue
		&& m_Div == rhs.m_Div
		&& m_Level == rhs.m_Level
		&& m_Event == rhs.m_Event
		&& m_Points == rhs.m_Points;
}

bool ARBDogExistingPoints::operator!=(const ARBDogExistingPoints& rhs) const
{
	return !operator==(rhs);
}

std::string ARBDogExistingPoints::GetGenericName() const
{
	std::string name;
	if (0 < m_Other.length())
		name = m_Other;
	else
		name = m_Event;
	return name;
}

size_t ARBDogExistingPoints::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	return 0;
}

bool ARBDogExistingPoints::Load(
	const ARBConfig& inConfig,
	const CElement& inTree,
	const ARBVersion& inVersion,
	std::string& ioErrMsg)
{
	std::string attrib;

	if (CElement::eInvalidValue == inTree.GetAttrib(ATTRIB_EXISTING_PTS_DATE, m_Date))
	{
		inTree.GetAttrib(ATTRIB_EXISTING_PTS_DATE, attrib);
		std::string msg(INVALID_DATE);
		msg += attrib;
		ioErrMsg += ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_DATE, msg.c_str());
		return false;
	}

	if (CElement::eFound != inTree.GetAttrib(ATTRIB_EXISTING_PTS_TYPE, attrib)
	|| 0 == attrib.length())
	{
		ioErrMsg += ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_TYPE);
		return false;
	}
	if (attrib == EXISTING_PTS_TYPE_OTHER)
		m_Type = eOtherPoints;
	else if (attrib == EXISTING_PTS_TYPE_RUNS)
		m_Type = eRuns;
	else if (attrib == EXISTING_PTS_TYPE_MACH)
		m_Type = eMach;
	else if (attrib == EXISTING_PTS_TYPE_QQ)
		m_Type = eQQ;
	else if (attrib == EXISTING_PTS_TYPE_SQ)
		m_Type = eSQ;
	else
	{
		std::string msg(INVALID_VALUE);
		msg += attrib;
		msg += "\n";
		msg += VALID_VALUES;
		msg += EXISTING_PTS_TYPE_OTHER;
		msg += ", ";
		msg += EXISTING_PTS_TYPE_RUNS;
		msg += ", ";
		msg += EXISTING_PTS_TYPE_MACH;
		msg += ", ";
		msg += EXISTING_PTS_TYPE_QQ;
		msg += ", ";
		msg += EXISTING_PTS_TYPE_SQ;
		ioErrMsg += ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_TYPE, msg.c_str());
		return false;
	}

	if (eOtherPoints == m_Type)
	{
		if (CElement::eFound == inTree.GetAttrib(ATTRIB_EXISTING_PTS_OTHER, m_Other)
		&& 0 < m_Other.length())
		{
			if (!inConfig.GetOtherPoints().VerifyOtherPoints(m_Other))
			{
				std::string msg(INVALID_OTHER_PTS_NAME);
				msg += m_Other;
				ioErrMsg += ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_OTHER, msg.c_str());
				return false;
			}
		}
		else
		{
			ioErrMsg += ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_OTHER);
			return false;
		}
	}

	if (CElement::eFound != inTree.GetAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue)
	|| 0 == m_Venue.length())
	{
		ioErrMsg += ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_VENUE);
		return false;
	}

	if (CElement::eFound != inTree.GetAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div)
	|| 0 == m_Div.length())
	{
		ioErrMsg += ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_DIV);
		return false;
	}

	if (CElement::eFound != inTree.GetAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level)
	|| 0 == m_Level.length())
	{
		ioErrMsg += ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_LEVEL);
		return false;
	}

	// I could add additional verification to make sure it's a valid SQ/mach/QQ
	// event... but let the UI handle that...
	if (eOtherPoints == m_Type || eRuns == m_Type || eSQ == m_Type)
	{
		if (CElement::eFound != inTree.GetAttrib(ATTRIB_EXISTING_PTS_EVENT, m_Event)
		|| 0 == m_Event.length())
		{
			ioErrMsg += ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_EVENT);
			return false;
		}
		if (!inConfig.GetVenues().VerifyEvent(m_Venue, m_Div, m_Level, m_Event))
		{
			std::string msg(INVALID_VENUE_NAME);
			msg += m_Venue;
			msg += "/";
			msg += m_Div;
			msg += "/";
			msg += m_Level;
			msg += "/";
			msg += m_Event;
			ioErrMsg += ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_REG_NUM_VENUE, msg.c_str());
			return false;
		}
	}
	else
	{
		if (!inConfig.GetVenues().VerifyLevel(m_Venue, m_Div, m_Level))
		{
			std::string msg(INVALID_VENUE_NAME);
			msg += m_Venue;
			msg += "/";
			msg += m_Div;
			msg += "/";
			msg += m_Level;
			msg += "/";
			msg += m_Event;
			ioErrMsg += ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_REG_NUM_VENUE, msg.c_str());
			return false;
		}
	}

	inTree.GetAttrib(ATTRIB_EXISTING_PTS_POINTS, m_Points);

	m_Comment = inTree.GetValue();

	return true;
}

bool ARBDogExistingPoints::Save(CElement& ioTree) const
{
	CElement& title = ioTree.AddElement(TREE_EXISTING_PTS);
	title.AddAttrib(ATTRIB_EXISTING_PTS_DATE, m_Date);
	switch (m_Type)
	{
	default:
		ASSERT(0);
		break;
	case eOtherPoints:
		title.AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_OTHER);
		title.AddAttrib(ATTRIB_EXISTING_PTS_OTHER, m_Other);
		title.AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title.AddAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div);
		title.AddAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level);
		title.AddAttrib(ATTRIB_EXISTING_PTS_EVENT, m_Event);
		break;
	case eRuns:
		title.AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_RUNS);
		title.AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title.AddAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div);
		title.AddAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level);
		title.AddAttrib(ATTRIB_EXISTING_PTS_EVENT, m_Event);
		break;
	case eMach:
		title.AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_MACH);
		title.AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title.AddAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div);
		title.AddAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level);
		break;
	case eQQ:
		title.AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_QQ);
		title.AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title.AddAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div);
		title.AddAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level);
		break;
	case eSQ:
		title.AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_SQ);
		title.AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title.AddAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div);
		title.AddAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level);
		title.AddAttrib(ATTRIB_EXISTING_PTS_EVENT, m_Event);
		break;
	}
	title.AddAttrib(ATTRIB_EXISTING_PTS_POINTS, m_Points);
	if (0 < m_Comment.length())
		title.SetValue(m_Comment);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class SortExistingPoints
{
public:
	SortExistingPoints() {}
	bool operator()(ARBDogExistingPoints* one, ARBDogExistingPoints* two) const
	{
		if (one->GetOtherPoints() == two->GetOtherPoints())
		{
			if (one->GetVenue() == two->GetVenue())
			{
				if (one->GetDivision() == two->GetDivision())
				{
					if (one->GetLevel() == two->GetLevel())
						return (one->GetEvent() < two->GetEvent());
					else
						return (one->GetLevel() < two->GetLevel());
				}
				else
					return (one->GetDivision() < two->GetDivision());
			}
			else
				return (one->GetVenue() < two->GetVenue());
		}
		else
			return (one->GetOtherPoints() < two->GetOtherPoints());
	}
};

void ARBDogExistingPointsList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortExistingPoints());
}

bool ARBDogExistingPointsList::HasPoints(const std::string& inVenue) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetType() != ARBDogExistingPoints::eOtherPoints
		&& (*iter)->GetVenue() == inVenue)
			return true;
	}
	return false;
}

bool ARBDogExistingPointsList::HasPoints(
	const ARBConfigVenue* inVenue,
	const ARBConfigDivision* inDiv,
	const ARBConfigLevel* inLevel,
	const ARBConfigEvent* inEvent) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBDogExistingPoints::PointType type = (*iter)->GetType();
		if (ARBDogExistingPoints::eOtherPoints != type
		&& (*iter)->GetVenue() == inVenue->GetName()
		&& (*iter)->GetDivision() == inDiv->GetName()
		&& ((*iter)->GetLevel() == inLevel->GetName()
		|| inLevel->GetSubLevels().FindSubLevel((*iter)->GetLevel())))
		{
			if (ARBDogExistingPoints::eRuns == type
			|| ARBDogExistingPoints::eSQ == type)
			{
				if ((*iter)->GetEvent() == inEvent->GetName())
					return true;
			}
			else
				return true;
		}
	}
	return false;
}

short ARBDogExistingPointsList::ExistingPoints(
	ARBDogExistingPoints::PointType inType,
	const ARBConfigVenue* inVenue,
	const ARBConfigDivision* inDiv,
	const ARBConfigLevel* inLevel,
	const ARBConfigEvent* inEvent) const
{
	short pts = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetType() == inType)
		{
			if (inVenue && (*iter)->GetVenue() != inVenue->GetName())
				continue;
			if (inDiv && (*iter)->GetDivision() != inDiv->GetName())
				continue;
			if (inLevel && (*iter)->GetLevel() != inLevel->GetName()
			&& !inLevel->GetSubLevels().FindSubLevel((*iter)->GetLevel()))
				continue;
			if (inEvent && (*iter)->GetEvent() != inEvent->GetName())
				continue;
			pts = pts + (*iter)->GetPoints();
		}
	}
	return pts;
}

int ARBDogExistingPointsList::NumExistingPointsInVenue(const std::string& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue)
			++count;
	}
	return count;
}

int ARBDogExistingPointsList::RenameVenue(
	const std::string& inOldVenue,
	const std::string& inNewVenue)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inOldVenue)
		{
			(*iter)->SetVenue(inNewVenue);
			++count;
		}
	}
	return count;
}

int ARBDogExistingPointsList::DeleteVenue(const std::string& inVenue)
{
	std::string venue(inVenue);
	int count = 0;
	for (iterator iter = begin(); iter != end(); )
	{
		if ((*iter)->GetVenue() == venue)
		{
			iter = erase(iter);
			++count;
		}
		else
			++iter;
	}
	return count;
}

int ARBDogExistingPointsList::NumExistingPointsInDivision(
	const ARBConfigVenue* inVenue,
	const std::string& inDiv) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue->GetName()
		&& (*iter)->GetDivision() == inDiv)
			++count;
	}
	return count;
}

int ARBDogExistingPointsList::RenameDivision(
	const std::string& inVenue,
	const std::string& inOldDiv,
	const std::string& inNewDiv)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetDivision() == inOldDiv)
		{
			(*iter)->SetDivision(inNewDiv);
			++count;
		}
	}
	return count;
}

int ARBDogExistingPointsList::DeleteDivision(
	const std::string& inVenue,
	const std::string& inDiv)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); )
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetDivision() == inDiv)
		{
			++count;
			iter = erase(iter);
		}
		else
			++iter;
	}
	return count;
}

int ARBDogExistingPointsList::NumLevelsInUse(
	const std::string& inVenue,
	const std::string& inDiv,
	const std::string& inLevel) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue
		&& (*iter)->GetDivision() == inDiv
		&& (*iter)->GetLevel() == inLevel)
			++count;
	}
	return count;
}

int ARBDogExistingPointsList::RenameLevel(
	const std::string& inVenue,
	const std::string& inDiv,
	const std::string& inOldLevel,
	const std::string& inNewLevel)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue
		&& (*iter)->GetDivision() == inDiv
		&& (*iter)->GetLevel() == inOldLevel)
		{
			++count;
			(*iter)->SetLevel(inNewLevel);
		}
	}
	return count;
}

int ARBDogExistingPointsList::DeleteLevel(
	const std::string& inVenue,
	const std::string& inDiv,
	const std::string& inLevel)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); )
	{
		if ((*iter)->GetVenue() == inVenue
		&& (*iter)->GetDivision() == inDiv
		&& (*iter)->GetLevel() == inLevel)
		{
			++count;
			iter = erase(iter);
		}
		else
			++iter;
	}
	return count;
}

int ARBDogExistingPointsList::NumEventsInUse(
	const std::string& inVenue,
	const std::string& inEvent) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue
		&& (*iter)->GetEvent() == inEvent)
			++count;
	}
	return count;
}

int ARBDogExistingPointsList::RenameEvent(
	const std::string& inVenue,
	const std::string& inOldEvent,
	const std::string& inNewEvent)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue
		&& (*iter)->GetEvent() == inOldEvent)
		{
			++count;
			(*iter)->SetEvent(inNewEvent);
		}
	}
	return count;
}

int ARBDogExistingPointsList::DeleteEvent(
	const std::string& inVenue,
	const std::string& inEvent)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); )
	{
		if ((*iter)->GetVenue() == inVenue
		&& (*iter)->GetEvent() == inEvent)
		{
			++count;
			iter = erase(iter);
		}
		else
			++iter;
	}
	return count;
}

int ARBDogExistingPointsList::NumOtherPointsInUse(const std::string& inOther) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetOtherPoints() == inOther)
			++count;
	}
	return count;
}

int ARBDogExistingPointsList::RenameOtherPoints(
	const std::string& inOldOther,
	const std::string& inNewOther)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetOtherPoints() == inOldOther)
		{
			(*iter)->SetOtherPoints(inNewOther);
			++count;
		}
	}
	return count;
}

int ARBDogExistingPointsList::DeleteOtherPoints(const std::string& inOther)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); )
	{
		if ((*iter)->GetOtherPoints() == inOther)
		{
			iter = erase(iter);
			++count;
		}
		else
			++iter;
	}
	return count;
}

ARBDogExistingPoints* ARBDogExistingPointsList::AddExistingPoints(ARBDogExistingPoints* inExistingPoints)
{
	if (inExistingPoints)
	{
		inExistingPoints->AddRef();
		push_back(inExistingPoints);
	}
	return inExistingPoints;
}

bool ARBDogExistingPointsList::DeleteExistingPoints(const ARBDogExistingPoints* inExistingPoints)
{
	if (inExistingPoints)
	{
		for (iterator iter = begin(); iter != end(); ++iter)
		{
			if (*(*iter) == *inExistingPoints)
			{
				erase(iter);
				return true;
			}
		}
	}
	return false;
}
