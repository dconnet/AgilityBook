/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "StdAfx.h"
#include "ARBDogTrial.h"

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBDogClub::ARBDogClub()
	: m_Name()
	, m_Venue()
{
}

ARBDogClub::ARBDogClub(const ARBDogClub& rhs)
	: m_Name(rhs.m_Name)
	, m_Venue(rhs.m_Venue)
{
}

ARBDogClub::~ARBDogClub()
{
}

ARBDogClub& ARBDogClub::operator=(const ARBDogClub& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Venue = rhs.m_Venue;
	}
	return *this;
}

bool ARBDogClub::operator==(const ARBDogClub& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Venue == rhs.m_Venue;
}

bool ARBDogClub::operator!=(const ARBDogClub& rhs) const
{
	return !operator==(rhs);
}

size_t ARBDogClub::GetSearchStrings(std::set<std::string>& ioStrings) const
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
	const ARBConfig& inConfig,
	const Element& inTree,
	const ARBVersion& inVersion,
	std::string& ioErrMsg)
{
	if (inVersion == ARBVersion(1,0))
	{
		if (Element::eFound != inTree.GetAttrib("Name", m_Name)
		|| 0 == m_Name.length())
		{
			ioErrMsg += ErrorMissingAttribute(TREE_CLUB, "Name");
			return false;
		}
	}
	else
		m_Name = inTree.GetValue();

	if (Element::eFound != inTree.GetAttrib(ATTRIB_CLUB_VENUE, m_Venue)
	|| 0 == m_Venue.length())
	{
		ioErrMsg += ErrorMissingAttribute(TREE_CLUB, ATTRIB_CLUB_VENUE);
		return false;
	}

	if (!inConfig.GetVenues().VerifyVenue(m_Venue))
	{
		std::string msg(INVALID_VENUE_NAME);
		msg += m_Venue;
		ioErrMsg += ErrorInvalidAttributeValue(TREE_CLUB, ATTRIB_CLUB_VENUE, msg.c_str());
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

const ARBDogClub* ARBDogClubList::GetPrimaryClub() const
{
	if (0 < size())
	{
		const_iterator iter = begin();
		return (*iter);
	}
	else
		return NULL;
}

const ARBConfigScoring* ARBDogClubList::FindEvent(
	const ARBConfig* inConfig,
	const std::string& inEvent,
	const std::string& inDivision,
	const std::string& inLevel,
	const ARBDate& inDate,
	std::string& ioErrMsg) const
{
	const ARBConfigScoring* pEvent = NULL;
	for (const_iterator iter = begin(); NULL == pEvent && iter != end(); ++iter)
	{
		pEvent = inConfig->GetVenues().FindEvent((*iter)->GetVenue(), inEvent, inDivision, inLevel, inDate);
	}
	if (!pEvent)
	{
		std::string msg(INVALID_EVENT);
		msg += inEvent;
		msg += " (";
		msg += inDivision;
		msg += "/";
		msg += inLevel;
		msg += ")";
		for (const_iterator iter = begin(); NULL == pEvent && iter != end(); ++iter)
		{
			msg += "\n";
			msg += (*iter)->GetName();
			msg += " [";
			msg += (*iter)->GetVenue();
			msg += "]";
		}
		ioErrMsg += ErrorInvalidAttributeValue(TREE_RUN, ATTRIB_RUN_EVENT, msg.c_str());
	}
	return pEvent;
}

const ARBDogClub* ARBDogClubList::FindVenue(const std::string& inVenue) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue)
			return (*iter);
	}
	return NULL;
}

ARBDogClub* ARBDogClubList::AddClub(const std::string& inName, const std::string& inVenue)
{
	ARBDogClub* pClub = new ARBDogClub();
	pClub->SetName(inName);
	pClub->SetVenue(inVenue);
	push_back(pClub);
	return pClub;
}

bool ARBDogClubList::DeleteClub(const std::string& inName, const std::string& inVenue)
{
	std::string name(inName);
	std::string venue(inVenue);
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
