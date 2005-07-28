/*
 * Copyright © 2005 David Connet. All Rights Reserved.
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
 * @brief The classes that make up the configuration information.
 * @author David Connet
 *
 * Revision History
 * @li 2005-07-15 DRC Created.
 */

#include "StdAfx.h"
#include "ARBConfigMultiQ.h"

#include <sstream>
#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

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

bool ARBConfigMultiQ::operator!=(ARBConfigMultiQ const& rhs) const
{
	return !operator==(rhs);
}

#if _MSC_VER >= 1300
// VC7's stream is trying to use the 'unsigned int' method. While
// technically this is ok, 64bit compatibility trips on it!
#pragma warning ( push )
#pragma warning ( disable : 4267)
#endif
std::string ARBConfigMultiQ::GetGenericName() const
{
	std::ostringstream str;
	str << m_Items.size() << 'Q';
	return str.str();
}
#if _MSC_VER >= 1300
#pragma warning ( pop )
#endif

size_t ARBConfigMultiQ::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigMultiQ::Load(
		ARBConfigVenue const& inVenue,
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (Element::eFound != inTree.GetAttrib(ATTRIB_MULTIQ_NAME, m_Name))
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_MULTIQ_ITEM, ATTRIB_MULTIQ_NAME));
		return false;
	}
	if (Element::eFound != inTree.GetAttrib(ATTRIB_MULTIQ_SHORTNAME, m_ShortName))
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_MULTIQ_ITEM, ATTRIB_MULTIQ_SHORTNAME));
		return false;
	}
	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_MULTIQ_VALID_FROM, m_ValidFrom))
	{
		std::string attrib;
		inTree.GetAttrib(ATTRIB_MULTIQ_VALID_FROM, attrib);
		std::string msg(INVALID_DATE);
		msg += attrib;
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_MULTIQ, ATTRIB_MULTIQ_VALID_FROM, msg.c_str()));
		return false;
	}
	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_MULTIQ_VALID_TO, m_ValidTo))
	{
		std::string attrib;
		inTree.GetAttrib(ATTRIB_MULTIQ_VALID_TO, attrib);
		std::string msg(INVALID_DATE);
		msg += attrib;
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_MULTIQ, ATTRIB_MULTIQ_VALID_TO, msg.c_str()));
		return false;
	}

	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		Element const& element = inTree.GetElement(i);
		if (element.GetName() == TREE_MULTIQ_ITEM)
		{
			MultiQItem item;
			// Read the data.
			if (Element::eFound != element.GetAttrib(ATTRIB_MULTIQ_ITEM_DIV, item.m_Div)
			|| 0 == item.m_Div.length())
			{
				ioCallback.LogMessage(ErrorMissingAttribute(TREE_MULTIQ_ITEM, ATTRIB_MULTIQ_ITEM_DIV));
				return false;
			}
			if (Element::eFound != element.GetAttrib(ATTRIB_MULTIQ_ITEM_LEVEL, item.m_Level)
			|| 0 == item.m_Level.length())
			{
				ioCallback.LogMessage(ErrorMissingAttribute(TREE_MULTIQ_ITEM, ATTRIB_MULTIQ_ITEM_LEVEL));
				return false;
			}
			if (Element::eFound != element.GetAttrib(ATTRIB_MULTIQ_ITEM_EVENT, item.m_Event)
			|| 0 == item.m_Event.length())
			{
				ioCallback.LogMessage(ErrorMissingAttribute(TREE_MULTIQ_ITEM, ATTRIB_MULTIQ_ITEM_EVENT));
				return false;
			}

			// Now verify it.
			ARBConfigDivision* pDiv;
			if (!inVenue.GetDivisions().FindDivision(item.m_Div, &pDiv))
			{
				std::string msg(INVALID_DIV_NAME);
				msg += item.m_Div;
				ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_MULTIQ_ITEM, ATTRIB_MULTIQ_ITEM_DIV, msg.c_str()));
				return false;
			}

			// Translate the sublevel to level.
			ARBConfigLevel* pLevel;
			if (!pDiv->GetLevels().FindSubLevel(item.m_Level, &pLevel))
			{
				std::string msg(INVALID_DIV_LEVEL);
				msg += item.m_Div;
				msg += "/";
				msg += item.m_Level;
				ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_MULTIQ_ITEM, ATTRIB_MULTIQ_ITEM_LEVEL, msg.c_str()));
				pDiv->Release();
				return false;
			}
			pDiv->Release();
			pDiv = NULL;
			std::string level = pLevel->GetName();
			pLevel->Release();
			pLevel = NULL;

			// Now we can verify the event.
			if (!inVenue.GetEvents().VerifyEvent(item.m_Event, item.m_Div, level))
			{
				std::string msg(INVALID_EVENT_NAME);
				msg += item.m_Div;
				msg += "/";
				msg += item.m_Level;
				msg += "/";
				msg += item.m_Event;
				ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_MULTIQ_ITEM, ATTRIB_MULTIQ_ITEM_EVENT, msg.c_str()));
				return false;
			}

			m_Items.insert(item);
		}
	}

	return true;
}

