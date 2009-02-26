/*
 * Copyright © 2004-2009 David Connet. All Rights Reserved.
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
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2008-09-05 DRC Put progress dialog into separate UI thread
 * @li 2004-10-01 DRC Created
 */

#include "stdafx.h"
#include "DlgProgress.h"

#pragma message PRAGMA_MESSAGE("TODO: Implement CDlgProgress")
#if 0
#include "AgilityBook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

struct ProgressSetRangeData
{
	short bar;
	int lower;
	int upper;
	ProgressSetRangeData(short b, int d1, int d2) : bar(b), lower(d1), upper(d2) {}
};

#define WM_PROGRESS_SETCAPTION		(WM_APP+1)
		// wparam: unused
		// lparam: LPCTSTR
		// returns: 0
#define WM_PROGRESS_SETMESSAGE		(WM_APP+2)
		// wparam: unused
		// lparam: LPCTSTR
		// returns: 0
#define WM_PROGRESS_SETNUMBARS		(WM_APP+3)
		// wparam: short
		// lparam: unused
		// returns: BOOL
#define WM_PROGRESS_SETRANGE		(WM_APP+4)
		// wparam: unused
		// lparam: ProgressSetRangeData*
		// returns: BOOL
#define WM_PROGRESS_SETSTEP			(WM_APP+5)
		// wparam: short
		// lparam: int
		// returns: BOOL
#define WM_PROGRESS_STEP			(WM_APP+6)
		// wparam: short
		// lparam: unused
		// returns: BOOL
#define WM_PROGRESS_OFFSETPOS		(WM_APP+7)
		// wparam: short
		// lparam: int
		// returns: BOOL
#define WM_PROGRESS_SETPOS			(WM_APP+8)
		// wparam: short
		// lparam: int
		// returns: BOOL
#define WM_PROGRESS_GETPOS			(WM_APP+9)
		// wparam: short
		// lparam: int*
		// returns: BOOL
#define WM_PROGRESS_ENABLECANCEL	(WM_APP+10)
		// wparam: BOOL
		// lparam: unused
		// returns: 0
#define WM_PROGRESS_HASCANCELED		(WM_APP+11)
		// wparam: unused
		// lparam: unused
		// returns: BOOL
#define WM_PROGRESS_SHOW			(WM_APP+12)
		// wparam: BOOL
		// lparam: unused
		// returns: 0
#define WM_PROGRESS_DISMISS			(WM_APP+13)
		// wparam: unused
		// lparam: unused
		// returns: 0


class CDlgProgress : public CDialog
{
public:
	CDlgProgress(HANDLE hEvent);
	~CDlgProgress();

