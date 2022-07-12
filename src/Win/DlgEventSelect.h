#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgEventSelect class
 * @author David Connet
 *
 * Revision History
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-07-30 Created.
 */

#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"


namespace dconSoft
{


class CDlgEventSelect : public wxDialog
{
public:
	CDlgEventSelect(
		ARB::ARBConfigVenuePtr const& inVenue,
		ARBCommon::ARBDate const& inDate,
		std::wstring const& inDivision,
		std::wstring const& inLevel,
		std::wstring const& inEvent,
		wxWindow* pParent = nullptr);

	std::wstring GetDivision() const;
	std::wstring GetLevel() const;
	std::wstring GetEvent() const;

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
	ARB::ARBConfigVenuePtr m_pVenue;
	ARBCommon::ARBDate m_Date;

	DECLARE_EVENT_TABLE()
	void OnSelchangeDivision(wxCommandEvent& evt);
	void OnSelchangeLevel(wxCommandEvent& evt);
	void OnSelchangeEvent(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
