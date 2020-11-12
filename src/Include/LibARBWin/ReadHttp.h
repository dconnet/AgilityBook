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
 * 2018-10-11 Moved to Win LibARBWin
 * 2010-02-08 Added new interfaces to support streaming to a file.
 * 2009-07-19 Changed from wxHTTP to wxURL to handle proxies.
 * 2009-01-06 Ported to wxWidgets.
 * 2007-08-03 Separated HTTP reading code from UpdateInfo.cpp
 */

#include "LibwxARBWin.h"

#include "ARBCommon/StringUtil.h"
class IDlgProgress;
class wxOutputStream;
class wxURL;


inline bool GotoURL(std::wstring const& inLink)
{
	return wxLaunchDefaultBrowser(inLink.c_str());
}


class ARBWIN_API CReadHttp
{
	DECLARE_NO_COPY_IMPLEMENTED(CReadHttp)
public:
	CReadHttp();

	bool CheckHttpFile(std::wstring const& inURL)
	{
		std::wstring outErrMsg;
		return ReadHttpFile(outErrMsg, nullptr, wxID_ANY, inURL, nullptr, nullptr, nullptr);
	}

	bool ReadHttpFileSync(std::wstring& outErrMsg, std::wstring const& inURL, std::string& outString)
	{
		wxString data;
		bool rc = ReadHttpFile(outErrMsg, nullptr, wxID_ANY, inURL, &data, nullptr, nullptr);
		if (rc)
			outString = data.mb_str(wxMBConvUTF8());
		return rc;
	}

	bool ReadHttpFileSync(std::wstring& outErrMsg, std::wstring const& inURL, wxString& outString)
	{
		return ReadHttpFile(outErrMsg, nullptr, wxID_ANY, inURL, &outString, nullptr, nullptr);
	}

	bool ReadHttpFileSync(
		std::wstring& outErrMsg,
		std::wstring const& inURL,
		wxOutputStream* outStream,
		IDlgProgress* pProgress = nullptr)
	{
		if (!outStream)
			return false;
		return ReadHttpFile(outErrMsg, nullptr, wxID_ANY, inURL, nullptr, outStream, pProgress);
	}

	bool ReadHttpFile(
		std::wstring& outErrMsg,
		wxEvtHandler* pParent,
		int idEvent,
		std::wstring const& inURL,
		wxOutputStream* outStream = nullptr, // If null, return via string in wxThreadEvent
		IDlgProgress* pProgress = nullptr)
	{
		return ReadHttpFile(outErrMsg, pParent, idEvent, inURL, nullptr, outStream, pProgress);
	}

private:
	bool ReadHttpFile(
		std::wstring& outErrMsg,
		wxEvtHandler* pParent,
		int idEvent,
		std::wstring const& inURL,
		wxString* outString = nullptr, // Only one of these 2 may be specified (except on check)
		wxOutputStream* outStream = nullptr,
		IDlgProgress* pProgress = nullptr);
};
