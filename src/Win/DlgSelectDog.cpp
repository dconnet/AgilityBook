/*
 * Copyright © 2002-2004 Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgSelectDog class
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgSelectDog.h"

#include "ARBDog.h"
#include "AgilityBookDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectDog dialog

CDlgSelectDog::CDlgSelectDog(CAgilityBookDoc* pDoc, std::vector<ARBDog*>& dogs, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectDog::IDD, pParent)
	, m_pDoc(pDoc)
	, m_Dogs(dogs)
{
	//{{AFX_DATA_INIT(CDlgSelectDog)
	//}}AFX_DATA_INIT
}

DOMODAL_RETVAL CDlgSelectDog::DoModal() 
{
	m_Dogs.clear();
	if (1 == m_pDoc->GetDogs().size())
	{
		m_Dogs.push_back(*(m_pDoc->GetDogs().begin()));
		return IDOK;
	}
	else
		return CDialog::DoModal();
}

void CDlgSelectDog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectDog)
	DDX_Control(pDX, IDC_PICK_NAME, m_ctrlList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgSelectDog, CDialog)
	//{{AFX_MSG_MAP(CDlgSelectDog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectDog message handlers

BOOL CDlgSelectDog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	ARBDogList const& dogs = m_pDoc->GetDogs();
	for (ARBDogList::const_iterator iter = dogs.begin(); iter != dogs.end(); ++iter)
	{
		ARBDog* pDog = *iter;
		int index = m_ctrlList.AddString(pDog->GetCallName().c_str());
		m_ctrlList.SetItemDataPtr(index, pDog);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectDog::OnOK() 
{
	m_Dogs.clear();
	for (int index = 0; index < m_ctrlList.GetCount(); ++index)
	{
		if (m_ctrlList.GetCheck(index))
		{
			ARBDog* pDog = reinterpret_cast<ARBDog*>(m_ctrlList.GetItemDataPtr(index));
			m_Dogs.push_back(pDog);
		}
	}
	CDialog::OnOK();
}
