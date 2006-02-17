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

#define CLIPDATA	_T("ClipData")

/**
 * Copy the given tree to the clipboard, along with a text form if given.
 * @param clpFmt Special registered clipboard format
 * @param tree Tree to save to clipboard
 * @param txtForm Text form to include on clipboard
 */
extern bool CopyDataToClipboard(
		UINT clpFmt,
		Element const& tree,
		CString const& txtForm);
//TODO: Remove the above call and use only class
class CClipboardDataWriter
{
public:
	CClipboardDataWriter();
	~CClipboardDataWriter();

	bool isOkay() const	{return m_bOkay;}

	bool SetData(
			UINT clpFmt,
			Element const& inTree);
	bool SetData(
			UINT clpFmt,
			TCHAR const* const inData,
			size_t inLen);

private:
	bool m_bOkay;
};

/**
 * Get the data from the clipboard.
 * @param clpFmt Special registered clipboard format
 * @param tree Tree to return.
 */
extern bool GetDataFromClipboard(
		UINT clpFmt,
		Element& tree);
