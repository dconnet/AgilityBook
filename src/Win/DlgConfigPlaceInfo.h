#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgConfigPlaceInfo class
 * @author David Connet
 *
 * Revision History
 * 2009-02-11 Ported to wxWidgets.
 * 2006-11-04 Created.
 */

#include <vector>


namespace dconSoft
{


class CDlgConfigPlaceInfo : public wxDialog
{
public:
	CDlgConfigPlaceInfo(
		std::vector<short> const& inExistingPlaces,
		short inPlace,
		double inValue,
		wxWindow* pParent = nullptr);

	short GetPlace() const
	{
		return m_Place;
	}
	double GetValue() const
	{
		return m_Value;
	}

private:
	short m_Place;
	double m_Value;
};

} // namespace dconSoft
