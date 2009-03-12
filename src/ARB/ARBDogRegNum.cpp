/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-11-22 DRC Added FindRegNum().
 */

#include "stdafx.h"
#include "ARBDogRegNum.h"
#include <algorithm>

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "ARBLocalization.h"
#include "Element.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

ARBDogRegNumPtr ARBDogRegNum::New()
{
	return ARBDogRegNumPtr(new ARBDogRegNum());
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


ARBDogRegNum::~ARBDogRegNum()
{
}


ARBDogRegNumPtr ARBDogRegNum::Clone() const
{
	return ARBDogRegNumPtr(new ARBDogRegNum(*this));
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


bool ARBDogRegNum::operator==(ARBDogRegNum const& rhs) const
{
	return m_Venue == rhs.m_Venue
		&& m_Number == rhs.m_Number
		&& m_Height == rhs.m_Height
		&& m_bReceived == rhs.m_bReceived
		&& m_Note == rhs.m_Note;
}


size_t ARBDogRegNum::GetSearchStrings(std::set<tstring>& ioStrings) const
{
	ioStrings.insert(GetGenericName());
	ioStrings.insert(GetNote());
	return 1;
}


bool ARBDogRegNum::Load(
		ARBConfig const& inConfig,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_REG_NUM)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_REG_NUM_VENUE, m_Venue)
	|| 0 == m_Venue.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_REG_NUM, ATTRIB_REG_NUM_VENUE));
		return false;
	}

	if (inVersion == ARBVersion(1,0))
	{
		if (ElementNode::eFound != inTree->GetAttrib(wxT("Number"), m_Number)
		|| 0 == m_Number.length())
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_REG_NUM, wxT("Number")));
			return false;
		}
	}
	else if (inVersion < ARBVersion(9,0))
		m_Number = inTree->GetValue();
	else
	{
		if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_REG_NUM_NUMBER, m_Number)
		|| 0 == m_Number.length())
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_REG_NUM, ATTRIB_REG_NUM_NUMBER));
			return false;
		}

		m_Note = inTree->GetValue();
	}

	inTree->GetAttrib(ATTRIB_REG_NUM_HEIGHT, m_Height);

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_REG_NUM_RECEIVED, m_bReceived))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_REG_NUM, ATTRIB_REG_NUM_RECEIVED, Localization()->ValidValuesBool().c_str()));
		return false;
	}

	if (!inConfig.GetVenues().VerifyVenue(m_Venue))
	{
		tstring msg(Localization()->InvalidVenueName());
		msg += m_Venue;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_REG_NUM, ATTRIB_REG_NUM_VENUE, msg.c_str()));
		return false;
	}

	return true;
}


bool ARBDogRegNum::Save(ElementNodePtr ioTree) const
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
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBDogRegNumPtr thing(ARBDogRegNum::New());
	if (!thing->Load(inConfig, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


class SortRegNum
{
public:
	SortRegNum() {}
	bool operator()(ARBDogRegNumPtr one, ARBDogRegNumPtr two) const
	{
		return one->GetVenue() < two->GetVenue();
	}
};


void ARBDogRegNumList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortRegNum());
}


int ARBDogRegNumList::NumRegNumsInVenue(tstring const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetVenue() == inVenue)
			++count;
	}
	return count;
}


int ARBDogRegNumList::RenameVenue(
		tstring const& inOldVenue,
		tstring const& inNewVenue)
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


int ARBDogRegNumList::DeleteVenue(tstring const& inVenue)
{
	tstring venue(inVenue);
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


bool ARBDogRegNumList::FindRegNum(
		tstring const& inVenue,
		ARBDogRegNumPtr* outRegNum) const
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


bool ARBDogRegNumList::AddRegNum(
		tstring const& inVenue,
		tstring const& inNumber,
		ARBDogRegNumPtr* outRegNum)
{
	ARBDogRegNumPtr pRegNum(ARBDogRegNum::New());
	pRegNum->SetVenue(inVenue);
	pRegNum->SetNumber(inNumber);
	push_back(pRegNum);
	if (outRegNum)
		*outRegNum = pRegNum;
	return true;
}


bool ARBDogRegNumList::AddRegNum(ARBDogRegNumPtr inRegNum)
{
	bool bAdded = false;
	if (inRegNum)
	{
		bAdded = true;
		push_back(inRegNum);
	}
	return bAdded;
}


int ARBDogRegNumList::DeleteRegNum(
		tstring const& inVenue,
		tstring const& inNumber)
{
	tstring venue(inVenue);
	tstring number(inNumber);
	int count = 0;
	for (iterator iter = begin(); iter != end(); )
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
