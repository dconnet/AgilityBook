/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-01-02 DRC Added subnames to events.
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
#include "ARBLocalization.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigEventPtr ARBConfigEvent::New()
{
	return ARBConfigEventPtr(new ARBConfigEvent());
}


ARBConfigEvent::ARBConfigEvent()
	: m_Name()
	, m_Desc()
	, m_bTable(false)
	, m_bHasPartner(false)
	, m_bHasSubNames(false)
	, m_SubNames()
	, m_Scoring()
{
}


ARBConfigEvent::ARBConfigEvent(ARBConfigEvent const& rhs)
	: m_Name(rhs.m_Name)
	, m_Desc(rhs.m_Desc)
	, m_bTable(rhs.m_bTable)
	, m_bHasPartner(rhs.m_bHasPartner)
	, m_bHasSubNames(rhs.m_bHasSubNames)
	, m_SubNames(rhs.m_SubNames)
	, m_Scoring()
{
	rhs.m_Scoring.Clone(m_Scoring);
}


ARBConfigEvent::~ARBConfigEvent()
{
}


ARBConfigEventPtr ARBConfigEvent::Clone() const
{
	return ARBConfigEventPtr(new ARBConfigEvent(*this));
}


ARBConfigEvent& ARBConfigEvent::operator=(ARBConfigEvent const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Desc = rhs.m_Desc;
		m_bTable = rhs.m_bTable;
		m_bHasPartner = rhs.m_bHasPartner;
		m_bHasSubNames = rhs.m_bHasSubNames;
		m_SubNames = rhs.m_SubNames;
		rhs.m_Scoring.Clone(m_Scoring);
	}
	return *this;
}


bool ARBConfigEvent::operator==(ARBConfigEvent const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Desc == rhs.m_Desc
		&& m_bTable == rhs.m_bTable
		&& m_bHasPartner == rhs.m_bHasPartner
		&& m_bHasSubNames == rhs.m_bHasSubNames
		&& m_SubNames == rhs.m_SubNames
		&& m_Scoring == rhs.m_Scoring;
}


bool ARBConfigEvent::Load(
		ARBConfigDivisionList const& inDivisions,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ASSERT(inTree);
	if (!inTree || inTree->GetName() != TREE_EVENT)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_EVENT_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_EVENT, ATTRIB_EVENT_NAME));
		return false;
	}

	// Introduced in file version 8.6.
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_EVENT_HAS_TABLE, m_bTable))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_EVENT, ATTRIB_EVENT_HAS_TABLE, Localization()->ValidValuesBool().c_str()));
		return false;
	}

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_EVENT_HASPARTNER, m_bHasPartner))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_EVENT, ATTRIB_EVENT_HASPARTNER, Localization()->ValidValuesBool().c_str()));
		return false;
	}

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_EVENT_HASSUBNAMES, m_bHasSubNames))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_EVENT, ATTRIB_EVENT_HASPARTNER, Localization()->ValidValuesBool().c_str()));
		return false;
	}

	for (int i = 0; i < inTree->GetElementCount(); ++i)
	{
		ElementNodePtr element = inTree->GetElementNode(i);
		if (!element)
			continue;
		if (element->GetName() == TREE_EVENT_DESC)
		{
			m_Desc = element->GetValue();
		}
		else if (element->GetName() == TREE_EVENT_SUBNAME)
		{
			m_SubNames.insert(element->GetValue());
		}
		else if (element->GetName() == TREE_SCORING)
		{
			// Ignore any errors...
			m_Scoring.Load(inDivisions, element, inVersion, ioCallback);
		}
	}
	return true;
}


