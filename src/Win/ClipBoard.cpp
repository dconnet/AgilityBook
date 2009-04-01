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
 * @brief Clipboard functions for copying data to/from clipboard.
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-26 DRC Ported to wxWidgets.
 * @li 2004-06-06 DRC Separated from AgilityBookTreeData.cpp
 */

#include "stdafx.h"
#include <sstream>
#include "AgilityBook.h"
#include "ClipBoard.h"

#include "AgilityBookOptions.h"
#include "Element.h"

#ifdef _DEBUG
#include <assert.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// Special clipboard formats

static wxDataFormat GetClipboardFormat(eClipFormat fmt)
{
	switch (fmt)
	{
	default:				return wxDataFormat(wxDF_INVALID);
	case eFormatDog:		return wxDataFormat(wxT("ARB-Dog"));
	case eFormatTrial:		return wxDataFormat(wxT("ARB-Trial"));
	case eFormatRun:		return wxDataFormat(wxT("ARB-Run"));
	case eFormatCalendar:	return wxDataFormat(wxT("ARB-Cal"));
	case eFormatiCalendar:	return wxDataFormat(wxT("+//ISBN 1-887687-00-9::versit::PDI//vCalendar"));
	case eFormatLog:		return wxDataFormat(wxT("ARB-Log"));
	// Not using wxDT_HTML aince OLE is defined which causes the wx code that
	// "supports" html copying to not be invoked. So we do it.
	case eFormatHtml:		return wxDataFormat(wxT("HTML Format"));
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
		data = tstringUtil::tstringA(txtData.GetText().c_str());
	}
	tstring err;
	bool bOk = outTree->LoadXMLBuffer(data.c_str(), data.length(), err);
	if (!bOk && 0 < err.length())
	{
		wxMessageBox(err.c_str(), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
	}
	return bOk;
}


bool CClipboardDataReader::GetData(wxString& outData)
{
	outData.Empty();
	if (!wxTheClipboard->IsSupported(wxDF_TEXT) || !Open())
		return false;
	wxTextDataObject data;
	wxTheClipboard->GetData(data);
	outData = data.GetText();
	return true;
}

////////////////////////////////////////////////////////////////////////////

CClipboardDataTable::CClipboardDataTable(wxString& ioText, wxString& ioHtml)
	: m_ioText(ioText)
	, m_ioHtml(ioHtml)
	, m_Closed(false)
{
	Reset();
}


void CClipboardDataTable::Reset()
{
	m_ioHtml = wxT("<table border=\"1\">");
	m_ioText.Empty();
	m_Closed = false;
}


void CClipboardDataTable::StartLine()
{
	m_ioHtml += wxT("<TR>");
}


void CClipboardDataTable::EndLine()
{
	m_ioText += wxT("\r\n");
	m_ioHtml += wxT("</TR>");
}


void CClipboardDataTable::Cell(int iCol, wxString const& inData)
{
	if (0 < iCol)
		m_ioText += '\t';
	m_ioText += inData;
	m_ioHtml += wxT("<TD>");
	m_ioHtml += inData;
	m_ioHtml += wxT("</TD>");
}


bool CClipboardDataTable::Write(CClipboardDataWriter& writer)
{
	if (!m_Closed)
	{
		m_ioHtml += wxT("</table>");
		m_Closed = true;
	}
	writer.AddData(eFormatHtml, m_ioHtml);
	writer.AddData(m_ioText);
	return writer.CommitData();
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

	wxString data;
	{
		std::ostringstream out;
		inTree->SaveXML(out);
		data = tstringUtil::TString(out.str());
	}
	return AddData(clpFmt, data);
}


bool CClipboardDataWriter::AddData(
		eClipFormat clpFmt,
		wxString const& inData)
{
	if (eFormatHtml == clpFmt)
	{
		std::string data = tstringUtil::tstringA(inData.c_str());
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
		wxCustomDataObject* dataObj = new wxCustomDataObject(wxDataFormat(wxT("HTML Format")));
		dataObj->SetData(data.length() + 1, data.c_str());
		if (!m_Object)
			m_Object = new wxDataObjectComposite();
		m_Object->Add(dataObj);
		return true;
	}
	else
	{
		wxTextDataObject* dataObj = new wxTextDataObject(inData);
		dataObj->SetFormat(GetClipboardFormat(clpFmt));
		if (!m_Object)
			m_Object = new wxDataObjectComposite();
		m_Object->Add(dataObj);
		return true;
	}
}


bool CClipboardDataWriter::AddData(wxString const& inData)
{
		if (!m_Object)
			m_Object = new wxDataObjectComposite();
	m_Object->Add(new wxTextDataObject(inData));
	return true;
}


bool CClipboardDataWriter::AddData(CClipboardDataTable& inData)
{
	return inData.Write(*this);
}


bool CClipboardDataWriter::CommitData()
{
	if (!m_Object)
		return false;
	wxTheClipboard->SetData(m_Object);
	m_Object = NULL;
	return true;
}
