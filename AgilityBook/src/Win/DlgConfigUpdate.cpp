/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 * @brief Configuration updater
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigUpdate.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigUpdate dialog

CDlgConfigUpdate::CDlgConfigUpdate(CWnd* pParent)
	: CDialog(CDlgConfigUpdate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgConfigUpdate)
	m_Update = 0;
	m_FileName = _T("");
	//}}AFX_DATA_INIT
}

void CDlgConfigUpdate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigUpdate)
	DDX_Radio(pDX, IDC_UPDATE_DEFAULT, m_Update);
	DDX_Control(pDX, IDC_FILENAME, m_ctrlFileNameEdit);
	DDX_Text(pDX, IDC_FILENAME, m_FileName);
	DDX_Control(pDX, IDC_NAME, m_ctrlFileName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigUpdate, CDialog)
	//{{AFX_MSG_MAP(CDlgConfigUpdate)
	ON_BN_CLICKED(IDC_UPDATE_DEFAULT, OnUpdateDefault)
	ON_BN_CLICKED(IDC_UPDATE_EXISTING, OnUpdateExisting)
	ON_BN_CLICKED(IDC_NAME, OnName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgConfigUpdate::EnableControls()
{
	if (0 == m_Update)
	{
		m_ctrlFileNameEdit.EnableWindow(FALSE);
		m_ctrlFileName.EnableWindow(FALSE);
	}
	else
	{
		m_ctrlFileNameEdit.EnableWindow(TRUE);
		m_ctrlFileName.EnableWindow(TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////

bool CDlgConfigUpdate::LoadConfig(const char* pFile)
{
	CWaitCursor wait;
	if (!pFile)
		m_Book.GetConfig().Default();
	else
	{
		CElement tree;
		// Translate the XML to a tree form.
		if (!tree.LoadXMLFile(pFile))
		{
			AfxMessageBox(AFX_IDP_FAILED_TO_OPEN_DOC);
			return false;
		}
		if (!m_Book.Load(tree, false, false, true, false))
			return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigUpdate message handlers

BOOL CDlgConfigUpdate::OnInitDialog() 
{
	CDialog::OnInitDialog();

	EnableControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgConfigUpdate::OnUpdateDefault() 
{
	UpdateData(TRUE);
	EnableControls();
}

void CDlgConfigUpdate::OnUpdateExisting() 
{
	UpdateData(TRUE);
	EnableControls();
}

void CDlgConfigUpdate::OnName() 
{
	CString def, fname, filter;
	def.LoadString(IDS_FILEEXT_DEF_ARB);
	fname.LoadString(IDS_FILEEXT_FNAME_ARB);
	filter.LoadString(IDS_FILEEXT_FILTER_ARB);
	CFileDialog file(TRUE, def, fname, OFN_FILEMUSTEXIST, filter);
	if (IDOK == file.DoModal())
	{
		m_FileName = file.GetPathName();
		UpdateData(FALSE);
	}
}

void CDlgConfigUpdate::OnOK() 
{
	if (!UpdateData(TRUE))
		return;
	const char* pFile = NULL;
	// We need a char*, even in UNICODE.
#if _MSC_VER < 1300
	// But since I'm not compiling in unicode...
	CString source(m_FileName);
#else
	CStringA source(m_FileName);
#endif
	if (1 == m_Update)
		pFile = (const char*)source;
	if (!LoadConfig(pFile))
		return;
	CDialog::OnOK();
}
