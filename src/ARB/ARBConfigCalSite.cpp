/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBConfigCalSite
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2007-08-02 Created
 */

#include "stdafx.h"
#include "ARB/ARBConfigCalSite.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

class ARBConfigCalSite_concrete : public ARBConfigCalSite
{
public:
	ARBConfigCalSite_concrete() {}
	ARBConfigCalSite_concrete(ARBConfigCalSite const& rhs)
		: ARBConfigCalSite(rhs)
	{
	}
};

ARBConfigCalSitePtr ARBConfigCalSite::New()
{
	return std::make_shared<ARBConfigCalSite_concrete>();
}


ARBConfigCalSite::ARBConfigCalSite()
	: m_Name()
	, m_Desc()
	, m_urlSearch()
	, m_urlHelp()
	, m_Locations()
	, m_Venues()
{
}


ARBConfigCalSite::ARBConfigCalSite(ARBConfigCalSite const& rhs)
	: m_Name(rhs.m_Name)
	, m_Desc(rhs.m_Desc)
	, m_urlSearch(rhs.m_urlSearch)
	, m_urlHelp(rhs.m_urlHelp)
	, m_Locations(rhs.m_Locations)
	, m_Venues(rhs.m_Venues)
{
}


ARBConfigCalSite::~ARBConfigCalSite()
{
}


ARBConfigCalSitePtr ARBConfigCalSite::Clone() const
{
	return std::make_shared<ARBConfigCalSite_concrete>(*this);
}


ARBConfigCalSite& ARBConfigCalSite::operator=(ARBConfigCalSite const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Desc = rhs.m_Desc;
		m_urlSearch = rhs.m_urlSearch;
		m_urlHelp = rhs.m_urlHelp;
		m_Locations = rhs.m_Locations;
		m_Venues = rhs.m_Venues;
	}
	return *this;
}


bool ARBConfigCalSite::operator==(ARBConfigCalSite const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Desc == rhs.m_Desc
		&& m_urlSearch == rhs.m_urlSearch
		&& m_urlHelp == rhs.m_urlHelp
		&& m_Locations == rhs.m_Locations
		&& m_Venues == rhs.m_Venues;
}


bool ARBConfigCalSite::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_CALSITE)
		return false;
	if (ElementNode::eNotFound == inTree->GetAttrib(ATTRIB_CALSITE_NAME, m_Name))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_CALSITE, ATTRIB_CALSITE_NAME));
		return false;
	}
	if (ElementNode::eNotFound == inTree->GetAttrib(ATTRIB_CALSITE_SEARCH, m_urlSearch))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_CALSITE, ATTRIB_CALSITE_SEARCH));
		return false;
	}
	inTree->GetAttrib(ATTRIB_CALSITE_HELP, m_urlHelp);

	for (int i = 0; i < inTree->GetElementCount(); ++i)
	{
		ElementNodePtr element = inTree->GetElementNode(i);
		if (!element)
			continue;
		std::wstring const& name = element->GetName();
		if (name == TREE_CALSITE_DESC)
		{
			m_Desc = element->GetValue();
		}
		else if (name == TREE_LOCCODE)
		{
			// Ignore any errors...
			std::wstring code;
			if (ElementNode::eNotFound != element->GetAttrib(ATTRIB_LOCCODE_CODE, code))
			{
				if (!code.empty())
				{
					std::wstring locname;
					element->GetAttrib(ATTRIB_LOCCODE_NAME, locname);
					m_Locations[code] = locname;
				}
			}
		}
		else if (name == TREE_VENUECODE)
		{
			// Ignore any errors...
			std::wstring code;
			if (ElementNode::eNotFound != element->GetAttrib(ATTRIB_VENUECODE_CODE, code))
			{
				if (!code.empty())
				{
					std::wstring venue;
					element->GetAttrib(ATTRIB_VENUECODE_VENUE, venue);
					m_Venues[code] = venue;
				}
			}
		}
	}
	return true;
}


bool ARBConfigCalSite::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr calsite = ioTree->AddElementNode(TREE_CALSITE);
	calsite->AddAttrib(ATTRIB_CALSITE_NAME, m_Name);
	calsite->AddAttrib(ATTRIB_CALSITE_SEARCH, m_urlSearch);
	if (!m_urlHelp.empty())
		calsite->AddAttrib(ATTRIB_CALSITE_HELP, m_urlHelp);
	if (!m_Desc.empty())
	{
		ElementNodePtr desc = calsite->AddElementNode(TREE_CALSITE_DESC);
		desc->SetValue(m_Desc);
	}
	std::map<std::wstring, std::wstring>::const_iterator i;
	for (i = m_Locations.begin(); i != m_Locations.end(); ++i)
	{
		ElementNodePtr loc = calsite->AddElementNode(TREE_LOCCODE);
		loc->AddAttrib(ATTRIB_LOCCODE_CODE, i->first);
		loc->AddAttrib(ATTRIB_LOCCODE_NAME, i->second);
	}
	for (i = m_Venues.begin(); i != m_Venues.end(); ++i)
	{
		ElementNodePtr loc = calsite->AddElementNode(TREE_VENUECODE);
		loc->AddAttrib(ATTRIB_VENUECODE_CODE, i->first);
		if (!i->second.empty())
			loc->AddAttrib(ATTRIB_VENUECODE_VENUE, i->second);
	}
	return true;
}


