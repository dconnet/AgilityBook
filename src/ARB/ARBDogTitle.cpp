/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
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
	, m_Instance(1)
	, m_bReceived(false)
	, m_bHidden(false)
{
}

ARBDogTitle::ARBDogTitle(ARBDogTitle const& rhs)
	: m_Date(rhs.m_Date)
	, m_Venue(rhs.m_Venue)
	, m_Name(rhs.m_Name)
	, m_Instance(rhs.m_Instance)
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
		m_Instance = rhs.m_Instance;
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
		&& m_Instance == rhs.m_Instance
		&& m_bReceived == rhs.m_bReceived
		&& m_bHidden == rhs.m_bHidden;
}

bool ARBDogTitle::operator!=(ARBDogTitle const& rhs) const
{
	return !operator==(rhs);
}

size_t ARBDogTitle::GetSearchStrings(std::set<ARBString>& ioStrings) const
{
	size_t nItems = 0;

	ioStrings.insert(m_Date.GetString(ARBDate::eDashYMD));
	++nItems;

	ioStrings.insert(m_Venue);
	++nItems;

	ioStrings.insert(GetGenericName());
	++nItems;

	return nItems;
}

bool ARBDogTitle::Load(
		ARBConfig const& inConfig,
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (Element::eFound != inTree.GetAttrib(ATTRIB_TITLE_VENUE, m_Venue)
	|| 0 == m_Venue.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_TITLE, ATTRIB_TITLE_VENUE));
		return false;
	}
	if (!inConfig.GetVenues().FindVenue(m_Venue))
	{
		ARBString msg(_T("Unknown venue name: '"));
		msg += m_Venue;
		msg += _T("'");
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_VENUE, msg.c_str()));
		return false;
	}

	if (Element::eFound != inTree.GetAttrib(ATTRIB_TITLE_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_TITLE, ATTRIB_TITLE_NAME));
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
			ioCallback.LogMessage(ErrorMissingAttribute(TREE_TITLE, ATTRIB_TITLE_DATE));
			return false;
		}
		m_bHidden = true;
		break;
	case Element::eInvalidValue:
		{
			ARBString attrib;
			inTree.GetAttrib(ATTRIB_TITLE_DATE, attrib);
			ARBString msg(INVALID_DATE);
			msg += attrib;
			ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_DATE, msg.c_str()));
			return false;
		}
	}

	inTree.GetAttrib(ATTRIB_TITLE_INSTANCE, m_Instance);

	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_TITLE_RECEIVED, m_bReceived))
	{
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_RECEIVED, VALID_VALUES_BOOL));
		return false;
	}

	if (!inConfig.GetVenues().FindTitle(m_Venue, m_Name))
	{
		// This fixes a bug in v1.0.0.8 where the 'nice' name was being written
		// as the title name.
		ARBConfigTitle* pTitle;
		if (!inConfig.GetVenues().FindTitleCompleteName(m_Venue, m_Name, true, &pTitle))
			inConfig.GetVenues().FindTitleCompleteName(m_Venue, m_Name, false, &pTitle);
		if (pTitle)
		{
			m_Name = pTitle->GetName();
			pTitle->Release();
		}
		else
		{
			ARBString msg(INVALID_TITLE);
			msg += m_Venue;
			msg += _T("/");
			msg += m_Name;
			ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_NAME, msg.c_str()));
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
	if (1 < m_Instance)
		title.AddAttrib(ATTRIB_TITLE_INSTANCE, m_Instance);
	if (m_bReceived) // Default is no.
		title.AddAttrib(ATTRIB_TITLE_RECEIVED, m_bReceived);
	return true;
}

inline ARBString ARBDogTitle::GetGenericName() const
{
	ARBString name;
	name = m_Name;
	if (1 < m_Instance)
	{
		ARBostringstream buffer;
		// Keep sync'd with ARBConfigTitle
		buffer << _T(" ") << m_Instance;
		name += buffer.str();
	}
	return name;
}

/////////////////////////////////////////////////////////////////////////////

