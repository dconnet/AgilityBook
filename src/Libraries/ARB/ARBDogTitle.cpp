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
 * 2017-12-09 Fixed recurring title issue when not-last title was deleted.
 * 2015-04-26 Fixed recurring title name (startat wasn't being written)
 * 2013-01-13 Added new recurring title suffix style.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-14 Moved 'Titles' to 'Venue'.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-11 Allow titles to be optionally entered multiple times.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-01-26 Recover from a bug where the title name was the nice name.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "stdafx.h"
#include "ARB/ARBDogTitle.h"

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
class ARBDogTitle_concrete : public ARBDogTitle
{
public:
	ARBDogTitle_concrete()
	{
	}
	ARBDogTitle_concrete(ARBDogTitle const& rhs)
		: ARBDogTitle(rhs)
	{
	}
};
}; // namespace


ARBDogTitlePtr ARBDogTitle::New()
{
	return std::make_shared<ARBDogTitle_concrete>();
}


ARBDogTitle::ARBDogTitle()
	: m_Date()
	, m_Venue()
	, m_Name()
	, m_Instance(1)
	, m_MultipleStartAt(0)
	, m_MultipleIncrement(1)
	, m_bShowInstanceOne(false)
	, m_MultipleStyle(ARBTitleStyle::Number)
	, m_MultipleSeparator(ARBTitleSeparator::None)
	, m_bReceived(false)
	, m_bHidden(false)
{
}


ARBDogTitle::ARBDogTitle(ARBDogTitle const& rhs)
	: m_Date(rhs.m_Date)
	, m_Venue(rhs.m_Venue)
	, m_Name(rhs.m_Name)
	, m_Instance(rhs.m_Instance)
	, m_MultipleStartAt(rhs.m_MultipleStartAt)
	, m_MultipleIncrement(rhs.m_MultipleIncrement)
	, m_bShowInstanceOne(rhs.m_bShowInstanceOne)
	, m_MultipleStyle(rhs.m_MultipleStyle)
	, m_MultipleSeparator(rhs.m_MultipleSeparator)
	, m_bReceived(rhs.m_bReceived)
	, m_bHidden(rhs.m_bHidden)
{
}


ARBDogTitle::ARBDogTitle(ARBDogTitle&& rhs)
	: m_Date(std::move(rhs.m_Date))
	, m_Venue(std::move(rhs.m_Venue))
	, m_Name(std::move(rhs.m_Name))
	, m_Instance(std::move(rhs.m_Instance))
	, m_MultipleStartAt(std::move(rhs.m_MultipleStartAt))
	, m_MultipleIncrement(std::move(rhs.m_MultipleIncrement))
	, m_bShowInstanceOne(std::move(rhs.m_bShowInstanceOne))
	, m_MultipleStyle(std::move(rhs.m_MultipleStyle))
	, m_MultipleSeparator(std::move(rhs.m_MultipleSeparator))
	, m_bReceived(std::move(rhs.m_bReceived))
	, m_bHidden(std::move(rhs.m_bHidden))
{
}


ARBDogTitle::~ARBDogTitle()
{
}


ARBDogTitlePtr ARBDogTitle::Clone() const
{
	return std::make_shared<ARBDogTitle_concrete>(*this);
}


ARBDogTitle& ARBDogTitle::operator=(ARBDogTitle const& rhs)
{
	if (this != &rhs)
	{
		m_Date = rhs.m_Date;
		m_Venue = rhs.m_Venue;
		m_Name = rhs.m_Name;
		m_Instance = rhs.m_Instance;
		m_MultipleStartAt = rhs.m_MultipleStartAt;
		m_MultipleIncrement = rhs.m_MultipleIncrement;
		m_bShowInstanceOne = rhs.m_bShowInstanceOne;
		m_MultipleStyle = rhs.m_MultipleStyle;
		m_MultipleSeparator = rhs.m_MultipleSeparator;
		m_bReceived = rhs.m_bReceived;
		m_bHidden = rhs.m_bHidden;
	}
	return *this;
}


ARBDogTitle& ARBDogTitle::operator=(ARBDogTitle&& rhs)
{
	if (this != &rhs)
	{
		m_Date = std::move(rhs.m_Date);
		m_Venue = std::move(rhs.m_Venue);
		m_Name = std::move(rhs.m_Name);
		m_Instance = std::move(rhs.m_Instance);
		m_MultipleStartAt = std::move(rhs.m_MultipleStartAt);
		m_MultipleIncrement = std::move(rhs.m_MultipleIncrement);
		m_bShowInstanceOne = std::move(rhs.m_bShowInstanceOne);
		m_MultipleStyle = std::move(rhs.m_MultipleStyle);
		m_MultipleSeparator = std::move(rhs.m_MultipleSeparator);
		m_bReceived = std::move(rhs.m_bReceived);
		m_bHidden = std::move(rhs.m_bHidden);
	}
	return *this;
}


