/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief The classes that make up the configuration information.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-07-15 Created.
 */

#include "stdafx.h"
#include "ARB/ARBConfigMultiQ.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class ARBConfigMultiQ_concrete : public ARBConfigMultiQ
	{
	public:
		ARBConfigMultiQ_concrete() {}
		ARBConfigMultiQ_concrete(ARBConfigMultiQ const& rhs)
			: ARBConfigMultiQ(rhs)
		{
		}
	};
};


ARBConfigMultiQPtr ARBConfigMultiQ::New()
{
	return std::make_shared<ARBConfigMultiQ_concrete>();
}


ARBConfigMultiQ::ARBConfigMultiQ()
	: m_Name()
	, m_ShortName()
	, m_ValidFrom()
	, m_ValidTo()
	, m_Items()
{
}


ARBConfigMultiQ::ARBConfigMultiQ(ARBConfigMultiQ const& rhs)
	: m_Name(rhs.m_Name)
	, m_ShortName(rhs.m_ShortName)
	, m_ValidFrom(rhs.m_ValidFrom)
	, m_ValidTo(rhs.m_ValidTo)
	, m_Items(rhs.m_Items)
{
}


ARBConfigMultiQ::~ARBConfigMultiQ()
{
}


ARBConfigMultiQPtr ARBConfigMultiQ::Clone() const
{
	return std::make_shared<ARBConfigMultiQ_concrete>(*this);
}


ARBConfigMultiQ& ARBConfigMultiQ::operator=(ARBConfigMultiQ const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_ShortName = rhs.m_ShortName;
		m_ValidFrom = rhs.m_ValidFrom;
		m_ValidTo = rhs.m_ValidTo;
		m_Items = rhs.m_Items;
	}
	return *this;
}


bool ARBConfigMultiQ::operator==(ARBConfigMultiQ const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_ShortName == rhs.m_ShortName
		&& m_ValidFrom == rhs.m_ValidFrom
		&& m_ValidTo == rhs.m_ValidTo
		&& m_Items == rhs.m_Items;
}


bool ARBConfigMultiQ::Load(
		ARBConfigVenue const& inVenue,
		ElementNodePtr const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_MULTIQ)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_MULTIQ_NAME, m_Name))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_MULTIQ_ITEM, ATTRIB_MULTIQ_NAME));
		return false;
	}
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_MULTIQ_SHORTNAME, m_ShortName))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_MULTIQ_ITEM, ATTRIB_MULTIQ_SHORTNAME));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_MULTIQ_VALID_FROM, m_ValidFrom))
	{
		std::wstring attrib;
		inTree->GetAttrib(ATTRIB_MULTIQ_VALID_FROM, attrib);
		std::wstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_MULTIQ, ATTRIB_MULTIQ_VALID_FROM, msg.c_str()));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_MULTIQ_VALID_TO, m_ValidTo))
	{
		std::wstring attrib;
		inTree->GetAttrib(ATTRIB_MULTIQ_VALID_TO, attrib);
		std::wstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_MULTIQ, ATTRIB_MULTIQ_VALID_TO, msg.c_str()));
		return false;
	}

	for (int i = 0; i < inTree->GetElementCount(); ++i)
	{
		ElementNodePtr element = inTree->GetElementNode(i);
		if (!element)
			continue;
		if (element->GetName() == TREE_MULTIQ_ITEM)
		{
			MultiQItem item;
			// Read the data.
			if (ElementNode::eFound != element->GetAttrib(ATTRIB_MULTIQ_ITEM_DIV, item.m_Div)
			|| 0 == item.m_Div.length())
			{
				ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_MULTIQ_ITEM, ATTRIB_MULTIQ_ITEM_DIV));
				return false;
			}
			if (ElementNode::eFound != element->GetAttrib(ATTRIB_MULTIQ_ITEM_LEVEL, item.m_Level)
			|| 0 == item.m_Level.length())
			{
				ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_MULTIQ_ITEM, ATTRIB_MULTIQ_ITEM_LEVEL));
				return false;
			}
			if (ElementNode::eFound != element->GetAttrib(ATTRIB_MULTIQ_ITEM_EVENT, item.m_Event)
			|| 0 == item.m_Event.length())
			{
				ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_MULTIQ_ITEM, ATTRIB_MULTIQ_ITEM_EVENT));
				return false;
			}

			// Now verify it.
			ARBConfigDivisionPtr pDiv;
			if (!inVenue.GetDivisions().FindDivision(item.m_Div, &pDiv))
			{
				std::wstring msg(Localization()->InvalidDivName());
				msg += item.m_Div;
				ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_MULTIQ_ITEM, ATTRIB_MULTIQ_ITEM_DIV, msg.c_str()));
				return false;
			}

			// Translate the sublevel to level.
			ARBConfigLevelPtr pLevel;
			if (!pDiv->GetLevels().FindSubLevel(item.m_Level, &pLevel))
			{
				std::wstring msg(Localization()->InvalidDivLevel());
				msg += item.m_Div;
				msg += L"/";
				msg += item.m_Level;
				ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_MULTIQ_ITEM, ATTRIB_MULTIQ_ITEM_LEVEL, msg.c_str()));
				return false;
			}
			pDiv.reset();
			std::wstring level = pLevel->GetName();
			pLevel.reset();

			// Now we can verify the event.
			if (!inVenue.GetEvents().VerifyEvent(item.m_Event, item.m_Div, level, ARBDate()))
			{
				std::wstring msg(Localization()->InvalidEventName());
				msg += item.m_Div;
				msg += L"/";
				msg += item.m_Level;
				msg += L"/";
				msg += item.m_Event;
				ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_MULTIQ_ITEM, ATTRIB_MULTIQ_ITEM_EVENT, msg.c_str()));
				return false;
			}

			m_Items.insert(item);
		}
	}

	return true;
}


