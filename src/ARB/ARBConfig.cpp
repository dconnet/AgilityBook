/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @brief The main configuration class.
 * @author David Connet
 *
 * Revision History
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-10-22 DRC Added static GetDTD() method.
 * @li 2003-07-16 DRC Allow the code to keep processing after errors are found.
 */

#include "StdAfx.h"
#ifdef WIN32
#include "resource.h"
#endif
#include "ARBConfig.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfig::ARBConfig()
	: m_Actions()
	, m_Venues()
	, m_FaultTypes()
	, m_OtherPoints()
{
}

ARBConfig::ARBConfig(const ARBConfig& rhs)
	: m_Actions(rhs.m_Actions)
	, m_Venues(rhs.m_Venues)
	, m_FaultTypes(rhs.m_FaultTypes)
	, m_OtherPoints(rhs.m_OtherPoints)
{
}

ARBConfig::~ARBConfig()
{
	clear();
}

ARBConfig& ARBConfig::operator=(const ARBConfig& rhs)
{
	if (this != &rhs)
	{
		m_Actions = rhs.m_Actions;
		m_Venues = rhs.m_Venues;
		m_FaultTypes = rhs.m_FaultTypes;
		m_OtherPoints = rhs.m_OtherPoints;
	}
	return *this;
}

bool ARBConfig::operator==(const ARBConfig& rhs) const
{
	return m_Actions == rhs.m_Actions
		&& m_Venues == rhs.m_Venues
		&& m_FaultTypes == rhs.m_FaultTypes
		&& m_OtherPoints == rhs.m_OtherPoints;
}

bool ARBConfig::operator!=(const ARBConfig& rhs) const
{
	return !operator==(rhs);
}

void ARBConfig::clear()
{
	m_Actions.clear();
	m_Venues.clear();
	m_FaultTypes.clear();
	m_OtherPoints.clear();
}

/**
 * FileVersion 3 moved faults from the venue to the config.
 * This function allows the venue to migrate old file formats.
 */
bool ARBConfig::LoadFault(
	const CElement& inTree,
	const ARBVersion& inVersion,
	std::string& ioErrMsg)
{
	if (inTree.GetName() == TREE_FAULTTYPE
	&& m_FaultTypes.Load(inTree, inVersion, ioErrMsg))
		return true;
	else
		return false;
}

/**
 * FileVersion 3 moved otherpoints from the venue to the config.
 * This function allows the venue to migrate old file formats.
 */
bool ARBConfig::LoadOtherPoints(
	const CElement& inTree,
	const ARBVersion& inVersion,
	std::string& ioErrMsg)
{
	if (inTree.GetName() == TREE_OTHERPTS
	&& m_OtherPoints.Load(inTree, inVersion, ioErrMsg))
		return true;
	else
		return false;
}

bool ARBConfig::Load(
	const CElement& inTree,
	const ARBVersion& inVersion,
	std::string& ioErrMsg)
{
	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		const CElement& element = inTree.GetElement(i);
		const std::string& name = element.GetName();
		if (name == TREE_ACTION)
		{
			// Ignore any errors...
			m_Actions.Load(element, inVersion, ioErrMsg);
		}
		else if (name == TREE_VENUE)
		{
			// Ignore any errors...
			m_Venues.Load(*this, element, inVersion, ioErrMsg);
		}
		else if (name == TREE_FAULTTYPE)
		{
			// Ignore any errors...
			// This was moved here in version 3.
			LoadFault(element, inVersion, ioErrMsg);
		}
		else if (name == TREE_OTHERPTS)
		{
			// Ignore any errors...
			// This was moved here in version 3.
			LoadOtherPoints(element, inVersion, ioErrMsg);
		}
	}
	m_Venues.sort();
	return true;
}

bool ARBConfig::Save(CElement& ioTree) const
{
	CElement& config = ioTree.AddElement(TREE_CONFIG);
	if (!m_Actions.Save(config))
		return false;
	if (!m_Venues.Save(config))
		return false;
	if (!m_FaultTypes.Save(config))
		return false;
	if (!m_OtherPoints.Save(config))
		return false;
	return true;
}

// Add a few known configurations.
void ARBConfig::Default()
{
	clear();
	// We could just distribute the .xml file and load it, but I'm taking
	// advantage of Win32 resources and stashing the default config inside
	// the program.
	bool bOk = false;
	std::string err;
	CElement tree;
#ifdef WIN32
	HRSRC hrSrc = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_XML_DEFAULT_CONFIG), "XML");
	if (hrSrc)
	{
		HGLOBAL hRes = LoadResource(AfxGetResourceHandle(), hrSrc);
		if (hRes)
		{
			DWORD size = SizeofResource(AfxGetResourceHandle(), hrSrc);
			const char* pData = reinterpret_cast<const char*>(LockResource(hRes));
			bOk = tree.LoadXMLBuffer(pData, size, err);
			FreeResource(hRes);
		}
	}
