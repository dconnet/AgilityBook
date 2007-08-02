#pragma once

/*
 * Copyright © 2007 David Connet. All Rights Reserved.
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
public:
	ARBConfigCalSite();
	~ARBConfigCalSite();
	ARBConfigCalSite(ARBConfigCalSite const& rhs);
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
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Get the formatted URL to use to request data.
	 */
	ARBString GetFormattedURL(
			std::vector<ARBString> const& inLocCodes,
			std::vector<ARBString> const& inVenueCodes) const;

	/*
	 * Getters/setters.
	 */
	ARBString GetURL() const
	{
		return m_URL;
	}
	void SetURL(ARBString const& inURL)
	{
		m_URL = inURL;
	}
	bool HasLocationCode(ARBString const& inCode) const;
	bool AddLocationCode(ARBString const& inCode, ARBString const& inName);
	bool RemoveLocationCode(ARBString const& inCode);
	bool RemoveAllLocationCodes();
	bool HasVenueCode(ARBString const& inCode) const;
	bool AddVenueCode(ARBString const& inCode, ARBString const& inVenue);
	bool RemoveVenueCode(ARBString const& inCode);
	bool RemoveAllVenueCodes();

private:
	ARBString m_URL;
	std::map<ARBString, ARBString> m_Locations;
	std::map<ARBString, ARBString> m_Venues;
};
