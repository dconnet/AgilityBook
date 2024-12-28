#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief CCalendarSiteUSDAA
 * @author David Connet
 *
 * Revision History
 * 2010-03-05 Created
 */

#include "ICalendarSite.h"


namespace dconSoft
{

class CCalendarSiteUSDAA : public ICalendarSite
{
	DECLARE_NO_COPY_IMPLEMENTED(CCalendarSiteUSDAA)
public:
	static ICalendarSitePtr Create();
	CCalendarSiteUSDAA();
	virtual ~CCalendarSiteUSDAA();

	wxString GetID() const override;
	bool GetVersion(ARBCommon::CVersionNum& outVer) const override;
	wxString GetName() const override;
	wxString GetDescription() const override;
	size_t GetLocationCodes(std::map<wxString, wxString>& locCodes) const override;
	size_t GetVenueCodes(std::map<wxString, wxString>& venueCodes) const override;
	std::string Process(
		std::vector<wxString> const& inLocCodes,
		std::vector<wxString> const& inVenueCodes,
		IProgressMeter* progress) const override;
};

} // namespace dconSoft
