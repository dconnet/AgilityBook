#pragma once

/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 */

#include <string>
#include "ARBBase.h"
#include "ARBVector.h"
class ARBConfig;
class CElement;

class ARBDogRunOtherPoints : public ARBBase
{
public:
	ARBDogRunOtherPoints();
	ARBDogRunOtherPoints(const ARBDogRunOtherPoints& rhs);
	ARBDogRunOtherPoints& operator=(const ARBDogRunOtherPoints& rhs);
	bool operator==(const ARBDogRunOtherPoints& rhs) const;
	bool operator!=(const ARBDogRunOtherPoints& rhs) const;
	void clear();

	virtual std::string GetGenericName() const	{return GetName();}

	bool Load(
		const ARBConfig& inConfig,
		const CElement& inTree,
		int inVersion);
	bool Save(CElement& ioTree) const;

	const std::string& GetName() const;
	void SetName(const std::string& inName);
	short GetPoints() const;
	void SetPoints(short inPts);

private:
	~ARBDogRunOtherPoints();
	std::string m_Name;
	short m_Points;
};

inline const std::string& ARBDogRunOtherPoints::GetName() const
{
	return m_Name;
}

inline void ARBDogRunOtherPoints::SetName(const std::string& inName)
{
	m_Name = inName;
}

inline short ARBDogRunOtherPoints::GetPoints() const
{
	return m_Points;
}

inline void ARBDogRunOtherPoints::SetPoints(short inPts)
{
	m_Points = inPts;
}

/////////////////////////////////////////////////////////////////////////////

class ARBDogRunOtherPointsList : public ARBVectorLoad2<ARBDogRunOtherPoints>
{
public:
	bool operator==(const ARBDogRunOtherPointsList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBDogRunOtherPointsList& rhs) const
	{
		return !isEqual(rhs);
	}

	ARBDogRunOtherPoints* AddOtherPoints(const ARBDogRunOtherPoints* inOther);
};
