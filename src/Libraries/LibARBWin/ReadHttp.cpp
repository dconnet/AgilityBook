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

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgAuthenticate.h"
#include "LibARBWin/DlgProgress.h"
#include <wx/sstream.h>
#include <wx/thread.h>
#include <wx/url.h>
#include <wx/wfstream.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


namespace
{
	constexpr size_t k_defaultBuffer = 4096;
}

class CReadHttpThread : public wxThread
{
	CReadHttpThread(
			wxEvtHandler* pParent,
			int id,
			CReadHttp* reader,
			wxInputStream* urlStream,
			wxOutputStream* outStream,
			bool hasProgress,
			size_t bufferSize);

public:
	// Async
	static CReadHttpThread* Start(
			wxEvtHandler* pParent,
			int id,
			CReadHttp* reader,
			wxInputStream* urlStream,
			wxOutputStream* outStream,
			bool hasProgress,
			size_t bufferSize = k_defaultBuffer);
	// Sync
	static bool Start(
			CReadHttp* reader,
			wxInputStream* urlStream,
			wxString* outString,
			wxOutputStream* outStream,
			size_t bufferSize = k_defaultBuffer);

	ExitCode Entry() override;

	// Called before action in context of calling thread.
	//void OnDelete() override;

	// Since the calling thread knows what it's doing, we don't need to reflect status back up.
	//void OnKill() override;

	// Called when thread exits (normally or via Delete())
	void OnExit() override;

private:
	bool ReadData(
			wxString& outErrMsg,
			wxString* output);

	wxEvtHandler* m_pParent; // Where event is sent
	int m_id; // ID to use when sending event
	CReadHttp* m_reader; // wxThread* 
	wxInputStream* m_urlStream; // The http data stream, takes possession
	wxOutputStream* m_outStream; // Where to write data (nullptr means pass back via string)
	bool m_hasProgress; // Are we supplying read progress (bytes read)?
	size_t m_bufferSize;
};


CReadHttpThread::CReadHttpThread(
		wxEvtHandler* pParent,
		int id,
		CReadHttp* reader,
		wxInputStream* urlStream,
		wxOutputStream* outStream,
		bool hasProgress,
		size_t bufferSize)
	: m_pParent(pParent)
	, m_id(id)
	, m_reader(reader)
	, m_urlStream(urlStream)
	, m_outStream(outStream)
	, m_hasProgress(hasProgress)
	, m_bufferSize(bufferSize == 0 ? k_defaultBuffer : bufferSize)
{
	// We checked the url before creating this, so assuming it's still good!
}


CReadHttpThread* CReadHttpThread::Start(
		wxEvtHandler* pParent,
		int id,
		CReadHttp* reader,
		wxInputStream* httpStream,
		wxOutputStream* outStream,
		bool hasProgress,
		size_t bufferSize)
{
	CReadHttpThread* thread = new CReadHttpThread(pParent, id, reader, httpStream, outStream, hasProgress, bufferSize);
	if (thread->Create() != wxTHREAD_NO_ERROR || thread->Run() != wxTHREAD_NO_ERROR)
	{
		delete thread;
		thread = nullptr;
	}
	return thread;
}


bool CReadHttpThread::Start(
		CReadHttp* reader,
		wxInputStream* urlStream,
		wxString* outString,
		wxOutputStream* outStream,
		size_t bufferSize)
{
	CReadHttpThread notThread(nullptr, wxID_ANY, reader, urlStream, outStream, false, bufferSize);
	wxString outErrMsg;
	return notThread.ReadData(outErrMsg, outString);
}


wxThread::ExitCode CReadHttpThread::Entry()
{
	// On success, use SetString to return data if a stream was not specificed.
	auto eventDone = new wxThreadEvent(wxEVT_THREAD, m_id);

	wxString outErrMsg, res;
	if (ReadData(outErrMsg, &res))
	{
		eventDone->SetInt(static_cast<int>(CReadHttp::Status::Success));
		if (!m_outStream)
			eventDone->SetString(res);
	}
	else
	{
		eventDone->SetInt(static_cast<int>(CReadHttp::Status::Error));
		eventDone->SetString(outErrMsg);
	}
	wxQueueEvent(m_pParent, eventDone);

	return 0;
}


