#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Clipboard functions for copying data to/from clipboard.
 * @author David Connet
 *
 * Revision History
 * 2009-01-26 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-06-06 Separated from AgilityBookTreeData.cpp
 */

#include "ARB/ARBTypes2.h"
#include <wx/clipbrd.h>


namespace dconSoft
{

// Name of root element when writing Elements.
constexpr wchar_t CLIPDATA[] = L"ClipData";

// Special clipboard formats
enum class ARBClipFormat
{
	None,
	Dog,
	Trial,
	Run,
	Calendar,
	iCalendar,
	Log,
	Html,
};


/**
 * Clipboard reader/writer base class.
 */
class CClipboardData
{
	DECLARE_NO_COPY_IMPLEMENTED(CClipboardData)
public:
	CClipboardData(bool bAutoOpen);
	virtual ~CClipboardData();

	bool Open();
	void Close();

	/**
	 * Check if clipboard is ready for use.
	 */
	bool isOkay() const
	{
		return m_bOkay;
	}

protected:
	bool m_bOkay;
};


class CClipboardDataReader : public CClipboardData
{
	DECLARE_NO_COPY_IMPLEMENTED(CClipboardDataReader)
public:
	CClipboardDataReader();

	static bool IsFormatAvailable(ARBClipFormat clpFmt);

	bool GetData(ARBClipFormat clpFmt, ARBCommon::ElementNodePtr const& outTree);

	bool GetData(wxString& outData);
};


class CClipboardDataWriter;

/**
 * Helper class to create html tables when copying to clipboard
 */
class CClipboardDataTable
{
	DECLARE_NO_COPY_IMPLEMENTED(CClipboardDataTable)
public:
	CClipboardDataTable(wxString& ioText, wxString& ioHtml);
	void Reset();
	void StartLine();
	void EndLine();
	void Cell(int nCol, wxString const& inData);
	bool Write(CClipboardDataWriter& writer, bool bCommit);

private:
	bool m_Closed;
	wxString& m_ioText;
	wxString& m_ioHtml;
};

/**
 * Copy data to the clipboard. SetData can be called on different formats.
 */
class CClipboardDataWriter : public CClipboardData
{
	DECLARE_NO_COPY_IMPLEMENTED(CClipboardDataWriter)
public:
	CClipboardDataWriter();
	~CClipboardDataWriter();

	bool AddData(ARBClipFormat clpFmt, ARBCommon::ElementNodePtr const& inTree);

	// Used to write special data.
	bool AddData(ARBClipFormat clpFmt, wxString const& inData);

	bool AddData(wxString const& inData);

	bool AddData(CClipboardDataTable& inData);

	bool CommitData();

private:
	wxDataObjectComposite* m_Object;
};

} // namespace dconSoft
