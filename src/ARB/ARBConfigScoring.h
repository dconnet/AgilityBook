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
 * @li 2003-12-27 DRC Added ValidTo, changed ValidFrom dtd name (was 'Date').
 * @li 2003-12-11 DRC Added ValidFrom, not fully implemented yet.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <string>
#include <vector>
#include "ARBBase.h"
#include "ARBConfigDivision.h"
#include "ARBConfigTitlePoints.h"
#include "ARBDate.h"
#include "ARBVector.h"
class ARBConfigEvent;
class ARBVersion;
class CElement;

class ARBConfigScoring : public ARBBase
{
public:
	typedef enum
	{
		eUnknown,
		eFaultsThenTime,
		eFaults100ThenTime,
		eFaults200ThenTime,
		eOCScoreThenTime,
		eScoreThenTime,
		eTimePlusFaults
	} ScoringStyle;
	static std::string GetScoringStyleStr(ScoringStyle inStyle);

	ARBConfigScoring();
	ARBConfigScoring(const ARBConfigScoring& rhs);
	ARBConfigScoring& operator=(const ARBConfigScoring& rhs);
	bool operator==(const ARBConfigScoring& rhs) const;
	bool operator!=(const ARBConfigScoring& rhs) const;

	virtual std::string GetGenericName() const;
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const;

	bool Load(
		const ARBConfigDivisionList& inDivisions,
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);
	bool Save(CElement& ioTree) const;

	const ARBDate& GetValidFrom() const;
	void SetValidFrom(const ARBDate& inDate);
	const ARBDate& GetValidTo() const;
	void SetValidTo(const ARBDate& inDate);
	std::string GetValidDateString() const;
	const std::string& GetDivision() const;
	void SetDivision(const std::string& inDiv);
	const std::string& GetLevel() const;
	void SetLevel(const std::string& inLevel);
	ScoringStyle GetScoringStyle() const;
	std::string GetScoringStyleStr() const;
	void SetScoringStyle(ARBConfigScoring::ScoringStyle inStyle);
	bool DropFractions() const;
	void SetDropFractions(bool inBool);
	const std::string& GetNote() const;
	void SetNote(const std::string& inNote);
	short GetRequiredOpeningPoints() const;
	void SetRequiredOpeningPoints(short inPoints);
	short GetRequiredClosingPoints() const;
	void SetRequiredClosingPoints(short inPoints);
	bool HasSuperQ() const;
	void SetHasSuperQ(bool inBool);
	bool HasDoubleQ() const;
	void SetHasDoubleQ(bool inBool);
	bool HasMachPts() const;
	void SetHasMachPts(bool inBool);
	const ARBConfigTitlePointsList& GetTitlePoints() const;
	ARBConfigTitlePointsList& GetTitlePoints();

private:
	~ARBConfigScoring();
	ARBDate m_ValidFrom;
	ARBDate m_ValidTo;
	std::string m_Division;
	std::string m_Level;
	ScoringStyle m_Style;
	bool m_bDropFractions;
	std::string m_Note;
	short m_OpeningPts;
	short m_ClosingPts;
	bool m_bSuperQ;
	bool m_bDoubleQ;
	bool m_bMachPts;
	ARBConfigTitlePointsList m_TitlePoints;
};

inline const ARBDate& ARBConfigScoring::GetValidFrom() const
{
	return m_ValidFrom;
}

inline void ARBConfigScoring::SetValidFrom(const ARBDate& inDate)
{
	m_ValidFrom = inDate;
}

inline const ARBDate& ARBConfigScoring::GetValidTo() const
{
	return m_ValidTo;
}

inline void ARBConfigScoring::SetValidTo(const ARBDate& inDate)
{
	m_ValidTo = inDate;
}

inline const std::string& ARBConfigScoring::GetDivision() const
{
	return m_Division;
}

inline void ARBConfigScoring::SetDivision(const std::string& inDiv)
{
	m_Division = inDiv;
}

inline const std::string& ARBConfigScoring::GetLevel() const
{
	return m_Level;
}

inline void ARBConfigScoring::SetLevel(const std::string& inLevel)
{
	m_Level = inLevel;
}

inline ARBConfigScoring::ScoringStyle ARBConfigScoring::GetScoringStyle() const
{
	return m_Style;
}

inline void ARBConfigScoring::SetScoringStyle(ARBConfigScoring::ScoringStyle inStyle)
{
	m_Style = inStyle;
	if (eOCScoreThenTime != m_Style && eScoreThenTime != m_Style)
		m_OpeningPts = m_ClosingPts = 0;
}

inline bool ARBConfigScoring::DropFractions() const
{
	return m_bDropFractions;
}

inline void ARBConfigScoring::SetDropFractions(bool inBool)
{
	m_bDropFractions = inBool;
}

inline const std::string& ARBConfigScoring::GetNote() const
{
	return m_Note;
}

inline void ARBConfigScoring::SetNote(const std::string& inNote)
{
	m_Note = inNote;
}

inline short ARBConfigScoring::GetRequiredOpeningPoints() const
{
	return m_OpeningPts;
}

inline void ARBConfigScoring::SetRequiredOpeningPoints(short inPoints)
{
	m_OpeningPts = inPoints;
}

inline short ARBConfigScoring::GetRequiredClosingPoints() const
{
	return m_ClosingPts;
}

inline void ARBConfigScoring::SetRequiredClosingPoints(short inPoints)
{
	m_ClosingPts = inPoints;
}

inline bool ARBConfigScoring::HasSuperQ() const
{
	return m_bSuperQ;
}

inline void ARBConfigScoring::SetHasSuperQ(bool inBool)
{
	m_bSuperQ = inBool;
}

inline bool ARBConfigScoring::HasDoubleQ() const
{
	return m_bDoubleQ;
}

inline void ARBConfigScoring::SetHasDoubleQ(bool inBool)
{
	m_bDoubleQ = inBool;
}

inline bool ARBConfigScoring::HasMachPts() const
{
	return m_bMachPts;
}

inline void ARBConfigScoring::SetHasMachPts(bool inBool)
{
	m_bMachPts = inBool;
}

inline const ARBConfigTitlePointsList& ARBConfigScoring::GetTitlePoints() const
{
	return m_TitlePoints;
}

inline ARBConfigTitlePointsList& ARBConfigScoring::GetTitlePoints()
{
	return m_TitlePoints;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigScoringList : public ARBVector<ARBConfigScoring>
{
public:
	bool Load(
		const ARBConfigDivisionList& inDivisions,
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	bool operator==(const ARBConfigScoringList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBConfigScoringList& rhs) const
	{
		return !isEqual(rhs);
	}

	/**
	 * @param inDivision Division name.
	 * @param inLevel Level (NOT sublevel) name.
	 * @param inTitlePoints Only look for events that have titling points.
	 * @param outList List of all found events.
	 */
	size_t FindAllEvents(
		const std::string& inDivision,
		const std::string& inLevel,
		bool inTitlePoints,
		std::vector<const ARBConfigScoring*>& outList) const;

	/**
	 * @param inDivision Division name.
	 * @param inLevel Level (NOT sublevel) name.
	 * @param inDate Date for requested scoring.
	 */
	const ARBConfigScoring* FindEvent(
		const std::string& inDivision,
		const std::string& inLevel,
		const ARBDate& inDate) const;

	ARBConfigScoring* AddScoring();
};
