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
 * @brief ARBConfigVenue and ARBConfigVenueList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2004-02-02 DRC Added VerifyEvent.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <string>
#include "ARBBase.h"
#include "ARBConfigDivision.h"
#include "ARBConfigEvent.h"
#include "ARBVector.h"
class ARBVersion;
class Element;

/**
 * Venue configuration information.
 */
class ARBConfigVenue : public ARBBase
{
public:
	ARBConfigVenue();
	ARBConfigVenue(const ARBConfigVenue& rhs);
	ARBConfigVenue& operator=(const ARBConfigVenue& rhs);
	bool operator==(const ARBConfigVenue& rhs) const;
	bool operator!=(const ARBConfigVenue& rhs) const;

	/**
	 * Reset the contents of this object and all sub-objects.
	 */
	void clear();

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
	 * Load a venue configuration.
	 * @pre inTree is the actual ARBConfigVenue element.
	 * @param ioConfig When migrating an old file, we may be updating the config.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool Load(
		ARBConfig& ioConfig,
		const Element& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigDivision element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Update this configuration from inVenueNew.
	 * @param indent Indentation level for generating messages.
	 * @param inVenueNew Venue to merge.
	 * @param ioInfo Accumulated messages about changes that have happened.
	 * @return Whether or not changes have occurred.
	 */
	bool Update(int indent, const ARBConfigVenue* inVenueNew, std::string& ioInfo);

	/*
	 * Getters/setters.
	 */
	const std::string GetName() const;
	void SetName(std::string inName);
	const std::string GetDesc() const;
	void SetDesc(std::string inDesc);
	const ARBConfigDivisionList& GetDivisions() const;
	ARBConfigDivisionList& GetDivisions();
	const ARBConfigEventList& GetEvents() const;
	ARBConfigEventList& GetEvents();

private:
	~ARBConfigVenue();
	std::string m_Name;
	std::string m_Desc;
	ARBConfigDivisionList m_Divisions;
	ARBConfigEventList m_Events;
};

inline std::string ARBConfigVenue::GetGenericName() const
{
	return m_Name;
}

inline const std::string ARBConfigVenue::GetName() const
{
	return m_Name;
}

inline void ARBConfigVenue::SetName(std::string inName)
{
	m_Name = inName;
}

inline const std::string ARBConfigVenue::GetDesc() const
{
	return m_Desc;
}

inline void ARBConfigVenue::SetDesc(std::string inDesc)
{
	m_Desc = inDesc;
}

inline const ARBConfigDivisionList& ARBConfigVenue::GetDivisions() const
{
	return m_Divisions;
}

inline ARBConfigDivisionList& ARBConfigVenue::GetDivisions()
{
	return m_Divisions;
}

inline const ARBConfigEventList& ARBConfigVenue::GetEvents() const
{
	return m_Events;
}

inline ARBConfigEventList& ARBConfigVenue::GetEvents()
{
	return m_Events;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigVenue objects.
 */
class ARBConfigVenueList : public ARBVector<ARBConfigVenue>
{
public:
	/**
	 * Load a venue configuration.
	 * @pre inTree is the actual ARBConfigVenue element.
	 * @param ioConfig When migrating an old file, we may be updating the config.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool Load(
		ARBConfig& ioConfig,
		const Element& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	bool operator==(const ARBConfigVenueList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBConfigVenueList& rhs) const
	{
		return !isEqual(rhs);
	}

	/**
	 * Sort the list by name.
	 * @param inDescending Sort in descending or ascending order.
	 */
	void sort(bool inDescending = true);

	/**
	 * Verify a venue exists.
	 * @param inVenue Venue to verify.
	 * @return Venue exists.
	 */
	bool VerifyVenue(const std::string& inVenue) const;

	/**
	 * Verify a level exists.
	 * @param inVenue Venue level exists in.
	 * @param inDivision Division level exists in.
	 * @param inLevel Level to verify.
	 * @return Level exists.
	 */
	bool VerifyLevel(
		const std::string& inVenue,
		const std::string& inDivision,
		const std::string& inLevel
		) const;

	/**
	 * Verify an event exists.
	 * @param inVenue Venue event exists in.
	 * @param inDivision Division event exists in.
	 * @param inLevel Level event exists in.
	 * @param inEvent Event to verify.
	 * @return Event exists.
	 */
	bool VerifyEvent(
		const std::string& inVenue,
		const std::string& inDivision,
		const std::string& inLevel,
		const std::string& inEvent
		) const;

	/**
	 * Find a title by the complete name.
	 * This api is used to fix a problem introduced in v1.0.0.8.
	 * @param inVenue Venue title is in.
	 * @param inName Complete name of title to find.
	 * @param bAbbrevFirst Name is before or after Longname.
	 * @return Pointer to found object, NULL if not found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	const ARBConfigTitle* FindTitleCompleteName(
		const std::string& inVenue,
		const std::string& inName,
		bool bAbbrevFirst = true) const;

	/**
	 * Find a title.
	 * @param inVenue Venue title is in.
	 * @param inTitle Name of title to find.
	 * @return Pointer to found object, NULL if not found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	const ARBConfigTitle* FindTitle(
		const std::string& inVenue,
		const std::string& inTitle) const;

	/**
	 * Find the named venue.
	 * @param inVenue Venue to find.
	 * @return Object that was found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	const ARBConfigVenue* FindVenue(const std::string& inVenue) const;
	ARBConfigVenue* FindVenue(const std::string& inVenue);

	/**
	 * Add a venue.
	 * @param inVenue Name of venue to add.
	 * @return Pointer to new object, NULL if name already exists or is empty.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBConfigVenue* AddVenue(const std::string& inVenue);

	/**
	 * Add a venue.
	 * @param inVenue Venue to add.
	 * @return Pointer to object, NULL if name already exists or is empty.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 *       The pointer is added to the list and its ref count is incremented.
	 */
	ARBConfigVenue* AddVenue(ARBConfigVenue* inVenue);

	/**
	 * Delete a venue.
	 * @param inVenue Name of venue to delete.
	 * @return Number of venues deleted (0 or 1).
	 */
	int DeleteVenue(const std::string& inVenue);

	/**
	 * Find an event.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event name
	 * @param inDivision Division name
	 * @param inLevel True Level name (sublevel)
	 * @param inDate Date for requested scoring.
	 * @return Pointer to object, NULL if not found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	const ARBConfigScoring* FindEvent(
		const std::string& inVenue,
		const std::string& inEvent,
		const std::string& inDivision,
		const std::string& inLevel,
		const ARBDate& inDate) const;
};
