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
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-11-22 Added FindRegNum().
 */

#include "stdafx.h"
#include "ARB/ARBDogRegNum.h"

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
class ARBDogRegNum_concrete : public ARBDogRegNum
{
public:
	ARBDogRegNum_concrete()
	{
	}
	ARBDogRegNum_concrete(ARBDogRegNum const& rhs)
		: ARBDogRegNum(rhs)
	{
	}
};
}; // namespace


ARBDogRegNumPtr ARBDogRegNum::New()
{
	return std::make_shared<ARBDogRegNum_concrete>();
}


ARBDogRegNum::ARBDogRegNum()
	: m_Venue()
	, m_Number()
	, m_Height()
	, m_bReceived(false)
	, m_Note()
{
}


ARBDogRegNum::ARBDogRegNum(ARBDogRegNum const& rhs)
	: m_Venue(rhs.m_Venue)
	, m_Number(rhs.m_Number)
	, m_Height(rhs.m_Height)
	, m_bReceived(rhs.m_bReceived)
	, m_Note(rhs.m_Note)
{
}


ARBDogRegNum::ARBDogRegNum(ARBDogRegNum&& rhs)
	: m_Venue(std::move(rhs.m_Venue))
	, m_Number(std::move(rhs.m_Number))
	, m_Height(std::move(rhs.m_Height))
	, m_bReceived(std::move(rhs.m_bReceived))
	, m_Note(std::move(rhs.m_Note))
{
}


ARBDogRegNum::~ARBDogRegNum()
{
}


ARBDogRegNumPtr ARBDogRegNum::Clone() const
{
	return std::make_shared<ARBDogRegNum_concrete>(*this);
}


ARBDogRegNum& ARBDogRegNum::operator=(ARBDogRegNum const& rhs)
{
	if (this != &rhs)
	{
		m_Venue = rhs.m_Venue;
		m_Number = rhs.m_Number;
		m_Height = rhs.m_Height;
		m_bReceived = rhs.m_bReceived;
		m_Note = rhs.m_Note;
	}
	return *this;
}


ARBDogRegNum& ARBDogRegNum::operator=(ARBDogRegNum&& rhs)
{
	if (this != &rhs)
	{
		m_Venue = std::move(rhs.m_Venue);
		m_Number = std::move(rhs.m_Number);
		m_Height = std::move(rhs.m_Height);
		m_bReceived = std::move(rhs.m_bReceived);
		m_Note = std::move(rhs.m_Note);
	}
	return *this;
}


bool ARBDogRegNum::operator==(ARBDogRegNum const& rhs) const
{
	// clang-format off
	return m_Venue == rhs.m_Venue
		&& m_Number == rhs.m_Number
		&& m_Height == rhs.m_Height
		&& m_bReceived == rhs.m_bReceived
		&& m_Note == rhs.m_Note;
	// clang-format on
}


size_t ARBDogRegNum::GetSearchStrings(std::set<std::wstring>& ioStrings) const
{
	size_t nItems = 0;

	ioStrings.insert(GetGenericName());
	++nItems;

	if (0 < m_Note.length())
	{
		ioStrings.insert(m_Note);
		++nItems;
	}

	return nItems;
}


