/*
 * Copyright © 2008 David Connet. All Rights Reserved.
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
 * @brief Test ARBConfigTitle and ARBConfigTitleList class.
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBConfigTitle.h"
#include "Element.h"

#if 0
class ARBConfigTitle : public ARBBase
{
	static ARBConfigTitlePtr New();
	ARBConfigTitlePtr Clone() const;
	ARBConfigTitle& operator=(ARBConfigTitle const& rhs);
	bool operator==(ARBConfigTitle const& rhs) const;
	bool operator!=(ARBConfigTitle const& rhs) const
	void clear();
	virtual tstring GetGenericName() const
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const
	bool Load(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);
	bool Save(ElementNodePtr ioTree) const;
	bool IsValidOn(ARBDate inDate) const
	tstring const& GetNiceName() const
	tstring GetCompleteName(
			short inInstance = 0,
			bool bShowInstance = false,
			bool bAbbrevFirst = true,
			bool bAddDates = false) const;
};

class ARBConfigTitleList : public ARBVector<ARBConfigTitlePtr>
{
	bool Load(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback,
			bool inCheckDups = false);
	void ReorderBy(ARBConfigTitleList const& inList);
	bool FindTitleCompleteName(
			tstring const& inName,
			short inInstance,
			bool bShowInstance,
			bool bAbbrevFirst = true,
			ARBConfigTitlePtr* outTitle = NULL) const;
	bool FindTitle(
			tstring const& inName,
			ARBConfigTitlePtr* outTitle = NULL) const;
	bool AddTitle(
			tstring const& inName,
			ARBConfigTitlePtr* outTitle = NULL);
	bool AddTitle(ARBConfigTitlePtr inTitle);
	bool DeleteTitle(tstring const& inName);
};
#endif

#pragma message PRAGMA_MESSAGE("TODO: Implement tests")
