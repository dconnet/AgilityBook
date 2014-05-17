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
class CAgilityBookDoc;
class wxDatePickerCtrl;


class CDlgTraining : public wxDialog
{
public:
	CDlgTraining(
			ARBTrainingPtr pTraining,
			CAgilityBookDoc* pDoc,
			wxWindow* pParent = nullptr);

private:
	ARBTrainingPtr m_pTraining;
	CAgilityBookDoc* m_pDoc;
	wxDatePickerCtrl* m_datePicker;
	wxString m_Name;
	wxString m_SubName;
	wxString m_Notes;

	DECLARE_EVENT_TABLE()
	void OnOk(wxCommandEvent& evt);
};
