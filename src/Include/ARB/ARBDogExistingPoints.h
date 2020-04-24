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
 * 2016-04-29 Separate lifetime points from title (run) points.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-02-02 Created
 */

#include "ARBBase.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"

#include "ARBCommon/ARBDate.h"


/**
 * Type of points.
 */
enum class ARBExistingPointType
{
	Unknown,
	OtherPoints, ///< Number of OtherPoint points.
	Lifetime,    ///< Number of Lifetime points.
	Title,       ///< Number of Titling points.
	Speed,       ///< Number of Speed points.
	MQ,          ///< Number of multi Qs.
	SQ           ///< Number of Super Qs.
};


/**
 * Initializes the titling points a dog has.
 */
class ARB_API ARBDogExistingPoints : public ARBBase
{
protected:
	ARBDogExistingPoints();
	ARBDogExistingPoints(ARBDogExistingPoints const& rhs);
	ARBDogExistingPoints(ARBDogExistingPoints&& rhs);

public:
	~ARBDogExistingPoints();
	static ARBDogExistingPointsPtr New();
	ARBDogExistingPointsPtr Clone() const;

	ARBDogExistingPoints& operator=(ARBDogExistingPoints const& rhs);
	ARBDogExistingPoints& operator=(ARBDogExistingPoints&& rhs);

