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

	std::wstring GetID() const override;
	bool GetVersion(ARBCommon::CVersionNum& outVer) const override;
	std::wstring GetName() const override;
	std::wstring GetDescription() const override;
	size_t GetLocationCodes(std::map<std::wstring, std::wstring>& locCodes) const override;
	size_t GetVenueCodes(std::map<std::wstring, std::wstring>& venueCodes) const override;
	std::string Process(
		std::vector<std::wstring> const& inLocCodes,
		std::vector<std::wstring> const& inVenueCodes,
		IProgressMeter* progress) const override;
};

} // namespace dconSoft
