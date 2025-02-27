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
 * 2016-04-29 Separate lifetime points from title (run) points.
 * 2016-01-06 Add support for named lifetime points.
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
#include "ARBCommon/StringUtil.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBCommon;
namespace ARB
{

namespace
{
constexpr wchar_t EXISTING_PTS_TYPE_OTHER[] = L"Other";
constexpr wchar_t EXISTING_PTS_TYPE_LIFETIME[] = L"Lifetime";
constexpr wchar_t EXISTING_PTS_TYPE_TITLE[] = L"Run";
constexpr wchar_t EXISTING_PTS_TYPE_SPEED[] = L"Speed";
constexpr wchar_t EXISTING_PTS_TYPE_MQ[] = L"MQ";
constexpr wchar_t EXISTING_PTS_TYPE_SQ[] = L"SQ";
} // namespace


/////////////////////////////////////////////////////////////////////////////
// static
wxString ARBDogExistingPoints::GetPointTypeName(ARBExistingPointType inType)
{
	wxString str;
	switch (inType)
	{
	case ARBExistingPointType::Unknown:
		break;
	case ARBExistingPointType::OtherPoints:
		str = Localization()->ExistingPointsOther();
		break;
	case ARBExistingPointType::Lifetime:
		str = Localization()->ExistingPointsLifetime();
		break;
	case ARBExistingPointType::Title:
		str = Localization()->ExistingPointsRun();
		break;
	case ARBExistingPointType::Speed:
		str = Localization()->ExistingPointsSpeed();
		break;
	case ARBExistingPointType::MQ:
		str = Localization()->ExistingPointsMQ();
		break;
	case ARBExistingPointType::SQ:
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
	ARBDogExistingPoints_concrete()
	{
	}
	ARBDogExistingPoints_concrete(ARBDogExistingPoints const& rhs)
		: ARBDogExistingPoints(rhs)
	{
	}
};
}; // namespace


ARBDogExistingPointsPtr ARBDogExistingPoints::New()
{
	return std::make_shared<ARBDogExistingPoints_concrete>();
}


ARBDogExistingPoints::ARBDogExistingPoints()
	: m_Date()
	, m_Comment()
	, m_Type(ARBExistingPointType::Title)
	, m_TypeName()
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
	, m_TypeName(rhs.m_TypeName)
	, m_Venue(rhs.m_Venue)
	, m_MultiQ(rhs.m_MultiQ)
	, m_Div(rhs.m_Div)
	, m_Level(rhs.m_Level)
	, m_Event(rhs.m_Event)
	, m_SubName(rhs.m_SubName)
	, m_Points(rhs.m_Points)
{
}


ARBDogExistingPoints::ARBDogExistingPoints(ARBDogExistingPoints&& rhs)
	: m_Date(std::move(rhs.m_Date))
	, m_Comment(std::move(rhs.m_Comment))
	, m_Type(std::move(rhs.m_Type))
	, m_TypeName(std::move(rhs.m_TypeName))
	, m_Venue(std::move(rhs.m_Venue))
	, m_MultiQ(std::move(rhs.m_MultiQ))
	, m_Div(std::move(rhs.m_Div))
	, m_Level(std::move(rhs.m_Level))
	, m_Event(std::move(rhs.m_Event))
	, m_SubName(std::move(rhs.m_SubName))
	, m_Points(std::move(rhs.m_Points))
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
		m_TypeName = rhs.m_TypeName;
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


ARBDogExistingPoints& ARBDogExistingPoints::operator=(ARBDogExistingPoints&& rhs)
{
	if (this != &rhs)
	{
		m_Date = std::move(rhs.m_Date);
		m_Comment = std::move(rhs.m_Comment);
		m_Type = std::move(rhs.m_Type);
		m_TypeName = std::move(rhs.m_TypeName);
		m_Venue = std::move(rhs.m_Venue);
		m_MultiQ = std::move(rhs.m_MultiQ);
		m_Div = std::move(rhs.m_Div);
		m_Level = std::move(rhs.m_Level);
		m_Event = std::move(rhs.m_Event);
		m_SubName = std::move(rhs.m_SubName);
		m_Points = std::move(rhs.m_Points);
	}
	return *this;
}


bool ARBDogExistingPoints::operator==(ARBDogExistingPoints const& rhs) const
{
	// clang-format off
	return m_Date == rhs.m_Date
		&& m_Comment == rhs.m_Comment
		&& m_Type == rhs.m_Type
		&& m_TypeName == rhs.m_TypeName
		&& m_Venue == rhs.m_Venue
		&& m_MultiQ == rhs.m_MultiQ
		&& m_Div == rhs.m_Div
		&& m_Level == rhs.m_Level
		&& m_Event == rhs.m_Event
		&& m_SubName == rhs.m_SubName
		&& m_Points == rhs.m_Points;
	// clang-format on
}


bool ARBDogExistingPoints::Load(
	ARBConfig const& inConfig,
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_EXISTING_PTS)
		return false;

