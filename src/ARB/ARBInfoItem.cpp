/*
 * Copyright (c) 2003-2009 David Connet. All Rights Reserved.
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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-12-11 DRC Merged separate club/judge/location classes.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-07 DRC Created
 */

#include "stdafx.h"
#include "ARBInfoItem.h"
#include <algorithm>

#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"
#include "Element.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

ARBInfoItemPtr ARBInfoItem::New()
{
	return ARBInfoItemPtr(new ARBInfoItem());
}


ARBInfoItem::ARBInfoItem()
	: m_Name()
	, m_Comment()
	, m_Visible(true)
{
}


ARBInfoItem::ARBInfoItem(ARBInfoItem const& rhs)
	: m_Name(rhs.m_Name)
	, m_Comment(rhs.m_Comment)
	, m_Visible(rhs.m_Visible)
{
}


ARBInfoItem::~ARBInfoItem()
{
}


ARBInfoItemPtr ARBInfoItem::Clone() const
{
	return ARBInfoItemPtr(new ARBInfoItem(*this));
}


ARBInfoItem& ARBInfoItem::operator=(ARBInfoItem const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Comment = rhs.m_Comment;
		m_Visible = rhs.m_Visible;
	}
	return *this;
}


bool ARBInfoItem::operator==(ARBInfoItem const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Comment == rhs.m_Comment
		&& m_Visible == rhs.m_Visible;
}


size_t ARBInfoItem::GetSearchStrings(std::set<tstring>& ioStrings) const
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
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback,
		tstring const& inItemName)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != inItemName)
		return false;
	if (ElementNode::eNotFound == inTree->GetAttrib(ATTRIB_INFO_NAME, m_Name))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(inItemName.c_str(), ATTRIB_INFO_NAME));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_INFO_VISIBLE, m_Visible))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(inItemName.c_str(), ATTRIB_INFO_VISIBLE, Localization()->ValidValuesBool().c_str()));
		return false;
	}
	m_Comment = inTree->GetValue();
	return true;
}


bool ARBInfoItem::Save(
		ElementNodePtr ioTree,
		tstring const& inItemName) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr info = ioTree->AddElementNode(inItemName);
	info->AddAttrib(ATTRIB_INFO_NAME, m_Name);
	if (!m_Visible)
		info->AddAttrib(ATTRIB_INFO_VISIBLE, m_Visible);
	if (0 < m_Comment.length())
		info->SetValue(m_Comment);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

ARBInfoItemList::ARBInfoItemList(tstring const& inItemName)
	: m_ItemName(inItemName)
{
}


ARBInfoItemList::ARBInfoItemList(ARBInfoItemList const& rhs)
	: ARBVector<ARBInfoItemPtr>(rhs)
	, m_ItemName(rhs.m_ItemName)
{
}


ARBInfoItemList& ARBInfoItemList::operator=(ARBInfoItemList const& rhs)
{
	if (this != &rhs)
	{
		ARBVector<ARBInfoItemPtr>::operator=(rhs);
		m_ItemName = rhs.m_ItemName;
	}
	return *this;
}


bool ARBInfoItemList::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBInfoItemPtr item(ARBInfoItem::New());
	if (!item->Load(inTree, inVersion, ioCallback, m_ItemName))
		return false;
	push_back(item);
	return true;
}


bool ARBInfoItemList::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
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
	SortInfoItem() {}
	bool operator()(ARBInfoItemPtr one, ARBInfoItemPtr two) const
	{
		return one->GetName() < two->GetName();
	}
};


void ARBInfoItemList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortInfoItem());
}


size_t ARBInfoItemList::GetAllItems(
		std::set<tstring>& outNames,
		bool inVisibleOnly) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBInfoItemPtr info = *iter;
		if (!inVisibleOnly || (inVisibleOnly && info->IsVisible()))
			outNames.insert(info->GetName());
		else if (inVisibleOnly && !info->IsVisible())
		{
			std::set<tstring>::iterator i = outNames.find(info->GetName());
			if (i != outNames.end())
				outNames.erase(i);
		}
	}
	return outNames.size();
}


void ARBInfoItemList::CondenseContent(std::set<tstring> const& inNamesInUse)
{
	// Remove any entries that have empty comments for items that we have
	// shown under. This is simply to keep the file size down.
	for (iterator iter = begin(); iter != end(); )
	{
		ARBInfoItemPtr item = *iter;
		if (item->GetComment().empty() && item->IsVisible())
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
		tstring const& inName,
		ARBInfoItemPtr* outItem) const
{
	if (outItem)
		outItem->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBInfoItemPtr info = *iter;
		if (info->GetName() == inName)
		{
			if (outItem)
				*outItem = info;
			return true;
		}
	}
	return false;
}


bool ARBInfoItemList::AddItem(
		tstring const& inItem,
		ARBInfoItemPtr* outItem)
{
	if (outItem)
		outItem->reset();
	if (!FindItem(inItem))
	{
		if (0 < inItem.length())
		{
			ARBInfoItemPtr item(ARBInfoItem::New());
			item->SetName(inItem);
			push_back(item);
			if (outItem)
				*outItem = item;
			return true;
		}
	}
	return false;
}


bool ARBInfoItemList::AddItem(ARBInfoItemPtr inItem)
{
	bool bAdded = false;
	if (inItem && !FindItem(inItem->GetName()))
	{
		bAdded = true;
		push_back(inItem);
	}
	return bAdded;
}


bool ARBInfoItemList::DeleteItem(ARBInfoItemPtr inItem)
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
