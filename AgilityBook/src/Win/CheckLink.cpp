/*
 * Copyright © 2005 David Connet. All Rights Reserved.
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
 * @brief Use CInternetSession to access files.
 * @author David Connet
 *
 * Revision History
 * @li 2005-09-20 DRC Created
 */

#include "stdafx.h"
#include <afxinet.h>
#include "AgilityBook.h"

#include "CheckLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
	
bool CheckLink(
		CInternetSession& inSession,
		std::string const& inLink)
{
	bool bOk = false;
	if (0 < inLink.length())
	{
		try
		{
			// Source for OpenURL is located in 'atlmfc/src/mfc/inet.cpp' 
			CStdioFile* pFile = inSession.OpenURL(inLink.c_str(), 1, INTERNET_FLAG_TRANSFER_BINARY);
			if (pFile)
			{
				bOk = true;
				pFile->Close();
				delete pFile;
			}
		}
		catch (CFileException* ex)
		{
			ex->Delete();
			// If the session threw, try normal file access apis...
			CFileStatus status;
			if (CFile::GetStatus(inLink.c_str(), status))
				bOk = true;
		}
		catch (CInternetException* ex)
		{
			// I'm not sure how to tell if the url is bad or
			// the connection doesn't exist...
			ex->Delete();
			bOk = false;
		}
	}
	return bOk;
}
