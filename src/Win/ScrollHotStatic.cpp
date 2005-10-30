// ScrollHotStatic.cpp : implementation file
// This code was obtained from www.codeguru.com by Zhibin Cao.
//

#include "stdafx.h"
#include "resource.h"
#include "ScrollHotStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScrollHotStatic
BOOL g_bControl = FALSE, g_bMoveControl = TRUE, g_bFirstTime = TRUE;
int	g_nMemOffset = 0, g_nMemHeight = 0;

IMPLEMENT_DYNAMIC(CScrollHotStatic, CStatic)

BEGIN_MESSAGE_MAP(CScrollHotStatic, CStatic)
	//{{AFX_MSG_MAP(CScrollHotStatic)
	ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor sets default colors = blue/purple.
//
#ifdef GLOBAL_VISITED_INFORMATION
BOOL CScrollHotStatic::m_bVisited = FALSE;
#endif

CScrollHotStatic::CScrollHotStatic()
	: m_colorUnvisited(UNVISITED_COLOR)
	, m_colorVisited(VISITED_COLOR)
	, m_colorHighlited(RGB(255,0,0))
	, m_colorBlack(RGB(0,0,0))
	, m_bSunken(FALSE)
	, m_bBorder(TRUE)
	, m_nFontHeight(-1)
	, m_nRedrawTimer(-1)
#ifndef GLOBAL_VISITED_INFORMATION
	, m_bVisited(FALSE)
#endif
{
	m_hHandCursor = AfxGetApp()->LoadCursor(MAKEINTRESOURCE(IDC_CROSS /*IDC_HAND*/));
	m_hArrowCursor = ::LoadCursor(NULL, IDC_ARROW);
	::SetCursor(m_hArrowCursor);
}

CScrollHotStatic::~CScrollHotStatic()
{
}

