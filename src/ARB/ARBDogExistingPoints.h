#pragma once

/*
 * Copyright © 2004-2006 David Connet. All Rights Reserved.
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
 * @brief ARBDogExistingPoints and ARBDogExistingPointsList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-02-02 DRC Created
 */

#include "ARBBase.h"
#include "ARBDate.h"
#include "ARBTypes.h"

/**
 * Initializes the titling points a dog has.
 */
class ARBDogExistingPoints : public ARBBase
{
protected:
	ARBDogExistingPoints();
	ARBDogExistingPoints(ARBDogExistingPoints const& rhs);

public:
	~ARBDogExistingPoints();
	static ARBDogExistingPointsPtr New()
	{
		return ARBDogExistingPointsPtr(new ARBDogExistingPoints());
	}
	ARBDogExistingPointsPtr Clone() const
	{
		return ARBDogExistingPointsPtr(new ARBDogExistingPoints(*this));
	}

	ARBDogExistingPoints& operator=(ARBDogExistingPoints const& rhs);

	bool operator==(ARBDogExistingPoints const& rhs) const;
	bool operator!=(ARBDogExistingPoints const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual ARBString GetGenericName() const
	{
		ARBString name;
		if (0 < m_Other.length())
			name = m_Other;
		else
			name = m_Event;
		return name;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<ARBString>& ioStrings) const
	{
		return 0;
	}

	/**
	 * Load an existing point.
	 * @pre inTree is the actual ARBDogExistingPoints element.
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
	 * @post The ARBDogExistingPoints element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Type of points.
	 */
	typedef enum
	{
		eUnknown,
		eOtherPoints,	///< OtherPoint points.
		eRuns,			///< Number of titling points for a run.
		eSpeed,			///< Number of Speed points.
		eMQ,			///< Number of multi Qs.
		eSQ				///< Number of Super Qs.
	} PointType;

	/**
	 * Translate the enum to a string.
	 */
	static ARBString GetPointTypeName(PointType inType);

	/*
	 * Getters/setters.
	 */
	PointType GetType() const
	{
		return m_Type;
	}
	void SetType(PointType inType)
	{
		m_Type = inType;
	}
	ARBDate const& GetDate() const
	{
		return m_Date;
	}
	void SetDate(ARBDate const& inDate)
	{
		m_Date = inDate;
	}
	ARBString const& GetComment() const
	{
		return m_Comment;
	}
	void SetComment(ARBString const& inComment)
	{
		m_Comment = inComment;
	}
	ARBString const& GetOtherPoints() const
	{
		return m_Other;
	}
	void SetOtherPoints(ARBString const& inOther)
	{
		m_Other = inOther;
	}
	ARBString const& GetVenue() const
	{
		return m_Venue;
	}
	void SetVenue(ARBString const& inVenue)
	{
		m_Venue = inVenue;
	}
	ARBString const& GetMultiQ() const
	{
		return m_MultiQ;
	}
	void SetMultiQ(ARBString const& inMultiQ)
	{
		m_MultiQ = inMultiQ;
	}
	ARBString const& GetDivision() const
	{
		return m_Div;
	}
	void SetDivision(ARBString const& inDiv)
	{
		m_Div = inDiv;
	}
	ARBString const& GetLevel() const
	{
		return m_Level;
	}
	void SetLevel(ARBString const& inLevel)
	{
		m_Level = inLevel;
	}
	ARBString const& GetEvent() const
	{
		return m_Event;
	}
	void SetEvent(ARBString const& inEvent)
	{
		m_Event = inEvent;
	}
	ARBString const& GetSubName() const
	{
		return m_SubName;
	}
	void SetSubName(ARBString const& inSubName)
	{
		m_SubName = inSubName;
	}
	double GetPoints() const
	{
		return m_Points;
	}
	void SetPoints(double inPoints)
	{
		m_Points = inPoints;
	}

private:
	ARBDate m_Date;
	ARBString m_Comment;
	PointType m_Type;
	ARBString m_Other;
	ARBString m_Venue;
	ARBString m_MultiQ;
	ARBString m_Div;
	ARBString m_Level;
	ARBString m_Event;
	ARBString m_SubName;
	double m_Points;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogExistingPoints objects.
 */
class ARBDogExistingPointsList : public ARBVector<ARBDogExistingPointsPtr>
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
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Sort by points/venue/division/level.
	 */
	void sort();

	/**
	 * Are there any existing points for the specified venue?
	 * @param inVenue Venue to search for.
	 * @return Whether any existing points exist.
	 */
	bool HasPoints(ARBString const& inVenue) const;

	/**
	 * Are there any existing points?
	 * @param inVenue Venue to search for.
	 * @param inDiv Division to search for.
	 * @param inLevel Level to search for.
	 * @param inEvent Event to search for.
	 * @param inDateFrom Date to throttle points.
	 * @param inDateTo Date to throttle points.
	 * @param inHasLifetime Are there any lifetime points?
	 * @return Whether any existing points exist.
	 */
	bool HasPoints(
			ARBConfigVenuePtr inVenue,
			ARBConfigDivisionPtr inDiv,
			ARBConfigLevelPtr inLevel,
			ARBConfigEventPtr inEvent,
			ARBDate inDateFrom,
			ARBDate inDateTo,
			bool inHasLifetime) const;

	/**
	 * Get the number of existing points.
	 * @param inType Type of existing points to tally.
	 * @param inVenue Venue to search for.
	 * @param inMultiQ MultiQ to search for.
	 * @param inDiv Division to search for.
	 * @param inLevel Level to search for.
	 * @param inEvent Event to search for.
	 * @param inDateFrom Date to throttle points.
	 * @param inDateTo Date to throttle points.
	 * @return The number of existing points.
	 */
	double ExistingPoints(
			ARBDogExistingPoints::PointType inType,
			ARBConfigVenuePtr inVenue,
			ARBConfigMultiQPtr inMultiQ,
			ARBConfigDivisionPtr inDiv,
			ARBConfigLevelPtr inLevel,
			ARBConfigEventPtr inEvent,
			ARBDate inDateFrom,
			ARBDate inDateTo) const;

	/**
	 * Get the number of existing point items in a venue.
	 * @param inVenue Venue to search for.
	 * @return Number of objects, not points.
	 */
	int NumExistingPointsInVenue(ARBString const& inVenue) const;

	/**
	 * Rename a venue, rename any dependent objects.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items changed.
	 */
	int RenameVenue(
			ARBString const& inOldVenue,
			ARBString const& inNewVenue);

	/**
	 * Delete a venue, remove any dependent objects.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(ARBString const& inVenue);

	/**
	 * Get the number of existing point entries in a division.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @param inDiv Division to tally.
	 * @return Number of objects, not points.
	 */
	int NumExistingPointsInDivision(
			ARBConfigVenuePtr inVenue,
			ARBString const& inDiv) const;

	/**
	 * Rename a division.
	 * @param inVenue Venue whose division is being renamed.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of items updated.
	 */
	int RenameDivision(
			ARBString const& inVenue,
			ARBString const& inOldDiv,
			ARBString const& inNewDiv);

	/**
	 * Delete a division.
	 * @param inVenue Venue whose division is being renamed.
	 * @param inDiv Division name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteDivision(
			ARBString const& inVenue,
			ARBString const& inDiv);

	/**
	 * Number of levels in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inLevel Level to tally.
	 * @return Number of objects.
	 */
	int NumLevelsInUse(
			ARBString const& inVenue,
			ARBString const& inDiv,
			ARBString const& inLevel) const;

	/**
	 * Rename a level, rename any dependent objects.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inOldLevel Level name being renamed.
	 * @param inNewLevel New level name.
	 * @return Number of items changed.
	 */
	int RenameLevel(
			ARBString const& inVenue,
			ARBString const& inDiv,
			ARBString const& inOldLevel,
			ARBString const& inNewLevel);

	/**
	 * Delete a level, remove any dependent objects.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inLevel Level name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteLevel(
			ARBString const& inVenue,
			ARBString const& inDiv,
			ARBString const& inLevel);

	/**
	 * Number of events in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event to tally.
	 * @return Number of objects.
	 */
	int NumEventsInUse(
			ARBString const& inVenue,
			ARBString const& inEvent) const;

	/**
	 * Rename an event, rename any dependent objects.
	 * @param inVenue Venue title is in.
	 * @param inOldEvent Event name being renamed.
	 * @param inNewEvent New event name.
	 * @return Number of items changed.
	 */
	int RenameEvent(
			ARBString const& inVenue,
			ARBString const& inOldEvent,
			ARBString const& inNewEvent);

	/**
	 * Delete an event, remove any dependent objects.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteEvent(
			ARBString const& inVenue,
			ARBString const& inEvent);

	/**
	 * Number of OtherPoint objects in use.
	 * Used to warning about impending configuration changes.
	 * @param inOther Name of item to look for.
	 * @return Number of objects, not points.
	 */
	int NumOtherPointsInUse(ARBString const& inOther) const;

	/**
	 * Rename an OtherPoint, rename any dependent objects.
	 * @param inOldOther OtherPoint name being renamed.
	 * @param inNewOther New OtherPoint name.
	 * @return Number of items changed.
	 */
	int RenameOtherPoints(
			ARBString const& inOldOther,
			ARBString const& inNewOther);

	/**
	 * Delete an OtherPoint, remove any dependent objects.
	 * @param inOther OtherPoint name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteOtherPoints(ARBString const& inOther);

	/**
	 * Number of MultiQ objects in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue MultiQ is in.
	 * @param inMultiQ Name of item to look for.
	 * @return Number of objects, not points.
	 */
	int NumMultiQsInUse(
			ARBString const& inVenue,
			ARBString const& inMultiQ) const;

	/**
	 * Rename a MultiQ, rename any dependent objects.
	 * @param inVenue Venue MultiQ is in.
	 * @param inOldMultiQ MultiQ name being renamed.
	 * @param inNewMultiQ New MultiQ name.
	 * @return Number of items changed.
	 */
	int RenameMultiQs(
			ARBString const& inVenue,
			ARBString const& inOldMultiQ,
			ARBString const& inNewMultiQ);

	/**
	 * Delete an MultiQ, remove any dependent objects.
	 * @param inConfig Configuration for looking up information.
	 * @param inVenue Venue in which MultiQ was deleted.
	 * @return Number of items removed.
	 */
	int DeleteMultiQs(
			ARBConfig const& inConfig,
			ARBString const& inVenue);

	/**
	 * Add an existing point object.
	 * @param inExistingPoints Object to add.
	 * @return Whether the object was added.
	 */
	bool AddExistingPoints(ARBDogExistingPointsPtr inExistingPoints);

	/**
	 * Delete an existing point object.
	 * @param inExistingPoints Object to delete.
	 * @return Whether object was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteExistingPoints(ARBDogExistingPointsPtr inExistingPoints);
};
