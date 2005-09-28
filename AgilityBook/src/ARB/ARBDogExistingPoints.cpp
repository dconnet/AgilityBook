/*
 * Copyright © 2004-2005 David Connet. All Rights Reserved.
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
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
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
		str = EXISTING_POINTS_OTHER;
		break;
	case eRuns:
		str = EXISTING_POINTS_RUN;
		break;
	case eSpeed:
		str = EXISTING_POINTS_SPEED;
		break;
	case eMQ:
		str = EXISTING_POINTS_MQ;
		break;
	case eSQ:
		str = EXISTING_POINTS_SQ;
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
	, m_MultiQ()
	, m_Div()
	, m_Level()
	, m_Event()
	, m_SubName()
	, m_Points(0)
{
}

ARBDogExistingPoints::ARBDogExistingPoints(ARBDogExistingPoints const& rhs)
	: m_Date(rhs.m_Date)
	, m_Comment(rhs.m_Comment)
	, m_Type(rhs.m_Type)
	, m_Other(rhs.m_Other)
	, m_Venue(rhs.m_Venue)
	, m_MultiQ(rhs.m_MultiQ)
	, m_Div(rhs.m_Div)
	, m_Level(rhs.m_Level)
	, m_Event(rhs.m_Event)
	, m_SubName(rhs.m_SubName)
	, m_Points(rhs.m_Points)
{
}

ARBDogExistingPoints::~ARBDogExistingPoints()
{
}

ARBDogExistingPoints& ARBDogExistingPoints::operator=(ARBDogExistingPoints const& rhs)
{
	if (this != &rhs)
	{
		m_Date = rhs.m_Date;
		m_Comment = rhs.m_Comment;
		m_Type = rhs.m_Type;
		m_Other = rhs.m_Other;
		m_Venue = rhs.m_Venue;
		m_MultiQ = rhs.m_MultiQ;
		m_Div = rhs.m_Div;
		m_Level = rhs.m_Level;
		m_Event = rhs.m_Event;
		m_SubName = rhs.m_SubName;
		m_Points = rhs.m_Points;
	}
	return *this;
}

bool ARBDogExistingPoints::operator==(ARBDogExistingPoints const& rhs) const
{
	return m_Date == rhs.m_Date
		&& m_Comment == rhs.m_Comment
		&& m_Type == rhs.m_Type
		&& m_Other == rhs.m_Other
		&& m_Venue == rhs.m_Venue
		&& m_MultiQ == rhs.m_MultiQ
		&& m_Div == rhs.m_Div
		&& m_Level == rhs.m_Level
		&& m_Event == rhs.m_Event
		&& m_SubName == rhs.m_SubName
		&& m_Points == rhs.m_Points;
}

bool ARBDogExistingPoints::operator!=(ARBDogExistingPoints const& rhs) const
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
		ARBConfig const& inConfig,
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	std::string attrib;

	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_EXISTING_PTS_DATE, m_Date))
	{
		inTree.GetAttrib(ATTRIB_EXISTING_PTS_DATE, attrib);
		std::string msg(INVALID_DATE);
		msg += attrib;
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_DATE, msg.c_str()));
		return false;
	}

	if (Element::eFound != inTree.GetAttrib(ATTRIB_EXISTING_PTS_TYPE, attrib)
	|| 0 == attrib.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_TYPE));
		return false;
	}
	bool bConvertedQQ = false;
	if (attrib == EXISTING_PTS_TYPE_OTHER)
		m_Type = eOtherPoints;
	else if (attrib == EXISTING_PTS_TYPE_RUNS)
		m_Type = eRuns;
	else if (attrib == EXISTING_PTS_TYPE_SPEED)
		m_Type = eSpeed;
	else if (attrib == EXISTING_PTS_TYPE_MQ)
		m_Type = eMQ;
	else if (attrib == EXISTING_PTS_TYPE_SQ)
		m_Type = eSQ;
	// Changed attribute from 'Mach' to 'Speed' in 10.1
	else if (inVersion < ARBVersion(10,1) && attrib == "Mach")
		m_Type = eSpeed;
	// Changed QQ to MQ (multi Q)
	else if (inVersion < ARBVersion(11,0) && attrib == "QQ")
	{
		m_Type = eMQ;
		bConvertedQQ = true;
	}
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
		msg += EXISTING_PTS_TYPE_SPEED;
		msg += ", ";
		msg += EXISTING_PTS_TYPE_MQ;
		msg += ", ";
		msg += EXISTING_PTS_TYPE_SQ;
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_TYPE, msg.c_str()));
		return false;
	}

	if (eOtherPoints == m_Type)
	{
		if (Element::eFound == inTree.GetAttrib(ATTRIB_EXISTING_PTS_OTHER, m_Other)
		&& 0 < m_Other.length())
		{
			if (!inConfig.GetOtherPoints().VerifyOtherPoints(m_Other))
			{
				std::string msg(INVALID_OTHER_PTS_NAME);
				msg += m_Other;
				ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_OTHER, msg.c_str()));
				return false;
			}
		}
		else
		{
			ioCallback.LogMessage(ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_OTHER));
			return false;
		}
	}

	if (Element::eFound == inTree.GetAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue)
	&& 0 < m_Venue.length())
	{
		if (!inConfig.GetVenues().VerifyVenue(m_Venue))
		{
			std::string msg(INVALID_VENUE_NAME);
			msg += m_Venue;
			ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_VENUE, msg.c_str()));
			return false;
		}
	}
	else
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_VENUE));
		return false;
	}

	if (eMQ == m_Type)
	{
		if (bConvertedQQ)
		{
			bConvertedQQ = false;
			ARBConfigVenue* pVenue;
			// Note, m_Venue should be "AKC".
			// Otherwise, we're dealing with a venue I don't know.
			if (inConfig.GetVenues().FindVenue(m_Venue, &pVenue))
			{
				ARBConfigMultiQ* pMulti;
				// If this isn't "AKC", or someone has edited the multiQ name,
				// this will fail.
				if (pVenue->GetMultiQs().FindMultiQ("QQ", true, &pMulti))
				{
					bConvertedQQ = true;
					m_MultiQ = pMulti->GetName();
					pMulti->Release();
				}
				// If we don't find it, assume AKC and hard code this in.
				// The problem is when we read this into the 11.0 file version,
				// the configuration hasn't been updated yet - so the multiQ
				// info doesn't exist yet!
				else
				{
					bConvertedQQ = true;
					m_MultiQ = "Double Q";
				}
				pVenue->Release();
			}
			if (!bConvertedQQ)
			{
				ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_MULTIQ, INVALID_MULTIQ_CONVERSION));
				return false;
			}
		}
		else
		{
			if (Element::eFound == inTree.GetAttrib(ATTRIB_EXISTING_PTS_MULTIQ, m_MultiQ)
			&& 0 < m_MultiQ.length())
			{
				if (!inConfig.GetVenues().VerifyMultiQ(m_Venue, m_MultiQ))
				{
					std::string msg(INVALID_MULTIQ_NAME);
					msg += m_Venue;
					msg += "/";
					msg += m_MultiQ;
					ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_MULTIQ, msg.c_str()));
					return false;
				}
			}
			else
			{
				ioCallback.LogMessage(ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_MULTIQ));
				return false;
			}
		}
	}

	else
	{
		if (Element::eFound != inTree.GetAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div)
		|| 0 == m_Div.length())
		{
			ioCallback.LogMessage(ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_DIV));
			return false;
		}

		if (Element::eFound != inTree.GetAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level)
		|| 0 == m_Level.length())
		{
			ioCallback.LogMessage(ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_LEVEL));
			return false;
		}
	}

	// I could add additional verification to make sure it's a valid SQ/Speed/MQ
	// event... but let the UI handle that...
	switch (m_Type)
	{
	case eMQ:
		break;
	case eOtherPoints:
	case eRuns:
	case eSQ:
		if (Element::eFound != inTree.GetAttrib(ATTRIB_EXISTING_PTS_EVENT, m_Event)
		|| 0 == m_Event.length())
		{
			ioCallback.LogMessage(ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_EVENT));
			return false;
		}
		if (!inConfig.GetVenues().VerifyEvent(m_Venue, m_Div, m_Level, m_Event))
		{
			std::string msg(INVALID_EVENT_NAME);
			msg += m_Venue;
			msg += "/";
			msg += m_Div;
			msg += "/";
			msg += m_Level;
			msg += "/";
			msg += m_Event;
			ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_EVENT, msg.c_str()));
			return false;
		}
		// Only used in eRuns.
		inTree.GetAttrib(ATTRIB_EXISTING_PTS_SUBNAME, m_SubName);
		break;
	default:
		if (!inConfig.GetVenues().VerifyLevel(m_Venue, m_Div, m_Level))
		{
			std::string msg(INVALID_VENUE_NAME);
			msg += m_Venue;
			msg += "/";
			msg += m_Div;
			msg += "/";
			msg += m_Level;
			ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_VENUE, msg.c_str()));
			return false;
		}
		break;
	}

	inTree.GetAttrib(ATTRIB_EXISTING_PTS_POINTS, m_Points);

	m_Comment = inTree.GetValue();

	return true;
}

bool ARBDogExistingPoints::Save(Element& ioTree) const
{
	Element& title = ioTree.AddElement(TREE_EXISTING_PTS);
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
		title.AddAttrib(ATTRIB_EXISTING_PTS_SUBNAME, m_SubName);
		break;
	case eSpeed:
		title.AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_SPEED);
		title.AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title.AddAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div);
		title.AddAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level);
		break;
	case eMQ:
		title.AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_MQ);
		title.AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title.AddAttrib(ATTRIB_EXISTING_PTS_MULTIQ, m_MultiQ);
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
						return one->GetEvent() < two->GetEvent();
					else
						return one->GetLevel() < two->GetLevel();
				}
				else
					return one->GetDivision() < two->GetDivision();
			}
			else
				return one->GetVenue() < two->GetVenue();
		}
		else
			return one->GetOtherPoints() < two->GetOtherPoints();
	}
};

void ARBDogExistingPointsList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortExistingPoints());
}

bool ARBDogExistingPointsList::HasPoints(std::string const& inVenue) const
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
		ARBConfigVenue const* inVenue,
		ARBConfigDivision const* inDiv,
		ARBConfigLevel const* inLevel,
		ARBConfigEvent const* inEvent,
		bool inHasLifetime) const
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
				{
					if (inHasLifetime)
					{
						ARBConfigScoring* pScoring;
						if (inEvent->GetScorings().FindEvent(inDiv->GetName(), inLevel->GetName(), (*iter)->GetDate(), &pScoring))
						{
							if (0 < pScoring->GetLifetimePoints().GetLifetimePoints(0.0))
							{
								pScoring->Release();
								return true;
							}
							pScoring->Release();
						}
					}
					else
						return true;
				}
			}
			else
				return true;
		}
	}
	return false;
}

short ARBDogExistingPointsList::ExistingPoints(
		ARBDogExistingPoints::PointType inType,
		ARBConfigVenue const* inVenue,
		ARBConfigMultiQ const* inMultiQ,
		ARBConfigDivision const* inDiv,
		ARBConfigLevel const* inLevel,
		ARBConfigEvent const* inEvent) const
{
	short pts = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetType() == inType)
		{
			if (inVenue && (*iter)->GetVenue() != inVenue->GetName())
				continue;
			if (inMultiQ && (*iter)->GetMultiQ() != inMultiQ->GetName())
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

int ARBDogExistingPointsList::NumExistingPointsInVenue(std::string const& inVenue) const
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
		std::string const& inOldVenue,
		std::string const& inNewVenue)
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

int ARBDogExistingPointsList::DeleteVenue(std::string const& inVenue)
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
		ARBConfigVenue const* inVenue,
		std::string const& inDiv) const
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
		std::string const& inVenue,
		std::string const& inOldDiv,
		std::string const& inNewDiv)
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
		std::string const& inVenue,
		std::string const& inDiv)
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
		std::string const& inVenue,
		std::string const& inDiv,
		std::string const& inLevel) const
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
		std::string const& inVenue,
		std::string const& inDiv,
		std::string const& inOldLevel,
		std::string const& inNewLevel)
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
		std::string const& inVenue,
		std::string const& inDiv,
		std::string const& inLevel)
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
		std::string const& inVenue,
		std::string const& inEvent) const
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
		std::string const& inVenue,
		std::string const& inOldEvent,
		std::string const& inNewEvent)
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
		std::string const& inVenue,
		std::string const& inEvent)
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

int ARBDogExistingPointsList::NumOtherPointsInUse(std::string const& inOther) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (ARBDogExistingPoints::eOtherPoints == (*iter)->GetType()
		&& (*iter)->GetOtherPoints() == inOther)
			++count;
	}
	return count;
}

int ARBDogExistingPointsList::RenameOtherPoints(
		std::string const& inOldOther,
		std::string const& inNewOther)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if (ARBDogExistingPoints::eOtherPoints == (*iter)->GetType()
		&& (*iter)->GetOtherPoints() == inOldOther)
		{
			(*iter)->SetOtherPoints(inNewOther);
			++count;
		}
	}
	return count;
}

int ARBDogExistingPointsList::DeleteOtherPoints(std::string const& inOther)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); )
	{
		if (ARBDogExistingPoints::eOtherPoints == (*iter)->GetType()
		&& (*iter)->GetOtherPoints() == inOther)
		{
			iter = erase(iter);
			++count;
		}
		else
			++iter;
	}
	return count;
}

int ARBDogExistingPointsList::NumMultiQsInUse(
		std::string const& inVenue,
		std::string const& inMultiQ) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (ARBDogExistingPoints::eMQ == (*iter)->GetType()
		&& (*iter)->GetVenue() == inVenue
		&& (*iter)->GetMultiQ() == inMultiQ)
			++count;
	}
	return count;
}

int ARBDogExistingPointsList::RenameMultiQs(
		std::string const& inVenue,
		std::string const& inOldMultiQ,
		std::string const& inNewMultiQ)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if (ARBDogExistingPoints::eMQ == (*iter)->GetType()
		&& (*iter)->GetVenue() == inVenue
		&& (*iter)->GetMultiQ() == inOldMultiQ)
		{
			(*iter)->SetMultiQ(inNewMultiQ);
			++count;
		}
	}
	return count;
}

int ARBDogExistingPointsList::DeleteMultiQs(
		ARBConfig const& inConfig,
		std::string const& inVenue)
{
	int count = 0;
	ARBConfigVenue* pVenue;
	if (inConfig.GetVenues().FindVenue(inVenue, &pVenue))
	{
		for (iterator iter = begin(); iter != end(); )
		{
			if (ARBDogExistingPoints::eMQ == (*iter)->GetType()
			&& !pVenue->GetMultiQs().FindMultiQ((*iter)->GetMultiQ(), true))
			{
				iter = erase(iter);
				++count;
			}
			else
				++iter;
		}
		pVenue->Release();
	}
	return count;
}

bool ARBDogExistingPointsList::AddExistingPoints(ARBDogExistingPoints* inExistingPoints)
{
	bool bAdded = false;
	if (inExistingPoints)
	{
		bAdded = true;
		inExistingPoints->AddRef();
		push_back(inExistingPoints);
	}
	return bAdded;
}

bool ARBDogExistingPointsList::DeleteExistingPoints(ARBDogExistingPoints const* inExistingPoints)
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
