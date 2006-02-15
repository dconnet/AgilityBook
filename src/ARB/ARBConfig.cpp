/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
#ifdef WIN32
#include "resource.h"
#endif
#include "ARBConfig.h"

#include "ARBAgilityRecordBook.h"
#include "ARBDogTitle.h"
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
	, m_Actions()
	, m_Venues()
	, m_FaultTypes()
	, m_OtherPoints()
{
}

ARBConfig::ARBConfig(ARBConfig const& rhs)
	: m_bUpdate(rhs.m_bUpdate)
	, m_Version(rhs.m_Version)
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

bool ARBConfig::operator==(ARBConfig const& rhs) const
{
	return m_bUpdate == rhs.m_bUpdate
		&& m_Version == rhs.m_Version
		&& m_Actions == rhs.m_Actions
		&& m_Venues == rhs.m_Venues
		&& m_FaultTypes == rhs.m_FaultTypes
		&& m_OtherPoints == rhs.m_OtherPoints;
}

bool ARBConfig::operator!=(ARBConfig const& rhs) const
{
	return !operator==(rhs);
}

void ARBConfig::clear()
{
	m_bUpdate = true;
	m_Version = 0;
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
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (inTree.GetName() == TREE_FAULTTYPE
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
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (inTree.GetName() == TREE_OTHERPTS
	&& m_OtherPoints.Load(inTree, inVersion, ioCallback))
		return true;
	else
		return false;
}

bool ARBConfig::Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_CONFIG_UPDATE, m_bUpdate))
	{
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_CONFIG, ATTRIB_CONFIG_UPDATE, VALID_VALUES_BOOL));
		return false;
	}
	inTree.GetAttrib(ATTRIB_CONFIG_VERSION, m_Version);
	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		Element const& element = inTree.GetElement(i);
		ARBString const& name = element.GetName();
		if (name == TREE_ACTION)
		{
			// Ignore any errors...
			m_Actions.Load(element, inVersion, ioCallback);
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

bool ARBConfig::Save(Element& ioTree) const
{
	Element& config = ioTree.AddElement(TREE_CONFIG);
	if (!m_bUpdate)
		config.AddAttrib(ATTRIB_CONFIG_UPDATE, m_bUpdate);
	config.AddAttrib(ATTRIB_CONFIG_VERSION, m_Version);
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
	ARBString errMsg;
	ARBErrorCallback err(errMsg);
	Element tree;
#ifdef WIN32
	HRSRC hrSrc = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_XML_DEFAULT_CONFIG), _T("XML"));
	if (hrSrc)
	{
		HGLOBAL hRes = LoadResource(AfxGetResourceHandle(), hrSrc);
		if (hRes)
		{
			DWORD size = SizeofResource(AfxGetResourceHandle(), hrSrc);
			char const* pData = reinterpret_cast<char const*>(LockResource(hRes));
			bOk = tree.LoadXMLBuffer(pData, size, errMsg);
			FreeResource(hRes);
		}
	}
#else
	// @todo: Porting issues: This needs more work...
	// This will work, but we need to make sure DefaultConfig.xml is
	// distributed - there's also the issue of paths...
	bOk = tree.LoadXMLFile(_T("DefaultConfig.xml"), errMsg);
#endif
	if (bOk && tree.GetName() == _T("DefaultConfig"))
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
ARBString ARBConfig::GetDTD(bool bNormalizeCRNL)
{
	ARBString dtd;
#ifdef WIN32
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
#ifdef UNICODE
			dtd = CString(data);
#else
			dtd = ARBString(data, data.GetLength());
#endif
			FreeResource(hRes);
		}
	}
#else
	// @todo: Porting issues: Not currently implemented
	dtd = _T("<!-- Not implemented on non-win32 platforms -->\n");
#endif
	return dtd;
}

ARBString ARBConfig::GetTitleNiceName(
		ARBString const& inVenue,
		ARBString const& inTitle) const
{
	ARBConfigTitlePtr pTitle;
	if (m_Venues.FindTitle(inVenue, inTitle, &pTitle))
	{
		ARBString name = pTitle->GetNiceName();
		return name;
	}
	else
		return inTitle;
}

ARBString ARBConfig::GetTitleCompleteName(
		ARBDogTitlePtr inTitle,
		bool bAbbrevFirst) const
{
	if (!inTitle)
		return _T("");
	ARBConfigTitlePtr pTitle;
	if (m_Venues.FindTitle(inTitle->GetVenue(), inTitle->GetRawName(), &pTitle))
		return pTitle->GetCompleteName(inTitle->GetInstance(), bAbbrevFirst);
	else
		return inTitle->GetGenericName();
}

bool ARBConfig::Update(
		int indent,
		ARBConfig const& inConfigNew,
		ARBString& ioInfo)
{
	ARBString info;

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
	if (0 < nNew || 0 < nChanges)
	{
		info += UPDATE_FORMAT_FAULTS(nNew, nSkipped);
	}

	// Update OtherPoints.
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
		info += UPDATE_FORMAT_OTHERPTS(nNew, nUpdated, nSkipped);
	}

	// Update Venues.
	nNew = 0;
	nUpdated = 0;
	nSkipped = 0;
	ARBString venueInfo;
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
		info += UPDATE_FORMAT_VENUES(nNew, nUpdated, nSkipped);
	}
	if (0 < venueInfo.length())
	{
		info += _T("\n");
		info += venueInfo;
	}
	// Even if there are no changes, update the version number so we don't
	// prompt anymore.
	if (m_Version < inConfigNew.GetVersion())
		m_Version = inConfigNew.GetVersion();
	bool bChanges = true;
	if (0 == nChanges)
	{
		bChanges = false;
		info.erase();
	}
	else
	{
		m_bUpdate = true;
		ioInfo += info;
	}
	return bChanges;
}
