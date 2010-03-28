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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
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
	virtual wxString GetGenericName() const;

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<wxString>& ioStrings) const;

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
	int NumOtherPointsInUse(wxString const& inOther) const;

	/**
	 * Rename an OtherPoint, rename any dependent objects.
	 * @param inOldName OtherPoint name being renamed.
	 * @param inNewName New OtherPoint name.
	 * @return Number of items changed.
	 */
	int RenameOtherPoints(
			wxString const& inOldName,
			wxString const& inNewName);

	/**
	 * Delete an OtherPoint, remove any dependent objects.
	 * @param inName OtherPoint name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteOtherPoints(wxString const& inName);

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
	wxString const& GetEvent() const
	{
		return m_Event;
	}
	void SetEvent(wxString const& inEvent)
	{
		m_Event = inEvent;
	}
	wxString const& GetSubName() const
	{
		return m_SubName;
	}
	void SetSubName(wxString const& inSubName)
	{
		m_SubName = inSubName;
	}
	wxString const& GetHeight() const
	{
		return m_Height;
	}
	void SetHeight(wxString const& inHeight)
	{
		m_Height = inHeight;
	}
	wxString const& GetConditions() const
	{
		return m_Conditions;
	}
	void SetConditions(wxString const& inConditions)
	{
		m_Conditions = inConditions;
	}
	wxString const& GetJudge() const
	{
		return m_Judge;
	}
	void SetJudge(wxString const& inJudge)
	{
		m_Judge = inJudge;
	}
	wxString const& GetHandler() const
	{
		return m_Handler;
	}
	void SetHandler(wxString const& inHandler)
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
	wxString const& GetCRCD() const
	{
		return m_Notes.GetCRCD();
	}
	void SetCRCD(wxString const& inCRCD)
	{
		m_Notes.SetCRCD(inCRCD);
	}
	wxString const& GetCRCDRawMetaData() const
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
	wxString const& GetNote() const
	{
		return m_Notes.GetNote();
	}
	void SetNote(wxString const& inNote)
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
	size_t GetLinks(std::set<wxString>& outLinks) const;
	bool HasLink(wxString const& inLink) const;
	void AddLink(wxString const& inLink);
	void RemoveLink(wxString const& inLink);

private:
	std::set<std::tr1::weak_ptr<ARBConfigMultiQ> > m_pMultiQs; //< Not persisted.
	ARBDate m_Date;
	wxString m_Division;
	wxString m_Level;
	wxString m_Height;
	wxString m_Event;
	wxString m_SubName; //< Only used if the config supports it.
	wxString m_Conditions;
	wxString m_Judge;
	wxString m_Handler;
	ARBDogRunPartnerList m_Partners;
	ARBDogRunScoring m_Scoring;
	ARB_Q m_Q;
	short m_Place;
	short m_InClass;
	short m_DogsQd;
	ARBDogRunOtherPointsList m_OtherPoints;
	ARBDogNotes m_Notes;
	ARBDogReferenceRunList m_RefRuns;
	typedef std::set<wxString> ARBDogRunLinks;
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
