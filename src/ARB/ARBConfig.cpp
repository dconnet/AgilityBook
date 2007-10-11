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
 * @brief The main configuration class.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-10-26 DRC Added option to prevent auto-update user query.
 * @li 2005-01-10 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-12-18 DRC Don't set version number lower when merging.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-02-26 DRC Added version number to configuration.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-10-22 DRC Added static GetDTD() method.
 * @li 2003-07-16 DRC Allow the code to keep processing after errors are found.
 */

#include "StdAfx.h"
#ifdef _WINDOWS
#include "resource.h"
#endif
#include "ARBConfig.h"

#include "ARBAgilityRecordBook.h"
#include "ARBDogTitle.h"
#include "ARBLocalization.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfig::ARBConfig()
	: m_bUpdate(true)
	, m_Version(0)
	, m_CalSites()
	, m_Actions()
	, m_Venues()
	, m_FaultTypes()
	, m_OtherPoints()
{
}


ARBConfig::ARBConfig(ARBConfig const& rhs)
	: m_bUpdate(rhs.m_bUpdate)
	, m_Version(rhs.m_Version)
	, m_CalSites(rhs.m_CalSites)
	, m_Actions()
	, m_Venues()
	, m_FaultTypes()
	, m_OtherPoints()
{
	rhs.m_Actions.Clone(m_Actions);
	rhs.m_Venues.Clone(m_Venues);
	rhs.m_FaultTypes.Clone(m_FaultTypes);
	rhs.m_OtherPoints.Clone(m_OtherPoints);
}


ARBConfig::~ARBConfig()
{
	clear();
}


/*
ARBConfig& ARBConfig::operator=(ARBConfig const& rhs)
{
	if (this != &rhs)
	{
		m_bUpdate = rhs.m_bUpdate;
		m_Version = rhs.m_Version;
		rhs.m_Actions.Clone(m_Actions);
		rhs.m_Venues.Clone(m_Venues);
		rhs.m_FaultTypes.Clone(m_FaultTypes);
		rhs.m_OtherPoints.Clone(m_OtherPoints);
	}
	return *this;
}
*/


bool ARBConfig::operator==(ARBConfig const& rhs) const
{
	// Equality does not include actions or the update flag
	return m_Version == rhs.m_Version
		&& m_CalSites == rhs.m_CalSites
		&& m_Venues == rhs.m_Venues
		&& m_FaultTypes == rhs.m_FaultTypes
		&& m_OtherPoints == rhs.m_OtherPoints;
}


void ARBConfig::clear()
{
	m_bUpdate = true;
	m_Version = 0;
	m_CalSites.clear();
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
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ASSERT(inTree);
	if (!inTree)
		return false;
	if (inTree->GetName() == TREE_FAULTTYPE
	&& m_FaultTypes.Load(inTree, inVersion, ioCallback))
		return true;
	else
		return false;
}


/**
 * FileVersion 3 moved otherpoints from the venue to the config.
 * This function allows the venue to migrate old file formats.
 */
bool ARBConfig::LoadOtherPoints(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ASSERT(inTree);
	if (!inTree)
		return false;
	if (inTree->GetName() == TREE_OTHERPTS
	&& m_OtherPoints.Load(inTree, inVersion, ioCallback))
		return true;
	else
		return false;
}


bool ARBConfig::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ASSERT(inTree);
	if (!inTree)
		return false;
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_CONFIG_UPDATE, m_bUpdate))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_CONFIG, ATTRIB_CONFIG_UPDATE, Localization()->ValidValuesBool().c_str()));
		return false;
	}
	inTree->GetAttrib(ATTRIB_CONFIG_VERSION, m_Version);
	for (int i = 0; i < inTree->GetElementCount(); ++i)
	{
		ElementNodePtr element = inTree->GetElementNode(i);
		if (!element)
			continue;
		tstring const& name = element->GetName();
		if (name == TREE_ACTION)
		{
			// Ignore any errors...
			m_Actions.Load(element, inVersion, ioCallback);
		}
		else if (name == TREE_CALSITE)
		{
			ARBConfigCalSite site;
			if (site.Load(element, inVersion, ioCallback))
				m_CalSites.push_back(site);
		}
		else if (name == TREE_VENUE)
		{
			// Ignore any errors...
			m_Venues.Load(*this, element, inVersion, ioCallback);
		}
		else if (name == TREE_FAULTTYPE)
		{
			// Ignore any errors...
			// This was moved here in version 3.
			LoadFault(element, inVersion, ioCallback);
		}
		else if (name == TREE_OTHERPTS)
		{
			// Ignore any errors...
			// This was moved here in version 3.
			LoadOtherPoints(element, inVersion, ioCallback);
		}
	}
	m_Venues.sort();
	return true;
}


