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
 * @brief ARBDogTrial and ARBDogTrialList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBDogClub.h"
#include "ARBDogRun.h"

/**
 * All the information about a trial.
 */
class ARBDogTrial : public ARBBase
{
protected:
	ARBDogTrial();
	ARBDogTrial(ARBCalendar const& inCal);
	ARBDogTrial(ARBDogTrial const& rhs);

public:
	~ARBDogTrial();
	static ARBDogTrialPtr New();
	static ARBDogTrialPtr New(ARBCalendar const& inCal);
	ARBDogTrialPtr Clone() const;

	ARBDogTrial& operator=(ARBDogTrial const& rhs);

	bool operator==(ARBDogTrial const& rhs) const;
	bool operator!=(ARBDogTrial const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual tstring GetGenericName() const
	{
		return m_Location;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const;

	/**
	 * Load a trial.
	 * @pre inTree is the actual ARBDogTrial element.
	 * @param inConfig Configuration for looking up information.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfig const& inConfig,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogTrial element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

	/**
	 * Set the MultiQ settings on individual runs. See ARBDogRun::GetMultiQ.
	 *
	 * @param inConfig Configuration for looking up information.
	 */
	void SetMultiQs(ARBConfig const& inConfig);

	/**
	 * Get the Speed points earned for this trial.
	 * @param inConfig Configuration for looking up information.
	 * @param inDiv Division to check.
	 * @param inLevel Level to check.
	 * @return Number of Speed points earned.
	 */
	short GetSpeedPoints(
			ARBConfig const& inConfig,
			tstring const& inDiv,
			tstring const& inLevel) const;

	/**
	 * Does this trial have a hosting club in the specified venue?
	 * @param inVenue Venue to look for.
	 * @return This trial is hosted by the venue.
	 */
	bool HasVenue(tstring const& inVenue) const;

	/*
	 * Getters/setters.
	 */
	tstring const& GetLocation() const
	{
		return m_Location;
	}
	void SetLocation(tstring const& inLoc)
	{
		m_Location = inLoc;
	}
	tstring const& GetNote() const
	{
		return m_Note;
	}
	void SetNote(tstring const& inNote)
	{
		m_Note = inNote;
	}
	bool IsVerified() const
	{
		return m_Verified;
	}
	void SetVerified(bool inVerified)
	{
		m_Verified = inVerified;
	}
	ARBDogClubList const& GetClubs() const
	{
		return m_Clubs;
	}
	ARBDogClubList& GetClubs()
	{
		return m_Clubs;
	}
	ARBDogRunList const& GetRuns() const
	{
		return m_Runs;
	}
	ARBDogRunList& GetRuns()
	{
		return m_Runs;
	}

private:
	tstring m_Location;
	tstring m_Note;
	bool m_Verified;
	ARBDogClubList m_Clubs;
	ARBDogRunList m_Runs;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogTrial objects.
 */
class ARBDogTrialList : public ARBVector<ARBDogTrialPtr>
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
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Sort the list by first date.
	 * @param inDescending Sort in descending or ascending order.
	 */
	void sort(bool inDescending);

	/**
	 * Get the number of trials in a venue.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @return Number of objects.
	 */
	int NumTrialsInVenue(tstring const& inVenue) const;

	/**
	 * Rename a venue, rename any dependent objects.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items changed.
	 */
	int RenameVenue(
			tstring const& inOldVenue,
			tstring const& inNewVenue);

	/**
	 * Delete a venue, remove any dependent objects.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(tstring const& inVenue);

	/**
	 * Number of OtherPoint objects in use.
	 * Used to warning about impending configuration changes.
	 * @param inOther Name of item to look for.
	 * @return Number of objects, not points.
	 */
	int NumOtherPointsInUse(tstring const& inOther) const;

	/**
	 * Rename an OtherPoint, rename any dependent objects.
	 * @param inOldOther OtherPoint name being renamed.
	 * @param inNewOther New OtherPoint name.
	 * @return Number of items changed.
	 */
	int RenameOtherPoints(
			tstring const& inOldOther,
			tstring const& inNewOther);

	/**
	 * Delete an OtherPoint, remove any dependent objects.
	 * @param inOther OtherPoint name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteOtherPoints(tstring const& inOther);

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
			tstring const& inVenue,
			tstring const& inDiv) const;

	/**
	 * Get the number of runs in a division.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @param inDiv Division to tally.
	 * @return Number of objects.
	 */
	int NumRunsInDivision(
			ARBConfigVenuePtr inVenue,
			tstring const& inDiv) const;

	/**
	 * Rename a division, rename any dependent objects.
	 * @param inVenue Venue division is in.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of items changed.
	 */
	int RenameDivision(
			ARBConfigVenuePtr inVenue,
			tstring const& inOldDiv,
			tstring const& inNewDiv);

	/**
	 * Delete a division, remove any dependent objects.
	 * @param inConfig Configuration for looking up information.
	 * @param inVenue Venue division is in.
	 * @param inDiv Division name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteDivision(
			ARBConfig const& inConfig,
			tstring const& inVenue,
			tstring const& inDiv);

	/**
	 * Number of levels in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inLevel Level to tally.
	 * @return Number of objects.
	 */
	int NumLevelsInUse(
			tstring const& inVenue,
			tstring const& inDiv,
			tstring const& inLevel) const;

	/**
	 * Rename a level, rename any dependent objects.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inOldLevel Level name being renamed.
	 * @param inNewLevel New level name.
	 * @return Number of items changed.
	 */
	int RenameLevel(
			tstring const& inVenue,
			tstring const& inDiv,
			tstring const& inOldLevel,
			tstring const& inNewLevel);

	/**
	 * Delete a level, remove any dependent objects.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inLevel Level name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteLevel(
			tstring const& inVenue,
			tstring const& inDiv,
			tstring const& inLevel);

	/**
	 * Number of events in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event to tally.
	 * @return Number of objects.
	 */
	int NumEventsInUse(
			tstring const& inVenue,
			tstring const& inEvent) const;

	/**
	 * Rename an event, rename any dependent objects.
	 * @param inVenue Venue title is in.
	 * @param inOldEvent Event name being renamed.
	 * @param inNewEvent New event name.
	 * @return Number of items changed.
	 */
	int RenameEvent(
			tstring const& inVenue,
			tstring const& inOldEvent,
			tstring const& inNewEvent);

	/**
	 * Delete an event, remove any dependent objects.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteEvent(
			tstring const& inVenue,
			tstring const& inEvent);

	/**
	 * Add a trial.
	 * @param inTrial Trial to add.
	 * @return Whether the object was added.
	 */
	bool AddTrial(ARBDogTrialPtr inTrial);

	/**
	 * Delete a trial.
	 * @param inTrial Object to delete.
	 * @return Whether trial was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteTrial(ARBDogTrialPtr inTrial);
};
