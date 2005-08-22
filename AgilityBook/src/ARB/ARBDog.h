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
 * @brief ARBDog and ARBDogList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-02-02 DRC Added ExistingPoints.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-08-18 DRC Added a deceased date.
 * @li 2003-07-24 DRC Removed built-in sort on dogs. Dogs are user-sorted now.
 */

#include <string>
#include "ARBBase.h"
#include "ARBDate.h"
#include "ARBDogExistingPoints.h"
#include "ARBDogRegNum.h"
#include "ARBDogTitle.h"
#include "ARBDogTrial.h"
#include "ARBTypes.h"
#include "ARBVector.h"
class ARBConfig;
class ARBConfigVenue;
class ARBErrorCallback;
class ARBVersion;
class Element;

/**
 * Information about a dog, titles, runs, etc.
 */
class ARBDog : public ARBBase
{
public:
	ARBDog();
	ARBDog(ARBDog const& rhs);
	ARBDog& operator=(ARBDog const& rhs);
	bool operator==(ARBDog const& rhs) const;
	bool operator!=(ARBDog const& rhs) const;

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual std::string GetGenericName() const;

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const;

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
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDog element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Rename a venue.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items updated.
	 */
	int RenameVenue(
			std::string const& inOldVenue,
			std::string const& inNewVenue);

	/**
	 * Delete a venue.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(std::string const& inVenue);

	/**
	 * Rename a division.
	 * @param inVenue Venue whose division is being renamed.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of items updated.
	 */
	int RenameDivision(
			ARBConfigVenue const* inVenue,
			std::string const& inOldDiv,
			std::string const& inNewDiv);

	/**
	 * Delete a division.
	 * @param inConfig Configuration for looking up information.
	 * @param inVenue Venue whose division is being renamed.
	 * @param inDiv Division name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteDivision(
			ARBConfig const& inConfig,
			std::string const& inVenue,
			std::string const& inDiv);

	/*
	 * Getters/setters.
	 */
	std::string const& GetCallName() const;
	void SetCallName(std::string const& inName);
	ARBDate const& GetDOB() const;
	void SetDOB(ARBDate const& inDOB);
	ARBDate const& GetDeceased() const;
	void SetDeceased(ARBDate const& inDeceased);
	std::string const& GetRegisteredName() const;
	void SetRegisteredName(std::string const& inName);
	std::string const& GetBreed() const;
	void SetBreed(std::string const& inBreed);
	std::string const& GetNote() const;
	void SetNote(std::string const& inNote);
	ARBDogExistingPointsList const& GetExistingPoints() const;
	ARBDogExistingPointsList& GetExistingPoints();
	ARBDogRegNumList const& GetRegNums() const;
	ARBDogRegNumList& GetRegNums();
	ARBDogTitleList const& GetTitles() const;
	ARBDogTitleList& GetTitles();
	ARBDogTrialList const& GetTrials() const;
	ARBDogTrialList& GetTrials();

private:
	~ARBDog();
	std::string m_CallName;
	ARBDate m_DOB;
	ARBDate m_Deceased;
	std::string m_RegName;
	std::string m_Breed;
	std::string m_Note;
	ARBDogExistingPointsList m_ExistingPoints;
	ARBDogRegNumList m_RegNums;
	ARBDogTitleList m_Titles;
	ARBDogTrialList m_Trials;
};

inline std::string ARBDog::GetGenericName() const
{
	return m_CallName;
}

inline std::string const& ARBDog::GetCallName() const
{
	return m_CallName;
}

inline void ARBDog::SetCallName(std::string const& inName)
{
	m_CallName = inName;
}

inline ARBDate const& ARBDog::GetDOB() const
{
	return m_DOB;
}

inline void ARBDog::SetDOB(ARBDate const& inDOB)
{
	m_DOB = inDOB;
}

inline ARBDate const& ARBDog::GetDeceased() const
{
	return m_Deceased;
}

inline void ARBDog::SetDeceased(ARBDate const& inDeceased)
{
	m_Deceased = inDeceased;
}

inline std::string const& ARBDog::GetRegisteredName() const
{
	return m_RegName;
}

