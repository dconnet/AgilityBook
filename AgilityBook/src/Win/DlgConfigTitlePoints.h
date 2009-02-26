#pragma once

/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
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
	wxTextCtrl* m_ctrlValue;
	wxChoice* m_ctrlType;
	double m_Faults;
	short m_Place;
	double m_Points;

	void OnSelchangeTitlePoints(wxCommandEvent& evt);
};