	wxString attrib;

	if (ARBAttribLookup::Invalid == inTree->GetAttrib(ATTRIB_EXISTING_PTS_DATE, m_Date))
	{
		inTree->GetAttrib(ATTRIB_EXISTING_PTS_DATE, attrib);
		wxString msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(
			Localization()->ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_DATE, msg));
		return false;
	}

	if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_EXISTING_PTS_TYPE, attrib) || 0 == attrib.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_TYPE));
		return false;
	}
	bool bConvertedQQ = false;
	if (attrib == EXISTING_PTS_TYPE_OTHER)
		m_Type = ARBExistingPointType::OtherPoints;
	else if (attrib == EXISTING_PTS_TYPE_LIFETIME)
		m_Type = ARBExistingPointType::Lifetime;
	else if (attrib == EXISTING_PTS_TYPE_TITLE)
		m_Type = ARBExistingPointType::Title;
	else if (attrib == EXISTING_PTS_TYPE_SPEED)
		m_Type = ARBExistingPointType::Speed;
	else if (attrib == EXISTING_PTS_TYPE_MQ)
		m_Type = ARBExistingPointType::MQ;
	else if (attrib == EXISTING_PTS_TYPE_SQ)
		m_Type = ARBExistingPointType::SQ;
	// Changed attribute from 'Mach' to 'Speed' in 10.1
	else if (inVersion < ARBVersion(10, 1) && attrib == L"Mach")
		m_Type = ARBExistingPointType::Speed;
	// Changed QQ to MQ (multi Q)
	else if (inVersion < ARBVersion(11, 0) && attrib == L"QQ")
	{
		m_Type = ARBExistingPointType::MQ;
		bConvertedQQ = true;
	}
	else
	{
		wxString msg(Localization()->ValidValues());
		msg += EXISTING_PTS_TYPE_OTHER;
		msg += L", ";
		msg += EXISTING_PTS_TYPE_LIFETIME;
		msg += L", ";
		msg += EXISTING_PTS_TYPE_TITLE;
		msg += L", ";
		msg += EXISTING_PTS_TYPE_SPEED;
		msg += L", ";
		msg += EXISTING_PTS_TYPE_MQ;
		msg += L", ";
		msg += EXISTING_PTS_TYPE_SQ;
		ioCallback.LogMessage(
			Localization()->ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_TYPE, msg));
		return false;
	}

	if (ARBExistingPointType::OtherPoints == m_Type || ARBExistingPointType::Lifetime == m_Type)
	{
		if (ARBAttribLookup::Found == inTree->GetAttrib(ATTRIB_EXISTING_PTS_OTHER, m_TypeName)
			&& 0 < m_TypeName.length())
		{
			if (ARBExistingPointType::OtherPoints == m_Type)
			{
				if (!inConfig.GetOtherPoints().VerifyOtherPoints(m_TypeName))
				{
					wxString msg(Localization()->InvalidOtherPtsName());
					msg += m_TypeName;
					ioCallback.LogMessage(
						Localization()->ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_OTHER, msg));
					return false;
				}
			}
		}
		else
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_OTHER));
			return false;
		}
	}

	if (ARBAttribLookup::Found == inTree->GetAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue) && 0 < m_Venue.length())
	{
		if (!inConfig.GetVenues().VerifyVenue(m_Venue))
		{
			wxString msg(Localization()->InvalidVenueName());
			msg += m_Venue;
			ioCallback.LogMessage(
				Localization()->ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_VENUE, msg));
			return false;
		}
		if (ARBExistingPointType::Lifetime == m_Type)
		{
			ARBConfigVenuePtr pVenue;
			if (!inConfig.GetVenues().FindVenue(m_Venue, &pVenue)
				|| !pVenue->GetLifetimeNames().VerifyLifetimeName(m_TypeName))
			{
				wxString msg(Localization()->InvalidLifetimeName());
				msg += m_TypeName;
				ioCallback.LogMessage(
					Localization()->ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_OTHER, msg));
				return false;
			}
		}
	}
	else
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_VENUE));
		return false;
	}

	if (ARBExistingPointType::MQ == m_Type)
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
				ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(
					TREE_EXISTING_PTS,
					ATTRIB_EXISTING_PTS_MULTIQ,
					Localization()->InvalidMultiqConversion()));
				return false;
			}
		}
		else
		{
			if (ARBAttribLookup::Found == inTree->GetAttrib(ATTRIB_EXISTING_PTS_MULTIQ, m_MultiQ)
				&& 0 < m_MultiQ.length())
			{
				if (!inConfig.GetVenues().VerifyMultiQ(m_Venue, m_MultiQ))
				{
					wxString msg(Localization()->InvalidMultiqName());
					msg += m_Venue;
					msg += L"/";
					msg += m_MultiQ;
					ioCallback.LogMessage(
						Localization()->ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_MULTIQ, msg));
					return false;
				}
			}
			else
			{
				ioCallback.LogMessage(
					Localization()->ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_MULTIQ));
				return false;
			}
		}
	}

	else
	{
		if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div) || 0 == m_Div.length())
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_DIV));
			return false;
		}

		if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level) || 0 == m_Level.length())
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_LEVEL));
			return false;
		}
	}

	// I could add additional verification to make sure it's a valid SQ/Speed/MQ
	// event... but let the UI handle that...
	switch (m_Type)
	{
	case ARBExistingPointType::OtherPoints:
	case ARBExistingPointType::Lifetime:
	case ARBExistingPointType::Title:
	case ARBExistingPointType::SQ:
		if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_EXISTING_PTS_EVENT, m_Event) || 0 == m_Event.length())
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_EVENT));
			return false;
		}
		if (!inConfig.GetVenues().VerifyEvent(m_Venue, m_Div, m_Level, m_Event, m_Date))
		{
			wxString msg(Localization()->InvalidEventName());
			msg += m_Venue;
			msg += L"/";
			msg += m_Div;
			msg += L"/";
			msg += m_Level;
			msg += L"/";
			msg += m_Event;
			ioCallback.LogMessage(
				Localization()->ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_EVENT, msg));
			return false;
		}
		// Only used in eRuns.
		inTree->GetAttrib(ATTRIB_EXISTING_PTS_SUBNAME, m_SubName);
		break;
	case ARBExistingPointType::MQ:
		break;
	case ARBExistingPointType::Speed:
		if (!inConfig.GetVenues().VerifyLevel(m_Venue, m_Div, m_Level))
		{
			wxString msg(Localization()->InvalidVenueName());
			msg += m_Venue;
			msg += L"/";
			msg += m_Div;
			msg += L"/";
			msg += m_Level;
			ioCallback.LogMessage(
				Localization()->ErrorInvalidAttributeValue(TREE_EXISTING_PTS, ATTRIB_EXISTING_PTS_VENUE, msg));
			return false;
		}
		break;
	case ARBExistingPointType::Unknown:
		break;
	}

	inTree->GetAttrib(ATTRIB_EXISTING_PTS_POINTS, m_Points);

	m_Comment = inTree->GetValue();

	return true;
}


