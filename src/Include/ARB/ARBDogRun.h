#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBDogRun and ARBDogRunList classes.
 * @author David Connet
 *
 * Revision History
 * 2016-01-06 Add support for named lifetime points.
 * 2015-05-19 Added GetName (generic name without date).
 * 2012-09-09 Added 'titlePts' to 'Placement'.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-10 Only sort runs one way, the UI handles everything else.
 * 2005-01-02 Added subnames to events.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-03-30 Added links.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBDogNotes.h"
#include "ARBDogReferenceRun.h"
#include "ARBDogRunOtherPoints.h"
#include "ARBDogRunPartner.h"
#include "ARBDogRunScoring.h"
#include "ARBTypes2.h"
#include "ARB_Q.h"
#include "LibwxARB.h"

#include "ARBCommon/ARBDate.h"
#include <set>


namespace dconSoft
{
namespace ARB
{

class ARB_API ARBDogRun : public ARBBase
{
protected:
	ARBDogRun();
	ARBDogRun(ARBDogRun const& rhs);
	ARBDogRun(ARBDogRun&& rhs);

public:
	~ARBDogRun();
	static ARBDogRunPtr New();
	ARBDogRunPtr Clone() const;

	ARBDogRun& operator=(ARBDogRun const& rhs);
	ARBDogRun& operator=(ARBDogRun&& rhs);

