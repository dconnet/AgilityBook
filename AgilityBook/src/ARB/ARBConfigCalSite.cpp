/*
 * Copyright © 2007 David Connet. All Rights Reserved.
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

#include "StdAfx.h"
#include "ARBConfigCalSite.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigCalSite::ARBConfigCalSite()
	: m_urlSearch()
	, m_urlHelp()
	, m_Locations()
	, m_Venues()
{
}

ARBConfigCalSite::ARBConfigCalSite(ARBConfigCalSite const& rhs)
	: m_urlSearch(rhs.m_urlSearch)
	, m_urlHelp(rhs.m_urlHelp)
	, m_Locations(rhs.m_Locations)
	, m_Venues(rhs.m_Venues)
{
}

ARBConfigCalSite::~ARBConfigCalSite()
{
}

ARBConfigCalSite& ARBConfigCalSite::operator=(ARBConfigCalSite const& rhs)
{
	if (this != &rhs)
	{
		m_urlSearch = rhs.m_urlSearch;
		m_urlHelp = rhs.m_urlHelp;
		m_Locations = rhs.m_Locations;
		m_Venues = rhs.m_Venues;
	}
	return *this;
}

bool ARBConfigCalSite::operator==(ARBConfigCalSite const& rhs) const
{
	return m_urlSearch == rhs.m_urlSearch
		&& m_urlHelp == rhs.m_urlHelp
		&& m_Locations == rhs.m_Locations
		&& m_Venues == rhs.m_Venues;
}

bool ARBConfigCalSite::Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (Element::eNotFound == inTree.GetAttrib(ATTRIB_CALSITE_SEARCH, m_urlSearch))
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_CALSITE, ATTRIB_CALSITE_SEARCH));
		return false;
	}
	inTree.GetAttrib(ATTRIB_CALSITE_HELP, m_urlHelp);

	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		Element const& element = inTree.GetElement(i);
		ARBString const& name = element.GetName();
		if (name == TREE_LOCCODE)
		{
			// Ignore any errors...
			ARBString code;
			if (Element::eNotFound != element.GetAttrib(ATTRIB_LOCCODE_CODE, code))
			{
				if (!code.empty())
				{
					ARBString name;
					element.GetAttrib(ATTRIB_LOCCODE_NAME, name);
					m_Locations[code] = name;
				}
			}
		}
		else if (name == TREE_VENUECODE)
		{
			// Ignore any errors...
			ARBString code;
			if (Element::eNotFound != element.GetAttrib(ATTRIB_VENUECODE_CODE, code))
			{
				if (!code.empty())
				{
					ARBString venue;
					element.GetAttrib(ATTRIB_VENUECODE_VENUE, venue);
					m_Locations[code] = venue;
				}
			}
		}
	}
	return true;
}

bool ARBConfigCalSite::Save(Element& ioTree) const
{
	Element& calsite = ioTree.AddElement(TREE_CALSITE);
	calsite.AddAttrib(ATTRIB_CALSITE_SEARCH, m_urlSearch);
	if (!m_urlHelp.empty())
		calsite.AddAttrib(ATTRIB_CALSITE_HELP, m_urlHelp);
	std::map<ARBString, ARBString>::const_iterator i;
	for (i = m_Locations.begin(); i != m_Locations.end(); ++i)
	{
		Element& loc = calsite.AddElement(TREE_LOCCODE);
		loc.AddAttrib(ATTRIB_LOCCODE_CODE, i->first);
		loc.AddAttrib(ATTRIB_LOCCODE_NAME, i->second);
	}
	for (i = m_Venues.begin(); i != m_Venues.end(); ++i)
	{
		Element& loc = calsite.AddElement(TREE_VENUECODE);
		loc.AddAttrib(ATTRIB_VENUECODE_CODE, i->first);
		if (!i->second.empty())
			loc.AddAttrib(ATTRIB_VENUECODE_VENUE, i->second);
	}
	return true;
}

ARBString ARBConfigCalSite::GetFormattedURL(
		std::vector<ARBString> const& inLocCodes,
		std::vector<ARBString> const& inVenueCodes) const
{
	ARBString url(m_urlSearch);
	ARBString::size_type pos = url.find(_T("!L!"));
	if (pos != ARBString::npos)
	{
		ARBString codes;
		for (std::vector<ARBString>::const_iterator i = inLocCodes.begin();
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
	if (pos != ARBString::npos)
	{
		ARBString codes;
		for (std::vector<ARBString>::const_iterator i = inVenueCodes.begin();
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

bool ARBConfigCalSite::HasLocationCode(ARBString const& inCode) const
{
	return m_Locations.end() != m_Locations.find(inCode);
}

bool ARBConfigCalSite::AddLocationCode(ARBString const& inCode, ARBString const& inName)
{
	if (HasLocationCode(inCode))
		return false;
	m_Locations[inCode] = inName;
	return true;
}

bool ARBConfigCalSite::RemoveLocationCode(ARBString const& inCode)
{
	bool bOk = false;
	std::map<ARBString, ARBString>::iterator i = m_Locations.find(inCode);
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

bool ARBConfigCalSite::HasVenueCode(ARBString const& inCode) const
{
	return m_Venues.end() != m_Venues.find(inCode);
}

bool ARBConfigCalSite::AddVenueCode(ARBString const& inCode, ARBString const& inVenue)
{
	if (HasVenueCode(inCode))
		return false;
	m_Venues[inCode] = inVenue;
	return true;
}

bool ARBConfigCalSite::RemoveVenueCode(ARBString const& inCode)
{
	bool bOk = false;
	std::map<ARBString, ARBString>::iterator i = m_Venues.find(inCode);
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
