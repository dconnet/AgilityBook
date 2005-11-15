#pragma once

/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @brief ARBDogRun and ARBDogRunList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-10 DRC Only sort runs one way, the UI handles everything else.
 * @li 2005-01-02 DRC Added subnames to events.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-03-30 DRC Added links.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <set>
#include "ARBBase.h"
#include "ARBDate.h"
#include "ARBDogNotes.h"
#include "ARBDogReferenceRun.h"
#include "ARBDogRunOtherPoints.h"
#include "ARBDogRunPartner.h"
#include "ARBDogRunScoring.h"
#include "ARBTypes.h"
#include "ARBVector.h"
class ARBConfig;
class ARBConfigMultiQ;
class ARBConfigScoring;
class ARBDogClubList;
class ARBDogTrial;
class ARBErrorCallback;
class ARBVersion;
class Element;

class ARBDogRun : public ARBBase
{
public:
	ARBDogRun();
	ARBDogRun(ARBDogRun const& rhs);
	ARBDogRun& operator=(ARBDogRun const& rhs);
	bool operator==(ARBDogRun const& rhs) const;
	bool operator!=(ARBDogRun const& rhs) const;

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual ARBString GetGenericName() const;

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<ARBString>& ioStrings) const;

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
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogRun element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Number of OtherPoint objects in use.
	 * Used to warning about impending configuration changes.
	 * @param inOther Name of item to look for.
	 * @return Number of objects, not points.
	 */
	int NumOtherPointsInUse(ARBString const& inOther) const;

	/**
	 * Rename an OtherPoint, rename any dependent objects.
	 * @param inOldName OtherPoint name being renamed.
	 * @param inNewName New OtherPoint name.
	 * @return Number of items changed.
	 */
	int RenameOtherPoints(
			ARBString const& inOldName,
			ARBString const& inNewName);

	/**
	 * Delete an OtherPoint, remove any dependent objects.
	 * @param inName OtherPoint name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteOtherPoints(ARBString const& inName);

	/**
	 * Get the number of Speed points earned in this run.
	 * @param inScoring Scoring method used.
	 * @return Number of Speed points earned.
	 */
	short GetSpeedPoints(ARBConfigScoring const* inScoring) const;

	/**
	 * Get the number of title points earned in this run.
	 * @param inScoring Scoring method used.
	 * @param outClean Was this a clean run?
	 * @param outLifeTime Lifetime points earned in this run.
	 * @return Number of title points earned.
	 */
	double GetTitlePoints(
			ARBConfigScoring const* inScoring,
			bool* outClean = NULL,
			double* outLifeTime = NULL) const;

	/**
	 * Get the score for this run.
	 * @param inScoring Scoring method used.
	 * @return Total score for this run. The meaning of a score varies by event.
	 */
	double GetScore(ARBConfigScoring const* inScoring) const;

	/*
	 * Getters/setters.
	 */
	ARBConfigMultiQ* GetMultiQ() const; ///< Note, this ptr is not AddRef'd
	void SetMultiQ(ARBConfigMultiQ* inMultiQ);
	ARBDate const& GetDate() const;
	void SetDate(ARBDate const& inDate);
	ARBString const& GetDivision() const;
	void SetDivision(ARBString const& inDiv);
	ARBString const& GetLevel() const;
	void SetLevel(ARBString const& inLevel);
	ARBString const& GetEvent() const;
	void SetEvent(ARBString const& inEvent);
	ARBString const& GetSubName() const;
	void SetSubName(ARBString const& inSubName);
	ARBString const& GetHeight() const;
	void SetHeight(ARBString const& inHeight);
	ARBString const& GetConditions() const;
	void SetConditions(ARBString const& inConditions);
	ARBString const& GetJudge() const;
	void SetJudge(ARBString const& inJudge);
	ARBString const& GetHandler() const;
	void SetHandler(ARBString const& inHandler);
	ARBDogRunPartnerList const& GetPartners() const;
	ARBDogRunPartnerList& GetPartners();
	ARBDogRunScoring const& GetScoring() const;
	ARBDogRunScoring& GetScoring();
	ARB_Q GetQ() const;
	void SetQ(ARB_Q inQ);
	short GetPlace() const;
	void SetPlace(short inPlace);
	short GetInClass() const;
	void SetInClass(short inInClass);
	short GetDogsQd() const;
	void SetDogsQd(short inDogsQd);
	ARBDogRunOtherPointsList const& GetOtherPoints() const;
	ARBDogRunOtherPointsList& GetOtherPoints();
	ARBDogFaultList const& GetFaults() const;
	ARBDogFaultList& GetFaults();
	ARBString const& GetCRCD() const;
	void SetCRCD(ARBString const& inCRCD);
	ARBString const& GetCRCDMetaFile() const;
	void SetCRCDMetaFile(ARBString const& inCRCDMeta);
	ARBString const& GetNote() const;
	void SetNote(ARBString const& inNote);
	ARBDogReferenceRunList const& GetReferenceRuns() const;
	ARBDogReferenceRunList& GetReferenceRuns();
	size_t NumLinks() const;
	size_t GetLinks(std::set<ARBString>& outLinks) const;
	bool HasLink(ARBString const& inLink) const;
	void AddLink(ARBString const& inLink);
	void RemoveLink(ARBString const& inLink);

