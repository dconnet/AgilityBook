#pragma once

/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
 * @brief Base class for reference counting.
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 */

#include <set>
#include <string>

class ARBBase
{
public:
	ARBBase();

	/**
	 * Add a reference to the object.
	 */
	virtual void AddRef();

	/**
	 * Remove a reference
	 * @post When the reference count goes to 0, the object will be deleted.
	 */
	virtual void Release();

	/**
	 * Get the generic name of an object.
	 * This allows us to display a list of ARBBase objects to the user
	 * without knowing what the objects actually are.
	 * @return Name of the object.
	 */
	virtual std::string GetGenericName() const = 0;

	/**
	 * Get all the strings to search in an object.
	 */
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const = 0;

	/**
	 * Get the filtered state of this object.
	 */
	virtual bool IsFiltered() const				{return m_bFiltered;}

	/**
	 * Set the filtered state of this object.
	 * This attribute is not persistent. It is up to the UI to manage this.
	 */
	virtual void SetFiltered(bool bFiltered)	{m_bFiltered = bFiltered;}

protected:
	/**
	 * Protect the dtor to make sure no one can delete this.
	 */
	virtual ~ARBBase();
	unsigned int m_RefCount;
	bool m_bFiltered;
};