class SortTitle
{
public:
	SortTitle() {}
	bool operator()(ARBDogTitle* one, ARBDogTitle* two) const
	{
		if (one->GetDate() == two->GetDate())
		{
			if (one->GetVenue() == two->GetVenue())
				return one->GetRawName() < two->GetRawName();
			else
				return one->GetVenue() < two->GetVenue();
		}
		else
			return one->GetDate() < two->GetDate();
	}
};

void ARBDogTitleList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortTitle());
}

int ARBDogTitleList::NumTitlesInVenue(ARBString const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue)
			++count;
	}
	return count;
}

bool ARBDogTitleList::FindTitle(
		ARBString const& inVenue,
		ARBString const& inTitle,
		ARBDogTitle** outTitle) const
{
	if (outTitle)
		*outTitle = NULL;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue
		&& (*iter)->GetRawName() == inTitle)
		{
			if (outTitle)
			{
				*outTitle = *iter;
				(*outTitle)->AddRef();
			}
			return true;
		}
	}
	return false;
}

short ARBDogTitleList::FindMaxInstance(
		ARBString const& inVenue,
		ARBString const& inTitle) const
{
	short inst = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue
		&& (*iter)->GetRawName() == inTitle
		&& (*iter)->GetInstance() > inst)
		{
			inst = (*iter)->GetInstance();
		}
	}
	return inst;
}

int ARBDogTitleList::RenameVenue(
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

int ARBDogTitleList::DeleteVenue(ARBString const& inVenue)
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

int ARBDogTitleList::NumTitlesInDivision(
		ARBConfigVenue const* inVenue,
		ARBString const& inDiv) const
{
	ASSERT(inVenue);
	int count = 0;
	ARBConfigDivision* pDiv;
	if (inVenue->GetDivisions().FindDivision(inDiv, &pDiv))
	{
		for (const_iterator iter = begin(); iter != end(); ++iter)
		{
			if ((*iter)->GetVenue() == inVenue->GetName())
			{
				for (ARBConfigTitleList::const_iterator iterTitles = pDiv->GetTitles().begin(); iterTitles != pDiv->GetTitles().end(); ++iterTitles)
				{
					if ((*iter)->GetRawName() == (*iterTitles)->GetName())
						++count;
				}
			}
		}
		pDiv->Release();
	}
	return count;
}

int ARBDogTitleList::RenameDivision(
		ARBConfigVenue const* inVenue,
		ARBString const& inOldDiv,
		ARBString const& inNewDiv)
{
	// Nothing to do here. Since the division name simply changed and we don't
	// track that here, there's nothing to update.
	return 0;
}

int ARBDogTitleList::DeleteDivision(
		ARBConfig const& inConfig,
		ARBString const& inVenue,
		ARBString const& inDiv)
{
	ARBString venue(inVenue);
	ARBString div(inDiv);
	ARBConfigVenue* pVenue;
	if (!inConfig.GetVenues().FindVenue(venue, &pVenue))
	{
		ASSERT(pVenue);
		return 0;
	}
	int count = 0;
	ARBConfigDivision* pDiv;
	if (pVenue->GetDivisions().FindDivision(div, &pDiv))
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
					if ((*iter)->GetRawName() == (*iterTitles)->GetName())
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
		pDiv->Release();
	}
	return count;
}

int ARBDogTitleList::NumTitlesInUse(
		ARBString const& inVenue,
		ARBString const& inTitle) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetRawName() == inTitle)
			++count;
	}
	return count;
}

int ARBDogTitleList::RenameTitle(
		ARBString const& inVenue,
		ARBString const& inOldTitle,
		ARBString const& inNewTitle)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetRawName() == inOldTitle)
		{
			++count;
			(*iter)->SetName(inNewTitle, (*iter)->GetInstance());
		}
	}
	return count;
}

bool ARBDogTitleList::AddTitle(ARBDogTitle* inTitle)
{
	bool bAdded = false;
	if (inTitle)
	{
		bAdded = true;
		inTitle->AddRef();
		push_back(inTitle);
	}
	return bAdded;
}

bool ARBDogTitleList::DeleteTitle(ARBDogTitle const* inTitle)
{
	if (!inTitle)
		return false;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if (*(*iter) == *inTitle)
		{
			erase(iter);
			return true;
		}
	}
	return false;
}
