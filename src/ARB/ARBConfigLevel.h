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
 * @brief Configuration class.
 * @author David Connet
 *
 * Revision History
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <string>
#include "ARBBase.h"
#include "ARBConfigSubLevel.h"
#include "ARBVector.h"
class ARBConfigEventList;
class ARBVersion;
class CElement;

class ARBConfigLevel : public ARBBase
{
public:
	ARBConfigLevel();
	ARBConfigLevel(const ARBConfigLevel& rhs);
	ARBConfigLevel& operator=(const ARBConfigLevel& rhs);
	bool operator==(const ARBConfigLevel& rhs) const;
	bool operator!=(const ARBConfigLevel& rhs) const;
	void clear();

	virtual std::string GetGenericName() const	{return GetName();}

	bool Load(
		const CElement& inTree,
		const ARBVersion& inVersion);
	bool Save(CElement& ioTree) const;

	std::string Update(int indent, const ARBConfigLevel* inLevelNew);

	const std::string& GetName() const;
	void SetName(const std::string& inName);
	const ARBConfigSubLevelList& GetSubLevels() const;
	ARBConfigSubLevelList& GetSubLevels();

private:
	~ARBConfigLevel();
	std::string m_Name;
	ARBConfigSubLevelList m_SubLevels;
};

inline const std::string& ARBConfigLevel::GetName() const
{
	return m_Name;
}

inline void ARBConfigLevel::SetName(const std::string& inName)
{
	m_Name = inName;
}

inline const ARBConfigSubLevelList& ARBConfigLevel::GetSubLevels() const
{
	return m_SubLevels;
}

inline ARBConfigSubLevelList& ARBConfigLevel::GetSubLevels()
{
	return m_SubLevels;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigLevelList : public ARBVectorLoad1<ARBConfigLevel>
{
public:
	bool operator==(const ARBConfigLevelList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBConfigLevelList& rhs) const
	{
		return !isEqual(rhs);
	}

	/**
	 * Verifies this is a valid level (not sublevel).
	 * Used to verify configuration stuff.
	 */
	bool VerifyLevel(
		const std::string& inName,
		bool inAllowWildCard = true) const;

	ARBConfigLevel* FindLevel(const std::string& inName);
	/**
	 * Find a level, only looks at leaf nodes
	 * (if there are sub-levels, the level name is ignored).
	 */
	bool FindTrueLevel(const std::string& inName) const;
	const ARBConfigLevel* FindSubLevel(const std::string& inName) const;
	ARBConfigLevel* AddLevel(const std::string& inName);
	ARBConfigLevel* AddLevel(ARBConfigLevel* inLevel);
	bool DeleteLevel(
		const std::string& inName,
		ARBConfigEventList& ioEvents);
	/**
	 * Delete a sublevel
	 * @param inName Name of sublevel to delete.
	 * @param outLevelModified Level containing the deleted sublevel has been renamed.
	 * @return Whether a sublevel was deleted.
	 * @post Deleting a sublevel may cause the parent level name to change.
	 */
	bool DeleteSubLevel(
		const std::string& inName,
		bool& outLevelModified);
};
