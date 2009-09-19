#pragma once

/*
 * Copyright (c) 2003-2009 David Connet. All Rights Reserved.
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
 * @brief Import Wizard
 * @author David Connet
 *
 * Revision History
 * @li 2009-06-14 DRC Fix wizard finish (wxEVT_WIZARD_FINISHED is only invoked
 *                    _after_ the dialog is destroyed).
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-01-04 DRC Implemented import, except for trials/runs.
 * @li 2003-12-10 DRC Created
 */

#include "AgilityBookOptions.h"
#include <vector>
#include <wx/wizard.h>
class CAgilityBookDoc;
class CWizard;
class wxListCtrl;
class wxSpinCtrl;
class wxSpinEvent;


class CWizardImport : public wxWizardPageSimple
{
	DECLARE_CLASS(CWizardImport)
public:
	CWizardImport(
			CWizard* pSheet,
			CAgilityBookDoc* pDoc,
			wxWizardPage* prev);
	void ResetData();

private:
	CAgilityBookOptions::ColumnOrder GetColumnInfo() const;
	wxString GetDelim() const;
	void UpdateButtons();
	void UpdatePreview();
	bool DoImportFile();
	bool DoWizardFinish();

	CWizard* m_pSheet;
	CAgilityBookDoc* m_pDoc;
	wxString m_FileName;
	std::vector<wxString> m_FileData; ///< For reading generic text files.
	std::vector< std::vector<wxString> > m_ExcelData; ///< For reading excel directly.
	long m_Row;
	wxSpinCtrl* m_ctrlRow;
	long m_Delim;
	wxString m_Delimiter;
	wxTextCtrl* m_ctrlOtherChar;
	wxButton* m_ctrlAssign;
	wxComboBox* m_ctrlDateFormat;
	wxStaticText* m_ctrlPreviewFile;
	wxListCtrl* m_ctrlPreview;

	void OnDeltaposImportRowSpin(wxSpinEvent& evt);
	void OnDelimTab(wxCommandEvent& evt);
	void OnDelimSpace(wxCommandEvent& evt);
	void OnDelimColon(wxCommandEvent& evt);
	void OnDelimSemicolon(wxCommandEvent& evt);
	void OnDelimComma(wxCommandEvent& evt);
	void OnDelimOther(wxCommandEvent& evt);
	void OnImportDelim(wxCommandEvent& evt);
	void OnImportAssign(wxCommandEvent& evt);
	void OnImportFile(wxCommandEvent& evt);
	void OnWizardChanging(wxWizardEvent& evt);
	void OnWizardChanged(wxWizardEvent& evt);
};
