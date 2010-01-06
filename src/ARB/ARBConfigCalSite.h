#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 * @brief ARBConfigCalSite
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2007-08-02 DRC Created
 */

#include "ARBTypes.h"
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
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 */
	bool Save(ElementNodePtr ioTree) const;

	/**
	 * Get the formatted URL to use to request data.
	 */
	wxString GetFormattedURL(
			std::vector<wxString> const& inLocCodes,
			std::vector<wxString> const& inVenueCodes) const;

	/*
	 * Getters/setters.
	 */
	wxString GetName() const
	{
		return m_Name;
	}
	void SetName(wxString const& inName)
	{
		m_Name = inName;
	}
	wxString GetDescription() const
	{
		return m_Desc;
	}
	void SetDescription(wxString const& inDesc)
	{
		m_Desc = inDesc;
	}
	wxString GetSearchURL() const
	{
		return m_urlSearch;
	}
	void SetSearchURL(wxString const& inURL)
	{
		m_urlSearch = inURL;
	}
	wxString GetHelpURL() const
	{
		return m_urlHelp;
	}
	void SetHelpURL(wxString const& inURL)
	{
		m_urlHelp = inURL;
	}
	std::map<wxString, wxString> const& LocationCodes() const
	{
		return m_Locations;
	}
	std::map<wxString, wxString> const& VenueCodes() const
	{
		return m_Venues;
	}

	bool HasLocationCode(wxString const& inCode) const;
	bool AddLocationCode(wxString const& inCode, wxString const& inName);
	bool RemoveLocationCode(wxString const& inCode);
	bool RemoveAllLocationCodes();
	bool HasVenueCode(wxString const& inCode) const;
	bool AddVenueCode(wxString const& inCode, wxString const& inVenue);
	bool RemoveVenueCode(wxString const& inCode);
	bool RemoveAllVenueCodes();

private:
	wxString m_Name;
	wxString m_Desc;
	wxString m_urlSearch;
	wxString m_urlHelp;
	std::map<wxString, wxString> m_Locations;
	std::map<wxString, wxString> m_Venues;
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
			ElementNodePtr inTree,
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
			wxString const& inSite,
			ARBConfigCalSitePtr* outSite = NULL) const;

	/**
	 * Add a site.
	 * @param inSite Name of site to add.
	 * @param outSite Pointer to new object, NULL if name already exists or is empty.
	 * @return Whether the object was added.
	 */
	bool AddSite(
			wxString const& inSite,
			ARBConfigCalSitePtr* outSite = NULL);

	/**
	 * Add a site.
	 * @param inSite Site to add.
	 * @return Whether the object was added.
	 */
	bool AddSite(ARBConfigCalSitePtr inSite);

	/**
	 * Delete a site.
	 * @param inSite Name of site to delete.
	 * @return Number of sites deleted (0 or 1).
	 */
	int DeleteSite(wxString const& inSite);
};
