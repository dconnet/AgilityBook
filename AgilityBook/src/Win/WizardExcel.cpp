/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * @brief Import/Export Wizard for Excel
 * @author David Connet
 *
 * Revision History
 * @li 2004-09-30 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "WizardExcel.h"

#include "excel8.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizardExcelImpl

/**
 * Hide all OLE implmentation details.
 */
class CWizardExcelImpl
{
public:
	CWizardExcelImpl();
	bool IsOkay() const		{return NULL != app.m_lpDispatch;}

	Excel8::_Application app;
};

CWizardExcelImpl::CWizardExcelImpl()
{
	// Get the ClassID from the ProgID.
	CLSID clsid;
	if (NOERROR != CLSIDFromProgID(L"Excel.Application", &clsid))
	{
		return;
	}

	// Get an interface to the running instance.
	//LPUNKNOWN lpUnk;
	//LPDISPATCH lpDispatch;
	//if (NOERROR == GetActiveObject(clsid, NULL, &lpUnk))
	//{
	//	HRESULT hr = lpUnk->QueryInterface(IID_IDispatch, (LPVOID*)&lpDispatch);
	//	lpUnk->Release();
	//	if (hr == NOERROR)
	//		app.AttachDispatch(lpDispatch, TRUE);
	//}

	// If dispatch ptr not attached yet, need to create one
	COleException e;
	if (app.m_lpDispatch == NULL && !app.CreateDispatch(clsid, &e))
	{
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWizardExcel

CWizardExcel::CWizardExcel()
	: m_Excel(NULL)
{
	m_Excel = new CWizardExcelImpl();
	if (m_Excel && !m_Excel->IsOkay())
	{
		delete m_Excel;
		m_Excel = NULL;
	}
}

CWizardExcel::~CWizardExcel()
{
	delete m_Excel;
}

bool CWizardExcel::IsAvailable() const
{
	return (m_Excel && m_Excel->IsOkay());
}