bool ARBDogTitle::operator==(ARBDogTitle const& rhs) const
{
	// clang-format off
	return m_Date == rhs.m_Date
		&& m_Venue == rhs.m_Venue
		&& m_Name == rhs.m_Name
		&& m_Instance == rhs.m_Instance
		&& m_MultipleStartAt == rhs.m_MultipleStartAt
		&& m_MultipleIncrement == rhs.m_MultipleIncrement
		&& m_bShowInstanceOne == rhs.m_bShowInstanceOne
		&& m_MultipleStyle == rhs.m_MultipleStyle
		&& m_MultipleSeparator == rhs.m_MultipleSeparator
		&& m_bReceived == rhs.m_bReceived
		&& m_bHidden == rhs.m_bHidden;
	// clang-format on
}


wxString ARBDogTitle::GetGenericName() const
{
	wxString name = m_Name
					+ TitleInstance(
						m_bShowInstanceOne,
						m_Instance,
						m_MultipleStartAt,
						m_MultipleIncrement,
						m_MultipleStyle,
						m_MultipleSeparator);
	return name;
}


size_t ARBDogTitle::GetSearchStrings(std::set<wxString>& ioStrings) const
{
	size_t nItems = 0;

	ioStrings.insert(m_Date.GetString(ARBDateFormat::DashYMD));
	++nItems;

	ioStrings.insert(m_Venue);
	++nItems;

	ioStrings.insert(GetGenericName());
	++nItems;

	return nItems;
}


bool ARBDogTitle::Load(
	ARBConfig const& inConfig,
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_TITLE)
		return false;
	if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_TITLE_VENUE, m_Venue) || 0 == m_Venue.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_TITLE, ATTRIB_TITLE_VENUE));
		return false;
	}
	if (!inConfig.GetVenues().FindVenue(m_Venue))
	{
		wxString msg(Localization()->InvalidVenueName());
		msg += m_Venue;
		msg += L"'";
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_VENUE, msg));
		return false;
	}

	if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_TITLE_NAME, m_Name) || 0 == m_Name.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_TITLE, ATTRIB_TITLE_NAME));
		return false;
	}

	// Must get Hidden before getting date.
	inTree->GetAttrib(ATTRIB_TITLE_HIDDEN, m_bHidden);

	switch (inTree->GetAttrib(ATTRIB_TITLE_DATE, m_Date))
	{
	case ARBAttribLookup::Found:
		break;
	case ARBAttribLookup::NotFound:
		// As of version 8.5, no date infers this is an unearned title
		// that we're hiding.
		if (inVersion < ARBVersion(8, 5))
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_TITLE, ATTRIB_TITLE_DATE));
			return false;
		}
		m_bHidden = true;
		break;
	case ARBAttribLookup::Invalid:
	{
		wxString attrib;
		inTree->GetAttrib(ATTRIB_TITLE_DATE, attrib);
		wxString msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_DATE, msg));
		return false;
	}
	}

	inTree->GetAttrib(ATTRIB_TITLE_INSTANCE, m_Instance);
	inTree->GetAttrib(ATTRIB_TITLE_INSTANCE_STARTAT, m_MultipleStartAt);
	inTree->GetAttrib(ATTRIB_TITLE_INSTANCE_INC, m_MultipleIncrement);
	inTree->GetAttrib(ATTRIB_TITLE_INSTANCE_SHOW, m_bShowInstanceOne);
	LoadTitleStyle(inTree, ATTRIB_TITLE_INSTANCE_STYLE, inVersion, m_MultipleStyle);
	LoadTitleSeparator(inTree, ATTRIB_TITLE_INSTANCE_SEP, inVersion, m_MultipleStyle, m_MultipleSeparator);

	if (ARBAttribLookup::Invalid == inTree->GetAttrib(ATTRIB_TITLE_RECEIVED, m_bReceived))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(
			TREE_TITLE,
			ATTRIB_TITLE_RECEIVED,
			Localization()->ValidValuesBool()));
		return false;
	}

	ARBConfigTitlePtr pConfigTitle;
	if (inConfig.GetVenues().FindTitle(m_Venue, m_Name, &pConfigTitle))
	{
		// Fix a bug in v3.1.0 that appeared in GetGenericName
		if (pConfigTitle->IsRecurring() && 0 == m_MultipleStartAt)
		{
			m_MultipleStartAt = pConfigTitle->GetMultipleStartAt();
		}
	}
	else
	{
		// This fixes a bug in v1.0.0.8 where the 'nice' name was being written
		// as the title name.
		ARBConfigTitlePtr pTitle;
		if (!inConfig.GetVenues().FindTitleCompleteName(m_Venue, m_Name, true, &pTitle))
			inConfig.GetVenues().FindTitleCompleteName(m_Venue, m_Name, false, &pTitle);
		if (pTitle)
		{
			m_Name = pTitle->GetName();
		}
		else
		{
			wxString msg(Localization()->InvalidTitle());
			msg += m_Venue;
			msg += L"/";
			msg += m_Name;
			ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_TITLE, ATTRIB_TITLE_NAME, msg));
			return false;
		}
	}

	return true;
}


