#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Export Wizard
 * @author David Connet
 *
 * Revision History
 * 2009-06-14 Fix wizard finish (wxEVT_WIZARD_FINISHED is only invoked
 *            _after_ the dialog is destroyed).
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-05-10 Place quotes are field on output as needed.
 * 2004-01-04 Added date format specification.
 * 2003-12-10 Created
 */

#include "AgilityBookOptions.h"
#include <wx/wizard.h>
class CAgilityBookDoc;
class CListCtrl;
class CTextCtrl;
class CWizard;


class CWizardExport : public wxWizardPageSimple
{
	DECLARE_CLASS(CWizardExport)
public:
	CWizardExport(
			CWizard* pSheet,
			CAgilityBookDoc* pDoc,
			wxWizardPage* prev);

private:
	CAgilityBookOptions::ColumnOrder GetColumnInfo() const;
	wchar_t GetDelim() const;
	void UpdateButtons();
	std::wstring AddPreviewData(
			long inLine,
			long inCol,
			std::wstring inData);
	void UpdatePreview();
	bool DoWizardFinish();

	CWizard* m_pSheet;
	CAgilityBookDoc* m_pDoc;
	long m_Delim;
	wxString m_Delimiter;
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
	CListCtrl* m_ctrlPreview;

	void OnDelimTab(wxCommandEvent& evt);
	void OnDelimSpace(wxCommandEvent& evt);
	void OnDelimColon(wxCommandEvent& evt);
	void OnDelimSemicolon(wxCommandEvent& evt);
	void OnDelimComma(wxCommandEvent& evt);
	void OnDelimOther(wxCommandEvent& evt);
	void OnExportDelim(wxCommandEvent& evt);
	void OnExportAssign(wxCommandEvent& evt);
	void OnSelchangeDate(wxCommandEvent& evt);
	void OnWizardChanging(wxWizardEvent& evt);
	void OnWizardChanged(wxWizardEvent& evt);
};
