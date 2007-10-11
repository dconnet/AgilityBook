/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
 * @li 2007-10-10 DRC Added 'LifetimeName'
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-01-01 DRC Added a long name to the venue.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-03-26 DRC Update didn't save desc changes if nothing else changed.
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
#include "ARBLocalization.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigVenuePtr ARBConfigVenue::New()
{
	return ARBConfigVenuePtr(new ARBConfigVenue());
}


ARBConfigVenue::ARBConfigVenue()
	: m_Name()
	, m_LongName()
	, m_URL()
	, m_Desc()
	, m_LifetimeName()
	, m_idxIcon(-1)
	, m_Titles()
	, m_Divisions()
	, m_Events()
	, m_MultiQs()
{
}


ARBConfigVenue::ARBConfigVenue(ARBConfigVenue const& rhs)
	: m_Name(rhs.m_Name)
	, m_LongName(rhs.m_LongName)
	, m_URL(rhs.m_URL)
	, m_Desc(rhs.m_Desc)
	, m_LifetimeName(rhs.m_LifetimeName)
	, m_idxIcon(rhs.m_idxIcon)
	, m_Titles()
	, m_Divisions()
	, m_Events()
	, m_MultiQs()
{
	rhs.m_Titles.Clone(m_Titles);
	rhs.m_Divisions.Clone(m_Divisions);
	rhs.m_Events.Clone(m_Events);
	rhs.m_MultiQs.Clone(m_MultiQs);
}


ARBConfigVenue::~ARBConfigVenue()
{
}


ARBConfigVenuePtr ARBConfigVenue::Clone() const
{
	return ARBConfigVenuePtr(new ARBConfigVenue(*this));
}


ARBConfigVenue& ARBConfigVenue::operator=(ARBConfigVenue const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_LongName = rhs.m_LongName;
		m_URL = rhs.m_URL;
		m_Desc = rhs.m_Desc;
		m_LifetimeName = rhs.m_LifetimeName;
		m_idxIcon = rhs.m_idxIcon;
		rhs.m_Titles.Clone(m_Titles);
		rhs.m_Divisions.Clone(m_Divisions);
		rhs.m_Events.Clone(m_Events);
		rhs.m_MultiQs.Clone(m_MultiQs);
	}
	return *this;
}


bool ARBConfigVenue::operator==(ARBConfigVenue const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_LongName == rhs.m_LongName
		&& m_URL == rhs.m_URL
		&& m_Desc == rhs.m_Desc
		&& m_LifetimeName == rhs.m_LifetimeName
		&& m_idxIcon == rhs.m_idxIcon
		&& m_Titles == rhs.m_Titles
		&& m_Divisions == rhs.m_Divisions
		&& m_Events == rhs.m_Events
		&& m_MultiQs == rhs.m_MultiQs;
}


void ARBConfigVenue::clear()
{
	m_Name.erase();
	m_LongName.erase();
	m_URL.erase();
	m_Desc.erase();
	m_LifetimeName.erase();
	m_idxIcon = -1;
	m_Titles.clear();
	m_Divisions.clear();
	m_Events.clear();
	m_MultiQs.clear();
}


