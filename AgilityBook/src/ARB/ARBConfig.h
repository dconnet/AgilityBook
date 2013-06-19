#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBConfig class.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
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
#include "ARBTypes2.h"


/**
 * Move platform specific code out of ARBConfig.
 * On Windows, these files are stored in the resources.
 * Other platforms may be different.
 */
class IARBConfigHandler
{
public:
	/// Load the default configuration
	virtual ElementNodePtr LoadDefaultConfig() const = 0;
	/// Load the DTD
	virtual std::string LoadDTD() const = 0;
};


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
	 * @param inHandler Interface to deal with platform specific resource issues
	 */
	void Default(IARBConfigHandler* inHandler);

	/**
	 * Get the DTD (AgilityRecordBook.dtd)
	 * @param inHandler Interface to deal with platform specific resource issues
	 */
	static std::string GetDTD(IARBConfigHandler* inHandler);

	/**
	 * Convenience function to get the nice name of a title.
	 * @param inVenue Venue that contains inTitle.
	 * @param inTitle Name of title.
	 * @return Nice name (longname) of a title.
	 */
	std::wstring GetTitleNiceName(
			std::wstring const& inVenue,
			std::wstring const& inTitle) const;

	/**
	 * Convenience function to get the complete name of a title.
	 * @param inTitle Title.
	 * @param bAbbrevFirst List Name before or after Longname.
	 * @return Complete name of title (name + longname).
	 */
	std::wstring GetTitleCompleteName(
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
			std::wostringstream& ioInfo);

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
