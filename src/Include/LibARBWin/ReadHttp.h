#pragma once

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
 * 2021-04-27 Convert libcurl to wxWebRequest.
 * 2020-11-14 Convert to libcurl.
 * 2018-10-11 Moved to Win LibARBWin
 * 2010-02-08 Added new interfaces to support streaming to a file.
 * 2009-07-19 Changed from wxHTTP to wxURL to handle proxies.
 * 2009-01-06 Ported to wxWidgets.
 * 2007-08-03 Separated HTTP reading code from UpdateInfo.cpp
 */

#include "LibwxARBWin.h"
#include <wx/webrequest.h>

class IDlgProgress;
class wxOutputStream;


class ARBWIN_API CReadHttp
{
public:
	enum class ReturnCode
	{
		Busy,    // Cannot determine, try later
		Failed,  // Call failed
		Success, // Call succeeded
	};

	/**
	 * @param state Download was successful
	 * @param filename If 'state' is Success, name of downloaded file.
	 */
	using OnFileComplete = std::function<void(wxWebRequest::State state)>;

	CReadHttp();
	~CReadHttp();

	bool IsOk() const;
	bool CancelDownload();

	/**
	 * @param parent Window to bind messages to.
	 * @param url File to download
	 * @param progress Progress dialog (may be null)
	 * @param output Where to write data
	 * @param callback Callback for completion
	 * Returns Busy or Failed
	 */
	ReturnCode DownloadFile(
		wxWindow* parent,
		wxString const& url,
		IDlgProgress* progress,
		wxOutputStream* output,
		OnFileComplete callback);

private:
	bool m_pendingCancel;
	wxWebRequest m_currentRequest;
	IDlgProgress* m_progress;
	size_t m_downloadCount;
	wxOutputStream* m_output;

	// It would be better to use wxSecretStore, but this is a generic interface
	// to download a file. Picking a service name for the store is a little more
	// difficult. So we'll just hold them in memory here.
	wxWebCredentials m_credentials;
};
