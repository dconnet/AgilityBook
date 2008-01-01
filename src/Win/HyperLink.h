#pragma once

/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @brief HyperLink static control.
 * @author David Connet
 *
 * Comments from original Hyperlink code:
 *  HyperLink static control. Will open the default browser with the given URL
 *  when the user clicks on the link.
 *  Copyright (C) 1997 - 1999 Chris Maunder
 *  All rights reserved. May not be sold for profit.
 *
 *  Thanks to Pal K. Tonder for auto-size and window caption changes.
 *
 *  "GotoURL" function by Stuart Patterson
 *  As seen in the August, 1997 Windows Developer's Journal.
 *  Copyright 1997 by Miller Freeman, Inc. All rights reserved.
 *  Modified by Chris Maunder to use TCHARs instead of chars.
 *
 *  "Default hand cursor" from Paul DiLascia's Jan 1998 MSJ article.
 *
 *  2/29/00 -- P. Shaffer standard font mod.
 * The code has been significantly changed and some problems with
 * the tooltip fixed.
 *
 * Revision History
 * @li 2005-04-01 DRC Made hyperlinks keyboard-capable.
 * @li 2004-03-05 DRC Made goto-url functionality visible.
 * @li 2003-08-16 DRC Modified code from Chris Maunder's original
 */

class CHyperLink : public CStatic
{
public:
	static bool GotoURL(CString const& url);
	CHyperLink();
	virtual ~CHyperLink();

public:
	enum UnderLineOptions
	{
		ulHover = -1,
		ulNone = 0,
		ulAlways = 1
	};

public:
	void SetURL(CString const& strURL);
	CString GetURL() const					{return m_strURL;}

	void SetColors(
			COLORREF crLinkColor,
			COLORREF crVisitedColor,
			COLORREF crHoverColor = -1);
	COLORREF GetLinkColor() const			{return m_crLinkColor;}
	COLORREF GetVisitedColor() const		{return m_crVisitedColor;}
	COLORREF GetHoverColor() const			{return m_crHoverColor;}

	void SetVisited(bool bVisited = true);
	bool GetVisited() const					{return m_bVisited;}

	void SetLinkCursor(HCURSOR hCursor);
	HCURSOR GetLinkCursor() const			{return m_hLinkCursor;}

	void SetUnderline(UnderLineOptions nUnderline = ulHover);
	UnderLineOptions GetUnderline() const	{return m_nUnderline;}

	void SetAutoSize(bool bAutoSize = true);
	bool GetAutoSize() const				{return m_bAdjustToFit;}

// Overrides
	//{{AFX_VIRTUAL(CHyperLink)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DrawFocusRect();
	void PositionWindow();
	void SetDefaultCursor();

// Protected attributes
protected:
	COLORREF m_crLinkColor;			// Hyperlink colours
	COLORREF m_crVisitedColor;		// Hyperlink colours
	COLORREF m_crHoverColor;		// Hover colour
	bool m_bOverControl;			// cursor over control?
	bool m_bVisited;				// Has it been visited?
	UnderLineOptions m_nUnderline;	// underline hyperlink?
	bool m_bAdjustToFit;			// Adjust window size to fit text?
	CString m_strURL;				// hyperlink URL
	CFont m_UnderlineFont;			// Font for underline display
	CFont m_StdFont;				// Standard font
	HCURSOR m_hLinkCursor;			// Cursor for hyperlink
	CToolTipCtrl m_ToolTip;			// The tooltip
	UINT m_nTimerID;

protected:
	//{{AFX_MSG(CHyperLink)
	afx_msg UINT OnGetDlgCode();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClicked();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