inline void ARBDog::SetRegisteredName(std::string const& inName)
{
	m_RegName = inName;
}

inline std::string const& ARBDog::GetBreed() const
{
	return m_Breed;
}

inline void ARBDog::SetBreed(std::string const& inBreed)
{
	m_Breed = inBreed;
}

inline std::string const& ARBDog::GetNote() const
{
	return m_Note;
}

inline void ARBDog::SetNote(std::string const& inNote)
{
	m_Note = inNote;
}

inline ARBDogExistingPointsList const& ARBDog::GetExistingPoints() const
{
	return m_ExistingPoints;
}

inline ARBDogExistingPointsList& ARBDog::GetExistingPoints()
{
	return m_ExistingPoints;
}

inline ARBDogRegNumList const& ARBDog::GetRegNums() const
{
	return m_RegNums;
}

inline ARBDogRegNumList& ARBDog::GetRegNums()
{
	return m_RegNums;
}

inline ARBDogTitleList const& ARBDog::GetTitles() const
{
	return m_Titles;
}

inline ARBDogTitleList& ARBDog::GetTitles()
{
	return m_Titles;
}

inline ARBDogTrialList const& ARBDog::GetTrials() const
{
	return m_Trials;
}

inline ARBDogTrialList& ARBDog::GetTrials()
{
	return m_Trials;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDog objects.
 */
class ARBDogList : public ARBVectorLoad2<ARBDog>
{
public:
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
	int NumExistingPointsInVenue(std::string const& inVenue) const;

	/**
	 * Get the number of registration numbers in a venue.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @return Number of objects.
	 */
	int NumRegNumsInVenue(std::string const& inVenue) const;

	/**
	 * Get the number of titles in a venue.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @return Number of objects.
	 */
	int NumTitlesInVenue(std::string const& inVenue) const;

	/**
	 * Get the number of trials in a venue.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @return Number of objects.
	 */
	int NumTrialsInVenue(std::string const& inVenue) const;

	/**
	 * Rename a venue, rename any dependent objects.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items changed.
	 */
	int RenameVenue(
			std::string const& inOldVenue,
			std::string const& inNewVenue);

	/**
	 * Delete a venue, remove any dependent objects.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(std::string const& inVenue);

	/**
	 * Number of OtherPoint objects in use.
	 * Used to warning about impending configuration changes.
	 * @param inOther Name of item to look for.
	 * @return Number of objects, not points.
	 */
	int NumOtherPointsInUse(std::string const& inOther) const;

	/**
	 * Rename an OtherPoint, rename any dependent objects.
	 * @param inOldOther OtherPoint name being renamed.
	 * @param inNewOther New OtherPoint name.
	 * @return Number of items changed.
	 */
	int RenameOtherPoints(
			std::string const& inOldOther,
			std::string const& inNewOther);

	/**
	 * Delete an OtherPoint, remove any dependent objects.
	 * @param inOther OtherPoint name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteOtherPoints(std::string const& inOther);

	/**
	 * Number of MultiQ objects in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue MultiQ is in.
	 * @param inMultiQ Name of item to look for.
	 * @return Number of objects, not points.
	 */
	int NumMultiQsInUse(
			std::string const& inVenue,
			std::string const& inMultiQ) const;

	/**
	 * Rename a MultiQ, rename any dependent objects.
	 * @param inVenue Venue MultiQ is in.
	 * @param inOldMultiQ MultiQ name being renamed.
	 * @param inNewMultiQ New MultiQ name.
	 * @return Number of items changed.
	 */
	int RenameMultiQs(
			std::string const& inVenue,
			std::string const& inOldMultiQ,
			std::string const& inNewMultiQ);

	/**
	 * Delete an MultiQ, remove any dependent objects.
	 * @param inConfig Configuration for looking up information.
	 * @param inVenue Venue in which MultiQ was deleted.
	 * @return Number of items removed.
	 */
	int DeleteMultiQs(
			ARBConfig const& inConfig,
			std::string const& inVenue);

	/**
	 * Number of multiple hosted trials in a division.
	 * Used to warning about impending configuration changes.
	 * @param inConfig Configuration for looking up information.
	 * @param inVenue Venue to tally.
	 * @param inDiv Division to tally.
	 * @return Number of objects.
	 */
	int NumMultiHostedTrialsInDivision(
			ARBConfig const& inConfig,
			std::string const& inVenue,
			std::string const& inDiv) const;

	/**
	 * Get the number of existing point entries in a division.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @param inDiv Division to tally.
	 * @return Number of objects, not points.
	 */
	int NumExistingPointsInDivision(
			ARBConfigVenue const* inVenue,
			std::string const& inDiv) const;

	/**
	 * Get the number of titles in a division.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @param inDiv Division to tally.
	 * @return Number of objects.
	 */
	int NumTitlesInDivision(
			ARBConfigVenue const* inVenue,
			std::string const& inDiv) const;

	/**
	 * Get the number of runs in a division.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @param inDiv Division to tally.
	 * @return Number of objects.
	 */
	int NumRunsInDivision(
			ARBConfigVenue const* inVenue,
			std::string const& inDiv) const;

	/**
	 * Rename a division, rename any dependent objects.
	 * @param inVenue Venue division is in.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of items changed.
	 */
	int RenameDivision(
			ARBConfigVenue const* inVenue,
			std::string const& inOldDiv,
			std::string const& inNewDiv);

	/**
	 * Delete a division, remove any dependent objects.
	 * @param inConfig Configuration for looking up information.
	 * @param inVenue Venue division is in.
	 * @param inDiv Division name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteDivision(
			ARBConfig const& inConfig,
			std::string const& inVenue,
			std::string const& inDiv);

	/**
	 * Number of levels in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inLevel Level to tally.
	 * @return Number of objects.
	 */
	int NumLevelsInUse(
			std::string const& inVenue,
			std::string const& inDiv,
			std::string const& inLevel) const;

	/**
	 * Rename a level, rename any dependent objects.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inOldLevel Level name being renamed.
	 * @param inNewLevel New level name.
	 * @return Number of items changed.
	 */
	int RenameLevel(
			std::string const& inVenue,
			std::string const& inDiv,
			std::string const& inOldLevel,
			std::string const& inNewLevel);

	/**
	 * Delete a level, remove any dependent objects.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inLevel Level name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteLevel(
			std::string const& inVenue,
			std::string const& inDiv,
			std::string const& inLevel);

	/**
	 * Number of titles in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue title is in.
	 * @param inTitle Title to tally.
	 * @return Number of objects.
	 */
	int NumTitlesInUse(
			std::string const& inVenue,
			std::string const& inTitle) const;

	/**
	 * Rename a title, rename any dependent objects.
	 * @param inVenue Venue title is in.
	 * @param inOldTitle Title name being renamed.
	 * @param inNewTitle New title name.
	 * @return Number of items changed.
	 */
	int RenameTitle(
			std::string const& inVenue,
			std::string const& inOldTitle,
			std::string const& inNewTitle);

	/**
	 * Delete a title, remove any dependent objects.
	 * @param inVenue Venue title is in.
	 * @param inTitle Title name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteTitle(
			std::string const& inVenue,
			std::string const& inTitle);

	/**
	 * Number of events in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event to tally.
	 * @return Number of objects.
	 */
	int NumEventsInUse(
			std::string const& inVenue,
			std::string const& inEvent) const;

	/**
	 * Rename an event, rename any dependent objects.
	 * @param inVenue Venue title is in.
	 * @param inOldEvent Event name being renamed.
	 * @param inNewEvent New event name.
	 * @return Number of items changed.
	 */
	int RenameEvent(
			std::string const& inVenue,
			std::string const& inOldEvent,
			std::string const& inNewEvent);

	/**
	 * Delete an event, remove any dependent objects.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteEvent(
			std::string const& inVenue,
			std::string const& inEvent);

	/**
	 * Add a dog.
	 * @param inDog Dog to add.
	 * @return Whetehr the object was added.
	 */
	bool AddDog(ARBDog* inDog);

	/**
	 * Delete a dog.
	 * @param inDog Object to delete.
	 * @return Whether dog was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteDog(ARBDog const* inDog);
};
