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
 * 2016-06-19 Add support for Lifetime names.
 * 2013-09-03 Added short name.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-01-02 Added subnames to events.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-03-26 Added 'hasTable'. Used to set default when creating a run.
 *            Update didn't save desc changes if nothing else changed.
 * 2004-02-02 Added VerifyEvent.
 * 2004-01-27 Updating could cause some false-positive messages because
 *            the ordering was different.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-12-27 Changed FindEvent to take a date.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-09-16 Fixed a bug in Update with Scoring.
 * 2003-07-16 Allow the code to keep processing after errors are found.
 */

#include "stdafx.h"
#include "ARB/ARBConfigEvent.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

class ARBConfigEvent_concrete : public ARBConfigEvent
{
public:
	ARBConfigEvent_concrete() {}
	ARBConfigEvent_concrete(ARBConfigEvent const& rhs)
		: ARBConfigEvent(rhs)
	{
	}
};


ARBConfigEventPtr ARBConfigEvent::New()
{
	return std::make_shared<ARBConfigEvent_concrete>();
}


ARBConfigEvent::ARBConfigEvent()
	: m_Name()
	, m_ShortName()
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
	, m_ShortName(rhs.m_ShortName)
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
	return std::make_shared<ARBConfigEvent_concrete>(*this);
}


ARBConfigEvent& ARBConfigEvent::operator=(ARBConfigEvent const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_ShortName = rhs.m_ShortName;
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
		&& m_ShortName == rhs.m_ShortName
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
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_EVENT)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_EVENT_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_EVENT, ATTRIB_EVENT_NAME));
		return false;
	}
	inTree->GetAttrib(ATTRIB_EVENT_SHORTNAME, m_ShortName);

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
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_EVENT, ATTRIB_EVENT_HASSUBNAMES, Localization()->ValidValuesBool().c_str()));
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
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr evtTree = ioTree->AddElementNode(TREE_EVENT);
	evtTree->AddAttrib(ATTRIB_EVENT_NAME, m_Name);
	if (0 < m_Desc.length())
	{
		ElementNodePtr desc = evtTree->AddElementNode(TREE_EVENT_DESC);
		desc->SetValue(m_Desc);
	}
	if (!m_ShortName.empty())
		evtTree->AddAttrib(ATTRIB_EVENT_SHORTNAME, m_ShortName);
	// No need to write if not set.
	if (m_bTable)
		evtTree->AddAttrib(ATTRIB_EVENT_HAS_TABLE, m_bTable);
	if (m_bHasPartner)
		evtTree->AddAttrib(ATTRIB_EVENT_HASPARTNER, m_bHasPartner);
	if (m_bHasSubNames)
	{
		evtTree->AddAttrib(ATTRIB_EVENT_HASSUBNAMES, m_bHasSubNames);
		for (std::set<std::wstring>::const_iterator iter = m_SubNames.begin();
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
		std::wstring& ioInfo)
{
	std::wstring info;

	std::wstring indentBuffer, indentName;
	for (int i = 0; i < indent-1; ++i)
		indentName += L"   ";
	indentBuffer = indentName + L"   ";
	indentName += L"-";

	bool bChanges = false;
	if (GetShortName() != inEventNew->GetShortName())
	{
		bChanges = true;
		SetShortName(inEventNew->GetShortName());
	}
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
	std::set<std::wstring> oldNames, newNames;
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
			info += L"\n";
		}
	}
	if (0 < info.length())
	{
		bChanges = true;
		ioInfo += info;
	}
	return bChanges;
}


size_t ARBConfigEvent::GetSubNames(std::set<std::wstring>& outNames) const
{
	outNames.clear();
	outNames = m_SubNames;
	return outNames.size();
}


void ARBConfigEvent::SetSubNames(std::set<std::wstring> const& inNames)
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
		ARBConfigEventList tmp;
		tmp.reserve(size());
		for (ARBConfigEventList::const_iterator i = inList.begin();
			i != inList.end();
			++i)
		{
			ARBConfigEventPtr evt;
			if (FindEvent((*i)->GetName(), &evt))
			{
				tmp.push_back(evt);
				ARBConfigEventList::iterator iFound = std::find(begin(), end(), evt);
				assert(iFound != end());
				erase(iFound);
			}
		}
		if (0 < size())
			tmp.insert(tmp.end(), begin(), end());
		std::swap(tmp, *this);
	}
}


bool ARBConfigEventList::VerifyEvent(
		std::wstring const& inEvent,
		std::wstring const& inDivision,
		std::wstring const& inLevel,
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
		std::wstring const& inEvent,
		std::wstring const& inDivision,
		std::wstring const& inLevel,
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
		std::wstring const& inOldDiv,
		std::wstring const& inNewDiv)
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


int ARBConfigEventList::DeleteDivision(std::wstring const& inDiv)
{
	std::wstring div(inDiv);
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
		std::wstring const& inOldDiv,
		std::wstring const& inOldLevel,
		std::wstring const& inNewLevel)
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
		std::wstring const& inDiv,
		std::wstring const& inLevel)
{
	std::wstring level(inLevel);
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


int ARBConfigEventList::RenameLifetimeName(
		std::wstring const& inOldName,
		std::wstring const& inNewName)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		for (ARBConfigScoringList::iterator iterScore = (*iter)->GetScorings().begin();
			iterScore != (*iter)->GetScorings().end(); ++iterScore)
		{
			for (ARBConfigLifetimePointsList::iterator iterPoints = (*iterScore)->GetLifetimePoints().begin();
				iterPoints != (*iterScore)->GetLifetimePoints().end(); ++iterPoints)
			{
				if ((*iterPoints)->GetName() == inOldName)
				{
					++count;
					(*iterPoints)->SetName(inNewName);
				}
			}
		}
	}
	return count;
}


int ARBConfigEventList::DeleteLifetimeName(std::wstring const& inName)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		for (ARBConfigScoringList::iterator iterScore = (*iter)->GetScorings().begin();
			iterScore != (*iter)->GetScorings().end(); ++iterScore)
		{
			for (ARBConfigLifetimePointsList::iterator iterPoints = (*iterScore)->GetLifetimePoints().begin();
				iterPoints != (*iterScore)->GetLifetimePoints().end(); )
			{
				if ((*iterPoints)->GetName() == inName)
				{
					++count;
					iterPoints = (*iterScore)->GetLifetimePoints().erase(iterPoints);
				}
				else
					++iterScore;
			}
		}
	}
	return count;
}


bool ARBConfigEventList::FindEvent(
		std::wstring const& inEvent,
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


bool ARBConfigEventList::DeleteEvent(std::wstring const& inEvent)
{
	std::wstring pEvent(inEvent);
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
