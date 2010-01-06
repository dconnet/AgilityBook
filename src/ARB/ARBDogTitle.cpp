/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
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
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-01-26 DRC Recover from a bug where the title name was the nice name.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "stdafx.h"
#include "ARBDogTitle.h"
#include <algorithm>

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "ARBLocalization.h"
#include "Element.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

ARBDogTitlePtr ARBDogTitle::New()
{
	return ARBDogTitlePtr(new ARBDogTitle());
}


ARBDogTitle::ARBDogTitle()
	: m_Date()
	, m_Venue()
	, m_Name()
	, m_bShowInstanceOne(false)
	, m_Instance(1)
	, m_MultipleStyle(eTitleNumber)
	, m_bReceived(false)
	, m_bHidden(false)
{
}


ARBDogTitle::ARBDogTitle(ARBDogTitle const& rhs)
	: m_Date(rhs.m_Date)
	, m_Venue(rhs.m_Venue)
	, m_Name(rhs.m_Name)
	, m_bShowInstanceOne(rhs.m_bShowInstanceOne)
	, m_Instance(rhs.m_Instance)
	, m_MultipleStyle(rhs.m_MultipleStyle)
	, m_bReceived(rhs.m_bReceived)
	, m_bHidden(rhs.m_bHidden)
{
}


ARBDogTitle::~ARBDogTitle()
{
}


ARBDogTitlePtr ARBDogTitle::Clone() const
{
	return ARBDogTitlePtr(new ARBDogTitle(*this));
}


ARBDogTitle& ARBDogTitle::operator=(ARBDogTitle const& rhs)
{
	if (this != &rhs)
	{
		m_Date = rhs.m_Date;
		m_Venue = rhs.m_Venue;
		m_Name = rhs.m_Name;
		m_bShowInstanceOne = rhs.m_bShowInstanceOne;
		m_Instance = rhs.m_Instance;
		m_MultipleStyle = rhs.m_MultipleStyle;
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
		&& m_bShowInstanceOne == rhs.m_bShowInstanceOne
		&& m_Instance == rhs.m_Instance
		&& m_MultipleStyle == rhs.m_MultipleStyle
		&& m_bReceived == rhs.m_bReceived
		&& m_bHidden == rhs.m_bHidden;
}


wxString ARBDogTitle::GetGenericName() const
{
	wxString name = m_Name + TitleInstance(m_bShowInstanceOne, m_Instance, m_MultipleStyle);
	return name;
}


size_t ARBDogTitle::GetSearchStrings(std::set<wxString>& ioStrings) const
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
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_TITLE)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_TITLE_VENUE, m_Venue)
	|| 0 == m_Venue.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_TITLE, ATTRIB_TITLE_VENUE));
		return false;
	}
	if (!inConfig.GetVenues().FindVenue(m_Venue))
	{
		wxString msg(Localization()->InvalidVenueName());
		msg += m_Venue;
		msg += wxT("'");
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_VENUE, msg));
		return false;
	}

	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_TITLE_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_TITLE, ATTRIB_TITLE_NAME));
		return false;
	}

	// Must get Hidden before getting date.
	inTree->GetAttrib(ATTRIB_TITLE_HIDDEN, m_bHidden);

	switch (inTree->GetAttrib(ATTRIB_TITLE_DATE, m_Date))
	{
	case ElementNode::eNotFound:
		// As of version 8.5, no date infers this is an unearned title
		// that we're hiding.
		if (inVersion < ARBVersion(8, 5))
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_TITLE, ATTRIB_TITLE_DATE));
			return false;
		}
		m_bHidden = true;
		break;
	case ElementNode::eInvalidValue:
		{
			wxString attrib;
			inTree->GetAttrib(ATTRIB_TITLE_DATE, attrib);
			wxString msg(Localization()->InvalidDate());
			msg += attrib;
			ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_DATE, msg));
			return false;
		}
	}

	inTree->GetAttrib(ATTRIB_TITLE_INSTANCE_SHOW, m_bShowInstanceOne);
	inTree->GetAttrib(ATTRIB_TITLE_INSTANCE, m_Instance);
	if (1 < m_Instance)
		m_bShowInstanceOne = true;
	short tmp;
	if (ElementNode::eFound == inTree->GetAttrib(ATTRIB_TITLE_STYLE, tmp))
		m_MultipleStyle = static_cast<ARBTitleStyle>(tmp);

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_TITLE_RECEIVED, m_bReceived))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_RECEIVED, Localization()->ValidValuesBool()));
		return false;
	}

	if (!inConfig.GetVenues().FindTitle(m_Venue, m_Name))
	{
		// This fixes a bug in v1.0.0.8 where the 'nice' name was being written
		// as the title name.
		ARBConfigTitlePtr pTitle;
		if (!inConfig.GetVenues().FindTitleCompleteName(m_Venue, m_Name, m_bShowInstanceOne, true, &pTitle))
			inConfig.GetVenues().FindTitleCompleteName(m_Venue, m_Name, m_bShowInstanceOne, false, &pTitle);
		if (pTitle)
		{
			m_Name = pTitle->GetName();
		}
		else
		{
			wxString msg(Localization()->InvalidTitle());
			msg += m_Venue;
			msg += wxT("/");
			msg += m_Name;
			ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_NAME, msg));
			return false;
		}
	}

	return true;
}


