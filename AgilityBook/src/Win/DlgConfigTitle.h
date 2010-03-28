#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgConfigTitle class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-01-05 DRC Created.
 */

#include "ARBTypes.h"
class CTextCtrl;
class wxDatePickerCtrl;


class CDlgConfigTitle : public wxDialog
{
public:
	CDlgConfigTitle(
			ARBConfigTitlePtr inTitle,
			wxWindow* pParent = NULL);

private:
	ARBConfigTitlePtr m_Title;
	wxString m_Name;
	bool m_Prefix;
	wxString m_LongName;
	wxString m_Desc;
	bool m_AllowMany;
	short m_Multiple;
	bool m_DateFrom;
	bool m_DateTo;
	CTextCtrl* m_ctrlMultiple;
	wxDatePickerCtrl* m_ctrlDateFrom;
	wxDatePickerCtrl* m_ctrlDateTo;
	wxComboBox* m_ctrlStyle;

protected:
	void UpdateButtons();

	DECLARE_EVENT_TABLE()
	void OnAllowMultiple(wxCommandEvent& evt);
	void OnCheck(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
