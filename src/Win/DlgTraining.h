#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgTraining class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2003-09-21 DRC Created
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
			wxWindow* pParent = NULL);

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
