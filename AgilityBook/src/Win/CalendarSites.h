#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Manage the direct download of calendar data from the web.
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-10 DRC Ported to wxWidgets.
 * @li 2007-08-12 DRC Created
 */

#include "ARB/ARBTypes.h"
class CAgilityBookDoc;
class CCalendarSitesImpl;


/**
 * This class manages the direct download of calendar data from the web.
 */
class CCalendarSites
{
public:
	CCalendarSites();
	~CCalendarSites();

	/**
	 * Iterate through all the active sites and add the desired entries.
	 */
	bool FindEntries(
			CAgilityBookDoc* pDoc,
			ARBCalendarList& inCalendar,
			wxWindow* pParent = NULL);

private:
	CCalendarSitesImpl* m_Impl;
};
