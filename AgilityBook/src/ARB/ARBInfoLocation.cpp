/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * @brief Comments about locations
 * @author David Connet
 *
 * Revision History
 * @li 2004-11-18 DRC Created
 */

#include "StdAfx.h"
#include "ARBInfoLocation.h"
#include <algorithm>

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBInfoLocation::ARBInfoLocation()
	: m_Name()
	, m_Comment()
{
}

ARBInfoLocation::ARBInfoLocation(ARBInfoLocation const& rhs)
	: m_Name(rhs.m_Name)
	, m_Comment(rhs.m_Comment)
{
}

ARBInfoLocation::~ARBInfoLocation()
{
}

ARBInfoLocation& ARBInfoLocation::operator=(ARBInfoLocation const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Comment = rhs.m_Comment;
	}
	return *this;
}

bool ARBInfoLocation::operator==(ARBInfoLocation const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Comment == rhs.m_Comment;
}

bool ARBInfoLocation::operator!=(ARBInfoLocation const& rhs) const
{
	return !operator==(rhs);
}

std::string ARBInfoLocation::GetGenericName() const
{
	return m_Name;
}

size_t ARBInfoLocation::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;

	ioStrings.insert(m_Name);
	++nItems;

	if (0 < m_Comment.length())
	{
		ioStrings.insert(m_Comment);
		++nItems;
	}

	return nItems;
}

bool ARBInfoLocation::Load(
	Element const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	if (Element::eNotFound == inTree.GetAttrib(ATTRIB_LOCATIONINFO_NAME, m_Name))
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_LOCATIONINFO, ATTRIB_LOCATIONINFO_NAME));
		return false;
	}
	m_Comment = inTree.GetValue();
	return true;
}

bool ARBInfoLocation::Save(Element& ioTree) const
{
	Element& info = ioTree.AddElement(TREE_LOCATIONINFO);
	info.AddAttrib(ATTRIB_LOCATIONINFO_NAME, m_Name);
	info.SetValue(m_Comment);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class SortInfoLocation
{
public:
	SortInfoLocation(bool bDescending) : m_bDescending(bDescending) {}
	bool operator()(ARBInfoLocation* one, ARBInfoLocation* two) const
	{
		if (one->GetName() < two->GetName())
			return m_bDescending;
		else if (one->GetName() > two->GetName())
			return !m_bDescending;
		else
			return !m_bDescending;
	}
private:
	bool m_bDescending;
};

void ARBInfoLocationList::sort(bool inDescending)
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortInfoLocation(inDescending));
}

size_t ARBInfoLocationList::GetAllLocations(std::set<std::string>& outNames) const
{
	outNames.clear();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBInfoLocation const* info = *iter;
		outNames.insert(info->GetName());
	}
	return outNames.size();
}

void ARBInfoLocationList::CondenseContent(std::set<std::string> const& inNamesInUse)
{
	// Remove any entries that have empty comments for locations that we have
	// shown under. This is simply to keep the file size down.
	for (iterator iter = begin(); iter != end(); )
	{
		ARBInfoLocation const* location = *iter;
		if (0 == location->GetComment().length())
		{
			if (inNamesInUse.end() == inNamesInUse.find(location->GetName()))
				++iter;
			else
				iter = erase(iter);
		}
		else
			++iter;
	}
}

ARBInfoLocation* ARBInfoLocationList::FindLocation(std::string const& inName) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBInfoLocation* info = *iter;
		if (info->GetName() == inName)
			return info;
	}
	return NULL;
}

ARBInfoLocation* ARBInfoLocationList::AddLocation(std::string const& inLocation)
{
	if (NULL != FindLocation(inLocation))
		return NULL;
	ARBInfoLocation* location = NULL;
	if (0 < inLocation.length())
	{
		location = new ARBInfoLocation();
		location->SetName(inLocation);
		push_back(location);
	}
	return location;
}

ARBInfoLocation* ARBInfoLocationList::AddLocation(ARBInfoLocation* inLocation)
{
	if (!inLocation)
		return NULL;
	if (FindLocation(inLocation->GetName()))
		return NULL;
	inLocation->AddRef();
	push_back(inLocation);
	return inLocation;
}

bool ARBInfoLocationList::DeleteLocation(ARBInfoLocation const* inLocation)
{
	if (inLocation)
	{
		for (iterator iter = begin(); iter != end(); ++iter)
		{
			if ((*iter) && *(*iter) == *inLocation)
			{
				erase(iter);
				return true;
			}
		}
	}
	return false;
}
