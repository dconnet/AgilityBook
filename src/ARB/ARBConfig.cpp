/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief The main configuration class.
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2015-11-01 Added ARBVersion to Default().
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-10-26 Added option to prevent auto-update user query.
 * 2005-01-10 Allow titles to be optionally entered multiple times.
 * 2004-12-18 Don't set version number lower when merging.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-02-26 Added version number to configuration.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-10-22 Added static GetDTD() method.
 * 2003-07-16 Allow the code to keep processing after errors are found.
 */

#include "stdafx.h"
#include "ARB/ARBConfig.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBDogTitle.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
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


ARBConfig::ARBConfig(ARBConfig&& rhs)
	: m_bUpdate(std::move(rhs.m_bUpdate))
	, m_Version(std::move(rhs.m_Version))
	, m_CalSites(std::move(rhs.m_CalSites))
	, m_Actions(std::move(rhs.m_Actions))
	, m_Venues(std::move(rhs.m_Venues))
	, m_FaultTypes(std::move(rhs.m_FaultTypes))
	, m_OtherPoints(std::move(rhs.m_OtherPoints))
{
}


ARBConfig::~ARBConfig()
{
	clear();
}


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
		ElementNodePtr const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
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
		ElementNodePtr const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (!inTree)
		return false;
	if (inTree->GetName() == TREE_OTHERPTS
	&& m_OtherPoints.Load(inTree, inVersion, ioCallback))
		return true;
	else
		return false;
}


bool ARBConfig::Load(
		ElementNodePtr const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_CONFIG)
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
		std::wstring const& name = element->GetName();
		if (name == TREE_ACTION)
		{
			// Ignore any errors...
			m_Actions.Load(element, inVersion, ioCallback);
		}
		else if (name == TREE_CALSITE)
		{
			// Ignore any errors...
			m_CalSites.Load(element, inVersion, ioCallback);
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
		else
		{
			// This can happen when editing DefaultConfig.xml
			assert(!"Unexpected element");
		}
	}
	m_Venues.sort();
	return true;
}


bool ARBConfig::Save(ElementNodePtr const& ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr config = ioTree->AddElementNode(TREE_CONFIG);
	if (!m_bUpdate)
		config->AddAttrib(ATTRIB_CONFIG_UPDATE, m_bUpdate);
	config->AddAttrib(ATTRIB_CONFIG_VERSION, m_Version);
	// Do not save actions - Actions are done only when loading/merging a
	// configuration. Keeping them around could cause possible future problems.
	if (!m_CalSites.Save(config))
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
void ARBConfig::Default(IARBConfigHandler* inHandler, ARBVersion* pVersion)
{
	clear();
	if (pVersion)
		pVersion->clear();
	if (inHandler)
	{
		ElementNodePtr tree = inHandler->LoadDefaultConfig();
		if (tree && tree->GetName() == L"DefaultConfig")
		{
			ARBVersion version = ARBAgilityRecordBook::GetCurrentDocVersion();
			if (pVersion)
				*pVersion = version;
			tree->GetAttrib(ATTRIB_BOOK_VERSION, version);
			int config = tree->FindElement(TREE_CONFIG);
			if (0 <= config)
			{
				fmt::wmemory_buffer errMsg;
				ARBErrorCallback err(errMsg);
				Load(tree->GetElementNode(config), version, err);
			}
		}
	}
}


/* static */
std::string ARBConfig::GetDTD(IARBConfigHandler* inHandler)
{
	if (inHandler)
		return inHandler->LoadDTD();
	return std::string();
}


std::wstring ARBConfig::GetTitleNiceName(
		std::wstring const& inVenue,
		std::wstring const& inTitle) const
{
	ARBConfigTitlePtr pTitle;
	if (m_Venues.FindTitle(inVenue, inTitle, &pTitle))
	{
		std::wstring name = pTitle->GetNiceName();
		return name;
	}
	else
		return inTitle;
}


std::wstring ARBConfig::GetTitleCompleteName(
		ARBDogTitlePtr const& inTitle,
		bool bAbbrevFirst) const
{
	if (!inTitle)
		return std::wstring();
	ARBConfigTitlePtr pTitle;
	if (m_Venues.FindTitle(inTitle->GetVenue(), inTitle->GetRawName(), &pTitle))
		return pTitle->GetCompleteName(inTitle->GetInstance(), bAbbrevFirst);
	else
		return inTitle->GetGenericName();
}


bool ARBConfig::Update(
		int indent,
		ARBConfig const& inConfigNew,
		fmt::wmemory_buffer& ioInfo)
{
	int nChanges = 0; // A simple count of changes that have occurred.
	// Update CalSites. (existing ones are never removed except by an action)
	// Maintained in sorted order.
	int nNew = 0;
	int nUpdated = 0;
	int nSkipped = 0;
	{
		for (ARBConfigCalSiteList::const_iterator iterCalSite = inConfigNew.GetCalSites().begin();
			iterCalSite != inConfigNew.GetCalSites().end();
			++iterCalSite)
		{
			ARBConfigCalSitePtr site;
			if (m_CalSites.FindSite((*iterCalSite)->GetName(), &site))
			{
				if (*site != *(*iterCalSite))
				{
					++nUpdated;
					*site = *(*iterCalSite);
				}
				else
					++nSkipped;
			}
			else
			{
				++nNew;
				m_CalSites.push_back((*iterCalSite)->Clone());
			}
		}
		if (0 < nNew || 0 < nUpdated)
		{
			fmt::format_to(ioInfo, L"{}\n", Localization()->UpdateCalSites(nNew, nUpdated, nSkipped));
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
		fmt::format_to(ioInfo, L"{}\n", Localization()->UpdateFaults(nNew, nSkipped));
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
		fmt::format_to(ioInfo, L"{}\n", Localization()->UpdateOtherPts(nNew, nUpdated, nSkipped));
	}

	// Update Venues.
	// Maintained in sorted order.
	nNew = 0;
	nUpdated = 0;
	nSkipped = 0;
	std::wstring venueInfo;
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
			venueInfo += L"+";
			venueInfo += (*iterVenue)->GetName();
			venueInfo += L"\n";
		}
	}
	if (0 < nNew || 0 < nChanges)
	{
		fmt::format_to(ioInfo, L"{}\n", Localization()->UpdateVenues(nNew, nUpdated, nSkipped));
	}
	if (0 < venueInfo.length())
	{
		fmt::format_to(ioInfo, L"{}", venueInfo);
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
