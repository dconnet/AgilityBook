#pragma once

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
 * @li 2007-08-03 DRC Separated HTTP reading code from UpdateInfo.cpp
 */

#include "ARBString.h"
class wxURL;

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
			wxString const& inURL,
			std::string* outData);

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
			wxString& userName,
			wxString& outErrMsg,
			wxWindow* pParent = NULL,
			bool bCheckOnly = false);

	bool CheckHttpFile(wxWindow* pParent = NULL);

private:
	wxString m_address;
	wxURL* m_URL;
	std::string* m_Data;
};
