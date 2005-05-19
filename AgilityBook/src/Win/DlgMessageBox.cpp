/*
 * Copyright © 2004-2005 David Connet. All Rights Reserved.
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
 * @brief Message box with a details button.
 * @author David Connet
 *
 * Revision History
 * @li 2004-09-11 DRC Created
 */

#include "stdafx.h"
#include "resource.h"
#include "DlgMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

IMessageBoxCallback::~IMessageBoxCallback()
{
}

/////////////////////////////////////////////////////////////////////////////

class CDlgMessageBox : public CDialog
{
public:
	CDlgMessageBox(
		CString const& inText,
		UINT inType,
		IMessageBoxCallback* inCallback,
		CWnd* inParent);

private:
// Dialog Data
	//{{AFX_DATA(CDlgMessageBox)
	enum { IDD = IDD_MESSAGE_BOX };
	CStatic	m_ctrlIcon;
	CStatic	m_ctrlText;
	//}}AFX_DATA
	enum { NUM_BUTTONS = 4 };
	CButton	m_ctrlButton[NUM_BUTTONS];
	CString const& m_Text;
	UINT m_Type;
	IMessageBoxCallback* m_Callback;

	//{{AFX_VIRTUAL(CDlgMessageBox)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgMessageBox)
	virtual BOOL OnInitDialog();
	afx_msg void OnDetails();
	//}}AFX_MSG
	afx_msg BOOL OnCmd(UINT id);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

// Since we display a variable number of buttons (max of 4),
// this aids in modifying a button's behavior to match what we want.
static void SetButtonInfo(CString const& strDetails, CButton& ctrlButton, UINT id)
{
	CString text;
	switch (id)
	{
	default:
		id = IDOK;
		// fallthru
	case IDOK:
		text.LoadString(IDS_BUTTON_OK);
		break;
	case IDCANCEL:
		text.LoadString(IDS_BUTTON_CANCEL);
		break;
	case IDYES:
		text.LoadString(IDS_BUTTON_YES);
		break;
	case IDNO:
		text.LoadString(IDS_BUTTON_NO);
		break;
	case IDABORT:
		text.LoadString(IDS_BUTTON_ABORT);
		break;
	case IDIGNORE:
		text.LoadString(IDS_BUTTON_IGNORE);
		break;
	case IDRETRY:
		text.LoadString(IDS_BUTTON_RETRY);
		break;
	case IDC_MESSAGE_BOX_DETAILS:
		text = strDetails;
		break;
	}
	ctrlButton.SetWindowText(text);
	ctrlButton.SetDlgCtrlID(id);
}

/////////////////////////////////////////////////////////////////////////////
// Protected dialog.

CDlgMessageBox::CDlgMessageBox(
	CString const& inText,
	UINT inType,
	IMessageBoxCallback* inCallback,
	CWnd* inParent)
	: CDialog(CDlgMessageBox::IDD, inParent)
	, m_Text(inText)
	, m_Type(inType)
	, m_Callback(inCallback)
{
	//{{AFX_DATA_INIT(CDlgMessageBox)
	//}}AFX_DATA_INIT
}

void CDlgMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMessageBox)
	DDX_Control(pDX, IDC_MESSAGE_BOX_ICON, m_ctrlIcon);
	DDX_Control(pDX, IDC_MESSAGE_BOX_TEXT, m_ctrlText);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_MESSAGE_BOX_BUTTON1, m_ctrlButton[0]);
	DDX_Control(pDX, IDC_MESSAGE_BOX_BUTTON2, m_ctrlButton[1]);
	DDX_Control(pDX, IDC_MESSAGE_BOX_BUTTON3, m_ctrlButton[2]);
	DDX_Control(pDX, IDC_MESSAGE_BOX_DETAILS, m_ctrlButton[3]);
}

