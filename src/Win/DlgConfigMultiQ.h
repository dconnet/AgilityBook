#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgConfigMultiQ class
 * @author David Connet
 *
 * Revision History
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-07-30 Created.
 */

#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"
class CListCtrl;
class CTextCtrl;
class wxDatePickerCtrl;
class wxListEvent;


class CDlgConfigMultiQ : public wxDialog
{
public:
	CDlgConfigMultiQ(ARBConfigVenuePtr const& inVenue, ARBConfigMultiQPtr const& inMultiQ, wxWindow* pParent = nullptr);

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
	ARBDate m_DateFrom;
	bool m_bTo;
	wxDatePickerCtrl* m_ctrlDateTo;
	ARBDate m_DateTo;
	CTextCtrl* m_ctrlName;
	CListCtrl* m_ctrlItems;
	wxButton* m_ctrlEdit;
	wxButton* m_ctrlRemove;

	DECLARE_ON_INIT()
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
