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
 * @li 2003-07-12 DRC Added as part of file version 5.
 */

#include <string>
#include "ARBBase.h"
#include "ARBVector.h"
class ARBVersion;
class CElement;

class ARBConfigTitlePoints : public ARBBase
{
public:
	ARBConfigTitlePoints();
	ARBConfigTitlePoints(short inPoints, short inFaults);
	ARBConfigTitlePoints(const ARBConfigTitlePoints& rhs);
	ARBConfigTitlePoints& operator=(const ARBConfigTitlePoints& rhs);
	bool operator==(const ARBConfigTitlePoints& rhs) const;
	bool operator!=(const ARBConfigTitlePoints& rhs) const;

	virtual std::string GetGenericName() const;
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const;

	bool Load(
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);
	bool Save(CElement& inTree) const;

	short GetPoints() const;
	void SetPoints(short inPoints);
	short GetFaults() const;
	/**
	 * If faults is set via this API, the caller MUST call
	 * ARBConfigTitlePointsList::sort.
	 */
	void SetFaults(short inFaults);

private:
	~ARBConfigTitlePoints();
	short m_Points;
	short m_Faults;
};

inline short ARBConfigTitlePoints::GetPoints() const
{
	return m_Points;
}

inline void ARBConfigTitlePoints::SetPoints(short inPoints)
{
	m_Points = inPoints;
}

inline short ARBConfigTitlePoints::GetFaults() const
{
	return m_Faults;
}

inline void ARBConfigTitlePoints::SetFaults(short inFaults)
{
	m_Faults = inFaults;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigTitlePointsList : public ARBVectorLoad1<ARBConfigTitlePoints>
{
public:
	bool operator==(const ARBConfigTitlePointsList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBConfigTitlePointsList& rhs) const
	{
		return !isEqual(rhs);
	}

	void sort();

	short GetTitlePoints(double inFaults) const;

	const ARBConfigTitlePoints* FindTitlePoints(short inFaults) const;
	ARBConfigTitlePoints* AddTitlePoints(short inPoints, short inFaults);
	bool DeleteTitlePoints(short inFaults);
};
