#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Import Wizard
 * @author David Connet
 *
 * Revision History
 * 2009-06-14 Fix wizard finish (wxEVT_WIZARD_FINISHED is only invoked
 *            _after_ the dialog is destroyed).
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-01-04 Implemented import, except for trials/runs.
 * 2003-12-10 Created
 */

#include "AgilityBookOptions.h"
#include <vector>
#include <wx/wizard.h>
class CAgilityBookDoc;
class CListCtrl;
class CTextCtrl;
class CWizard;
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
	wchar_t GetDelim() const;
	void UpdateButtons();
	void UpdatePreview();
	bool DoImportFile();
	bool DoWizardFinish();

	CWizard* m_pSheet;
	CAgilityBookDoc* m_pDoc;
	std::wstring m_FileName;
	std::vector<std::wstring> m_FileData; ///< For reading generic text files.
	std::vector< std::vector<std::wstring> > m_ExcelData; ///< For reading excel directly.
	long m_Row;
	wxSpinCtrl* m_ctrlRow;
	ARBImportExportDelim m_Delim;
	std::wstring m_Delimiter;
	wxStaticBox* m_boxDelimiters;
	wxRadioButton* m_ctrlTab;
	wxRadioButton* m_ctrlColon;
	wxRadioButton* m_ctrlComma;
	wxRadioButton* m_ctrlSpace;
	wxRadioButton* m_ctrlSemicolon;
	wxRadioButton* m_ctrlOther;
	CTextCtrl* m_ctrlOtherChar;
	wxButton* m_ctrlAssign;
	wxComboBox* m_ctrlDateFormat;
	wxStaticText* m_ctrlPreviewFile;
	CListCtrl* m_ctrlPreview;

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
