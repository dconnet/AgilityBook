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
 * This is a simple class to download data. Do not attempt multiple concurrent
 * transfers (curl usage must be changed for that).
 *
 * Revision History
 * 2021-04-27 Convert libcurl to wxWebRequest.
 * 2020-11-14 Convert to libcurl.
 * 2018-10-11 Moved to Win LibARBWin
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
#include "LibARBWin/ReadHttp.h"

#include "LibARBWin/DlgProgress.h"
#include <wx/creddlg.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


CReadHttp::CReadHttp()
	: m_pendingCancel(false)
	, m_currentRequest()
	, m_progress(nullptr)
	, m_downloadCount(0)
	, m_output(nullptr)
	, m_credentials()
{
}


CReadHttp::~CReadHttp()
{
	// Make sure we've canceled!
	CancelDownload();
	// We have to block until the web request completes, but we need to
	// process events while doing it.
	while (m_currentRequest.IsOk())
	{
		wxYield();
	}
}


bool CReadHttp::IsOk() const
{
	return m_currentRequest.IsOk();
}


bool CReadHttp::CancelDownload()
{
	if (!m_pendingCancel && m_currentRequest.IsOk())
	{
		m_pendingCancel = true;
		m_currentRequest.Cancel();
	}
	return m_pendingCancel;
}


CReadHttp::ReturnCode CReadHttp::DownloadFile(
	wxWindow* parent,
	wxString const& url,
	IDlgProgress* progress,
	wxOutputStream* output,
	OnFileComplete callback)
{
	// If closing, don't start another. And only allow 1 download at a time.
	if (m_pendingCancel || m_currentRequest.IsOk())
		return ReturnCode::Failed;
	if (!parent)
		return ReturnCode::Failed;
	if (!callback)
	{
		assert(0);
		return ReturnCode::Failed;
	}

	m_progress = progress;
	m_output = output;
	m_currentRequest = wxWebSession::GetDefault().CreateRequest(parent, url);
	m_currentRequest.SetStorage(wxWebRequest::Storage_None);
	m_downloadCount = 0;

	parent->Bind(wxEVT_WEBREQUEST_DATA, [this](wxWebRequestEvent& evt) {
		if (m_progress && m_progress->HasCanceled())
		{
			CancelDownload();
			return;
		}
		auto size = evt.GetDataSize();
		m_downloadCount += size;
		m_output->Write(evt.GetDataBuffer(), size);
		if (m_progress)
		{
			// Really not worried about overflow. We don't download really big things!
			m_progress->SetPos(1, static_cast<int>(m_downloadCount));
		}
	});

	parent->Bind(wxEVT_WEBREQUEST_STATE, [this, parent, callback](wxWebRequestEvent& evt) {
		bool stillActive = false;
		switch (evt.GetState())
		{
		case wxWebRequest::State_Completed:
			break;
		case wxWebRequest::State_Failed:
			// wxLogError("Web Request failed: %s", evt.GetErrorDescription());
			break;
		case wxWebRequest::State_Cancelled:
			if (m_pendingCancel)
				m_pendingCancel = false;
			// wxLogStatus("Cancelled");
			break;
		case wxWebRequest::State_Unauthorized:
		{
			wxWebAuthChallenge auth = m_currentRequest.GetAuthChallenge();
			if (!auth.IsOk())
			{
				// wxLogStatus("Unexpectedly missing auth challenge");
				break;
			}

			wxCredentialEntryDialog dialog(
				parent,
				wxString::Format(L"%s\n%s", _("Please enter credentials for accessing:"), evt.GetResponse().GetURL()),
				_("Credentials"),
				m_credentials);
			if (dialog.ShowModal() == wxID_OK)
			{
				m_credentials = dialog.GetCredentials();
				auth.SetCredentials(m_credentials);
				// wxLogStatus("Trying to authenticate...");
				stillActive = true;
			}
		}
		break;
		case wxWebRequest::State_Active:
			stillActive = true;
			break;
		case wxWebRequest::State_Idle:
			break;
		}
		if (!stillActive)
		{
			m_currentRequest = wxWebRequest();
			m_progress = nullptr;
			m_downloadCount = 0;
			m_output = nullptr;
			callback(evt.GetState());
		}
	});

	m_currentRequest.Start();
	return CReadHttp::ReturnCode::Busy;
}
