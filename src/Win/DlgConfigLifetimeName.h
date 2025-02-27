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
#include "LibARBWin/Logger.h"


namespace dconSoft
{


class CDlgConfigLifetimeName : public wxDialog
{
public:
	// New
	CDlgConfigLifetimeName(ARB::ARBConfigVenuePtr const& inVenue, wxWindow* pParent = nullptr);
	// Existing
	CDlgConfigLifetimeName(ARB::ARBConfigVenuePtr const& inVenue, wxString const& inName, wxWindow* pParent = nullptr);

	wxString GetLifetimeName() const
	{
		return m_NewName;
	}

private:
	void InitDlg(wxWindow* pParent);

	ARB::ARBConfigVenuePtr m_Venue;
	bool m_bCheckOldName;
	wxString m_Name;
	wxString m_NewName;
	ARBWin::CStackLogger m_trace;

	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
