#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgTraining class
 * @author David Connet
 *
 * Revision History
 * 2009-02-09 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2003-09-21 Created
 */

#include "ARB/ARBTypes2.h"
#include "LibARBWin/Logger.h"
class wxDatePickerCtrl;


namespace dconSoft
{
class CAgilityBookDoc;


class CDlgTraining : public wxDialog
{
public:
	CDlgTraining(ARB::ARBTrainingPtr const& inTraining, CAgilityBookDoc* pDoc, wxWindow* pParent = nullptr);

private:
	ARB::ARBTrainingPtr m_pTraining;
	CAgilityBookDoc* m_pDoc;
	wxDatePickerCtrl* m_datePicker;
	wxString m_Name;
	wxString m_SubName;
	wxString m_Notes;
	ARBWin::CStackLogger m_trace;

	DECLARE_EVENT_TABLE()
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
