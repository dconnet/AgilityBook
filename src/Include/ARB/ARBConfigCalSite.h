#pragma once

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

#include "ARB/ARBTypes2.h"
#include <map>
#include <vector>


/**
 * Configuration information for auto-loading calendar entries from the web.
 */
class ARBConfigCalSite
{
protected:
	ARBConfigCalSite();
	ARBConfigCalSite(ARBConfigCalSite const& rhs);

public:
	~ARBConfigCalSite();
	static ARBConfigCalSitePtr New();
	ARBConfigCalSitePtr Clone() const;

	ARBConfigCalSite& operator=(ARBConfigCalSite const& rhs);
	bool operator==(ARBConfigCalSite const& rhs) const;
	bool operator!=(ARBConfigCalSite const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Load a calendar configuration.
	 * @pre inTree is the actual ARBConfigCalSite element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ElementNodePtr const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 */
	bool Save(ElementNodePtr const& ioTree) const;

	/**
	 * Get the formatted URL to use to request data.
	 */
	std::wstring GetFormattedURL(
			std::vector<std::wstring> const& inLocCodes,
			std::vector<std::wstring> const& inVenueCodes) const;

	/*
	 * Getters/setters.
	 */
	std::wstring GetName() const
	{
		return m_Name;
	}
	void SetName(std::wstring const& inName)
	{
		m_Name = inName;
	}
	std::wstring GetDescription() const
	{
		return m_Desc;
	}
	void SetDescription(std::wstring const& inDesc)
	{
		m_Desc = inDesc;
	}
	std::wstring GetSearchURL() const
	{
		return m_urlSearch;
	}
	void SetSearchURL(std::wstring const& inURL)
	{
		m_urlSearch = inURL;
	}
	std::wstring GetHelpURL() const
	{
		return m_urlHelp;
	}
	void SetHelpURL(std::wstring const& inURL)
	{
		m_urlHelp = inURL;
	}
	std::map<std::wstring, std::wstring> const& LocationCodes() const
	{
		return m_Locations;
	}
	std::map<std::wstring, std::wstring> const& VenueCodes() const
	{
		return m_Venues;
	}

	bool HasLocationCode(std::wstring const& inCode) const;
	bool AddLocationCode(std::wstring const& inCode, std::wstring const& inName);
	bool RemoveLocationCode(std::wstring const& inCode);
	bool RemoveAllLocationCodes();
	bool HasVenueCode(std::wstring const& inCode) const;
	bool AddVenueCode(std::wstring const& inCode, std::wstring const& inVenue);
	bool RemoveVenueCode(std::wstring const& inCode);
	bool RemoveAllVenueCodes();

private:
	std::wstring m_Name;
	std::wstring m_Desc;
	std::wstring m_urlSearch;
	std::wstring m_urlHelp;
	std::map<std::wstring, std::wstring> m_Locations;
	std::map<std::wstring, std::wstring> m_Venues;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigCalSite objects.
 */
class ARBConfigCalSiteList : public ARBVector<ARBConfigCalSitePtr>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual T element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ElementNodePtr const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Sort the list by name.
	 */
	void sort();

	/**
	 * Find the named Calendar Site.
	 * @param inSite Site to find.
	 * @param outSite Object that was found.
	 * @return Whether the object was found.
	 */
	bool FindSite(
			std::wstring const& inSite,
			ARBConfigCalSitePtr* outSite = nullptr) const;

	/**
	 * Add a site.
	 * @param inSite Name of site to add.
	 * @param outSite Pointer to new object, NULL if name already exists or is empty.
	 * @return Whether the object was added.
	 */
	bool AddSite(
			std::wstring const& inSite,
			ARBConfigCalSitePtr* outSite = nullptr);

	/**
	 * Add a site.
	 * @param inSite Site to add.
	 * @return Whether the object was added.
	 */
	bool AddSite(ARBConfigCalSitePtr const& inSite);

	/**
	 * Delete a site.
	 * @param inSite Name of site to delete.
	 * @return Number of sites deleted (0 or 1).
	 */
	int DeleteSite(std::wstring const& inSite);
};
