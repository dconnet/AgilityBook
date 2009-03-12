#pragma once

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
 * @brief interface of the VersionNum class
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-28 DRC Removed Windows VERSIONNUM support (use VersionNumber.h)
 * @li 2009-01-01 DRC Ported to wxWidgets.
 * @li 2007-10-24 DRC Changed ctor and added <= operators
 * @li 2004-03-04 DRC Created
 */


class CVersionNum
{
public:
	typedef struct
	{
		unsigned short part1;
		unsigned short part2;
		unsigned short part3;
		unsigned short part4;
	} VERSION_NUMBER;

	CVersionNum();
	CVersionNum(CVersionNum const& rhs);
	CVersionNum& operator=(CVersionNum const& rhs);

	/**
	 * Parse a version number, must be in form "n.n.n.n".
	 */
	bool Parse(wxString inFileName, wxString inVer);

	/**
	 * Equality is based solely on the version number, not any language aspects.
	 */
	bool operator==(CVersionNum const& rhs) const;
	bool operator<(CVersionNum const& rhs) const;
	bool operator<=(CVersionNum const& rhs) const;
	bool operator>(CVersionNum const& rhs) const;
	bool operator>=(CVersionNum const& rhs) const;

	void clear();
	bool Valid() const						{return m_Valid;}
	wxString GetVersionString() const;
	void GetVersion(VERSION_NUMBER& outVer) const;

private:
	bool m_Valid;
	VERSION_NUMBER m_Version;
};
