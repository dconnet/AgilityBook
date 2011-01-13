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
#include "ARBDogTrial.h"

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "ARBDate.h"
#include "ARBLocalization.h"
#include "Element.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

ARBDogClubPtr ARBDogClub::New()
{
	return ARBDogClubPtr(new ARBDogClub());
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
	return ARBDogClubPtr(new ARBDogClub(*this));
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


size_t ARBDogClub::GetSearchStrings(std::set<wxString>& ioStrings) const
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
		if (ElementNode::eFound != inTree->GetAttrib(wxT("Name"), m_Name)
		|| 0 == m_Name.length())
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_CLUB, wxT("Name")));
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
		wxString msg(Localization()->InvalidVenueName());
		msg += m_Venue;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_CLUB, ATTRIB_CLUB_VENUE, msg));
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


wxString ARBDogClubList::GetPrimaryClubName() const
{
	if (0 < size())
	{
		const_iterator iter = begin();
		return (*begin())->GetName();
	}
	else
		return wxT("");
}


wxString ARBDogClubList::GetPrimaryClubVenue() const
{
	if (0 < size())
	{
		const_iterator iter = begin();
		return (*begin())->GetVenue();
	}
	else
		return wxT("");
}


bool ARBDogClubList::FindEvent(
		ARBConfig const& inConfig,
		wxString const& inEvent,
		wxString const& inDivision,
		wxString const& inLevel,
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
		wxString msg(Localization()->InvalidEvent());
		msg += inEvent;
		msg += wxT(" (");
		msg += inDivision;
		msg += wxT("/");
		msg += inLevel;
		msg += wxT(")");
		for (const_iterator iter = begin(); !pScoring && iter != end(); ++iter)
		{
			msg += wxT("\n");
			msg += (*iter)->GetName();
			msg += wxT(" [");
			msg += (*iter)->GetVenue();
			msg += wxT("]");
		}
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_RUN, ATTRIB_RUN_EVENT, msg));
	}
	return bFound;
}


bool ARBDogClubList::FindVenue(
		wxString const& inVenue,
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
		wxString const& inName,
		wxString const& inVenue,
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
		wxString const& inName,
		wxString const& inVenue)
{
	wxString name(inName);
	wxString venue(inVenue);
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