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
#include "ARBVector.h"
class ARBVersion;
class CElement;

class ARBConfigOtherPoints : public ARBBase
{
public:
	typedef enum
	{
		eTallyAll,
		eTallyAllByEvent,
		eTallyLevel,
		eTallyLevelByEvent
	} eOtherPointsTally;

	ARBConfigOtherPoints();
	ARBConfigOtherPoints(const ARBConfigOtherPoints& rhs);
	ARBConfigOtherPoints& operator=(const ARBConfigOtherPoints& rhs);
	bool operator==(const ARBConfigOtherPoints& rhs) const;
	bool operator!=(const ARBConfigOtherPoints& rhs) const;
	void clear();

	virtual std::string GetGenericName() const	{return GetName();}
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const;

	bool Load(
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);
	bool Save(CElement& ioTree) const;

	const std::string& GetName() const;
	void SetName(const std::string& inName);
	const std::string& GetDescription() const;
	void SetDescription(const std::string& inDesc);
	eOtherPointsTally GetTally() const;
	void SetTally(eOtherPointsTally inTally);

private:
	~ARBConfigOtherPoints();
	std::string m_Name;
	eOtherPointsTally m_Tally;
	std::string m_Desc;
};

inline const std::string& ARBConfigOtherPoints::GetName() const
{
	return m_Name;
}

inline void ARBConfigOtherPoints::SetName(const std::string& inName)
{
	m_Name = inName;
}

inline const std::string& ARBConfigOtherPoints::GetDescription() const
{
	return m_Desc;
}

inline void ARBConfigOtherPoints::SetDescription(const std::string& inDesc)
{
	m_Desc = inDesc;
}

inline ARBConfigOtherPoints::eOtherPointsTally ARBConfigOtherPoints::GetTally() const
{
	return m_Tally;
}

inline void ARBConfigOtherPoints::SetTally(ARBConfigOtherPoints::eOtherPointsTally inTally)
{
	m_Tally = inTally;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigOtherPointsList : public ARBVectorLoad1<ARBConfigOtherPoints>
{
public:
	bool operator==(const ARBConfigOtherPointsList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBConfigOtherPointsList& rhs) const
	{
		return !isEqual(rhs);
	}

	bool VerifyOtherPoints(const std::string& inName) const;

	const ARBConfigOtherPoints* FindOtherPoints(const std::string& inName) const;
	ARBConfigOtherPoints* FindOtherPoints(const std::string& inName);

	ARBConfigOtherPoints* AddOtherPoints(ARBConfigOtherPoints* inOther);

	bool DeleteOtherPoints(const std::string& inName);
};
