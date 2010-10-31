#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Export Wizard
 * @author David Connet
 *
 * Revision History
 * @li 2009-06-14 DRC Fix wizard finish (wxEVT_WIZARD_FINISHED is only invoked
 *                    _after_ the dialog is destroyed).
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-05-10 DRC Place quotes are field on output as needed.
 * @li 2004-01-04 DRC Added date format specification.
 * @li 2003-12-10 DRC Created
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
	wxChar GetDelim() const;
	void UpdateButtons();
	wxString AddPreviewData(
			long inLine,
			long inCol,
			wxString inData);
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