void CScrollHotStatic::AddText(
		LPCTSTR lpStrText,
		LPCTSTR lpStrLink,
		DWORD dwStartXPos)
{
	STATICITEM	item;

	if (lpStrLink != NULL)
		strcpy(item.m_strLink, lpStrLink);
	else
		item.m_strLink[0] = 0;

	strcpy(item.m_strText,lpStrText);
	item.m_dwFlag = 0;
	item.m_dwStartXPos = dwStartXPos;
	m_ItemVector.push_back(item);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Handle mouse click: open URL/file.
//
void CScrollHotStatic::OnClicked()
{
	int j = m_ItemVector.size();
	for (int i = 0; i < j && !(m_ItemVector[i].m_dwFlag & ITEM_CHOOSED); ++i)
		;

	if (i == j)
		return;

	HINSTANCE h = ShellExecute(NULL, _T("open"), m_ItemVector[i].m_strLink, NULL, NULL, SW_SHOWNORMAL);
	if (static_cast<UINT>(h) > 32)
	{
		m_bVisited = TRUE;
		m_ItemVector[i].m_dwFlag |= ITEM_VISITED;
		Invalidate();
	}
	else
	{
		MessageBeep(0);
		TRACE(_T("*** WARNING: CScrollHotStatic: unable to execute file %s\n"),(LPCTSTR)m_ItemVector[0].m_strLink);
	}
}

void CScrollHotStatic::OnMouseMove(
		UINT nFlags,
		CPoint point)
{
	if (g_bMoveControl)
	{
		KillTimer(m_nRedrawTimer);

		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE;
		_TrackMouseEvent(&tme);
		g_bMoveControl = FALSE;
	}
	else
	{
		int nHeight = point.y + g_nMemHeight - g_nMemOffset;
		int bSelected = FALSE;
		for (int i = 0; i < m_ItemVector.size(); ++i)
		{
			m_ItemVector[i].m_dwFlag &= ~ITEM_CHOOSED;
			if (m_ItemVector[i].m_strLink[0] != 0
			&& m_nFontHeight != -1
			&& nHeight < m_nFontHeight * (i+1)
			&& nHeight >= m_nFontHeight * i)
			{
				m_ItemVector[i].m_dwFlag |= ITEM_CHOOSED;
				bSelected = TRUE;
			}
		}

		if (bSelected)
			::SetCursor(m_hHandCursor);
		else
			::SetCursor(m_hArrowCursor);
		Invalidate(FALSE);
		UpdateWindow();
	}
}

LPARAM CScrollHotStatic::OnMouseLeave(
		WPARAM wp,
		LPARAM lp)
{
	g_bMoveControl = TRUE;
	SetTimer(m_nRedrawTimer, 30, NULL);
	::SetCursor(m_hArrowCursor);
	for (int i = 0; i < m_ItemVector.size(); ++i)
		m_ItemVector[i].m_dwFlag &= ~ITEM_CHOOSED;

	return 0;
}

void CScrollHotStatic::OnPaint()
{
	if (g_bFirstTime)
	{
		::SetClassLong(m_hWnd, GCL_HCURSOR, NULL);
		g_bFirstTime = FALSE;
	}

	CPaintDC PaintDC(this);
	CScrollMemDC dc(&PaintDC);
	BOOL bPic = FALSE;

	if (m_nRedrawTimer == -1)
		m_nRedrawTimer = SetTimer(1000, 30, NULL);

	if (!reinterpret_cast<HFONT>(m_font))
	{
		CFont *pFont = GetFont();
		if (pFont)
		{
			LOGFONT lf;
			pFont->GetObject(sizeof(lf), &lf);
			lf.lfUnderline = TRUE;
			m_font.CreateFontIndirect(&lf);
		}
		CSize size;
		GetTextExtentPoint32(dc.GetSafeHdc(), _T("A"), 1, &size);
		m_nFontHeight = size.cy;
	}

	if (!reinterpret_cast<HFONT>(m_NoUnderLineFont))
	{
		CFont *pFont = GetFont();
		if (pFont)
		{
			LOGFONT lf;
			pFont->GetObject(sizeof(lf), &lf);
			lf.lfUnderline = FALSE;
			m_NoUnderLineFont.CreateFontIndirect(&lf);
		}
	}

	CRect rect;
	GetClientRect(&rect);
	for (int i = 0 ; i < m_ItemVector.size(); ++i)
	{
		if (m_ItemVector[i].m_strLink[0] != 0)
		{
			dc.SelectObject(&m_font);
			dc.SetTextColor((m_ItemVector[i].m_dwFlag & ITEM_CHOOSED)
				? m_colorHighlited : ((m_ItemVector[i].m_dwFlag & ITEM_VISITED)
					? m_colorVisited : m_colorUnvisited));
		}
		else
		{
			dc.SelectObject(&m_NoUnderLineFont);
			dc.SetTextColor(m_colorBlack);
		}

		dc.SetBkMode(TRANSPARENT);
		rect.left = m_ItemVector[i].m_dwStartXPos;
		dc.DrawText(m_ItemVector[i].m_strText, rect, DT_LEFT | DT_WORDBREAK);
		rect.top += m_nFontHeight;
	}
}

void CScrollHotStatic::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == m_nRedrawTimer)
	{
		Invalidate(FALSE);
		UpdateWindow();
	}
	CStatic::OnTimer(nIDEvent);
}

void CScrollHotStatic::OnDestroy()
{
	KillTimer(m_nRedrawTimer);
	if (m_hHandCursor)
		::DestroyCursor(m_hHandCursor);
	if (m_hArrowCursor)
		::DestroyCursor(m_hArrowCursor);
	CStatic::OnDestroy();
}

BOOL CScrollHotStatic::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

BOOL CScrollHotStatic::OnSetCursor(
		CWnd* /*pWnd*/,
		UINT /*nHitTest*/,
		UINT /*message*/)
{
	return TRUE;
}
