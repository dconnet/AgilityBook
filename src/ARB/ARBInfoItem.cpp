/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Comments about clubs, judges, and location
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
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

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
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


size_t ARBInfoItem::GetSearchStrings(std::set<wxString>& ioStrings) const
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
		wxString const& inItemName)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != inItemName)
		return false;
	if (ElementNode::eNotFound == inTree->GetAttrib(ATTRIB_INFO_NAME, m_Name))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(inItemName, ATTRIB_INFO_NAME));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_INFO_VISIBLE, m_Visible))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(inItemName, ATTRIB_INFO_VISIBLE, Localization()->ValidValuesBool()));
		return false;
	}
	m_Comment = inTree->GetValue();
	return true;
}


bool ARBInfoItem::Save(
		ElementNodePtr ioTree,
		wxString const& inItemName) const
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

ARBInfoItemList::ARBInfoItemList(wxString const& inItemName)
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
		std::set<wxString>& outNames,
		bool inVisibleOnly) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBInfoItemPtr info = *iter;
		if (!inVisibleOnly || (inVisibleOnly && info->IsVisible()))
			outNames.insert(info->GetName());
		else if (inVisibleOnly && !info->IsVisible())
		{
			std::set<wxString>::iterator i = outNames.find(info->GetName());
			if (i != outNames.end())
				outNames.erase(i);
		}
	}
	return outNames.size();
}


void ARBInfoItemList::CondenseContent(std::set<wxString> const& inNamesInUse)
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
		wxString const& inName,
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
		wxString const& inItem,
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
