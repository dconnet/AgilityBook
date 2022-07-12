#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgRegNum class
 * @author David Connet
 *
 * Revision History
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-06-29 Added Note to regnum.
 */

#include "ARB/ARBTypes2.h"


namespace dconSoft
{
class CVenueComboBox;


class CDlgRegNum : public wxDialog
{
public:
	CDlgRegNum(
		ARB::ARBConfig const& config,
		ARB::ARBDogRegNumList& regnums,
		ARB::ARBDogRegNumPtr const& inRegNum,
		wxWindow* pParent = nullptr);

private:
	ARB::ARBDogRegNumList& m_RegNums;
	ARB::ARBDogRegNumPtr m_pRegNum;
	CVenueComboBox* m_ctrlVenues;
	wxString m_Venue;
	wxString m_RegNum;
	wxString m_Height;
	bool m_bReceived;
	wxString m_Note;

	DECLARE_EVENT_TABLE()
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
