/*
 * Copyright © 2007-2008 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgLanguage class
 * @author David Connet
 *
 * Revision History
 * @li 2007-09-22 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgLanguage.h"

#include "LanguageManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLanguage dialog

CDlgLanguage::CDlgLanguage(CLanguageManager& langMgr, CWnd* pParent)
	: CDlgBaseDialog(CDlgLanguage::IDD, pParent)
	, m_LangMgr(langMgr)
	, m_LangID(langMgr.CurrentLanguage())
{
	//{{AFX_DATA_INIT(CDlgLanguage)
	//}}AFX_DATA_INIT
}


void CDlgLanguage::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLanguage)
	DDX_Control(pDX, IDC_LANGUAGE, m_ctrlLanguages);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLanguage, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgLanguage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLanguage message handlers

#define MAX_LANGUAGE 255

BOOL CDlgLanguage::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	TCHAR langName[MAX_LANGUAGE];
	GetLocaleInfo(m_LangMgr.ExeLanguage(), LOCALE_SLANGUAGE, langName, MAX_LANGUAGE);
	int idx = m_ctrlLanguages.AddString(langName);
	m_ctrlLanguages.SetItemData(idx, m_LangMgr.ExeLanguage());
	if (m_LangMgr.ExeLanguage() == m_LangMgr.CurrentLanguage())
		m_ctrlLanguages.SetCurSel(idx);
	for (CLanguageManager::LangResources::const_iterator iLang = m_LangMgr.Languages().begin();
		iLang != m_LangMgr.Languages().end();
		++iLang)
	{
		GetLocaleInfo(iLang->first, LOCALE_SLANGUAGE, langName, MAX_LANGUAGE);
		idx = m_ctrlLanguages.AddString(langName);
		m_ctrlLanguages.SetItemData(idx, iLang->first);
		if (iLang->first == m_LangMgr.CurrentLanguage())
			m_ctrlLanguages.SetCurSel(idx);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgLanguage::OnOK()
{
	if (!UpdateData(TRUE))
		return;

	int index = m_ctrlLanguages.GetCurSel();
	if (CB_ERR == index)
	{
		GotoDlgCtrl(&m_ctrlLanguages);
		return;
	}
	m_LangID = static_cast<LANGID>(m_ctrlLanguages.GetItemData(index));

	CDlgBaseDialog::OnOK();
}
