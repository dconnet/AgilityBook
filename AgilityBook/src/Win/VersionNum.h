#pragma once

/*
 * Copyright © 2004-2005 David Connet. All Rights Reserved.
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
 * @brief interface of the VersionNum class
 * @author David Connet
 *
 * Revision History
 * @li 2004-03-04 DRC Created
 */

class CVersionNum
{
public:
	typedef struct
	{
		WORD part1;
		WORD part2;
		WORD part3;
		WORD part4;
	} VERSION_NUMBER;

	CVersionNum(WORD inwLangID = 0, WORD inwCharSet = 0);
	/**
	 * Used to translate the "version.txt" on www.agilityrecordbook.com
	 */
	explicit CVersionNum(CString inVer);

	bool operator==(CVersionNum const& rhs) const;
	bool operator<(CVersionNum const& rhs) const;
	bool operator>(CVersionNum const& rhs) const;

	void clear();
	bool Valid() const						{return m_Valid;}
	CString GetName() const					{return m_Name;}
	CString GetVersionString() const;
	void GetVersion(VERSION_NUMBER& outVer) const; 

private:
	bool m_Valid;
	CString m_Name;
	VERSION_NUMBER m_Version;
};
