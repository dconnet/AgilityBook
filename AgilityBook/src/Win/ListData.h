#pragma once

/*
 * Copyright © 2006-2008 David Connet. All Rights Reserved.
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
 * @brief Make selection in a listctrl easier.
 * @author David Connet
 *
 * Revision History
 * @li 2008-11-22 DRC Moved GetDispInfo accessors into base class.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2006-02-12 DRC Created
 */

#include "ARBString.h"

/// Controls that store data all insert the data via reinterpret_cast to this.
class CListData
{
public:
	CListData();
	virtual ~CListData();
};


/// For data pulled out of the control (via reinterpret_cast), dynamic_cast
/// to this class to make sure we have the right data.
class CListDataDispInfo : public CListData
{
public:
	CListDataDispInfo();
	virtual ~CListDataDispInfo();
	virtual tstring OnNeedText() const;	 /// For trees (or things that want a single text string of row)
	virtual tstring OnNeedText(int iCol) const;	 /// For list controls
	virtual int OnNeedIcon() const;
};


template <typename T>
class CListPtrData : public CListData
{
public:
	CListPtrData(T inData) : m_Data(inData) {}
	T GetData() const	{return m_Data;}
protected:
	T m_Data;
};