bool ARBConfigMultiQ::Save(Element& ioTree) const
{
	Element& multiQ = ioTree.AddElement(TREE_MULTIQ);
	multiQ.AddAttrib(ATTRIB_MULTIQ_NAME, m_Name);
	multiQ.AddAttrib(ATTRIB_MULTIQ_SHORTNAME, m_ShortName);
	if (m_ValidFrom.IsValid())
		multiQ.AddAttrib(ATTRIB_MULTIQ_VALID_FROM, m_ValidFrom);
	if (m_ValidTo.IsValid())
		multiQ.AddAttrib(ATTRIB_MULTIQ_VALID_TO, m_ValidTo);
	for (std::set<MultiQItem>::const_iterator iter = m_Items.begin(); iter != m_Items.end(); ++iter)
	{
		Element& item = multiQ.AddElement(TREE_MULTIQ_ITEM);
		item.AddAttrib(ATTRIB_MULTIQ_ITEM_DIV, (*iter).m_Div);
		item.AddAttrib(ATTRIB_MULTIQ_ITEM_LEVEL, (*iter).m_Level);
		item.AddAttrib(ATTRIB_MULTIQ_ITEM_EVENT, (*iter).m_Event);
	}
	return true;
}

bool ARBConfigMultiQ::Match(ARBVectorBase<ARBDogRun> const& inRuns) const
{
	if (inRuns.size() < m_Items.size())
		return false;
	// One assumption we are making is that a given run can only match one
	// multi-q definition.
	std::vector<bool> bItems;
	bItems.insert(bItems.begin(), m_Items.size(), false);
	for (ARBVectorBase<ARBDogRun>::const_iterator iterR = inRuns.begin();
		iterR != inRuns.end();
		++iterR)
	{
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
	return nMatch == m_Items.size();
}

int ARBConfigMultiQ::RenameDivision(
		std::string const& inOldDiv,
		std::string const& inNewDiv)
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

int ARBConfigMultiQ::DeleteDivision(std::string const& inDiv)
{
	int count = 0;
	for (std::set<MultiQItem>::iterator iter = m_Items.begin(); iter != m_Items.end(); ++iter)
	{
		if ((*iter).m_Div == inDiv)
		{
			++count;
			iter = m_Items.erase(iter);
		}
		else
			++iter;
	}
	return count;
}

int ARBConfigMultiQ::RenameLevel(
		std::string const& inDiv,
		std::string const& inOldLevel,
		std::string const& inNewLevel)
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

int ARBConfigMultiQ::DeleteLevel(std::string const& inLevel)
{
	int count = 0;
	for (std::set<MultiQItem>::iterator iter = m_Items.begin(); iter != m_Items.end(); ++iter)
	{
		if ((*iter).m_Level == inLevel)
		{
			++count;
			iter = m_Items.erase(iter);
		}
		else
			++iter;
	}
	return count;
}

int ARBConfigMultiQ::RenameEvent(
		std::string const& inOldEvent,
		std::string const& inNewEvent)
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

int ARBConfigMultiQ::DeleteEvent(std::string const& inEvent)
{
	int count = 0;
	for (std::set<MultiQItem>::iterator iter = m_Items.begin(); iter != m_Items.end(); ++iter)
	{
		if ((*iter).m_Event == inEvent)
		{
			++count;
			iter = m_Items.erase(iter);
		}
		else
			++iter;
	}
	return count;
}

bool ARBConfigMultiQ::AddEvent(
		std::string const& inDiv,
		std::string const& inLevel,
		std::string const& inEvent)
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

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigMultiQList::Load(
		ARBConfigVenue const& inVenue,
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigMultiQ* thing = new ARBConfigMultiQ();
	if (!thing->Load(inVenue, inTree, inVersion, ioCallback))
	{
		thing->Release();
		return false;
	}
	push_back(thing);
	return true;
}

bool ARBConfigMultiQList::FindMultiQ(
	ARBConfigMultiQ const& inMultiQ,
	ARBConfigMultiQ** outMultiQ)
{
	if (outMultiQ)
		*outMultiQ = NULL;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if (*iter && *(*iter) == inMultiQ)
		{
			if (outMultiQ)
			{
				*outMultiQ = *iter;
				(*outMultiQ)->AddRef();
			}
			return true;
		}
	}
	return false;
}

int ARBConfigMultiQList::RenameDivision(
		std::string const& inOldDiv,
		std::string const& inNewDiv)
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

int ARBConfigMultiQList::DeleteDivision(std::string const& inDiv)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->DeleteDivision(inDiv);
	}
	return count;
}

int ARBConfigMultiQList::RenameLevel(
		std::string const& inDiv,
		std::string const& inOldLevel,
		std::string const& inNewLevel)
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

int ARBConfigMultiQList::DeleteLevel(std::string const& inLevel)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->DeleteLevel(inLevel);
	}
	return count;
}

int ARBConfigMultiQList::RenameEvent(
		std::string const& inOldEvent,
		std::string const& inNewEvent)
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

int ARBConfigMultiQList::DeleteEvent(std::string const& inEvent)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->DeleteEvent(inEvent);
	}
	return count;
}

bool ARBConfigMultiQList::AddMultiQ(ARBConfigMultiQ* inMultiQ)
{
	if (!inMultiQ
	|| FindMultiQ(*inMultiQ))
		return false;
	inMultiQ->AddRef();
	push_back(inMultiQ);
	return true;
}

bool ARBConfigMultiQList::DeleteMultiQ(ARBConfigMultiQ* inMultiQ)
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
