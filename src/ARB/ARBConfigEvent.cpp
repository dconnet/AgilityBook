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
 * @brief The classes that make up the configuration information.
 * @author David Connet
 *
 * Revision History
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
	, m_bHasPartner(false)
	, m_Scoring()
{
}

ARBConfigEvent::ARBConfigEvent(const ARBConfigEvent& rhs)
	: m_Name(rhs.m_Name)
	, m_Desc(rhs.m_Desc)
	, m_bHasPartner(rhs.m_bHasPartner)
	, m_Scoring(rhs.m_Scoring)
{
}

ARBConfigEvent::~ARBConfigEvent()
{
}

ARBConfigEvent& ARBConfigEvent::operator=(const ARBConfigEvent& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Desc = rhs.m_Desc;
		m_bHasPartner = rhs.m_bHasPartner;
		m_Scoring = rhs.m_Scoring;
	}
	return *this;
}

bool ARBConfigEvent::operator==(const ARBConfigEvent& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Desc == rhs.m_Desc
		&& m_bHasPartner == rhs.m_bHasPartner
		&& m_Scoring == rhs.m_Scoring;
}

bool ARBConfigEvent::operator!=(const ARBConfigEvent& rhs) const
{
	return !operator==(rhs);
}

size_t ARBConfigEvent::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigEvent::Load(
	const ARBConfigDivisionList& inDivisions,
	const CElement& inTree,
	const ARBVersion& inVersion,
	std::string& ioErrMsg)
{
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_EVENT_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioErrMsg += ErrorMissingAttribute(TREE_EVENT, ATTRIB_EVENT_NAME);
		return false;
	}
	if (CElement::eInvalidValue == inTree.GetAttrib(ATTRIB_EVENT_HASPARTNER, m_bHasPartner))
	{
		ioErrMsg += ErrorInvalidAttributeValue(TREE_EVENT, ATTRIB_EVENT_HASPARTNER, VALID_VALUES_BOOL);
		return false;
	}

	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		const CElement& element = inTree.GetElement(i);
		if (element.GetName() == TREE_EVENT_DESC)
		{
			m_Desc = element.GetValue();
		}
		else if (element.GetName() == TREE_SCORING)
		{
			// Ignore any errors...
			m_Scoring.Load(inDivisions, element, inVersion, ioErrMsg);
		}
	}
	return true;
}

bool ARBConfigEvent::Save(CElement& ioTree) const
{
	CElement& event = ioTree.AddElement(TREE_EVENT);
	event.AddAttrib(ATTRIB_EVENT_NAME, m_Name);
	if (0 < m_Desc.length())
	{
		CElement& desc = event.AddElement(TREE_EVENT_DESC);
		desc.SetValue(m_Desc);
	}
	// No need to write if not set.
	if (m_bHasPartner)
		event.AddAttrib(ATTRIB_EVENT_HASPARTNER, m_bHasPartner);
	if (!m_Scoring.Save(event))
		return false;
	return true;
}

std::string ARBConfigEvent::Update(int indent, const ARBConfigEvent* inEventNew)
{
	std::string info;

	std::string indentBuffer, indentName;
	for (int i = 0; i < indent-1; ++i)
		indentName += "   ";
	indentBuffer = indentName + "   ";
	indentName += "-";

	if (GetDesc() != inEventNew->GetDesc())
		SetDesc(inEventNew->GetDesc());
	if (HasPartner() != inEventNew->HasPartner())
		SetHasPartner(inEventNew->HasPartner());
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
		GetScorings() = inEventNew->GetScorings();
		char buffer[1000];
		sprintf(buffer, UPDATE_FORMAT_RULES, nAdded, nDeleted, nChanged, nSkipped);
		info += indentBuffer;
		info += GetName();
		info += buffer;
	}
	return info;
}

size_t ARBConfigEvent::FindAllEvents(
	const std::string& inDivision,
	const std::string& inLevel,
	bool inTitlePoints,
	std::vector<const ARBConfigScoring*>& outList) const
{
	return m_Scoring.FindAllEvents(inDivision, inLevel, inTitlePoints, outList);
}

const ARBConfigScoring* ARBConfigEvent::FindEvent(
	const std::string& inDivision,
	const std::string& inLevel,
	const ARBDate& inDate) const
{
	return m_Scoring.FindEvent(inDivision, inLevel, inDate);
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigEventList::Load(
	const ARBConfigDivisionList& inDivisions,
	const CElement& inTree,
	const ARBVersion& inVersion,
	std::string& ioErrMsg)
{
	ARBConfigEvent* thing = new ARBConfigEvent();
	if (!thing->Load(inDivisions, inTree, inVersion, ioErrMsg))
	{
		thing->Release();
		return false;
	}
	push_back(thing);
	return true;
}

const ARBConfigScoring* ARBConfigEventList::FindEvent(
	const std::string& inEvent,
	const std::string& inDivision,
	const std::string& inLevel,
	const ARBDate& inDate) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inEvent)
			return (*iter)->FindEvent(inDivision, inLevel, inDate);
	}
	return NULL;
}

int ARBConfigEventList::RenameDivision(const std::string& inOldDiv, const std::string& inNewDiv)
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

int ARBConfigEventList::DeleteDivision(const std::string& inDiv)
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

int ARBConfigEventList::RenameLevel(const std::string& inOldLevel, const std::string& inNewLevel)
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

int ARBConfigEventList::DeleteLevel(const std::string& inLevel)
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

const ARBConfigEvent* ARBConfigEventList::FindEvent(const std::string& inEvent) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inEvent)
			return (*iter);
	}
	return NULL;
}

ARBConfigEvent* ARBConfigEventList::FindEvent(const std::string& inEvent)
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

bool ARBConfigEventList::DeleteEvent(const std::string& inEvent)
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
