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
 * @li 2009-07-19 DRC Changed from wxHTTP to wxURL to handle proxies.
 * @li 2009-01-06 DRC Ported to wxWidgets.
 * @li 2008-01-01 DRC Fix closing the connection - Close didn't reset null.
 * @li 2007-08-03 DRC Separated HTTP reading code from UpdateInfo.cpp
 */

#include "stdafx.h"
#include "ReadHttp.h"

#include "DlgAuthenticate.h"
#include <wx/sstream.h>
#include <wx/url.h>


CReadHttp::CReadHttp(
		wxString const& inURL,
		std::string* outData)
	: m_address(inURL)
	, m_URL(new wxURL(inURL))
	, m_Data(outData)
{
}


CReadHttp::~CReadHttp()
{
	delete m_URL;
}


bool CReadHttp::ReadHttpFile(
		wxString& userName,
		wxString& outErrMsg,
		wxWindow* pParent,
		bool bCheckOnly)
{
	if (m_Data)
		m_Data->erase();
	if (!m_URL || !m_URL->IsOk())
	{
		outErrMsg = _("IDS_HTTP_INVALID_URL");
		outErrMsg += wxT(": ");
		outErrMsg += m_address;
		return false;
	}
	outErrMsg.Empty();
	wxBusyCursor wait;

	wxInputStream* stream = m_URL->GetInputStream();
	if (!stream || !stream->IsOk())
	{
		/* this doesn't work. suggestion is to use wxCURL
		 * for now, we just don't support authentication
		CDlgAuthenticate dlg(userName, pParent);
		if (wxID_OK == dlg.ShowModal())
		{
			m_URL->GetProtocol().SetUser(dlg.GetUserName());
			m_URL->GetProtocol().SetPassword(dlg.GetPassword());
			stream = m_URL->GetInputStream();
		}
		if (!stream || !stream->IsOk())
		*/
			return false;
	}

	wxString res;
	if (m_Data)
	{
		wxStringOutputStream outStream(&res);
		stream->Read(outStream);
	}
	delete stream;
	if (m_Data)
		*m_Data = res.mb_str(wxMBConvUTF8());

	return true;
}


bool CReadHttp::CheckHttpFile(wxWindow* pParent)
{
	wxString userName, outErrMsg;
	return ReadHttpFile(userName, outErrMsg, pParent, true);
}
