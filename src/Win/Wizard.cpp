/*
 * Copyright © 2003-2005 David Connet. All Rights Reserved.
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
 * @li 2003-12-10 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "Wizard.h"

#include "WizardExport.h"
#include "WizardImport.h"
#include "WizardStart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizard

IMPLEMENT_DYNAMIC(CWizard, CDlgBaseSheet)

#pragma warning( push )
// Disable "warning C4355: 'this' : used in base member initializer list"
#pragma warning( disable : 4355 )
CWizard::CWizard(
		CAgilityBookDoc* pDoc,
		ARBVectorBase<ARBCalendar>* pCalEntries,
		CWnd* pParentWnd)
	: CDlgBaseSheet(_T(""), pParentWnd, 0)
	, m_pDoc(pDoc)
	, m_CalEntries(pCalEntries)
	, m_pageStart(NULL)
	, m_pageImport(NULL)
	, m_pageExport(NULL)
	, m_Excel()
	, m_ImportExportItem(-1)
	, m_ImportExportStyle(-1)
{
	m_psh.dwFlags |= PSH_WIZARDCONTEXTHELP;
	SetWizardMode();
	// Have to delay these as their ctor will use m_Excel.
	m_pageStart = new CWizardStart(this, m_pDoc);
	m_pageImport = new CWizardImport(this, m_pDoc);
	m_pageExport = new CWizardExport(this, m_pDoc);
	AddPage(m_pageStart);
	AddPage(m_pageImport);
	AddPage(m_pageExport);
}
#pragma warning( pop )

CWizard::~CWizard()
{
	delete m_pageStart;
	delete m_pageImport;
	delete m_pageExport;
}

BEGIN_MESSAGE_MAP(CWizard, CDlgBaseSheet)
	//{{AFX_MSG_MAP(CWizard)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CWizard::ResetData()
{
	if (m_pageImport)
		m_pageImport->ResetData();
}

/////////////////////////////////////////////////////////////////////////////
// CWizard message handlers
