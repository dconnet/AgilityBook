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
 * 2010-02-08 Added new interfaces to support streaming to a file.
 * 2009-07-19 Changed from wxHTTP to wxURL to handle proxies.
 * 2009-01-06 Ported to wxWidgets.
 * 2007-08-03 Separated HTTP reading code from UpdateInfo.cpp
 */

#include "ARBCommon/StringUtil.h"
class IDlgProgress;
class wxOutputStream;
class wxURL;


inline bool GotoURL(std::wstring const& inLink)
{
	return wxLaunchDefaultBrowser(inLink.c_str());
}


/**
 * Read data from the internet
 */
class CReadHttp
{
public:
	/**
	 * Construction
	 * @param inURL URL to read data from.
	 * @param outData Returned data, in raw ascii form, not unicode (makes XML parsing easier)
	 * @note If outData is NULL, ReadHttpFile will only check for connectivity.
	 */
	CReadHttp(
			std::wstring const& inURL,
			std::string* outData);
	CReadHttp(
			std::wstring const& inURL,
			wxOutputStream& outStream,
			IDlgProgress* pProgress = NULL);

	~CReadHttp();

	/**
	 * Read the HTTP via a 'GET'
	 * @param userName Default username, if required. May be modified.
	 * @param outErrMsg Generated error messages
	 * @param pParent Owner of any dialogs that may be displayed
	 * @param bCheckOnly Only check connection, do not read, do not prompt for password
	 * @return Success
	 */
	bool ReadHttpFile(
			std::wstring& userName,
			std::wstring& outErrMsg,
			wxWindow* pParent = NULL,
			bool bCheckOnly = false);

	bool ReadHttpFile(
			std::wstring& outErrMsg,
			wxWindow* pParent = NULL,
			bool bCheckOnly = false);

	bool CheckHttpFile(wxWindow* pParent = NULL);

private:
	std::wstring m_address;
	wxURL* m_URL;
	std::string* m_Data;
	wxOutputStream* m_Stream;
	IDlgProgress* m_pProgress;
};
