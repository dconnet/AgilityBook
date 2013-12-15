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
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-02-02 Created
 */

#include "stdafx.h"
#include "ARB/ARBDogExistingPoints.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// static

#define EXISTING_PTS_TYPE_OTHER	L"Other"
#define EXISTING_PTS_TYPE_RUNS	L"Run"
#define EXISTING_PTS_TYPE_SPEED	L"Speed"
#define EXISTING_PTS_TYPE_MQ	L"MQ"
#define EXISTING_PTS_TYPE_SQ	L"SQ"

std::wstring ARBDogExistingPoints::GetPointTypeName(ARBDogExistingPoints::PointType inType)
{
	std::wstring str;
	switch (inType)
	{
	default:
		break;
	case eOtherPoints:
		str = Localization()->ExistingPointsOther();
		break;
	case eRuns:
		str = Localization()->ExistingPointsRun();
		break;
	case eSpeed:
		str = Localization()->ExistingPointsSpeed();
		break;
	case eMQ:
		str = Localization()->ExistingPointsMQ();
		break;
	case eSQ:
		str = Localization()->ExistingPointsSQ();
		break;
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class ARBDogExistingPoints_concrete : public ARBDogExistingPoints
	{
	public:
		ARBDogExistingPoints_concrete() {}
		ARBDogExistingPoints_concrete(ARBDogExistingPoints const& rhs)
			: ARBDogExistingPoints(rhs)
		{
		}
	};
};


ARBDogExistingPointsPtr ARBDogExistingPoints::New()
{
	return std::make_shared<ARBDogExistingPoints_concrete>();
}


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


ARBDogExistingPointsPtr ARBDogExistingPoints::Clone() const
{
	return std::make_shared<ARBDogExistingPoints_concrete>(*this);
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
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_EXISTING_PTS)
		return false;

	std::wstring attrib;

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_EXISTING_PTS_DATE, m_Date))
	{
		inTree->GetAttrib(ATTRIB_EXISTING_PTS_DATE, attrib);
		std::wstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_DATE, msg.c_str()));
		return false;
	}

	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_EXISTING_PTS_TYPE, attrib)
	|| 0 == attrib.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_TYPE));
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
	else if (inVersion < ARBVersion(10,1) && attrib == L"Mach")
		m_Type = eSpeed;
	// Changed QQ to MQ (multi Q)
	else if (inVersion < ARBVersion(11,0) && attrib == L"QQ")
	{
		m_Type = eMQ;
		bConvertedQQ = true;
	}
	else
	{
		std::wstring msg(Localization()->ValidValues());
		msg += EXISTING_PTS_TYPE_OTHER;
		msg += L", ";
		msg += EXISTING_PTS_TYPE_RUNS;
		msg += L", ";
		msg += EXISTING_PTS_TYPE_SPEED;
		msg += L", ";
		msg += EXISTING_PTS_TYPE_MQ;
		msg += L", ";
		msg += EXISTING_PTS_TYPE_SQ;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_TYPE, msg.c_str()));
		return false;
	}

	if (eOtherPoints == m_Type)
	{
		if (ElementNode::eFound == inTree->GetAttrib(ATTRIB_EXISTING_PTS_OTHER, m_Other)
		&& 0 < m_Other.length())
		{
			if (!inConfig.GetOtherPoints().VerifyOtherPoints(m_Other))
			{
				std::wstring msg(Localization()->InvalidOtherPtsName());
				msg += m_Other;
				ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_OTHER, msg.c_str()));
				return false;
			}
		}
		else
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_OTHER));
			return false;
		}
	}

	if (ElementNode::eFound == inTree->GetAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue)
	&& 0 < m_Venue.length())
	{
		if (!inConfig.GetVenues().VerifyVenue(m_Venue))
		{
			std::wstring msg(Localization()->InvalidVenueName());
			msg += m_Venue;
			ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_VENUE, msg.c_str()));
			return false;
		}
	}
	else
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_VENUE));
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
				if (pVenue->GetMultiQs().FindMultiQ(L"QQ", true, &pMulti))
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
					m_MultiQ = L"Double Q";
				}
			}
			if (!bConvertedQQ)
			{
				ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_MULTIQ, Localization()->InvalidMultiqConversion().c_str()));
				return false;
			}
		}
		else
		{
			if (ElementNode::eFound == inTree->GetAttrib(ATTRIB_EXISTING_PTS_MULTIQ, m_MultiQ)
			&& 0 < m_MultiQ.length())
			{
				if (!inConfig.GetVenues().VerifyMultiQ(m_Venue, m_MultiQ))
				{
					std::wstring msg(Localization()->InvalidMultiqName());
					msg += m_Venue;
					msg += L"/";
					msg += m_MultiQ;
					ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_MULTIQ, msg.c_str()));
					return false;
				}
			}
			else
			{
				ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_MULTIQ));
				return false;
			}
		}
	}

	else
	{
		if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div)
		|| 0 == m_Div.length())
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_DIV));
			return false;
		}

		if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level)
		|| 0 == m_Level.length())
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_LEVEL));
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
		if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_EXISTING_PTS_EVENT, m_Event)
		|| 0 == m_Event.length())
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_EVENT));
			return false;
		}
		if (!inConfig.GetVenues().VerifyEvent(m_Venue, m_Div, m_Level, m_Event, m_Date))
		{
			std::wstring msg(Localization()->InvalidEventName());
			msg += m_Venue;
			msg += L"/";
			msg += m_Div;
			msg += L"/";
			msg += m_Level;
			msg += L"/";
			msg += m_Event;
			ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_EVENT, msg.c_str()));
			return false;
		}
		// Only used in eRuns.
		inTree->GetAttrib(ATTRIB_EXISTING_PTS_SUBNAME, m_SubName);
		break;
	default:
		if (!inConfig.GetVenues().VerifyLevel(m_Venue, m_Div, m_Level))
		{
			std::wstring msg(Localization()->InvalidVenueName());
			msg += m_Venue;
			msg += L"/";
			msg += m_Div;
			msg += L"/";
			msg += m_Level;
			ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_VENUE, msg.c_str()));
			return false;
		}
		break;
	}

	inTree->GetAttrib(ATTRIB_EXISTING_PTS_POINTS, m_Points);

	m_Comment = inTree->GetValue();

	return true;
}


