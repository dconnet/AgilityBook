#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgCalendarQueryDetail class
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2007-10-05 Created
 */

#include "ARB/ARBTypes2.h"
#include <wx/listctrl.h>
#include <map>
#include <vector>


namespace dconSoft
{
class ARBConfigCalSiteList;
class CCheckListCtrl;


class CDlgCalendarQueryDetail : public wxDialog
{
public:
	// For configuring what codes are available
	CDlgCalendarQueryDetail(
		ARBConfigCalSiteList const& sites,
		std::map<std::wstring, std::wstring> const& inLocCodes,
		std::map<std::wstring, std::wstring> const& inVenueCodes,
		wxWindow* pParent = nullptr);
	// For selecting from available list
	CDlgCalendarQueryDetail(
		ARBConfigCalSiteList const& sites,
		std::map<std::wstring, std::wstring> const& inLocCodes,
		std::vector<std::wstring> const& inSelectedLocCodes,
		std::map<std::wstring, std::wstring> const& inVenueCodes,
		std::vector<std::wstring> const& inSelectedVenueCodes,
		wxWindow* pParent = nullptr);

	std::map<std::wstring, std::wstring> const& GetLocationCodes() const
	{
		return m_LocCodes;
	}
	std::map<std::wstring, std::wstring> const& GetVenueCodes() const
	{
		return m_VenueCodes;
	}
	std::vector<std::wstring> const& GetSelectedLocationCodes() const
	{
		return m_Locations;
	}
	std::vector<std::wstring> const& GetSelectedVenueCodes() const
	{
		return m_Venues;
	}

private:
	void Create(wxWindow* pParent);
	void UpdateButtons();
	void EditLocationCode();
	void EditVenueCode();

	bool m_EditCodes;
	ARBConfigCalSiteList const& m_sites;
	std::map<std::wstring, std::wstring> m_LocCodes;
	std::map<std::wstring, std::wstring> m_VenueCodes;
	std::vector<std::wstring> m_Locations;
	std::vector<std::wstring> m_Venues;
	CCheckListCtrl* m_ctrlLocations;
	CCheckListCtrl* m_ctrlVenues;
	wxButton* m_ctrlNewLoc;
	wxButton* m_ctrlEditLoc;
	wxButton* m_ctrlDeleteLoc;
	wxButton* m_ctrlNewVenue;
	wxButton* m_ctrlEditVenue;
	wxButton* m_ctrlDeleteVenue;

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

} // namespace dconSoft
