#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Interface for CalendarSite "plug-in"s to read data.
 * @author David Connet
 *
 * In order to read data directly from a website, we need to parse the HTML.
 * Since all sites will be different, we'll deal with this thru a plug-in
 * architecture. Note: This used to be a "plug-in" DLL architecture - this
 * has since been changed to compile into ARB (see Win/CalendarSite*.*)
 * I never did get the plugin to work properly on the Mac, and in wx2.9.1
 * on Win7, it's now crashing. So simplify. And now that code can make use
 * of all the classes in ARB and not need extra interfaces.
 *
 * Revision History
 * 2010-03-05 Removed IReadHttpData interface, dynamic DLL loading
 * 2009-04-01 Added IReadHttpData interface
 * 2007-08-12 Created
 */

#include <map>
#include <string>
#include <vector>
class CVersionNum;		// ARB/VersionNum.h
class IProgressMeter;	// Include/IProgressMeter.h


class ICalendarSite
{
public:
	/**
	 * Get the identifier of the plugin.
	 * Previously, this was the DLL file name. Now, just ensure a unique name
	 * for registry access purposes.
	 * @return ID
	 */
	virtual std::wstring GetID() const = 0;

	/**
	 * Get the version of the plugin.
	 * We're not directly parsing VERSIONINFO as that is not platform portable.
	 * @return Whether the version was returned.
	 */
	virtual bool GetVersion(CVersionNum& outVer) const = 0;

	/**
	 * Get a short name of the site this parses.
	 * @return Name of plugin, must release with releaseBuffer()
	 */
	virtual std::wstring GetName() const = 0;

	/**
	 * Get a description of the site this parses.
	 * @return Description of plugin, must release with releaseBuffer()
	 */
	virtual std::wstring GetDescription() const = 0;

	/**
	 * Get a list of location codes.
	 * @return Number of pairs.
	 * @note First item is the value that will be passed
	 *       in the query. Second item is a pretty name.
	 */
	virtual size_t GetLocationCodes(
			std::map<std::wstring, std::wstring>& locCodes) const = 0;

	/**
	 * Get a list of venue codes.
	 * @return Number of pairs.
	 * @note First item is the value that will be passed in the query. Second
	 *       item is the mapping to ARBs venue code. If they are the same,
	 *       specifying the 2nd item is optional.
	 */
	virtual size_t GetVenueCodes(
			std::map<std::wstring, std::wstring>& venueCodes) const = 0;

	/**
	 * Get the processed data. The returned data should be in the form of
	 * a valid ARB file. If this were to be saved as a file, we could then
	 * directly import it in ARB.
	 * @param inLocCodes List of location codes, may be empty.
	 * @param inVenueCodes List of venue codes, may be empty.
	 * @param progress Allow the plugin to update the progress
	 * @return Data to be processed, must release with releaseBuffer()
	 * @note If NULL is returned, this module will be removed from future
	 * updates (during the executable's current session)
	 */
	virtual std::string Process(
			std::vector<std::wstring> const& inLocCodes,
			std::vector<std::wstring> const& inVenueCodes,
			IProgressMeter* progress) const = 0;
};
typedef std::shared_ptr<ICalendarSite> ICalendarSitePtr;
