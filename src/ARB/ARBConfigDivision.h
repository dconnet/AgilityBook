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
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <string>
#include "ARBBase.h"
#include "ARBConfigLevel.h"
#include "ARBConfigTitle.h"
#include "ARBVector.h"
class ARBConfigEventList;
class ARBConfigVenue;
class ARBVersion;
class CElement;

class ARBConfigDivision : public ARBBase
{
public:
	ARBConfigDivision();
	ARBConfigDivision(const ARBConfigDivision& rhs);
	ARBConfigDivision& operator=(const ARBConfigDivision& rhs);
	bool operator==(const ARBConfigDivision& rhs) const;
	bool operator!=(const ARBConfigDivision& rhs) const;
	void clear();

	virtual std::string GetGenericName() const	{return GetName();}
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const;

	bool Load(
		const CElement& inTree,
		const ARBVersion& inVersion);
	bool Save(CElement& ioTree) const;

	std::string Update(int indent, const ARBConfigDivision* inDiv);

	const std::string& GetName() const;
	void SetName(const std::string& inName);
	const ARBConfigLevelList& GetLevels() const;
	ARBConfigLevelList& GetLevels();
	const ARBConfigTitleList& GetTitles() const;
	ARBConfigTitleList& GetTitles();

private:
	~ARBConfigDivision();
	std::string m_Name;
	ARBConfigLevelList m_Levels;
	ARBConfigTitleList m_Titles;
};

inline const std::string& ARBConfigDivision::GetName() const
{
	return m_Name;
}

inline void ARBConfigDivision::SetName(const std::string& inName)
{
	m_Name = inName;
}

inline const ARBConfigLevelList& ARBConfigDivision::GetLevels() const
{
	return m_Levels;
}

inline ARBConfigLevelList& ARBConfigDivision::GetLevels()
{
	return m_Levels;
}

inline const ARBConfigTitleList& ARBConfigDivision::GetTitles() const
{
	return m_Titles;
}

inline ARBConfigTitleList& ARBConfigDivision::GetTitles()
{
	return m_Titles;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigDivisionList : public ARBVector<ARBConfigDivision>
{
public:
	bool Load(
		const CElement& tree,
		const ARBVersion& inVersion);

	bool operator==(const ARBConfigDivisionList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBConfigDivisionList& rhs) const
	{
		return !isEqual(rhs);
	}

	bool VerifyLevel(
		const std::string& inDiv,
		const std::string& inLevel) const;
	bool VerifyTitle(const std::string& inTitle) const;

	const ARBConfigDivision* FindDivision(const std::string& inDiv) const;
	ARBConfigDivision* FindDivision(const std::string& inDiv);
	ARBConfigDivision* AddDivision(const std::string& inDiv);
	ARBConfigDivision* AddDivision(ARBConfigDivision* inDiv);
	int DeleteDivision(
		const std::string& inDiv,
		ARBConfigEventList& ioEvents);

	const ARBConfigTitle* FindTitle(const std::string& inTitle) const;
};