bool ARBConfigVenue::Load(
		ARBConfig& ioConfig,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ASSERT(inTree);
	if (!inTree)
		return false;
	// Get the venue name.
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_VENUE_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_VENUE, ATTRIB_VENUE_NAME));
		return false;
	}
	// Long name added in v10.1
	inTree->GetAttrib(ATTRIB_VENUE_LONGNAME, m_LongName);
	// URL added in v12.3
	inTree->GetAttrib(ATTRIB_VENUE_URL, m_URL);
	// LifetimeName added in v12.6
	inTree->GetAttrib(ATTRIB_VENUE_LIFETIME_NAME, m_LifetimeName);
	// Icon index added in 12.5
	inTree->GetAttrib(ATTRIB_VENUE_ICON, m_idxIcon);
	for (int i = 0; i < inTree->GetElementCount(); ++i)
	{
		ElementNodePtr element = inTree->GetElementNode(i);
		if (!element)
			continue;
		tstring const& name = element->GetName();
		if (name == TREE_VENUE_DESC)
		{
			m_Desc = element->GetValue();
		}
		else if (element->GetName() == TREE_TITLES)
		{
			// Ignore any errors...
			m_Titles.Load(element, inVersion, ioCallback);
		}
		else if (name == TREE_DIVISION)
		{
			if (0 < m_Events.size())
			{
				ioCallback.LogMessage(ErrorInvalidDocStructure(INVALID_VENUE_CONFIG));
				return false;
			}
			// Ignore any errors...
			m_Divisions.Load(*this, element, inVersion, ioCallback);
		}
		else if (name == TREE_EVENT)
		{
			// Ignore any errors...
			m_Events.Load(m_Divisions, element, inVersion, ioCallback);
		}
		else if (name == TREE_MULTIQ)
		{
			// Ignore any errors...
			m_MultiQs.Load(*this, element, inVersion, ioCallback);
		}
		if (inVersion < ARBVersion(3,0))
		{
			if (name == TREE_FAULTTYPE)
			{
				ARBConfigFaultPtr pFault(ARBConfigFault::New());
				// Kind-of ignore any errors...
				bool bOk = pFault->Load(element, inVersion, ioCallback);
				// When migrating, avoid duplicate fault names.
				// We do allow the user have duplicates. (just not here)
				bool bExists = (bOk && ioConfig.GetFaults().FindFault(pFault->GetName()));
				if (bOk && !bExists)
				{
					ioConfig.LoadFault(element, inVersion, ioCallback);
				}
			}
			else if (name == TREE_OTHERPTS)
			{
				// Ignore any errors...
				ioConfig.LoadOtherPoints(element, inVersion, ioCallback);
			}
		}
	}

	// Convert old double Qs into new ones.
	if (inVersion < ARBVersion(11, 0))
	{
		ARBConfigMultiQPtr pMulti;
		for (ARBConfigEventList::iterator iter = m_Events.begin();
			iter != m_Events.end();
			++iter)
		{
			for (ARBConfigScoringList::iterator iterS = (*iter)->GetScorings().begin();
				iterS != (*iter)->GetScorings().end();
				++iterS)
			{
				if ((*iterS)->ConvertDoubleQ())
				{
					if (!pMulti)
						pMulti = ARBConfigMultiQPtr(ARBConfigMultiQ::New());
					pMulti->SetName(_T("Double Q"));
					pMulti->SetShortName(_T("QQ"));
					pMulti->AddItem((*iterS)->GetDivision(), (*iterS)->GetLevel(), (*iter)->GetName());
				}
			}
		}
		if (pMulti && 1 < pMulti->GetNumItems())
			m_MultiQs.AddMultiQ(pMulti);
	}

	return true;
}