bool ARBDogExistingPoints::Save(ElementNodePtr const& ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr title = ioTree->AddElementNode(TREE_EXISTING_PTS);
	title->AddAttrib(ATTRIB_EXISTING_PTS_DATE, m_Date);
	switch (m_Type)
	{
	case ARBExistingPointType::Unknown:
		assert(0);
		break;
	case ARBExistingPointType::OtherPoints:
		title->AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_OTHER);
		title->AddAttrib(ATTRIB_EXISTING_PTS_OTHER, m_TypeName);
		title->AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title->AddAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div);
		title->AddAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level);
		title->AddAttrib(ATTRIB_EXISTING_PTS_EVENT, m_Event);
		break;
	case ARBExistingPointType::Lifetime:
		title->AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_LIFETIME);
		title->AddAttrib(ATTRIB_EXISTING_PTS_OTHER, m_TypeName);
		title->AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title->AddAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div);
		title->AddAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level);
		title->AddAttrib(ATTRIB_EXISTING_PTS_EVENT, m_Event);
		break;
	case ARBExistingPointType::Title:
		title->AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_TITLE);
		title->AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title->AddAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div);
		title->AddAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level);
		title->AddAttrib(ATTRIB_EXISTING_PTS_EVENT, m_Event);
		title->AddAttrib(ATTRIB_EXISTING_PTS_SUBNAME, m_SubName);
		break;
	case ARBExistingPointType::Speed:
		title->AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_SPEED);
		title->AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title->AddAttrib(ATTRIB_EXISTING_PTS_DIV, m_Div);
		title->AddAttrib(ATTRIB_EXISTING_PTS_LEVEL, m_Level);
		break;
	case ARBExistingPointType::MQ:
		title->AddAttrib(ATTRIB_EXISTING_PTS_TYPE, EXISTING_PTS_TYPE_MQ);
		title->AddAttrib(ATTRIB_EXISTING_PTS_VENUE, m_Venue);
		title->AddAttrib(ATTRIB_EXISTING_PTS_MULTIQ, m_MultiQ);
		break;
	case ARBExistingPointType::SQ:
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
	ElementNodePtr const& inTree,
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
	SortExistingPoints()
	{
	}
	bool operator()(ARBDogExistingPointsPtr const& one, ARBDogExistingPointsPtr const& two) const
	{
		if (one->GetTypeName() == two->GetTypeName())
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
							return StringUtil::CompareNoCase(one->GetDivision(), two->GetDivision()) < 0;
					}
					else
						return one->GetDate() < two->GetDate();
				}
				else
					return StringUtil::CompareNoCase(one->GetEvent(), two->GetEvent()) < 0;
			}
			else
				return StringUtil::CompareNoCase(one->GetVenue(), two->GetVenue()) < 0;
		}
		else
			return one->GetTypeName() < two->GetTypeName();
	}
};


