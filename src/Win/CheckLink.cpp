/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Use CInternetSession to access files.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-20 DRC Ported to wxWidgets.
 * @li 2005-10-19 DRC Fixed a problem with CFile::GetStatus (see AgilityBook.cpp).
 * @li 2005-09-20 DRC Created, fixed a problem with GetStatus throwing.
 */

#include "stdafx.h"
#include "CheckLink.h"

#include "ReadHttp.h"
#include <wx/file.h>


bool CheckLink(
		wxString const& inLink,
		wxWindow* parent)
{
	bool bOk = false;
	if (0 < inLink.length())
	{
		// Check local file first
		bOk = wxFile::Access(inLink, wxFile::read);
		if (!bOk)
		{
			// Ok, check URL
			CReadHttp http(inLink, NULL);
			bOk = http.CheckHttpFile(parent);
		}
	}
	return bOk;
}