BEGIN_MESSAGE_MAP(CDlgMessageBox, CDialog)
	//{{AFX_MSG_MAP(CDlgMessageBox)
	ON_COMMAND(IDC_MESSAGE_BOX_DETAILS, OnDetails)
	//}}AFX_MSG_MAP
	ON_COMMAND_EX(IDABORT, OnCmd)
	ON_COMMAND_EX(IDCANCEL, OnCmd)
	ON_COMMAND_EX(IDIGNORE, OnCmd)
	ON_COMMAND_EX(IDNO, OnCmd)
	ON_COMMAND_EX(IDOK, OnCmd)
	ON_COMMAND_EX(IDRETRY, OnCmd)
	ON_COMMAND_EX(IDYES, OnCmd)
END_MESSAGE_MAP()

BOOL CDlgMessageBox::OnInitDialog() 
{
	SetRedraw(FALSE);

	CDialog::OnInitDialog();
	// Get the extra button info.
	CString strDetails;
	m_ctrlButton[3].GetWindowText(strDetails);

	// Set the window caption.
	char szExeName[_MAX_PATH] = "";
	::GetModuleFileName(NULL, szExeName, _MAX_PATH);
	char* pStr = strrchr(szExeName, '.');
	ASSERT(pStr != NULL);
	ASSERT(*pStr == '.');
	*pStr = 0;       // no suffix
	pStr = strrchr(szExeName, '\\');
	ASSERT(pStr);
	SetWindowText(pStr+1);

	// Set the text.
	m_ctrlText.SetWindowText(m_Text);

	// Set the icon.
	switch (m_Type & MB_ICONMASK)
	{
	default:
		m_ctrlIcon.ShowWindow(SW_HIDE);
		// Since the icon doesn't exist, expand the text into the icon area.
		// This allows the resizing we do below to work properly.
		{
			CRect rIcon, rText;
			m_ctrlIcon.GetWindowRect(&rIcon);
			ScreenToClient(&rIcon);
			m_ctrlText.GetWindowRect(&rText);
			ScreenToClient(&rText);
			rText.left = rIcon.left;
			m_ctrlText.SetWindowPos(NULL, rText.left, rText.top, rText.Width(), rText.Height(), SWP_NOZORDER);
		}
		break;
	case MB_ICONHAND: // same as MB_ICONSTOP
		m_ctrlIcon.SetIcon(::LoadIcon(NULL, IDI_HAND));
		break;
	case MB_ICONQUESTION:
		m_ctrlIcon.SetIcon(::LoadIcon(NULL, IDI_QUESTION));
		break;
	case MB_ICONEXCLAMATION:
		m_ctrlIcon.SetIcon(::LoadIcon(NULL, IDI_EXCLAMATION));
		break;
	case MB_ICONASTERISK:
		m_ctrlIcon.SetIcon(::LoadIcon(NULL, IDI_ASTERISK));
		break;
	}

	// Set the visible buttons. We'll move things later.
	int nButtons = 0;
	switch (m_Type & MB_TYPEMASK)
	{
	default:
	case MB_OK:
		nButtons = 1;
		SetButtonInfo(strDetails, m_ctrlButton[0], IDOK);
		m_ctrlButton[1].ShowWindow(SW_HIDE);
		m_ctrlButton[2].ShowWindow(SW_HIDE);
		m_ctrlButton[3].ShowWindow(SW_HIDE);
		break;
	case MB_OKCANCEL:
		nButtons = 2;
		SetButtonInfo(strDetails, m_ctrlButton[0], IDOK);
		SetButtonInfo(strDetails, m_ctrlButton[1], IDCANCEL);
		m_ctrlButton[2].ShowWindow(SW_HIDE);
		m_ctrlButton[3].ShowWindow(SW_HIDE);
		break;
	case MB_ABORTRETRYIGNORE:
		nButtons = 3;
		SetButtonInfo(strDetails, m_ctrlButton[0], IDABORT);
		SetButtonInfo(strDetails, m_ctrlButton[1], IDRETRY);
		SetButtonInfo(strDetails, m_ctrlButton[2], IDIGNORE);
		m_ctrlButton[3].ShowWindow(SW_HIDE);
		break;
	case MB_YESNOCANCEL:
		nButtons = 3;
		SetButtonInfo(strDetails, m_ctrlButton[0], IDYES);
		SetButtonInfo(strDetails, m_ctrlButton[1], IDNO);
		SetButtonInfo(strDetails, m_ctrlButton[2], IDCANCEL);
		m_ctrlButton[3].ShowWindow(SW_HIDE);
		break;
	case MB_YESNO:
		nButtons = 2;
		SetButtonInfo(strDetails, m_ctrlButton[0], IDYES);
		SetButtonInfo(strDetails, m_ctrlButton[1], IDNO);
		m_ctrlButton[2].ShowWindow(SW_HIDE);
		m_ctrlButton[3].ShowWindow(SW_HIDE);
		break;
	case MB_RETRYCANCEL:
		nButtons = 2;
		SetButtonInfo(strDetails, m_ctrlButton[0], IDRETRY);
		SetButtonInfo(strDetails, m_ctrlButton[1], IDCANCEL);
		m_ctrlButton[2].ShowWindow(SW_HIDE);
		m_ctrlButton[3].ShowWindow(SW_HIDE);
		break;
	}
	// If a callback is provided, the Details button is shown.
	if (m_Callback)
	{
		m_ctrlButton[nButtons].ShowWindow(SW_SHOW);
		SetButtonInfo(strDetails, m_ctrlButton[nButtons], IDC_MESSAGE_BOX_DETAILS);
		++nButtons;
	}

	// Set up the default button.
	int nDefButton = 0;
	switch (m_Type & MB_DEFMASK)
	{
	default: nDefButton = 0; break;
	case MB_DEFBUTTON2: nDefButton = 1; break;
	case MB_DEFBUTTON3: nDefButton = 2; break;
	}
	SetDefID(m_ctrlButton[nDefButton].GetDlgCtrlID());
	if (0 != nDefButton)
	{
		m_ctrlButton[0].SetButtonStyle(BS_PUSHBUTTON);
		m_ctrlButton[nDefButton].SetButtonStyle(BS_DEFPUSHBUTTON);
	}

	// Now arrange things. The fun of dialog layout...
	CRect rectDlg;
	GetWindowRect(&rectDlg);
	CRect rText;
	m_ctrlText.GetWindowRect(&rText);
	ScreenToClient(&rText);
	int nDlgSpace = rectDlg.Width() - rText.Width();

	// Determine the space between buttons.
	CRect rButton1, rButton2;
	m_ctrlButton[0].GetWindowRect(&rButton1);
	ScreenToClient(&rButton1);
	m_ctrlButton[1].GetWindowRect(&rButton2);
	ScreenToClient(&rButton2);
	int nBtnSpace = rButton2.left - rButton1.right;

	// Determine how much space the text requires.
	CDC* dc = m_ctrlText.GetDC();
	CFont* pOldFont = dc->SelectObject(m_ctrlText.GetFont());
	CRect newRect(0,0,0,0);
	dc->DrawText(m_Text, &newRect, DT_CALCRECT | DT_NOPREFIX | DT_LEFT);

	// Define the max width to be 80% of the screen.
	CRect rWorkArea(0,0,0,0);
	// TODO: Use the monitor functions and get the size of the monitor
	// under the cursor.
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rWorkArea, 0);
	int maxDlgWidth = (4 * rWorkArea.Width()) / 5;

	// If the calculated space is too big, recalc for height.
	if (newRect.Width() > maxDlgWidth)
	{
		newRect = CRect(0, 0, maxDlgWidth - nDlgSpace, 0);
		dc->DrawText(m_Text, &newRect, DT_CALCRECT | DT_NOPREFIX | DT_LEFT | DT_WORDBREAK);
	}
	// Ok, we're resizing...
	CSize szTweak(0,0);
	if (newRect.Width() != rText.Width() || newRect.Height() != rText.Height())
	{
		szTweak.cx += newRect.Width() - rText.Width();
		int yTweak = newRect.Height() - rText.Height();
		szTweak.cy += yTweak;
		m_ctrlText.SetWindowPos(NULL, 0, 0, newRect.Width(), newRect.Height(), SWP_NOZORDER | SWP_NOMOVE);
		// Tweak up the lower controls.
		if (yTweak != 0)
		{
			for (int i = 0; i < NUM_BUTTONS; ++i)
			{
				CRect rect;
				m_ctrlButton[i].GetWindowRect(&rect);
				ScreenToClient(&rect);
				m_ctrlButton[i].SetWindowPos(NULL, rect.left, rect.top + yTweak, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			// Need to reset the rect.
			m_ctrlButton[0].GetWindowRect(&rButton1);
			ScreenToClient(&rButton1);
		}
	}
	dc->SelectObject(pOldFont);
	m_ctrlText.ReleaseDC(dc);
	dc = NULL;

	// Next, horizontally position the buttons.
	// Assume all buttons are the same width;
	int nAvail = (rectDlg.Width() + szTweak.cx)
		- nButtons * rButton1.Width()
		- (nButtons - 1 ) * nBtnSpace;
	int nPos = nAvail / 2;
	for (int i = 0; i < nButtons; ++i)
	{
		CRect rect;
		m_ctrlButton[i].GetWindowRect(&rect);
		ScreenToClient(&rect);
		m_ctrlButton[i].SetWindowPos(NULL, nPos, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		nPos += rect.Width();
		nPos += nBtnSpace;
	}

	// Finally, adjust the dialog size.
	if (szTweak.cx != 0 || szTweak.cy != 0)
	{
		SetWindowPos(NULL, 0, 0, rectDlg.Width() + szTweak.cx, rectDlg.Height() + szTweak.cy, SWP_NOZORDER | SWP_NOMOVE);
		GetWindowRect(&rectDlg);
	}

	// And now center it.
	CRect rParent;
	CWnd* pParent = GetParent();
	if (pParent)
		pParent->GetWindowRect(&rParent);
	else
		rParent = rWorkArea;
	int x = 0;
	// Take the center of the parent, subtract 1/2 the width of the window
	// and adjust back using the parent's left side. This gives us the left
	// side of the where the window should be. If that value is less than
	// the actual workspace area, adjust the position so the left edge of
	// the window is fully visible.
	if ((x = rParent.left + rParent.Width() / 2 - rectDlg.Width() / 2) < rWorkArea.left)
		x = rWorkArea.left;
	// Otherwise, if the right side of the window exceeds the workspace,
	// shift the window back to the left so the right edge is visible.
	else if (x > rWorkArea.right - rectDlg.Width())
		x = rWorkArea.right - rectDlg.Width();
	int y = 0;
	// Now do exactly the same as above, only for vertical positioning.
	if ((y = rParent.top + rParent.Height() / 2 - rectDlg.Height() / 2) < rWorkArea.top)
		y = rWorkArea.top;
	else if (y > rWorkArea.bottom - rectDlg.Height())
		y = rWorkArea.bottom - rectDlg.Height();
	// Finally, move the window.
	SetWindowPos(NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	SetRedraw(TRUE);
	Invalidate();
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMessageBox::OnDetails()
{
	if (m_Callback)
		m_Callback->OnDetails(this);
}

BOOL CDlgMessageBox::OnCmd(UINT id)
{
	UpdateData(TRUE);
	EndDialog(id);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

int AfxMessageBox2(UINT inIDText, UINT inType,
	IMessageBoxCallback* inCallback, CWnd* inParent)
{
	CString str;
	str.LoadString(inIDText);
	return AfxMessageBox2(str, inType, inCallback, inParent);
}

int AfxMessageBox2(CString const& inText, UINT inType,
	IMessageBoxCallback* inCallback, CWnd* inParent)
{
	CDlgMessageBox dlg(inText, inType, inCallback, inParent);
	return static_cast<int>(dlg.DoModal());
}
