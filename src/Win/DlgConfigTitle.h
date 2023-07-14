#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgConfigTitle class
 * @author David Connet
 *
 * Revision History
 * 2013-01-13 Added new recurring title suffix style.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-01-11 Allow titles to be optionally entered multiple times.
 * 2004-01-05 Created.
 */

#include "ARB/ARBTypes2.h"
#include "LibARBWin/Logger.h"
class wxDatePickerCtrl;


namespace dconSoft
{
namespace ARBWin
{
class CTextCtrl;
} // namespace ARBWin


class CDlgConfigTitle : public wxDialog
{
public:
	CDlgConfigTitle(ARB::ARBConfigTitlePtr const& inTitle, wxWindow* pParent = nullptr);

private:
	void UpdateButtons();

	ARB::ARBConfigTitlePtr m_Title;
	wxString m_Name;
	bool m_Prefix;
	wxString m_LongName;
	wxString m_Desc;
	bool m_AllowMany;
	short m_MultipleStartAt;
	unsigned short m_MultipleInc;
	bool m_bShowFirst;
	bool m_DateFrom;
	bool m_DateTo;
	wxCheckBox* m_ctrlShowFirst;
	ARBWin::CTextCtrl* m_ctrlMultipleStartAt;
	ARBWin::CTextCtrl* m_ctrlMultipleInc;
	wxDatePickerCtrl* m_ctrlDateFrom;
	wxDatePickerCtrl* m_ctrlDateTo;
	wxComboBox* m_ctrlStyle;
	wxComboBox* m_ctrlSep;
	ARBWin::CStackLogger m_trace;

	void OnAllowMultiple(wxCommandEvent& evt);
	void OnCheck(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
