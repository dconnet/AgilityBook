#pragma once

/*
 * Copyright © 2003-2004 David Connet. All Rights Reserved.
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

#include "DlgBaseSheet.h"
#include "WizardExport.h"
#include "WizardImport.h"
#include "WizardStart.h"
class CAgilityBookDoc;

// These must agree with the order of items in sc_Items in WizardStart.cpp.
// They are simply for ease-of-coding.
#define WIZ_IMPORT_RUNS				0
#define WIZ_EXPORT_RUNS				1
#define WIZ_IMPORT_CALENDAR			2
#define WIZ_EXPORT_CALENDAR			3
#define WIZ_IMPORT_LOG				4
#define WIZ_EXPORT_LOG				5
#define WIZ_IMPORT_CONFIGURATION	6
#define WIZ_EXPORT_CONFIGURATION	7
#define WIZ_EXPORT_DTD				8
#define WIZ_EXPORT_XML				9

class CWizard : public CDlgBaseSheet
{
	DECLARE_DYNAMIC(CWizard)

// Construction
public:
	CWizard(CAgilityBookDoc* pDoc, CWnd* pParentWnd = NULL);

// Attributes
private:
	CAgilityBookDoc* m_pDoc;
	CWizardStart m_pageStart;
	CWizardImport m_pageImport;
	CWizardExport m_pageExport;
	int m_ImportExportItem;

// Operations
public:
	/**
	 * This lets CWizardImport/Export know what data is being processed.
	 */
	int GetImportExportItem() const		{return m_ImportExportItem;}
	/**
	 * Only CWizardStart should call this.
	 */
	void SetImportExportItem(int item)	{m_ImportExportItem = item;}

// Overrides
	//{{AFX_VIRTUAL(CWizard)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWizard();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWizard)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
