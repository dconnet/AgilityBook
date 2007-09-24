/*
 * Copyright © 2004-2007 David Connet. All Rights Reserved.
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
 * @brief implementation of the CVersionNum class
 * @author David Connet
 *
 * Revision History
 * @li 2006-12-10 DRC Fixed operator< and operator>.
 * @li 2004-03-04 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"

#include "VersionNum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CVersionNum::CVersionNum(
		HMODULE inModule,
		WORD inwLangID,
		WORD inwCharSet)
	: m_Valid(false)
{
	m_Version.part1 = m_Version.part2 = m_Version.part3 = m_Version.part4 = 0;

	// Get the filename and other info...
	LPTSTR tp = m_FileName.GetBuffer(MAX_PATH);
	::GetModuleFileName(inModule, tp, MAX_PATH);
	m_FileName.ReleaseBuffer();
#ifndef _UNICODE
	m_FileName.OemToAnsi();
#endif

	// Get the version information size for allocate the buffer
	LPTSTR pAppName = m_FileName.GetBuffer(0);
	DWORD fvHandle;
	DWORD dwSize = ::GetFileVersionInfoSize(pAppName, &fvHandle);
	if (0 == dwSize)
	{
		m_FileName.ReleaseBuffer();
		return;
	}

	// Allocate buffer and retrieve version information
	BYTE* pFVData = new BYTE[dwSize];
	if (!::GetFileVersionInfo(pAppName, fvHandle, dwSize, pFVData))
	{
		m_FileName.ReleaseBuffer();
		return;
	}
	m_FileName.ReleaseBuffer();
	// Retrieve the language and character-set identifier
	UINT nQuerySize;
	DWORD* pTransBlock;
	if (!::VerQueryValue(pFVData, _T("\\VarFileInfo\\Translation"), reinterpret_cast<void**>(&pTransBlock), &nQuerySize))
	{
		delete [] pFVData;
		pFVData = NULL;
		return;
	}
	DWORD dwData = 0;
	if (nQuerySize > 4 && (0 != inwLangID || 0 != inwCharSet))
	{
		bool bOk = false;
		for (DWORD offset = 0; !bOk && offset < nQuerySize; offset += sizeof(DWORD))
		{
			dwData = pTransBlock[0];
			if (inwLangID == LOWORD(dwData) && inwCharSet == HIWORD(dwData))
				bOk = true;
		}
		if (!bOk)
			dwData = pTransBlock[0];
	}
	else
		dwData = pTransBlock[0];
	// Swap the words so wsprintf will print the lang-codepage in
	// the correct format.
	//  Hi-order: IBM code page
	//  Lo-order: Microsoft language id
	m_wLangID = LOWORD(dwData);
	WORD wCodePage = HIWORD(dwData);

	// Cache the fixed info.
	VS_FIXEDFILEINFO* pffi;
	if (!::VerQueryValue(pFVData, _T("\\"), reinterpret_cast<void**>(&pffi), &nQuerySize))
	{
		delete [] pFVData;
		pFVData = NULL;
		return;
	}
	// Date didn't get set. Get the create date.
	if (0 == pffi->dwFileDateLS && 0 == pffi->dwFileDateMS)
	{
		HANDLE hFile = CreateFile(m_FileName, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile)
		{
			FILETIME ft;
			GetFileTime(hFile, &ft, NULL, NULL);
			CloseHandle(hFile);
			pffi->dwFileDateLS = ft.dwLowDateTime;
			pffi->dwFileDateMS = ft.dwHighDateTime;
		}
	}

	// Get the real name.
	TCHAR subBlockName[255];
	wsprintf(subBlockName, _T("\\StringFileInfo\\%04hx%04hx\\ProductName"), m_wLangID, wCodePage);
	void* pValue = NULL;
	UINT vSize;
	::VerQueryValue(pFVData, subBlockName, &pValue, &vSize);
	m_ProdName = reinterpret_cast<LPCTSTR>(pValue);

	m_Version.part1 = HIWORD(pffi->dwProductVersionMS);
	m_Version.part2 = LOWORD(pffi->dwProductVersionMS);
	m_Version.part3 = HIWORD(pffi->dwProductVersionLS);
	m_Version.part4 = LOWORD(pffi->dwProductVersionLS);

	delete [] pFVData;
	m_Valid = true;
}


// This ctor is for reading the version number from the web.
CVersionNum::CVersionNum(CString inVer)
	: m_Valid(false)
{
	m_FileName.Empty();
	m_ProdName.Empty();
	// This is a static string in "version.txt"
	static CString const idStr(_T("ARB Version "));
	if (0 == inVer.Find(idStr))
	{
		inVer = inVer.Mid(idStr.GetLength());
		int pos = inVer.Find('.');
		if (0 <= pos)
		{
			m_Version.part1 = static_cast<WORD>(_tstol((LPCTSTR)inVer));
			inVer = inVer.Mid(pos+1);
			pos = inVer.Find('.');
			if (0 <= pos)
			{
				m_Version.part2 = static_cast<WORD>(_tstol((LPCTSTR)inVer));
				inVer = inVer.Mid(pos+1);
				pos = inVer.Find('.');
				if (0 <= pos)
				{
					m_Version.part3 = static_cast<WORD>(_tstol((LPCTSTR)inVer));
					inVer = inVer.Mid(pos+1);
					m_Version.part4 = static_cast<WORD>(_tstol((LPCTSTR)inVer));
					m_Valid = true;
				}
			}
		}
	}
}


// Equality is based solely on the version number, not any language aspects.
bool CVersionNum::operator==(CVersionNum const& rhs) const
{
	return m_Version.part1 == rhs.m_Version.part1
		&& m_Version.part2 == rhs.m_Version.part2
		&& m_Version.part3 == rhs.m_Version.part3
		&& m_Version.part4 == rhs.m_Version.part4;
}


bool CVersionNum::operator<(CVersionNum const& rhs) const
{
	if (m_Version.part1 < rhs.m_Version.part1
	|| (m_Version.part1 == rhs.m_Version.part1
		&& m_Version.part2 < rhs.m_Version.part2)
	|| (m_Version.part1 == rhs.m_Version.part1
		&& m_Version.part2 == rhs.m_Version.part2
		&& m_Version.part3 < rhs.m_Version.part3)
	|| (m_Version.part1 == rhs.m_Version.part1
		&& m_Version.part2 == rhs.m_Version.part2
		&& m_Version.part3 == rhs.m_Version.part3
		&& m_Version.part4 < rhs.m_Version.part4))
	{
		return true;
	}
	return false;
}


bool CVersionNum::operator>(CVersionNum const& rhs) const
{
	if (m_Version.part1 > rhs.m_Version.part1
	|| (m_Version.part1 == rhs.m_Version.part1
		&& m_Version.part2 > rhs.m_Version.part2)
	|| (m_Version.part1 == rhs.m_Version.part1
		&& m_Version.part2 == rhs.m_Version.part2
		&& m_Version.part3 > rhs.m_Version.part3)
	|| (m_Version.part1 == rhs.m_Version.part1
		&& m_Version.part2 == rhs.m_Version.part2
		&& m_Version.part3 == rhs.m_Version.part3
		&& m_Version.part4 > rhs.m_Version.part4))
	{
		return true;
	}
	return false;
}


void CVersionNum::clear()
{
	m_Valid = false;
	m_FileName.Empty();
	m_ProdName.Empty();
	m_Version.part1 = m_Version.part2 = m_Version.part3 = m_Version.part4 = 0;
}


CString CVersionNum::GetVersionString() const
{
	otstringstream str;
	str << m_Version.part1
		<< '.' << m_Version.part2
		<< '.' << m_Version.part3
		<< '.' << m_Version.part4;
	return str.str().c_str();
}


void CVersionNum::GetVersion(VERSION_NUMBER& outVer) const
{
	outVer = m_Version;
}