bool ARBConfig::Save(ElementNodePtr ioTree) const
{
	ASSERT(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr config = ioTree->AddElementNode(TREE_CONFIG);
	if (!m_bUpdate)
		config->AddAttrib(ATTRIB_CONFIG_UPDATE, m_bUpdate);
	config->AddAttrib(ATTRIB_CONFIG_VERSION, m_Version);
	for (std::vector<ARBConfigCalSite>::const_iterator i = m_CalSites.begin();
		i != m_CalSites.end();
		++i)
	{
		if (!(*i).Save(config))
			return false;
	}
	// Do not save actions - Actions are done only when loading/merging a
	// configuration. Keeping them around could cause possible future problems.
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
	tstring errMsg;
	ARBErrorCallback err(errMsg);
	ElementNodePtr tree(ElementNode::New());
#ifdef _WINDOWS
	HRSRC hrSrc = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_XML_DEFAULT_CONFIG), _T("XML"));
	if (hrSrc)
	{
		HGLOBAL hRes = LoadResource(AfxGetResourceHandle(), hrSrc);
		if (hRes)
		{
			DWORD size = SizeofResource(AfxGetResourceHandle(), hrSrc);
			char const* pData = reinterpret_cast<char const*>(LockResource(hRes));
			bOk = tree->LoadXMLBuffer(pData, size, errMsg);
			FreeResource(hRes);
		}
	}
#else
	// @todo: Porting issues: This needs more work...
	// This will work, but we need to make sure DefaultConfig.xml is
	// distributed - there's also the issue of paths...
	bOk = tree->LoadXMLFile(_T("DefaultConfig.xml"), errMsg);
#endif
	if (bOk && tree->GetName() == _T("DefaultConfig"))
	{
		ARBVersion version = ARBAgilityRecordBook::GetCurrentDocVersion();
		tree->GetAttrib(ATTRIB_BOOK_VERSION, version);
		int config = tree->FindElement(TREE_CONFIG);
		if (0 <= config)
		{
			bOk = Load(tree->GetElementNode(config), version, err);
		}
	}
}


/* static */
std::string ARBConfig::GetDTD(bool bNormalizeCRNL)
{
	std::string dtd;
#ifdef _WINDOWS
	HRSRC hrSrc = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_DTD_AGILITYRECORDBOOK), _T("DTD"));
	if (hrSrc)
	{
		HGLOBAL hRes = LoadResource(AfxGetResourceHandle(), hrSrc);
		if (hRes)
		{
			DWORD size = SizeofResource(AfxGetResourceHandle(), hrSrc);
			char const* pData = reinterpret_cast<char const*>(LockResource(hRes));
			CStringA data(pData, size);
			if (bNormalizeCRNL)
				data.Replace("\r\n", "\n");
			dtd = std::string(data, data.GetLength());
			FreeResource(hRes);
		}
	}
#else
	// @todo: Porting issues: Not currently implemented
	dtd = "<!-- Not implemented on non-windows platforms -->\n";
#endif
	return dtd;
}


tstring ARBConfig::GetTitleNiceName(
		tstring const& inVenue,
		tstring const& inTitle) const
{
	ARBConfigTitlePtr pTitle;
	if (m_Venues.FindTitle(inVenue, inTitle, &pTitle))
	{
		tstring name = pTitle->GetNiceName();
		return name;
	}
	else
		return inTitle;
}


tstring ARBConfig::GetTitleCompleteName(
		ARBDogTitlePtr inTitle,
		bool bAbbrevFirst) const
{
	if (!inTitle)
		return _T("");
	ARBConfigTitlePtr pTitle;
	if (m_Venues.FindTitle(inTitle->GetVenue(), inTitle->GetRawName(), &pTitle))
		return pTitle->GetCompleteName(inTitle->GetInstance(), inTitle->ShowInstanceOne(), bAbbrevFirst);
	else
		return inTitle->GetGenericName();
}


