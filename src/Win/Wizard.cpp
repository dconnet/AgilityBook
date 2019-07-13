/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Import/Export Wizard
 * @author David Connet
 *
 * Revision History
 * 2009-07-24 Center the wizard on the parent.
 * 2009-02-10 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2003-12-10 Created
 */

#include "stdafx.h"
#include "Wizard.h"

#include "AgilityBook.h"
#include "WizardExport.h"
#include "WizardImport.h"
#include "WizardStart.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


#include "Platform/arbWarningPush.h"
CWizard::CWizard(
		CAgilityBookDoc* pDoc,
		std::vector<ARBCalendarPtr>* pCalEntries,
		wxWindow* pParent)
	: wxWizard()
	, m_pDoc(pDoc)
	, m_CalEntries(pCalEntries)
	, m_pageStart(nullptr)
	, m_pageImport(nullptr)
	, m_pageExport(nullptr)
	, m_Excel()
	, m_Calc()
	, m_ImportExportItem(-1)
	, m_ImportExportStyle(-1)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_WIZARD_START"), wxNullBitmap, wxDefaultPosition, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	m_Finish = wxDynamicCast(FindWindow(wxID_FORWARD), wxButton);
	assert(m_Finish);

	m_Excel = ISpreadSheet::Create(ARBSpreadSheetType::MicrosoftExcel);
	m_Calc = ISpreadSheet::Create(ARBSpreadSheetType::OpenOfficeCalc);
	// Have to delay these as their ctor will use m_Excel.
	m_pageStart = new CWizardStart(this, m_pDoc);
	m_pageImport = new CWizardImport(this, m_pDoc, m_pageStart);
	m_pageExport = new CWizardExport(this, m_pDoc, m_pageStart);

	GetPageAreaSizer()->Add(m_pageStart);
	GetPageAreaSizer()->Add(m_pageImport);
	GetPageAreaSizer()->Add(m_pageExport);

	CenterOnParent();
	RunWizard(m_pageStart);
	Destroy();
}
#include "Platform/arbWarningPop.h"


wxWizardPage* CWizard::GetImportPage() const
{
	return m_pageImport;
}


wxWizardPage* CWizard::GetExportPage() const
{
	return m_pageExport;
}


void CWizard::ResetData()
{
	if (m_pageImport)
		m_pageImport->ResetData();
}


void CWizard::UpdateButtons(bool enableNextOrFinish)
{
	bool hasNext = false;
	wxWizardPage* cur = GetCurrentPage();
	if (cur)
		hasNext = !!cur->GetNext();
	// See <wx>/src/generic/wizard.cpp
	m_Finish->SetLabel(hasNext ? _("&Next >") : _("&Finish"));
	m_Finish->Enable(enableNextOrFinish);
}
