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


class CDlgConfigLifetimeName : public wxDialog
{
public:
	// New
	CDlgConfigLifetimeName(
			ARBConfigVenuePtr inVenue,
			wxWindow* pParent = nullptr);
	// Existing
	CDlgConfigLifetimeName(
		ARBConfigVenuePtr inVenue,
		std::wstring const& inName,
		wxWindow* pParent = nullptr);

	std::wstring GetLifetimeName() const	{return m_NewName;}

private:
	void Init(wxWindow* pParent);

	ARBConfigVenuePtr m_Venue;
	bool m_bCheckOldName;
	wxString m_Name;
	std::wstring m_NewName;

protected:

	DECLARE_ON_INIT()
	DECLARE_EVENT_TABLE()
	void OnOk(wxCommandEvent& evt);
};
