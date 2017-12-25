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


class CCalendarSiteUSDAA : public ICalendarSite
{
protected:
	virtual ~CCalendarSiteUSDAA();
public:
	CCalendarSiteUSDAA();
	virtual void Release();

	virtual std::wstring GetID() const;
	virtual bool GetVersion(CVersionNum& outVer) const;
	virtual std::wstring GetName() const;
	virtual std::wstring GetDescription() const;
	virtual size_t GetLocationCodes(
			std::map<std::wstring, std::wstring>& locCodes) const;
	virtual size_t GetVenueCodes(
			std::map<std::wstring, std::wstring>& venueCodes) const;
	virtual std::string Process(
			std::vector<std::wstring> const& inLocCodes,
			std::vector<std::wstring> const& inVenueCodes,
			IProgressMeter* progress) const;
};
