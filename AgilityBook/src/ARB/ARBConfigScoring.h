#pragma once

/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @brief ARBConfigScoring and ARBConfigScoringList class.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-04 DRC Added support for NADAC bonus titling points.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-12-18 DRC Added a time fault multiplier.
 * @li 2004-11-15 DRC Added time fault computation on T+F.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-02-02 DRC Added VerifyEvent.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Added ValidTo, changed ValidFrom dtd name (was 'Date').
 * @li 2003-12-11 DRC Added ValidFrom, not fully implemented yet.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBConfigLifetimePoints.h"
#include "ARBConfigPlaceInfo.h"
#include "ARBConfigTitlePoints.h"
#include "ARBDate.h"
#include "ARBTypes.h"

/**
 * Scoring methods for an event.
 */
class ARBConfigScoring : public ARBBase
{
public:
	/**
	 * Types of scoring methods.
	 */
	typedef enum
	{
		eUnknown,			///< Unknown scoring method.
		eFaultsThenTime,	///< By faults then time.
		eFaults100ThenTime,	///< By faults then time (AKC).
		eFaults200ThenTime,	///< By faults then time (UKC).
		eOCScoreThenTime,	///< Open/Closing points then time.
		eScoreThenTime,		///< Points then time.
		eTimePlusFaults		///< Time plus faults.
	} ScoringStyle;

	/**
	 * Translate the enum to a string.
	 */
	static ARBString GetScoringStyleStr(ScoringStyle inStyle);

protected:
	ARBConfigScoring();
	ARBConfigScoring(ARBConfigScoring const& rhs);

public:
	~ARBConfigScoring();
	static ARBConfigScoringPtr New()
	{
		return ARBConfigScoringPtr(new ARBConfigScoring());
	}
	ARBConfigScoringPtr Clone() const
	{
		return ARBConfigScoringPtr(new ARBConfigScoring(*this));
	}

	ARBConfigScoring& operator=(ARBConfigScoring const& rhs);

