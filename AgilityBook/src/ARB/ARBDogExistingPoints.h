#pragma once

/*
 * Copyright � 2004 David Connet. All Rights Reserved.
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
 * @li 2003-02-02 DRC Created
 */

#include <string>
#include "ARBBase.h"
#include "ARBTypes.h"
#include "ARBVector.h"
class ARBConfig;
class ARBConfigVenue;
class ARBVersion;
class CElement;

class ARBDogExistingPoints : public ARBBase
{
public:
	ARBDogExistingPoints();
	ARBDogExistingPoints(const ARBDogExistingPoints& rhs);
	ARBDogExistingPoints& operator=(const ARBDogExistingPoints& rhs);
	bool operator==(const ARBDogExistingPoints& rhs) const;
	bool operator!=(const ARBDogExistingPoints& rhs) const;

	virtual std::string GetGenericName() const;
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const;

	bool Load(
		const ARBConfig& inConfig,
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);
	bool Save(CElement& ioTree) const;

	const std::string& GetOther() const;
	void SetOther(const std::string& inOther);
	const std::string& GetVenue() const;
	void SetVenue(const std::string& inVenue);
	const std::string& GetDivision() const;
	void SetDivision(const std::string& inDiv);
	const std::string& GetLevel() const;
	void SetLevel(const std::string& inLevel);
	const std::string& GetEvent() const;
	void SetEvent(const std::string& inEvent);
	short GetPoints() const;
	void SetPoints(short inPoints);

private:
	~ARBDogExistingPoints();
	std::string m_Other;
	std::string m_Venue;
	std::string m_Div;
	std::string m_Level;
	std::string m_Event;
	short m_Points;
};

inline const std::string& ARBDogExistingPoints::GetOther() const
{
	return m_Other;
}

inline void ARBDogExistingPoints::SetOther(const std::string& inOther)
{
	m_Other = inOther;
}

inline const std::string& ARBDogExistingPoints::GetVenue() const
{
	return m_Venue;
}

inline void ARBDogExistingPoints::SetVenue(const std::string& inVenue)
{
	m_Venue = inVenue;
}

inline const std::string& ARBDogExistingPoints::GetDivision() const
{
	return m_Div;
}

inline void ARBDogExistingPoints::SetDivision(const std::string& inDiv)
{
	m_Div = inDiv;
}

inline const std::string& ARBDogExistingPoints::GetLevel() const
{
	return m_Level;
}

inline void ARBDogExistingPoints::SetLevel(const std::string& inLevel)
{
	m_Level = inLevel;
}

inline const std::string& ARBDogExistingPoints::GetEvent() const
{
	return m_Event;
}

inline void ARBDogExistingPoints::SetEvent(const std::string& inEvent)
{
	m_Event = inEvent;
}

inline short ARBDogExistingPoints::GetPoints() const
{
	return m_Points;
}

inline void ARBDogExistingPoints::SetPoints(short inPoints)
{
	m_Points = inPoints;
}

/////////////////////////////////////////////////////////////////////////////

class ARBDogExistingPointsList : public ARBVectorLoad2<ARBDogExistingPoints>
{
public:
	bool operator==(const ARBDogExistingPointsList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBDogExistingPointsList& rhs) const
	{
		return !isEqual(rhs);
	}

	void sort();

	long ExistingPoints(
		const std::string& inOther,
		const std::string& inVenue,
		const std::string& inDiv,
		const std::string& inLevel,
		const std::string& inEvent) const;

	int NumExistingPointsInVenue(const std::string& inVenue) const;
	int RenameVenue(
		const std::string& inOldVenue,
		const std::string& inNewVenue);
	int DeleteVenue(const std::string& inVenue);
	int NumExistingPointsInDivision(
		const std::string& inVenue,
		const std::string& inDiv) const;
	int RenameDivision(
		const std::string& inVenue,
		const std::string& inOldDiv,
		const std::string& inNewDiv);
	int DeleteDivision(
		const std::string& inVenue,
		const std::string& inDiv);

	int NumOtherPointsInUse(const std::string& inOther) const;
	int RenameOtherPoints(
		const std::string& inOldOther,
		const std::string& inNewOther);
	int DeleteOtherPoints(const std::string& inOther);

	ARBDogExistingPoints* AddExistingPoints(ARBDogExistingPoints* inExistingPoints);
};