bool ARBConfigVenue::Save(ElementNodePtr ioTree) const
{
	ASSERT(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr venue = ioTree->AddElementNode(TREE_VENUE);
	venue->AddAttrib(ATTRIB_VENUE_NAME, m_Name);
	if (0 < m_LongName.length())
		venue->AddAttrib(ATTRIB_VENUE_LONGNAME, m_LongName);
	if (0 < m_URL.length())
		venue->AddAttrib(ATTRIB_VENUE_URL, m_URL);
	if (0 < m_LifetimeName.length())
		venue->AddAttrib(ATTRIB_VENUE_LIFETIME_NAME, m_LifetimeName);
	venue->AddAttrib(ATTRIB_VENUE_ICON, m_idxIcon);
	if (0 < m_Desc.length())
	{
		ElementNodePtr desc = venue->AddElementNode(TREE_VENUE_DESC);
		desc->SetValue(m_Desc);
	}
	if (!m_Titles.Save(venue))
		return false;
	if (!m_Divisions.Save(venue))
		return false;
	if (!m_Events.Save(venue))
		return false;
	if (!m_MultiQs.Save(venue))
		return false;
	return true;
}


bool ARBConfigVenue::Update(
		int indent,
		ARBConfigVenuePtr inVenueNew,
		tstring& ioInfo)
{
	tstring info;
	if (GetName() != inVenueNew->GetName())
		return false;

	tstring indentBuffer, indentName;
	for (int i = 0; i < indent-1; ++i)
		indentName += _T("   ");
	indentBuffer = indentName + _T("   ");
	indentName += _T("-");

	bool bChanges = false;
	if (GetLongName() != inVenueNew->GetLongName())
	{
		bChanges = true;
		SetLongName(inVenueNew->GetLongName());
	}

	if (GetURL() != inVenueNew->GetURL())
	{
		bChanges = true;
		SetURL(inVenueNew->GetURL());
	}

	if (GetDesc() != inVenueNew->GetDesc())
	{
		bChanges = true;
		SetDesc(inVenueNew->GetDesc());
	}

	if (GetLifetimeName() != inVenueNew->GetLifetimeName())
	{
		bChanges = true;
		SetLifetimeName(inVenueNew->GetLifetimeName());
	}

	if (GetIcon() != inVenueNew->GetIcon())
	{
		bChanges = true;
		SetIcon(inVenueNew->GetIcon());
	}

	// If the order is different, we will fall into this...
	if (GetTitles() != inVenueNew->GetTitles())
	{
		int nChanged, nAdded, nSkipped;
		nChanged = nAdded = nSkipped = 0;
		for (ARBConfigTitleList::const_iterator iterTitle = inVenueNew->GetTitles().begin();
			iterTitle != inVenueNew->GetTitles().end();
			++iterTitle)
		{
			ARBConfigTitlePtr pTitle;
			if (GetTitles().FindTitle((*iterTitle)->GetName(), &pTitle))
			{
				if (*(*iterTitle) == *pTitle)
					++nSkipped;
				else
				{
					++nChanged;
					pTitle->SetMultiple((*iterTitle)->GetMultiple());
					pTitle->SetPrefix((*iterTitle)->GetPrefix());
					pTitle->SetLongName((*iterTitle)->GetLongName());
					pTitle->SetDescription((*iterTitle)->GetDescription());
					pTitle->SetValidFrom((*iterTitle)->GetValidFrom());
					pTitle->SetValidTo((*iterTitle)->GetValidTo());
				}
			}
			else
			{
				++nAdded;
				GetTitles().AddTitle((*iterTitle));
			}
		}
		GetTitles().ReorderBy(inVenueNew->GetTitles());
		// ... so only generate a message if we added or changed.
		if (0 < nAdded || 0 < nChanged)
		{
			info += indentBuffer;
			info += UPDATE_FORMAT_TITLES(nAdded, nChanged, nSkipped);
		}
		else
			info += UPDATE_FORMAT_TITLES_REORDERED();
	}

	// If the order is different, we will fall into this...
	if (GetDivisions() != inVenueNew->GetDivisions())
	{
		tstring info2;
		int nChanged, nAdded, nSkipped;
		nChanged = nAdded = nSkipped = 0;
		for (ARBConfigDivisionList::const_iterator iterDiv = inVenueNew->GetDivisions().begin();
			iterDiv != inVenueNew->GetDivisions().end();
			++iterDiv)
		{
			ARBConfigDivisionPtr pDiv;
			if (GetDivisions().FindDivision((*iterDiv)->GetName(), &pDiv))
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
				info2 += _T("+");
				info2 += (*iterDiv)->GetName();
				info2 += _T("\n");
			}
		}
		GetDivisions().ReorderBy(inVenueNew->GetDivisions());
		// ... so only generate a message if we added or changed.
		if (0 < nAdded || 0 < nChanged)
		{
			info += indentBuffer;
			info += UPDATE_FORMAT_DIVISIONS(nAdded, nChanged, nSkipped);
			info += info2;
		}
		else
			info += UPDATE_FORMAT_DIVISIONS_REORDERED();
	}

	// If the order is different, we will fall into this...
	if (GetEvents() != inVenueNew->GetEvents())
	{
		tstring info2;
		int nChanged, nAdded, nSkipped;
		nChanged = nAdded = nSkipped = 0;
		for (ARBConfigEventList::const_iterator iterEvent = inVenueNew->GetEvents().begin();
			iterEvent != inVenueNew->GetEvents().end();
			++iterEvent)
		{
			ARBConfigEventPtr pEvent;
			if (GetEvents().FindEvent((*iterEvent)->GetName(), &pEvent))
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
				info2 += _T("+");
				info2 += (*iterEvent)->GetName();
				info2 += _T("\n");
			}
		}
		GetEvents().ReorderBy(inVenueNew->GetEvents());
		// ... so only generate a message if we added or changed.
		if (0 < nAdded || 0 < nChanged)
		{
			info += indentBuffer;
			info += UPDATE_FORMAT_EVENTS(nAdded, nChanged, nSkipped);
			info += info2;
		}
		else
			info += UPDATE_FORMAT_EVENTS_REORDERED();
	}

	// If the order is different, we will fall into this...
	if (GetMultiQs() != inVenueNew->GetMultiQs())
	{
		int nAdded, nDeleted, nChanged, nSkipped;
		nAdded = nDeleted = nChanged = nSkipped = 0;
		ARBConfigMultiQList::iterator iter1;
		ARBConfigMultiQList::const_iterator iter2;
		// Look for items that will be removed.
		for (iter1 = GetMultiQs().begin(); iter1 != GetMultiQs().end(); ++iter1)
		{
			bool bFound = false;
			for (iter2 = inVenueNew->GetMultiQs().begin(); iter2 != inVenueNew->GetMultiQs().end(); ++iter2)
			{
				if (*(*iter1) == *(*iter2))
				{
					bFound = true;
					++nSkipped;
					break;
				}
			}
			if (!bFound)
				++nDeleted;
		}
		// Now look for items that will be added.
		for (iter2 = inVenueNew->GetMultiQs().begin(); iter2 != inVenueNew->GetMultiQs().end(); ++iter2)
		{
			bool bFound = false;
			for (iter1 = GetMultiQs().begin(); iter1 != GetMultiQs().end(); ++iter1)
			{
				if (*(*iter1) == *(*iter2))
				{
					bFound = true;
					break;
				}
			}
			if (!bFound)
				++nAdded;
		}
		inVenueNew->GetMultiQs().Clone(GetMultiQs());
		if (0 < nAdded || 0 < nDeleted)
		{
			info += indentBuffer;
			info += GetName();
			info += UPDATE_FORMAT_MULTIQS(nAdded, nDeleted, nSkipped);
		}
		else
			info += UPDATE_FORMAT_MULTIQS_REORDERED();
	}

	if (0 < info.length())
	{
		bChanges = true;
		ioInfo += indentName + GetName() + _T("\n") + info;
	}
	return bChanges;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigVenueList::Load(
		ARBConfig& ioConfig,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigVenuePtr thing(ARBConfigVenue::New());
	if (!thing->Load(ioConfig, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


class SortConfigVenue
{
public:
	SortConfigVenue() {}
	bool operator()(ARBConfigVenuePtr one, ARBConfigVenuePtr two) const
	{
		return one->GetName() < two->GetName();
	}
};


void ARBConfigVenueList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortConfigVenue());
}


bool ARBConfigVenueList::VerifyMultiQ(
		tstring const& inVenue,
		tstring const& inMultiQ,
		bool inUseShortName) const
{
	ARBConfigVenuePtr pVenue;
	if (FindVenue(inVenue, &pVenue))
		return pVenue->GetMultiQs().FindMultiQ(inMultiQ, inUseShortName);
	return false;
}


bool ARBConfigVenueList::VerifyLevel(
		tstring const& inVenue,
		tstring const& inDivision,
		tstring const& inLevel) const
{
	ARBConfigVenuePtr pVenue;
	if (FindVenue(inVenue, &pVenue))
		return pVenue->GetDivisions().VerifyLevel(inDivision, inLevel);
	return false;
}


bool ARBConfigVenueList::VerifyEvent(
		tstring const& inVenue,
		tstring const& inDivision,
		tstring const& inLevel,
		tstring const& inEvent,
		ARBDate const& inDate) const
{
	ARBConfigVenuePtr pVenue;
	bool bFound = false;
	if (FindVenue(inVenue, &pVenue))
	{
		// Translate the sublevel to level.
		ARBConfigDivisionPtr pDiv;
		if (pVenue->GetDivisions().FindDivision(inDivision, &pDiv))
		{
			ARBConfigLevelPtr pLevel;
			if (pDiv->GetLevels().FindSubLevel(inLevel, &pLevel))
			{
				bFound = pVenue->GetEvents().VerifyEvent(inEvent, inDivision, pLevel->GetName(), inDate);
			}
		}
	}
	return bFound;
}


bool ARBConfigVenueList::FindTitleCompleteName(
		tstring const& inVenue,
		tstring const& inName,
		bool bShowInstance,
		bool bAbbrevFirst,
		ARBConfigTitlePtr* outTitle) const
{
	if (outTitle)
		outTitle->reset();
	bool bFound = false;
	ARBConfigVenuePtr pVenue;
	if (FindVenue(inVenue, &pVenue))
	{
		ARBConfigTitlePtr pTitle;
		if (pVenue->GetTitles().FindTitleCompleteName(inName, bShowInstance, bAbbrevFirst, true, &pTitle))
		{
			if (outTitle)
				*outTitle = pTitle;
			bFound = true;
		}
	}
	return bFound;
}


bool ARBConfigVenueList::FindTitle(
		tstring const& inVenue,
		tstring const& inTitle,
		ARBConfigTitlePtr* outTitle) const
{
	if (outTitle)
		outTitle->reset();
	ARBConfigVenuePtr pVenue;
	if (FindVenue(inVenue, &pVenue))
		return pVenue->GetTitles().FindTitle(inTitle, outTitle);
	return false;
}


bool ARBConfigVenueList::DeleteTitle(tstring const& inTitle)
{
	bool bDeleted = false;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetTitles().FindTitle(inTitle))
		{
			bDeleted = (*iter)->GetTitles().DeleteTitle(inTitle);
			break;
		}
	}
	return bDeleted;
}


