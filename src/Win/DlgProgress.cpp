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
 * @brief Progress dialog
 * @author David Connet
 *
 * Revision History
 * @li 2004-10-01 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CDlgProgress : public CDialog, public IDlgProgress
{
public:
	CDlgProgress(CWnd* pParent = NULL);
	~CDlgProgress();

	// Setup dialog.
	/// Set the caption of the dialog.
	virtual void SetCaption(LPCTSTR inCaption);
	/// Set a visible message.
	virtual void SetMessage(LPCTSTR inMessage);
	/// Set the number of progress bars (0,1,2)
	virtual bool SetNumProgressBars(short nBars);

	// Progress bar interface (these are thin wrappers on the progress bar)
	virtual bool SetRange(short inBar, int inLower, int inUpper);
	virtual bool SetStep(short inBar, int inStep);
	virtual bool StepIt(short inBar);
	virtual bool OffsetPos(short inBar, int inDelta);
	virtual bool SetPos(short inBar, int inPos);
	virtual bool GetPos(short inBar, int& outPos);

	/// Show/hide the dialog.
	virtual void Show(bool bShow = true);
	/// Shut down (delete) the dialog.
	virtual void Dismiss();

private:
// Dialog Data
	//{{AFX_DATA(CDlgProgress)
	enum { IDD = IDD_PROGRESS_DLG };
	CStatic	m_ctrlMessage;
	CProgressCtrl	m_ctrlBar1;
	CProgressCtrl	m_ctrlBar2;
	//}}AFX_DATA
	int m_nBars;

// Overrides
	//{{AFX_VIRTUAL(CDlgProgress)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	CProgressCtrl* GetBar(short inBar)
	{
		CProgressCtrl* pBar = NULL;
		if (1 == inBar)
			pBar = &m_ctrlBar1;
		else if (2 == inBar)
			pBar = &m_ctrlBar2;
		return pBar;
	}

	//{{AFX_MSG(CDlgProgress)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CDlgProgress::CDlgProgress(CWnd* pParent)
	: CDialog(CDlgProgress::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProgress)
	//}}AFX_DATA_INIT
	CDialog::Create(CDlgProgress::IDD, pParent);
}

CDlgProgress::~CDlgProgress()
{
	if (m_hWnd && ::IsWindow(m_hWnd))
		DestroyWindow();
}

void CDlgProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProgress)
	DDX_Control(pDX, IDC_PROGRESS_MESSAGE, m_ctrlMessage);
	DDX_Control(pDX, IDC_PROGRESS_BAR1, m_ctrlBar1);
	DDX_Control(pDX, IDC_PROGRESS_BAR2, m_ctrlBar2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgProgress, CDialog)
	//{{AFX_MSG_MAP(CDlgProgress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDlgProgress::SetCaption(LPCTSTR inCaption)
{
	SetWindowText(inCaption);
}

void CDlgProgress::SetMessage(LPCTSTR inMessage)
{
	m_ctrlMessage.SetWindowText(inMessage);
	m_ctrlMessage.UpdateWindow();
	UpdateWindow();
}

bool CDlgProgress::SetNumProgressBars(short nBars)
{
	if (0 > nBars || 2 < nBars)
	{
		ASSERT(0);
		return false;
	}
	UINT nShow1 = SW_HIDE;
	UINT nShow2 = SW_HIDE;
	if (1 == nBars)
		nShow1 = SW_SHOW;
	else if (2 == nBars)
		nShow1 = nShow2 = SW_SHOW;
	m_ctrlBar1.ShowWindow(nShow1);
	m_ctrlBar2.ShowWindow(nShow2);
	m_nBars = nBars;
	return true;
}

bool CDlgProgress::SetRange(short inBar, int inLower, int inUpper)
{
	bool bOk = false;
	CProgressCtrl* pBar = GetBar(inBar);
	if (pBar)
	{
		pBar->SetRange32(inLower, inUpper);
		bOk = true;
	}
	return bOk;
}

bool CDlgProgress::SetStep(short inBar, int inStep)
{
	bool bOk = false;
	CProgressCtrl* pBar = GetBar(inBar);
	if (pBar)
	{
		pBar->SetStep(inStep);
		bOk = true;
	}
	return bOk;
}

bool CDlgProgress::StepIt(short inBar)
{
	bool bOk = false;
	CProgressCtrl* pBar = GetBar(inBar);
	if (pBar)
	{
		pBar->StepIt();
		bOk = true;
	}
	return bOk;
}

bool CDlgProgress::OffsetPos(short inBar, int inDelta)
{
	bool bOk = false;
	CProgressCtrl* pBar = GetBar(inBar);
	if (pBar)
	{
		pBar->OffsetPos(inDelta);
		bOk = true;
	}
	return bOk;
}

bool CDlgProgress::SetPos(short inBar, int inPos)
{
	bool bOk = false;
	CProgressCtrl* pBar = GetBar(inBar);
	if (pBar)
	{
		pBar->SetPos(inPos);
		bOk = true;
	}
	return bOk;
}

bool CDlgProgress::GetPos(short inBar, int& outPos)
{
	bool bOk = false;
	CProgressCtrl* pBar = GetBar(inBar);
	if (pBar)
	{
		outPos = pBar->GetPos();
		bOk = true;
	}
	return bOk;
}

void CDlgProgress::Show(bool bShow)
{
	ShowWindow(bShow ? SW_SHOW : SW_HIDE);
}

void CDlgProgress::Dismiss()
{
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgProgress message handlers

BOOL CDlgProgress::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctrlBar1.SetStep(1);
	m_ctrlBar1.SetRange(0, 10);
	m_ctrlBar2.SetStep(1);
	m_ctrlBar2.SetRange(0, 10);
	m_ctrlBar2.ShowWindow(SW_HIDE);
	m_nBars = 1;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////

IDlgProgress* IDlgProgress::CreateProgress(CWnd* pParent)
{
	return new CDlgProgress(pParent);
}

IDlgProgress::~IDlgProgress()
{
}