bool ARBConfig::Update(
		int indent,
		ARBConfig const& inConfigNew,
		otstringstream& ioInfo)
{
	int nChanges = 0; // A simple count of changes that have occurred.
	// Update CalSites.
	int nNew = 0;
	int nUpdated = 0;
	int nSkipped = 0;
	{
		int nDeleted = 0;
		std::vector<ARBConfigCalSite>::iterator iter1;
		std::vector<ARBConfigCalSite>::const_iterator iter2;
		// Look for items that will be removed.
		for (iter1 = m_CalSites.begin(); iter1 != m_CalSites.end(); ++iter1)
		{
			bool bFound = false;
			for (iter2 = inConfigNew.GetCalSites().begin(); iter2 != inConfigNew.GetCalSites().end(); ++iter2)
			{
				if (*iter1 == *iter2)
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
		for (iter2 = inConfigNew.GetCalSites().begin(); iter2 != inConfigNew.GetCalSites().end(); ++iter2)
		{
			bool bFound = false;
			for (iter1 = m_CalSites.begin(); iter1 != m_CalSites.end(); ++iter1)
			{
				if (*iter1 == *iter2)
				{
					bFound = true;
					break;
				}
			}
			if (!bFound)
				++nNew;
		}
		m_CalSites = inConfigNew.GetCalSites();
		if (0 < nNew || 0 < nDeleted)
		{
			ioInfo << Localization()->UpdateCalSites(nNew, nDeleted, nSkipped) << _T("\n");
		}
	}

	// Update Faults.
	// UI maintains a sorted order, so we don't need to worry about list order.
	// Simply merge new ones.
	nNew = 0;
	nUpdated = 0;
	nSkipped = 0;
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
	if (0 < nNew || 0 < nChanges)
	{
		ioInfo << Localization()->UpdateFaults(nNew, nSkipped) << _T("\n");
	}

	// Update OtherPoints.
	// UI maintains a sorted order, so we don't need to worry about list order.
	nNew = 0;
	nUpdated = 0;
	nSkipped = 0;
	for (ARBConfigOtherPointsList::const_iterator iterOther = inConfigNew.GetOtherPoints().begin();
	iterOther != inConfigNew.GetOtherPoints().end();
	++iterOther)
	{
		ARBConfigOtherPointsPtr pOther;
		if (GetOtherPoints().FindOtherPoints((*iterOther)->GetName(), &pOther))
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
	if (0 < nNew || 0 < nChanges)
	{
		ioInfo << Localization()->UpdateOtherPts(nNew, nUpdated, nSkipped) << _T("\n");
	}

	// Update Venues.
	// UI maintains a sorted order, so we don't need to worry about list order.
	nNew = 0;
	nUpdated = 0;
	nSkipped = 0;
	tstring venueInfo;
	for (ARBConfigVenueList::const_iterator iterVenue = inConfigNew.GetVenues().begin();
	iterVenue != inConfigNew.GetVenues().end();
	++iterVenue)
	{
		ARBConfigVenuePtr pVenue;
		if (GetVenues().FindVenue((*iterVenue)->GetName(), &pVenue))
		{
			if (*pVenue != *(*iterVenue))
			{
				if (pVenue->Update(indent+1, (*iterVenue), venueInfo))
				{
					++nUpdated;
					++nChanges;
				}
			}
			else
				++nSkipped;
		}
		else
		{
			++nNew;
			++nChanges;
			GetVenues().AddVenue((*iterVenue));
			venueInfo += _T("+");
			venueInfo += (*iterVenue)->GetName();
			venueInfo += _T("\n");
		}
	}
	if (0 < nNew || 0 < nChanges)
	{
		ioInfo << Localization()->UpdateVenues(nNew, nUpdated, nSkipped) << _T("\n");
	}
	if (0 < venueInfo.length())
	{
		ioInfo << venueInfo;
	}
	// Even if there are no changes, update the version number so we don't
	// prompt anymore.
	if (m_Version < inConfigNew.GetVersion())
		m_Version = inConfigNew.GetVersion();
	bool bChanges = true;
	if (0 == nChanges)
	{
		bChanges = false;
	}
	else
	{
		m_bUpdate = true;
	}
	return bChanges;
}
