#pragma once

/*
 * Copyright © 2003-2004 David Connet. All Rights Reserved.
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
 * @brief ARBInfo class
 * @author David Connet
 *
 * Revision History
 * @li 2004-11-18 DRC Added Club/Location info.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-07 DRC Created
 */

#include "ARBInfoClub.h"
#include "ARBInfoJudge.h"
#include "ARBInfoLocation.h"
class ARBErrorCallback;
class ARBVersion;
class Element;

/**
 * General information to be tracked.
 *
 * This class is contained in ARBAgilityRecordBook.
 * Currently the only stuff here is judge info.
 */
class ARBInfo
{
public:
	ARBInfo();
	ARBInfo(ARBInfo const& rhs);
	~ARBInfo();
	ARBInfo& operator=(ARBInfo const& rhs);
	bool operator==(ARBInfo const& rhs) const;
	bool operator!=(ARBInfo const& rhs) const;

	/**
	 * Reset the contents of this object and all sub-objects.
	 */
	void clear();

	/**
	 * Load the info.
	 * @pre inTree is the actual ARBInfo element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBInfo element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Getters.
	 */
	ARBInfoClubList const& GetClubInfo() const;
	ARBInfoClubList& GetClubInfo();
	ARBInfoJudgeList const& GetJudgeInfo() const;
	ARBInfoJudgeList& GetJudgeInfo();
	ARBInfoLocationList const& GetLocationInfo() const;
	ARBInfoLocationList& GetLocationInfo();

private:
	ARBInfoClubList m_ClubInfo;
	ARBInfoJudgeList m_JudgeInfo;
	ARBInfoLocationList m_LocationInfo;
};

inline ARBInfoClubList const& ARBInfo::GetClubInfo() const
{
	return m_ClubInfo;
}

inline ARBInfoClubList& ARBInfo::GetClubInfo()
{
	return m_ClubInfo;
}

inline ARBInfoJudgeList const& ARBInfo::GetJudgeInfo() const
{
	return m_JudgeInfo;
}

inline ARBInfoJudgeList& ARBInfo::GetJudgeInfo()
{
	return m_JudgeInfo;
}

inline ARBInfoLocationList const& ARBInfo::GetLocationInfo() const
{
	return m_LocationInfo;
}

inline ARBInfoLocationList& ARBInfo::GetLocationInfo()
{
	return m_LocationInfo;
}