bool ARBDogTitle::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr title = ioTree->AddElementNode(TREE_TITLE);
	if (m_Date.IsValid())
	{
		title->AddAttrib(ATTRIB_TITLE_DATE, m_Date);
		if (m_bHidden) // Default is no
			title->AddAttrib(ATTRIB_TITLE_HIDDEN, m_bHidden);
	}
	else
	{
		bool bTemp = true;
		title->AddAttrib(ATTRIB_TITLE_HIDDEN, bTemp);
	}
	title->AddAttrib(ATTRIB_TITLE_VENUE, m_Venue);
	title->AddAttrib(ATTRIB_TITLE_NAME, m_Name);
	if (1 == m_Instance && m_bShowInstanceOne)
		title->AddAttrib(ATTRIB_TITLE_INSTANCE_SHOW, m_bShowInstanceOne);
	if (1 < m_Instance)
		title->AddAttrib(ATTRIB_TITLE_INSTANCE, m_Instance);
	if (eTitleNumber != m_MultipleStyle)
		title->AddAttrib(ATTRIB_TITLE_STYLE, static_cast<short>(m_MultipleStyle));
	if (m_bReceived) // Default is no.
		title->AddAttrib(ATTRIB_TITLE_RECEIVED, m_bReceived);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBDogTitleList::Load(
		ARBConfig const& inConfig,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBDogTitlePtr thing(ARBDogTitle::New());
	if (!thing->Load(inConfig, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


class SortTitle
{
public:
	SortTitle() {}
	bool operator()(ARBDogTitlePtr one, ARBDogTitlePtr two) const
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


int ARBDogTitleList::NumTitlesInVenue(wxString const& inVenue) const
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
		wxString const& inVenue,
		wxString const& inTitle,
		ARBDogTitlePtr* outTitle) const
{
	if (outTitle)
		outTitle->reset();
	ARBDogTitlePtr pTitle;
	int maxInstance = -1;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue
		&& (*iter)->GetRawName() == inTitle)
		{
			if (maxInstance < (*iter)->GetInstance())
			{
				maxInstance = (*iter)->GetInstance();
				pTitle = *iter;
			}
		}
	}
	if (outTitle)
		*outTitle = pTitle;
	return -1 < maxInstance;
}


short ARBDogTitleList::FindMaxInstance(
		wxString const& inVenue,
		wxString const& inTitle) const
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
		wxString const& inOldVenue,
		wxString const& inNewVenue)
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


int ARBDogTitleList::DeleteVenue(wxString const& inVenue)
{
	wxString venue(inVenue);
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


int ARBDogTitleList::NumTitlesInUse(
		wxString const& inVenue,
		wxString const& inTitle) const
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
		wxString const& inVenue,
		wxString const& inOldTitle,
		wxString const& inNewTitle)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetRawName() == inOldTitle)
		{
			++count;
			(*iter)->SetName(inNewTitle, (*iter)->GetInstance(), (*iter)->ShowInstanceOne(), (*iter)->GetStyle());
		}
	}
	return count;
}


bool ARBDogTitleList::AddTitle(ARBDogTitlePtr inTitle)
{
	bool bAdded = false;
	if (inTitle)
	{
		bAdded = true;
		push_back(inTitle);
		sort();
	}
	return bAdded;
}


bool ARBDogTitleList::DeleteTitle(ARBDogTitlePtr inTitle)
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
