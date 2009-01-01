#pragma once

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
 * @li 2007-08-03 DRC Separated HTTP reading code from UpdateInfo.cpp
 */

#include <afxinet.h>
class CInternetSession;
class CHttpConnection;
class CHttpFile;

/**
 * Read data from the internet
 */
class CReadHttp
{
public:
	/**
	 * Construction
	 * @param inURL URL to read data from.
	 * @param outData Returned data, in raw ascii form, not unicode (makes XML parsing easier)
	 */
	CReadHttp(CString const& inURL, CStringA& outData);
	~CReadHttp();

	/**
	 * Close everything.
	 */
	void Close();

	/**
	 * Read the HTTP via a 'GET'
	 * @param userName Default username, if required. May be modified.
	 * @param outErrMsg Generated error messages
	 * @param pParent Owner of any dialogs that may be displayed
	 * @return Success
	 */
	bool ReadHttpFile(CString& userName, CString& outErrMsg, CWnd* pParent = NULL);

private:
	/**
	 * Close the connection and file, not session
	 */
	void CloseFiles();

	/**
	 * Create the session, connection and file.
	 * @param userName Default username, if required. May be modified.
	 * @param outErrMsg Generated error messages
	 * @param pParent Owner of any dialogs that may be displayed
	 */
	DWORD Connect(CString& userName, CString& outErrMsg, CWnd* pParent);

	CString m_URL; // Note, this cannot be a reference - otherwise CReadHttp("http://www", data) fails as the reference goes away.
	CStringA& m_Data;
	CInternetSession* m_session;
	CHttpConnection* m_pServer;
	CHttpFile* m_pFile;
	CString m_strServerName;
	CString m_strObject;
	INTERNET_PORT m_nPort;
};