void ARBDogExistingPointsList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortExistingPoints());
}


bool ARBDogExistingPointsList::HasPoints(wxString const& inVenue) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetType() != ARBExistingPointType::OtherPoints && (*iter)->GetVenue() == inVenue)
			return true;
	}
	return false;
}


bool ARBDogExistingPointsList::HasPoints(
	ARBConfigVenuePtr const& inVenue,
	ARBConfigDivisionPtr const& inDiv,
	ARBConfigLevelPtr const& inLevel,
	ARBConfigEventPtr const& inEvent,
	ARBDate inDateFrom,
	ARBDate inDateTo,
	bool inHasLifetime) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBExistingPointType type = (*iter)->GetType();
		if (ARBExistingPointType::OtherPoints != type && (*iter)->GetVenue() == inVenue->GetName()
			&& (*iter)->GetDivision() == inDiv->GetName()
			&& ((*iter)->GetLevel() == inLevel->GetName() || inLevel->GetSubLevels().FindSubLevel((*iter)->GetLevel())))
		{
			if ((inDateFrom.IsValid() && (*iter)->GetDate() < inDateFrom)
				|| (inDateTo.IsValid() && (*iter)->GetDate() > inDateTo))
				return false;
			if (!inHasLifetime && (ARBExistingPointType::Title == type || ARBExistingPointType::SQ == type))
			{
				if ((*iter)->GetEvent() == inEvent->GetName())
					return true;
			}
			else if (inHasLifetime && ARBExistingPointType::Lifetime == type)
			{
				if ((*iter)->GetEvent() == inEvent->GetName())
				{
					ARBConfigScoringPtr pScoring;
					if (inEvent->GetScorings()
							.FindEvent(inDiv->GetName(), inLevel->GetName(), (*iter)->GetDate(), &pScoring))
					{
						for (ARBConfigLifetimeNameList::iterator iterN = inVenue->GetLifetimeNames().begin();
							 iterN != inVenue->GetLifetimeNames().end();
							 ++iterN)
						{
							if (0 < pScoring->GetLifetimePoints().GetLifetimePoints((*iterN)->GetName(), 0.0, 0))
								return true;
						}
					}
				}
			}
			else
				return true;
		}
	}
	return false;
}


