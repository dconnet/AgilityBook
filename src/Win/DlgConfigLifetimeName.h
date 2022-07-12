#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgConfigLifetimeName class
 * @author David Connet
 *
 * Revision History
 * 2016-06-19 Created.
 */

#include "ARB/ARBTypes2.h"


namespace dconSoft
{


class CDlgConfigLifetimeName : public wxDialog
{
public:
	// New
	CDlgConfigLifetimeName(ARB::ARBConfigVenuePtr const& inVenue, wxWindow* pParent = nullptr);
	// Existing
	CDlgConfigLifetimeName(
		ARB::ARBConfigVenuePtr const& inVenue,
		std::wstring const& inName,
		wxWindow* pParent = nullptr);

	std::wstring GetLifetimeName() const
	{
		return m_NewName;
	}

private:
	void InitDlg(wxWindow* pParent);

	ARB::ARBConfigVenuePtr m_Venue;
	bool m_bCheckOldName;
	wxString m_Name;
	std::wstring m_NewName;

protected:
	DECLARE_EVENT_TABLE()
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