	bool operator==(ARBDogExistingPoints const& rhs) const;
	bool operator!=(ARBDogExistingPoints const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	std::wstring GetGenericName() const override
	{
		std::wstring name;
		if (0 < m_TypeName.length())
			name = m_TypeName;
		else
			name = m_Event;
		return name;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const override
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
		ElementNodePtr const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogExistingPoints element will be created in ioTree.
	 */
	bool Save(ElementNodePtr const& ioTree) const;

	/**
	 * Translate the enum to a string.
	 */
	static std::wstring GetPointTypeName(ARBExistingPointType inType);

	/*
	 * Getters/setters.
	 */
	ARBExistingPointType GetType() const
	{
		return m_Type;
	}
	void SetType(ARBExistingPointType inType)
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
	std::wstring const& GetComment() const
	{
		return m_Comment;
	}
	void SetComment(std::wstring const& inComment)
	{
		m_Comment = inComment;
	}
	std::wstring const& GetTypeName() const
	{
		return m_TypeName;
	}
	void SetTypeName(std::wstring const& inTypeName)
	{
		m_TypeName = inTypeName;
	}
	std::wstring const& GetVenue() const
	{
		return m_Venue;
	}
	void SetVenue(std::wstring const& inVenue)
	{
		m_Venue = inVenue;
	}
	std::wstring const& GetMultiQ() const
	{
		return m_MultiQ;
	}
	void SetMultiQ(std::wstring const& inMultiQ)
	{
		m_MultiQ = inMultiQ;
	}
	std::wstring const& GetDivision() const
	{
		return m_Div;
	}
	void SetDivision(std::wstring const& inDiv)
	{
		m_Div = inDiv;
	}
	std::wstring const& GetLevel() const
	{
		return m_Level;
	}
	void SetLevel(std::wstring const& inLevel)
	{
		m_Level = inLevel;
	}
	std::wstring const& GetEvent() const
	{
		return m_Event;
	}
	void SetEvent(std::wstring const& inEvent)
	{
		m_Event = inEvent;
	}
	std::wstring const& GetSubName() const
	{
		return m_SubName;
	}
	void SetSubName(std::wstring const& inSubName)
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
	std::wstring m_Comment;
	ARBExistingPointType m_Type;
	std::wstring m_TypeName; // Name of OtherPoints or Lifetime item
	std::wstring m_Venue;
	std::wstring m_MultiQ;
	std::wstring m_Div;
	std::wstring m_Level;
	std::wstring m_Event;
	std::wstring m_SubName;
	double m_Points;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogExistingPoints objects.
 */
class ARB_API ARBDogExistingPointsList : public ARBVector<ARBDogExistingPointsPtr>
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
		ElementNodePtr const& inTree,
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
	bool HasPoints(std::wstring const& inVenue) const;

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
		ARBConfigVenuePtr const& inVenue,
		ARBConfigDivisionPtr const& inDiv,
		ARBConfigLevelPtr const& inLevel,
		ARBConfigEventPtr const& inEvent,
		ARBDate inDateFrom,
		ARBDate inDateTo,
		bool inHasLifetime) const;

	/**
	 * Get the number of existing points.
	 * @param inType Type of existing points to tally, not lifetime
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
		ARBExistingPointType inType,
		ARBConfigVenuePtr const& inVenue,
		ARBConfigMultiQPtr const& inMultiQ,
		ARBConfigDivisionPtr const& inDiv,
		ARBConfigLevelPtr const& inLevel,
		ARBConfigEventPtr const& inEvent,
		ARBDate inDateFrom,
		ARBDate inDateTo) const;

	/**
	 * Get the number of existing points.
	 * @param inName Name of lifetime points to tally.
	 * @param inVenue Venue to search for.
	 * @param inDiv Division to search for.
	 * @param inLevel Level to search for.
	 * @param inEvent Event to search for.
	 * @param inDateFrom Date to throttle points.
	 * @param inDateTo Date to throttle points.
	 * @return The number of existing points.
	 */
	double ExistingLifetimePoints(
		ARBConfigLifetimeNamePtr const& inName,
		ARBConfigVenuePtr const& inVenue,
		ARBConfigDivisionPtr const& inDiv,
		ARBConfigLevelPtr const& inLevel,
		ARBConfigEventPtr const& inEvent,
		ARBDate inDateFrom,
		ARBDate inDateTo) const;

	/**
	 * Get the number of existing point items in a venue.
	 * @param inVenue Venue to search for.
	 * @return Number of objects, not points.
	 */
	int NumExistingPointsInVenue(std::wstring const& inVenue) const;

	/**
	 * Rename a venue, rename any dependent objects.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items changed.
	 */
	int RenameVenue(std::wstring const& inOldVenue, std::wstring const& inNewVenue);

	/**
	 * Delete a venue, remove any dependent objects.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(std::wstring const& inVenue);

	/**
	 * Get the number of existing point entries in a division.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @param inDiv Division to tally.
	 * @return Number of objects, not points.
	 */
	int NumExistingPointsInDivision(ARBConfigVenuePtr const& inVenue, std::wstring const& inDiv) const;

	/**
	 * Rename a division.
	 * @param inVenue Venue whose division is being renamed.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of items updated.
	 */
	int RenameDivision(std::wstring const& inVenue, std::wstring const& inOldDiv, std::wstring const& inNewDiv);

	/**
	 * Delete a division.
	 * @param inVenue Venue whose division is being renamed.
	 * @param inDiv Division name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteDivision(std::wstring const& inVenue, std::wstring const& inDiv);

	/**
	 * Number of levels in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue level is in.
	 * @param inDiv Division level is in.
	 * @param inLevel Level to tally.
	 * @return Number of objects.
	 */
	int NumLevelsInUse(std::wstring const& inVenue, std::wstring const& inDiv, std::wstring const& inLevel) const;

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
	int DeleteLevel(std::wstring const& inVenue, std::wstring const& inDiv, std::wstring const& inLevel);

	/**
	 * Number of events in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event to tally.
	 * @return Number of objects.
	 */
	int NumEventsInUse(std::wstring const& inVenue, std::wstring const& inEvent) const;

	/**
	 * Rename an event, rename any dependent objects.
	 * @param inVenue Venue title is in.
	 * @param inOldEvent Event name being renamed.
	 * @param inNewEvent New event name.
	 * @return Number of items changed.
	 */
	int RenameEvent(std::wstring const& inVenue, std::wstring const& inOldEvent, std::wstring const& inNewEvent);

	/**
	 * Delete an event, remove any dependent objects.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteEvent(std::wstring const& inVenue, std::wstring const& inEvent);

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
	int RenameOtherPoints(std::wstring const& inOldOther, std::wstring const& inNewOther);

	/**
	 * Delete an OtherPoint, remove any dependent objects.
	 * @param inOther OtherPoint name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteOtherPoints(std::wstring const& inOther);

	/**
	 * Number of Lifetime objects in use.
	 * Used to warning about impending configuration changes.
	 * @param inLifetime Name of item to look for.
	 * @return Number of objects, not points.
	 */
	int NumLifetimePointsInUse(std::wstring const& inLifetime) const;

	/**
	 * Rename a Lifetime name.
	 * @param inVenue Venue name is in.
	 * @param inOldName Lifetime name being renamed.
	 * @param inNewName New Lifetime name.
	 * @return Number of items changed.
	 */
	int RenameLifetimeName(std::wstring const& inVenue, std::wstring const& inOldName, std::wstring const& inNewName);

	/**
	 * Delete a Lifetime name.
	 * @param inVenue Venue name is in.
	 * @param inName Lifetime name to delete.
	 * @return Number of items removed.
	 */
	int DeleteLifetimeName(std::wstring const& inVenue, std::wstring const& inName);

	/**
	 * Number of MultiQ objects in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue MultiQ is in.
	 * @param inMultiQ Name of item to look for.
	 * @return Number of objects, not points.
	 */
	int NumMultiQsInUse(std::wstring const& inVenue, std::wstring const& inMultiQ) const;

	/**
	 * Rename a MultiQ, rename any dependent objects.
	 * @param inVenue Venue MultiQ is in.
	 * @param inOldMultiQ MultiQ name being renamed.
	 * @param inNewMultiQ New MultiQ name.
	 * @return Number of items changed.
	 */
	int RenameMultiQs(std::wstring const& inVenue, std::wstring const& inOldMultiQ, std::wstring const& inNewMultiQ);

	/**
	 * Delete any MultiQs no longer in venue.
	 * @param inConfig Configuration for looking up information.
	 * @param inVenue Venue in which MultiQ was deleted.
	 * @return Number of items removed.
	 */
	int DeleteMultiQs(ARBConfig const& inConfig, std::wstring const& inVenue);

	/**
	 * Add an existing point object.
	 * @param inExistingPoints Object to add.
	 * @return Whether the object was added.
	 */
	bool AddExistingPoints(ARBDogExistingPointsPtr const& inExistingPoints);

	/**
	 * Delete an existing point object.
	 * @param inExistingPoints Object to delete.
	 * @return Whether object was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteExistingPoints(ARBDogExistingPointsPtr const& inExistingPoints);
};
