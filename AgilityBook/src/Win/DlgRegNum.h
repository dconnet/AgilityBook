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
class CVenueComboBox;


class CDlgRegNum : public wxDialog
{
public:
	CDlgRegNum(
			ARBConfig const& config,
			ARBDogRegNumList& regnums,
			ARBDogRegNumPtr pRegNum,
			wxWindow* pParent = NULL);

private:
	ARBDogRegNumList& m_RegNums;
	ARBDogRegNumPtr m_pRegNum;
	CVenueComboBox* m_ctrlVenues;
	wxString m_Venue;
	wxString m_RegNum;
	wxString m_Height;
	bool m_bReceived;
	wxString m_Note;

	DECLARE_ON_INIT()
	DECLARE_EVENT_TABLE()
	void OnOk(wxCommandEvent& evt);
};
