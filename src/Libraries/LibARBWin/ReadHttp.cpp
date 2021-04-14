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

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgAuthenticate.h"
#include "LibARBWin/DlgProgress.h"
#include "fmt/format.h"
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <curl/curl.h>

#if LIBCURL_VERSION_NUM < 0x072000
#error Need a newer curl
#endif

#if defined(__WXMSW__)
#if defined(_DEBUG)
#pragma comment(lib, "libcurl_a_debug.lib")
#else
#pragma comment(lib, "libcurl_a.lib")
#endif
#endif

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


namespace
{
int xferinfo(void* ptr, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
	IDlgProgress* pProgress = static_cast<IDlgProgress*>(ptr);
	if (pProgress)
	{
		if (pProgress->HasCanceled())
			return 1;
		pProgress->SetPos(1, static_cast<int>(dlnow));
	}
	return 0;
}


size_t write_data_stream(void* pData, size_t size, size_t nmemb, void* stream)
{
	// If no stream, pretend it worked. We're just checking for existence.
	if (stream)
	{
		wxOutputStream* pStream = static_cast<wxOutputStream*>(stream);
		pStream->Write(pData, size * nmemb);
	}
	return size * nmemb;
}


long ReadHttpFile(
	std::wstring& outErrMsg,
	std::wstring const& inURL,
	wxString* outString, // Only one of these 2 may be specified
	wxOutputStream* outStream,
	IDlgProgress* pProgress)
{
	if (!outString && !outStream)
		return 200;

	wxString res;
	wxStringOutputStream out(&res);

	bool bCancelEnable = true;

	char errorBuffer[CURL_ERROR_SIZE] = {0};
	CURL* curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL, StringUtil::stringA(inURL).c_str());

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_stream);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
	if (outString)
	{
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo);
		curl_easy_setopt(curl, CURLOPT_XFERINFODATA, pProgress);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, outStream);
	}

	long code = 400; // Just default to "Bad Request"
	std::string errMsg;

	CURLcode rcCurl = curl_easy_perform(curl);
	if (CURLE_OK == rcCurl)
		curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &code);
	else
		errMsg = errorBuffer;

	curl_easy_cleanup(curl);

	if (pProgress)
		pProgress->EnableCancel(bCancelEnable);

	if (CURLE_URL_MALFORMAT == rcCurl)
	{
		fmt::wmemory_buffer buffer;
		fmt::format_to(buffer, L"{}: {}\n{}", _("Invalid URL").wx_str(), inURL, StringUtil::stringW(errMsg));
		outErrMsg += fmt::to_string(buffer);
	}
	else if (CURLE_OK != rcCurl)
	{
		fmt::wmemory_buffer buffer;
		fmt::format_to(
			buffer,
			L"{}\n{}",
			fmt::format(_("Error ({0}) reading {1}").wx_str(), rcCurl, inURL),
			StringUtil::stringW(errMsg));
		outErrMsg += fmt::to_string(buffer);
	}
	else if (404 == code)
	{
		outErrMsg += fmt::format(_("File not found: {}").wx_str(), inURL);
	}

	if (CURLE_OK == rcCurl)
	{
		if (200 == code && outString)
		{
			*outString = res.mb_str(wxMBConvUTF8());
		}
	}

	return code;
}
} // namespace


namespace ReadHttp
{
bool CheckHttpFile(std::wstring const& inURL)
{
	CURL* curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL, StringUtil::stringA(inURL).c_str());
	curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

	long code = 0;
	if (CURLE_OK == curl_easy_perform(curl))
	{
		curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &code);
	}

	curl_easy_cleanup(curl);

	return 200 == code;
}


long ReadHttpFileSync(std::wstring& outErrMsg, std::wstring const& inURL, std::string& outString)
{
	wxString data;
	long rc = ReadHttpFile(outErrMsg, inURL, &data, nullptr, nullptr);
	if (200 == rc)
		outString = data.mb_str(wxMBConvUTF8());
	return rc;
}


long ReadHttpFileSync(std::wstring& outErrMsg, std::wstring const& inURL, wxString& outString)
{
	return ReadHttpFile(outErrMsg, inURL, &outString, nullptr, nullptr);
}


long ReadHttpFileSync(
	std::wstring& outErrMsg,
	std::wstring const& inURL,
	wxOutputStream* outStream,
	IDlgProgress* pProgress)
{
	if (!outStream)
		return 400;
	return ReadHttpFile(outErrMsg, inURL, nullptr, outStream, pProgress);
}

} // namespace ReadHttp
