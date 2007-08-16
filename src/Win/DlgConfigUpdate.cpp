/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-01-26 DRC Display errors on non-fatal load.
 * @li 2003-12-07 DRC Changed Load/Save api to support new info section.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgConfigUpdate.h"

#include "AgilityBookDoc.h"
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
	: CDlgBaseDialog(CDlgConfigUpdate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgConfigUpdate)
	m_Update = 0;
	m_FileName = _T("");
	//}}AFX_DATA_INIT
}

void CDlgConfigUpdate::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConfigUpdate)
	DDX_Radio(pDX, IDC_CONFIG_UPDATE_DEFAULT, m_Update);
	DDX_Control(pDX, IDC_CONFIG_UPDATE_FILENAME, m_ctrlFileNameEdit);
	DDX_Text(pDX, IDC_CONFIG_UPDATE_FILENAME, m_FileName);
	DDX_Control(pDX, IDC_CONFIG_UPDATE_NAME, m_ctrlFileName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgConfigUpdate, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgConfigUpdate)
	ON_BN_CLICKED(IDC_CONFIG_UPDATE_DEFAULT, OnUpdateDefault)
	ON_BN_CLICKED(IDC_CONFIG_UPDATE_EXISTING, OnUpdateExisting)
	ON_BN_CLICKED(IDC_CONFIG_UPDATE_NAME, OnName)
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

bool CDlgConfigUpdate::LoadConfig(TCHAR const* pFile)
{
	CWaitCursor wait;
	if (!pFile)
		m_Book.GetConfig().Default();
	else
	{
		ARBString errMsg;
		ElementNodePtr tree(ElementNode::New());
		// Translate the XML to a tree form.
		CStringA fileName(pFile);
		if (!tree->LoadXMLFile(fileName, errMsg))
		{
			CString msg;
			msg.LoadString(AFX_IDP_FAILED_TO_OPEN_DOC);
			if (0 < errMsg.length())
			{
				msg += _T("\n\n");
				msg += errMsg.c_str();
			}
			AfxMessageBox(msg, MB_ICONEXCLAMATION);
			return false;
		}
		CErrorCallback err;
		if (!m_Book.Load(tree, false, false, true, false, false, err))
		{
			if (0 < err.m_ErrMsg.length())
				AfxMessageBox(err.m_ErrMsg.c_str(), MB_ICONWARNING);
			return false;
		}
		else if (0 < err.m_ErrMsg.length())
			AfxMessageBox(err.m_ErrMsg.c_str(), MB_ICONINFORMATION);
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigUpdate message handlers

BOOL CDlgConfigUpdate::OnInitDialog() 
{
	CDlgBaseDialog::OnInitDialog();

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
	LPCTSTR pFile = NULL;
	CString source(m_FileName);
	if (1 == m_Update)
		pFile = source;
	if (!LoadConfig(pFile))
		return;
	CDlgBaseDialog::OnOK();
}
