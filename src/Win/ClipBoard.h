#pragma once

/*
 * Copyright © 2004-2006 David Connet. All Rights Reserved.
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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-06-06 DRC Separated from AgilityBookTreeData.cpp
 */

class Element;

// Name of root element when writing Elements.
#define CLIPDATA	_T("ClipData")

// Special clipboard formats
typedef enum
{
	eFormatDog,
	eFormatTrial,
	eFormatRun,
	eFormatCalendar,
	eFormatiCalendar,
	eFormatLog,
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

	static BOOL IsFormatAvailable(eClipFormat clpFmt);

	bool GetData(
			eClipFormat clpFmt,
			Element& outTree);

	bool GetData(CStringA& outData);
#if _MSC_VER >= 1300
	bool GetData(CStringW& outData);
#endif

private:
	bool GetData(
			UINT uFormat,
			CStringA& outData);
};

/**
 * Copy data to the clipboard. SetData can be called on different formats.
 */
class CClipboardDataWriter : public CClipboardData
{
public:
	CClipboardDataWriter();

	bool SetData(
			eClipFormat clpFmt,
			Element const& inTree);

	// Used to write special data.
	bool SetData(
			eClipFormat clpFmt,
			std::string const& inData);
	bool SetData(
			eClipFormat clpFmt,
			CStringA const& inData);

	// Write data as CF_TEXT (in Unicode builds, CF_UNICODETEXT also)
	bool SetData(ARBString const& inData);
	bool SetData(CString const& inData);

	// Raw form.
	bool SetData(
			UINT uFormat,
			void const* inData,
			size_t inLen);
};