std::wstring ARBConfigCalSite::GetFormattedURL(
		std::vector<std::wstring> const& inLocCodes,
		std::vector<std::wstring> const& inVenueCodes) const
{
	std::wstring url(m_urlSearch);
	std::wstring::size_type pos = url.find(L"!L!");
	if (pos != std::wstring::npos)
	{
		std::wstring codes;
		for (std::vector<std::wstring>::const_iterator i = inLocCodes.begin();
			i != inLocCodes.end();
			++i)
		{
			if (!codes.empty())
				codes += L"+";
			codes += *i;
		}
		url.replace(pos, 3, codes);
	}
	pos = url.find(L"!V!");
	if (pos != std::wstring::npos)
	{
		std::wstring codes;
		for (std::vector<std::wstring>::const_iterator i = inVenueCodes.begin();
			i != inVenueCodes.end();
			++i)
		{
			if (!codes.empty())
				codes += L"+";
			codes += *i;
		}
		url.replace(pos, 3, codes);
	}
	return url;
}


bool ARBConfigCalSite::HasLocationCode(std::wstring const& inCode) const
{
	return m_Locations.end() != m_Locations.find(inCode);
}


bool ARBConfigCalSite::AddLocationCode(std::wstring const& inCode, std::wstring const& inName)
{
	if (HasLocationCode(inCode))
		return false;
	m_Locations[inCode] = inName;
	return true;
}


bool ARBConfigCalSite::RemoveLocationCode(std::wstring const& inCode)
{
	bool bOk = false;
	std::map<std::wstring, std::wstring>::iterator i = m_Locations.find(inCode);
	if (m_Locations.end() != i)
	{
		bOk = true;
		m_Locations.erase(i);
	}
	return bOk;
}


bool ARBConfigCalSite::RemoveAllLocationCodes()
{
	bool bOk = false;
	if (m_Locations.size())
	{
		bOk = true;
		m_Locations.clear();
	}
	return bOk;
}


bool ARBConfigCalSite::HasVenueCode(std::wstring const& inCode) const
{
	return m_Venues.end() != m_Venues.find(inCode);
}


bool ARBConfigCalSite::AddVenueCode(std::wstring const& inCode, std::wstring const& inVenue)
{
	if (HasVenueCode(inCode))
		return false;
	m_Venues[inCode] = inVenue;
	return true;
}


bool ARBConfigCalSite::RemoveVenueCode(std::wstring const& inCode)
{
	bool bOk = false;
	std::map<std::wstring, std::wstring>::iterator i = m_Venues.find(inCode);
	if (m_Venues.end() != i)
	{
		bOk = true;
		m_Venues.erase(i);
	}
	return bOk;
}


bool ARBConfigCalSite::RemoveAllVenueCodes()
{
	bool bOk = false;
	if (m_Venues.size())
	{
		bOk = true;
		m_Venues.clear();
	}
	return bOk;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigCalSiteList::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigCalSitePtr thing(ARBConfigCalSite::New());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


#ifndef ARB_HAS_LAMBDA
class SortConfigCalSite
{
public:
	SortConfigCalSite() {}
	bool operator()(ARBConfigCalSitePtr const& one, ARBConfigCalSitePtr const& two) const
	{
		return StringUtil::CompareNoCase(one->GetName(), two->GetName()) < 0;
	}
};
#endif


void ARBConfigCalSiteList::sort()
{
	if (2 > size())
		return;
#ifdef ARB_HAS_LAMBDA
	std::stable_sort(begin(), end(),
		[](ARBConfigCalSitePtr const& one, ARBConfigCalSitePtr const& two)
		{
			return StringUtil::CompareNoCase(one->GetName(), two->GetName()) < 0;
		}
	);
#else
	std::stable_sort(begin(), end(), SortConfigCalSite());
#endif
}


bool ARBConfigCalSiteList::FindSite(
		std::wstring const& inSite,
		ARBConfigCalSitePtr* outSite) const
{
	if (outSite)
		outSite->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inSite)
		{
			if (outSite)
				*outSite = *iter;
			return true;
		}
	}
	return false;
}


bool ARBConfigCalSiteList::AddSite(
		std::wstring const& inSite,
		ARBConfigCalSitePtr* outSite)
{
	if (outSite)
		outSite->reset();
	if (inSite.empty())
		return false;
	if (FindSite(inSite))
		return false;
	ARBConfigCalSitePtr pSite(ARBConfigCalSite::New());
	pSite->SetName(inSite);
	push_back(pSite);
	sort();
	if (outSite)
		*outSite = pSite;
	return true;
}


bool ARBConfigCalSiteList::AddSite(ARBConfigCalSitePtr inSite)
{
	if (!inSite)
		return false;
	if (FindSite(inSite->GetName()))
		return false;
	push_back(inSite);
	sort();
	return true;
}


int ARBConfigCalSiteList::DeleteSite(std::wstring const& inSite)
{
	std::wstring site(inSite);
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == site)
		{
			erase(iter);
			return 1; // Names are unique, no need to continue.
		}
	}
	return 0;
}
