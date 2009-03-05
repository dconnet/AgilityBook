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
 * @li 2009-01-06 DRC Ported to wxWidgets.
 * @li 2008-01-01 DRC Fix closing the connection - Close didn't reset null.
 * @li 2007-08-03 DRC Separated HTTP reading code from UpdateInfo.cpp
 */

#include "stdafx.h"
#include "ReadHttp.h"

#include "DlgAuthenticate.h"
#include <wx/sstream.h>


CReadHttp::CReadHttp(
		wxString const& inURL,
		std::string* outData)
	: m_Valid(false)
	, m_URL(inURL)
	, m_Protocol()
	, m_Host()
	, m_Request()
	, m_Data(outData)
{
	wxString url(inURL);
	int pos = url.Find(wxT(':'));
	if (wxNOT_FOUND != pos)
	{
		m_Protocol = url(0, pos);
		if (pos + 2 < (int)url.length() && url[pos+1] == wxT('/') && url[pos+2] == wxT('/'))
		{
			url = url(pos + 3, url.length() - 2);
			pos = url.Find(wxT('/'));
			m_Host = url;
			m_Request = wxT("/");
			pos = m_Host.Find(wxT('/'));
			if (wxNOT_FOUND != pos)
			{
				m_Request = m_Host(pos, m_Host.length());
				m_Host = m_Host(0, pos);
			}
			m_Valid = true;
		}
	}
}


bool CReadHttp::ReadHttpFile(
		wxString& userName,
		wxString& outErrMsg,
		wxWindow* pParent,
		bool bCheckOnly)
{
	if (m_Data)
		m_Data->erase();
	if (!m_Valid)
	{
		outErrMsg = _("IDS_HTTP_INVALID_URL");
		outErrMsg += wxT(": ");
		outErrMsg += m_URL;
		return false;
	}
	outErrMsg.Empty();
	wxBusyCursor wait;

	wxHTTP http;
	http.SetHeader(wxT("Content-type"), wxT("text/html; charset=utf-8"));
	http.SetTimeout(10); // 10 seconds of timeout instead of 10 minutes ...

	// This will wait until the user connects to the internet. It is
	// important in case of dialup (or ADSL) connections
	int nTrys = 0;
	while (!http.Connect(m_Host))
	{
		if (++nTrys > 5)
		{
#pragma message PRAGMA_MESSAGE("TODO: TranslateError message")
			outErrMsg = wxT("Time out");
			return false;
		}
		wxSleep(5);
	}

	wxApp::IsMainLoopRunning(); // should return true

	wxInputStream* stream = http.GetInputStream(m_Request);
	while (wxPROTO_NOERR != http.GetError())
	{
		if (stream)
			delete stream;
		http.Close();
		if (bCheckOnly)
			return false;
		CDlgAuthenticate dlg(userName, pParent);
		if (wxID_OK == dlg.ShowModal())
		{
			http.SetUser(dlg.GetUserName());
			http.SetPassword(dlg.GetPassword());
		}
		else
			return false;
		while (!http.Connect(m_Host))
			wxSleep(5);
		stream = http.GetInputStream(m_Request);
	}

	wxString res;
	if (m_Data)
	{
		wxStringOutputStream outStream(&res);
		stream->Read(outStream);
	}
	delete stream;
	http.Close();
	if (m_Data)
		*m_Data = res.mb_str(wxMBConvUTF8());

	return true;
}


bool CReadHttp::CheckHttpFile(wxWindow* pParent)
{
	wxString userName, outErrMsg;
	return ReadHttpFile(userName, outErrMsg, pParent, true);
}
