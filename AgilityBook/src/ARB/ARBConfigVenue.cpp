/*
 * Copyright � 2002-2004 David Connet. All Rights Reserved.
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
 * @li 2004-02-02 DRC Added VerifyEvent.
 * @li 2004-01-27 DRC Updating could cause some false-positive messages because
 *                    the ordering was different.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-07-16 DRC Allow the code to keep processing after errors are found.
 */

#include "StdAfx.h"
#include "ARBConfigVenue.h"
#include <algorithm>

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "ARBConfigDivision.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigVenue::ARBConfigVenue()
	: m_Name()
	, m_Desc()
	, m_Divisions()
	, m_Events()
{
}

ARBConfigVenue::ARBConfigVenue(const ARBConfigVenue& rhs)
	: m_Name(rhs.m_Name)
	, m_Desc(rhs.m_Desc)
	, m_Divisions(rhs.m_Divisions)
	, m_Events(rhs.m_Events)
{
}

ARBConfigVenue::~ARBConfigVenue()
{
}

ARBConfigVenue& ARBConfigVenue::operator=(const ARBConfigVenue& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Desc = rhs.m_Desc;
		m_Divisions = rhs.m_Divisions;
		m_Events = rhs.m_Events;
	}
	return *this;
}

bool ARBConfigVenue::operator==(const ARBConfigVenue& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Desc == rhs.m_Desc
		&& m_Divisions == rhs.m_Divisions
		&& m_Events == rhs.m_Events;
}

bool ARBConfigVenue::operator!=(const ARBConfigVenue& rhs) const
{
	return !operator==(rhs);
}

void ARBConfigVenue::clear()
{
	m_Name.erase();
	m_Desc.erase();
	m_Divisions.clear();
	m_Events.clear();
}

size_t ARBConfigVenue::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigVenue::Load(
	ARBConfig& ioConfig,
	const CElement& inTree,
	const ARBVersion& inVersion,
	std::string& ioErrMsg)
{
	// Get the venue name.
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_VENUE_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioErrMsg += ErrorMissingAttribute(TREE_VENUE, ATTRIB_VENUE_NAME);
		return false;
	}
	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		const CElement& element = inTree.GetElement(i);
		const std::string& name = element.GetName();
		if (name == TREE_VENUE_DESC)
		{
			m_Desc = element.GetValue();
		}
		else if (name == TREE_DIVISION)
		{
			if (0 < m_Events.size())
			{
				ioErrMsg += ErrorInvalidDocStructure(INVALID_VENUE_CONFIG);
				return false;
			}
			// Ignore any errors...
			m_Divisions.Load(element, inVersion, ioErrMsg);
		}
		else if (name == TREE_EVENT)
		{
			// Ignore any errors...
			m_Events.Load(m_Divisions, element, inVersion, ioErrMsg);
		}
		if (inVersion < ARBVersion(3,0))
		{
			if (name == TREE_FAULTTYPE)
			{
				ARBConfigFault* pFault = new ARBConfigFault();
				// Kind-of ignore any errors...
				bool bOk = pFault->Load(element, inVersion, ioErrMsg);
				// When migrating, avoid duplicate fault names.
				// We do allow the user have duplicates. (just not here)
				bool bExists = (bOk && NULL != ioConfig.GetFaults().FindFault(pFault->GetName()));
				pFault->Release();
				if (bOk && !bExists)
				{
					ioConfig.LoadFault(element, inVersion, ioErrMsg);
				}
			}
			else if (name == TREE_OTHERPTS)
			{
				// Ignore any errors...
				ioConfig.LoadOtherPoints(element, inVersion, ioErrMsg);
			}
		}
	}
	return true;
}

bool ARBConfigVenue::Save(CElement& ioTree) const
{
	CElement& venue = ioTree.AddElement(TREE_VENUE);
	venue.AddAttrib(ATTRIB_VENUE_NAME, m_Name);
	if (0 < m_Desc.length())
	{
		CElement& desc = venue.AddElement(TREE_VENUE_DESC);
		desc.SetValue(m_Desc);
	}
	if (!m_Divisions.Save(venue))
		return false;
	if (!m_Events.Save(venue))
		return false;
	return true;
}

