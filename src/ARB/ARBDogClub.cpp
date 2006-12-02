/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "StdAfx.h"
#include "ARBDogTrial.h"

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "ARBDate.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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

size_t ARBDogClub::GetSearchStrings(std::set<ARBString>& ioStrings) const
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
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (inVersion == ARBVersion(1,0))
	{
		if (Element::eFound != inTree.GetAttrib(_T("Name"), m_Name)
		|| 0 == m_Name.length())
		{
			ioCallback.LogMessage(ErrorMissingAttribute(TREE_CLUB, _T("Name")));
			return false;
		}
	}
	else
		m_Name = inTree.GetValue();

	if (Element::eFound != inTree.GetAttrib(ATTRIB_CLUB_VENUE, m_Venue)
	|| 0 == m_Venue.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_CLUB, ATTRIB_CLUB_VENUE));
		return false;
	}

	if (!inConfig.GetVenues().VerifyVenue(m_Venue))
	{
		ARBString msg(INVALID_VENUE_NAME);
		msg += m_Venue;
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_CLUB, ATTRIB_CLUB_VENUE, msg.c_str()));
		return false;
	}

	return true;
}

bool ARBDogClub::Save(Element& ioTree) const
{
	Element& club = ioTree.AddElement(TREE_CLUB);
	club.AddAttrib(ATTRIB_CLUB_VENUE, m_Venue);
	club.SetValue(m_Name);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBDogClubList::Load(
		ARBConfig const& inConfig,
		Element const& inTree,
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

ARBString ARBDogClubList::GetPrimaryClubName() const
{
	if (0 < size())
	{
		const_iterator iter = begin();
		return (*begin())->GetName();
	}
	else
		return _T("");
}

ARBString ARBDogClubList::GetPrimaryClubVenue() const
{
	if (0 < size())
	{
		const_iterator iter = begin();
		return (*begin())->GetVenue();
	}
	else
		return _T("");
}

bool ARBDogClubList::FindEvent(
		ARBConfig const& inConfig,
		ARBString const& inEvent,
		ARBString const& inDivision,
		ARBString const& inLevel,
		ARBDate const& inDate,
		ARBErrorCallback& ioCallback,
		ARBConfigScoringPtr* outScoring) const
{
	if (outScoring)
		outScoring->reset();
	ARBConfigScoringPtr pEvent;
	for (const_iterator iter = begin(); !pEvent && iter != end(); ++iter)
	{
		inConfig.GetVenues().FindEvent((*iter)->GetVenue(), inEvent, inDivision, inLevel, inDate, &pEvent);
	}
	bool bFound = false;
	if (pEvent)
	{
		bFound = true;
		if (outScoring)
			*outScoring = pEvent;
	}
	else
	{
		ARBString msg(INVALID_EVENT);
		msg += inEvent;
		msg += _T(" (");
		msg += inDivision;
		msg += _T("/");
		msg += inLevel;
		msg += _T(")");
		for (const_iterator iter = begin(); !pEvent && iter != end(); ++iter)
		{
			msg += _T("\n");
			msg += (*iter)->GetName();
			msg += _T(" [");
			msg += (*iter)->GetVenue();
			msg += _T("]");
		}
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_RUN, ATTRIB_RUN_EVENT, msg.c_str()));
	}
	return bFound;
}

bool ARBDogClubList::FindVenue(
		ARBString const& inVenue,
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
		ARBString const& inName,
		ARBString const& inVenue,
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
		ARBString const& inName,
		ARBString const& inVenue)
{
	ARBString name(inName);
	ARBString venue(inVenue);
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
