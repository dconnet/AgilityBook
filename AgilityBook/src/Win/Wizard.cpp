/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Import/Export Wizard
 * @author David Connet
 *
 * Revision History
 * @li 2009-07-24 DRC Center the wizard on the parent.
 * @li 2009-02-10 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2003-12-10 DRC Created
 */

#include "stdafx.h"
#include "Wizard.h"

#include "AgilityBook.h"
#include "WizardExport.h"
#include "WizardImport.h"
#include "WizardStart.h"


#ifdef WIN32
#pragma warning( push )
// Disable "warning C4355: 'this' : used in base member initializer list"
#pragma warning( disable : 4355 )
#endif
CWizard::CWizard(
		CAgilityBookDoc* pDoc,
		std::vector<ARBCalendarPtr>* pCalEntries,
		wxWindow* pParent)
	: wxWizard()
	, m_pDoc(pDoc)
	, m_CalEntries(pCalEntries)
	, m_pageStart(NULL)
	, m_pageImport(NULL)
	, m_pageExport(NULL)
	, m_Excel()
	, m_Calc()
	, m_ImportExportItem(-1)
	, m_ImportExportStyle(-1)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	Create(pParent, wxID_ANY, _("IDD_WIZARD_START"), wxNullBitmap, wxDefaultPosition, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	m_Finish = wxDynamicCast(FindWindow(wxID_FORWARD), wxButton);
	assert(m_Finish);

	m_Excel = IWizardSpreadSheet::Create(IWizardSpreadSheet::eMicrosoftExcel);
	m_Calc = IWizardSpreadSheet::Create(IWizardSpreadSheet::eOpenOfficeCalc);
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
#ifdef WIN32
#pragma warning( pop )
#endif


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
		hasNext = (NULL != cur->GetNext());
	// See <wx>/src/generic/wizard.cpp
	m_Finish->SetLabel(hasNext ? _("&Next >") : _("&Finish"));
	m_Finish->Enable(enableNextOrFinish);
}
