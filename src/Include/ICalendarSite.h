#pragma once

/*
 * Copyright © 2007-2009 David Connet. All Rights Reserved.
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
 * @brief Interface for plug-in DLLs to read data.
 * @author David Connet
 *
 * In order to read data directly from a website, we need to parse the HTML.
 * Since all sites will be different, we'll deal with this thru a plug-in
 * architecture. All DLLs must export an undecorated api 'GetCalendarInterface'
 * that will be obtained using GetProcAddress.
 * wxWidget note: wxDynamicLibrary is used
 *
 * All extension DLLs should be named 'cal_<yourname>.dll' and placed in the
 * same directory as the executable.
 * [Unix note: 'libcal_<yourname>.so']
 *
 * If you multithread your DLL, you MUST call the IProgressMeter on the original
 * thread since this will be directly modifying UI elements.
 *
 * Revision History
 * @li 2009-04-01 DRC Added IReadHttpData interface
 * @li 2007-08-12 DRC Created
 */

class CVersionNum;		// ARB/VersionNum.h
class IProgressMeter;	// Include/IProgressMeter.h


// This is a class ARB implements to allow plugsin access to reading URLs.
// This is done to keep all UI elements in ARB and not in the plugins.
// It simplifies UI thread issues
class IReadHttpData
{
public:
	/**
	 * Read a URL
	 * @param inURL URL to process.
	 * @return Raw data from URL, must release with releaseBuffer
	 */
	virtual char* ReadData(char const* inURL) const = 0;

	/**
	 * Release data.
	 */
	virtual void releaseBuffer(char* pData) const = 0;
};


class ICalendarSite
{
public:
	/**
	 * Release object.
	 * @note Implementers will typically do a 'delete this'.
	 */
	virtual void Release() = 0;

	/**
	 * Get the version of the plugin.
	 * We're not directly parsing VERSIONINFO as that is not platform portable.
	 * @return Whether the version was returned.
	 */
	virtual bool GetVersion(CVersionNum* ver) const = 0;

	/**
	 * Release a string returned from GetDescription or Process.
	 * This must be done because the objects may have been allocated on
	 * different heaps. If would be easier to just return an STL string,
	 * but again, different heaps will corrupt memory.
	 */
	virtual void releaseBuffer(char* pData) const = 0;

	/**
	 * Get a short name of the site this parses.
	 * @return Name of plugin, must release with releaseBuffer()
	 */
	virtual char* GetName() const = 0;

	/**
	 * Get a description of the site this parses.
	 * @return Description of plugin, must release with releaseBuffer()
	 */
	virtual char* GetDescription() const = 0;

	/**
	 * Get a list of location codes.
	 * @return '\n' separated list of ':' separated pairs of code/name,
	 *          must release with releaseBuffer()
	 * @note This may return NULL. 'code' is the value that will be passed
	 * in the query. 'name' is a pretty name.
	 */
	virtual char* GetLocationCodes() const = 0;

	/**
	 * Get a list of venue codes.
	 * @return '\n' separated list of ':' separated pairs of code/venue,
	 *          must release with releaseBuffer()
	 * @note 'code' is the value that will be passed in the query. 'venue'
	 * is the mapping to ARBs venue code. If the same, the 'venue' part is
	 * optional.
	 */
	virtual char* GetVenueCodes() const = 0;

	/**
	 * Get the processed data. The returned data should be in the form of
	 * a valid ARB file. If this were to be saved as a file, we could then
	 * directly import it in ARB.
	 * @param dataReader Use to read data from a URL.
	 * @param inLocCodes ':' separated list of location codes, may be NULL.
	 * @param inVenueCodes ':' separated list of venue codes, may be NULL.
	 * @param progress Allow the plugin to update the progress
	 * @return Data to be processed, must release with releaseBuffer()
	 * @note If NULL is returned, this module will be removed from future
	 * updates (during the executable's current session)
	 */
	virtual char* Process(
			IReadHttpData* dataReader,
			char const* inLocCodes,
			char const* inVenueCodes,
			IProgressMeter* progress) const = 0;
};

typedef ICalendarSite* (*GETCALENDARINTERFACE)();
