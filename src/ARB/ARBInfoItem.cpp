/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Comments about clubs, judges, and location
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-12-11 Merged separate club/judge/location classes.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-12-07 Created
 */

#include "stdafx.h"
#include "ARB/ARBInfoItem.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class ARBInfoItem_concrete : public ARBInfoItem
	{
	public:
		ARBInfoItem_concrete() {}
		ARBInfoItem_concrete(ARBInfoItem const& rhs)
			: ARBInfoItem(rhs)
		{
		}
	};
};


ARBInfoItemPtr ARBInfoItem::New()
{
	return std::make_shared<ARBInfoItem_concrete>();
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
	return std::make_shared<ARBInfoItem_concrete>(*this);
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


size_t ARBInfoItem::GetSearchStrings(std::set<std::wstring>& ioStrings) const
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
		std::wstring const& inItemName)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != inItemName)
		return false;
	if (ElementNode::eNotFound == inTree->GetAttrib(ATTRIB_INFO_NAME, m_Name))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(inItemName.c_str(), ATTRIB_INFO_NAME).c_str());
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
		std::wstring const& inItemName) const
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

ARBInfoItemList::ARBInfoItemList(std::wstring const& inItemName)
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


#ifndef ARB_HAS_LAMBDA
class SortInfoItem
{
public:
	SortInfoItem() {}
	bool operator()(ARBInfoItemPtr const& one, ARBInfoItemPtr const& two) const
	{
		return StringUtil::CompareNoCase(one->GetName(), two->GetName()) < 0;
	}
};
#endif


void ARBInfoItemList::sort()
{
	if (2 > size())
		return;
#ifdef ARB_HAS_LAMBDA
	std::stable_sort(begin(), end(),
		[](ARBInfoItemPtr const& one, ARBInfoItemPtr const& two)
		{
			return StringUtil::CompareNoCase(one->GetName(), two->GetName()) < 0;
		}
	);
#else
	std::stable_sort(begin(), end(), SortInfoItem());
#endif
}


size_t ARBInfoItemList::GetAllItems(
		std::set<std::wstring>& outNames,
		bool inVisibleOnly) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBInfoItemPtr info = *iter;
		if (!inVisibleOnly || (inVisibleOnly && info->IsVisible()))
			outNames.insert(info->GetName());
		else if (inVisibleOnly && !info->IsVisible())
		{
			std::set<std::wstring>::iterator i = outNames.find(info->GetName());
			if (i != outNames.end())
				outNames.erase(i);
		}
	}
	return outNames.size();
}


void ARBInfoItemList::CondenseContent(std::set<std::wstring> const& inNamesInUse)
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
		std::wstring const& inName,
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
		std::wstring const& inItem,
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
