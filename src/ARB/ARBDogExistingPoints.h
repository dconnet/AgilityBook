#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBDogExistingPoints and ARBDogExistingPointsList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
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
	static ARBDogExistingPointsPtr New();
	ARBDogExistingPointsPtr Clone() const;

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
	virtual wxString GetGenericName() const
	{
		wxString name;
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
	virtual size_t GetSearchStrings(std::set<wxString>& ioStrings) const
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
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogExistingPoints element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

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
	static wxString GetPointTypeName(PointType inType);

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
	wxString const& GetComment() const
	{
		return m_Comment;
	}
	void SetComment(wxString const& inComment)
	{
		m_Comment = inComment;
	}
	wxString const& GetOtherPoints() const
	{
		return m_Other;
	}
	void SetOtherPoints(wxString const& inOther)
	{
		m_Other = inOther;
	}
	wxString const& GetVenue() const
	{
		return m_Venue;
	}
	void SetVenue(wxString const& inVenue)
	{
		m_Venue = inVenue;
	}
	wxString const& GetMultiQ() const
	{
		return m_MultiQ;
	}
	void SetMultiQ(wxString const& inMultiQ)
	{
		m_MultiQ = inMultiQ;
	}
	wxString const& GetDivision() const
	{
		return m_Div;
	}
	void SetDivision(wxString const& inDiv)
	{
		m_Div = inDiv;
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
	wxString m_Comment;
	PointType m_Type;
	wxString m_Other;
	wxString m_Venue;
	wxString m_MultiQ;
	wxString m_Div;
	wxString m_Level;
	wxString m_Event;
	wxString m_SubName;
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
			ElementNodePtr inTree,
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
	bool HasPoints(wxString const& inVenue) const;

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
	int NumExistingPointsInVenue(wxString const& inVenue) const;

	/**
	 * Rename a venue, rename any dependent objects.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items changed.
	 */
	int RenameVenue(
			wxString const& inOldVenue,
			wxString const& inNewVenue);

	/**
	 * Delete a venue, remove any dependent objects.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(wxString const& inVenue);

	/**
	 * Get the number of existing point entries in a division.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @param inDiv Division to tally.
	 * @return Number of objects, not points.
	 */
	int NumExistingPointsInDivision(
			ARBConfigVenuePtr inVenue,
			wxString const& inDiv) const;

	/**
	 * Rename a division.
	 * @param inVenue Venue whose division is being renamed.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of items updated.
	 */
	int RenameDivision(
			wxString const& inVenue,
			wxString const& inOldDiv,
			wxString const& inNewDiv);

	/**
	 * Delete a division.
	 * @param inVenue Venue whose division is being renamed.
	 * @param inDiv Division name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteDivision(
			wxString const& inVenue,
			wxString const& inDiv);

	/**
	 * Number of levels in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inLevel Level to tally.
	 * @return Number of objects.
	 */
	int NumLevelsInUse(
			wxString const& inVenue,
			wxString const& inDiv,
			wxString const& inLevel) const;

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
	 * @param inLevel Level name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteLevel(
			wxString const& inVenue,
			wxString const& inDiv,
			wxString const& inLevel);

	/**
	 * Number of events in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event to tally.
	 * @return Number of objects.
	 */
	int NumEventsInUse(
			wxString const& inVenue,
			wxString const& inEvent) const;

	/**
	 * Rename an event, rename any dependent objects.
	 * @param inVenue Venue title is in.
	 * @param inOldEvent Event name being renamed.
	 * @param inNewEvent New event name.
	 * @return Number of items changed.
	 */
	int RenameEvent(
			wxString const& inVenue,
			wxString const& inOldEvent,
			wxString const& inNewEvent);

	/**
	 * Delete an event, remove any dependent objects.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteEvent(
			wxString const& inVenue,
			wxString const& inEvent);

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
	int RenameOtherPoints(
			wxString const& inOldOther,
			wxString const& inNewOther);

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
	int NumMultiQsInUse(
			wxString const& inVenue,
			wxString const& inMultiQ) const;

	/**
	 * Rename a MultiQ, rename any dependent objects.
	 * @param inVenue Venue MultiQ is in.
	 * @param inOldMultiQ MultiQ name being renamed.
	 * @param inNewMultiQ New MultiQ name.
	 * @return Number of items changed.
	 */
	int RenameMultiQs(
			wxString const& inVenue,
			wxString const& inOldMultiQ,
			wxString const& inNewMultiQ);

	/**
	 * Delete any MultiQs no longer in venue.
	 * @param inConfig Configuration for looking up information.
	 * @param inVenue Venue in which MultiQ was deleted.
	 * @return Number of items removed.
	 */
	int DeleteMultiQs(
			ARBConfig const& inConfig,
			wxString const& inVenue);

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
