#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgConfigTitlePoints class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-10-06 DRC Removed ARB classes so it could be used to lifetime pts.
 */

#include "ARBTypes.h"
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
			wxWindow* pParent = NULL);

	double Faults() const			{return m_Faults;}
	short Place() const				{return m_Place;}
	double Points() const			{return m_Points;}
	ETitlePointType Type() const	{return static_cast<ETitlePointType>(m_Type);}

private:
	ARBConfigVenuePtr m_Venue;
	int m_Type;
	wxStaticText* m_textValue;
	CTextCtrl* m_ctrlValue;
	wxChoice* m_ctrlType;
	double m_Faults;
	short m_Place;
	double m_Points;

	void OnSelchangeTitlePoints(wxCommandEvent& evt);
};
