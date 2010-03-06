#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief CCalendarSiteUSDAA
 * @author David Connet
 *
 * Revision History
 * @li 2010-03-05 DRC Created
 */

#include "ICalendarSite.h"


class CCalendarSiteUSDAA : public ICalendarSite
{
protected:
	virtual ~CCalendarSiteUSDAA();
public:
	CCalendarSiteUSDAA();
	virtual void Release();

	virtual wxString GetID() const;
	virtual bool GetVersion(CVersionNum& outVer) const;
	virtual wxString GetName() const;
	virtual wxString GetDescription() const;
	virtual size_t GetLocationCodes(
			std::map<wxString, wxString>& locCodes) const;
	virtual size_t GetVenueCodes(
			std::map<wxString, wxString>& venueCodes) const;
	virtual std::string Process(
			std::vector<wxString> const& inLocCodes,
			std::vector<wxString> const& inVenueCodes,
			IProgressMeter* progress) const;
};
