/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-03-26 DRC Added 'hasTable'. Used to set default when creating a run.
 *                    Update didn't save desc changes if nothing else changed.
 * @li 2004-02-02 DRC Added VerifyEvent.
 * @li 2004-01-27 DRC Updating could cause some false-positive messages because
 *                    the ordering was different.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-09-16 DRC Fixed a bug in Update with Scoring.
 * @li 2003-07-16 DRC Allow the code to keep processing after errors are found.
 */

#include "StdAfx.h"
#include "ARBConfigEvent.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigEvent::ARBConfigEvent()
	: m_Name()
	, m_Desc()
	, m_bTable(false)
	, m_bHasPartner(false)
	, m_Scoring()
{
}

ARBConfigEvent::ARBConfigEvent(ARBConfigEvent const& rhs)
	: m_Name(rhs.m_Name)
	, m_Desc(rhs.m_Desc)
	, m_bTable(rhs.m_bTable)
	, m_bHasPartner(rhs.m_bHasPartner)
	, m_Scoring(rhs.m_Scoring)
{
}

ARBConfigEvent::~ARBConfigEvent()
{
}

ARBConfigEvent& ARBConfigEvent::operator=(ARBConfigEvent const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Desc = rhs.m_Desc;
		m_bTable = rhs.m_bTable;
		m_bHasPartner = rhs.m_bHasPartner;
		m_Scoring = rhs.m_Scoring;
	}
	return *this;
}

bool ARBConfigEvent::operator==(ARBConfigEvent const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Desc == rhs.m_Desc
		&& m_bTable == rhs.m_bTable
		&& m_bHasPartner == rhs.m_bHasPartner
		&& m_Scoring == rhs.m_Scoring;
}

bool ARBConfigEvent::operator!=(ARBConfigEvent const& rhs) const
{
	return !operator==(rhs);
}

size_t ARBConfigEvent::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigEvent::Load(
	ARBConfigDivisionList const& inDivisions,
	Element const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	if (Element::eFound != inTree.GetAttrib(ATTRIB_EVENT_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_EVENT, ATTRIB_EVENT_NAME));
		return false;
	}

	// Introduced in file version 8.6.
	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_EVENT_HAS_TABLE, m_bTable))
	{
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_EVENT, ATTRIB_EVENT_HAS_TABLE, VALID_VALUES_BOOL));
		return false;
	}

	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_EVENT_HASPARTNER, m_bHasPartner))
	{
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_EVENT, ATTRIB_EVENT_HASPARTNER, VALID_VALUES_BOOL));
		return false;
	}

	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		Element const& element = inTree.GetElement(i);
		if (element.GetName() == TREE_EVENT_DESC)
		{
			m_Desc = element.GetValue();
		}
		else if (element.GetName() == TREE_SCORING)
		{
			// Ignore any errors...
			m_Scoring.Load(inDivisions, element, inVersion, ioCallback);
		}
	}
	return true;
}

bool ARBConfigEvent::Save(Element& ioTree) const
{
	Element& event = ioTree.AddElement(TREE_EVENT);
	event.AddAttrib(ATTRIB_EVENT_NAME, m_Name);
	if (0 < m_Desc.length())
	{
		Element& desc = event.AddElement(TREE_EVENT_DESC);
		desc.SetValue(m_Desc);
	}
	// No need to write if not set.
	if (m_bTable)
		event.AddAttrib(ATTRIB_EVENT_HAS_TABLE, m_bTable);
	if (m_bHasPartner)
		event.AddAttrib(ATTRIB_EVENT_HASPARTNER, m_bHasPartner);
	if (!m_Scoring.Save(event))
		return false;
	return true;
}

bool ARBConfigEvent::Update(int indent, ARBConfigEvent const* inEventNew, std::string& ioInfo)
{
	std::string info;

	std::string indentBuffer, indentName;
	for (int i = 0; i < indent-1; ++i)
		indentName += "   ";
	indentBuffer = indentName + "   ";
	indentName += "-";

	bool bChanges = false;
	if (GetDesc() != inEventNew->GetDesc())
	{
		bChanges = true;
		SetDesc(inEventNew->GetDesc());
	}
	if (HasPartner() != inEventNew->HasPartner())
	{
		bChanges = true;
		SetHasPartner(inEventNew->HasPartner());
	}
	if (HasTable() != inEventNew->HasTable())
	{
		bChanges = true;
		SetHasTable(inEventNew->HasTable());
	}
	// If the order is different, we will fall into this...
	if (GetScorings() != inEventNew->GetScorings())
	{
		int nAdded, nDeleted, nChanged, nSkipped;
		nAdded = nDeleted = nChanged = nSkipped = 0;
		ARBConfigScoringList::iterator iter1;
		ARBConfigScoringList::const_iterator iter2;
		// Look for changed items and items that will be removed.
		for (iter1 = GetScorings().begin(); iter1 != GetScorings().end(); ++iter1)
		{
			bool bFound = false;
			for (iter2 = inEventNew->GetScorings().begin(); iter2 != inEventNew->GetScorings().end(); ++iter2)
			{
				if ((*iter1)->GetDivision() == (*iter2)->GetDivision()
				&& (*iter1)->GetLevel() == (*iter2)->GetLevel())
				{
					bFound = true;
					if (*(*iter1) == *(*iter2))
						++nSkipped;
					else
						++nChanged;
					break;
				}
			}
			if (!bFound)
				++nDeleted;
		}
		// Now look for items that will be added.
		for (iter2 = inEventNew->GetScorings().begin(); iter2 != inEventNew->GetScorings().end(); ++iter2)
		{
			bool bFound = false;
			for (iter1 = GetScorings().begin(); iter1 != GetScorings().end(); ++iter1)
			{
				if ((*iter1)->GetDivision() == (*iter2)->GetDivision()
				&& (*iter1)->GetLevel() == (*iter2)->GetLevel())
				{
					bFound = true;
					break;
				}
			}
			if (!bFound)
				++nAdded;
		}
		// TODO: This can actually invalidate existing runs
		GetScorings() = inEventNew->GetScorings();
		// ... so only generate a message if we added or changed.
		if (0 < nAdded || 0 < nDeleted || 0 < nChanged)
		{
			char buffer[1000];
			sprintf(buffer, UPDATE_FORMAT_RULES, nAdded, nDeleted, nChanged, nSkipped);
			info += indentBuffer;
			info += GetName();
			info += buffer;
		}
	}
	if (0 < info.length())
	{
		bChanges = true;
		ioInfo += info;
	}
	return bChanges;
}

