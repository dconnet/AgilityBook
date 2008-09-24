/*
 * Copyright © 2004-2008 David Connet. All Rights Reserved.
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
 * @brief Clipboard functions for copying data to/from clipboard.
 * @author David Connet
 *
 * Revision History
 * @li 2004-06-06 DRC Separated from AgilityBookTreeData.cpp
 */

#include "stdafx.h"
#include <sstream>
#include "AgilityBook.h"
#include "ClipBoard.h"

#include "AgilityBookOptions.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Special clipboard formats

static UINT GetClipboardFormat(eClipFormat fmt)
{
	static bool bInitialized = false;
	static UINT uDog = 0;
	static UINT uTrial = 0;
	static UINT uRun = 0;
	static UINT uCal = 0;
	static UINT uiCal = 0;
	static UINT uLog = 0;
	static UINT uHtml = 0;
	if (!bInitialized)
	{
		bInitialized = true;
		uDog = RegisterClipboardFormat(_T("ARB-Dog"));
		uTrial = RegisterClipboardFormat(_T("ARB-Trial"));
		uRun = RegisterClipboardFormat(_T("ARB-Run"));
		uCal = RegisterClipboardFormat(_T("ARB-Cal"));
		uiCal = RegisterClipboardFormat(_T("+//ISBN 1-887687-00-9::versit::PDI//vCalendar"));
		uLog = RegisterClipboardFormat(_T("ARB-Log"));
		uHtml = RegisterClipboardFormat(_T("HTML Format"));
	}
	switch (fmt)
	{
	default:				return 0;
	case eFormatDog:		return uDog;
	case eFormatTrial:		return uTrial;
	case eFormatRun:		return uRun;
	case eFormatCalendar:	return uCal;
	case eFormatiCalendar:	return uiCal;
	case eFormatLog:		return uLog;
	case eFormatHtml:		return uHtml;
	}
}

////////////////////////////////////////////////////////////////////////////

CClipboardData::CClipboardData(bool bAutoOpen)
	: m_bOkay(false)
{
	if (bAutoOpen)
		Open();
}


CClipboardData::~CClipboardData()
{
	Close();
}


bool CClipboardData::Open()
{

	if (!m_bOkay)
	{
		if (AfxGetMainWnd() && AfxGetMainWnd()->OpenClipboard())
			m_bOkay = true;
	}
	return m_bOkay;
}


void CClipboardData::Close()
{
	if (m_bOkay)
	{
		m_bOkay = false;
		CloseClipboard();
	}
}

////////////////////////////////////////////////////////////////////////////

CClipboardDataReader::CClipboardDataReader()
	: CClipboardData(false)
{
}


BOOL CClipboardDataReader::IsFormatAvailable(eClipFormat clpFmt)
{
	return ::IsClipboardFormatAvailable(GetClipboardFormat(clpFmt));
}


bool CClipboardDataReader::GetData(
		eClipFormat clpFmt,
		ElementNodePtr outTree)
{
	bool bOk = false;
	outTree->clear();
	CStringA data; // Our internal formats are always written as ascii text
	if (GetData(GetClipboardFormat(clpFmt), data))
	{
#ifdef _DEBUG
		OutputDebugStringA((LPCSTR)data);
		OutputDebugStringA("\n");
#endif
		tstring err;
		bOk = outTree->LoadXMLBuffer((LPCSTR)data, data.GetLength(), err);
		if (!bOk && 0 < err.length())
		{
			AfxMessageBox(err.c_str(), MB_ICONEXCLAMATION);
		}
	}
	return bOk;
}


bool CClipboardDataReader::GetData(CStringA& outData)
{
	return GetData(CF_TEXT, outData);
}


#if _MSC_VER >= 1300
bool CClipboardDataReader::GetData(CStringW& outData)
{
	outData.Empty();
	if (!m_bOkay)
	{
		if (!Open())
			return false;
	}
	bool bOk = false;
	if (IsClipboardFormatAvailable(CF_UNICODETEXT))
	{
		bOk = true;
		HANDLE hData = GetClipboardData(CF_UNICODETEXT);
		outData = CStringW(reinterpret_cast<wchar_t const*>(GlobalLock(hData)));
		GlobalUnlock(hData);
	}
	return bOk;
}
#endif


/**
 * Internal function for reading ascii text.
 * This will either be CF_TEXT or one of our internal formats.
 */
bool CClipboardDataReader::GetData(
		UINT uFormat,
		CStringA& outData)
{
	outData.Empty();
	if (!m_bOkay)
	{
		if (!Open())
			return false;
	}
	bool bOk = false;
	if (IsClipboardFormatAvailable(uFormat))
	{
		bOk = true;
		HANDLE hData = GetClipboardData(uFormat);
		outData = CStringA(reinterpret_cast<char const*>(GlobalLock(hData)));
		GlobalUnlock(hData);
	}
	return bOk;
}

////////////////////////////////////////////////////////////////////////////

CClipboardDataTable::CClipboardDataTable(CString& ioText, CString& ioHtml)
	: m_ioText(ioText)
	, m_ioHtml(ioHtml)
	, m_Closed(false)
{
	Reset();
}


void CClipboardDataTable::Reset()
{
	m_ioHtml = _T("<table border=\"1\">");
	m_ioText.Empty();
	m_Closed = false;
}


void CClipboardDataTable::StartLine()
{
	m_ioHtml += _T("<TR>");
}


void CClipboardDataTable::EndLine()
{
	m_ioText += _T("\r\n");
	m_ioHtml += _T("</TR>");
}


void CClipboardDataTable::Cell(int iCol, CString const& inData)
{
	if (0 < iCol)
		m_ioText += '\t';
	m_ioText += inData;
	m_ioHtml += _T("<TD>");
	m_ioHtml += inData;
	m_ioHtml += _T("</TD>");
}


