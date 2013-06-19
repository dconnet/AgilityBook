/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "stdafx.h"
#include "ARB/ARBDogTrial.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/ARBDate.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class ARBDogClub_concrete : public ARBDogClub
	{
	public:
		ARBDogClub_concrete() {}
		ARBDogClub_concrete(ARBDogClub const& rhs)
			: ARBDogClub(rhs)
		{
		}
	};
};


ARBDogClubPtr ARBDogClub::New()
{
	return std::make_shared<ARBDogClub_concrete>();
}


ARBDogClub::ARBDogClub()
	: m_Name()
	, m_Venue()
{
}


ARBDogClub::ARBDogClub(ARBDogClub const& rhs)
	: m_Name(rhs.m_Name)
	, m_Venue(rhs.m_Venue)
{
}


ARBDogClub::~ARBDogClub()
{
}


ARBDogClubPtr ARBDogClub::Clone() const
{
	return std::make_shared<ARBDogClub_concrete>(*this);
}


ARBDogClub& ARBDogClub::operator=(ARBDogClub const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Venue = rhs.m_Venue;
	}
	return *this;
}


bool ARBDogClub::operator==(ARBDogClub const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Venue == rhs.m_Venue;
}


size_t ARBDogClub::GetSearchStrings(std::set<std::wstring>& ioStrings) const
{
	size_t nItems = 0;

	if (0 < m_Name.length())
	{
		ioStrings.insert(m_Name);
		++nItems;
	}

	if (0 < m_Venue.length())
	{
		ioStrings.insert(m_Venue);
		++nItems;
	}

	return nItems;
}


bool ARBDogClub::Load(
		ARBConfig const& inConfig,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_CLUB)
		return false;
	if (inVersion == ARBVersion(1,0))
	{
		if (ElementNode::eFound != inTree->GetAttrib(L"Name", m_Name)
		|| 0 == m_Name.length())
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_CLUB, L"Name"));
			return false;
		}
	}
	else
		m_Name = inTree->GetValue();

	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_CLUB_VENUE, m_Venue)
	|| 0 == m_Venue.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_CLUB, ATTRIB_CLUB_VENUE));
		return false;
	}

	if (!inConfig.GetVenues().VerifyVenue(m_Venue))
	{
		std::wstring msg(Localization()->InvalidVenueName());
		msg += m_Venue;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_CLUB, ATTRIB_CLUB_VENUE, msg.c_str()));
		return false;
	}

	return true;
}


bool ARBDogClub::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr club = ioTree->AddElementNode(TREE_CLUB);
	club->AddAttrib(ATTRIB_CLUB_VENUE, m_Venue);
	if (0 < m_Name.length())
		club->SetValue(m_Name);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBDogClubList::Load(
		ARBConfig const& inConfig,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBDogClubPtr thing(ARBDogClub::New());
	if (!thing->Load(inConfig, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


bool ARBDogClubList::GetPrimaryClub(ARBDogClubPtr* outClub) const
{
	if (outClub)
		outClub->reset();
	if (0 < size())
	{
		const_iterator iter = begin();
		if (outClub)
			*outClub = *iter;
		return true;
	}
	else
		return false;
}


std::wstring ARBDogClubList::GetPrimaryClubName() const
{
	if (0 < size())
	{
		const_iterator iter = begin();
		return (*begin())->GetName();
	}
	else
		return std::wstring();
}


std::wstring ARBDogClubList::GetPrimaryClubVenue() const
{
	if (0 < size())
	{
		const_iterator iter = begin();
		return (*begin())->GetVenue();
	}
	else
		return std::wstring();
}


bool ARBDogClubList::FindEvent(
		ARBConfig const& inConfig,
		std::wstring const& inEvent,
		std::wstring const& inDivision,
		std::wstring const& inLevel,
		ARBDate const& inDate,
		ARBErrorCallback& ioCallback,
		ARBConfigEventPtr* outEvent,
		ARBConfigScoringPtr* outScoring) const
{
	if (outEvent)
		outEvent->reset();
	if (outScoring)
		outScoring->reset();
	ARBConfigEventPtr pEvent;
	ARBConfigScoringPtr pScoring;
	for (const_iterator iter = begin(); !pScoring && iter != end(); ++iter)
	{
		inConfig.GetVenues().FindEvent((*iter)->GetVenue(), inEvent, inDivision, inLevel, inDate, &pEvent, &pScoring);
	}
	bool bFound = false;
	if (pScoring)
	{
		bFound = true;
		if (outEvent)
			*outEvent = pEvent;
		if (outScoring)
			*outScoring = pScoring;
	}
	else
	{
		std::wstring msg(Localization()->InvalidEvent());
		msg += inEvent;
		msg += L" (";
		msg += inDivision;
		msg += L"/";
		msg += inLevel;
		msg += L")";
		for (const_iterator iter = begin(); !pScoring && iter != end(); ++iter)
		{
			msg += L"\n";
			msg += (*iter)->GetName();
			msg += L" [";
			msg += (*iter)->GetVenue();
			msg += L"]";
		}
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_RUN, ATTRIB_RUN_EVENT, msg.c_str()));
	}
	return bFound;
}


bool ARBDogClubList::FindVenue(
		std::wstring const& inVenue,
		ARBDogClubPtr* outClub) const
{
	if (outClub)
		outClub->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue)
		{
			if (outClub)
				*outClub = *iter;
			return true;
		}
	}
	return false;
}


bool ARBDogClubList::AddClub(
		std::wstring const& inName,
		std::wstring const& inVenue,
		ARBDogClubPtr* outClub)
{
	ARBDogClubPtr pClub(ARBDogClub::New());
	pClub->SetName(inName);
	pClub->SetVenue(inVenue);
	push_back(pClub);
	if (outClub)
		*outClub = pClub;
	return true;
}


bool ARBDogClubList::DeleteClub(
		std::wstring const& inName,
		std::wstring const& inVenue)
{
	std::wstring name(inName);
	std::wstring venue(inVenue);
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == name && (*iter)->GetVenue() == venue)
		{
			erase(iter);
			return true;
		}
	}
	return false;
}