	bool operator==(ARBConfigScoring const& rhs) const;
	bool operator!=(ARBConfigScoring const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual ARBString GetGenericName() const
	{
		return m_Division + _T(" ") + m_Level;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<ARBString>& ioStrings) const
	{
		return 0;
	}

	/**
	 * Load a scoring configuration.
	 * @pre inTree is the actual ARBConfigScoring element.
	 * @param inDivisions Configurate division used for verification.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfigDivisionList const& inDivisions,
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigOtherPoints element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Determine if this method is valid on the given date.
	 * @param inDate Date to check, if not valid, this method is valid.
	 */
	bool IsValidOn(ARBDate inDate) const
	{
		if (inDate.IsValid()
		&& ((m_ValidFrom.IsValid() && inDate < m_ValidFrom)
		|| (m_ValidTo.IsValid() && inDate > m_ValidTo)))
		{
			return false;
		}
		return true;
	}

	/*
	 * Getters/setters.
	 */
	ARBDate const& GetValidFrom() const
	{
		return m_ValidFrom;
	}
	void SetValidFrom(ARBDate const& inDate)
	{
		m_ValidFrom = inDate;
	}
	ARBDate const& GetValidTo() const
	{
		return m_ValidTo;
	}
	void SetValidTo(ARBDate const& inDate)
	{
		m_ValidTo = inDate;
	}
	ARBString const& GetDivision() const
	{
		return m_Division;
	}
	void SetDivision(ARBString const& inDiv)
	{
		m_Division = inDiv;
	}
	ARBString const& GetLevel() const
	{
		return m_Level;
	}
	void SetLevel(ARBString const& inLevel)
	{
		m_Level = inLevel;
	}
	ScoringStyle GetScoringStyle() const
	{
		return m_Style;
	}
	ARBString GetScoringStyleStr() const
	{
		return GetScoringStyleStr(m_Style);
	}
	void SetScoringStyle(ARBConfigScoring::ScoringStyle inStyle)
	{
		m_Style = inStyle;
		if (eOCScoreThenTime != m_Style && eScoreThenTime != m_Style)
			m_OpeningPts = m_ClosingPts = 0;
	}
	bool DropFractions() const ///< Only valid for F/T, T+F
	{
		return m_bDropFractions;
	}
	void SetDropFractions(bool inBool) ///< Only valid for F/T, T+F
	{
		m_bDropFractions = inBool;
	}
	bool QsMustBeClean() const ///< Only valid for T+F
	{
		return m_bCleanQ;
	}
	void SetQsMustBeClean(bool inBool) ///< Only valid for T+F
	{
		m_bCleanQ = inBool;
	}
	bool ComputeTimeFaultsUnder() const
	{
		return m_bTimeFaultsUnder;
	}
	void SetComputeTimeFaultsUnder(bool inBool)
	{
		m_bTimeFaultsUnder = inBool;
	}
	bool ComputeTimeFaultsOver() const
	{
		return m_bTimeFaultsOver;
	}
	void SetComputeTimeFaultsOver(bool inBool)
	{
		m_bTimeFaultsOver = inBool;
	}
	bool SubtractTimeFaultsFromScore() const
	{
		return m_bSubtractTimeFaults;
	}
	void SetSubtractTimeFaultsFromScore(bool inBool)
	{
		m_bSubtractTimeFaults = inBool;
	}
	short TimeFaultMultiplier() const
	{
		return m_TimeFaultMultiplier;
	}
	void SetTimeFaultMultiplier(short inMultiplier)
	{
		m_TimeFaultMultiplier = inMultiplier;
		if (0 >= m_TimeFaultMultiplier)
			m_TimeFaultMultiplier = 1;
	}
	short GetRequiredOpeningPoints() const ///< Only valid for point-based
	{
		return m_OpeningPts;
	}
	void SetRequiredOpeningPoints(short inPoints) ///< Only valid for point-based
	{
		m_OpeningPts = inPoints;
	}
	short GetRequiredClosingPoints() const ///< Only valid for point-based
	{
		return m_ClosingPts;
	}
	void SetRequiredClosingPoints(short inPoints) ///< Only valid for point-based
	{
		m_ClosingPts = inPoints;
	}
	ARBString const& GetNote() const
	{
		return m_Note;
	}
	void SetNote(ARBString const& inNote)
	{
		m_Note = inNote;
	}
	bool HasSuperQ() const
	{
		return m_bSuperQ;
	}
	void SetHasSuperQ(bool inBool)
	{
		m_bSuperQ = inBool;
	}
	bool HasSpeedPts() const
	{
		return m_bSpeedPts;
	}
	void SetHasSpeedPts(bool inBool)
	{
		m_bSpeedPts = inBool;
	}
	bool HasBonusPts() const
	{
		return m_bBonusPts;
	}
	void SetHasBonusPts(bool inBool)
	{
		m_bBonusPts = inBool;
	}
	ARBConfigPlaceInfoList const& GetPlaceInfo() const
	{
		return m_PlaceInfo;
	}
	ARBConfigPlaceInfoList& GetPlaceInfo()
	{
		return m_PlaceInfo;
	}
	ARBConfigTitlePointsList const& GetTitlePoints() const
	{
		return m_TitlePoints;
	}
	ARBConfigTitlePointsList& GetTitlePoints()
	{
		return m_TitlePoints;
	}
	ARBConfigLifetimePointsList const& GetLifetimePoints() const
	{
		return m_LifePoints;
	}
	ARBConfigLifetimePointsList& GetLifetimePoints()
	{
		return m_LifePoints;
	}

	/**
	 * Obsolete, used only for converting old files.
	 * This information is now contained in the venue (see ARBConfigMultiQ).
	 */
	bool ConvertDoubleQ() const
	{
		return m_bDoubleQ;
	}

private:
	ARBDate m_ValidFrom;
	ARBDate m_ValidTo;
	ARBString m_Division;
	ARBString m_Level;
	ScoringStyle m_Style;
	bool m_bDropFractions;
	bool m_bCleanQ;
	bool m_bTimeFaultsUnder;
	bool m_bTimeFaultsOver;
	bool m_bSubtractTimeFaults;
	short m_TimeFaultMultiplier;
	ARBString m_Note;
	short m_OpeningPts;
	short m_ClosingPts;
	bool m_bSuperQ;
	bool m_bDoubleQ;
	bool m_bSpeedPts;
	bool m_bBonusPts;
	ARBConfigPlaceInfoList m_PlaceInfo;
	ARBConfigTitlePointsList m_TitlePoints;
	ARBConfigLifetimePointsList m_LifePoints;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigScoring objects.
 */
class ARBConfigScoringList : public ARBVector<ARBConfigScoringPtr>
{
public:
	/**
	 * Load a scoring configuration.
	 * @pre inTree is the actual ARBConfigScoring element.
	 * @param inDivisions Configurate division used for verification.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfigDivisionList const& inDivisions,
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Find all the scoring methods that match.
	 * @param inDivision Division event is in.
	 * @param inLevel Level (NOT sublevel) event is in.
	 * @param inDate Date for matching methods.
	 * @param inTitlePoints Only return scoring methods that have title points.
	 * @param outList List of scoring methods found.
	 * @return Number of items found.
	 */
	size_t FindAllEvents(
			ARBString const& inDivision,
			ARBString const& inLevel,
			ARBDate const& inDate,
			bool inTitlePoints,
			ARBVector<ARBConfigScoringPtr>& outList) const;

	/**
	 * Find an event.
	 * @param inDivision Division event exists in.
	 * @param inLevel Level (NOT sublevel) event exists in.
	 * @param inDate Date for requested scoring.
	 * @param outEvent Pointer to object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindEvent(
			ARBString const& inDivision,
			ARBString const& inLevel,
			ARBDate const& inDate,
			ARBConfigScoringPtr* outEvent = NULL) const;

	/**
	 * Verify a scoring method exists.
	 * @param inDivision Division event is in.
	 * @param inLevel Level event is in.
	 * @param inDate Date for requested scoring.
	 * @return true if FindAllEvents() > 0.
	 */
	bool VerifyEvent(
			ARBString const& inDivision,
			ARBString const& inLevel,
			ARBDate const& inDate) const;

	/**
	 * Create a new scoring method.
	 * @return Pointer to a new method.
	 */
	ARBConfigScoringPtr AddScoring();
};
