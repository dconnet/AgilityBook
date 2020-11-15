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
 * 2020-11-14 Convert to libcurl.
 * 2018-10-11 Moved to Win LibARBWin
 * 2010-02-08 Added new interfaces to support streaming to a file.
 * 2009-07-19 Changed from wxHTTP to wxURL to handle proxies.
 * 2009-01-06 Ported to wxWidgets.
 * 2007-08-03 Separated HTTP reading code from UpdateInfo.cpp
 */

#include "LibwxARBWin.h"

class IDlgProgress;
class wxOutputStream;


namespace ReadHttp
{
bool GotoURL(std::wstring const& inLink);
bool CheckHttpFile(std::wstring const& inURL);
bool ReadHttpFileSync(std::wstring& outErrMsg, std::wstring const& inURL, std::string& outString);
bool ReadHttpFileSync(std::wstring& outErrMsg, std::wstring const& inURL, wxString& outString);
bool ReadHttpFileSync(
	std::wstring& outErrMsg,
	std::wstring const& inURL,
	wxOutputStream* outStream,
	IDlgProgress* pProgress = nullptr);
};
