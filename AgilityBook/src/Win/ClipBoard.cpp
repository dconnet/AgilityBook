/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Clipboard functions for copying data to/from clipboard.
 * @author David Connet
 *
 * Revision History
 * @li 2012-03-16 DRC Renamed LoadXML functions, added stream version.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-01-26 DRC Ported to wxWidgets.
 * @li 2004-06-06 DRC Separated from AgilityBookTreeData.cpp
 */

#include "stdafx.h"
#include "ClipBoard.h"

#include "AgilityBook.h"
#include "AgilityBookOptions.h"

#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include <sstream>
#include <wx/mstream.h>

#ifdef _DEBUG
#include <assert.h>
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// Special clipboard formats

static wxDataFormat GetClipboardFormat(eClipFormat fmt)
{
	switch (fmt)
	{
	default:				return wxDataFormat(wxDF_INVALID);
	case eFormatDog:		return wxDataFormat(L"ARB-Dog");
	case eFormatTrial:		return wxDataFormat(L"ARB-Trial");
	case eFormatRun:		return wxDataFormat(L"ARB-Run");
	case eFormatCalendar:	return wxDataFormat(L"ARB-Cal");
	case eFormatiCalendar:	return wxDataFormat(L"+//ISBN 1-887687-00-9::versit::PDI//vCalendar");
	case eFormatLog:		return wxDataFormat(L"ARB-Log");
	// Not using wxDT_HTML aince OLE is defined which causes the wx code that
	// "supports" html copying to not be invoked. So we do it.
	case eFormatHtml:		return wxDataFormat(L"HTML Format");
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
		if (wxTheClipboard && wxTheClipboard->Open())
			m_bOkay = true;
	}
	return m_bOkay;
}


void CClipboardData::Close()
{
	if (m_bOkay)
	{
		m_bOkay = false;
		wxTheClipboard->Flush();
		wxTheClipboard->Close();
	}
}

////////////////////////////////////////////////////////////////////////////

CClipboardDataReader::CClipboardDataReader()
	: CClipboardData(false)
{
}


bool CClipboardDataReader::IsFormatAvailable(eClipFormat clpFmt)
{
	return wxTheClipboard && wxTheClipboard->IsSupported(GetClipboardFormat(clpFmt));
}


