#pragma once

/*
 * Copyright © 2003 David Connet. All Rights Reserved.
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

#include "WizardExport.h"
#include "WizardImport.h"
#include "WizardStart.h"
class CAgilityBookDoc;

class CWizard : public CPropertySheet
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

// Operations
public:

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