bool ARBDogExistingPoints::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr title = ioTree->AddElementNode(TREE_EXISTING_PTS);
	title->AddAttrib(ATTRIB_EXISTING_PTS_DATE, m_Date);
	switch (m_Type)
	{
	default:
		assert(0);
		break;
	case eOtherPoints:
		title->AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_OTHER);
		title->AddAttrib(ATTRIB_EXISTING_PTS_OTHER, m_Other);
		title->AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title->AddAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div);
		title->AddAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level);
		title->AddAttrib(ATTRIB_EXISTING_PTS_EVENT, m_Event);
		break;
	case eRuns:
		title->AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_RUNS);
		title->AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title->AddAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div);
		title->AddAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level);
		title->AddAttrib(ATTRIB_EXISTING_PTS_EVENT, m_Event);
		title->AddAttrib(ATTRIB_EXISTING_PTS_SUBNAME, m_SubName);
		break;
	case eSpeed:
		title->AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_SPEED);
		title->AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title->AddAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div);
		title->AddAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level);
		break;
	case eMQ:
		title->AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_MQ);
		title->AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title->AddAttrib(ATTRIB_EXISTING_PTS_MULTIQ, m_MultiQ);
		break;
	case eSQ:
		title->AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_SQ);
		title->AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title->AddAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div);
		title->AddAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level);
		title->AddAttrib(ATTRIB_EXISTING_PTS_EVENT, m_Event);
		break;
	}
	title->AddAttrib(ATTRIB_EXISTING_PTS_POINTS, m_Points);
	if (0 < m_Comment.length())
		title->SetValue(m_Comment);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBDogExistingPointsList::Load(
		ARBConfig const& inConfig,
		ElementNodePtr inTree,
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
	bool operator()(ARBDogExistingPointsPtr const& one, ARBDogExistingPointsPtr const& two) const
	{
		if (one->GetOtherPoints() == two->GetOtherPoints())
		{
			if (one->GetVenue() == two->GetVenue())
			{
				if (one->GetEvent() == two->GetEvent())
				{
					if (one->GetDate() == two->GetDate())
					{
						if (one->GetDivision() == two->GetDivision())
							return one->GetLevel() < two->GetLevel();
						else
							return one->GetDivision() < two->GetDivision();
					}
					else
						return one->GetDate() < two->GetDate();
				}
				else
					return one->GetEvent() < two->GetEvent();
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


bool ARBDogExistingPointsList::HasPoints(std::wstring const& inVenue) const
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


int ARBDogExistingPointsList::NumExistingPointsInVenue(std::wstring const& inVenue) const
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
		std::wstring const& inOldVenue,
		std::wstring const& inNewVenue)
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


int ARBDogExistingPointsList::DeleteVenue(std::wstring const& inVenue)
{
	std::wstring venue(inVenue);
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
		std::wstring const& inDiv) const
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
		std::wstring const& inVenue,
		std::wstring const& inOldDiv,
		std::wstring const& inNewDiv)
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
		std::wstring const& inVenue,
		std::wstring const& inDiv)
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
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inLevel) const
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
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inOldLevel,
		std::wstring const& inNewLevel)
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
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inLevel)
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
		std::wstring const& inVenue,
		std::wstring const& inEvent) const
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
		std::wstring const& inVenue,
		std::wstring const& inOldEvent,
		std::wstring const& inNewEvent)
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
		std::wstring const& inVenue,
		std::wstring const& inEvent)
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


int ARBDogExistingPointsList::NumOtherPointsInUse(std::wstring const& inOther) const
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
		std::wstring const& inOldOther,
		std::wstring const& inNewOther)
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


int ARBDogExistingPointsList::DeleteOtherPoints(std::wstring const& inOther)
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
		std::wstring const& inVenue,
		std::wstring const& inMultiQ) const
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
		std::wstring const& inVenue,
		std::wstring const& inOldMultiQ,
		std::wstring const& inNewMultiQ)
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
		std::wstring const& inVenue)
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
