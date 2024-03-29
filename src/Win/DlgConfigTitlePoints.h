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

#include "ARB/ARBCalcPoints.h"
#include "ARB/ARBTypes2.h"
#include "LibARBWin/Logger.h"


namespace dconSoft
{
namespace ARBWin
{
class CTextCtrl;
} // namespace ARBWin

enum class ARBTitlePointType
{
	// These numbers correspond to the index in the choice list
	Normal = 0,
	Lifetime,
	Placement,
	Max
};


class CDlgConfigTitlePoints : public wxDialog
{
	void InitDlg(wxWindow* pParent);

public:
	CDlgConfigTitlePoints(
		ARB::ARBConfigVenuePtr const& inVenue,
		double inValue, // Faults or Place
		double inPoints,
		ARBTitlePointType inType,
		ARB::ARBPointsType inTypeNormal,
		wxWindow* pParent = nullptr);

	CDlgConfigTitlePoints(
		ARB::ARBConfigVenuePtr const& inVenue,
		ARB::ARBConfigLifetimePointsPtr const& inLifetime,
		wxWindow* pParent = nullptr);

	double Faults() const
	{
		return m_Faults;
	}
	short Place() const
	{
		return m_Place;
	}
	bool UseSpeedPts() const
	{
		return m_bSpeed;
	}
	double Points() const
	{
		return m_Points;
	}
	wxString LifetimeName() const
	{
		return m_LifetimeName;
	}
	ARBTitlePointType Type() const
	{
		return m_Type;
	}
	ARB::ARBPointsType TypeNormal() const
	{
		return m_TypeNormal;
	}

private:
	bool UpdateControls(ARBTitlePointType oldType);

	ARB::ARBConfigVenuePtr m_Venue;
	ARBTitlePointType m_Type;
	ARB::ARBPointsType m_TypeNormal;
	wxStaticText* m_textValue;
	ARBWin::CTextCtrl* m_ctrlValue;
	wxCheckBox* m_ctrlSpeedPts;
	wxStaticText* m_textPoints;
	ARBWin::CTextCtrl* m_ctrlPoints;
	wxChoice* m_ctrlType;
	wxChoice* m_ctrlTypeNormal;
	wxStaticText* m_textLifetimeName;
	wxComboBox* m_ctrlLifetimeName;
	double m_Faults;
	short m_Place;
	bool m_bSpeed;
	double m_Points;
	wxString m_LifetimeName;
	ARBWin::CStackLogger m_trace;

	void OnUseSpeedPoints(wxCommandEvent& evt);
	void OnSelchangeTitlePoints(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
