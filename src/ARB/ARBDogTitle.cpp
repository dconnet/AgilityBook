/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
{
}

ARBDogTitle::ARBDogTitle(const ARBDogTitle& rhs)
	: m_Date(rhs.m_Date)
	, m_Venue(rhs.m_Venue)
	, m_Name(rhs.m_Name)
	, m_bReceived(rhs.m_bReceived)
{
}

ARBDogTitle::~ARBDogTitle()
{
}

ARBDogTitle& ARBDogTitle::operator=(const ARBDogTitle& rhs)
{
	if (this != &rhs)
	{
		m_Date = rhs.m_Date;
		m_Venue = rhs.m_Venue;
		m_Name = rhs.m_Name;
		m_bReceived = rhs.m_bReceived;
	}
	return *this;
}

bool ARBDogTitle::operator==(const ARBDogTitle& rhs) const
{
	return m_Date == rhs.m_Date
		&& m_Venue == rhs.m_Venue
		&& m_Name == rhs.m_Name
		&& m_bReceived == rhs.m_bReceived;
}

bool ARBDogTitle::operator!=(const ARBDogTitle& rhs) const
{
	return !operator==(rhs);
}

bool ARBDogTitle::Load(
	const ARBConfig& inConfig,
	const CElement& inTree,
	const ARBVersion& inVersion)
{
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_TITLE_VENUE, m_Venue)
	|| 0 == m_Venue.length())
	{
		ErrorMissingAttribute(TREE_TITLE, ATTRIB_TITLE_VENUE);
		return false;
	}

	if (CElement::eFound != inTree.GetAttrib(ATTRIB_TITLE_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ErrorMissingAttribute(TREE_TITLE, ATTRIB_TITLE_NAME);
		return false;
	}

	switch (inTree.GetAttrib(ATTRIB_TITLE_DATE, m_Date))
	{
	case CElement::eNotFound:
		ErrorMissingAttribute(TREE_TITLE, ATTRIB_TITLE_DATE);
		return false;
	case CElement::eInvalidValue:
		{
			std::string attrib;
			inTree.GetAttrib(ATTRIB_TITLE_DATE, attrib);
			std::string msg(INVALID_DATE);
			msg += attrib;
			ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_DATE, msg.c_str());
			return false;
		}
	}

	if (CElement::eInvalidValue == inTree.GetAttrib(ATTRIB_TITLE_RECEIVED, m_bReceived))
	{
		ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_RECEIVED, VALID_VALUES_BOOL);
		return false;
	}

	if (!inConfig.GetVenues().VerifyTitle(m_Venue, m_Name))
	{
		std::string msg(INVALID_TITLE);
		msg += m_Venue;
		msg += "/";
		msg += m_Name;
		ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_NAME, msg.c_str());
		return false;
	}

	return true;
}

bool ARBDogTitle::Save(CElement& ioTree) const
{
	CElement& title = ioTree.AddElement(TREE_TITLE);
	title.AddAttrib(ATTRIB_TITLE_DATE, m_Date);
	title.AddAttrib(ATTRIB_TITLE_VENUE, m_Venue);
	title.AddAttrib(ATTRIB_TITLE_NAME, m_Name);
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

int ARBDogTitleList::NumTitlesInVenue(const std::string& inVenue) const
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

int ARBDogTitleList::DeleteVenue(const std::string& inVenue)
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
	const ARBConfigVenue* inVenue,
	const std::string& inDiv) const
{
	ASSERT(inVenue);
	int count = 0;
	const ARBConfigDivision* pDiv = inVenue->GetDivisions().FindDivision(inDiv);
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
	const ARBConfigVenue* inVenue,
	const std::string& inOldDiv,
	const std::string& inNewDiv)
{
	// Nothing to do here. Since the division name simply changed and we don't
	// track that here, there's nothing to update.
	return 0;
}

int ARBDogTitleList::DeleteDivision(
	const ARBConfig& inConfig,
	const std::string& inVenue,
	const std::string& inDiv)
{
	std::string venue(inVenue);
	std::string div(inDiv);
	const ARBConfigVenue* pVenue = inConfig.GetVenues().FindVenue(venue);
	ASSERT(pVenue);
	int count = 0;
	const ARBConfigDivision* pDiv = pVenue->GetDivisions().FindDivision(div);
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
	const std::string& inVenue,
	const std::string& inTitle) const
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
	const std::string& inVenue,
	const std::string& inOldTitle,
	const std::string& inNewTitle)
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
	const std::string& inVenue,
	const std::string& inTitle)
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