private:
	~ARBDogRun();
	ARBConfigMultiQ* m_pMultiQ; //< Not persisted.
	ARBDate m_Date;
	ARBString m_Division;
	ARBString m_Level;
	ARBString m_Height;
	ARBString m_Event;
	ARBString m_SubName; //< Only used if the config supports it.
	ARBString m_Conditions;
	ARBString m_Judge;
	ARBString m_Handler;
	ARBDogRunPartnerList m_Partners;
	ARBDogRunScoring m_Scoring;
	ARB_Q m_Q;
	short m_Place;
	short m_InClass;
	short m_DogsQd;
	ARBDogRunOtherPointsList m_OtherPoints;
	ARBDogNotes m_Notes;
	ARBDogReferenceRunList m_RefRuns;
	typedef std::set<ARBString> ARBDogRunLinks;
	ARBDogRunLinks m_Links;
};

inline ARBConfigMultiQ* ARBDogRun::GetMultiQ() const
{
	return m_pMultiQ;
}

inline ARBDate const& ARBDogRun::GetDate() const
{
	return m_Date;
}

inline void ARBDogRun::SetDate(ARBDate const& inDate)
{
	m_Date = inDate;
}

inline ARBString const& ARBDogRun::GetDivision() const
{
	return m_Division;
}

inline void ARBDogRun::SetDivision(ARBString const& inDiv)
{
	m_Division = inDiv;
}

inline ARBString const& ARBDogRun::GetLevel() const
{
	return m_Level;
}

inline void ARBDogRun::SetLevel(ARBString const& inLevel)
{
	m_Level = inLevel;
}

inline ARBString const& ARBDogRun::GetEvent() const
{
	return m_Event;
}

inline void ARBDogRun::SetEvent(ARBString const& inEvent)
{
	m_Event = inEvent;
}

inline ARBString const& ARBDogRun::GetSubName() const
{
	return m_SubName;
}

inline void ARBDogRun::SetSubName(ARBString const& inSubName)
{
	m_SubName = inSubName;
}

inline ARBString const& ARBDogRun::GetHeight() const
{
	return m_Height;
}

inline void ARBDogRun::SetHeight(ARBString const& inHeight)
{
	m_Height = inHeight;
}

inline ARBString const& ARBDogRun::GetConditions() const
{
	return m_Conditions;
}

inline void ARBDogRun::SetConditions(ARBString const& inConditions)
{
	m_Conditions = inConditions;
}

inline ARBString const& ARBDogRun::GetJudge() const
{
	return m_Judge;
}