bool ARBConfigVenue::Update(int indent, const ARBConfigVenue* inVenueNew, std::string& ioInfo)
{
	std::string info;
	if (GetName() != inVenueNew->GetName())
		return false;

	std::string indentBuffer, indentName;
	for (int i = 0; i < indent-1; ++i)
		indentName += "   ";
	indentBuffer = indentName + "   ";
	indentName += "-";

	if (GetDesc() != inVenueNew->GetDesc())
		SetDesc(inVenueNew->GetDesc());
	// If the order is different, we will fall into this...
	if (GetDivisions() != inVenueNew->GetDivisions())
	{
		std::string info2;
		int nChanged, nAdded, nSkipped;
		nChanged = nAdded = nSkipped = 0;
		for (ARBConfigDivisionList::const_iterator iterDiv = inVenueNew->GetDivisions().begin();
			iterDiv != inVenueNew->GetDivisions().end();
			++iterDiv)
		{
			ARBConfigDivision* pDiv = GetDivisions().FindDivision((*iterDiv)->GetName());
			if (pDiv)
			{
				if (*(*iterDiv) == *pDiv)
					++nSkipped;
				else
				{
					if (pDiv->Update(indent+1, (*iterDiv), info2))
						++nChanged;
				}
			}
			else
			{
				++nAdded;
				GetDivisions().AddDivision((*iterDiv));
				info2 += indentBuffer;
				info2 += "+";
				info2 += (*iterDiv)->GetName();
				info2 += "\n";
			}
		}
		// ... so only generate a message if we added or changed.
		if (0 < nAdded || 0 < nChanged)
		{
			char buffer[1000];
			sprintf(buffer, UPDATE_FORMAT_DIVISIONS, nAdded, nChanged, nSkipped);
			info += indentBuffer;
			info += buffer;
			info += info2;
		}
	}

	// If the order is different, we will fall into this...
	if (GetEvents() != inVenueNew->GetEvents())
	{
		std::string info2;
		int nChanged, nAdded, nSkipped;
		nChanged = nAdded = nSkipped = 0;
		for (ARBConfigEventList::const_iterator iterEvent = inVenueNew->GetEvents().begin();
			iterEvent != inVenueNew->GetEvents().end();
			++iterEvent)
		{
			ARBConfigEvent* pEvent = GetEvents().FindEvent((*iterEvent)->GetName());
			if (pEvent)
			{
				if (*(*iterEvent) == *pEvent)
					++nSkipped;
				else
				{
					if (pEvent->Update(indent+1, (*iterEvent), info2))
						++nChanged;
				}
			}
			else
			{
				++nAdded;
				GetEvents().AddEvent((*iterEvent));
				info2 += indentBuffer;
				info2 += "+";
				info2 += (*iterEvent)->GetName();
				info2 += "\n";
			}
		}
		// ... so only generate a message if we added or changed.
		if (0 < nAdded || 0 < nChanged)
		{
			char buffer[1000];
			sprintf(buffer, UPDATE_FORMAT_EVENTS, nAdded, nChanged, nSkipped);
			info += indentBuffer;
			info += buffer;
			info += info2;
		}
	}
	bool bChanges = false;
	if (0 < info.length())
	{
		bChanges = true;
		ioInfo += indentName + GetName() + "\n" + info;
	}
	return bChanges;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigVenueList::Load(
	ARBConfig& ioConfig,
	const CElement& inTree,
	const ARBVersion& inVersion,
	std::string& ioErrMsg)
{
	ARBConfigVenue* thing = new ARBConfigVenue();
	if (!thing->Load(ioConfig, inTree, inVersion, ioErrMsg))
	{
		thing->Release();
		return false;
	}
	push_back(thing);
	return true;
}

class SortConfigVenue
{
public:
	SortConfigVenue(bool bDescending) : m_bDescending(bDescending) {}
	bool operator()(ARBConfigVenue* one, ARBConfigVenue* two) const
	{
		if (one->GetName() < two->GetName())
			return m_bDescending;
		else
			return !m_bDescending;
	}
private:
	bool m_bDescending;
};

void ARBConfigVenueList::sort(bool inDescending)
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortConfigVenue(inDescending));
}

