#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBDog and ARBDogList classes.
 * @author David Connet
 *
 * Revision History
 * 2016-06-19 Add support for Lifetime names.
 * 2012-09-09 Added 'titlePts' to 'Placement'.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-14 Moved 'Titles' to 'Venue'.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-02-02 Added ExistingPoints.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-08-18 Added a deceased date.
 * 2003-07-24 Removed built-in sort on dogs. Dogs are user-sorted now.
 */

#include "ARBBase.h"
#include "ARBDogExistingPoints.h"
#include "ARBDogRegNum.h"
#include "ARBDogTitle.h"
#include "ARBDogTrial.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"

#include "ARBCommon/ARBDate.h"


namespace dconSoft
{
namespace ARB
{

/**
 * Information about a dog, titles, runs, etc.
 */
class ARB_API ARBDog : public ARBBase
{
protected:
	ARBDog();
	ARBDog(ARBDog const& rhs);
	ARBDog(ARBDog&& rhs);

public:
	~ARBDog();
	static ARBDogPtr New();
	ARBDogPtr Clone() const;

	ARBDog& operator=(ARBDog const& rhs);
	ARBDog& operator=(ARBDog&& rhs);

	bool operator==(ARBDog const& rhs) const;
	bool operator!=(ARBDog const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	wxString GetGenericName() const override
	{
		return m_CallName;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<wxString>& ioStrings) const override;

	/**
	 * Load a dog.
	 * @pre inTree is the actual ARBDog element.
	 * @param inConfig Configuration for looking up information.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
		ARBConfig const& inConfig,
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @param inConfig Configuration.
	 * @return Success
	 * @post The ARBDog element will be created in ioTree.
	 */
	bool Save(ARBCommon::ElementNodePtr const& ioTree, ARBConfig const& inConfig) const;

	/**
	 * Rename a venue.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items updated.
	 */
	int RenameVenue(wxString const& inOldVenue, wxString const& inNewVenue);

	/**
	 * Delete a venue.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(wxString const& inVenue);

	/**
	 * Rename a division.
	 * @param inVenue Venue whose division is being renamed.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of items updated.
	 */
	int RenameDivision(ARBConfigVenuePtr const& inVenue, wxString const& inOldDiv, wxString const& inNewDiv);

	/**
	 * Delete a division.
	 * @param inConfig Configuration for looking up information.
	 * @param inVenue Venue whose division is being renamed.
	 * @param inDiv Division name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteDivision(ARBConfig const& inConfig, wxString const& inVenue, wxString const& inDiv);

	/*
	 * Getters/setters.
	 */
	wxString const& GetCallName() const
	{
		return m_CallName;
	}
	void SetCallName(wxString const& inName)
	{
		m_CallName = inName;
	}
	ARBCommon::ARBDate const& GetDOB() const
	{
		return m_DOB;
	}
	void SetDOB(ARBCommon::ARBDate const& inDOB)
	{
		m_DOB = inDOB;
	}
	ARBCommon::ARBDate const& GetDeceased() const
	{
		return m_Deceased;
	}
	void SetDeceased(ARBCommon::ARBDate const& inDeceased)
	{
		m_Deceased = inDeceased;
	}
	wxString const& GetRegisteredName() const
	{
		return m_RegName;
	}
	void SetRegisteredName(wxString const& inName)
	{
		m_RegName = inName;
	}
	wxString const& GetBreed() const
	{
		return m_Breed;
	}
	void SetBreed(wxString const& inBreed)
	{
		m_Breed = inBreed;
	}
	wxString const& GetNote() const
	{
		return m_Note;
	}
	void SetNote(wxString const& inNote)
	{
		m_Note = inNote;
	}
	ARBDogExistingPointsList const& GetExistingPoints() const
	{
		return m_ExistingPoints;
	}
	ARBDogExistingPointsList& GetExistingPoints()
	{
		return m_ExistingPoints;
	}
	ARBDogRegNumList const& GetRegNums() const
	{
		return m_RegNums;
	}
	ARBDogRegNumList& GetRegNums()
	{
		return m_RegNums;
	}
	ARBDogTitleList const& GetTitles() const
	{
		return m_Titles;
	}
	ARBDogTitleList& GetTitles()
	{
		return m_Titles;
	}
	ARBDogTrialList const& GetTrials() const
	{
		return m_Trials;
	}
	ARBDogTrialList& GetTrials()
	{
		return m_Trials;
	}

private:
	wxString m_CallName;
	ARBCommon::ARBDate m_DOB;
	ARBCommon::ARBDate m_Deceased;
	wxString m_RegName;
	wxString m_Breed;
	wxString m_Note;
	ARBDogExistingPointsList m_ExistingPoints;
	ARBDogRegNumList m_RegNums;
	ARBDogTitleList m_Titles;
	ARBDogTrialList m_Trials;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDog objects.
 */
class ARB_API ARBDogList : public ARBVectorSaveConfig<ARBDogPtr>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual T element.
	 * @param inConfig Configuration information to verify data to load against.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
		ARBConfig const& inConfig,
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Set the MultiQ settings on individual runs. See ARBDogRun::GetMultiQ.
	 *
	 * @param inConfig Configuration for looking up information.
	 */
	void SetMultiQs(ARBConfig const& inConfig);

	/**
	 * Get the number of existing point entries in a venue.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @return Number of objects, not points.
	 */
	int NumExistingPointsInVenue(wxString const& inVenue) const;

	/**
	 * Get the number of registration numbers in a venue.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @return Number of objects.
	 */
	int NumRegNumsInVenue(wxString const& inVenue) const;

	/**
	 * Get the number of titles in a venue.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @return Number of objects.
	 */
	int NumTitlesInVenue(wxString const& inVenue) const;

	/**
	 * Get the number of trials in a venue.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @return Number of objects.
	 */
	int NumTrialsInVenue(wxString const& inVenue) const;

	/**
	 * Rename a venue, rename any dependent objects.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items changed.
	 */
	int RenameVenue(wxString const& inOldVenue, wxString const& inNewVenue);

	/**
	 * Delete a venue, remove any dependent objects.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(wxString const& inVenue);

	/**
	 * Number of OtherPoint objects in use.
	 * Used to warning about impending configuration changes.
	 * @param inOther Name of item to look for.
	 * @return Number of objects, not points.
	 */
	int NumOtherPointsInUse(wxString const& inOther) const;

	/**
	 * Rename an OtherPoint, rename any dependent objects.
	 * @param inOldOther OtherPoint name being renamed.
	 * @param inNewOther New OtherPoint name.
	 * @return Number of items changed.
	 */
	int RenameOtherPoints(wxString const& inOldOther, wxString const& inNewOther);

	/**
	 * Delete an OtherPoint, remove any dependent objects.
	 * @param inOther OtherPoint name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteOtherPoints(wxString const& inOther);

	/**
	 * Number of MultiQ objects in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue MultiQ is in.
	 * @param inMultiQ Name of item to look for.
	 * @return Number of objects, not points.
	 */
	int NumMultiQsInUse(wxString const& inVenue, wxString const& inMultiQ) const;

	/**
	 * Rename a MultiQ, rename any dependent objects.
	 * @param inVenue Venue MultiQ is in.
	 * @param inOldMultiQ MultiQ name being renamed.
	 * @param inNewMultiQ New MultiQ name.
	 * @return Number of items changed.
	 */
	int RenameMultiQs(wxString const& inVenue, wxString const& inOldMultiQ, wxString const& inNewMultiQ);

	/**
	 * Delete any MultiQs no longer in venue.
	 * @param inConfig Configuration for looking up information.
	 * @param inVenue Venue in which MultiQ was deleted.
	 * @return Number of items removed.
	 */
	int DeleteMultiQs(ARBConfig const& inConfig, wxString const& inVenue);

	/**
	 * Number of multiple hosted trials in a division.
	 * Used to warning about impending configuration changes.
	 * @param inConfig Configuration for looking up information.
	 * @param inVenue Venue to tally.
	 * @param inDiv Division to tally.
	 * @return Number of objects.
	 */
	int NumMultiHostedTrialsInDivision(ARBConfig const& inConfig, wxString const& inVenue, wxString const& inDiv) const;

	/**
	 * Get the number of existing point entries in a division.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @param inDiv Division to tally.
	 * @return Number of objects, not points.
	 */
	int NumExistingPointsInDivision(ARBConfigVenuePtr const& inVenue, wxString const& inDiv) const;

	/**
	 * Get the number of runs in a division.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @param inDiv Division to tally.
	 * @return Number of objects.
	 */
	int NumRunsInDivision(ARBConfigVenuePtr const& inVenue, wxString const& inDiv) const;

	/**
	 * Rename a division, rename any dependent objects.
	 * @param inVenue Venue division is in.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of items changed.
	 */
	int RenameDivision(ARBConfigVenuePtr const& inVenue, wxString const& inOldDiv, wxString const& inNewDiv);

	/**
	 * Delete a division, remove any dependent objects.
	 * @param inConfig Configuration for looking up information.
	 * @param inVenue Venue division is in.
	 * @param inDiv Division name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteDivision(ARBConfig const& inConfig, wxString const& inVenue, wxString const& inDiv);

	/**
	 * Number of levels in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inLevel Level (leaf node) to tally.
	 * @return Number of objects.
	 */
	int NumLevelsInUse(wxString const& inVenue, wxString const& inDiv, wxString const& inLevel) const;

	/**
	 * Rename a level, rename any dependent objects.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inOldLevel Level name being renamed.
	 * @param inNewLevel New level name.
	 * @return Number of items changed.
	 */
	int RenameLevel(
		wxString const& inVenue,
		wxString const& inDiv,
		wxString const& inOldLevel,
		wxString const& inNewLevel);

	/**
	 * Delete a level, remove any dependent objects.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inLevel Level name (leaf node) being deleted.
	 * @return Number of items removed.
	 */
	int DeleteLevel(wxString const& inVenue, wxString const& inDiv, wxString const& inLevel);

	/**
	 * Number of titles in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue title is in.
	 * @param inTitle Title to tally.
	 * @return Number of objects.
	 */
	int NumTitlesInUse(wxString const& inVenue, wxString const& inTitle) const;

	/**
	 * Rename a title, rename any dependent objects.
	 * @param inVenue Venue title is in.
	 * @param inOldTitle Title name being renamed.
	 * @param inNewTitle New title name.
	 * @return Number of items changed.
	 */
	int RenameTitle(wxString const& inVenue, wxString const& inOldTitle, wxString const& inNewTitle);

	/**
	 * Delete a title, remove any dependent objects.
	 * @param inVenueConfig Venue that has title being deleted.
	 * @param inVenue Venue title is in.
	 * @param inTitle Title name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteTitle(ARBConfigVenuePtr const& inVenueConfig, wxString const& inVenue, wxString const& inTitle);

	/**
	 * Number of events in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event to tally.
	 * @return Number of objects.
	 */
	int NumEventsInUse(wxString const& inVenue, wxString const& inEvent) const;

	/**
	 * Rename an event, rename any dependent objects.
	 * @param inVenue Venue title is in.
	 * @param inOldEvent Event name being renamed.
	 * @param inNewEvent New event name.
	 * @return Number of items changed.
	 */
	int RenameEvent(wxString const& inVenue, wxString const& inOldEvent, wxString const& inNewEvent);

	/**
	 * Delete an event, remove any dependent objects.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteEvent(wxString const& inVenue, wxString const& inEvent);

	/**
	 * Rename a lifetime name.
	 * @param inVenue Venue name is in.
	 * @param inOldName Lifetime name being renamed.
	 * @param inNewName New Lifetime name.
	 * @return Number of items changed.
	 */
	int RenameLifetimeName(wxString const& inVenue, wxString const& inOldName, wxString const& inNewName);

	/**
	 * Delete a lifetime name.
	 * @param inVenue Venue name is in.
	 * @param inName Lifetime name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteLifetimeName(wxString const& inVenue, wxString const& inName);

	/**
	 * Add a dog.
	 * @param inDog Dog to add.
	 * @return Whetehr the object was added.
	 */
	bool AddDog(ARBDogPtr const& inDog);

	/**
	 * Delete a dog.
	 * @param inDog Object to delete.
	 * @return Whether dog was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteDog(ARBDogPtr const& inDog);
};

} // namespace ARB
} // namespace dconSoft