bool ARBConfigMultiQ::Save(ElementNodePtr const& ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr multiQ = ioTree->AddElementNode(TREE_MULTIQ);
	multiQ->AddAttrib(ATTRIB_MULTIQ_NAME, m_Name);
	multiQ->AddAttrib(ATTRIB_MULTIQ_SHORTNAME, m_ShortName);
	if (m_ValidFrom.IsValid())
		multiQ->AddAttrib(ATTRIB_MULTIQ_VALID_FROM, m_ValidFrom);
	if (m_ValidTo.IsValid())
		multiQ->AddAttrib(ATTRIB_MULTIQ_VALID_TO, m_ValidTo);
	for (std::set<MultiQItem>::const_iterator iter = m_Items.begin(); iter != m_Items.end(); ++iter)
	{
		ElementNodePtr item = multiQ->AddElementNode(TREE_MULTIQ_ITEM);
		item->AddAttrib(ATTRIB_MULTIQ_ITEM_DIV, (*iter).m_Div);
		item->AddAttrib(ATTRIB_MULTIQ_ITEM_LEVEL, (*iter).m_Level);
		item->AddAttrib(ATTRIB_MULTIQ_ITEM_EVENT, (*iter).m_Event);
	}
	return true;
}


// Note, this is only called from ARBDogTrial
bool ARBConfigMultiQ::Match(
		std::vector<ARBDogRunPtr> const& inRuns,
		std::vector<ARBDogRunPtr>& outRuns) const
{
	outRuns.clear();
	if (inRuns.size() < m_Items.size())
		return false;
	// One assumption we are making is that a given run can only match one
	// multi-q definition.
	std::vector<bool> bItems;
	std::vector<ARBDogRunPtr> runs(inRuns);
	bItems.insert(bItems.begin(), m_Items.size(), false);
	for (std::vector<ARBDogRunPtr>::iterator iterR = runs.begin();
		iterR != runs.end();
		++iterR)
	{
		if (m_ValidFrom.IsValid() && (*iterR)->GetDate() < m_ValidFrom)
			continue;
		if (m_ValidTo.IsValid() && (*iterR)->GetDate() > m_ValidTo)
			continue;
		int idx = 0;
		for (std::set<MultiQItem>::const_iterator iter = m_Items.begin(); iter != m_Items.end(); ++idx, ++iter)
		{
			if ((*iter).m_Div == (*iterR)->GetDivision()
			&& (*iter).m_Level == (*iterR)->GetLevel()
			&& (*iter).m_Event == (*iterR)->GetEvent())
			{
				bItems[idx] = true;
			}
		}
	}
	size_t nMatch = 0;
	for (std::vector<bool>::iterator iterB = bItems.begin(); iterB != bItems.end(); ++iterB)
		if (*iterB)
			++nMatch;
	bool bOk = false;
	if (nMatch == m_Items.size())
	{
		bOk = true;
		for (std::vector<ARBDogRunPtr>::iterator iterR = runs.begin();
			iterR != runs.end();
			)
		{
			bool bInc = true;
			if (m_ValidFrom.IsValid() && (*iterR)->GetDate() < m_ValidFrom)
				continue;
			if (m_ValidTo.IsValid() && (*iterR)->GetDate() > m_ValidTo)
				continue;
			int idx = 0;
			for (std::set<MultiQItem>::const_iterator iter = m_Items.begin(); iter != m_Items.end(); ++idx, ++iter)
			{
				if ((*iter).m_Div == (*iterR)->GetDivision()
				&& (*iter).m_Level == (*iterR)->GetLevel()
				&& (*iter).m_Event == (*iterR)->GetEvent())
				{
					bInc = false;
					outRuns.push_back(*iterR);
					iterR = runs.erase(iterR);
					break;
				}
			}
			if (bInc)
				++iterR;
		}
	}
	return bOk;
}


