#pragma once

/*
 * Copyright © 2005-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
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
