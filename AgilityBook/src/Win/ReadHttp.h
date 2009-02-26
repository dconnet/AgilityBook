#pragma once

/*
 * Copyright © 2004-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 *
 * @brief Read HTTP from the net.
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-06 DRC Ported to wxWidgets.
 * @li 2007-08-03 DRC Separated HTTP reading code from UpdateInfo.cpp
 */

#include "ARBString.h"
#include <wx/protocol/http.h>


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
	bool m_Valid;
	wxString m_URL;
	wxString m_Protocol;
	wxString m_Host;
	wxString m_Request;
	std::string* m_Data;
};
