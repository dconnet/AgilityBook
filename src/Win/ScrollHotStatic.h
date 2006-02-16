#pragma once

/////////////////////////////////////////////////////////////////////////////
// This code was obtained from www.codeguru.com by Zhibin Cao.

// ScrollHotStatic.h : header file
//

#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CScrollHotStatic window
#define HOT_COLOR RGB(255,0,0)			// red
#define UNVISITED_COLOR RGB(0,0,255)	// blue
#define VISITED_COLOR RGB(128,0,128)	// purple

#define ITEM_CHOOSED	1
#define ITEM_VISITED	2

typedef struct {
	char m_strText[255];
	char m_strLink[255];
	DWORD	m_dwStartXPos;
	DWORD	m_dwFlag;
} STATICITEM;
typedef std::vector<STATICITEM> ITEMVECTOR;

extern int	g_nMemOffset, g_nMemHeight, g_bMoveControl;
extern BOOL g_bControl;

//#define GLOBAL_VISITED_INFORMATION
class CScrollMemDC : public CDC
{
public:
    // constructor sets up the memory DC
    CScrollMemDC(CDC* pDC) : CDC()
    {
        ASSERT(pDC != NULL);

        m_pDC = pDC;
        m_pOldBitmap = NULL;
#ifndef WCE_NO_PRINTING
        m_bMemDC = !pDC->IsPrinting();
#else
        m_bMemDC = FALSE;
#endif

        if (m_bMemDC)    // Create a Memory DC
        {
			pDC->GetWindow()->GetClientRect(&m_rect);
            CreateCompatibleDC(pDC);
            m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
            m_pOldBitmap = SelectObject(&m_bitmap);
#ifndef _WIN32_WCE
            SetWindowOrg(m_rect.left, m_rect.top);
#endif
			FillRect(m_rect, &CBrush(GetSysColor(COLOR_BTNFACE)));
        }
        else // Make a copy of the relevent parts of the current DC for printing
        {
#if !defined(_WIN32_WCE) || ((_WIN32_WCE > 201) && !defined(WCE_NO_PRINTING))
            m_bPrinting = pDC->m_bPrinting;
#endif
            m_hDC       = pDC->m_hDC;
            m_hAttribDC = pDC->m_hAttribDC;
        }

		if( !g_bControl && g_nMemHeight == 0 && g_nMemOffset == 0 )
		{
			g_nMemOffset = m_rect.Height();
			g_bControl = TRUE;
		}

		if( g_bControl && g_nMemOffset == 0 )
		{
			g_nMemHeight = 0;
			g_bControl = !g_bControl;
		}

		if( !g_bControl && g_nMemHeight == m_rect.Height() )
		{
			g_nMemHeight = 0;
			g_nMemOffset = m_rect.Height();
			g_bControl = !g_bControl;
		}
	
    }

    // Destructor copies the contents of the mem DC to the original DC
    ~CScrollMemDC()
    {
        if (m_bMemDC)
        {
            // Copy the offscreen bitmap onto the screen.
            m_pDC->BitBlt(m_rect.left, m_rect.top+g_nMemOffset, m_rect.Width(), m_rect.Height()-(g_bControl?g_nMemOffset:g_nMemHeight),
                          this, m_rect.left, m_rect.top+g_nMemHeight, SRCCOPY);

			if( g_bMoveControl )
			{
				if( !g_bControl )
					g_nMemHeight ++;
				else
					g_nMemOffset --;
			}

            //Swap back the original bitmap.
            SelectObject(m_pOldBitmap);
        }
		else
		{
            // All we need to do is replace the DC with an illegal value,
            // this keeps us from accidently deleting the handles associated with
            // the CDC that was passed to the constructor.
            m_hDC = m_hAttribDC = NULL;
        }
    }

    // Allow usage as a pointer
    CScrollMemDC* operator->() {return this;}
        
    // Allow usage as a pointer
    operator CScrollMemDC*() {return this;}

private:
    CBitmap  m_bitmap;      // Offscreen bitmap
    CBitmap* m_pOldBitmap;  // bitmap originally found in CMemDC
    CDC*     m_pDC;         // Saves CDC passed in constructor
    CRect    m_rect;        // Rectangle of drawing area.
    BOOL     m_bMemDC;      // TRUE if CDC really is a Memory DC.
	int		m_nRedrawTimer;
};

class CScrollHotStatic : public CStatic
{
	COLORREF		m_colorUnvisited;         // color for unvisited
	COLORREF		m_colorVisited;           // color for visited
	COLORREF		m_colorHighlited;         // color for highlited
	COLORREF		m_colorBlack;

	BOOL			m_bSunken;
	BOOL			m_bBorder;
public:
	// Construction
	CScrollHotStatic();
	~CScrollHotStatic();
	
#ifdef GLOBAL_VISITED_INFORMATION
	static BOOL		m_bVisited;               // whether visited or not
#else
	BOOL		m_bVisited;					  // whether visited or not
#endif
	
	void AddText(LPCTSTR lpStrText, LPCTSTR lpStrLink=NULL, DWORD dwStartXPos=0);

	void SetVisitedColor(COLORREF rgb)
	{
		m_colorVisited = rgb;
	};

	void SetUnisitedColor(COLORREF rgb)
	{
		m_colorUnvisited = rgb;
	};

	void SetHighlitedColor(COLORREF rgb)
	{
		m_colorHighlited = rgb;
	};

	void SetBorder(BOOL bBorder)
	{
		m_bBorder = bBorder;
	};

	void SetSunken(BOOL bSunken)
	{
		m_bSunken = bSunken;
	};
	
protected:
	DECLARE_DYNAMIC(CScrollHotStatic)
	CFont		m_font;                  // underline font for text control
	CFont		m_NoUnderLineFont;
	UINT		m_nFontHeight;
	UINT		m_nRedrawTimer;
	HCURSOR		m_hHandCursor;
	HCURSOR		m_hArrowCursor;

	// message handlers
	//{{AFX_MSG(CScrollHotStatic)
	afx_msg void    OnClicked();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	LPARAM  OnMouseLeave(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()

	ITEMVECTOR	m_ItemVector;
};
