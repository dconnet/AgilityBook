/*
 * Copyright � 2002-2004 David Connet. All Rights Reserved.
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
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-01-26 DRC Recover from a bug where the title name was the nice name.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "StdAfx.h"
#include "ARBDogTitle.h"
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

ARBDogTitle::ARBDogTitle()
	: m_Date()
	, m_Venue()
	, m_Name()
	, m_bReceived(false)
	, m_bHidden(false)
{
}

ARBDogTitle::ARBDogTitle(ARBDogTitle const& rhs)
	: m_Date(rhs.m_Date)
	, m_Venue(rhs.m_Venue)
	, m_Name(rhs.m_Name)
	, m_bReceived(rhs.m_bReceived)
	, m_bHidden(rhs.m_bHidden)
{
}

ARBDogTitle::~ARBDogTitle()
{
}

ARBDogTitle& ARBDogTitle::operator=(ARBDogTitle const& rhs)
{
	if (this != &rhs)
	{
		m_Date = rhs.m_Date;
		m_Venue = rhs.m_Venue;
		m_Name = rhs.m_Name;
		m_bReceived = rhs.m_bReceived;
		m_bHidden = rhs.m_bHidden;
	}
	return *this;
}

bool ARBDogTitle::operator==(ARBDogTitle const& rhs) const
{
	return m_Date == rhs.m_Date
		&& m_Venue == rhs.m_Venue
		&& m_Name == rhs.m_Name
		&& m_bReceived == rhs.m_bReceived
		&& m_bHidden == rhs.m_bHidden;
}

bool ARBDogTitle::operator!=(ARBDogTitle const& rhs) const
{
	return !operator==(rhs);
}

size_t ARBDogTitle::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;

	ioStrings.insert(m_Date.GetString(ARBDate::eDashYMD));
	++nItems;

	ioStrings.insert(m_Venue);
	++nItems;

	ioStrings.insert(m_Name);
	++nItems;

	return nItems;
}

bool ARBDogTitle::Load(
	ARBConfig const& inConfig,
	Element const& inTree,
	ARBVersion const& inVersion,
	std::string& ioErrMsg)
{
	if (Element::eFound != inTree.GetAttrib(ATTRIB_TITLE_VENUE, m_Venue)
	|| 0 == m_Venue.length())
	{
		ioErrMsg += ErrorMissingAttribute(TREE_TITLE, ATTRIB_TITLE_VENUE);
		return false;
	}
	ARBConfigVenue const* pVenue = inConfig.GetVenues().FindVenue(m_Venue);
	if (!pVenue)
	{
		std::string msg("Unknown venue name: '");
		msg += m_Venue;
		msg += "'";
		ioErrMsg += ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_VENUE, msg.c_str());
		return false;
	}

	if (Element::eFound != inTree.GetAttrib(ATTRIB_TITLE_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioErrMsg += ErrorMissingAttribute(TREE_TITLE, ATTRIB_TITLE_NAME);
		return false;
	}

	// Must get Hidden before getting date.
	inTree.GetAttrib(ATTRIB_TITLE_HIDDEN, m_bHidden);

	switch (inTree.GetAttrib(ATTRIB_TITLE_DATE, m_Date))
	{
	case Element::eNotFound:
		// As of version 8.5, no date infers this is an unearned title
		// that we're hiding.
		if (inVersion < ARBVersion(8, 5))
		{
			ioErrMsg += ErrorMissingAttribute(TREE_TITLE, ATTRIB_TITLE_DATE);
			return false;
		}
		m_bHidden = true;
		break;
	case Element::eInvalidValue:
		{
			std::string attrib;
			inTree.GetAttrib(ATTRIB_TITLE_DATE, attrib);
			std::string msg(INVALID_DATE);
			msg += attrib;
			ioErrMsg += ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_DATE, msg.c_str());
			return false;
		}
	}

	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_TITLE_RECEIVED, m_bReceived))
	{
		ioErrMsg += ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_RECEIVED, VALID_VALUES_BOOL);
		return false;
	}

	if (NULL == inConfig.GetVenues().FindTitle(m_Venue, m_Name))
	{
		// This fixes a bug in v1.0.0.8 where the 'nice' name was being written
		// as the title name.
		ARBConfigTitle const* pTitle = inConfig.GetVenues().FindTitleCompleteName(m_Venue, m_Name, true);
		if (!pTitle)
			pTitle = inConfig.GetVenues().FindTitleCompleteName(m_Venue, m_Name, false);
		if (pTitle)
			m_Name = pTitle->GetName();
		else
		{
			std::string msg(INVALID_TITLE);
			msg += m_Venue;
			msg += "/";
			msg += m_Name;
			ioErrMsg += ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_NAME, msg.c_str());
			return false;
		}
	}

	return true;
}

bool ARBDogTitle::Save(Element& ioTree) const
{
	Element& title = ioTree.AddElement(TREE_TITLE);
	if (m_Date.IsValid())
	{
		title.AddAttrib(ATTRIB_TITLE_DATE, m_Date);
		if (m_bHidden) // Default is no
			title.AddAttrib(ATTRIB_TITLE_HIDDEN, m_bHidden);
	}
	else
	{
		bool bTemp = true;
		title.AddAttrib(ATTRIB_TITLE_HIDDEN, bTemp);
	}
	title.AddAttrib(ATTRIB_TITLE_VENUE, m_Venue);
	title.AddAttrib(ATTRIB_TITLE_NAME, m_Name);
	if (m_bReceived) // Default is no.
		title.AddAttrib(ATTRIB_TITLE_RECEIVED, m_bReceived);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class SortTitle
{
public:
	SortTitle(bool bDescending) : m_bDescending(bDescending) {}
	bool operator()(ARBDogTitle* one, ARBDogTitle* two) const
	{
		if (one->GetDate() < two->GetDate())
			return m_bDescending;
		else
			return !m_bDescending;
	}
private:
	bool m_bDescending;
};

void ARBDogTitleList::sort(bool inDescending)
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortTitle(inDescending));
}

int ARBDogTitleList::NumTitlesInVenue(std::string const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue)
			++count;
	}
	return count;
}

int ARBDogTitleList::RenameVenue(
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

int ARBDogTitleList::DeleteVenue(std::string const& inVenue)
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

int ARBDogTitleList::NumTitlesInDivision(
	ARBConfigVenue const* inVenue,
	std::string const& inDiv) const
{
	ASSERT(inVenue);
	int count = 0;
	ARBConfigDivision const* pDiv = inVenue->GetDivisions().FindDivision(inDiv);
	if (pDiv)
	{
		for (const_iterator iter = begin(); iter != end(); ++iter)
		{
			if ((*iter)->GetVenue() == inVenue->GetName())
			{
				for (ARBConfigTitleList::const_iterator iterTitles = pDiv->GetTitles().begin(); iterTitles != pDiv->GetTitles().end(); ++iterTitles)
				{
					if ((*iter)->GetName() == (*iterTitles)->GetName())
						++count;
				}
			}
		}
	}
	return count;
}

int ARBDogTitleList::RenameDivision(
	ARBConfigVenue const* inVenue,
	std::string const& inOldDiv,
	std::string const& inNewDiv)
{
	// Nothing to do here. Since the division name simply changed and we don't
	// track that here, there's nothing to update.
	return 0;
}

int ARBDogTitleList::DeleteDivision(
	ARBConfig const& inConfig,
	std::string const& inVenue,
	std::string const& inDiv)
{
	std::string venue(inVenue);
	std::string div(inDiv);
	ARBConfigVenue const* pVenue = inConfig.GetVenues().FindVenue(venue);
	ASSERT(pVenue);
	int count = 0;
	ARBConfigDivision const* pDiv = pVenue->GetDivisions().FindDivision(div);
	if (pDiv)
	{
		for (iterator iter = begin(); iter != end(); )
		{
			bool bFound = false;
			if ((*iter)->GetVenue() == pVenue->GetName())
			{
				for (ARBConfigTitleList::const_iterator iterTitles = pDiv->GetTitles().begin();
					!bFound && iterTitles != pDiv->GetTitles().end();
					++iterTitles)
				{
					if ((*iter)->GetName() == (*iterTitles)->GetName())
					{
						++count;
						iter = erase(iter);
						bFound = true;
					}
				}
			}
			if (!bFound)
				++iter;
		}
	}
	return count;
}

int ARBDogTitleList::NumTitlesInUse(
	std::string const& inVenue,
	std::string const& inTitle) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetName() == inTitle)
			++count;
	}
	return count;
}

int ARBDogTitleList::RenameTitle(
	std::string const& inVenue,
	std::string const& inOldTitle,
	std::string const& inNewTitle)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetName() == inOldTitle)
		{
			++count;
			(*iter)->SetName(inNewTitle);
		}
	}
	return count;
}

int ARBDogTitleList::DeleteTitle(
	std::string const& inVenue,
	std::string const& inTitle)
{
	std::string venue(inVenue);
	std::string title(inTitle);
	int count = 0;
	for (iterator iter = begin(); iter != end(); )
	{
		if ((*iter)->GetVenue() == venue && (*iter)->GetName() == title)
		{
			++count;
			erase(iter);
		}
		else
			++iter;
	}
	return count;
}

ARBDogTitle* ARBDogTitleList::AddTitle(ARBDogTitle* inTitle)
{
	if (inTitle)
	{
		inTitle->AddRef();
		push_back(inTitle);
	}
	return inTitle;
}
