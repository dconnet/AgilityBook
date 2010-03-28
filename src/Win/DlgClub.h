#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgClub class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2008-02-01 DRC Only modify existing club, don't add a new one (caller does).
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "ARBTypes.h"
class CAgilityBookDoc;
class CVenueComboBox;


class CDlgClub : public wxDialog
{
public:
	CDlgClub(
			CAgilityBookDoc* pDoc,
			ARBDogClubList& inClubs,
			ARBDogClubPtr pClub,
			wxWindow* pParent = NULL);

	wxString Club() const	{return m_Club;}
	wxString Venue() const	{return m_Venue;}

private:
	CAgilityBookDoc* m_pDoc;
	ARBDogClubPtr m_pClub;
	wxComboBox* m_ctrlClubs;
	CVenueComboBox* m_ctrlVenues;
	wxString m_Club;
	wxString m_Venue;

	DECLARE_EVENT_TABLE()
	void OnOk(wxCommandEvent& evt);
};