void CReadHttpThread::OnExit()
{
	{
		wxCriticalSectionLocker locker(m_reader->m_critsect);
		m_reader->m_thread = nullptr;
	}

	// We could sent a final event here. But not needed since we send a
	// success/fail at the end of Entry().
}


bool CReadHttpThread::ReadData(wxString& outErrMsg, wxString* output)
{
	bool bOk = false;

	assert(m_outStream || output);

	if (m_urlStream)
	{
		bOk = true;
		if (!m_outStream)
		{
			wxStringOutputStream outStream(output);
			m_urlStream->Read(outStream);
		}
		else
		{
			if (m_hasProgress)
			{
				auto buffer = std::make_unique<unsigned char>(m_bufferSize);
				while (!m_urlStream->Eof())
				{
					if (TestDestroy())
					{
						bOk = false;
						break;
					}
					m_urlStream->Read(buffer.get(), m_bufferSize);
					size_t read = m_urlStream->LastRead();
					if (0 < read)
					{
						auto eventProgress = new wxThreadEvent(wxEVT_THREAD, m_id);
						eventProgress->SetInt(static_cast<int>(CReadHttp::Status::Read));
						eventProgress->SetExtraLong(read);
						wxQueueEvent(m_pParent, eventProgress);
						m_outStream->Write(buffer.get(), read);
					}
				}
			}
			else
			{
				m_urlStream->Read(*m_outStream);
			}
		}
		if (m_urlStream->GetLastError() == wxSTREAM_READ_ERROR)
		{
			outErrMsg << L"Error reading stream";// << m_URL.GetURL();
		}
		delete m_urlStream;
		m_urlStream = nullptr;
	}

	return bOk;
}

/////////////////////////////////////////////////////////////////////////////

CReadHttp::CReadHttp()
	: m_thread(nullptr)
{
}


bool CReadHttp::Exit()
{
	CReadHttpThread* thread = nullptr;
	{
		wxCriticalSectionLocker locker(m_critsect);
		thread = m_thread;
		// CReadHttpThread::OnExit will also null it if we call Delete().
		m_thread = nullptr;
	}
	if (thread)
		thread->Delete();
	return !!thread;
}


bool CReadHttp::ReadHttpFile(
		std::wstring& outErrMsg,
		wxEvtHandler* pParent,
		int idEvent,
		std::wstring const& inURL,
		wxString* outString, // Only one of these 2 may be specified (except on check)
		wxOutputStream* outStream,
		IDlgProgress* pProgress)
{
	wxURL url(StringUtil::stringWX(inURL));
	if (!url.IsOk())
	{
		outErrMsg = _("Invalid URL");
		outErrMsg += L": ";
		outErrMsg += inURL;
		return false;
	}

	wxInputStream* urlStream = url.GetInputStream();
	if (!urlStream || !urlStream->IsOk())
	{
		/* this doesn't work. suggestion is to use wxCURL
		 * for now, we just don't support authentication
		CDlgAuthenticate dlg(userName, pParent);
		if (wxID_OK == dlg.ShowModal())
		{
			url.GetProtocol().SetUser(dlg.GetUserName());
			url.GetProtocol().SetPassword(dlg.GetPassword());
			urlStream = url.GetInputStream();
		}
		if (!urlStream || !urlStream->IsOk())
		*/
		outErrMsg += L"Error reading ";
		outErrMsg += inURL;
		delete urlStream;
		return false;
	}
	// In theory, this could be a check with a parent - but the wrapper functions prevent that.
	if (!outString && !outStream && !pParent)
	{
		delete urlStream;
		return true;
	}

	// If sync...
	if (!pParent)
	{
		return CReadHttpThread::Start(this, urlStream, outString, outStream);
	}

	wxCriticalSectionLocker locker(m_critsect);

	if (m_thread)
		return false;

	m_thread = CReadHttpThread::Start(pParent, idEvent, this, urlStream, outStream, !!pProgress);
	return !!m_thread;
}
