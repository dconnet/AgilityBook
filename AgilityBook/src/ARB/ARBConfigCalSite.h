#pragma once

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
 * @brief ARBConfigCalSite
 * @author David Connet
 *
 * Revision History
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
	tstring GetFormattedURL(
			std::vector<tstring> const& inLocCodes,
			std::vector<tstring> const& inVenueCodes) const;

	/*
	 * Getters/setters.
	 */
	tstring GetName() const
	{
		return m_Name;
	}
	void SetName(tstring const& inName)
	{
		m_Name = inName;
	}
	tstring GetDescription() const
	{
		return m_Desc;
	}
	void SetDescription(tstring const& inDesc)
	{
		m_Desc = inDesc;
	}
	tstring GetSearchURL() const
	{
		return m_urlSearch;
	}
	void SetSearchURL(tstring const& inURL)
	{
		m_urlSearch = inURL;
	}
	tstring GetHelpURL() const
	{
		return m_urlHelp;
	}
	void SetHelpURL(tstring const& inURL)
	{
		m_urlHelp = inURL;
	}
	std::map<tstring, tstring> const& LocationCodes() const
	{
		return m_Locations;
	}
	std::map<tstring, tstring> const& VenueCodes() const
	{
		return m_Venues;
	}

	bool HasLocationCode(tstring const& inCode) const;
	bool AddLocationCode(tstring const& inCode, tstring const& inName);
	bool RemoveLocationCode(tstring const& inCode);
	bool RemoveAllLocationCodes();
	bool HasVenueCode(tstring const& inCode) const;
	bool AddVenueCode(tstring const& inCode, tstring const& inVenue);
	bool RemoveVenueCode(tstring const& inCode);
	bool RemoveAllVenueCodes();

private:
	tstring m_Name;
	tstring m_Desc;
	tstring m_urlSearch;
	tstring m_urlHelp;
	std::map<tstring, tstring> m_Locations;
	std::map<tstring, tstring> m_Venues;
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
			tstring const& inSite,
			ARBConfigCalSitePtr* outSite = NULL) const;

	/**
	 * Add a site.
	 * @param inSite Name of site to add.
	 * @param outSite Pointer to new object, NULL if name already exists or is empty.
	 * @return Whether the object was added.
	 */
	bool AddSite(
			tstring const& inSite,
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
	int DeleteSite(tstring const& inSite);
};
