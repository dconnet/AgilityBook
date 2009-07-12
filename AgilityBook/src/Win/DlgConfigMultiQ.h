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
 * @brief interface of the CDlgConfigMultiQ class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-07-30 DRC Created.
 */

#include "ARBDate.h"
class wxDatePickerCtrl;
class wxListView;
class wxListEvent;


class CDlgConfigMultiQ : public wxDialog
{
public:
	CDlgConfigMultiQ(
			ARBConfigVenuePtr inVenue,
			ARBConfigMultiQPtr inMultiQ,
			wxWindow* pParent = NULL);

private:
	void UpdateControls();
	void EditItem();

	ARBConfigVenuePtr m_pVenue;
	ARBConfigMultiQPtr m_pOrigMultiQ;
	ARBConfigMultiQPtr m_pMultiQ;
	wxString m_Name;
	wxString m_ShortName;
	bool m_bFrom;
	wxDatePickerCtrl* m_ctrlDateFrom;
	ARBDate	m_DateFrom;
	bool	m_bTo;
	wxDatePickerCtrl* m_ctrlDateTo;
	ARBDate	m_DateTo;
	wxTextCtrl* m_ctrlName;
	wxListView* m_ctrlItems;
	wxButton* m_ctrlEdit;
	wxButton* m_ctrlRemove;

	DECLARE_EVENT_TABLE()
	void OnItemchanged(wxListEvent& evt);
	void OnItemActivated(wxListEvent& evt);
	void OnKeydownItem(wxKeyEvent& evt);
	void OnClickFrom(wxCommandEvent& evt);
	void OnClickTo(wxCommandEvent& evt);
	void OnAdd(wxCommandEvent& evt);
	void OnEdit(wxCommandEvent& evt);
	void OnRemove(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
