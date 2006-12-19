/*
 * Copyright © 2006 David Connet. All Rights Reserved.
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
 * @brief interface of the CAgilityBookViewHtml class
 * @author David Connet
 *
 * Revision History
 * @li 2006-09-10 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "AgilityBookViewHtml.h"
#if _MSC_VER < 1300
#include "htmlhelp.h"
#endif

#include <sstream>
#include <afxpriv.h> // wm_commandhelp
#include "comdef.h" // for IHTMLDocument2Ptr
#if _MSC_VER >= 1300
#include <atlsafe.h>
#else
#include <atlbase.h>
#include "MSHTML.H"
#endif

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTreeData.h"
#include "ClipBoard.h"
#include "DlgPointsViewSort.h"
#include "FilterOptions.h"
#include "MainFrm.h"
#include "PointsData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookViewHtml

IMPLEMENT_DYNCREATE(CAgilityBookViewHtml, CHtmlView)

BEGIN_MESSAGE_MAP(CAgilityBookViewHtml, CHtmlView)
	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_MOUSEACTIVATE()
	ON_UPDATE_COMMAND_UI(ID_AGILITY_NEW_TITLE, OnUpdateAgilityNewTitle)
	ON_COMMAND(ID_AGILITY_NEW_TITLE, OnAgilityNewTitle)
	ON_COMMAND(ID_VIEW_POINTS_VIEW_SORT, OnViewPointsViewSort)
	ON_COMMAND(ID_VIEW_HIDDEN, OnViewHiddenTitles)
END_MESSAGE_MAP()

// CAgilityBookViewHtml construction/destruction

CAgilityBookViewHtml::CAgilityBookViewHtml()
	: m_DocAsControl(NULL)
	, m_Items(new CPointsDataItems())
{
}

CAgilityBookViewHtml::CAgilityBookViewHtml(CAgilityBookDoc* inDocAsControl)
	: m_DocAsControl(inDocAsControl)
	, m_Items(new CPointsDataItems())
{
}

CAgilityBookViewHtml::~CAgilityBookViewHtml()
{
	m_Items->clear();
	delete m_Items;
}

CAgilityBookDoc* CAgilityBookViewHtml::GetDocument() const
{
	if (m_DocAsControl)
		return m_DocAsControl;
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAgilityBookDoc)));
	return reinterpret_cast<CAgilityBookDoc*>(m_pDocument);
}

BOOL CAgilityBookViewHtml::CreateFromStatic(UINT nID, CWnd* pParent)
{
	CStatic wndStatic;
	if (!wndStatic.SubclassDlgItem(nID, pParent))
		return FALSE;

	// Get the styles we want.
	DWORD style = wndStatic.GetStyle();
	style &= (WS_BORDER | WS_GROUP | WS_TABSTOP);
	style |= (WS_CHILD | WS_VISIBLE);
	// Get static control rect, convert to parent's client coords.
	CRect rc;
	wndStatic.GetWindowRect(&rc);
	pParent->ScreenToClient(&rc);
	wndStatic.DestroyWindow();

	// create HTML control (CHtmlView)
	BOOL bOk = Create(NULL,	// class name
			NULL,			// title
			style,			// style
			rc,				// rectangle
			pParent,		// parent
			nID,			// control ID
			NULL);			// frame/doc context not used

	return bOk;
}

bool CAgilityBookViewHtml::SetHTML(ARBString const& csHTML)
{
	// Make sure we're initialized.
	LPDISPATCH lpDispatch = GetHtmlDocument();
	if (!lpDispatch)
	{
		Navigate(_T("about:blank"));
		lpDispatch = GetHtmlDocument();
	}

	bool bOk = false;
	if (lpDispatch)
	{
		IHTMLDocument2Ptr pDoc2 = NULL;
		if (SUCCEEDED(lpDispatch->QueryInterface(IID_IHTMLDocument2, (LPVOID*)&pDoc2)))
		{
			CComBSTR content(csHTML.c_str());
#if _MSC_VER >= 1300
			CComSafeArray<VARIANT> sar;
			if (SUCCEEDED(sar.Create(1, 0)))
			{
				bOk = true;
				sar[0] = content;
			}
#else
			SAFEARRAY* sar = ::SafeArrayCreateVector(VT_VARIANT, 0, 1);
			VARIANT *pElement = NULL;
			if (sar)
			{
				if (SUCCEEDED(::SafeArrayAccessData(sar, (LPVOID*)&pElement)))
				{
					pElement->vt = VT_BSTR;
					pElement->bstrVal = content;
					if (SUCCEEDED(SafeArrayUnaccessData(sar)))
					{
						bOk = true;
					}
				}
			}
#endif
			if (bOk)
			{
				bOk = false;
				// open doc and write
				LPDISPATCH lpdRet;
				if (SUCCEEDED(pDoc2->open(CComBSTR("text/html"),
					CComVariant(CComBSTR("_self")),
					CComVariant(CComBSTR("")),
					CComVariant((bool)1),
					&lpdRet)))
				{
					if (SUCCEEDED(pDoc2->write(sar)))
					{
						bOk = true;
					}
					pDoc2->close();
				}
				if (lpdRet)
					lpdRet->Release();
			}
#if _MSC_VER >= 1300
#else
			if (sar)
				SafeArrayDestroy(sar);
#endif
		}
		lpDispatch->Release();
	}

	return bOk;
}

bool CAgilityBookViewHtml::IsFiltered() const
{
	return CFilterOptions::Options().IsFilterEnabled();
}

bool CAgilityBookViewHtml::GetMessage(CString& msg) const
{
	msg.LoadString(IDS_INDICATOR_BLANK);
	return true;
}

bool CAgilityBookViewHtml::GetMessage2(CString& msg) const
{
	if (GetDocument()->GetCurrentDog())
	{
		msg = GetDocument()->GetCurrentDog()->GetCallName().c_str();
		return true;
	}
	else
	{
		msg.Empty();
		return false;
	}
}

void CAgilityBookViewHtml::LoadData()
{
	CWaitCursor wait;

	m_Items->LoadData(this, GetDocument(), GetDocument()->GetCurrentDog());

	ARBDate today(ARBDate::Today());
	ARBostringstream data;

	CString title;
	title.LoadString(IDS_TITLING_POINTS);

	data << _T("<html>")
		<< std::endl
		<< _T("<head><title>") << title << _T(" ")
		<< today.GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints))
		<< _T("</title></head>")
		<< std::endl
		<< _T("<body>")
		<< std::endl;

	size_t nItems = m_Items->NumLines();
	if (0 < nItems)
	{
		CPointsDataBasePtr item = m_Items->GetLine(0);
		data << item->GetHtml(0);
	}
	for (size_t nItem = 1; nItem < nItems; ++nItem)
	{
		CPointsDataBasePtr item = m_Items->GetLine(nItem);
		data << item->GetHtml(nItem);
	}
	data << _T("</body></html>") << std::endl;
	SetHTML(data.str());
}

// CAgilityBookViewHtml diagnostics

#ifdef _DEBUG
void CAgilityBookViewHtml::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CAgilityBookViewHtml::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

#if _MSC_VER < 1300
// <q241750>

CString CAgilityBookViewHtml::GetFullName() const
{
	ASSERT(m_pBrowserApp != NULL);
	BSTR bstr;
	m_pBrowserApp->get_FullName(&bstr);
	CString retVal(bstr);
	SysFreeString(bstr); // Added this line to prevent leak.
	return retVal;
}

CString CAgilityBookViewHtml::GetType() const
{
	ASSERT(m_pBrowserApp != NULL);
	BSTR bstr;
	m_pBrowserApp->get_Type(&bstr);
	CString retVal(bstr);
	SysFreeString(bstr); // Added this line to prevent leak.
	return retVal;
}

CString CAgilityBookViewHtml::GetLocationName() const
{
	ASSERT(m_pBrowserApp != NULL);
	BSTR bstr;
	m_pBrowserApp->get_LocationName(&bstr);
	CString retVal(bstr);
	SysFreeString(bstr); // Added this line to prevent leak.
	return retVal;
}

CString CAgilityBookViewHtml::GetLocationURL() const
{
	ASSERT(m_pBrowserApp != NULL);
	BSTR bstr;
	m_pBrowserApp->get_LocationURL(&bstr);
	CString retVal(bstr);
	SysFreeString(bstr); // Added this line to prevent leak.
	return retVal;
}

void CAgilityBookViewHtml::Navigate(
		LPCTSTR lpszURL,
		DWORD dwFlags,
		LPCTSTR lpszTargetFrameName,
		LPCTSTR lpszHeaders,
		LPVOID lpvPostData,
		DWORD dwPostDataLen)
{
	CString strURL(lpszURL);
	BSTR bstrURL = strURL.AllocSysString();
	COleSafeArray vPostData;
	if (lpvPostData != NULL)
	{
		if (dwPostDataLen == 0)
			dwPostDataLen = lstrlen((LPCTSTR)lpvPostData);
		vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
	}
	m_pBrowserApp->Navigate(bstrURL,
			COleVariant((long) dwFlags, VT_I4),
			COleVariant(lpszTargetFrameName, VT_BSTR),
			vPostData,
			COleVariant(lpszHeaders, VT_BSTR));
	SysFreeString(bstrURL); // Added this line to prevent leak.
}

BOOL CAgilityBookViewHtml::LoadFromResource(LPCTSTR lpszResource)
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);
	CString strResourceURL;
	BOOL bRetVal = TRUE;
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];
	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		strResourceURL.Format(_T("res://%s/%s"), lpszModule, lpszResource);
		Navigate(strResourceURL, 0, 0, 0);
	}
	else
		bRetVal = FALSE;
	delete [] lpszModule;
	return bRetVal;
}

BOOL CAgilityBookViewHtml::LoadFromResource(UINT nRes)
{
	HINSTANCE hInstance = AfxGetResourceHandle();
	ASSERT(hInstance != NULL);
	CString strResourceURL;
	BOOL bRetVal = TRUE;
	LPTSTR lpszModule = new TCHAR[_MAX_PATH];
	if (GetModuleFileName(hInstance, lpszModule, _MAX_PATH))
	{
		strResourceURL.Format(_T("res://%s/%d"), lpszModule, nRes);
		Navigate(strResourceURL, 0, 0, 0);
	}
	else
		bRetVal = FALSE;
	delete [] lpszModule;
	return bRetVal;
}

#endif

/////////////////////////////////////////////////////////////////////////////

BOOL CAgilityBookViewHtml::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPCHILDREN; // q220021
	return CHtmlView::PreCreateWindow(cs);
}

void CAgilityBookViewHtml::OnActivateView(
		BOOL bActivate,
		CView* pActivateView,
		CView* pDeactiveView)
{
	CHtmlView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if (pActivateView)
	{
		CString msg;
		if (GetMessage(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText(msg, IsFiltered());
		if (GetMessage2(msg))
			reinterpret_cast<CMainFrame*>(AfxGetMainWnd())->SetStatusText2(msg);
	}
}

void CAgilityBookViewHtml::OnUpdate(
		CView* pSender,
		LPARAM lHint,
		CObject* pHint)
{
	if (0 == lHint
	|| (UPDATE_POINTS_VIEW & lHint)
	|| (UPDATE_OPTIONS & lHint)
	|| (UPDATE_CONFIG & lHint))
		LoadData();
}

void CAgilityBookViewHtml::PostNcDestroy()
{
	//m_Items->clear();
	if (!m_DocAsControl)
		CHtmlView::PostNcDestroy();
	else
		m_pBrowserApp = NULL;
}

void CAgilityBookViewHtml::OnInitialUpdate()
{
	LoadData();
}

void CAgilityBookViewHtml::OnBeforeNavigate2(
		LPCTSTR lpszURL,
		DWORD nFlags,
		LPCTSTR lpszTargetFrameName,
		CByteArray& baPostedData,
		LPCTSTR lpszHeaders,
		BOOL* pbCancel)
{
	static const TCHAR ABOUT_PROTOCOL[] = _T("about:");
	static const TCHAR HTTP_PROTOCOL[] = _T("http:");
	static const TCHAR HTTPS_PROTOCOL[] = _T("https:");
	static size_t lenAbout = _tcslen(ABOUT_PROTOCOL);
	static size_t lenHttp = _tcslen(HTTP_PROTOCOL);
	static size_t lenHttps = _tcslen(HTTPS_PROTOCOL);
	static size_t lenApp = _tcslen(ARB_PROTOCOL);

	if (_tcsnicmp(lpszURL, ABOUT_PROTOCOL, lenAbout) == 0)
	{
		// Let About thru (for "about:blank")
	}
	else if (_tcsnicmp(lpszURL, ARB_PROTOCOL, lenApp) == 0)
	{
		// Our special internal link
		// Remember, spaces are now %20. Other special chars may
		// need fixing too. Just don't use those in our links.
		bool bDidIt = false;
		ARBString url(lpszURL+lenApp);
		if (!url.empty())
		{
			size_t nItem = _ttol(url.c_str());

			if (nItem < m_Items->NumLines())
			{
				bDidIt = true;

				CPointsDataBasePtr item = m_Items->GetLine(nItem);
				item->Details();
			}
		}
		if (!bDidIt)
		{
			MessageBeep(MB_ICONEXCLAMATION);
		}
		*pbCancel = TRUE;
	}
	else if (_tcsnicmp(lpszURL, HTTP_PROTOCOL, lenHttp) == 0
	|| _tcsnicmp(lpszURL, HTTPS_PROTOCOL, lenHttps) == 0)
	{
		// Don't allow links to replace us.
		RunCommand(lpszURL);
		// Note, using 'target="new"' in the html href tag will cause the new
		// window to open in IE, which is not necessarily the default browser.
		*pbCancel = TRUE;
	}
	else
	{
		// Don't allow any other types of links.
		TRACE("Preventing navigation to '%s'\n", lpszURL);
		MessageBeep(MB_ICONEXCLAMATION);
		*pbCancel = TRUE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Message Handlers

LRESULT CAgilityBookViewHtml::OnCommandHelp(WPARAM, LPARAM)
{
	AfxGetApp()->WinHelp(HID_BASE_RESOURCE+IDR_POINTS, HH_HELP_CONTEXT);
	return 1;
}

int CAgilityBookViewHtml::OnCreate(LPCREATESTRUCT lpcs)
{
	if (-1 == CHtmlView::OnCreate(lpcs))
		return -1;

	return 0;
}

void CAgilityBookViewHtml::OnDestroy()
{
	if (!m_DocAsControl)
		CHtmlView::OnDestroy();
	else
		CWnd::OnDestroy();
}

int CAgilityBookViewHtml::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT msg)
{
	if (!m_DocAsControl)
		return CHtmlView::OnMouseActivate(pDesktopWnd, nHitTest, msg);
	else
		return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, msg);
}

void CAgilityBookViewHtml::OnUpdateAgilityNewTitle(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	ARBDogPtr pDog = GetDocument()->GetCurrentDog();
	if (pDog)
		bEnable = TRUE;
	pCmdUI->Enable(bEnable);
}

void CAgilityBookViewHtml::OnAgilityNewTitle()
{
	ARBDogPtr pDog = GetDocument()->GetCurrentDog();
	if (pDog)
	{
		// Convenience! No duplicated code!
		CAgilityBookTreeDataDog data(GetDocument()->GetTreeView(), pDog);
		if (data.OnCmd(ID_AGILITY_NEW_TITLE, NULL))
			GetDocument()->SetModifiedFlag();
	}
}

void CAgilityBookViewHtml::OnViewPointsViewSort()
{
	CDlgPointsViewSort dlg(this);
	if (IDOK == dlg.DoModal())
		LoadData();
}

void CAgilityBookViewHtml::OnViewHiddenTitles()
{
	CAgilityBookOptions::SetViewHiddenTitles(!CAgilityBookOptions::GetViewHiddenTitles());
	std::vector<CVenueFilter> venues;
	CFilterOptions::Options().GetFilterVenue(venues);
	for (ARBDogList::iterator iterDogs = GetDocument()->GetDogs().begin();
			iterDogs != GetDocument()->GetDogs().end();
			++iterDogs)
	{
		for (ARBDogTitleList::iterator iterTitle = (*iterDogs)->GetTitles().begin();
				iterTitle != (*iterDogs)->GetTitles().end();
				++iterTitle)
		{
			GetDocument()->ResetVisibility(venues, *iterTitle);
		}
	}
	LoadData();
}