bool ARBConfigVenueList::VerifyVenue(const std::string& inVenue) const
{
	return NULL != FindVenue(inVenue);
}

bool ARBConfigVenueList::VerifyLevel(
	const std::string& inVenue,
	const std::string& inDivision,
	const std::string& inLevel) const
{
	const ARBConfigVenue* pVenue = FindVenue(inVenue);
	if (pVenue)
		return pVenue->GetDivisions().VerifyLevel(inDivision, inLevel);
	return false;
}

bool ARBConfigVenueList::VerifyEvent(
	const std::string& inVenue,
	const std::string& inDivision,
	const std::string& inLevel,
	const std::string& inEvent
	) const
{
	const ARBConfigVenue* pVenue = FindVenue(inVenue);
	if (pVenue)
	{
		// Translate the sublevel to level.
		const ARBConfigDivision* pDiv = pVenue->GetDivisions().FindDivision(inDivision);
		if (pDiv)
		{
			const ARBConfigLevel* pLevel = pDiv->GetLevels().FindSubLevel(inLevel);
			if (pLevel)
				return pVenue->GetEvents().VerifyEvent(inEvent, inDivision, pLevel->GetName());
		}
	}
	return NULL;
}

const ARBConfigTitle* ARBConfigVenueList::FindTitleCompleteName(
	const std::string& inVenue,
	const std::string& inName,
	bool bAbbrevFirst) const
{
	const ARBConfigVenue* pVenue = FindVenue(inVenue);
	if (pVenue)
		return pVenue->GetDivisions().FindTitleCompleteName(inName, bAbbrevFirst);
	return NULL;
}

const ARBConfigTitle* ARBConfigVenueList::FindTitle(
	const std::string& inVenue,
	const std::string& inTitle) const
{
	const ARBConfigVenue* pVenue = FindVenue(inVenue);
	if (pVenue)
		return pVenue->GetDivisions().FindTitle(inTitle);
	return NULL;
}

const ARBConfigVenue* ARBConfigVenueList::FindVenue(const std::string& inVenue) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inVenue)
			return (*iter);
	}
	return NULL;
}

ARBConfigVenue* ARBConfigVenueList::FindVenue(const std::string& inVenue)
{
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inVenue)
			return (*iter);
	}
	return NULL;
}

ARBConfigVenue* ARBConfigVenueList::AddVenue(const std::string& inVenue)
{
	if (0 == inVenue.length())
		return NULL;
	if (NULL != FindVenue(inVenue))
		return NULL;
	ARBConfigVenue* pVenue = new ARBConfigVenue();
	pVenue->SetName(inVenue);
	push_back(pVenue);
	sort();
	return pVenue;
}

ARBConfigVenue* ARBConfigVenueList::AddVenue(ARBConfigVenue* inVenue)
{
	if (!inVenue)
		return NULL;
	if (NULL != FindVenue(inVenue->GetName()))
		return NULL;
	inVenue->AddRef();
	push_back(inVenue);
	sort();
	return inVenue;
}

int ARBConfigVenueList::DeleteVenue(const std::string& inVenue)
{
	std::string venue(inVenue);
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == venue)
		{
			erase(iter);
			return 1; // Names are unique, no need to continue.
		}
	}
	return 0;
}

// inLevel is the true (ie: sub) level.
// This is the only 'FindEvent' that takes a true level. All others take
// a ARBConfigLevel.
const ARBConfigScoring* ARBConfigVenueList::FindEvent(
	const std::string& inVenue,
	const std::string& inEvent,
	const std::string& inDivision,
	const std::string& inLevel,
	const ARBDate& inDate) const
{
	const ARBConfigVenue* pVenue = FindVenue(inVenue);
	if (pVenue)
	{
		// Translate the sublevel to level.
		const ARBConfigDivision* pDiv = pVenue->GetDivisions().FindDivision(inDivision);
		ASSERT(pDiv);
		if (!pDiv)
			return NULL;
		const ARBConfigLevel* pLevel = pDiv->GetLevels().FindSubLevel(inLevel);
		ASSERT(pLevel);
		if (!pLevel)
			return NULL;
		return pVenue->GetEvents().FindEvent(inEvent, inDivision, pLevel->GetName(), inDate);
	}
	return NULL;
}
