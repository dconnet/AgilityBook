#pragma once

/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
#include "ARBVector.h"
class ARBVersion;
class CElement;

class ARBConfigSubLevel : public ARBBase
{
public:
	ARBConfigSubLevel();
	ARBConfigSubLevel(const ARBConfigSubLevel& rhs);
	ARBConfigSubLevel& operator=(const ARBConfigSubLevel& rhs);
	bool operator==(const ARBConfigSubLevel& rhs) const;
	bool operator!=(const ARBConfigSubLevel& rhs) const;

	virtual std::string GetGenericName() const	{return GetName();}
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const;

	bool Load(
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);
	bool Save(CElement& ioTree) const;

	const std::string& GetName() const;
	void SetName(const std::string& inName);

private:
	~ARBConfigSubLevel();
	std::string m_Name;
};

inline const std::string& ARBConfigSubLevel::GetName() const
{
	return m_Name;
}

inline void ARBConfigSubLevel::SetName(const std::string& inName)
{
	m_Name = inName;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigSubLevelList : public ARBVectorLoad1<ARBConfigSubLevel>
{
public:
	bool operator==(const ARBConfigSubLevelList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBConfigSubLevelList& rhs) const
	{
		return !isEqual(rhs);
	}

	bool FindSubLevel(const std::string& inName) const;
	ARBConfigSubLevel* AddSubLevel(const std::string& inName);
	bool DeleteSubLevel(const std::string& inName);
};
