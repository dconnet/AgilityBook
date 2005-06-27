/*
 * Copyright © 2003-2005 David Connet. All Rights Reserved.
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
 * @brief Comments about clubs, judges, and location
 * @author David Connet
 *
 * Revision History
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-12-11 DRC Merged separate club/judge/location classes.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-07 DRC Created
 */

#include "StdAfx.h"
#include "ARBInfoItem.h"
#include <algorithm>

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBInfoItem::ARBInfoItem()
	: m_Name()
	, m_Comment()
{
}

ARBInfoItem::ARBInfoItem(
	ARBInfoItem const& rhs)
	: m_Name(rhs.m_Name)
	, m_Comment(rhs.m_Comment)
{
}

ARBInfoItem::~ARBInfoItem()
{
}

ARBInfoItem& ARBInfoItem::operator=(
	ARBInfoItem const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Comment = rhs.m_Comment;
	}
	return *this;
}

bool ARBInfoItem::operator==(
	ARBInfoItem const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Comment == rhs.m_Comment;
}

bool ARBInfoItem::operator!=(
	ARBInfoItem const& rhs) const
{
	return !operator==(rhs);
}

std::string ARBInfoItem::GetGenericName() const
{
	return m_Name;
}

size_t ARBInfoItem::GetSearchStrings(
	std::set<std::string>& ioStrings) const
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

bool ARBInfoItem::Load(
	Element const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback,
	std::string const& inItemName)
{
	if (Element::eNotFound == inTree.GetAttrib(ATTRIB_INFO_NAME, m_Name))
	{
		ioCallback.LogMessage(ErrorMissingAttribute(inItemName.c_str(), ATTRIB_INFO_NAME));
		return false;
	}
	m_Comment = inTree.GetValue();
	return true;
}

bool ARBInfoItem::Save(
	Element& ioTree,
	std::string const& inItemName) const
{
	Element& info = ioTree.AddElement(inItemName);
	info.AddAttrib(ATTRIB_INFO_NAME, m_Name);
	info.SetValue(m_Comment);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

ARBInfoItemList::ARBInfoItemList(
	std::string const& inItemName)
	: m_ItemName(inItemName)
{
}

ARBInfoItemList::ARBInfoItemList(
	ARBInfoItemList const& rhs)
	: ARBVector<ARBInfoItem>(rhs)
	, m_ItemName(rhs.m_ItemName)
{
}

ARBInfoItemList& ARBInfoItemList::operator=(
	ARBInfoItemList const& rhs)
{
	if (this != &rhs)
	{
		ARBVector<ARBInfoItem>::operator=(rhs);
		m_ItemName = rhs.m_ItemName;
	}
	return *this;
}

bool ARBInfoItemList::Load(
	Element const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	ARBInfoItem* item = new ARBInfoItem();
	if (!item->Load(inTree, inVersion, ioCallback, m_ItemName))
	{
		item->Release();
		return false;
	}
	push_back(item);
	return true;
}

bool ARBInfoItemList::Save(
	Element& ioTree) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (!(*iter)->Save(ioTree, m_ItemName))
			return false;
	}
	return true;
}

class SortInfoItem
{
public:
	SortInfoItem(bool bDescending) : m_bDescending(bDescending) {}
	bool operator()(ARBInfoItem* one, ARBInfoItem* two) const
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

void ARBInfoItemList::sort(
	bool inDescending)
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortInfoItem(inDescending));
}

size_t ARBInfoItemList::GetAllItems(
	std::set<std::string>& outNames) const
{
	outNames.clear();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBInfoItem const* info = *iter;
		outNames.insert(info->GetName());
	}
	return outNames.size();
}

void ARBInfoItemList::CondenseContent(
	std::set<std::string> const& inNamesInUse)
{
	// Remove any entries that have empty comments for items that we have
	// shown under. This is simply to keep the file size down.
	for (iterator iter = begin(); iter != end(); )
	{
		ARBInfoItem const* item = *iter;
		if (0 == item->GetComment().length())
		{
			if (inNamesInUse.end() == inNamesInUse.find(item->GetName()))
				++iter;
			else
				iter = erase(iter);
		}
		else
			++iter;
	}
}

bool ARBInfoItemList::FindItem(
	std::string const& inName,
	ARBInfoItem** outItem) const
{
	if (outItem)
		*outItem = NULL;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBInfoItem* info = *iter;
		if (info->GetName() == inName)
		{
			if (outItem)
			{
				*outItem = info;
				(*outItem)->AddRef();
			}
			return true;
		}
	}
	return false;
}

bool ARBInfoItemList::AddItem(
	std::string const& inItem,
	ARBInfoItem** outItem)
{
	if (outItem)
		*outItem = NULL;
	if (!FindItem(inItem))
	{
		if (0 < inItem.length())
		{
			ARBInfoItem* item = new ARBInfoItem();
			item->SetName(inItem);
			push_back(item);
			if (outItem)
			{
				*outItem = item;
				(*outItem)->AddRef();
			}
			return true;
		}
	}
	return false;
}

bool ARBInfoItemList::AddItem(
	ARBInfoItem* inItem)
{
	bool bAdded = false;
	if (inItem && !FindItem(inItem->GetName()))
	{
		bAdded = true;
		inItem->AddRef();
		push_back(inItem);
	}
	return bAdded;
}

bool ARBInfoItemList::DeleteItem(
	ARBInfoItem const* inItem)
{
	if (inItem)
	{
		for (iterator iter = begin(); iter != end(); ++iter)
		{
			if ((*iter) && *(*iter) == *inItem)
			{
				erase(iter);
				return true;
			}
		}
	}
	return false;
}
