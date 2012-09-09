#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBDogTrial and ARBDogTrialList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2012-09-09 DRC Added 'titlePts' to 'Placement'.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
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
	virtual std::wstring GetGenericName() const;

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const;

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
	 * @param inConfig Configuration.
	 * @return Success
	 * @post The ARBDogTrial element will be created in ioTree.
	 */
	bool Save(
			ElementNodePtr ioTree,
			ARBConfig const& inConfig) const;

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
			std::wstring const& inDiv,
			std::wstring const& inLevel) const;

	/**
	 * Does this trial have a hosting club in the specified venue?
	 * @param inVenue Venue to look for.
	 * @return This trial is hosted by the venue.
	 */
	bool HasVenue(std::wstring const& inVenue) const;

	/*
	 * Getters/setters.
	 */
	std::wstring const& GetLocation() const
	{
		return m_Location;
	}
	void SetLocation(std::wstring const& inLoc)
	{
		m_Location = inLoc;
	}
	std::wstring const& GetNote() const
	{
		return m_Note;
	}
	void SetNote(std::wstring const& inNote)
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
	std::wstring m_Location;
	std::wstring m_Note;
	bool m_Verified;
	ARBDogClubList m_Clubs;
	ARBDogRunList m_Runs;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogTrial objects.
 */
class ARBDogTrialList : public ARBVectorSaveConfig<ARBDogTrialPtr>
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
	int NumTrialsInVenue(std::wstring const& inVenue) const;

	/**
	 * Rename a venue, rename any dependent objects.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items changed.
	 */
	int RenameVenue(
			std::wstring const& inOldVenue,
			std::wstring const& inNewVenue);

	/**
	 * Delete a venue, remove any dependent objects.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(std::wstring const& inVenue);

	/**
	 * Number of OtherPoint objects in use.
	 * Used to warning about impending configuration changes.
	 * @param inOther Name of item to look for.
	 * @return Number of objects, not points.
	 */
	int NumOtherPointsInUse(std::wstring const& inOther) const;

	/**
	 * Rename an OtherPoint, rename any dependent objects.
	 * @param inOldOther OtherPoint name being renamed.
	 * @param inNewOther New OtherPoint name.
	 * @return Number of items changed.
	 */
	int RenameOtherPoints(
			std::wstring const& inOldOther,
			std::wstring const& inNewOther);

	/**
	 * Delete an OtherPoint, remove any dependent objects.
	 * @param inOther OtherPoint name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteOtherPoints(std::wstring const& inOther);

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
			std::wstring const& inVenue,
			std::wstring const& inDiv) const;

	/**
	 * Get the number of runs in a division.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @param inDiv Division to tally.
	 * @return Number of objects.
	 */
	int NumRunsInDivision(
			ARBConfigVenuePtr inVenue,
			std::wstring const& inDiv) const;

	/**
	 * Rename a division, rename any dependent objects.
	 * @param inVenue Venue division is in.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of items changed.
	 */
	int RenameDivision(
			ARBConfigVenuePtr inVenue,
			std::wstring const& inOldDiv,
			std::wstring const& inNewDiv);

	/**
	 * Delete a division, remove any dependent objects.
	 * @param inConfig Configuration for looking up information.
	 * @param inVenue Venue division is in.
	 * @param inDiv Division name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteDivision(
			ARBConfig const& inConfig,
			std::wstring const& inVenue,
			std::wstring const& inDiv);

	/**
	 * Number of levels in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inLevel Level to tally.
	 * @return Number of objects.
	 */
	int NumLevelsInUse(
			std::wstring const& inVenue,
			std::wstring const& inDiv,
			std::wstring const& inLevel) const;

	/**
	 * Rename a level, rename any dependent objects.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inOldLevel Level name being renamed.
	 * @param inNewLevel New level name.
	 * @return Number of items changed.
	 */
	int RenameLevel(
			std::wstring const& inVenue,
			std::wstring const& inDiv,
			std::wstring const& inOldLevel,
			std::wstring const& inNewLevel);

	/**
	 * Delete a level, remove any dependent objects.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inLevel Level name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteLevel(
			std::wstring const& inVenue,
			std::wstring const& inDiv,
			std::wstring const& inLevel);

	/**
	 * Number of events in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event to tally.
	 * @return Number of objects.
	 */
	int NumEventsInUse(
			std::wstring const& inVenue,
			std::wstring const& inEvent) const;

	/**
	 * Rename an event, rename any dependent objects.
	 * @param inVenue Venue title is in.
	 * @param inOldEvent Event name being renamed.
	 * @param inNewEvent New event name.
	 * @return Number of items changed.
	 */
	int RenameEvent(
			std::wstring const& inVenue,
			std::wstring const& inOldEvent,
			std::wstring const& inNewEvent);

	/**
	 * Delete an event, remove any dependent objects.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteEvent(
			std::wstring const& inVenue,
			std::wstring const& inEvent);

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