	bool operator==(ARBDogRun const& rhs) const;
	bool operator!=(ARBDogRun const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the name of this object.
	 * This is the generic name without a date.
	 * @return The name of this object.
	 */
	std::wstring GetName() const;

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	std::wstring GetGenericName() const override;

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const override;

	/**
	 * Load a run.
	 * @pre inTree is the actual ARBDogRun element.
	 * @param inConfig Configuration for looking up information.
	 * @param inClubs Clubs hosting trial, for looking up information.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
		ARBConfig const& inConfig,
		ARBDogClubList const& inClubs,
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @param pTrial Parent trial.
	 * @param inConfig Configuration.
	 * @return Success
	 * @post The ARBDogRun element will be created in ioTree.
	 */
	bool Save(ARBCommon::ElementNodePtr const& ioTree, ARBDogTrial const* pTrial, ARBConfig const& inConfig) const;

	/**
	 * Number of OtherPoint objects in use.
	 * Used to warning about impending configuration changes.
	 * @param inOther Name of item to look for.
	 * @return Number of objects, not points.
	 */
	int NumOtherPointsInUse(std::wstring const& inOther) const;

	/**
	 * Rename an OtherPoint, rename any dependent objects.
	 * @param inOldName OtherPoint name being renamed.
	 * @param inNewName New OtherPoint name.
	 * @return Number of items changed.
	 */
	int RenameOtherPoints(std::wstring const& inOldName, std::wstring const& inNewName);

	/**
	 * Delete an OtherPoint, remove any dependent objects.
	 * @param inName OtherPoint name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteOtherPoints(std::wstring const& inName);

	/**
	 * Get the number of Speed points earned in this run.
	 * @param inScoring Scoring method used.
	 * @return Number of Speed points earned.
	 */
	short GetSpeedPoints(ARBConfigScoringPtr const& inScoring) const;

	/**
	 * Get the number of title points earned in this run.
	 * @param inScoring Scoring method used.
	 * @param outClean Was this a clean run?
	 * @return Number of title points earned.
	 */
	double GetTitlePoints(ARBConfigScoringPtr const& inScoring, bool* outClean = nullptr) const;

	/**
	 * Get the number of lifetime points earned in this run.
	 * @param inScoring Scoring method used.
	 * @param inLifetimeName Name of Lifetime points to tally.
	 * @return Number of lifetime points earned.
	 */
	double GetLifetimePoints(ARBConfigScoringPtr const& inScoring, std::wstring const& inLifetimeName) const;

	/**
	 * Get the number of lifetime placement points earned in this run.
	 * @param inScoring Scoring method used.
	 * @return Number of placement points earned.
	 */
	double GetPlacementPoints(ARBConfigScoringPtr const& inScoring) const;

	/**
	 * Get the score for this run.
	 * @param inScoring Scoring method used.
	 * @return Total score for this run. The meaning of a score varies by event.
	 */
	double GetScore(ARBConfigScoringPtr const& inScoring) const;

	/*
	 * Getters/setters.
	 */
	size_t GetMultiQs(std::vector<ARBConfigMultiQPtr>& outMultiQs) const;
	void ClearMultiQs();
	void AddMultiQ(ARBConfigMultiQPtr const& inMultiQ);
	ARBCommon::ARBDate const& GetDate() const
	{
		return m_Date;
	}
	void SetDate(ARBCommon::ARBDate const& inDate)
	{
		m_Date = inDate;
	}
	ARBDogClubPtr GetClub() const
	{
		return m_Club;
	}
	void SetClub(ARBDogClubPtr club)
	{
		m_Club = club;
	}
	std::wstring const& GetDivision() const
	{
		return m_Division;
	}
	void SetDivision(std::wstring const& inDiv)
	{
		m_Division = inDiv;
	}
	std::wstring const& GetLevel() const
	{
		return m_Level;
	}
	void SetLevel(std::wstring const& inLevel)
	{
		m_Level = inLevel;
	}
	std::wstring const& GetEvent() const
	{
		return m_Event;
	}
	void SetEvent(std::wstring const& inEvent)
	{
		m_Event = inEvent;
	}
	std::wstring const& GetSubName() const
	{
		return m_SubName;
	}
	void SetSubName(std::wstring const& inSubName)
	{
		m_SubName = inSubName;
	}
	bool IsAtHome() const
	{
		return m_isAtHome;
	}
	void SetAtHome(bool value)
	{
		m_isAtHome = value;
	}
	std::wstring const& GetHeight() const
	{
		return m_Height;
	}
	void SetHeight(std::wstring const& inHeight)
	{
		m_Height = inHeight;
	}
	std::wstring const& GetConditions() const
	{
		return m_Conditions;
	}
	void SetConditions(std::wstring const& inConditions)
	{
		m_Conditions = inConditions;
	}
	std::wstring const& GetJudge() const
	{
		return m_Judge;
	}
	void SetJudge(std::wstring const& inJudge)
	{
		m_Judge = inJudge;
	}
	std::wstring const& GetHandler() const
	{
		return m_Handler;
	}
	void SetHandler(std::wstring const& inHandler)
	{
		m_Handler = inHandler;
	}
	ARBDogRunPartnerList const& GetPartners() const
	{
		return m_Partners;
	}
	ARBDogRunPartnerList& GetPartners()
	{
		return m_Partners;
	}
	ARBDogRunScoring const& GetScoring() const
	{
		return m_Scoring;
	}
	ARBDogRunScoring& GetScoring()
	{
		return m_Scoring;
	}
	ARB_Q GetQ() const
	{
		return m_Q;
	}
	void SetQ(ARB_Q inQ)
	{
		m_Q = inQ;
	}
	short GetPlace() const
	{
		return m_Place;
	}
	void SetPlace(short inPlace)
	{
		m_Place = inPlace;
	}
	short GetInClass() const
	{
		return m_InClass;
	}
	void SetInClass(short inInClass)
	{
		m_InClass = inInClass;
	}
	short GetDogsQd() const
	{
		return m_DogsQd;
	}
	void SetDogsQd(short inDogsQd)
	{
		m_DogsQd = inDogsQd;
	}
	ARBDogFaultList const& GetFaults() const
	{
		return m_Notes.GetFaults();
	}
	ARBDogFaultList& GetFaults()
	{
		return m_Notes.GetFaults();
	}
	std::wstring const& GetCRCD() const
	{
		return m_Notes.GetCRCD();
	}
	void SetCRCD(std::wstring const& inCRCD)
	{
		m_Notes.SetCRCD(inCRCD);
	}
	std::string const& GetCRCDRawMetaData() const
	{
		return m_Notes.GetCRCDRawMetaData();
	}
	ARBMetaDataPtr GetCRCDMetaData() const
	{
		return m_Notes.GetCRCDMetaData();
	}
	void SetCRCDMetaData(std::vector<unsigned char> const& inCRCDMeta)
	{
		m_Notes.SetCRCDMetaData(inCRCDMeta);
	}
	std::wstring const& GetNote() const
	{
		return m_Notes.GetNote();
	}
	void SetNote(std::wstring const& inNote)
	{
		m_Notes.SetNote(inNote);
	}
	ARBDogReferenceRunList const& GetReferenceRuns() const
	{
		return m_RefRuns;
	}
	ARBDogReferenceRunList& GetReferenceRuns()
	{
		return m_RefRuns;
	}
	ARBDogRunOtherPointsList const& GetOtherPoints() const
	{
		return m_OtherPoints;
	}
	ARBDogRunOtherPointsList& GetOtherPoints()
	{
		return m_OtherPoints;
	}
	size_t NumLinks() const
	{
		return m_Links.size();
	}
	size_t GetLinks(std::set<std::wstring>& outLinks) const;
	bool HasLink(std::wstring const& inLink) const;
	void AddLink(std::wstring const& inLink);
	void RemoveLink(std::wstring const& inLink);

private:
	std::set<ARBConfigMultiQPtr> m_pMultiQs; //< Not persisted.
	ARBCommon::ARBDate m_Date;
	ARBDogClubPtr m_Club;
	std::wstring m_Division;
	std::wstring m_Level;
	std::wstring m_Height;
	std::wstring m_Event;
	std::wstring m_SubName; //< Only used if the config supports it.
	bool m_isAtHome;
	std::wstring m_Conditions;
	std::wstring m_Judge;
	std::wstring m_Handler;
	ARBDogRunPartnerList m_Partners;
	ARBDogRunScoring m_Scoring;
	ARB_Q m_Q;
	short m_Place;
	short m_InClass;
	short m_DogsQd;
	ARBDogRunOtherPointsList m_OtherPoints;
	ARBDogNotes m_Notes;
	ARBDogReferenceRunList m_RefRuns;
	typedef std::set<std::wstring> ARBDogRunLinks;
	ARBDogRunLinks m_Links;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogRun objects.
 */
class ARB_API ARBDogRunList : public ARBVectorNoSave<ARBDogRunPtr>
{
public:
	/**
	 * Load a run.
	 * @pre inTree is the actual ARBDogRun element.
	 * @param inConfig Configuration for looking up information.
	 * @param inClubs Clubs hosting trial, for looking up information.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
		ARBConfig const& inConfig,
		ARBDogClubList const& inClubs,
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @param pTrial Parent trial.
	 * @param inConfig Configuration.
	 * @return Success
	 * @post The T element will be created in ioTree.
	 */
	bool Save(ARBCommon::ElementNodePtr const& ioTree, ARBDogTrial const* pTrial, ARBConfig const& inConfig) const;

	/**
	 * Sort the list by date in ascending order. The user interface handles
	 * displaying different sort orders.
	 */
	void sort();

	/**
	 * Get the earliest date in the list.
	 */
	ARBCommon::ARBDate GetStartDate() const;

	/**
	 * Get the latest date in the list.
	 */
	ARBCommon::ARBDate GetEndDate() const;

	/**
	 * Add a run.
	 * @param inRun Run to add.
	 * @return Whether the object was added.
	 */
	bool AddRun(ARBDogRunPtr const& inRun);

	/**
	 * Delete a run.
	 * @param inRun Object to delete.
	 * @return Whether run was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteRun(ARBDogRunPtr const& inRun);
};

} // namespace ARB
} // namespace dconSoft