int ARBConfigMultiQ::RenameDivision(
		std::wstring const& inOldDiv,
		std::wstring const& inNewDiv)
{
	if (inOldDiv == inNewDiv)
		return 0;
	int count = 0;
	for (std::set<MultiQItem>::iterator iter = m_Items.begin(); iter != m_Items.end(); )
	{
		if ((*iter).m_Div == inOldDiv)
		{
			MultiQItem item = *iter;
			item.m_Div = inNewDiv;
			m_Items.erase(iter);
			m_Items.insert(item);
			iter = m_Items.begin();
			++count;
		}
		else
			++iter;
	}
	return count;
}


int ARBConfigMultiQ::DeleteDivision(std::wstring const& inDiv)
{
	int count = 0;
	for (std::set<MultiQItem>::iterator iter = m_Items.begin(); iter != m_Items.end(); )
	{
		if ((*iter).m_Div == inDiv)
		{
			++count;
#ifdef ARB_SET_ERASE_RETURNS_ITERATOR
			iter = m_Items.erase(iter);
#else
			m_Items.erase(iter++);
#endif
		}
		else
			++iter;
	}
	return count;
}


int ARBConfigMultiQ::RenameLevel(
		std::wstring const& inDiv,
		std::wstring const& inOldLevel,
		std::wstring const& inNewLevel)
{
	if (inOldLevel == inNewLevel)
		return 0;
	int count = 0;
	for (std::set<MultiQItem>::iterator iter = m_Items.begin(); iter != m_Items.end(); ++iter)
	{
		if ((*iter).m_Div == inDiv
		&& (*iter).m_Level == inOldLevel)
		{
			MultiQItem item = *iter;
			item.m_Level = inNewLevel;
			m_Items.erase(iter);
			m_Items.insert(item);
			iter = m_Items.begin();
			++count;
		}
	}
	return count;
}


int ARBConfigMultiQ::DeleteLevel(std::wstring const& inLevel)
{
	int count = 0;
	for (std::set<MultiQItem>::iterator iter = m_Items.begin(); iter != m_Items.end(); )
	{
		if ((*iter).m_Level == inLevel)
		{
			++count;
#ifdef ARB_SET_ERASE_RETURNS_ITERATOR
			iter = m_Items.erase(iter);
#else
			m_Items.erase(iter++);
#endif
		}
		else
			++iter;
	}
	return count;
}


int ARBConfigMultiQ::RenameEvent(
		std::wstring const& inOldEvent,
		std::wstring const& inNewEvent)
{
	if (inOldEvent == inNewEvent)
		return 0;
	int count = 0;
	for (std::set<MultiQItem>::iterator iter = m_Items.begin(); iter != m_Items.end(); ++iter)
	{
		if ((*iter).m_Event == inOldEvent)
		{
			MultiQItem item = *iter;
			item.m_Event = inNewEvent;
			m_Items.erase(iter);
			m_Items.insert(item);
			iter = m_Items.begin();
			++count;
		}
	}
	return count;
}


int ARBConfigMultiQ::DeleteEvent(std::wstring const& inEvent)
{
	int count = 0;
	for (std::set<MultiQItem>::iterator iter = m_Items.begin(); iter != m_Items.end(); )
	{
		if ((*iter).m_Event == inEvent)
		{
			++count;
#ifdef ARB_SET_ERASE_RETURNS_ITERATOR
			iter = m_Items.erase(iter);
#else
			m_Items.erase(iter++);
#endif
		}
		else
			++iter;
	}
	return count;
}


bool ARBConfigMultiQ::AddItem(
		std::wstring const& inDiv,
		std::wstring const& inLevel,
		std::wstring const& inEvent)
{
	bool bInserted = false;
	if (0 < inDiv.length() && 0 < inLevel.length() && 0 < inEvent.length())
	{
		MultiQItem item;
		item.m_Div = inDiv;
		item.m_Level = inLevel;
		item.m_Event = inEvent;
		bInserted = m_Items.insert(item).second;
	}
	return bInserted;
}


