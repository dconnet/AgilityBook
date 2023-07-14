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
#include "LibARBWin/Logger.h"
class wxDatePickerCtrl;
class wxListEvent;


namespace dconSoft
{
namespace ARBWin
{
class CListCtrl;
class CTextCtrl;
} // namespace ARBWin


class CDlgConfigMultiQ : public wxDialog
{
public:
	CDlgConfigMultiQ(
		ARB::ARBConfigVenuePtr const& inVenue,
		ARB::ARBConfigMultiQPtr const& inMultiQ,
		wxWindow* pParent = nullptr);

private:
	void UpdateControls();
	void EditItem();

	ARB::ARBConfigVenuePtr m_pVenue;
	ARB::ARBConfigMultiQPtr m_pOrigMultiQ;
	ARB::ARBConfigMultiQPtr m_pMultiQ;
	wxString m_Name;
	wxString m_ShortName;
	bool m_bFrom;
	wxDatePickerCtrl* m_ctrlDateFrom;
	ARBCommon::ARBDate m_DateFrom;
	bool m_bTo;
	wxDatePickerCtrl* m_ctrlDateTo;
	ARBCommon::ARBDate m_DateTo;
	ARBWin::CTextCtrl* m_ctrlName;
	ARBWin::CListCtrl* m_ctrlItems;
	wxButton* m_ctrlEdit;
	wxButton* m_ctrlRemove;
	ARBWin::CStackLogger m_trace;

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

} // namespace dconSoft
