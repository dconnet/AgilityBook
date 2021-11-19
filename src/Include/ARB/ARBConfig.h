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
 * 2018-12-16 Convert to fmt.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-10-26 Added option to prevent auto-update user query.
 * 2005-01-10 Allow titles to be optionally entered multiple times.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-02-26 Added version number to configuration.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-10-22 Added static GetDTD() method.
 */

#include "ARBConfigAction.h"
#include "ARBConfigFault.h"
#include "ARBConfigOtherPoints.h"
#include "ARBConfigVenue.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"

#include "fmt/xchar.h"


/**
 * Move platform specific code out of ARBConfig.
 * On Windows, these files are stored in the resources.
 * Other platforms may be different.
 */
class ARB_API IARBConfigHandler
{
public:
	virtual ~IARBConfigHandler() = default;
	/// Load the default configuration
	virtual ElementNodePtr LoadDefaultConfig() const = 0;
	/// Load the DTD
	virtual std::string LoadDTD() const = 0;
};


/**
 * The main configuration class.
 */
class ARB_API ARBConfig
{
	// We allow copying, but not assignment. Updating a config object is
	// accomplished via the Update() method.
	ARBConfig& operator=(ARBConfig const& rhs) = delete;
	ARBConfig& operator=(ARBConfig&& rhs) = delete;

public:
	ARBConfig();
	~ARBConfig();
	ARBConfig(ARBConfig const& rhs);
	ARBConfig(ARBConfig&& rhs);
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
	bool LoadFault(ElementNodePtr const& inTree, ARBVersion const& inVersion, ARBErrorCallback& ioCallback);

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
	bool LoadOtherPoints(ElementNodePtr const& inTree, ARBVersion const& inVersion, ARBErrorCallback& ioCallback);

	/**
	 * Load the configuration.
	 * @pre inTree is the actual ARBConfig element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(ElementNodePtr const& inTree, ARBVersion const& inVersion, ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfig element will be created in ioTree.
	 */
	bool Save(ElementNodePtr const& ioTree) const;

	/**
	 * Set the configuration to the default (DefaultConfig.xml)
	 * @param inHandler Interface to deal with platform specific resource issues
	 * @param pVersion Version number of default config.
	 */
	void Default(IARBConfigHandler const* inHandler, ARBVersion* pVersion = nullptr);

	/**
	 * Get the DTD (AgilityRecordBook.dtd)
	 * @param inHandler Interface to deal with platform specific resource issues
	 */
	static std::string GetDTD(IARBConfigHandler const* inHandler);

	/**
	 * Convenience function to get the nice name of a title.
	 * @param inVenue Venue that contains inTitle.
	 * @param inTitle Name of title.
	 * @return Nice name (longname) of a title.
	 */
	std::wstring GetTitleNiceName(std::wstring const& inVenue, std::wstring const& inTitle) const;

	/**
	 * Convenience function to get the complete name of a title.
	 * @param inTitle Title.
	 * @param bAbbrevFirst List Name before or after Longname.
	 * @return Complete name of title (name + longname).
	 */
	std::wstring GetTitleCompleteName(ARBDogTitlePtr const& inTitle, bool bAbbrevFirst = true) const;

	/**
	 * Update this configuration from inConfigNew.
	 * @param indent Indentation level for generating messages.
	 * @param inConfigNew Configuration to merge.
	 * @param ioInfo Accumulated messages about changes that have happened.
	 * @return Whether or not changes have occurred.
	 */
	bool Update(int indent, ARBConfig const& inConfigNew, fmt::wmemory_buffer& ioInfo);

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
	ARBConfigVenueList m_Venues;
	ARBConfigFaultList m_FaultTypes;
	ARBConfigOtherPointsList m_OtherPoints;
};
