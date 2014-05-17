#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgClub class
 * @author David Connet
 *
 * Revision History
 * 2012-05-07 Added autocompletion to combo boxes.
 * 2009-02-11 Ported to wxWidgets.
 * 2008-02-01 Only modify existing club, don't add a new one (caller does).
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "ARB/ARBTypes2.h"
class CAutoFillComboBox;
class CAgilityBookDoc;
class CVenueComboBox;


class CDlgClub : public wxDialog
{
public:
	CDlgClub(
			CAgilityBookDoc* pDoc,
			ARBDogClubList& inClubs,
			ARBDogClubPtr pClub,
			wxWindow* pParent = nullptr);

	std::wstring Club() const;
	std::wstring Venue() const;

private:
	CAgilityBookDoc* m_pDoc;
	ARBDogClubPtr m_pClub;
	CAutoFillComboBox* m_ctrlClubs;
	CVenueComboBox* m_ctrlVenues;
	wxString m_Club;
	wxString m_Venue;

	DECLARE_ON_INIT()
	DECLARE_EVENT_TABLE()
	void OnOk(wxCommandEvent& evt);
};