	enum { IDD = IDD_PROGRESS_DLG };
private:
	//{{AFX_DATA(CDlgProgress)
	CStatic	m_ctrlMessage;
	CProgressCtrl	m_ctrlBar1;
	CProgressCtrl	m_ctrlBar2;
	CButton	m_ctrlCancel;
	//}}AFX_DATA
	HANDLE m_Event;
	int m_nBars;
	BOOL m_HasCanceled;

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
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg LRESULT OnProgressSetCaption(WPARAM, LPARAM);
	afx_msg LRESULT OnProgressSetMessage(WPARAM, LPARAM);
	afx_msg LRESULT OnProgressSetNumBars(WPARAM, LPARAM);
	afx_msg LRESULT OnProgressSetRange(WPARAM, LPARAM);
	afx_msg LRESULT OnProgressSetStep(WPARAM, LPARAM);
	afx_msg LRESULT OnProgressStep(WPARAM, LPARAM);
	afx_msg LRESULT OnProgressOffsetPos(WPARAM, LPARAM);
	afx_msg LRESULT OnProgressSetPos(WPARAM, LPARAM);
	afx_msg LRESULT OnProgressGetPos(WPARAM, LPARAM);
	afx_msg LRESULT OnProgressEnableCancel(WPARAM, LPARAM);
	afx_msg LRESULT OnProgressHasCanceled(WPARAM, LPARAM);
	afx_msg LRESULT OnProgressShow(WPARAM, LPARAM);
	afx_msg LRESULT OnProgressDismiss(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};


CDlgProgress::CDlgProgress(HANDLE hEvent)
	: CDialog()
	, m_Event(hEvent)
	, m_nBars(1)
	, m_HasCanceled(FALSE)
{
	ASSERT(m_Event);
	// Don't Create/ShowWindow here. It must be done in the thread's
	// InitInstance after we set the mainwnd pointer.
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
	DDX_Control(pDX, IDCANCEL, m_ctrlCancel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProgress, CDialog)
	//{{AFX_MSG_MAP(CDlgProgress)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_PROGRESS_SETCAPTION, OnProgressSetCaption)
	ON_MESSAGE(WM_PROGRESS_SETMESSAGE, OnProgressSetMessage)
	ON_MESSAGE(WM_PROGRESS_SETNUMBARS, OnProgressSetNumBars)
	ON_MESSAGE(WM_PROGRESS_SETRANGE, OnProgressSetRange)
	ON_MESSAGE(WM_PROGRESS_SETSTEP, OnProgressSetStep)
	ON_MESSAGE(WM_PROGRESS_STEP, OnProgressStep)
	ON_MESSAGE(WM_PROGRESS_OFFSETPOS, OnProgressOffsetPos)
	ON_MESSAGE(WM_PROGRESS_SETPOS, OnProgressSetPos)
	ON_MESSAGE(WM_PROGRESS_GETPOS, OnProgressGetPos)
	ON_MESSAGE(WM_PROGRESS_ENABLECANCEL, OnProgressEnableCancel)
	ON_MESSAGE(WM_PROGRESS_HASCANCELED, OnProgressHasCanceled)
	ON_MESSAGE(WM_PROGRESS_SHOW, OnProgressShow)
	ON_MESSAGE(WM_PROGRESS_DISMISS, OnProgressDismiss)
END_MESSAGE_MAP()

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

	::SetEvent(m_Event); // We're initialized, let the main thread go.
	m_Event = NULL;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgProgress::OnCancel()
{
	m_HasCanceled = TRUE;
	m_ctrlCancel.EnableWindow(FALSE);
}


LRESULT CDlgProgress::OnProgressSetCaption(WPARAM, LPARAM lParam)
{
	SetWindowText(reinterpret_cast<LPCTSTR>(lParam));
	return 0;
}


LRESULT CDlgProgress::OnProgressSetMessage(WPARAM, LPARAM lParam)
{
	m_ctrlMessage.SetWindowText(reinterpret_cast<LPCTSTR>(lParam));
	return 0;
}


LRESULT CDlgProgress::OnProgressSetNumBars(WPARAM wParam, LPARAM)
{
	short nBars = static_cast<short>(wParam);
	if (0 >= nBars || 2 < nBars)
	{
		ASSERT(0);
		return FALSE;
	}
	UINT nShow1 = SW_HIDE;
	UINT nShow2 = SW_HIDE;
	if (1 == nBars)
	{
		nShow1 = SW_SHOW;
	}
	else if (2 == nBars)
	{
		nShow1 = SW_SHOW;
		nShow2 = SW_SHOW;
	}
	m_nBars = nBars;
	m_ctrlBar1.ShowWindow(nShow1);
	m_ctrlBar2.ShowWindow(nShow2);
	return TRUE;
}


LRESULT CDlgProgress::OnProgressSetRange(WPARAM, LPARAM lParam)
{
	ProgressSetRangeData* pData = reinterpret_cast<ProgressSetRangeData*>(lParam);
	BOOL bOk = FALSE;
	CProgressCtrl* pBar = GetBar(pData->bar);
	if (pBar)
	{
		bOk = TRUE;
		pBar->SetRange32(pData->lower, pData->upper);
	}
	return bOk;
}


LRESULT CDlgProgress::OnProgressSetStep(WPARAM wParam, LPARAM lParam)
{
	BOOL bOk = FALSE;
	short nBar = static_cast<short>(wParam);
	CProgressCtrl* pBar = GetBar(nBar);
	if (pBar)
	{
		bOk = TRUE;
		pBar->SetStep(static_cast<int>(lParam));
	}
	return bOk;
}


LRESULT CDlgProgress::OnProgressStep(WPARAM wParam, LPARAM)
{
	BOOL bOk = FALSE;
	short nBar = static_cast<short>(wParam);
	CProgressCtrl* pBar = GetBar(nBar);
	if (pBar)
	{
		bOk = TRUE;
		pBar->StepIt();
		pBar->UpdateWindow();
	}
	return bOk;
}


LRESULT CDlgProgress::OnProgressOffsetPos(WPARAM wParam, LPARAM lParam)
{
	BOOL bOk = FALSE;
	short nBar = static_cast<short>(wParam);
	CProgressCtrl* pBar = GetBar(nBar);
	if (pBar)
	{
		bOk = TRUE;
		pBar->OffsetPos(static_cast<int>(lParam));
	}
	return bOk;
}


LRESULT CDlgProgress::OnProgressSetPos(WPARAM wParam, LPARAM lParam)
{
	BOOL bOk = FALSE;
	short nBar = static_cast<short>(wParam);
	CProgressCtrl* pBar = GetBar(nBar);
	if (pBar)
	{
		bOk = TRUE;
		pBar->SetPos(static_cast<int>(lParam));
		pBar->UpdateWindow();
	}
	return bOk;
}


LRESULT CDlgProgress::OnProgressGetPos(WPARAM wParam, LPARAM lParam)
{
	BOOL bOk = FALSE;
	short nBar = static_cast<short>(wParam);
	CProgressCtrl* pBar = GetBar(nBar);
	if (pBar)
	{
		bOk = TRUE;
		*reinterpret_cast<int*>(lParam) = pBar->GetPos();
	}
	return bOk;
}


LRESULT CDlgProgress::OnProgressEnableCancel(WPARAM bEnable, LPARAM)
{
	if (!m_HasCanceled)
		m_ctrlCancel.EnableWindow(bEnable ? TRUE : FALSE);
	return 0;
}


LRESULT CDlgProgress::OnProgressHasCanceled(WPARAM, LPARAM)
{
	return m_HasCanceled;
}


LRESULT CDlgProgress::OnProgressShow(WPARAM wParam, LPARAM)
{
	ShowWindow(wParam ? SW_SHOW : SW_HIDE);
	return 0;
}


LRESULT CDlgProgress::OnProgressDismiss(WPARAM, LPARAM)
{
	if (m_Event != NULL)
	{
		// Delay it. We're not done initializing.
		PostMessage(WM_PROGRESS_DISMISS);
	}
	else
		DestroyWindow();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

class CProgressThread : public CWinThread, public IDlgProgress
{
	DECLARE_DYNCREATE(CProgressThread)
public:
	CProgressThread();
	~CProgressThread();
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();
	HWND m_hwndParent;
	HANDLE m_Wait;
	CDlgProgress* m_Dialog;

// IDlgProgress
	// Setup dialog.
	/// Set the caption of the dialog.
	virtual void SetCaption(LPCTSTR inCaption);
	/// Set a visible message.
	virtual void SetMessage(LPCTSTR inMessage);
	/// Set the number of progress bars (0,1,2)
	virtual bool SetNumProgressBars(short nBars);

	// Progress bar interface (these are thin wrappers on the progress bar)
	virtual bool SetRange(
			short inBar,
			int inLower,
			int inUpper);
	virtual bool SetStep(
			short inBar,
			int inStep);
	virtual bool StepIt(short inBar);
	virtual bool OffsetPos(
			short inBar,
			int inDelta);
	virtual bool SetPos(
			short inBar,
			int inPos);
	virtual bool GetPos(
			short inBar,
			int& outPos);
	virtual void EnableCancel(bool bEnable = true);
	virtual bool HasCanceled() const;

	/// Show/hide the dialog.
	virtual void Show(bool bShow = true);
	/// Steal back focus
	virtual void SetForegroundWindow();
	/// Shut down (delete) the dialog.
	virtual void Dismiss();
};


IMPLEMENT_DYNCREATE(CProgressThread, CWinThread)


CProgressThread::CProgressThread()
	: m_Wait(NULL)
	, m_hwndParent(NULL)
	, m_Dialog(NULL)
{
}


CProgressThread::~CProgressThread()
{
	if (m_hwndParent)
	{
		// If we're in here, the progress dialog died off on it's own (without
		// a 'Dismiss'). That means we never re-enabled the main window, hence
		// windows will have flipped focus to a different application. After
		// re-enabling, steal the foreground window back too.
		::EnableWindow(m_hwndParent, TRUE);
		::SetForegroundWindow(m_hwndParent);
	}
}


// Thread2.
BOOL CProgressThread::InitInstance()
{
	// Do not disable the parent window here. We can deadlock otherwise.
	// Any actions on the parent cause a need for message processing,
	// and since we are not processing the messages in thread1,
	// this thread would get stuck.
	m_Dialog = new CDlgProgress(m_Wait);
	m_pMainWnd = m_Dialog;
	m_Dialog->Create(CDlgProgress::IDD, NULL);
	m_pMainWnd->ShowWindow(SW_SHOW);
	return TRUE;
}


BOOL CProgressThread::ExitInstance()
{
	delete m_Dialog;
	m_Dialog = NULL;
	return CWinThread::ExitInstance();
}

// All these are executed in thread1

void CProgressThread::SetCaption(LPCTSTR pCaption)
{
	m_pMainWnd->SendMessage(WM_PROGRESS_SETCAPTION, 0, reinterpret_cast<LPARAM>(pCaption));
}


void CProgressThread::SetMessage(LPCTSTR pMsg)
{
	m_pMainWnd->SendMessage(WM_PROGRESS_SETMESSAGE, 0, reinterpret_cast<LPARAM>(pMsg));
}


bool CProgressThread::SetNumProgressBars(short nBars)
{
	return m_pMainWnd->SendMessage(WM_PROGRESS_SETNUMBARS, nBars) ? true : false;
}


bool CProgressThread::SetRange(short inBar, int inLower, int inUpper)
{
	ProgressSetRangeData data(inBar, inLower, inUpper);
	return m_pMainWnd->SendMessage(WM_PROGRESS_SETRANGE,
			0, reinterpret_cast<LPARAM>(&data)) ? true : false;
}


bool CProgressThread::SetStep(short inBar, int inStep)
{
	return m_pMainWnd->SendMessage(WM_PROGRESS_SETSTEP, inBar, inStep) ? true : false;
}


bool CProgressThread::StepIt(short inBar)
{
	return m_pMainWnd->SendMessage(WM_PROGRESS_STEP, inBar, 0) ? true : false;
}


bool CProgressThread::OffsetPos(short inBar, int inDelta)
{
	return m_pMainWnd->SendMessage(WM_PROGRESS_OFFSETPOS, inBar, inDelta) ? true : false;
}


bool CProgressThread::SetPos(short inBar, int inPos)
{
	return m_pMainWnd->SendMessage(WM_PROGRESS_SETPOS, inBar, inPos) ? true : false;
}


bool CProgressThread::GetPos(short inBar, int& outPos)
{
	return m_pMainWnd->SendMessage(WM_PROGRESS_GETPOS,
			inBar, reinterpret_cast<LPARAM>(&outPos)) ? true : false;
}


void CProgressThread::EnableCancel(bool bEnable)
{
	m_pMainWnd->SendMessage(WM_PROGRESS_ENABLECANCEL, bEnable ? TRUE : FALSE);
}


bool CProgressThread::HasCanceled() const
{
	return m_pMainWnd->SendMessage(WM_PROGRESS_HASCANCELED) ? true : false;
}


void CProgressThread::Show(bool bShow)
{
	if (m_hwndParent)
	{
		if (bShow)
			::EnableWindow(m_hwndParent, FALSE);
		else
		{
			::EnableWindow(m_hwndParent, TRUE);
			::SetForegroundWindow(m_hwndParent);
		}
	}
	m_pMainWnd->SendMessage(WM_PROGRESS_SHOW, bShow ? TRUE : FALSE);
}


void CProgressThread::SetForegroundWindow()
{
	m_pMainWnd->SetForegroundWindow();
}


void CProgressThread::Dismiss()
{
	if (m_hwndParent)
	{
		// Before we kill off the progress dialog, re-enable our main window
		// so focus shifts back to it. Otherwise windows shifts to the next
		// application in the z-order (which is not us - and hence the call
		// in the dtor above).
		::EnableWindow(m_hwndParent, TRUE);
		m_hwndParent = NULL;
	}
	m_pMainWnd->SendMessage(WM_PROGRESS_DISMISS);
}

/////////////////////////////////////////////////////////////////////////////

IDlgProgress* IDlgProgress::CreateProgress(HWND hwndParent)
{
	if (!hwndParent)
		hwndParent = AfxGetMainWnd()->GetSafeHwnd();
	if (hwndParent)
		::EnableWindow(hwndParent, FALSE);

	CProgressThread* pThread = (CProgressThread*)AfxBeginThread(
		RUNTIME_CLASS(CProgressThread),
		THREAD_PRIORITY_ABOVE_NORMAL,
		0,
		CREATE_SUSPENDED);

	// Set up the object while it's suspended.
	pThread->m_hwndParent = hwndParent;
	// The event is required as we cannot allow any interface calls until
	// the dialog is up and ready. This is used so the dialog can tell us
	// when it's ready
	pThread->m_Wait = CreateEvent(NULL, TRUE, FALSE, NULL); // Manual, unsignaled event
	ASSERT(pThread->m_Wait);

	// Now that it is initialized properly, go...
	pThread->ResumeThread();
	// Wait for the dialog to tell us it's okay to go...
	WaitForSingleObject(pThread->m_Wait, INFINITE);
	// Cleanup, and start the work.
	CloseHandle(pThread->m_Wait);
	pThread->m_Wait = NULL;
	// And return control.
	if (hwndParent)
	{
		// This seems to be the only way to get the parent frame to repaint.
		AfxGetApp()->PumpMessage();
	}
	return pThread;
}


IDlgProgress::~IDlgProgress()
{
}
#endif
