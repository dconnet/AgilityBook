/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief ARBConfigCalSite
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2007-08-02 DRC Created
 */

#include "stdafx.h"
#include "ARBConfigCalSite.h"

#include <algorithm>
#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"
#include "Element.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigCalSitePtr ARBConfigCalSite::New()
{
	return ARBConfigCalSitePtr(new ARBConfigCalSite());
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
	return ARBConfigCalSitePtr(new ARBConfigCalSite(*this));
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
		wxString const& name = element->GetName();
		if (name == TREE_CALSITE_DESC)
		{
			m_Desc = element->GetValue();
		}
		else if (name == TREE_LOCCODE)
		{
			// Ignore any errors...
			wxString code;
			if (ElementNode::eNotFound != element->GetAttrib(ATTRIB_LOCCODE_CODE, code))
			{
				if (!code.empty())
				{
					wxString locname;
					element->GetAttrib(ATTRIB_LOCCODE_NAME, locname);
					m_Locations[code] = locname;
				}
			}
		}
		else if (name == TREE_VENUECODE)
		{
			// Ignore any errors...
			wxString code;
			if (ElementNode::eNotFound != element->GetAttrib(ATTRIB_VENUECODE_CODE, code))
			{
				if (!code.empty())
				{
					wxString venue;
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
	std::map<wxString, wxString>::const_iterator i;
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


wxString ARBConfigCalSite::GetFormattedURL(
		std::vector<wxString> const& inLocCodes,
		std::vector<wxString> const& inVenueCodes) const
{
	wxString url(m_urlSearch);
	wxString::size_type pos = url.find(wxT("!L!"));
	if (pos != wxString::npos)
	{
		wxString codes;
		for (std::vector<wxString>::const_iterator i = inLocCodes.begin();
			i != inLocCodes.end();
			++i)
		{
			if (!codes.empty())
				codes += wxT("+");
			codes += *i;
		}
		url.replace(pos, 3, codes);
	}
	pos = url.find(wxT("!V!"));
	if (pos != wxString::npos)
	{
		wxString codes;
		for (std::vector<wxString>::const_iterator i = inVenueCodes.begin();
			i != inVenueCodes.end();
			++i)
		{
			if (!codes.empty())
				codes += wxT("+");
			codes += *i;
		}
		url.replace(pos, 3, codes);
	}
	return url;
}


bool ARBConfigCalSite::HasLocationCode(wxString const& inCode) const
{
	return m_Locations.end() != m_Locations.find(inCode);
}


bool ARBConfigCalSite::AddLocationCode(wxString const& inCode, wxString const& inName)
{
	if (HasLocationCode(inCode))
		return false;
	m_Locations[inCode] = inName;
	return true;
}


bool ARBConfigCalSite::RemoveLocationCode(wxString const& inCode)
{
	bool bOk = false;
	std::map<wxString, wxString>::iterator i = m_Locations.find(inCode);
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


bool ARBConfigCalSite::HasVenueCode(wxString const& inCode) const
{
	return m_Venues.end() != m_Venues.find(inCode);
}


bool ARBConfigCalSite::AddVenueCode(wxString const& inCode, wxString const& inVenue)
{
	if (HasVenueCode(inCode))
		return false;
	m_Venues[inCode] = inVenue;
	return true;
}


bool ARBConfigCalSite::RemoveVenueCode(wxString const& inCode)
{
	bool bOk = false;
	std::map<wxString, wxString>::iterator i = m_Venues.find(inCode);
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


class SortConfigCalSite
{
public:
	SortConfigCalSite() {}
	bool operator()(ARBConfigCalSitePtr one, ARBConfigCalSitePtr two) const
	{
		return one->GetName() < two->GetName();
	}
};


void ARBConfigCalSiteList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortConfigCalSite());
}


bool ARBConfigCalSiteList::FindSite(
		wxString const& inSite,
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
		wxString const& inSite,
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


int ARBConfigCalSiteList::DeleteSite(wxString const& inSite)
{
	wxString site(inSite);
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
