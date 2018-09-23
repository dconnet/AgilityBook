/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Read HTTP from the net.
 * @author David Connet
 *
 * Revision History
 * 2014-02-19 Added some error logging.
 * 2010-04-19 Calling Yield (for cancel dlg) results in corrupted downloads.
 * 2010-03-05 Workaround a memory leak in wxURL with proxies.
 * 2010-02-08 Added new interfaces to support streaming to a file.
 * 2009-07-19 Changed from wxHTTP to wxURL to handle proxies.
 * 2009-01-06 Ported to wxWidgets.
 * 2008-01-01 Fix closing the connection - Close didn't reset null.
 * 2007-08-03 Separated HTTP reading code from UpdateInfo.cpp
 */

#include "stdafx.h"
#include "ReadHttp.h"

#include "DlgAuthenticate.h"
#include "DlgProgress.h"

#include "ARBCommon/StringUtil.h"
#include <wx/sstream.h>
#include <wx/url.h>
#include <wx/wfstream.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


CReadHttp::CReadHttp(
		std::wstring const& inURL,
		std::string* outData)
	: m_address(inURL)
	, m_URL(nullptr)
	, m_Data(outData)
	, m_Stream(nullptr)
	, m_pProgress(nullptr)
{
	m_URL = new wxURL(StringUtil::stringWX(inURL));
}


CReadHttp::CReadHttp(
		std::wstring const& inURL,
		wxOutputStream& outStream,
		IDlgProgress* pProgress)
	: m_address(inURL)
	, m_URL(nullptr)
	, m_Data(nullptr)
	, m_Stream(&outStream)
	, m_pProgress(pProgress)
{
	m_URL = new wxURL(StringUtil::stringWX(inURL));
}


CReadHttp::~CReadHttp()
{
	delete m_URL;
}


bool CReadHttp::ReadHttpFile(
		std::wstring& userName,
		std::wstring& outErrMsg,
		wxWindow* pParent,
		bool bCheckOnly)
{
	if (m_Data)
		m_Data->erase();
	if (!m_URL || !m_URL->IsOk())
	{
		outErrMsg = _("Invalid URL");
		outErrMsg += L": ";
		outErrMsg += m_address;
		return false;
	}
	outErrMsg.clear();
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
		{
			outErrMsg += L"Error reading ";
			outErrMsg += m_address;
			delete stream;
			return false;
		}
	}

	bool bOk = true;
	wxString res;
	if (m_Data)
	{
		wxStringOutputStream outStream(&res);
		stream->Read(outStream);
	}
	else if (m_Stream)
	{
		if (m_pProgress)
		{
			bool bCancelEnable = m_pProgress->EnableCancel(false);
			unsigned char buffer[4096];
			while (!stream->Eof())
			{
				stream->Read(buffer, sizeof(buffer));
				size_t read = stream->LastRead();
				if (0 < read)
				{
					m_pProgress->OffsetPos(1, static_cast<int>(read));
					m_Stream->Write(buffer, read);
				}
			}
			m_pProgress->EnableCancel(bCancelEnable);
		}
		else
		{
			stream->Read(*m_Stream);
		}
	}
	if (stream->GetLastError() == wxSTREAM_READ_ERROR)
	{
		outErrMsg += L"Error reading ";
		outErrMsg += m_address;
	}
	delete stream;
	if (m_Data)
		*m_Data = res.mb_str(wxMBConvUTF8());

	return bOk;
}


bool CReadHttp::ReadHttpFile(
		std::wstring& outErrMsg,
		wxWindow* pParent,
		bool bCheckOnly)
{
	std::wstring username;
	return ReadHttpFile(username, outErrMsg, pParent, bCheckOnly);
}


bool CReadHttp::CheckHttpFile(wxWindow* pParent)
{
	std::wstring userName, outErrMsg;
	return ReadHttpFile(userName, outErrMsg, pParent, true);
}
