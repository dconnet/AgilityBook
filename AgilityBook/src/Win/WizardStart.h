#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief Import/Export Wizard
 * @author David Connet
 *
 * Revision History
 * @li 2009-06-14 DRC Fix wizard finish (wxEVT_WIZARD_FINISHED is only invoked
 *                    _after_ the dialog is destroyed).
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2003-12-10 DRC Created
 */

#include <wx/wizard.h>
class CAgilityBookDoc;
class CWizard;


class CWizardStart : public wxWizardPage
{
	DECLARE_CLASS(CWizardStart)
public:
	CWizardStart(
			CWizard* pSheet,
			CAgilityBookDoc* pDoc);

	wxWizardPage* GetPrev() const	{return NULL;}
	wxWizardPage* GetNext() const	{return m_Next;}

private:
	void UpdateList(bool bInit = false);
	void DoUpdateExportList(bool bInit = false);
	bool DoWizardFinish();

	CWizard* m_pSheet;
	CAgilityBookDoc* m_pDoc;
	wxListBox* m_ctrlList;
	wxStaticText* m_ctrlDesc;
	long m_Style;
	wxWizardPage* m_Next;

	void OnWizardStyleExcel(wxCommandEvent& evt);
	void OnWizardStyleCalc(wxCommandEvent& evt);
	void OnWizardStyleSpread(wxCommandEvent& evt);
	void OnWizardStyleArb(wxCommandEvent& evt);
	void OnSelchangeExportList(wxCommandEvent& evt);
	void OnWizardChanging(wxWizardEvent& evt);
	void OnWizardChanged(wxWizardEvent& evt);
};