bool CClipboardDataReader::GetData(
		eClipFormat clpFmt,
		ElementNodePtr outTree)
{
	outTree->clear();
	if (!wxTheClipboard->IsSupported(GetClipboardFormat(clpFmt)) || !Open())
		return false;
	std::string data;
	{
		wxTextDataObject txtData;
		txtData.SetFormat(GetClipboardFormat(clpFmt));
		wxTheClipboard->GetData(txtData);
		data = txtData.GetText().ToUTF8();
	}
	std::wostringstream err;
	bool bOk = false;
	{
		wxBusyCursor wait;
		bOk = outTree->LoadXML(data.c_str(), data.length(), err);
	}
	if (!bOk && 0 < err.str().length())
	{
		wxMessageBox(StringUtil::stringWX(err.str()), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
	}
	return bOk;
}


bool CClipboardDataReader::GetData(std::wstring& outData)
{
	outData.clear();
	if (!wxTheClipboard->IsSupported(wxDF_TEXT) || !Open())
		return false;
	wxTextDataObject data;
	wxTheClipboard->GetData(data);
	outData = data.GetText();
	return true;
}

////////////////////////////////////////////////////////////////////////////

CClipboardDataTable::CClipboardDataTable(std::wstring& ioText, std::wstring& ioHtml)
	: m_ioText(ioText)
	, m_ioHtml(ioHtml)
	, m_Closed(false)
{
	Reset();
}


void CClipboardDataTable::Reset()
{
	m_ioHtml = L"<table border=\"1\">";
	m_ioText.clear();
	m_Closed = false;
}


void CClipboardDataTable::StartLine()
{
	m_ioHtml += L"<TR>";
}


void CClipboardDataTable::EndLine()
{
	m_ioText += L"\r\n";
	m_ioHtml += L"</TR>";
}


void CClipboardDataTable::Cell(int iCol, std::wstring const& inData)
{
	if (0 < iCol)
		m_ioText += L"\t";
	m_ioText += inData;
	m_ioHtml += L"<TD>";
	m_ioHtml += inData;
	m_ioHtml += L"</TD>";
}


bool CClipboardDataTable::Write(CClipboardDataWriter& writer, bool bCommit)
{
	bool rc = false;
	if (!m_Closed)
	{
		m_ioHtml += L"</table>";
		m_Closed = true;
	}
	rc |= writer.AddData(eFormatHtml, m_ioHtml);
	rc |= writer.AddData(m_ioText);
	if (bCommit)
		return writer.CommitData();
	else
		return rc;
}

/////////////////////////////////////////////////////////////////////////////

CClipboardDataWriter::CClipboardDataWriter()
	: CClipboardData(true)
	, m_Object(NULL)
{
	if (m_bOkay)
	{
		wxTheClipboard->Clear();
	}
}


CClipboardDataWriter::~CClipboardDataWriter()
{
	CommitData();
}


bool CClipboardDataWriter::AddData(
		eClipFormat clpFmt,
		ElementNodePtr inTree)
{
	if (!m_bOkay)
		return false;

	std::wstring data;
	{
		std::stringstream out;
		inTree->SaveXML(out);
		data = StringUtil::stringW(out.str());
	}
	return AddData(clpFmt, data);
}


bool CClipboardDataWriter::AddData(
		eClipFormat clpFmt,
		std::wstring const& inData)
{
	if (eFormatHtml == clpFmt)
	{
		std::string data = StringUtil::stringA(inData);
		{
			std::string startHtml("<html><body>\r\n");
			std::string endHtml("</body>\r\n</html>\r\n");
			std::string startFragment("<!--StartFragment-->");
			std::string endFragment("<!--EndFragment-->");
#if _MSC_VER >= 1300 && _MSC_VER < 1400
			// VC7 has issues streaming size_t: warnings about 'size_t' to 'unsigned int' conversion
			int lenData = static_cast<int>(data.length());
			int lenHeader = 97;
			int lenStartHtml = static_cast<int>(startHtml.length());
			int lenEndHtml = static_cast<int>(endHtml.length());
			int lenStartFragment = static_cast<int>(startFragment.length());
			int lenEndFragment = static_cast<int>(endFragment.length());
#else
			size_t lenData = data.length();
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
			assert(out.str().length() == static_cast<std::string::size_type>(lenHeader));
#endif
			out << startHtml << startFragment
				<< data
				<< endFragment << endHtml;
			data = out.str();
		}
		wxCustomDataObject* dataObj = new wxCustomDataObject(wxDataFormat(L"HTML Format"));
		dataObj->SetData(data.length() + 1, data.c_str());
		if (!m_Object)
			m_Object = new wxDataObjectComposite();
		m_Object->Add(dataObj);
		return true;
	}
	else
	{
		wxTextDataObject* dataObj = new wxTextDataObject(inData.c_str());
		dataObj->SetFormat(GetClipboardFormat(clpFmt));
		if (!m_Object)
			m_Object = new wxDataObjectComposite();
		m_Object->Add(dataObj);
		return true;
	}
}


bool CClipboardDataWriter::AddData(std::wstring const& inData)
{
	if (!m_Object)
		m_Object = new wxDataObjectComposite();
	m_Object->Add(new wxTextDataObject(inData.c_str()));
	return true;
}


bool CClipboardDataWriter::AddData(CClipboardDataTable& inData)
{
	return inData.Write(*this, false);
}


bool CClipboardDataWriter::CommitData()
{
	bool bOk = false;
	if (!m_Object)
		return bOk;
	if (wxTheClipboard->SetData(m_Object))
		bOk = true;
	else
		delete m_Object;
	m_Object = NULL;
	return bOk;
}
