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

ARBDogExistingPoints::ARBDogExistingPoints()
	: m_Other()
	, m_Venue()
	, m_Div()
	, m_Level()
	, m_Event()
	, m_Points(0)
{
}

ARBDogExistingPoints::ARBDogExistingPoints(const ARBDogExistingPoints& rhs)
	: m_Other(rhs.m_Other)
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
	return m_Other == rhs.m_Other
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
	if (CElement::eFound == inTree.GetAttrib(ATTRIB_EXISTING_PTS_OTHER, m_Other)
	&& 0 == m_Other.length())
	{
		if (!inConfig.GetOtherPoints().VerifyOtherPoints(m_Other))
		{
			std::string msg(INVALID_OTHER_PTS_NAME);
			msg += m_Other;
			ioErrMsg += ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_OTHER, msg.c_str());
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

	inTree.GetAttrib(ATTRIB_EXISTING_PTS_POINTS, m_Points);

	return true;
}

bool ARBDogExistingPoints::Save(CElement& ioTree) const
{
	CElement& title = ioTree.AddElement(TREE_EXISTING_PTS);
	if (0 < m_Other.length())
		title.AddAttrib(ATTRIB_EXISTING_PTS_OTHER, m_Other);
	title.AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
	title.AddAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div);
	title.AddAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level);
	title.AddAttrib(ATTRIB_EXISTING_PTS_EVENT, m_Event);
	title.AddAttrib(ATTRIB_EXISTING_PTS_POINTS, m_Points);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class SortExistingPoints
{
public:
	SortExistingPoints() {}
	bool operator()(ARBDogExistingPoints* one, ARBDogExistingPoints* two) const
	{
		if (one->GetOther() == two->GetOther())
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
			return (one->GetOther() < two->GetOther());
	}
};

void ARBDogExistingPointsList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortExistingPoints());
}

long ARBDogExistingPointsList::ExistingPoints(
	const std::string& inOther,
	const std::string& inVenue,
	const std::string& inDiv,
	const std::string& inLevel,
	const std::string& inEvent) const
{
	long pts = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetOther() == inOther
		&& (*iter)->GetVenue() == inVenue
		&& (*iter)->GetDivision() == inDiv
		&& (*iter)->GetLevel() == inLevel
		&& (*iter)->GetEvent() == inEvent)
			pts += (*iter)->GetPoints();
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
	const std::string& inVenue,
	const std::string& inDiv) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetDivision() == inDiv)
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

int ARBDogExistingPointsList::NumOtherPointsInUse(const std::string& inOther) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetOther() == inOther)
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
		if ((*iter)->GetOther() == inOldOther)
		{
			(*iter)->SetOther(inNewOther);
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
		if ((*iter)->GetOther() == inOther)
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