bool ARBConfigMultiQ::RemoveItem(
		std::wstring const& inDiv,
		std::wstring const& inLevel,
		std::wstring const& inEvent)
{
	bool bRemoved = false;
	if (0 < inDiv.length() && 0 < inLevel.length() && 0 < inEvent.length())
	{
		MultiQItem item;
		item.m_Div = inDiv;
		item.m_Level = inLevel;
		item.m_Event = inEvent;
		std::set<MultiQItem>::iterator iter = std::find(m_Items.begin(), m_Items.end(), item);
		if (iter != m_Items.end())
		{
			bRemoved = true;
			m_Items.erase(iter);
		}
	}
	return bRemoved;
}


bool ARBConfigMultiQ::RemoveAllItems()
{
	if (0 < m_Items.size())
	{
		m_Items.clear();
		return true;
	}
	else
		return false;
}


bool ARBConfigMultiQ::GetItem(
		size_t inIndex,
		std::wstring& outDivision,
		std::wstring& outLevel,
		std::wstring& outEvent) const
{
	if (inIndex >= m_Items.size())
		return false;
	std::set<MultiQItem>::const_iterator iter = m_Items.begin();
	size_t n = 0;
	for (; n < inIndex && iter != m_Items.end(); ++n, ++iter)
		;
	if (iter == m_Items.end())
		return false;
	outDivision = (*iter).m_Div;
	outLevel = (*iter).m_Level;
	outEvent = (*iter).m_Event;
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigMultiQList::Load(
		ARBConfigVenue const& inVenue,
		ElementNodePtr const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigMultiQPtr thing(ARBConfigMultiQ::New());
	if (!thing->Load(inVenue, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


bool ARBConfigMultiQList::FindMultiQ(
		std::wstring const& inName,
		bool inUseShortName,
		ARBConfigMultiQPtr* outMultiQ) const
{
	if (outMultiQ)
		outMultiQ->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((!inUseShortName && *iter && (*iter)->GetName() == inName)
		|| (inUseShortName && *iter && (*iter)->GetShortName() == inName))
		{
			if (outMultiQ)
				*outMultiQ = *iter;
			return true;
		}
	}
	return false;
}


bool ARBConfigMultiQList::FindMultiQ(
	ARBConfigMultiQ const& inMultiQ,
	ARBConfigMultiQPtr* outMultiQ) const
{
	if (outMultiQ)
		outMultiQ->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (*iter && *(*iter) == inMultiQ)
		{
			if (outMultiQ)
				*outMultiQ = *iter;
			return true;
		}
	}
	return false;
}


int ARBConfigMultiQList::RenameDivision(
		std::wstring const& inOldDiv,
		std::wstring const& inNewDiv)
{
	if (inOldDiv == inNewDiv)
		return 0;
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->RenameDivision(inOldDiv, inNewDiv);
	}
	return count;
}


int ARBConfigMultiQList::DeleteDivision(std::wstring const& inDiv)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->DeleteDivision(inDiv);
	}
	return count;
}


int ARBConfigMultiQList::RenameLevel(
		std::wstring const& inDiv,
		std::wstring const& inOldLevel,
		std::wstring const& inNewLevel)
{
	if (inOldLevel == inNewLevel)
		return 0;
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->RenameLevel(inDiv, inOldLevel, inNewLevel);
	}
	return count;
}


int ARBConfigMultiQList::DeleteLevel(std::wstring const& inLevel)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->DeleteLevel(inLevel);
	}
	return count;
}


int ARBConfigMultiQList::RenameEvent(
		std::wstring const& inOldEvent,
		std::wstring const& inNewEvent)
{
	if (inOldEvent == inNewEvent)
		return 0;
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->RenameEvent(inOldEvent, inNewEvent);
	}
	return count;
}


int ARBConfigMultiQList::DeleteEvent(std::wstring const& inEvent)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->DeleteEvent(inEvent);
	}
	return count;
}


bool ARBConfigMultiQList::AddMultiQ(ARBConfigMultiQPtr const& inMultiQ)
{
	if (!inMultiQ
	|| FindMultiQ(*inMultiQ))
		return false;
	push_back(inMultiQ);
	return true;
}


bool ARBConfigMultiQList::DeleteMultiQ(ARBConfigMultiQPtr const& inMultiQ)
{
	if (inMultiQ)
	{
		for (iterator iter = begin(); iter != end(); ++iter)
		{
			if (*(*iter) == *inMultiQ)
			{
				erase(iter);
				return true;
			}
		}
	}
	return false;
}
