#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBConfigScoring and ARBConfigScoringList class.
 * @author David Connet
 *
 * Revision History
 * 2011-07-31 Allow a time fault multipler of 0.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-04 Added support for NADAC bonus titling points.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-12-18 Added a time fault multiplier.
 * 2004-11-15 Added time fault computation on T+F.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-02-02 Added VerifyEvent.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-12-27 Added ValidTo, changed ValidFrom dtd name (was 'Date').
 * 2003-12-11 Added ValidFrom, not fully implemented yet.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBConfigLifetimePoints.h"
#include "ARBConfigPlaceInfo.h"
#include "ARBConfigTitlePoints.h"
#include "ARBTypes2.h"
#include "ARB_Q.h"
#include "LibwxARB.h"

#include "ARBCommon/ARBDate.h"


namespace dconSoft
{
namespace ARB
{

/**
 * Types of scoring methods.
 * Note: If a new style is added, update CDlgConfigEventMethod.
 */
enum class ARBScoringStyle
{
	Unknown,           ///< Unknown scoring method.
	FaultsThenTime,    ///< By faults then time.
	Faults100ThenTime, ///< By faults then time (AKC).
	Faults200ThenTime, ///< By faults then time (UKC).
	OCScoreThenTime,   ///< Open/Closing points then time.
	ScoreThenTime,     ///< Points then time.
	TimePlusFaults,    ///< Time plus faults.
	TimeNoPlaces,      ///< Time, no placement (AKC FCAT).
	PassFail,          ///< Simple pass/fail (AKC Fetch).
};


/**
 * Scoring methods for an event.
 */
class ARB_API ARBConfigScoring : public ARBBase
{
public:
	/**
	 * Translate the enum to a string.
	 */
	static wxString GetScoringStyleStr(ARBScoringStyle inStyle);

protected:
	ARBConfigScoring();
	ARBConfigScoring(ARBConfigScoring const& rhs);
	ARBConfigScoring(ARBConfigScoring&& rhs);

public:
	~ARBConfigScoring();
	static ARBConfigScoringPtr New();
	ARBConfigScoringPtr Clone() const;

	ARBConfigScoring& operator=(ARBConfigScoring const& rhs);
	ARBConfigScoring& operator=(ARBConfigScoring&& rhs);

