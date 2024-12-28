#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Configuration updater
 * @author David Connet
 *
 * Revision History
 * 2009-02-10 Ported to wxWidgets.
 */

#include "ARB/ARBAgilityRecordBook.h"
#include "LibARBWin/Logger.h"


namespace dconSoft
{
namespace ARBWin
{
class CTextCtrl;
} // namespace ARBWin


class CDlgConfigUpdate : public wxDialog
{
public:
	CDlgConfigUpdate(wxWindow* parent = nullptr);

	ARB::ARBConfig& GetConfig()
	{
		return m_Book.GetConfig();
	}
	// This should not be called if the dialog was flown. It is intended
	// to allow loading config data without flying the dialog (for instance,
	// from the import menu item).
	// emptystring will load the default configuration.
	bool LoadConfig(wxString const& file);

private:
	void EnableControls();

	wxRadioButton* m_radioDefault;
	wxRadioButton* m_radioExisting;
	wxButton* m_btnPick;
	ARBWin::CTextCtrl* m_FileName;
	// CEdit	m_ctrlFileNameEdit;
	// CString	m_FileName;
	// CButton	m_ctrlFileName;
	ARB::ARBAgilityRecordBook m_Book;
	ARBWin::CStackLogger m_trace;

	void OnUpdateDefault(wxCommandEvent& evt);
	void OnUpdateExisting(wxCommandEvent& evt);
	void OnName(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
