#pragma once

/*
 * Copyright � 2007 David Connet. All Rights Reserved.
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
 * @brief Interface for plug-in DLLs to read data.
 * @author David Connet
 *
 * In order to read data directly from a website, we need to parse the HTML.
 * Since all sites will be different, we'll deal with this thru a plug-in
 * architecture. All DLLs must export an undecorated api 'GetCalendarInterface'
 * that will be obtained using GetProcAddress.
 *
 * All extension DLLs should be named 'cal_<yourname>.dll' and placed in the
 * same directory as the executable.
 *
 * Revision History
 * @li 2007-08-12 DRC Created
 */

class ICalendarSite
{
public:
	/**
	 * Release object.
	 */
	virtual void Release() = 0;

	/**
	 * Release a string returned from GetDescription or Process.
	 * This must be done because the objects may have been allocated on
	 * different heaps. Kind of sucks, would be much easier to just return
	 * an STL string.
	 */
	virtual void releaseBuffer(char* pData) const = 0;

	/**
	 * Get a description of the site this parses.
	 */
	virtual char* GetDescription() const = 0;

	/**
	 * Get the processed data. The returned data should be in the form of
	 * a valid ARB file. If this were to be saved as a file, we could then
	 * directly import it in ARB.
	 */
	virtual char* Process() const = 0;
};

typedef ICalendarSite* (*GETCALENDARINTERFACE)();
