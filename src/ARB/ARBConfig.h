#pragma once

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
 * @brief ARBConfig class.
 * @author David Connet
 *
 * Revision History
 * @li 2004-02-26 DRC Added version number to configuration.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-10-22 DRC Added static GetDTD() method.
 */

#include "ARBConfigAction.h"
#include "ARBConfigFault.h"
#include "ARBConfigOtherPoints.h"
#include "ARBConfigVenue.h"
class ARBVersion;
class Element;

/**
 * The main configuration class.
 */
class ARBConfig
{
public:
	ARBConfig();
	ARBConfig(const ARBConfig& rhs);
	~ARBConfig();
	ARBConfig& operator=(const ARBConfig& rhs);
	bool operator==(const ARBConfig& rhs) const;
	bool operator!=(const ARBConfig& rhs) const;

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
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool LoadFault(
		const Element& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	/**
	 * Load an OtherPoint object.
	 * This functionality is here to enable backwards compatibility.
	 * FileVersion 3 moved OtherPoints from the venue to the config.
	 * @pre inTree is the actual ARBConfigOtherPoint element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool LoadOtherPoints(
		const Element& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	/**
	 * Load the configuration.
	 * @pre inTree is the actual ARBConfig element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool Load(
		const Element& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfig element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Set the configuration to the default (DefaultConfig.xml)
	 */
	void Default();

	/**
	 * Get the DTD (AgilityRecordBook.dtd)
	 */
	static std::string GetDTD();

	/**
	 * Convenience function to get the nice name of a title.
	 * @param inVenue Venue that contains inTitle.
	 * @param inTitle Name of title.
	 * @return Nice name (longname) of a title.
	 */
	std::string GetTitleNiceName(
		const std::string& inVenue,
		const std::string& inTitle) const;

	/**
	 * Convenience function to get the complete name of a title.
	 * @param inVenue Venue that contains inTitle.
	 * @param inTitle Name of title.
	 * @param bAbbrevFirst List Name before or after Longname.
	 * @return Complete name of title (name + longname).
	 */
	std::string GetTitleCompleteName(
		const std::string& inVenue,
		const std::string& inTitle,
		bool bAbbrevFirst = true) const;

	/**
	 * Update this configuration from inConfigNew.
	 * @param indent Indentation level for generating messages.
	 * @param inConfigNew Configuration to merge.
	 * @param ioInfo Accumulated messages about changes that have happened.
	 * @return Whether or not changes have occurred.
	 */
	bool Update(int indent, const ARBConfig& inConfigNew, std::string& ioInfo);

	/*
	 * Getters.
	 */
	short GetVersion() const;
	const ARBConfigActionList& GetActions() const;
	ARBConfigActionList& GetActions();
	const ARBConfigVenueList& GetVenues() const;
	ARBConfigVenueList& GetVenues();
	const ARBConfigFaultList& GetFaults() const;
	ARBConfigFaultList& GetFaults();
	const ARBConfigOtherPointsList& GetOtherPoints() const;
	ARBConfigOtherPointsList& GetOtherPoints();

private:
	short m_Version;
	ARBConfigActionList m_Actions;
	ARBConfigVenueList m_Venues;
	ARBConfigFaultList m_FaultTypes;
	ARBConfigOtherPointsList m_OtherPoints;
};

inline short ARBConfig::GetVersion() const
{
	return m_Version;
}

inline const ARBConfigActionList& ARBConfig::GetActions() const
{
	return m_Actions;
}

inline ARBConfigActionList& ARBConfig::GetActions()
{
	return m_Actions;
}

inline const ARBConfigVenueList& ARBConfig::GetVenues() const
{
	return m_Venues;
}

inline ARBConfigVenueList& ARBConfig::GetVenues()
{
	return m_Venues;
}

inline const ARBConfigFaultList& ARBConfig::GetFaults() const
{
	return m_FaultTypes;
}

inline ARBConfigFaultList& ARBConfig::GetFaults()
{
	return m_FaultTypes;
}

inline const ARBConfigOtherPointsList& ARBConfig::GetOtherPoints() const
{
	return m_OtherPoints;
}

inline ARBConfigOtherPointsList& ARBConfig::GetOtherPoints()
{
	return m_OtherPoints;
}
