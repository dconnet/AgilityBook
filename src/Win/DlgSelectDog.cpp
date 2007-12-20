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
 * @brief implementation of the CDlgSelectDog class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-02-18 DRC Remember selected dogs.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgSelectDog.h"

#include <algorithm>
#include <set>
#include "ARBDog.h"
#include "ARBTypes.h"
#include "AgilityBookDoc.h"
#include "ListData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectDog dialog

CDlgSelectDog::CDlgSelectDog(
		CAgilityBookDoc* pDoc,
		std::vector<ARBDogPtr>& dogs,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgSelectDog::IDD, pParent)
	, m_ctrlList(true)
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
		return CDlgBaseDialog::DoModal();
}


void CDlgSelectDog::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectDog)
	DDX_Control(pDX, IDC_PICK_NAME, m_ctrlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelectDog, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgSelectDog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectDog message handlers

BOOL CDlgSelectDog::OnInitDialog() 
{
	CDlgBaseDialog::OnInitDialog();

	std::set<tstring> selection;
	int nDogs = theApp.GetProfileInt(_T("Selection"), _T("nDogs"), 0);
	for (int iDog = 1; iDog <= nDogs; ++iDog)
	{
		otstringstream item;
		item << _T("Dog") << iDog;
		CString dog = theApp.GetProfileString(_T("Selection"), item.str().c_str(), _T(""));
		if (!dog.IsEmpty())
			selection.insert((LPCTSTR)dog);
	}

	ARBDogList const& dogs = m_pDoc->GetDogs();
	for (ARBDogList::const_iterator iter = dogs.begin(); iter != dogs.end(); ++iter)
	{
		ARBDogPtr pDog = *iter;
		int index = m_ctrlList.AddString(pDog->GetCallName().c_str());
		m_ctrlList.SetItemDataPtr(index,
			new CListPtrData<ARBDogPtr>(pDog));
		if (selection.end() != std::find(selection.begin(), selection.end(), pDog->GetCallName()))
			m_ctrlList.SetCheck(index, 1);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgSelectDog::OnOK() 
{
	// Erase existing.
	int nDogs = theApp.GetProfileInt(_T("Selection"), _T("nDogs"), 0);
	for (int iDog = 1; iDog <= nDogs; ++iDog)
	{
		otstringstream item;
		item << _T("Dog") << iDog;
		theApp.WriteProfileString(_T("Selection"), item.str().c_str(), NULL);
	}
	// Now commit the selection.
	nDogs = 0;
	m_Dogs.clear();
	for (int index = 0; index < m_ctrlList.GetCount(); ++index)
	{
		if (m_ctrlList.GetCheck(index))
		{
			ARBDogPtr pDog = reinterpret_cast<CListPtrData<ARBDogPtr>*>(m_ctrlList.GetItemDataPtr(index))->GetData();
			m_Dogs.push_back(pDog);
			++nDogs;
			otstringstream item;
			item << _T("Dog") << nDogs;
			theApp.WriteProfileString(_T("Selection"), item.str().c_str(), pDog->GetCallName().c_str());
		}
	}
	theApp.WriteProfileInt(_T("Selection"), _T("nDogs"), nDogs);
	CDlgBaseDialog::OnOK();
}
