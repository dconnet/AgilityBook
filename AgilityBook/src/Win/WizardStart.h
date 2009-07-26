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