bool CClipboardDataTable::Write(CClipboardDataWriter& writer)
{
	if (!m_Closed)
	{
		m_ioHtml += _T("</table>");
		m_Closed = true;
	}
	writer.SetData(eFormatHtml, m_ioHtml);
	return writer.SetData(m_ioText);
}

/////////////////////////////////////////////////////////////////////////////

CClipboardDataWriter::CClipboardDataWriter()
	: CClipboardData(true)
{
	if (m_bOkay)
	{
		if (!EmptyClipboard())
			Close();
	}
}


bool CClipboardDataWriter::SetData(
		eClipFormat clpFmt,
		ElementNodePtr inTree)
{
	if (!m_bOkay)
		return false;

	std::ostringstream out;
	inTree->SaveXML(out);
	return SetData(clpFmt, out.str());
}


bool CClipboardDataWriter::SetData(
		eClipFormat clpFmt,
		std::string const& inData)
{
	return SetData(GetClipboardFormat(clpFmt), inData.c_str(), sizeof(char)*(inData.length()+1));
}


bool CClipboardDataWriter::SetData(
		eClipFormat clpFmt,
		std::wstring const& inData)
{
	if (eFormatHtml == clpFmt)
		return SetData(clpFmt, tstringUtil::Convert(inData));
	else
		return SetData(GetClipboardFormat(clpFmt), inData.c_str(), sizeof(wchar_t)*(inData.length()+1));
}


bool CClipboardDataWriter::SetData(
		eClipFormat clpFmt,
		CStringA const& inData)
{
	return SetData(GetClipboardFormat(clpFmt), inData, sizeof(char)*(inData.GetLength()+1));
}


#if _MSC_VER >= 1300
bool CClipboardDataWriter::SetData(
		eClipFormat clpFmt,
		CStringW const& inData)
{
	if (eFormatHtml == clpFmt)
		return SetData(clpFmt, CStringA(inData));
	else
		return SetData(GetClipboardFormat(clpFmt), (LPCWSTR)inData, sizeof(wchar_t)*(inData.GetLength()+1));
}
#endif


// Note, when setting text, the OS will auto-populate the other formats too.
bool CClipboardDataWriter::SetData(std::string const& inData)
{
	return SetData(CF_TEXT, inData.c_str(), inData.length());
}


bool CClipboardDataWriter::SetData(std::wstring const& inData)
{
	return SetData(CF_UNICODETEXT, inData.c_str(), sizeof(wchar_t)*(inData.length()+1));
}


bool CClipboardDataWriter::SetData(CStringA const& inData)
{
	return SetData(CF_TEXT, (LPCSTR)inData, inData.GetLength());
}


#if _MSC_VER >= 1300
bool CClipboardDataWriter::SetData(CStringW const& inData)
{
	return SetData(CF_UNICODETEXT, (LPCWSTR)inData, sizeof(wchar_t)*(inData.GetLength()+1));
}
#endif


bool CClipboardDataWriter::SetData(CClipboardDataTable& inData)
{
	return inData.Write(*this);
}


bool CClipboardDataWriter::SetData(
		UINT uFormat,
		void const* inData,
		size_t inLen)
{
	if (!m_bOkay)
		return false;

	std::string html;
	if (GetClipboardFormat(eFormatHtml) == uFormat)
	{
		std::string data(reinterpret_cast<char const*>(inData), inLen);
		inLen = data.length();
		std::string startHtml("<html><body>\r\n");
		std::string endHtml("</body>\r\n</html>\r\n");
		std::string startFragment("<!--StartFragment-->");
		std::string endFragment("<!--EndFragment-->");
#if _MSC_VER >= 1300 && _MSC_VER < 1400
		// VC7 has issues streaming size_t: warnings about 'size_t' to 'unsigned int' conversion
		int lenData = static_cast<int>(inLen);
		int lenHeader = 97;
		int lenStartHtml = static_cast<int>(startHtml.length());
		int lenEndHtml = static_cast<int>(endHtml.length());
		int lenStartFragment = static_cast<int>(startFragment.length());
		int lenEndFragment = static_cast<int>(endFragment.length());
#else
		size_t lenData = inLen;
		size_t lenHeader = 97;
		size_t lenStartHtml = startHtml.length();
		size_t lenEndHtml = endHtml.length();
		size_t lenStartFragment = startFragment.length();
		size_t lenEndFragment = endFragment.length();
#endif
		std::ostringstream out;
		out.fill('0');
		out << "Version:0.9\r\nStartHTML:";
		out.width(8);
		out << lenHeader << "\r\nEndHTML:";
		out.width(8);
		out << lenHeader + lenStartHtml + lenStartFragment + lenData + lenEndFragment + lenEndHtml
			<< "\r\nStartFragment:";
		out.width(8);
		out << lenHeader + lenStartHtml + lenStartFragment
			<< "\r\nEndFragment:";
		out.width(8);
		out << lenHeader + lenStartHtml + lenStartFragment + lenData << "\r\n";
#ifdef _DEBUG
		ASSERT(out.str().length() == lenHeader);
#endif
		out << startHtml << startFragment
			<< data
			<< endFragment << endHtml;
		html = out.str();
		inData = html.c_str();
		inLen = html.length() + 1;
	}

	bool bOk = false;
	if (inData && 0 < inLen)
	{
		HGLOBAL temp = GlobalAlloc(GHND, inLen);
		if (NULL != temp)
		{
			bOk = true;
			void* pData = GlobalLock(temp);
			memcpy(pData, inData, inLen);
			GlobalUnlock(temp);
			// send data to clipbard
			SetClipboardData(uFormat, temp);
		}
	}
	return bOk;
}