bool ARBDogRegNum::Load(
	ARBConfig const& inConfig,
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_REG_NUM)
		return false;
	if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_REG_NUM_VENUE, m_Venue) || 0 == m_Venue.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_REG_NUM, ATTRIB_REG_NUM_VENUE));
		return false;
	}

	if (inVersion == ARBVersion(1, 0))
	{
		if (ARBAttribLookup::Found != inTree->GetAttrib(L"Number", m_Number) || 0 == m_Number.length())
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_REG_NUM, L"Number"));
			return false;
		}
	}
	else if (inVersion < ARBVersion(9, 0))
		m_Number = inTree->GetValue();
	else
	{
		if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_REG_NUM_NUMBER, m_Number) || 0 == m_Number.length())
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_REG_NUM, ATTRIB_REG_NUM_NUMBER));
			return false;
		}

		m_Note = inTree->GetValue();
	}

	inTree->GetAttrib(ATTRIB_REG_NUM_HEIGHT, m_Height);

	if (ARBAttribLookup::Invalid == inTree->GetAttrib(ATTRIB_REG_NUM_RECEIVED, m_bReceived))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(
			TREE_REG_NUM,
			ATTRIB_REG_NUM_RECEIVED,
			Localization()->ValidValuesBool().c_str()));
		return false;
	}

	if (!inConfig.GetVenues().VerifyVenue(m_Venue))
	{
		std::wstring msg(Localization()->InvalidVenueName());
		msg += m_Venue;
		ioCallback.LogMessage(
			Localization()->ErrorInvalidAttributeValue(TREE_REG_NUM, ATTRIB_REG_NUM_VENUE, msg.c_str()));
		return false;
	}

	return true;
}


bool ARBDogRegNum::Save(ElementNodePtr const& ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr title = ioTree->AddElementNode(TREE_REG_NUM);
	title->AddAttrib(ATTRIB_REG_NUM_VENUE, m_Venue);
	title->AddAttrib(ATTRIB_REG_NUM_NUMBER, m_Number);
	if (0 < m_Height.length())
		title->AddAttrib(ATTRIB_REG_NUM_HEIGHT, m_Height);
	if (m_bReceived)
		title->AddAttrib(ATTRIB_REG_NUM_RECEIVED, m_bReceived);
	if (0 < m_Note.length())
		title->SetValue(m_Note);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBDogRegNumList::Load(
	ARBConfig const& inConfig,
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	ARBDogRegNumPtr thing(ARBDogRegNum::New());
	if (!thing->Load(inConfig, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


void ARBDogRegNumList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), [](ARBDogRegNumPtr const& one, ARBDogRegNumPtr const& two) {
		return StringUtil::CompareNoCase(one->GetVenue(), two->GetVenue()) < 0;
	});
}


int ARBDogRegNumList::NumRegNumsInVenue(std::wstring const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue)
			++count;
	}
	return count;
}


int ARBDogRegNumList::RenameVenue(std::wstring const& inOldVenue, std::wstring const& inNewVenue)
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


int ARBDogRegNumList::DeleteVenue(std::wstring const& inVenue)
{
	std::wstring venue(inVenue);
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


bool ARBDogRegNumList::FindRegNum(std::wstring const& inVenue, ARBDogRegNumPtr* outRegNum) const
{
	if (outRegNum)
		outRegNum->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue)
		{
			if (outRegNum)
				*outRegNum = *iter;
			return true;
		}
	}
	return false;
}


bool ARBDogRegNumList::AddRegNum(std::wstring const& inVenue, std::wstring const& inNumber, ARBDogRegNumPtr* outRegNum)
{
	ARBDogRegNumPtr pRegNum(ARBDogRegNum::New());
	pRegNum->SetVenue(inVenue);
	pRegNum->SetNumber(inNumber);
	push_back(pRegNum);
	if (outRegNum)
		*outRegNum = pRegNum;
	return true;
}


bool ARBDogRegNumList::AddRegNum(ARBDogRegNumPtr const& inRegNum)
{
	bool bAdded = false;
	if (inRegNum)
	{
		bAdded = true;
		push_back(inRegNum);
	}
	return bAdded;
}


int ARBDogRegNumList::DeleteRegNum(std::wstring const& inVenue, std::wstring const& inNumber)
{
	std::wstring venue(inVenue);
	std::wstring number(inNumber);
	int count = 0;
	for (iterator iter = begin(); iter != end();)
	{
		if ((*iter)->GetVenue() == venue && (*iter)->GetNumber() == number)
		{
			iter = erase(iter);
			++count;
		}
		else
			++iter;
	}
	return count;
}

} // namespace ARB
} // namespace dconSoft
