#pragma once

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
 * @li 2009-01-26 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-06-06 DRC Separated from AgilityBookTreeData.cpp
 */

#include "ARB/ARBTypes2.h"
#include <wx/clipbrd.h>

// Name of root element when writing Elements.
#define CLIPDATA	L"ClipData"

// Special clipboard formats
typedef enum
{
	eFormatNone,
	eFormatDog,
	eFormatTrial,
	eFormatRun,
	eFormatCalendar,
	eFormatiCalendar,
	eFormatLog,
	eFormatHtml,
} eClipFormat;


/**
 * Clipboard reader/writer base class.
 */
class CClipboardData
{
public:
	CClipboardData(bool bAutoOpen);
	virtual ~CClipboardData();

	bool Open();
	void Close();

	/**
	 * Check if clipboard is ready for use.
	 */
	bool isOkay() const	{return m_bOkay;}

protected:
	bool m_bOkay;
};


class CClipboardDataReader : public CClipboardData
{
public:
	CClipboardDataReader();

	static bool IsFormatAvailable(eClipFormat clpFmt);

	bool GetData(
			eClipFormat clpFmt,
			ElementNodePtr outTree);

	bool GetData(std::wstring& outData);
};


class CClipboardDataWriter;

/**
 * Helper class to create html tables when copying to clipboard
 */
class CClipboardDataTable
{
	DECLARE_NO_COPY_IMPLEMENTED(CClipboardDataTable)
public:
	CClipboardDataTable(std::wstring& ioText, std::wstring& ioHtml);
	void Reset();
	void StartLine();
	void EndLine();
	void Cell(int nCol, std::wstring const& inData);
	bool Write(CClipboardDataWriter& writer, bool bCommit);
private:
	bool m_Closed;
	std::wstring& m_ioText;
	std::wstring& m_ioHtml;
};

/**
 * Copy data to the clipboard. SetData can be called on different formats.
 */
class CClipboardDataWriter : public CClipboardData
{
public:
	CClipboardDataWriter();
	virtual ~CClipboardDataWriter();

	bool AddData(
			eClipFormat clpFmt,
			ElementNodePtr inTree);

	// Used to write special data.
	bool AddData(
			eClipFormat clpFmt,
			std::wstring const& inData);

	bool AddData(std::wstring const& inData);

	bool AddData(CClipboardDataTable& inData);

	bool CommitData();

private:
	wxDataObjectComposite* m_Object;
};