	bool operator==(ARBConfigScoring const& rhs) const;
	bool operator!=(ARBConfigScoring const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	wxString GetGenericName() const override
	{
		return m_Division + L" " + m_Level;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<wxString>& ioStrings) const override
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
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigOtherPoints element will be created in ioTree.
	 */
	bool Save(ARBCommon::ElementNodePtr const& ioTree) const;

	/**
	 * Determine if this method is valid on the given date.
	 * @param inDate Date to check, if not valid, this method is valid.
	 */
	bool IsValidOn(ARBCommon::ARBDate inDate) const
	{
		if (inDate.IsValid()
			&& ((m_ValidFrom.IsValid() && inDate < m_ValidFrom) || (m_ValidTo.IsValid() && inDate > m_ValidTo)))
		{
			return false;
		}
		return true;
	}

	/**
	 * Does this scoring support this type of Q?
	 * @param q Q to check
	 * @return Supported
	 */
	bool Supports(Q q) const;

	/*
	 * Getters/setters.
	 */
	ARBCommon::ARBDate const& GetValidFrom() const
	{
		return m_ValidFrom;
	}
	void SetValidFrom(ARBCommon::ARBDate const& inDate)
	{
		m_ValidFrom = inDate;
	}
	ARBCommon::ARBDate const& GetValidTo() const
	{
		return m_ValidTo;
	}
	void SetValidTo(ARBCommon::ARBDate const& inDate)
	{
		m_ValidTo = inDate;
	}
	wxString const& GetDivision() const
	{
		return m_Division;
	}
	void SetDivision(wxString const& inDiv)
	{
		m_Division = inDiv;
	}
	wxString const& GetLevel() const
	{
		return m_Level;
	}
	void SetLevel(wxString const& inLevel)
	{
		m_Level = inLevel;
	}
	ARBScoringStyle GetScoringStyle() const
	{
		return m_Style;
	}
	wxString GetScoringStyleStr() const
	{
		return GetScoringStyleStr(m_Style);
	}
	void SetScoringStyle(ARBScoringStyle inStyle)
	{
		m_Style = inStyle;
		if (ARBScoringStyle::OCScoreThenTime != m_Style && ARBScoringStyle::ScoreThenTime != m_Style)
			m_OpeningPts = m_ClosingPts = 0;
	}
	bool HasTable() const
	{
		return m_bTable;
	}
	void SetHasTable(bool inBool)
	{
		m_bTable = inBool;
	}
	bool HasSubNames() const
	{
		return m_bHasSubNames;
	}
	void SetHasSubNames(bool inHas)
	{
		m_bHasSubNames = inHas;
	}
	size_t GetSubNames(std::set<wxString>& outNames, bool bClear = true) const;
	void SetSubNames(std::set<wxString> const& inNames);
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
	bool ComputeTitlingPointsRawFaults() const
	{
		return m_bTitlingPointsRawFaults;
	}
	void SetComputeTitlingPointsRawFaults(bool inBool)
	{
		m_bTitlingPointsRawFaults = inBool;
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
		if (0 > m_TimeFaultMultiplier)
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
	wxString const& GetNote() const
	{
		return m_Note;
	}
	void SetNote(wxString const& inNote)
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
	bool HasFEO() const
	{
		return m_bFEO;
	}
	void SetHasFEO(bool inBool)
	{
		m_bFEO = inBool;
	}
	bool HasSpeedPts() const
	{
		return m_bSpeedPts;
	}
	void SetHasSpeedPts(bool inBool)
	{
		m_bSpeedPts = inBool;
	}
	bool HasBonusTitlePts() const
	{
		return m_bBonusTitlePts;
	}
	void SetHasBonusTitlePts(bool inBool)
	{
		m_bBonusTitlePts = inBool;
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
	ARBConfigPlaceInfoList const& GetPlacements() const
	{
		return m_Placements;
	}
	ARBConfigPlaceInfoList& GetPlacements()
	{
		return m_Placements;
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
	ARBCommon::ARBDate m_ValidFrom;
	ARBCommon::ARBDate m_ValidTo;
	wxString m_Division;
	wxString m_Level;
	ARBScoringStyle m_Style;
	bool m_bTable;
	bool m_bHasSubNames;
	std::set<wxString> m_SubNames;
	bool m_bDropFractions;
	bool m_bCleanQ;
	bool m_bTimeFaultsUnder;
	bool m_bTimeFaultsOver;
	bool m_bTitlingPointsRawFaults; // On T+F runs, use raw faults for titling
	bool m_bSubtractTimeFaults;
	short m_TimeFaultMultiplier;
	wxString m_Note;
	short m_OpeningPts;
	short m_ClosingPts;
	bool m_bSuperQ;
	bool m_bFEO;
	bool m_bDoubleQ;
	bool m_bSpeedPts;
	bool m_bBonusTitlePts;
	ARBConfigPlaceInfoList m_PlaceInfo; ///< Used for speed points
	ARBConfigTitlePointsList m_TitlePoints;
	ARBConfigLifetimePointsList m_LifePoints;
	ARBConfigPlaceInfoList m_Placements; ///< Used for place points
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigScoring objects.
 */
class ARB_API ARBConfigScoringList : public ARBVector<ARBConfigScoringPtr>
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
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
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
		wxString const& inDivision,
		wxString const& inLevel,
		ARBCommon::ARBDate const& inDate,
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
		wxString const& inDivision,
		wxString const& inLevel,
		ARBCommon::ARBDate const& inDate,
		ARBConfigScoringPtr* outEvent = nullptr) const;

	/**
	 * Verify a scoring method exists.
	 * @param inDivision Division event is in.
	 * @param inLevel Level event is in.
	 * @param inDate Date for requested scoring.
	 * @return true if FindAllEvents() > 0.
	 */
	bool VerifyEvent(wxString const& inDivision, wxString const& inLevel, ARBCommon::ARBDate const& inDate) const;

	/**
	 * Create a new scoring method.
	 * @return Pointer to a new method.
	 */
	ARBConfigScoringPtr AddScoring();
};

} // namespace ARB
} // namespace dconSoft
