#pragma once

/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-02-02 DRC Created
 */

#include <string>
#include "ARBBase.h"
#include "ARBDate.h"
#include "ARBTypes.h"
#include "ARBVector.h"
class ARBConfig;
class ARBConfigDivision;
class ARBConfigEvent;
class ARBConfigLevel;
class ARBConfigVenue;
class ARBErrorCallback;
class ARBVersion;
class Element;

/**
 * Initializes the titling points a dog has.
 */
class ARBDogExistingPoints : public ARBBase
{
public:
	ARBDogExistingPoints();
	ARBDogExistingPoints(ARBDogExistingPoints const& rhs);
	ARBDogExistingPoints& operator=(ARBDogExistingPoints const& rhs);
	bool operator==(ARBDogExistingPoints const& rhs) const;
	bool operator!=(ARBDogExistingPoints const& rhs) const;

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
		eOtherPoints,	///< OtherPoint points.
		eRuns,			///< Number of titling points for a run.
		eMach,			///< Number of MACH points.
		eQQ,			///< Number of double Qs.
		eSQ				///< Number of Super Qs.
	} PointType;

	/**
	 * Translate the enum to a string.
	 */
	static std::string GetPointTypeName(PointType inType);

	/*
	 * Getters/setters.
	 */
	PointType GetType() const;
	void SetType(PointType inType);
	ARBDate const& GetDate() const;
	void SetDate(ARBDate const& inDate);
	std::string const& GetComment() const;
	void SetComment(std::string const& inComment);
	std::string const& GetOtherPoints() const;
	void SetOtherPoints(std::string const& inOther);
	std::string const& GetVenue() const;
	void SetVenue(std::string const& inVenue);
	std::string const& GetDivision() const;
	void SetDivision(std::string const& inDiv);
	std::string const& GetLevel() const;
	void SetLevel(std::string const& inLevel);
	std::string const& GetEvent() const;
	void SetEvent(std::string const& inEvent);
	std::string const& GetSubCategory() const;
	void SetSubCategory(std::string const& inSubCat);
	short GetPoints() const;
	void SetPoints(short inPoints);

private:
	~ARBDogExistingPoints();
	ARBDate m_Date;
	std::string m_Comment;
	PointType m_Type;
	std::string m_Other;
	std::string m_Venue;
	std::string m_Div;
	std::string m_Level;
	std::string m_Event;
	std::string m_SubCategory;
	short m_Points;
};

inline ARBDogExistingPoints::PointType ARBDogExistingPoints::GetType() const
{
	return m_Type;
}

inline void ARBDogExistingPoints::SetType(ARBDogExistingPoints::PointType inType)
{
	m_Type = inType;
}

inline ARBDate const& ARBDogExistingPoints::GetDate() const
{
	return m_Date;
}

inline void ARBDogExistingPoints::SetDate(ARBDate const& inDate)
{
	m_Date = inDate;
}

inline std::string const& ARBDogExistingPoints::GetComment() const
{
	return m_Comment;
}

inline void ARBDogExistingPoints::SetComment(std::string const& inComment)
{
	m_Comment = inComment;
}

inline std::string const& ARBDogExistingPoints::GetOtherPoints() const
{
	return m_Other;
}

inline void ARBDogExistingPoints::SetOtherPoints(std::string const& inOther)
{
	m_Other = inOther;
}

inline std::string const& ARBDogExistingPoints::GetVenue() const
{
	return m_Venue;
}

inline void ARBDogExistingPoints::SetVenue(std::string const& inVenue)
{
	m_Venue = inVenue;
}

inline std::string const& ARBDogExistingPoints::GetDivision() const
{
	return m_Div;
}

inline void ARBDogExistingPoints::SetDivision(std::string const& inDiv)
{
	m_Div = inDiv;
}

inline std::string const& ARBDogExistingPoints::GetLevel() const
{
	return m_Level;
}

inline void ARBDogExistingPoints::SetLevel(std::string const& inLevel)
{
	m_Level = inLevel;
}

inline std::string const& ARBDogExistingPoints::GetEvent() const
{
	return m_Event;
}

inline void ARBDogExistingPoints::SetEvent(std::string const& inEvent)
{
	m_Event = inEvent;
}

inline std::string const& ARBDogExistingPoints::GetSubCategory() const
{
	return m_SubCategory;
}

inline void ARBDogExistingPoints::SetSubCategory(std::string const& inSubCat)
{
	m_SubCategory = inSubCat;
}

inline short ARBDogExistingPoints::GetPoints() const
{
	return m_Points;
}

inline void ARBDogExistingPoints::SetPoints(short inPoints)
{
	m_Points = inPoints;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogExistingPoints objects.
 */
class ARBDogExistingPointsList : public ARBVectorLoad2<ARBDogExistingPoints>
{
public:
	/**
	 * Sort by points/venue/division/level.
	 */
	void sort();

	/**
	 * Are there any existing points for the specified venue?
	 * @param inVenue Venue to search for.
	 * @return Whether any existing points exist.
	 */
	bool HasPoints(std::string const& inVenue) const;

	/**
	 * Are there any existing points?
	 * @param inVenue Venue to search for.
	 * @param inDiv Division to search for.
	 * @param inLevel Level to search for.
	 * @param inEvent Event to search for.
	 * @param inHasLifetime Are there any lifetime points?
	 * @return Whether any existing points exist.
	 */
	bool HasPoints(
		ARBConfigVenue const* inVenue,
		ARBConfigDivision const* inDiv,
		ARBConfigLevel const* inLevel,
		ARBConfigEvent const* inEvent,
		bool inHasLifetime) const;

	/**
	 * Get the number of existing points.
	 * @param inType Type of existing points to tally.
	 * @param inVenue Venue to search for.
	 * @param inDiv Division to search for.
	 * @param inLevel Level to search for.
	 * @param inEvent Event to search for.
	 * @return The number of existing points.
	 */
	short ExistingPoints(
		ARBDogExistingPoints::PointType inType,
		ARBConfigVenue const* inVenue,
		ARBConfigDivision const* inDiv,
		ARBConfigLevel const* inLevel,
		ARBConfigEvent const* inEvent) const;

	/**
	 * Get the number of existing point items in a venue.
	 * @param inVenue Venue to search for.
	 * @return Number of objects, not points.
	 */
	int NumExistingPointsInVenue(std::string const& inVenue) const;

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
	 * Rename a division.
	 * @param inVenue Venue whose division is being renamed.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of items updated.
	 */
	int RenameDivision(
		std::string const& inVenue,
		std::string const& inOldDiv,
		std::string const& inNewDiv);

	/**
	 * Delete a division.
	 * @param inVenue Venue whose division is being renamed.
	 * @param inDiv Division name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteDivision(
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
	 * Add an existing point object.
	 * @param inExistingPoints Object to add.
	 * @return Pointer to object.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 *       The pointer is added to the list and its ref count is incremented.
	 */
	ARBDogExistingPoints* AddExistingPoints(ARBDogExistingPoints* inExistingPoints);

	/**
	 * Delete an existing point object.
	 * @param inExistingPoints Object to delete.
	 * @return Whether object was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteExistingPoints(ARBDogExistingPoints const* inExistingPoints);
};
