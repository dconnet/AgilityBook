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
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-11-22 DRC Added FindRegNum().
 */

#include "StdAfx.h"
#include "ARBDogRegNum.h"
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

ARBDogRegNum::ARBDogRegNum()
	: m_Venue()
	, m_Number()
	, m_Height()
	, m_bReceived(false)
{
}

ARBDogRegNum::ARBDogRegNum(const ARBDogRegNum& rhs)
	: m_Venue(rhs.m_Venue)
	, m_Number(rhs.m_Number)
	, m_Height(rhs.m_Height)
	, m_bReceived(rhs.m_bReceived)
{
}

ARBDogRegNum::~ARBDogRegNum()
{
}

ARBDogRegNum& ARBDogRegNum::operator=(const ARBDogRegNum& rhs)
{
	if (this != &rhs)
	{
		m_Venue = rhs.m_Venue;
		m_Number = rhs.m_Number;
		m_Height = rhs.m_Height;
		m_bReceived = rhs.m_bReceived;
	}
	return *this;
}

bool ARBDogRegNum::operator==(const ARBDogRegNum& rhs) const
{
	return m_Venue == rhs.m_Venue
		&& m_Number == rhs.m_Number
		&& m_Height == rhs.m_Height
		&& m_bReceived == rhs.m_bReceived;
}

bool ARBDogRegNum::operator!=(const ARBDogRegNum& rhs) const
{
	return !operator==(rhs);
}

std::string ARBDogRegNum::GetGenericName() const
{
	return GetVenue() + " " + GetNumber();
}

size_t ARBDogRegNum::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	ioStrings.insert(GetGenericName());
	return 1;
}

bool ARBDogRegNum::Load(
	const ARBConfig& inConfig,
	const CElement& inTree,
	const ARBVersion& inVersion,
	std::string& ioErrMsg)
{
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_REG_NUM_VENUE, m_Venue)
	|| 0 == m_Venue.length())
	{
		ioErrMsg += ErrorMissingAttribute(TREE_REG_NUM, ATTRIB_REG_NUM_VENUE);
		return false;
	}

	if (inVersion == ARBVersion(1,0))
	{
		if (CElement::eFound != inTree.GetAttrib("Number", m_Number)
		|| 0 == m_Number.length())
		{
			ioErrMsg += ErrorMissingAttribute(TREE_REG_NUM, "Number");
			return false;
		}
	}
	else
		m_Number = inTree.GetValue();

	inTree.GetAttrib(ATTRIB_REG_NUM_HEIGHT, m_Height);

	if (CElement::eInvalidValue == inTree.GetAttrib(ATTRIB_REG_NUM_RECEIVED, m_bReceived))
	{
		ioErrMsg += ErrorInvalidAttributeValue(TREE_REG_NUM, ATTRIB_REG_NUM_RECEIVED, VALID_VALUES_BOOL);
		return false;
	}

	if (!inConfig.GetVenues().VerifyVenue(m_Venue))
	{
		std::string msg(INVALID_VENUE_NAME);
		msg += m_Venue;
		ioErrMsg += ErrorInvalidAttributeValue(TREE_REG_NUM, ATTRIB_REG_NUM_VENUE, msg.c_str());
		return false;
	}

	return true;
}

bool ARBDogRegNum::Save(CElement& ioTree) const
{
	CElement& title = ioTree.AddElement(TREE_REG_NUM);
	title.AddAttrib(ATTRIB_REG_NUM_VENUE, m_Venue);
	if (0 < m_Height.length())
		title.AddAttrib(ATTRIB_REG_NUM_HEIGHT, m_Height);
	if (m_bReceived)
		title.AddAttrib(ATTRIB_REG_NUM_RECEIVED, m_bReceived);
	title.SetValue(m_Number);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class SortRegNum
{
public:
	SortRegNum(bool bDescending) : m_bDescending(bDescending) {}
	bool operator()(ARBDogRegNum* one, ARBDogRegNum* two) const
	{
		if (one->GetVenue() < two->GetVenue())
			return m_bDescending;
		else
			return !m_bDescending;
	}
private:
	bool m_bDescending;
};

void ARBDogRegNumList::sort(bool inDescending)
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortRegNum(inDescending));
}

int ARBDogRegNumList::NumRegNumsInVenue(const std::string& inVenue) const
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

int ARBDogRegNumList::DeleteVenue(const std::string& inVenue)
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

const ARBDogRegNum* ARBDogRegNumList::FindRegNum(const std::string& inVenue) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
		if ((*iter)->GetVenue() == inVenue)
			return *iter;
	return NULL;
}

int ARBDogRegNumList::DeleteRegNum(const std::string& inVenue, const std::string& inNumber)
{
	std::string venue(inVenue);
	std::string number(inNumber);
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

ARBDogRegNum* ARBDogRegNumList::AddRegNum(
	const std::string& inVenue,
	const std::string& inNumber)
{
	ARBDogRegNum* pRegNum = new ARBDogRegNum();
	pRegNum->SetVenue(inVenue);
	pRegNum->SetNumber(inNumber);
	push_back(pRegNum);
	return pRegNum;
}
