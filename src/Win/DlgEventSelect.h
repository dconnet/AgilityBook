#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgEventSelect class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-07-30 DRC Created.
 */

#include "ARBDate.h"


class CDlgEventSelect : public wxDialog
{
public:
	CDlgEventSelect(
			ARBConfigVenuePtr inVenue,
			ARBDate const& inDate,
			wxString const& inDivision,
			wxString const& inLevel,
			wxString const& inEvent,
			wxWindow* pParent = NULL);

	wxString GetDivision() const	{return m_Division;}
	wxString GetLevel() const		{return m_Level;}
	wxString GetEvent() const		{return m_Event;}

private:
	void UpdateControls();
	void FillLevels();
	void FillEvents();

	wxComboBox* m_ctrlDivisions;
	wxComboBox* m_ctrlLevels;
	wxComboBox* m_ctrlEvents;
	wxButton* m_ctrlOk;
	wxString m_Division;
	wxString m_Level;
	wxString m_Event;
	wxString m_inLevel;
	wxString m_inEvent;
	ARBConfigVenuePtr m_pVenue;
	ARBDate m_Date;

	DECLARE_EVENT_TABLE()
	void OnSelchangeDivision(wxCommandEvent& evt);
	void OnSelchangeLevel(wxCommandEvent& evt);
	void OnSelchangeEvent(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
