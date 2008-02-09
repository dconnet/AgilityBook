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
 * @brief Make inserting items in a listbox easier.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-05 DRC Added CVenueComboBox
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2006-02-12 DRC Created
 */

#include "ARBTypes.h"
class CListData;


/// Combobox that cleans up cached data objects.
class CComboBox2 : public CComboBox
{
	DECLARE_DYNAMIC(CComboBox2)
public:
	CComboBox2(bool bAutoDelete);
	virtual ~CComboBox2();

	/*
	 * Simple wrappers to control data access.
	 * Only valid if autodelete is set.
	 */
	CListData* GetData(int index) const;
	void SetData(int index, CListData* inData);

protected:
// Overrides
	//{{AFX_VIRTUAL(CComboBox2)
	//}}AFX_VIRTUAL
	bool m_bAutoDelete;

protected:
	//{{AFX_MSG(CComboBox2)
	afx_msg void OnDestroy();
	LRESULT OnResetContent(WPARAM, LPARAM);
	LRESULT OnDeleteString(WPARAM, LPARAM);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/// Combobox specifically for listing venues
class CVenueComboBox : public CComboBox2
{
	DECLARE_DYNAMIC(CVenueComboBox)
public:
	CVenueComboBox(bool bAutoDelete);
	virtual ~CVenueComboBox();

	/**
	 * If autodelete is set, the ARBConfigVenuePtr will be cached as data
	 * as CListPtrData<ARBConfigVenuePtr>.
	 * @param inVenues List of venues to add
	 * @param inSelectVenue Name of venue to initially select
	 * @param useLongName Show the long name
	 * @return Whether selection was set.
	 * @note This does not clear the existing contents.
	 */
	bool Initialize(
			ARBConfigVenueList const& inVenues,
			tstring const& inSelectVenue,
			bool useLongName = false);

protected:
	DECLARE_MESSAGE_MAP()
};