bool ARBConfigEvent::Save(ElementNodePtr ioTree) const
{
	ASSERT(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr evtTree = ioTree->AddElementNode(TREE_EVENT);
	evtTree->AddAttrib(ATTRIB_EVENT_NAME, m_Name);
	if (0 < m_Desc.length())
	{
		ElementNodePtr desc = evtTree->AddElementNode(TREE_EVENT_DESC);
		desc->SetValue(m_Desc);
	}
	// No need to write if not set.
	if (m_bTable)
		evtTree->AddAttrib(ATTRIB_EVENT_HAS_TABLE, m_bTable);
	if (m_bHasPartner)
		evtTree->AddAttrib(ATTRIB_EVENT_HASPARTNER, m_bHasPartner);
	if (m_bHasSubNames)
	{
		evtTree->AddAttrib(ATTRIB_EVENT_HASSUBNAMES, m_bHasSubNames);
		for (std::set<tstring>::const_iterator iter = m_SubNames.begin();
			iter != m_SubNames.end();
			++iter)
		{
			if (0 < (*iter).length())
			{
				ElementNodePtr name = evtTree->AddElementNode(TREE_EVENT_SUBNAME);
				name->SetValue(*iter);
			}
		}
	}
	if (!m_Scoring.Save(evtTree))
		return false;
	return true;
}


bool ARBConfigEvent::Update(
		int indent,
		ARBConfigEventPtr inEventNew,
		tstring& ioInfo)
{
	tstring info;

	tstring indentBuffer, indentName;
	for (int i = 0; i < indent-1; ++i)
		indentName += _T("   ");
	indentBuffer = indentName + _T("   ");
	indentName += _T("-");

	bool bChanges = false;
	if (GetDesc() != inEventNew->GetDesc())
	{
		bChanges = true;
		SetDesc(inEventNew->GetDesc());
	}
	if (HasTable() != inEventNew->HasTable())
	{
		bChanges = true;
		SetHasTable(inEventNew->HasTable());
	}
	if (HasPartner() != inEventNew->HasPartner())
	{
		bChanges = true;
		SetHasPartner(inEventNew->HasPartner());
	}

	if (HasSubNames() != inEventNew->HasSubNames())
	{
		bChanges = true;
		SetHasSubNames(inEventNew->HasSubNames());
	}
	std::set<tstring> oldNames, newNames;
	GetSubNames(oldNames);
	inEventNew->GetSubNames(newNames);
	if (oldNames != newNames)
	{
		bChanges = true;
		SetSubNames(newNames);
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
		inEventNew->GetScorings().Clone(GetScorings());
		if (0 < nAdded || 0 < nDeleted || 0 < nChanged)
		{
			info += indentBuffer;
			info += GetName();
			info += Localization()->UpdateRules(nAdded, nDeleted, nChanged, nSkipped);
			info += _T("\n");
		}
	}
	if (0 < info.length())
	{
		bChanges = true;
		ioInfo += info;
	}
	return bChanges;
}


size_t ARBConfigEvent::GetSubNames(std::set<tstring>& outNames) const
{
	outNames.clear();
	outNames = m_SubNames;
	return outNames.size();
}


void ARBConfigEvent::SetSubNames(std::set<tstring> const& inNames)
{
	m_SubNames.clear();
	m_SubNames = inNames;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigEventList::Load(
		ARBConfigDivisionList const& inDivisions,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigEventPtr thing(ARBConfigEvent::New());
	if (!thing->Load(inDivisions, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


void ARBConfigEventList::ReorderBy(ARBConfigEventList const& inList)
{
	if (*this != inList)
	{
		ASSERT(inList.size() == size());
		ARBConfigEventList tmp;
		tmp.reserve(size());
		for (ARBConfigEventList::const_iterator i = inList.begin();
			i != inList.end();
			++i)
		{
			ARBConfigEventPtr event;
			FindEvent((*i)->GetName(), &event);
			tmp.push_back(event);
		}
		std::swap(tmp, *this);
	}
}


bool ARBConfigEventList::VerifyEvent(
		tstring const& inEvent,
		tstring const& inDivision,
		tstring const& inLevel,
		ARBDate const& inDate) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inEvent)
			return (*iter)->VerifyEvent(inDivision, inLevel, inDate);
	}
	return false;
}


bool ARBConfigEventList::FindEvent(
		tstring const& inEvent,
		tstring const& inDivision,
		tstring const& inLevel,
		ARBDate const& inDate,
		ARBConfigEventPtr* outEvent,
		ARBConfigScoringPtr* outScoring) const
{
	if (outScoring)
		outScoring->reset();
	if (outEvent)
		outEvent->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inEvent)
		{
			bool bFound = (*iter)->FindEvent(inDivision, inLevel, inDate, outScoring);
			if (bFound && outEvent)
				*outEvent = *iter;
			return bFound;
		}
	}
	return false;
}


int ARBConfigEventList::RenameDivision(
		tstring const& inOldDiv,
		tstring const& inNewDiv)
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


int ARBConfigEventList::DeleteDivision(tstring const& inDiv)
{
	tstring div(inDiv);
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


int ARBConfigEventList::RenameLevel(
		tstring const& inOldDiv,
		tstring const& inOldLevel,
		tstring const& inNewLevel)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		for (ARBConfigScoringList::iterator iterScore = (*iter)->GetScorings().begin();
			iterScore != (*iter)->GetScorings().end(); ++iterScore)
		{
			if ((*iterScore)->GetLevel() == inOldLevel
			&& ((*iterScore)->GetDivision() == WILDCARD_DIVISION
			|| (*iterScore)->GetDivision() == inOldDiv))
			{
				++count;
				(*iterScore)->SetLevel(inNewLevel);
			}
		}
	}
	return count;
}


int ARBConfigEventList::DeleteLevel(
		tstring const& inDiv,
		tstring const& inLevel)
{
	tstring level(inLevel);
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		for (ARBConfigScoringList::iterator iterScore = (*iter)->GetScorings().begin();
			iterScore != (*iter)->GetScorings().end(); )
		{
			if ((*iterScore)->GetLevel() == level
			&& ((*iterScore)->GetDivision() == WILDCARD_DIVISION
			|| (*iterScore)->GetDivision() == inDiv))
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


bool ARBConfigEventList::FindEvent(
		tstring const& inEvent,
		ARBConfigEventPtr* outEvent) const
{
	if (outEvent)
		outEvent->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inEvent)
		{
			if (outEvent)
				*outEvent = *iter;
			return true;
		}
	}
	return false;
}


bool ARBConfigEventList::AddEvent(ARBConfigEventPtr inEvent)
{
	if (!inEvent
	|| 0 == inEvent->GetName().length()
	|| FindEvent(inEvent->GetName()))
		return false;
	push_back(inEvent);
	return true;
}


bool ARBConfigEventList::DeleteEvent(tstring const& inEvent)
{
	tstring pEvent(inEvent);
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == pEvent)
		{
			erase(iter);
			return true;
		}
	}
	return false;
}