size_t ARBConfigEvent::FindAllEvents(
	std::string const& inDivision,
	std::string const& inLevel,
	bool inTitlePoints,
	std::vector<ARBConfigScoring const*>& outList) const
{
	return m_Scoring.FindAllEvents(inDivision, inLevel, inTitlePoints, outList);
}

bool ARBConfigEvent::VerifyEvent(
	std::string const& inDivision,
	std::string const& inLevel) const
{
	return m_Scoring.VerifyEvent(inDivision, inLevel);
}

ARBConfigScoring const* ARBConfigEvent::FindEvent(
	std::string const& inDivision,
	std::string const& inLevel,
	ARBDate const& inDate) const
{
	return m_Scoring.FindEvent(inDivision, inLevel, inDate);
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigEventList::Load(
	ARBConfigDivisionList const& inDivisions,
	Element const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	ARBConfigEvent* thing = new ARBConfigEvent();
	if (!thing->Load(inDivisions, inTree, inVersion, ioCallback))
	{
		thing->Release();
		return false;
	}
	push_back(thing);
	return true;
}

bool ARBConfigEventList::VerifyEvent(
	std::string const& inEvent,
	std::string const& inDivision,
	std::string const& inLevel) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inEvent)
			return (*iter)->VerifyEvent(inDivision, inLevel);
	}
	return false;
}

ARBConfigScoring const* ARBConfigEventList::FindEvent(
	std::string const& inEvent,
	std::string const& inDivision,
	std::string const& inLevel,
	ARBDate const& inDate) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inEvent)
			return (*iter)->FindEvent(inDivision, inLevel, inDate);
	}
	return NULL;
}

int ARBConfigEventList::RenameDivision(std::string const& inOldDiv, std::string const& inNewDiv)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		for (ARBConfigScoringList::iterator iterScore = (*iter)->GetScorings().begin();
			iterScore != (*iter)->GetScorings().end(); ++iterScore)
		{
			if ((*iterScore)->GetDivision() == inOldDiv)
			{
				++count;
				(*iterScore)->SetDivision(inNewDiv);
			}
		}
	}
	return count;
}

int ARBConfigEventList::DeleteDivision(std::string const& inDiv)
{
	std::string div(inDiv);
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		for (ARBConfigScoringList::iterator iterScore = (*iter)->GetScorings().begin();
			iterScore != (*iter)->GetScorings().end(); )
		{
			if ((*iterScore)->GetDivision() == div)
			{
				++count;
				iterScore = (*iter)->GetScorings().erase(iterScore);
			}
			else
				++iterScore;
		}
	}
	return count;
}

int ARBConfigEventList::RenameLevel(std::string const& inOldLevel, std::string const& inNewLevel)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		for (ARBConfigScoringList::iterator iterScore = (*iter)->GetScorings().begin();
			iterScore != (*iter)->GetScorings().end(); ++iterScore)
		{
			if ((*iterScore)->GetLevel() == inOldLevel)
			{
				++count;
				(*iterScore)->SetLevel(inNewLevel);
			}
		}
	}
	return count;
}

int ARBConfigEventList::DeleteLevel(std::string const& inLevel)
{
	std::string level(inLevel);
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		for (ARBConfigScoringList::iterator iterScore = (*iter)->GetScorings().begin();
			iterScore != (*iter)->GetScorings().end(); )
		{
			if ((*iterScore)->GetLevel() == level)
			{
				++count;
				iterScore = (*iter)->GetScorings().erase(iterScore);
			}
			else
				++iterScore;
		}
	}
	return count;
}

ARBConfigEvent const* ARBConfigEventList::FindEvent(std::string const& inEvent) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inEvent)
			return (*iter);
	}
	return NULL;
}

ARBConfigEvent* ARBConfigEventList::FindEvent(std::string const& inEvent)
{
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inEvent)
			return (*iter);
	}
	return NULL;
}

ARBConfigEvent* ARBConfigEventList::AddEvent(ARBConfigEvent* inEvent)
{
	if (!inEvent
	|| 0 == inEvent->GetName().length()
	|| FindEvent(inEvent->GetName()))
		return NULL;
	inEvent->AddRef();
	push_back(inEvent);
	return inEvent;
}

bool ARBConfigEventList::DeleteEvent(std::string const& inEvent)
{
	std::string event(inEvent);
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == event)
		{
			erase(iter);
			return true;
		}
	}
	return false;
}
