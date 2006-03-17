/*
 * Copyright © 2004-2006 David Connet. All Rights Reserved.
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
ARBString ARBDogExistingPoints::GetPointTypeName(ARBDogExistingPoints::PointType inType)
{
	ARBString str;
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

bool ARBDogExistingPoints::Load(
		ARBConfig const& inConfig,
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBString attrib;

	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_EXISTING_PTS_DATE, m_Date))
	{
		inTree.GetAttrib(ATTRIB_EXISTING_PTS_DATE, attrib);
		ARBString msg(INVALID_DATE);
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
	else if (inVersion < ARBVersion(10,1) && attrib == _T("Mach"))
		m_Type = eSpeed;
	// Changed QQ to MQ (multi Q)
	else if (inVersion < ARBVersion(11,0) && attrib == _T("QQ"))
	{
		m_Type = eMQ;
		bConvertedQQ = true;
	}
	else
	{
		ARBString msg(INVALID_VALUE);
		msg += attrib;
		msg += _T("\n");
		msg += VALID_VALUES;
		msg += EXISTING_PTS_TYPE_OTHER;
		msg += _T(", ");
		msg += EXISTING_PTS_TYPE_RUNS;
		msg += _T(", ");
		msg += EXISTING_PTS_TYPE_SPEED;
		msg += _T(", ");
		msg += EXISTING_PTS_TYPE_MQ;
		msg += _T(", ");
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
				ARBString msg(INVALID_OTHER_PTS_NAME);
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
			ARBString msg(INVALID_VENUE_NAME);
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
			ARBConfigVenuePtr pVenue;
			// Note, m_Venue should be "AKC".
			// Otherwise, we're dealing with a venue I don't know.
			if (inConfig.GetVenues().FindVenue(m_Venue, &pVenue))
			{
				ARBConfigMultiQPtr pMulti;
				// If this isn't "AKC", or someone has edited the multiQ name,
				// this will fail.
				if (pVenue->GetMultiQs().FindMultiQ(_T("QQ"), true, &pMulti))
				{
					bConvertedQQ = true;
					m_MultiQ = pMulti->GetName();
				}
				// If we don't find it, assume AKC and hard code this in.
				// The problem is when we read this into the 11.0 file version,
				// the configuration hasn't been updated yet - so the multiQ
				// info doesn't exist yet!
				else
				{
					bConvertedQQ = true;
					m_MultiQ = _T("Double Q");
				}
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
					ARBString msg(INVALID_MULTIQ_NAME);
					msg += m_Venue;
					msg += _T("/");
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
		if (!inConfig.GetVenues().VerifyEvent(m_Venue, m_Div, m_Level, m_Event, m_Date))
		{
			ARBString msg(INVALID_EVENT_NAME);
			msg += m_Venue;
			msg += _T("/");
			msg += m_Div;
			msg += _T("/");
			msg += m_Level;
			msg += _T("/");
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
			ARBString msg(INVALID_VENUE_NAME);
			msg += m_Venue;
			msg += _T("/");
			msg += m_Div;
			msg += _T("/");
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

bool ARBDogExistingPointsList::Load(
		ARBConfig const& inConfig,
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBDogExistingPointsPtr thing(ARBDogExistingPoints::New());
	if (!thing->Load(inConfig, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}

class SortExistingPoints
{
public:
	SortExistingPoints() {}
	bool operator()(ARBDogExistingPointsPtr one, ARBDogExistingPointsPtr two) const
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

bool ARBDogExistingPointsList::HasPoints(ARBString const& inVenue) const
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
		ARBConfigVenuePtr inVenue,
		ARBConfigDivisionPtr inDiv,
		ARBConfigLevelPtr inLevel,
		ARBConfigEventPtr inEvent,
		ARBDate inDateFrom,
		ARBDate inDateTo,
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
			if ((inDateFrom.IsValid() && (*iter)->GetDate() < inDateFrom)
			|| (inDateTo.IsValid() && (*iter)->GetDate() > inDateTo))
				return false;
			if (ARBDogExistingPoints::eRuns == type
			|| ARBDogExistingPoints::eSQ == type)
			{
				if ((*iter)->GetEvent() == inEvent->GetName())
				{
					if (inHasLifetime)
					{
						ARBConfigScoringPtr pScoring;
						if (inEvent->GetScorings().FindEvent(inDiv->GetName(), inLevel->GetName(), (*iter)->GetDate(), &pScoring))
						{
							if (0 < pScoring->GetLifetimePoints().GetLifetimePoints(0.0))
								return true;
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

double ARBDogExistingPointsList::ExistingPoints(
		ARBDogExistingPoints::PointType inType,
		ARBConfigVenuePtr inVenue,
		ARBConfigMultiQPtr inMultiQ,
		ARBConfigDivisionPtr inDiv,
		ARBConfigLevelPtr inLevel,
		ARBConfigEventPtr inEvent,
		ARBDate inDateFrom,
		ARBDate inDateTo) const
{
	double pts = 0.0;
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
			// If we made it this far, we now need to check against the scoring method for dates.
			if ((inDateFrom.IsValid() && (*iter)->GetDate() < inDateFrom)
			|| (inDateTo.IsValid() && (*iter)->GetDate() > inDateTo))
				continue;
			pts += (*iter)->GetPoints();
		}
	}
	return pts;
}

int ARBDogExistingPointsList::NumExistingPointsInVenue(ARBString const& inVenue) const
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
		ARBString const& inOldVenue,
		ARBString const& inNewVenue)
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

int ARBDogExistingPointsList::DeleteVenue(ARBString const& inVenue)
{
	ARBString venue(inVenue);
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
		ARBConfigVenuePtr inVenue,
		ARBString const& inDiv) const
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
		ARBString const& inVenue,
		ARBString const& inOldDiv,
		ARBString const& inNewDiv)
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
		ARBString const& inVenue,
		ARBString const& inDiv)
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
		ARBString const& inVenue,
		ARBString const& inDiv,
		ARBString const& inLevel) const
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
		ARBString const& inVenue,
		ARBString const& inDiv,
		ARBString const& inOldLevel,
		ARBString const& inNewLevel)
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
		ARBString const& inVenue,
		ARBString const& inDiv,
		ARBString const& inLevel)
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
		ARBString const& inVenue,
		ARBString const& inEvent) const
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
		ARBString const& inVenue,
		ARBString const& inOldEvent,
		ARBString const& inNewEvent)
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
		ARBString const& inVenue,
		ARBString const& inEvent)
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

int ARBDogExistingPointsList::NumOtherPointsInUse(ARBString const& inOther) const
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
		ARBString const& inOldOther,
		ARBString const& inNewOther)
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

int ARBDogExistingPointsList::DeleteOtherPoints(ARBString const& inOther)
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
		ARBString const& inVenue,
		ARBString const& inMultiQ) const
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
		ARBString const& inVenue,
		ARBString const& inOldMultiQ,
		ARBString const& inNewMultiQ)
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
		ARBString const& inVenue)
{
	int count = 0;
	ARBConfigVenuePtr pVenue;
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
	}
	return count;
}

bool ARBDogExistingPointsList::AddExistingPoints(ARBDogExistingPointsPtr inExistingPoints)
{
	bool bAdded = false;
	if (inExistingPoints)
	{
		bAdded = true;
		push_back(inExistingPoints);
	}
	return bAdded;
}

bool ARBDogExistingPointsList::DeleteExistingPoints(ARBDogExistingPointsPtr inExistingPoints)
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
