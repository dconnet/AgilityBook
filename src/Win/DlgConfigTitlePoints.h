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
 * 2016-06-14 Add support for named/speedpt lifetime points.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-10-06 Removed ARB classes so it could be used to lifetime pts.
 */

#include "ARB/ARBTypes2.h"
#include "ARB/ARBCalcPoints.h"
class CTextCtrl;


class CDlgConfigTitlePoints : public wxDialog
{
	void Init(wxWindow* pParent);

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
			ARBConfigVenuePtr const& inVenue,
			double inValue, // Faults or Place
			double inPoints,
			ETitlePointType inType,
			ARBPointsType inTypeNormal,
			wxWindow* pParent = nullptr);

	CDlgConfigTitlePoints(
			ARBConfigVenuePtr const& inVenue,
			ARBConfigLifetimePointsPtr const& inLifetime,
			wxWindow* pParent = nullptr);

	double Faults() const			{return m_Faults;}
	short Place() const				{return m_Place;}
	bool UseSpeedPts() const		{return m_bSpeed;}
	double Points() const			{return m_Points;}
	wxString LifetimeName() const	{return m_LifetimeName;}
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
	wxCheckBox* m_ctrlSpeedPts;
	wxStaticText* m_textPoints;
	CTextCtrl* m_ctrlPoints;
	wxChoice* m_ctrlType;
	wxChoice* m_ctrlTypeNormal;
	wxStaticText* m_textLifetimeName;
	wxComboBox* m_ctrlLifetimeName;
	double m_Faults;
	short m_Place;
	bool m_bSpeed;
	double m_Points;
	wxString m_LifetimeName;

	bool UpdateControls(int oldType);

	DECLARE_EVENT_TABLE()
	DECLARE_ON_INIT()
	void OnUseSpeedPoints(wxCommandEvent& evt);
	void OnSelchangeTitlePoints(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
