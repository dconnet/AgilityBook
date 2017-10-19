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
 * 2017-10-18 Fixed FindTitleCompleteName call.
 * 2016-06-29 Fix when default lifetime name is created during conversion.
 * 2016-01-16 Finish lifetime conversion.
 * 2016-01-06 Removed LifetimeName, added LifetimeNames.
 * 2013-01-13 Added new recurring title suffix style.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2007-10-10 Added 'LifetimeName'
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-14 Moved 'Titles' to 'Venue'.
 * 2005-01-01 Added a long name to the venue.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-03-26 Update didn't save desc changes if nothing else changed.
 * 2004-02-02 Added VerifyEvent.
 * 2004-01-27 Updating could cause some false-positive messages because
 *            the ordering was different.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-12-27 Changed FindEvent to take a date.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-07-16 Allow the code to keep processing after errors are found.
 */

#include "stdafx.h"
#include "ARB/ARBConfigVenue.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBConfigDivision.h"
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
	class ARBConfigVenue_concrete : public ARBConfigVenue
	{
	public:
		ARBConfigVenue_concrete() {}
		ARBConfigVenue_concrete(ARBConfigVenue const& rhs)
			: ARBConfigVenue(rhs)
		{
		}
	};
};


ARBConfigVenuePtr ARBConfigVenue::New()
{
	return std::make_shared<ARBConfigVenue_concrete>();
}


ARBConfigVenue::ARBConfigVenue()
	: m_Name()
	, m_LongName()
	, m_URL()
	, m_Desc()
	, m_LifetimeNames()
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
	, m_idxIcon(rhs.m_idxIcon)
	, m_Titles()
	, m_Divisions()
	, m_Events()
	, m_MultiQs()
{
	rhs.m_LifetimeNames.Clone(m_LifetimeNames);
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
	return std::make_shared<ARBConfigVenue_concrete>(*this);
}


ARBConfigVenue& ARBConfigVenue::operator=(ARBConfigVenue const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_LongName = rhs.m_LongName;
		m_URL = rhs.m_URL;
		m_Desc = rhs.m_Desc;
		rhs.m_LifetimeNames.Clone(m_LifetimeNames);
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
		&& m_LifetimeNames == rhs.m_LifetimeNames
		&& m_idxIcon == rhs.m_idxIcon
		&& m_Titles == rhs.m_Titles
		&& m_Divisions == rhs.m_Divisions
		&& m_Events == rhs.m_Events
		&& m_MultiQs == rhs.m_MultiQs;
}


