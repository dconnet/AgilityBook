#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Manage the direct download of calendar data from the web.
 * @author David Connet
 *
 * Revision History
 * 2009-02-10 Ported to wxWidgets.
 * 2007-08-12 Created
 */

#include "ARB/ARBTypes2.h"
#include "ARBCommon/VersionNum.h"

#include "ICalendarSite.h"


namespace dconSoft
{
class ARBConfigCalSiteList;


class CalSiteData
{
	// Copy semantics don't work well with our cleanup code!
	DECLARE_NO_COPY_IMPLEMENTED(CalSiteData);

public:
	CalSiteData(ICalendarSitePtr pSite);
	~CalSiteData();

	void Connect();

	bool isValid() const
	{
		return !!m_pSite;
	}
	void Unload(bool bPermanently = false);

	std::wstring GetID() const
	{
		return m_pSite->GetID();
	}
	std::wstring GetName() const
	{
		return m_pSite->GetName();
	}
	std::wstring GetDescription() const
	{
		return m_pSite->GetDescription();
	}
	std::map<std::wstring, std::wstring> const& QueryLocationCodes() const
	{
		return m_LocCodes;
	}
	std::map<std::wstring, std::wstring> const& QueryVenueCodes() const
	{
		return m_VenueCodes;
	}
	std::string Process(
		IProgressMeter* progress,
		std::vector<std::wstring> const& inLocationCodes,
		std::vector<std::wstring> const& inVenueCodes);

private:
	ICalendarSitePtr m_pSite;
	std::wstring m_id;
	ARBCommon::CVersionNum m_Version;
	std::map<std::wstring, std::wstring> m_LocCodes;
	std::map<std::wstring, std::wstring> m_VenueCodes;
};
typedef std::shared_ptr<CalSiteData> CalSiteDataPtr;


/**
 * This class manages the direct download of calendar data from the web.
 */
class CCalendarSites
{
	DECLARE_NO_COPY_IMPLEMENTED(CCalendarSites)
public:
	CCalendarSites();
	~CCalendarSites();

	/**
	 * Iterate through all the active sites and add the desired entries.
	 */
	bool FindEntries(ARBConfigCalSiteList& sites, wxWindow* pParent = nullptr);

private:
	std::vector<CalSiteDataPtr> m_DirectAccess;
};

} // namespace dconSoft
