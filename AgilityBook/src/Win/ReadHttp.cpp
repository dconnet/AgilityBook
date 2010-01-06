/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
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