void ARBConfigVenue::clear()
{
	m_Name.clear();
	m_LongName.clear();
	m_URL.clear();
	m_Desc.clear();
	m_LifetimeNames.clear();
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
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_VENUE)
		return false;
	// Get the venue name.
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_VENUE_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_VENUE, ATTRIB_VENUE_NAME));
		return false;
	}
	// Long name added in v10.1
	inTree->GetAttrib(ATTRIB_VENUE_LONGNAME, m_LongName);
	// URL added in v12.3
	inTree->GetAttrib(ATTRIB_VENUE_URL, m_URL);
	// LifetimeName added in v12.6, removed in 14.4
	bool bAddLifetime = false;
	if (inVersion < ARBVersion(14,4))
	{
		std::wstring lifetimeName;
		if (ElementNode::eFound == inTree->GetAttrib(L"LifetimeName", lifetimeName))
			m_LifetimeNames.AddLifetimeName(lifetimeName);
	}
	// Icon index added in 12.5
	inTree->GetAttrib(ATTRIB_VENUE_ICON, m_idxIcon);
	for (int i = 0; i < inTree->GetElementCount(); ++i)
	{
		ElementNodePtr element = inTree->GetElementNode(i);
		if (!element)
			continue;
		std::wstring const& name = element->GetName();
		if (name == TREE_VENUE_DESC)
		{
			m_Desc = element->GetValue();
		}
		else if (name == TREE_VENUE_LIFETIMENAMES)
		{
			m_LifetimeNames.Load(element, inVersion, ioCallback);
		}
		else if (name == TREE_TITLES)
		{
			// Ignore any errors...
			m_Titles.Load(element, inVersion, ioCallback);
		}
		else if (name == TREE_DIVISION)
		{
			if (0 < m_Events.size())
			{
				ioCallback.LogMessage(Localization()->ErrorInvalidDocStructure(Localization()->InvalidVenueConfig()));
				return false;
			}
			// Ignore any errors...
			m_Divisions.Load(*this, element, inVersion, ioCallback);
		}
		else if (name == TREE_EVENT)
		{
			// Ignore any errors...
			m_Events.Load(m_Divisions, element, inVersion, ioCallback);
			// Look for any lifetime points for conversion.
			if (inVersion < ARBVersion(14,4))
			{
				for (ARBConfigEventList::iterator iter = m_Events.begin();
					!bAddLifetime && iter != m_Events.end();
					++iter)
				{
					for (ARBConfigScoringList::iterator iterS = (*iter)->GetScorings().begin();
						!bAddLifetime && iterS != (*iter)->GetScorings().end();
						++iterS)
					{
						if (!(*iterS)->GetLifetimePoints().empty())
							bAddLifetime = true;
					}
				}
			}
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
	if (bAddLifetime && m_LifetimeNames.empty())
		m_LifetimeNames.AddLifetimeName(L"");

	// Finish lifetime name conversion.
	if (inVersion < ARBVersion(14,4))
	{
		if (1 == m_LifetimeNames.size() && !m_LifetimeNames.front()->GetName().empty())
		{
			std::wstring name = m_LifetimeNames.front()->GetName();
			for (ARBConfigEventList::iterator iter = m_Events.begin();
				iter != m_Events.end();
				++iter)
			{
				for (ARBConfigScoringList::iterator iterS = (*iter)->GetScorings().begin();
					iterS != (*iter)->GetScorings().end();
					++iterS)
				{
					for (ARBConfigLifetimePointsList::iterator iterL = (*iterS)->GetLifetimePoints().begin();
						iterL != (*iterS)->GetLifetimePoints().end();
						++iterL)
					{
						if ((*iterL)->GetName().empty())
						{
							(*iterL)->FixName(name);
						}
					}
				}
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
					pMulti->SetName(L"Double Q");
					pMulti->SetShortName(L"QQ");
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
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr venue = ioTree->AddElementNode(TREE_VENUE);
	venue->AddAttrib(ATTRIB_VENUE_NAME, m_Name);
	if (0 < m_LongName.length())
		venue->AddAttrib(ATTRIB_VENUE_LONGNAME, m_LongName);
	if (0 < m_URL.length())
		venue->AddAttrib(ATTRIB_VENUE_URL, m_URL);
	venue->AddAttrib(ATTRIB_VENUE_ICON, m_idxIcon);
	if (0 < m_Desc.length())
	{
		ElementNodePtr desc = venue->AddElementNode(TREE_VENUE_DESC);
		desc->SetValue(m_Desc);
	}
	if (!m_LifetimeNames.Save(venue))
		return false;
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
		std::wstring& ioInfo)
{
	std::wstring info;
	if (GetName() != inVenueNew->GetName())
		return false;

	std::wstring indentBuffer, indentName;
	for (int i = 0; i < indent-1; ++i)
		indentName += L"   ";
	indentBuffer = indentName + L"   ";
	indentName += L"-";

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

	if (GetLifetimeNames() != inVenueNew->GetLifetimeNames())
	{
		int nNew = 0;
		int nChanges = 0;
		int nSkipped = 0;
		for (ARBConfigLifetimeNameList::const_iterator iter = inVenueNew->GetLifetimeNames().begin();
			iter != inVenueNew->GetLifetimeNames().end();
			++iter)
		{
			if (!GetLifetimeNames().FindLifetimeName((*iter)->GetName()))
			{
				++nNew;
				++nChanges;
				GetLifetimeNames().AddLifetimeName((*iter)->GetName());
			}
			else
				++nSkipped;
		}
		if (0 < nNew || 0 < nChanges)
		{
			ioInfo += Localization()->UpdateLifetimeNames(nNew, nSkipped);
			ioInfo += L"\n";
		}

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
					pTitle->SetLongName((*iterTitle)->GetLongName());
					pTitle->SetMultipleStartAt((*iterTitle)->GetMultipleStartAt());
					pTitle->SetMultipleIncrement((*iterTitle)->GetMultipleIncrement());
					pTitle->SetMultipleStyle((*iterTitle)->GetMultipleStyle(), (*iterTitle)->GetMultipleStyleSeparator());
					pTitle->SetPrefix((*iterTitle)->GetPrefix());
					pTitle->SetValidFrom((*iterTitle)->GetValidFrom());
					pTitle->SetValidTo((*iterTitle)->GetValidTo());
					pTitle->SetDescription((*iterTitle)->GetDescription());
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
			info += Localization()->UpdateTitles(nAdded, nChanged, nSkipped);
			info += L"\n";
		}
		else
		{
			info += indentBuffer;
			info += Localization()->UpdateTitlesReordered();
			info += L"\n";
		}
	}

	// If the order is different, we will fall into this...
	if (GetDivisions() != inVenueNew->GetDivisions())
	{
		std::wstring info2;
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
				info2 += L"+";
				info2 += (*iterDiv)->GetName();
				info2 += L"\n";
			}
		}
		GetDivisions().ReorderBy(inVenueNew->GetDivisions());
		// ... so only generate a message if we added or changed.
		if (0 < nAdded || 0 < nChanged)
		{
			info += indentBuffer;
			info += Localization()->UpdateDivisions(nAdded, nChanged, nSkipped);
			info += L"\n";
			info += info2;
		}
		else
		{
			info += indentBuffer;
			info += Localization()->UpdateDivisionsReordered();
			info += L"\n";
		}
	}

	// If the order is different, we will fall into this...
	if (GetEvents() != inVenueNew->GetEvents())
	{
		std::wstring info2;
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
				info2 += L"+";
				info2 += (*iterEvent)->GetName();
				info2 += L"\n";
			}
		}
		GetEvents().ReorderBy(inVenueNew->GetEvents());
		// ... so only generate a message if we added or changed.
		if (0 < nAdded || 0 < nChanged)
		{
			info += indentBuffer;
			info += Localization()->UpdateEvents(nAdded, nChanged, nSkipped);
			info += L"\n";
			info += info2;
		}
		else
		{
			info += indentBuffer;
			info += Localization()->UpdateEventsReordered();
			info += L"\n";
		}
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
			info += Localization()->UpdateMultiqs(nAdded, nDeleted, nSkipped);
			info += L"\n";
		}
		else
		{
			info += indentBuffer;
			info += Localization()->UpdateMultiqsReordered();
			info += L"\n";
		}
	}

	if (0 < info.length())
	{
		bChanges = true;
		ioInfo += indentName + GetName() + L"\n" + info;
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


#ifndef ARB_HAS_LAMBDA
class SortConfigVenue
{
public:
	SortConfigVenue() {}
	bool operator()(ARBConfigVenuePtr const& one, ARBConfigVenuePtr const& two) const
	{
		return StringUtil::CompareNoCase(one->GetName(), two->GetName()) < 0;
	}
};
#endif


void ARBConfigVenueList::sort()
{
	if (2 > size())
		return;
#ifdef ARB_HAS_LAMBDA
	std::stable_sort(begin(), end(),
		[](ARBConfigVenuePtr const& one, ARBConfigVenuePtr const& two)
		{
			return StringUtil::CompareNoCase(one->GetName(), two->GetName()) < 0;
		}
	);
#else
	std::stable_sort(begin(), end(), SortConfigVenue());
#endif
}


bool ARBConfigVenueList::VerifyMultiQ(
		std::wstring const& inVenue,
		std::wstring const& inMultiQ,
		bool inUseShortName) const
{
	ARBConfigVenuePtr pVenue;
	if (FindVenue(inVenue, &pVenue))
		return pVenue->GetMultiQs().FindMultiQ(inMultiQ, inUseShortName);
	return false;
}


bool ARBConfigVenueList::VerifyLevel(
		std::wstring const& inVenue,
		std::wstring const& inDivision,
		std::wstring const& inLevel) const
{
	ARBConfigVenuePtr pVenue;
	if (FindVenue(inVenue, &pVenue))
		return pVenue->GetDivisions().VerifyLevel(inDivision, inLevel);
	return false;
}


bool ARBConfigVenueList::VerifyEvent(
		std::wstring const& inVenue,
		std::wstring const& inDivision,
		std::wstring const& inLevel,
		std::wstring const& inEvent,
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
		std::wstring const& inVenue,
		std::wstring const& inName,
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
		if (pVenue->GetTitles().FindTitleCompleteName(inName, 0, bAbbrevFirst, &pTitle))
		{
			if (outTitle)
				*outTitle = pTitle;
			bFound = true;
		}
	}
	return bFound;
}


bool ARBConfigVenueList::FindTitle(
		std::wstring const& inVenue,
		std::wstring const& inTitle,
		ARBConfigTitlePtr* outTitle) const
{
	if (outTitle)
		outTitle->reset();
	ARBConfigVenuePtr pVenue;
	if (FindVenue(inVenue, &pVenue))
		return pVenue->GetTitles().FindTitle(inTitle, outTitle);
	return false;
}


bool ARBConfigVenueList::DeleteTitle(std::wstring const& inTitle)
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
		std::wstring const& inVenue,
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
		std::wstring const& inVenue,
		ARBConfigVenuePtr* outVenue)
{
	if (outVenue)
		outVenue->reset();
	if (inVenue.empty())
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


int ARBConfigVenueList::DeleteVenue(std::wstring const& inVenue)
{
	std::wstring venue(inVenue);
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
		std::wstring const& inVenue,
		std::wstring const& inEvent,
		std::wstring const& inDivision,
		std::wstring const& inLevel,
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
			//	assert(pLevel);
		}
		else
			assert(pDiv);
	}
	return bFound;
}