inline void ARBDogRun::SetJudge(ARBString const& inJudge)
{
	m_Judge = inJudge;
}

inline ARBString const& ARBDogRun::GetHandler() const
{
	return m_Handler;
}

inline void ARBDogRun::SetHandler(ARBString const& inHandler)
{
	m_Handler = inHandler;
}

inline ARBDogRunPartnerList const& ARBDogRun::GetPartners() const
{
	return m_Partners;
}

inline ARBDogRunPartnerList& ARBDogRun::GetPartners()
{
	return m_Partners;
}

inline ARBDogRunScoring const& ARBDogRun::GetScoring() const
{
	return m_Scoring;
}

inline ARBDogRunScoring& ARBDogRun::GetScoring()
{
	return m_Scoring;
}

inline ARB_Q ARBDogRun::GetQ() const
{
	return m_Q;
}

inline void ARBDogRun::SetQ(ARB_Q inQ)
{
	m_Q = inQ;
}

inline short ARBDogRun::GetPlace() const
{
	return m_Place;
}

inline void ARBDogRun::SetPlace(short inPlace)
{
	m_Place = inPlace;
}

inline short ARBDogRun::GetInClass() const
{
	return m_InClass;
}

inline void ARBDogRun::SetInClass(short inInClass)
{
	m_InClass = inInClass;
}

inline short ARBDogRun::GetDogsQd() const
{
	return m_DogsQd;
}

inline void ARBDogRun::SetDogsQd(short inDogsQd)
{
	m_DogsQd = inDogsQd;
}

inline ARBDogFaultList const& ARBDogRun::GetFaults() const
{
	return m_Notes.GetFaults();
}

inline ARBDogFaultList& ARBDogRun::GetFaults()
{
	return m_Notes.GetFaults();
}

inline ARBString const& ARBDogRun::GetCRCD() const
{
	return m_Notes.GetCRCD();
}

inline void ARBDogRun::SetCRCD(ARBString const& inCRCD)
{
	m_Notes.SetCRCD(inCRCD);
}

inline ARBString const& ARBDogRun::GetCRCDMetaFile() const
{
	return m_Notes.GetCRCDMetaFile();
}

inline void ARBDogRun::SetCRCDMetaFile(ARBString const& inCRCDMeta)
{
	m_Notes.SetCRCDMetaFile(inCRCDMeta);
}

inline ARBString const& ARBDogRun::GetNote() const
{
	return m_Notes.GetNote();
}

inline void ARBDogRun::SetNote(ARBString const& inNote)
{
	m_Notes.SetNote(inNote);
}

inline ARBDogReferenceRunList const& ARBDogRun::GetReferenceRuns() const
{
	return m_RefRuns;
}

inline ARBDogReferenceRunList& ARBDogRun::GetReferenceRuns()
{
	return m_RefRuns;
}

inline ARBDogRunOtherPointsList const& ARBDogRun::GetOtherPoints() const
{
	return m_OtherPoints;
}

inline ARBDogRunOtherPointsList& ARBDogRun::GetOtherPoints()
{
	return m_OtherPoints;
}

inline size_t ARBDogRun::NumLinks() const
{
	return m_Links.size();
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogRun objects.
 */
class ARBDogRunList : public ARBVector<ARBDogRun>
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
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Sort the list by date in ascending order. The user interface handles
	 * displaying different sort orders.
	 */
	void sort();

	/**
	 * Get the earliest date in the list.
	 */
	ARBDate GetStartDate() const;

	/**
	 * Get the latest date in the list.
	 */
	ARBDate GetEndDate() const;

	/**
	 * Add a run.
	 * @param inRun Run to add.
	 * @return Whether the object was added.
	 */
	bool AddRun(ARBDogRun* inRun);

	/**
	 * Delete a run.
	 * @param inRun Object to delete.
	 * @return Whether run was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteRun(ARBDogRun const* inRun);
};