bool ARBDogTitle::Save(ElementNodePtr const& ioTree) const
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
	if (1 < m_Instance)
		title->AddAttrib(ATTRIB_TITLE_INSTANCE, m_Instance);
	if (0 < m_MultipleStartAt)
		title->AddAttrib(ATTRIB_TITLE_INSTANCE_STARTAT, m_MultipleStartAt);
	if (1 < m_MultipleIncrement)
		title->AddAttrib(ATTRIB_TITLE_INSTANCE_INC, m_MultipleIncrement);
	if (m_bShowInstanceOne)
		title->AddAttrib(ATTRIB_TITLE_INSTANCE_SHOW, m_bShowInstanceOne);
	SaveTitleStyle(title, ATTRIB_TITLE_INSTANCE_STYLE, m_MultipleStyle);
	SaveTitleSeparator(title, ATTRIB_TITLE_INSTANCE_SEP, m_MultipleSeparator);
	if (m_bReceived) // Default is no.
		title->AddAttrib(ATTRIB_TITLE_RECEIVED, m_bReceived);
	return true;
}


void ARBDogTitle::SetName(wxString const& inName, short inInstance, ARBConfigTitlePtr const& inConfigTitle)
{
	m_Name = inName;
	m_Instance = inInstance;
	m_MultipleStartAt = inConfigTitle->GetMultipleStartAt();
	m_bShowInstanceOne = inConfigTitle->ShowMultipleOnFirstInstance();
	m_MultipleIncrement = inConfigTitle->GetMultipleIncrement();
	m_MultipleStyle = inConfigTitle->GetMultipleStyle();
	m_MultipleSeparator = inConfigTitle->GetMultipleStyleSeparator();
}

/////////////////////////////////////////////////////////////////////////////

bool ARBDogTitleList::Load(
	ARBConfig const& inConfig,
	ElementNodePtr const& inTree,
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
	SortTitle()
	{
	}
	bool operator()(ARBDogTitlePtr const& one, ARBDogTitlePtr const& two) const
	{
		if (one->GetDate() == two->GetDate())
		{
			if (one->GetVenue() == two->GetVenue())
				return StringUtil::CompareNoCase(one->GetRawName(), two->GetRawName()) < 0;
			else
				return StringUtil::CompareNoCase(one->GetVenue(), two->GetVenue()) < 0;
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


bool ARBDogTitleList::FindTitle(wxString const& inVenue, wxString const& inTitle, ARBDogTitlePtr* outTitle) const
{
	if (outTitle)
		outTitle->reset();
	ARBDogTitlePtr pTitle;
	int maxInstance = -1;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetRawName() == inTitle)
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


short ARBDogTitleList::FindMaxInstance(wxString const& inVenue, wxString const& inTitle, std::vector<short>* outMissing)
	const
{
	std::set<short> instances;
	short inst = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetRawName() == inTitle)
		{
			if ((*iter)->GetInstance() > inst)
			{
				inst = (*iter)->GetInstance();
			}
			if (outMissing)
			{
				instances.insert((*iter)->GetInstance());
			}
		}
	}
	if (outMissing && static_cast<short>(instances.size()) < inst)
	{
		outMissing->clear();
		for (short i = 1; i <= inst; ++i)
		{
			if (instances.end() == instances.find(i))
				outMissing->push_back(i);
		}
	}
	return inst;
}


int ARBDogTitleList::RenameVenue(wxString const& inOldVenue, wxString const& inNewVenue)
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


int ARBDogTitleList::NumTitlesInUse(wxString const& inVenue, wxString const& inTitle) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetRawName() == inTitle)
			++count;
	}
	return count;
}


int ARBDogTitleList::RenameTitle(wxString const& inVenue, wxString const& inOldTitle, wxString const& inNewTitle)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue && (*iter)->GetRawName() == inOldTitle)
		{
			++count;
			(*iter)->Rename(inNewTitle);
		}
	}
	return count;
}


bool ARBDogTitleList::AddTitle(ARBDogTitlePtr const& inTitle)
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


bool ARBDogTitleList::DeleteTitle(ARBConfigVenuePtr const& inVenue, ARBDogTitlePtr const& inTitle)
{
	bool bDeleted = false;
	if (!inTitle)
		return bDeleted;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if (*(*iter) == *inTitle)
		{
			erase(iter);
			bDeleted = true;
			break;
		}
	}

	// Fix recurring titles.
	ARBConfigTitlePtr title;
	if (inVenue && inVenue->GetTitles().FindTitle(inTitle->GetRawName(), &title) && title->IsRecurring())
	{
		std::vector<ARBDogTitlePtr> titles;
		for (iterator iter = begin(); iter != end(); ++iter)
		{
			if ((*iter)->GetVenue() == inVenue->GetName() && (*iter)->GetRawName() == title->GetName())
			{
				titles.push_back(*iter);
			}
		}
		// Titles are already sorted.
		short instance = 1;
		for (auto iter = titles.begin(); iter != titles.end(); ++iter, ++instance)
		{
			(*iter)->SetName(title->GetName(), instance, title);
		}
	}
	return bDeleted;
}

} // namespace ARB
} // namespace dconSoft
