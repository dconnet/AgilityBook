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
 * @brief ARBDog and ARBDogList classes.
 * @author David Connet
 *
 * Revision History
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
class ARBVersion;
class CElement;

/**
 * Information about a dog, titles, runs, etc.
 */
class ARBDog : public ARBBase
{
public:
	ARBDog();
	ARBDog(const ARBDog& rhs);
	ARBDog& operator=(const ARBDog& rhs);
	bool operator==(const ARBDog& rhs) const;
	bool operator!=(const ARBDog& rhs) const;

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
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool Load(
		const ARBConfig& inConfig,
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDog element will be created in ioTree.
	 */
	bool Save(CElement& ioTree) const;

	/**
	 * Rename a venue.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items updated.
	 */
	int RenameVenue(
		const std::string& inOldVenue,
		const std::string& inNewVenue);

	/**
	 * Delete a venue.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(const std::string& inVenue);

	/**
	 * Rename a division.
	 * @param inVenue Venue whose division is being renamed.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameDivision(
		const ARBConfigVenue* inVenue,
		const std::string& inOldDiv,
		const std::string& inNewDiv);

	/**
	 * Delete a division.
	 * @param inConfig Configuration for looking up information.
	 * @param inVenue Venue whose division is being renamed.
	 * @param inDiv Division name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteDivision(
		const ARBConfig& inConfig,
		const std::string& inVenue,
		const std::string& inDiv);

	/*
	 * Getters/setters.
	 */
	const std::string& GetCallName() const;
	void SetCallName(const std::string& inName);
	const ARBDate& GetDOB() const;
	void SetDOB(const ARBDate& inDOB);
	const ARBDate& GetDeceased() const;
	void SetDeceased(const ARBDate& inDeceased);
	const std::string& GetRegisteredName() const;
	void SetRegisteredName(const std::string& inName);
	const std::string& GetBreed() const;
	void SetBreed(const std::string& inBreed);
	const std::string& GetNote() const;
	void SetNote(const std::string& inNote);
	const ARBDogExistingPointsList& GetExistingPoints() const;
	ARBDogExistingPointsList& GetExistingPoints();
	const ARBDogRegNumList& GetRegNums() const;
	ARBDogRegNumList& GetRegNums();
	const ARBDogTitleList& GetTitles() const;
	ARBDogTitleList& GetTitles();
	const ARBDogTrialList& GetTrials() const;
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

inline const std::string& ARBDog::GetCallName() const
{
	return m_CallName;
}

inline void ARBDog::SetCallName(const std::string& inName)
{
	m_CallName = inName;
}

inline const ARBDate& ARBDog::GetDOB() const
{
	return m_DOB;
}

inline void ARBDog::SetDOB(const ARBDate& inDOB)
{
	m_DOB = inDOB;
}

inline const ARBDate& ARBDog::GetDeceased() const
{
	return m_Deceased;
}

inline void ARBDog::SetDeceased(const ARBDate& inDeceased)
{
	m_Deceased = inDeceased;
}

inline const std::string& ARBDog::GetRegisteredName() const
{
	return m_RegName;
}

inline void ARBDog::SetRegisteredName(const std::string& inName)
{
	m_RegName = inName;
}

inline const std::string& ARBDog::GetBreed() const
{
	return m_Breed;
}

inline void ARBDog::SetBreed(const std::string& inBreed)
{
	m_Breed = inBreed;
}

inline const std::string& ARBDog::GetNote() const
{
	return m_Note;
}

inline void ARBDog::SetNote(const std::string& inNote)
{
	m_Note = inNote;
}

inline const ARBDogExistingPointsList& ARBDog::GetExistingPoints() const
{
	return m_ExistingPoints;
}

inline ARBDogExistingPointsList& ARBDog::GetExistingPoints()
{
	return m_ExistingPoints;
}

inline const ARBDogRegNumList& ARBDog::GetRegNums() const
{
	return m_RegNums;
}

inline ARBDogRegNumList& ARBDog::GetRegNums()
{
	return m_RegNums;
}

inline const ARBDogTitleList& ARBDog::GetTitles() const
{
	return m_Titles;
}

inline ARBDogTitleList& ARBDog::GetTitles()
{
	return m_Titles;
}

inline const ARBDogTrialList& ARBDog::GetTrials() const
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
	bool operator==(const ARBDogList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBDogList& rhs) const
	{
		return !isEqual(rhs);
	}

	/**
	 * Get the number of existing point objects for a dog.
	 * @param inVenue Venue to tally.
	 */
	int NumExistingPointsInVenue(const std::string& inVenue) const;

	/**
	 * Get the number of registration numbers for a dog.
	 * @param inVenue Venue to tally.
	 */
	int NumRegNumsInVenue(const std::string& inVenue) const;

	/**
	 * Get the number of titles for a dog.
	 * @param inVenue Venue to tally.
	 */
	int NumTitlesInVenue(const std::string& inVenue) const;

	/**
	 * Get the number of trials for a dog.
	 * @param inVenue Venue to tally.
	 */
	int NumTrialsInVenue(const std::string& inVenue) const;

	/**
	 * Rename a venue, rename any dependent objects.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items changed.
	 */
	int RenameVenue(
		const std::string& inOldVenue,
		const std::string& inNewVenue);

	/**
	 * Delete a venue, remove any dependent objects.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(const std::string& inVenue);

	/**
	 * Number of OtherPoint objects in use for a dog.
	 * @param inOther Name of item to look for.
	 */
	int NumOtherPointsInUse(const std::string& inOther) const;

	/**
	 * Rename an OtherPoint, rename any dependent objects.
	 * @param inOldOther OtherPoint name being renamed.
	 * @param inNewOther New OtherPoint name.
	 * @return Number of items changed.
	 */
	int RenameOtherPoints(
		const std::string& inOldOther,
		const std::string& inNewOther);

	/**
	 * Delete an OtherPoint, remove any dependent objects.
	 * @param inOther OtherPoint name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteOtherPoints(const std::string& inOther);

	int NumMultiHostedTrialsInDivision(
		const ARBConfig& inConfig,
		const std::string& inVenue,
		const std::string& inDiv);
	int NumExistingPointsInDivision(
		const ARBConfigVenue* inVenue,
		const std::string& inDiv) const;
	int NumTitlesInDivision(
		const ARBConfigVenue* inVenue,
		const std::string& inDiv) const;
	int NumRunsInDivision(
		const ARBConfigVenue* inVenue,
		const std::string& inDiv) const;
	int RenameDivision(
		const ARBConfigVenue* inVenue,
		const std::string& inOldDiv,
		const std::string& inNewDiv);
	int DeleteDivision(
		const ARBConfig& inConfig,
		const std::string& inVenue,
		const std::string& inDiv);

	int NumLevelsInUse(
		const std::string& inVenue,
		const std::string& inDiv,
		const std::string& inLevel) const;
	int RenameLevel(
		const std::string& inVenue,
		const std::string& inDiv,
		const std::string& inOldLevel,
		const std::string& inNewLevel);
	int DeleteLevel(
		const std::string& inVenue,
		const std::string& inDiv,
		const std::string& inLevel);

	int NumTitlesInUse(
		const std::string& inVenue,
		const std::string& inTitle) const;
	int RenameTitle(
		const std::string& inVenue,
		const std::string& inOldTitle,
		const std::string& inNewTitle);
	int DeleteTitle(
		const std::string& inVenue,
		const std::string& inTitle);

	int NumEventsInUse(
		const std::string& inVenue,
		const std::string& inEvent) const;
	int RenameEvent(
		const std::string& inVenue,
		const std::string& inOldEvent,
		const std::string& inNewEvent);
	int DeleteEvent(
		const std::string& inVenue,
		const std::string& inEvent);

	/**
	 * Add a dog.
	 * @param inDog Dog to add.
	 * @return Pointer to object.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 *       The pointer is added to the list and its ref count is incremented.
	 */
	ARBDog* AddDog(ARBDog* inDog);

	/**
	 * Delete a dog.
	 * @param inName Name of title to delete.
	 * @return Whether title was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteDog(const ARBDog* inDog);
};
