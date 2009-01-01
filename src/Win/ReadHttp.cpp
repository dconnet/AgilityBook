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
 * @brief Read HTTP from the net.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-01 DRC Fix closing the connection - Close didn't reset null.
 * @li 2007-08-03 DRC Separated HTTP reading code from UpdateInfo.cpp
 */

#include "stdafx.h"
#include "resource.h"
#include "ReadHttp.h"

#include "DlgAuthenticate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CReadHttp::CReadHttp(
		CString const& inURL,
		CStringA& outData)
	: m_URL(inURL)
	, m_Data(outData)
	, m_session(NULL)
	, m_pServer(NULL)
	, m_pFile(NULL)
	, m_strServerName()
	, m_strObject()
	, m_nPort(0)
{
}


CReadHttp::~CReadHttp()
{
	Close();
}


void CReadHttp::CloseFiles()
{
	if (m_pFile)
	{
		m_pFile->Close();
		delete m_pFile;
		m_pFile = NULL;
	}
	if (m_pServer)
	{
		m_pServer->Close();
		delete m_pServer;
		m_pServer = NULL;
	}
}


void CReadHttp::Close()
{
	CloseFiles();
	if (m_session)
	{
		m_session->Close();
		delete m_session;
		m_session = NULL;
	}
}


// existing connect is for ftp
static DWORD dwHttpRequestFlags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT;
static const TCHAR szHeaders[] = _T("Accept: text\r\n");


DWORD CReadHttp::Connect(CString& userName, CString& outErrMsg, CWnd* pParent)
{
	if (!m_session)
		m_session = new CInternetSession(_T("my version"));
	m_pServer = m_session->GetHttpConnection(m_strServerName, m_nPort);
	if (!m_pServer)
		return HTTP_STATUS_SERVER_ERROR;
	m_pFile = m_pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET,
			m_strObject, NULL, 1, NULL, NULL, dwHttpRequestFlags);
	if (!m_pFile)
		return HTTP_STATUS_SERVER_ERROR;
	m_pFile->AddRequestHeaders(szHeaders);
	m_pFile->SendRequest();
	DWORD dwRet;
	m_pFile->QueryInfoStatusCode(dwRet);
	while (dwRet == HTTP_STATUS_DENIED && m_pServer && m_pFile)
	{
		CloseFiles();
		CDlgAuthenticate dlg(userName, pParent);
		if (IDOK == dlg.DoModal())
		{
			m_pServer = m_session->GetHttpConnection(m_strServerName, m_nPort, dlg.GetUserName(), dlg.GetPassword());
			if (!m_pServer)
				return HTTP_STATUS_SERVER_ERROR;
			m_pFile = m_pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET,
					m_strObject, NULL, 1, NULL, NULL, dwHttpRequestFlags);
			if (!m_pFile)
				return HTTP_STATUS_SERVER_ERROR;
			m_pFile->AddRequestHeaders(szHeaders);
			m_pFile->SendRequest();
			m_pFile->QueryInfoStatusCode(dwRet);
		}
		else
		{
			outErrMsg.LoadString(IDS_HTTP_ACCESS_DENIED);
		}
	}
	return dwRet;
}


bool CReadHttp::ReadHttpFile(CString& userName, CString& outErrMsg, CWnd* pParent)
{
	bool bOk = false;
	outErrMsg.Empty();
	m_Data.Empty();
	CWaitCursor wait;
	try
	{
		// Code is based on the MSDN 'TEAR' sample.
		// Simply using OpenURL() is easier, but it doesn't handle redirection.
		DWORD dwServiceType;
		if (AfxParseURL(m_URL, dwServiceType, m_strServerName, m_strObject, m_nPort)
		&& dwServiceType == INTERNET_SERVICE_HTTP)
		{
			DWORD dwRet = Connect(userName, outErrMsg, pParent);

			// If we've been redirected, re-parse. In theory SendRequest()
			// should do this if NO_AUTO_REDIRECT is not specified. I haven't
			// figured out how yet...
			if (dwRet == HTTP_STATUS_MOVED
			|| dwRet == HTTP_STATUS_REDIRECT
			|| dwRet == HTTP_STATUS_REDIRECT_METHOD)
			{
				CString strNewLocation;
				m_pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, strNewLocation);
				int nPlace = strNewLocation.Find(_T("Location: "));
				if (-1 == nPlace)
				{
					CString tmp;
					tmp.LoadString(IDS_HTTP_INVALID_HEADER_QUERY);
					outErrMsg += _T("\n") + tmp + _T(": ") + strNewLocation;
					strNewLocation.Empty();
				}
				else
				{
					strNewLocation = strNewLocation.Mid(nPlace + 10);
					nPlace = strNewLocation.Find('\n');
					if (nPlace > 0)
						strNewLocation = strNewLocation.Left(nPlace);
					// Put what we're accessing back into the url.
					strNewLocation += m_strObject;
				}
				// Now close the existing connections...
				CloseFiles();
				// And reopen them.
				if (!strNewLocation.IsEmpty()
				&& AfxParseURL(strNewLocation, dwServiceType, m_strServerName, m_strObject, m_nPort))
				{
					// try again at the new location
					dwRet = Connect(userName, outErrMsg, pParent);

					if (dwRet == HTTP_STATUS_MOVED
					|| dwRet == HTTP_STATUS_REDIRECT
					|| dwRet == HTTP_STATUS_REDIRECT_METHOD)
					{
						CString tmp;
						tmp.LoadString(IDS_HTTP_URL_REDIRECTION);
						outErrMsg += _T("\n") + tmp + _T(": ") + m_URL;
						tmp.LoadString(IDS_HTTP_INVALID_URL2);
						outErrMsg += _T("\n") + tmp + _T(": ") + strNewLocation;
						CloseFiles();
					}
					else if (dwRet != HTTP_STATUS_OK)
					{
						CString msg;
						msg.FormatMessage(IDS_HTTP_INVALID_URL_ERR, dwRet);
						outErrMsg += _T("\n") + msg + _T(": ") + strNewLocation;
						CloseFiles();
					}
				}
				else
				{
					CString tmp;
					tmp.LoadString(IDS_HTTP_INVALID_URL);
					outErrMsg += _T("\n") + tmp + _T(": ") + strNewLocation;
				}
			}
			else if (dwRet != HTTP_STATUS_OK)
			{
				CString msg;
				msg.FormatMessage(IDS_HTTP_INVALID_URL_ERR, dwRet);
				outErrMsg += _T("\n") + msg + _T(": ") + m_URL;
				CloseFiles();
			}
			if (m_pFile)
			{
				char buffer[1025];
				UINT nChars;
				while (0 < (nChars = m_pFile->Read(buffer, sizeof(buffer)/sizeof(buffer[1])-1)))
				{
					buffer[nChars] = 0;
					m_Data += buffer;
				}
				bOk = true;
			}
			CloseFiles();
		}
		else
		{
			CString tmp;
			tmp.LoadString(IDS_HTTP_INVALID_URL);
			outErrMsg += _T("\n") + tmp + _T(": ") + m_URL;
		}
	}
	catch (CInternetException* ex)
	{
		m_Data.Empty();
		CString err;
		ex->GetErrorMessage(err.GetBuffer(1024), 1023);
		err.ReleaseBuffer();
		ex->Delete();
		outErrMsg += _T("\n") + err;
	}
	return bOk;
}
