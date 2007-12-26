#pragma once

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
 * @brief ARBConfig class.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-10-26 DRC Added option to prevent auto-update user query.
 * @li 2005-01-10 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-02-26 DRC Added version number to configuration.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-10-22 DRC Added static GetDTD() method.
 */

#include "ARBConfigAction.h"
#include "ARBConfigCalSite.h"
#include "ARBConfigFault.h"
#include "ARBConfigOtherPoints.h"
#include "ARBConfigVenue.h"
#include "ARBTypes.h"

/**
 * The main configuration class.
 */
class ARBConfig
{
	// We allow copying, but not assignment. Updating a config object is
	// accomplished via the Update() method.
	ARBConfig& operator=(ARBConfig const& rhs); // not implemented
public:
	ARBConfig();
	~ARBConfig();
	ARBConfig(ARBConfig const& rhs);
	bool operator==(ARBConfig const& rhs) const;
	bool operator!=(ARBConfig const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Reset the contents of this object and all sub-objects.
	 */
	void clear();

	/**
	 * Load a Fault object.
	 * This functionality is here to enable backwards compatibility.
	 * FileVersion 3 moved faults from the venue to the config.
	 * @pre inTree is the actual ARBConfigFault element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool LoadFault(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Load an OtherPoint object.
	 * This functionality is here to enable backwards compatibility.
	 * FileVersion 3 moved OtherPoints from the venue to the config.
	 * @pre inTree is the actual ARBConfigOtherPoint element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool LoadOtherPoints(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Load the configuration.
	 * @pre inTree is the actual ARBConfig element.
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
	 * @post The ARBConfig element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

	/**
	 * Set the configuration to the default (DefaultConfig.xml)
	 */
	void Default();

	/**
	 * Get the DTD (AgilityRecordBook.dtd)
	 */
	static std::string GetDTD(bool bNormalizeCRNL = true);

	/**
	 * Convenience function to get the nice name of a title.
	 * @param inVenue Venue that contains inTitle.
	 * @param inTitle Name of title.
	 * @return Nice name (longname) of a title.
	 */
	tstring GetTitleNiceName(
			tstring const& inVenue,
			tstring const& inTitle) const;

	/**
	 * Convenience function to get the complete name of a title.
	 * @param inTitle Title.
	 * @param bAbbrevFirst List Name before or after Longname.
	 * @return Complete name of title (name + longname).
	 */
	tstring GetTitleCompleteName(
			ARBDogTitlePtr inTitle,
			bool bAbbrevFirst = true) const;

	/**
	 * Update this configuration from inConfigNew.
	 * @param indent Indentation level for generating messages.
	 * @param inConfigNew Configuration to merge.
	 * @param ioInfo Accumulated messages about changes that have happened.
	 * @return Whether or not changes have occurred.
	 */
	bool Update(
			int indent,
			ARBConfig const& inConfigNew,
			otstringstream& ioInfo);

	/*
	 * Getters.
	 */
	bool GetUpdateStatus() const
	{
		return m_bUpdate;
	}
	void SetUpdateStatus(bool inUpdate)
	{
		m_bUpdate = inUpdate;
	}
	short GetVersion() const
	{
		return m_Version;
	}
	ARBConfigCalSiteList const& GetCalSites() const
	{
		return m_CalSites;
	}
	ARBConfigCalSiteList& GetCalSites()
	{
		return m_CalSites;
	}
	ARBConfigActionList const& GetActions() const
	{
		return m_Actions;
	}
	ARBConfigActionList& GetActions()
	{
		return m_Actions;
	}
	ARBConfigVenueList const& GetVenues() const
	{
		return m_Venues;
	}
	ARBConfigVenueList& GetVenues()
	{
		return m_Venues;
	}
	ARBConfigFaultList const& GetFaults() const
	{
		return m_FaultTypes;
	}
	ARBConfigFaultList& GetFaults()
	{
		return m_FaultTypes;
	}
	ARBConfigOtherPointsList const& GetOtherPoints() const
	{
		return m_OtherPoints;
	}
	ARBConfigOtherPointsList& GetOtherPoints()
	{
		return m_OtherPoints;
	}

private:
	bool m_bUpdate;
	short m_Version;
	ARBConfigActionList m_Actions;
	ARBConfigCalSiteList m_CalSites;
	ARBConfigVenueList m_Venues;
	ARBConfigFaultList m_FaultTypes;
	ARBConfigOtherPointsList m_OtherPoints;
};