bool ARBConfigVenueList::FindVenue(
		tstring const& inVenue,
		ARBConfigVenuePtr* outVenue) const
{
	if (outVenue)
		outVenue->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inVenue)
		{
			if (outVenue)
				*outVenue = *iter;
			return true;
		}
	}
	return false;
}


bool ARBConfigVenueList::AddVenue(
		tstring const& inVenue,
		ARBConfigVenuePtr* outVenue)
{
	if (outVenue)
		outVenue->reset();
	if (0 == inVenue.length())
		return false;
	if (FindVenue(inVenue))
		return false;
	ARBConfigVenuePtr pVenue(ARBConfigVenue::New());
	pVenue->SetName(inVenue);
	push_back(pVenue);
	sort();
	if (outVenue)
		*outVenue = pVenue;
	return true;
}


bool ARBConfigVenueList::AddVenue(ARBConfigVenuePtr inVenue)
{
	if (!inVenue)
		return false;
	if (FindVenue(inVenue->GetName()))
		return false;
	push_back(inVenue);
	sort();
	return true;
}


int ARBConfigVenueList::DeleteVenue(tstring const& inVenue)
{
	tstring venue(inVenue);
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
bool ARBConfigVenueList::FindEvent(
		tstring const& inVenue,
		tstring const& inEvent,
		tstring const& inDivision,
		tstring const& inLevel,
		ARBDate const& inDate,
		ARBConfigEventPtr* outEvent,
		ARBConfigScoringPtr* outScoring) const
{
	if (outEvent)
		outEvent->reset();
	if (outScoring)
		outScoring->reset();
	bool bFound = false;
	ARBConfigVenuePtr pVenue;
	if (FindVenue(inVenue, &pVenue))
	{
		// Translate the sublevel to level.
		ARBConfigDivisionPtr pDiv;
		if (pVenue->GetDivisions().FindDivision(inDivision, &pDiv))
		{
			ARBConfigLevelPtr pLevel;
			if (pDiv->GetLevels().FindSubLevel(inLevel, &pLevel))
			{
				bFound = pVenue->GetEvents().FindEvent(inEvent, inDivision, pLevel->GetName(), inDate, outEvent, outScoring);
			}
			// Note, some users have changed NADAC to remove Novice A/B and only
			// have Novice (no sublevels). This means during a config update,
			// all hell will break loose. Don't bother asserting here...
			//else
			//	ASSERT(pLevel);
		}
		else
			ASSERT(pDiv);
	}
	return bFound;
}
