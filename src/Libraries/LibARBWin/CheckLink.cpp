/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Use CInternetSession to access files.
 * @author David Connet
 *
 * Revision History
 * 2018-10-11 Moved to Win LibARBWin
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-20 Ported to wxWidgets.
 * 2005-10-19 Fixed a problem with CFile::GetStatus (see AgilityBook.cpp).
 * 2005-09-20 Created, fixed a problem with GetStatus throwing.
 */

#include "stdafx.h"
#include "LibARBWin/CheckLink.h"

#include "LibARBWin/ReadHttp.h"
#include <wx/file.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


bool CheckLink(std::wstring const& inLink, wxWindow* parent)
{
	bool bOk = false;
	if (0 < inLink.length())
	{
		// Check local file first
		bOk = wxFile::Access(inLink.c_str(), wxFile::read);
		if (!bOk)
		{
			// Ok, check URL
			bOk = ReadHttp::CheckHttpFile(inLink);
		}
	}
	return bOk;
}
