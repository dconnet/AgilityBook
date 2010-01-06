#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
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

#include "ARBTypes.h"
#include <wx/clipbrd.h>

// Name of root element when writing Elements.
#define CLIPDATA	wxT("ClipData")

// Special clipboard formats
typedef enum
{
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

	bool GetData(wxString& outData);
};


class CClipboardDataWriter;

/**
 * Helper class to create html tables when copying to clipboard
 */
class CClipboardDataTable
{
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
public:
	CClipboardDataWriter();
	virtual ~CClipboardDataWriter();

	bool AddData(
			eClipFormat clpFmt,
			ElementNodePtr inTree);

	// Used to write special data.
	bool AddData(
			eClipFormat clpFmt,
			wxString const& inData);

	bool AddData(wxString const& inData);

	bool AddData(CClipboardDataTable& inData);

	bool CommitData();

private:
	wxDataObjectComposite* m_Object;
};