double ARBDogExistingPointsList::ExistingPoints(
	ARBExistingPointType inType,
	ARBConfigVenuePtr const& inVenue,
	ARBConfigMultiQPtr const& inMultiQ,
	ARBConfigDivisionPtr const& inDiv,
	ARBConfigLevelPtr const& inLevel,
	ARBConfigEventPtr const& inEvent,
	ARBDate inDateFrom,
	ARBDate inDateTo) const
{
	if (ARBExistingPointType::Lifetime == inType)
		return 0.0;
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


double ARBDogExistingPointsList::ExistingLifetimePoints(
	ARBConfigLifetimeNamePtr const& inName,
	ARBConfigVenuePtr const& inVenue,
	ARBConfigDivisionPtr const& inDiv,
	ARBConfigLevelPtr const& inLevel,
	ARBConfigEventPtr const& inEvent,
	ARBDate inDateFrom,
	ARBDate inDateTo) const
{
	double pts = 0.0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (ARBExistingPointType::Lifetime == (*iter)->GetType())
		{
			if (inName && (*iter)->GetTypeName() != inName->GetName())
				continue;
			if (inVenue && (*iter)->GetVenue() != inVenue->GetName())
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


int ARBDogExistingPointsList::NumExistingPointsInVenue(wxString const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue)
			++count;
	}
	return count;
}


int ARBDogExistingPointsList::RenameVenue(wxString const& inOldVenue, wxString const& inNewVenue)
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


int ARBDogExistingPointsList::DeleteVenue(wxString const& inVenue)
{
	wxString venue(inVenue);
	int count = 0;
	for (iterator iter = begin(); iter != end();)
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


int ARBDogExistingPointsList::NumExistingPointsInDivision(ARBConfigVenuePtr const& inVenue, wxString const& inDiv) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue->GetName() && (*iter)->GetDivision() == inDiv)
			++count;
	}
	return count;
}


int ARBDogExistingPointsList::RenameDivision(
	wxString const& inVenue,
	wxString const& inOldDiv,
	wxString const& inNewDiv)
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


int ARBDogExistingPointsList::DeleteDivision(wxString const& inVenue, wxString const& inDiv)
{
	int count = 0;
	for (iterator iter = begin(); iter != end();)
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


int ARBDogExistingPointsList::NumLevelsInUse(wxString const& inVenue, wxString const& inDiv, wxString const& inLevel)
	const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetDivision() == inDiv && (*iter)->GetLevel() == inLevel)
			++count;
	}
	return count;
}


int ARBDogExistingPointsList::RenameLevel(
	wxString const& inVenue,
	wxString const& inDiv,
	wxString const& inOldLevel,
	wxString const& inNewLevel)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetDivision() == inDiv && (*iter)->GetLevel() == inOldLevel)
		{
			++count;
			(*iter)->SetLevel(inNewLevel);
		}
	}
	return count;
}


int ARBDogExistingPointsList::DeleteLevel(wxString const& inVenue, wxString const& inDiv, wxString const& inLevel)
{
	int count = 0;
	for (iterator iter = begin(); iter != end();)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetDivision() == inDiv && (*iter)->GetLevel() == inLevel)
		{
			++count;
			iter = erase(iter);
		}
		else
			++iter;
	}
	return count;
}


int ARBDogExistingPointsList::NumEventsInUse(wxString const& inVenue, wxString const& inEvent) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetEvent() == inEvent)
			++count;
	}
	return count;
}


int ARBDogExistingPointsList::RenameEvent(
	wxString const& inVenue,
	wxString const& inOldEvent,
	wxString const& inNewEvent)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetEvent() == inOldEvent)
		{
			++count;
			(*iter)->SetEvent(inNewEvent);
		}
	}
	return count;
}