#else
	// @todo: Porting issues: This needs more work...
	// This will work, but we need to make sure DefaultConfig.xml is
	// distributed - there's also the issue of paths...
	bOk = tree.LoadXMLFile("DefaultConfig.xml", err);
#endif
	if (bOk && tree.GetName() == "DefaultConfig")
	{
		ARBVersion version = ARBAgilityRecordBook::GetCurrentDocVersion();
		tree.GetAttrib(ATTRIB_BOOK_VERSION, version);
		int config = tree.FindElement(TREE_CONFIG);
		if (0 <= config)
		{
			bOk = Load(tree.GetElement(config), version, err);
		}
	}
}

/* static */
std::string ARBConfig::GetDTD()
{
	std::string dtd;
#ifdef WIN32
	HRSRC hrSrc = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_DTD_AGILITYRECORDBOOK), "DTD");
	if (hrSrc)
	{
		HGLOBAL hRes = LoadResource(AfxGetResourceHandle(), hrSrc);
		if (hRes)
		{
			DWORD size = SizeofResource(AfxGetResourceHandle(), hrSrc);
			const char* pData = reinterpret_cast<const char*>(LockResource(hRes));
			dtd = std::string(pData, size);
			FreeResource(hRes);
		}
	}
#else
	// @todo: Porting issues: Not currently implemented
	dtd = "<!-- Not implemented on non-win32 platforms -->\n";
#endif
	return dtd;
}

std::string ARBConfig::Update(int indent, const ARBConfig& inConfigNew)
{
	char buffer[1000];
	std::string info;

	int nChanges = 0;
	// Update Faults.
	// Simply merge new ones.
	int nNew = 0;
	int nUpdated = 0;
	int nSkipped = 0;
	for (ARBConfigFaultList::const_iterator iterFaults = inConfigNew.GetFaults().begin();
	iterFaults != inConfigNew.GetFaults().end();
	++iterFaults)
	{
		if (!GetFaults().FindFault((*iterFaults)->GetName()))
		{
			++nNew;
			++nChanges;
			GetFaults().AddFault((*iterFaults)->GetName());
		}
		else
			++nSkipped;
	}
	sprintf(buffer, UPDATE_FORMAT_FAULTS, nNew, nSkipped);
	info += buffer;

	// Update OtherPoints.
	nNew = 0;
	nUpdated = 0;
	nSkipped = 0;
	for (ARBConfigOtherPointsList::const_iterator iterOther = inConfigNew.GetOtherPoints().begin();
	iterOther != inConfigNew.GetOtherPoints().end();
	++iterOther)
	{
		ARBConfigOtherPoints* pOther = GetOtherPoints().FindOtherPoints((*iterOther)->GetName());
		if (pOther)
		{
			if (*pOther != *(*iterOther))
			{
				++nUpdated;
				++nChanges;
				// To update OtherPoints, simply replace. The Name is the
				// matching item, so the other fields can safely be updated.
				*pOther = *(*iterOther);
			}
			else
				++nSkipped;
		}
		else
		{
			++nNew;
			++nChanges;
			GetOtherPoints().AddOtherPoints((*iterOther));
		}
	}
	sprintf(buffer, UPDATE_FORMAT_OTHERPTS, nNew, nUpdated, nSkipped);
	info += buffer;

	// Update Venues.
	nNew = 0;
	nUpdated = 0;
	nSkipped = 0;
	std::string venueInfo;
	for (ARBConfigVenueList::const_iterator iterVenue = inConfigNew.GetVenues().begin();
	iterVenue != inConfigNew.GetVenues().end();
	++iterVenue)
	{
		ARBConfigVenue* pVenue = GetVenues().FindVenue((*iterVenue)->GetName());
		if (pVenue)
		{
			if (*pVenue != *(*iterVenue))
			{
				++nUpdated;
				++nChanges;
				venueInfo += pVenue->Update(indent+1, (*iterVenue));
			}
			else
				++nSkipped;
		}
		else
		{
			++nNew;
			++nChanges;
			GetVenues().AddVenue((*iterVenue));
			venueInfo += "+";
			venueInfo += (*iterVenue)->GetName();
			venueInfo += "\n";
		}
	}
	sprintf(buffer, UPDATE_FORMAT_VENUES, nNew, nUpdated, nSkipped);
	info += buffer;
	if (0 < venueInfo.length())
	{
		info += "\n";
		info += venueInfo;
	}
	if (0 == nChanges)
		info.erase();
	return info;
}
