#pragma once

/*
 * Copyright (c) 2007-2009 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgCalendarQueryDetail class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2007-10-05 DRC Created
 */

#include "ARBTypes.h"
#include <map>
#include <vector>
class ARBConfig;
class CCheckListCtrl;


class CDlgCalendarQueryDetail : public wxDialog
{
public:
	// For configuring what codes are available
	CDlgCalendarQueryDetail(
			ARBConfig const& inConfig,
			std::map<tstring, tstring> const& inLocCodes,
			std::map<tstring, tstring> const& inVenueCodes,
			wxWindow* pParent = NULL);
	// For selecting from available list
	CDlgCalendarQueryDetail(
			ARBConfig const& inConfig,
			std::map<tstring, tstring> const& inLocCodes,
			std::vector<tstring> const& inSelectedLocCodes,
			std::map<tstring, tstring> const& inVenueCodes,
			std::vector<tstring> const& inSelectedVenueCodes,
			wxWindow* pParent = NULL);

	std::map<tstring, tstring> const& GetLocationCodes() const
	{
		return m_LocCodes;
	}
	std::map<tstring, tstring> const& GetVenueCodes() const
	{
		return m_VenueCodes;
	}
	std::vector<tstring> const& GetSelectedLocationCodes() const
	{
		return m_Locations;
	}
	std::vector<tstring> const& GetSelectedVenueCodes() const
	{
		return m_Venues;
	}

private:
	void Create(wxWindow* pParent);
	void UpdateButtons();
	void EditLocationCode();
	void EditVenueCode();

	bool m_EditCodes;
	ARBConfig const& m_Config;
	std::map<tstring, tstring> m_LocCodes;
	std::map<tstring, tstring> m_VenueCodes;
	std::vector<tstring> m_Locations;
	std::vector<tstring> m_Venues;
	CCheckListCtrl* m_ctrlLocations;
	CCheckListCtrl* m_ctrlVenues;
	wxButton* m_ctrlNewLoc;
	wxButton* m_ctrlEditLoc;
	wxButton* m_ctrlDeleteLoc;
	wxButton* m_ctrlNewVenue;
	wxButton* m_ctrlEditVenue;
	wxButton* m_ctrlDeleteVenue;

	DECLARE_EVENT_TABLE()
	void OnLocationsItemSelected(wxListEvent& evt);
	void OnLocationsItemActivated(wxListEvent& evt);
	void OnVenuesItemSelected(wxListEvent& evt);
	void OnVenuesItemActivated(wxListEvent& evt);
	void OnNewLocationCode(wxCommandEvent& evt);
	void OnEditLocationCode(wxCommandEvent& evt);
	void OnDeleteLocationCode(wxCommandEvent& evt);
	void OnNewVenueCode(wxCommandEvent& evt);
	void OnEditVenueCode(wxCommandEvent& evt);
	void OnDeleteVenueCode(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