int ARBDogExistingPointsList::DeleteEvent(wxString const& inVenue, wxString const& inEvent)
{
	int count = 0;
	for (iterator iter = begin(); iter != end();)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetEvent() == inEvent)
		{
			++count;
			iter = erase(iter);
		}
		else
			++iter;
	}
	return count;
}


int ARBDogExistingPointsList::NumOtherPointsInUse(wxString const& inOther) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (ARBExistingPointType::OtherPoints == (*iter)->GetType() && (*iter)->GetTypeName() == inOther)
			++count;
	}
	return count;
}


int ARBDogExistingPointsList::RenameOtherPoints(wxString const& inOldOther, wxString const& inNewOther)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if (ARBExistingPointType::OtherPoints == (*iter)->GetType() && (*iter)->GetTypeName() == inOldOther)
		{
			(*iter)->SetTypeName(inNewOther);
			++count;
		}
	}
	return count;
}


int ARBDogExistingPointsList::DeleteOtherPoints(wxString const& inOther)
{
	int count = 0;
	for (iterator iter = begin(); iter != end();)
	{
		if (ARBExistingPointType::OtherPoints == (*iter)->GetType() && (*iter)->GetTypeName() == inOther)
		{
			iter = erase(iter);
			++count;
		}
		else
			++iter;
	}
	return count;
}


int ARBDogExistingPointsList::NumLifetimePointsInUse(wxString const& inLifetime) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (ARBExistingPointType::Lifetime == (*iter)->GetType() && (*iter)->GetTypeName() == inLifetime)
			++count;
	}
	return count;
}


int ARBDogExistingPointsList::RenameLifetimeName(
	wxString const& inVenue,
	wxString const& inOldName,
	wxString const& inNewName)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if (ARBExistingPointType::Lifetime == (*iter)->GetType() && (*iter)->GetVenue() == inVenue
			&& (*iter)->GetTypeName() == inOldName)
		{
			(*iter)->SetTypeName(inNewName);
			++count;
		}
	}
	return count;
}


int ARBDogExistingPointsList::DeleteLifetimeName(wxString const& inVenue, wxString const& inName)
{
	int count = 0;
	for (iterator iter = begin(); iter != end();)
	{
		if (ARBExistingPointType::Lifetime == (*iter)->GetType() && (*iter)->GetVenue() == inVenue
			&& (*iter)->GetTypeName() == inName)
		{
			iter = erase(iter);
			++count;
		}
		else
			++iter;
	}
	return count;
}


int ARBDogExistingPointsList::NumMultiQsInUse(wxString const& inVenue, wxString const& inMultiQ) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (ARBExistingPointType::MQ == (*iter)->GetType() && (*iter)->GetVenue() == inVenue
			&& (*iter)->GetMultiQ() == inMultiQ)
			++count;
	}
	return count;
}


int ARBDogExistingPointsList::RenameMultiQs(
	wxString const& inVenue,
	wxString const& inOldMultiQ,
	wxString const& inNewMultiQ)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if (ARBExistingPointType::MQ == (*iter)->GetType() && (*iter)->GetVenue() == inVenue
			&& (*iter)->GetMultiQ() == inOldMultiQ)
		{
			(*iter)->SetMultiQ(inNewMultiQ);
			++count;
		}
	}
	return count;
}


int ARBDogExistingPointsList::DeleteMultiQs(ARBConfig const& inConfig, wxString const& inVenue)
{
	int count = 0;
	ARBConfigVenuePtr pVenue;
	if (inConfig.GetVenues().FindVenue(inVenue, &pVenue))
	{
		for (iterator iter = begin(); iter != end();)
		{
			if (ARBExistingPointType::MQ == (*iter)->GetType()
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


bool ARBDogExistingPointsList::AddExistingPoints(ARBDogExistingPointsPtr const& inExistingPoints)
{
	bool bAdded = false;
	if (inExistingPoints)
	{
		bAdded = true;
		push_back(inExistingPoints);
	}
	return bAdded;
}


bool ARBDogExistingPointsList::DeleteExistingPoints(ARBDogExistingPointsPtr const& inExistingPoints)
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

} // namespace ARB
} // namespace dconSoft
