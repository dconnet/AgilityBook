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
#include <wx/url.h>
#include <wx/wfstream.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


namespace
{
constexpr size_t k_defaultBuffer = 4096;
}


CReadHttp::CReadHttp()
{
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
		{
			outErrMsg += L"Error reading ";
			outErrMsg += inURL;
			delete urlStream;
			return false;
		}
	}
	// In theory, this could be a check with a parent - but the wrapper functions prevent that.
	if (!outString && !outStream && !pParent)
	{
		delete urlStream;
		return true;
	}

	bool bOk = true;
	if (outString)
	{
		wxString res;
		wxStringOutputStream out(&res);
		urlStream->Read(out);
		*outString = res.mb_str(wxMBConvUTF8());
	}
	else if (outStream)
	{
		if (pProgress)
		{
			bool bCancelEnable = pProgress->EnableCancel(false);
			unsigned char buffer[k_defaultBuffer] = {0};
			while (!urlStream->Eof())
			{
				urlStream->Read(buffer, sizeof(buffer));
				size_t read = urlStream->LastRead();
				if (0 < read)
				{
					pProgress->OffsetPos(1, static_cast<int>(read));
					outStream->Write(buffer, read);
				}
			}
			pProgress->EnableCancel(bCancelEnable);
		}
		else
		{
			urlStream->Read(*outStream);
		}
	}
	if (urlStream->GetLastError() == wxSTREAM_READ_ERROR)
	{
		outErrMsg += L"Error reading ";
		outErrMsg += inURL;
	}
	delete urlStream;

	return bOk;
}
