#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgConfigPlaceInfo class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-11-04 DRC Created.
 */

#include <vector>


class CDlgConfigPlaceInfo : public wxDialog
{
public:
	CDlgConfigPlaceInfo(
			std::vector<short> const& inExistingPlaces,
			short inPlace,
			double inValue,
			wxWindow* pParent = NULL);

	short GetPlace() const		{return m_Place;}
	double GetValue() const		{return m_Value;}

private:
	short m_Place;
	double m_Value;
	double m_origValue;
};