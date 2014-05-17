#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgConfigTitlePoints class
 * @author David Connet
 *
 * Revision History
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-10-06 Removed ARB classes so it could be used to lifetime pts.
 */

#include "ARB/ARBTypes2.h"
#include "ARB/ARBCalcPoints.h"
class CTextCtrl;


class CDlgConfigTitlePoints : public wxDialog
{
public:
	enum ETitlePointType
	{
		// These numbers correspond to the index in the choice list
		eTitleNormal = 0,
		eTitleLifetime,
		eTitlePlacement,
		eTitleMax
	};

	CDlgConfigTitlePoints(
			ARBConfigVenuePtr inVenue,
			double inValue, // Faults or Place
			double inPoints,
			ETitlePointType inType,
			ARBPointsType inTypeNormal,
			wxWindow* pParent = nullptr);

	double Faults() const	{return m_Faults;}
	short Place() const		{return m_Place;}
	double Points() const	{return m_Points;}
	ETitlePointType Type() const
	{
		return static_cast<ETitlePointType>(m_Type);
	}
	ARBPointsType TypeNormal() const
	{
		return static_cast<ARBPointsType>(m_TypeNormal);
	}

private:
	ARBConfigVenuePtr m_Venue;
	int m_Type;
	int m_TypeNormal;
	wxStaticText* m_textValue;
	CTextCtrl* m_ctrlValue;
	wxStaticText* m_textPoints;
	CTextCtrl* m_ctrlPoints;
	wxChoice* m_ctrlType;
	wxChoice* m_ctrlTypeNormal;
	double m_Faults;
	short m_Place;
	double m_Points;

	DECLARE_ON_INIT()
	void OnSelchangeTitlePoints(wxCommandEvent& evt);
};
