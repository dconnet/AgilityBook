/*
 * Copyright © 2007-2008 David Connet. All Rights Reserved.
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
 * @brief ARBConfigCalSite
 * @author David Connet
 *
 * Revision History
 * @li 2007-08-02 DRC Created
 */

#include "stdafx.h"
#include "ARBConfigCalSite.h"

#include <algorithm>
#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
	ASSERT(inTree);
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
		tstring const& name = element->GetName();
		if (name == TREE_CALSITE_DESC)
		{
			m_Desc = element->GetValue();
		}
		else if (name == TREE_LOCCODE)
		{
			// Ignore any errors...
			tstring code;
			if (ElementNode::eNotFound != element->GetAttrib(ATTRIB_LOCCODE_CODE, code))
			{
				if (!code.empty())
				{
					tstring locname;
					element->GetAttrib(ATTRIB_LOCCODE_NAME, locname);
					m_Locations[code] = locname;
				}
			}
		}
		else if (name == TREE_VENUECODE)
		{
			// Ignore any errors...
			tstring code;
			if (ElementNode::eNotFound != element->GetAttrib(ATTRIB_VENUECODE_CODE, code))
			{
				if (!code.empty())
				{
					tstring venue;
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
	ASSERT(ioTree);
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
	std::map<tstring, tstring>::const_iterator i;
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


tstring ARBConfigCalSite::GetFormattedURL(
		std::vector<tstring> const& inLocCodes,
		std::vector<tstring> const& inVenueCodes) const
{
	tstring url(m_urlSearch);
	tstring::size_type pos = url.find(_T("!L!"));
	if (pos != tstring::npos)
	{
		tstring codes;
		for (std::vector<tstring>::const_iterator i = inLocCodes.begin();
			i != inLocCodes.end();
			++i)
		{
			if (!codes.empty())
				codes += _T("+");
			codes += *i;
		}
		url.replace(pos, 3, codes);
	}
	pos = url.find(_T("!V!"));
	if (pos != tstring::npos)
	{
		tstring codes;
		for (std::vector<tstring>::const_iterator i = inVenueCodes.begin();
			i != inVenueCodes.end();
			++i)
		{
			if (!codes.empty())
				codes += _T("+");
			codes += *i;
		}
		url.replace(pos, 3, codes);
	}
	return url;
}


bool ARBConfigCalSite::HasLocationCode(tstring const& inCode) const
{
	return m_Locations.end() != m_Locations.find(inCode);
}


bool ARBConfigCalSite::AddLocationCode(tstring const& inCode, tstring const& inName)
{
	if (HasLocationCode(inCode))
		return false;
	m_Locations[inCode] = inName;
	return true;
}


bool ARBConfigCalSite::RemoveLocationCode(tstring const& inCode)
{
	bool bOk = false;
	std::map<tstring, tstring>::iterator i = m_Locations.find(inCode);
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


bool ARBConfigCalSite::HasVenueCode(tstring const& inCode) const
{
	return m_Venues.end() != m_Venues.find(inCode);
}


bool ARBConfigCalSite::AddVenueCode(tstring const& inCode, tstring const& inVenue)
{
	if (HasVenueCode(inCode))
		return false;
	m_Venues[inCode] = inVenue;
	return true;
}


bool ARBConfigCalSite::RemoveVenueCode(tstring const& inCode)
{
	bool bOk = false;
	std::map<tstring, tstring>::iterator i = m_Venues.find(inCode);
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
		tstring const& inSite,
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
		tstring const& inSite,
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


int ARBConfigCalSiteList::DeleteSite(tstring const& inSite)
{
	tstring site(inSite);
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
