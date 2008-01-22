#pragma once

/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
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
#include <boost/weak_ptr.hpp>
#include "ARBBase.h"
#include "ARBDate.h"
#include "ARBDogNotes.h"
#include "ARBDogReferenceRun.h"
#include "ARBDogRunOtherPoints.h"
#include "ARBDogRunPartner.h"
#include "ARBDogRunScoring.h"
#include "ARBTypes.h"

class ARBDogRun : public ARBBase
{
protected:
	ARBDogRun();
	ARBDogRun(ARBDogRun const& rhs);

public:
	~ARBDogRun();
	static ARBDogRunPtr New();
	ARBDogRunPtr Clone() const;

	ARBDogRun& operator=(ARBDogRun const& rhs);

	bool operator==(ARBDogRun const& rhs) const;
	bool operator!=(ARBDogRun const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual tstring GetGenericName() const;

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const;

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
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogRun element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

	/**
	 * Number of OtherPoint objects in use.
	 * Used to warning about impending configuration changes.
	 * @param inOther Name of item to look for.
	 * @return Number of objects, not points.
	 */
	int NumOtherPointsInUse(tstring const& inOther) const;

	/**
	 * Rename an OtherPoint, rename any dependent objects.
	 * @param inOldName OtherPoint name being renamed.
	 * @param inNewName New OtherPoint name.
	 * @return Number of items changed.
	 */
	int RenameOtherPoints(
			tstring const& inOldName,
			tstring const& inNewName);

	/**
	 * Delete an OtherPoint, remove any dependent objects.
	 * @param inName OtherPoint name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteOtherPoints(tstring const& inName);

	/**
	 * Get the number of Speed points earned in this run.
	 * @param inScoring Scoring method used.
	 * @return Number of Speed points earned.
	 */
	short GetSpeedPoints(ARBConfigScoringPtr inScoring) const;

	/**
	 * Get the number of title points earned in this run.
	 * @param inScoring Scoring method used.
	 * @param outClean Was this a clean run?
	 * @param outLifeTime Lifetime points earned in this run.
	 * @param outPlacement Placement titling points earned in this run.
	 * @return Number of title points earned.
	 */
	double GetTitlePoints(
			ARBConfigScoringPtr inScoring,
			bool* outClean = NULL,
			double* outLifeTime = NULL,
			double* outPlacement = NULL) const;

	/**
	 * Get the score for this run.
	 * @param inScoring Scoring method used.
	 * @return Total score for this run. The meaning of a score varies by event.
	 */
	double GetScore(ARBConfigScoringPtr inScoring) const;

	/*
	 * Getters/setters.
	 */
	size_t GetMultiQs(std::vector<ARBConfigMultiQPtr>& outMultiQs) const;
	void ClearMultiQs();
	void AddMultiQ(ARBConfigMultiQPtr inMultiQ);
	ARBDate const& GetDate() const
	{
		return m_Date;
	}
	void SetDate(ARBDate const& inDate)
	{
		m_Date = inDate;
	}
	tstring const& GetDivision() const
	{
		return m_Division;
	}
	void SetDivision(tstring const& inDiv)
	{
		m_Division = inDiv;
	}
	tstring const& GetLevel() const
	{
		return m_Level;
	}
	void SetLevel(tstring const& inLevel)
	{
		m_Level = inLevel;
	}
	tstring const& GetEvent() const
	{
		return m_Event;
	}
	void SetEvent(tstring const& inEvent)
	{
		m_Event = inEvent;
	}
	tstring const& GetSubName() const
	{
		return m_SubName;
	}
	void SetSubName(tstring const& inSubName)
	{
		m_SubName = inSubName;
	}
	tstring const& GetHeight() const
	{
		return m_Height;
	}
	void SetHeight(tstring const& inHeight)
	{
		m_Height = inHeight;
	}
	tstring const& GetConditions() const
	{
		return m_Conditions;
	}
	void SetConditions(tstring const& inConditions)
	{
		m_Conditions = inConditions;
	}
	tstring const& GetJudge() const
	{
		return m_Judge;
	}
	void SetJudge(tstring const& inJudge)
	{
		m_Judge = inJudge;
	}
	tstring const& GetHandler() const
	{
		return m_Handler;
	}
	void SetHandler(tstring const& inHandler)
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
	tstring const& GetCRCD() const
	{
		return m_Notes.GetCRCD();
	}
	void SetCRCD(tstring const& inCRCD)
	{
		m_Notes.SetCRCD(inCRCD);
	}
	tstring const& GetCRCDRawMetaData() const
	{
		return m_Notes.GetCRCDRawMetaData();
	}
	ARBMetaDataPtr GetCRCDMetaData() const
	{
		return m_Notes.GetCRCDMetaData();
	}
	void SetCRCDMetaData(unsigned char const* inCRCDMeta, size_t inBytes)
	{
		m_Notes.SetCRCDMetaData(inCRCDMeta, inBytes);
	}
	tstring const& GetNote() const
	{
		return m_Notes.GetNote();
	}
	void SetNote(tstring const& inNote)
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
	size_t GetLinks(std::set<tstring>& outLinks) const;
	bool HasLink(tstring const& inLink) const;
	void AddLink(tstring const& inLink);
	void RemoveLink(tstring const& inLink);

private:
	std::set<boost::weak_ptr<ARBConfigMultiQ> > m_pMultiQs; //< Not persisted.
	ARBDate m_Date;
	tstring m_Division;
	tstring m_Level;
	tstring m_Height;
	tstring m_Event;
	tstring m_SubName; //< Only used if the config supports it.
	tstring m_Conditions;
	tstring m_Judge;
	tstring m_Handler;
	ARBDogRunPartnerList m_Partners;
	ARBDogRunScoring m_Scoring;
	ARB_Q m_Q;
	short m_Place;
	short m_InClass;
	short m_DogsQd;
	ARBDogRunOtherPointsList m_OtherPoints;
	ARBDogNotes m_Notes;
	ARBDogReferenceRunList m_RefRuns;
	typedef std::set<tstring> ARBDogRunLinks;
	ARBDogRunLinks m_Links;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogRun objects.
 */
class ARBDogRunList : public ARBVector<ARBDogRunPtr>
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
			ElementNodePtr inTree,
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
	bool AddRun(ARBDogRunPtr inRun);

	/**
	 * Delete a run.
	 * @param inRun Object to delete.
	 * @return Whether run was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteRun(ARBDogRunPtr inRun);
};
