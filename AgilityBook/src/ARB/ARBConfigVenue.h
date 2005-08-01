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
 * @brief ARBConfigVenue and ARBConfigVenueList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-02-02 DRC Added VerifyEvent.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <string>
#include "ARBBase.h"
#include "ARBConfigDivision.h"
#include "ARBConfigEvent.h"
#include "ARBConfigMultiQ.h"
#include "ARBVector.h"
class ARBErrorCallback;
class ARBVersion;
class Element;

/**
 * Venue configuration information.
 */
class ARBConfigVenue : public ARBBase
{
public:
	ARBConfigVenue();
	ARBConfigVenue(ARBConfigVenue const& rhs);
	ARBConfigVenue& operator=(ARBConfigVenue const& rhs);
	bool operator==(ARBConfigVenue const& rhs) const;
	bool operator!=(ARBConfigVenue const& rhs) const;

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
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfig& ioConfig,
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

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
	bool Update(
			int indent,
			ARBConfigVenue const* inVenueNew,
			std::string& ioInfo);

	/*
	 * Getters/setters.
	 */
	std::string const& GetName() const;
	void SetName(std::string const& inName);
	std::string const& GetLongName() const;
	void SetLongName(std::string const& inName);
	std::string const& GetDesc() const;
	void SetDesc(std::string const& inDesc);
	ARBConfigDivisionList const& GetDivisions() const;
	ARBConfigDivisionList& GetDivisions();
	ARBConfigEventList const& GetEvents() const;
	ARBConfigEventList& GetEvents();
	ARBConfigMultiQList const& GetMultiQs() const;
	ARBConfigMultiQList& GetMultiQs();

private:
	~ARBConfigVenue();
	std::string m_Name;
	std::string m_LongName;
	std::string m_Desc;
	ARBConfigDivisionList m_Divisions;
	ARBConfigEventList m_Events;
	ARBConfigMultiQList m_MultiQs;
};

inline std::string ARBConfigVenue::GetGenericName() const
{
	return m_Name;
}

inline std::string const& ARBConfigVenue::GetName() const
{
	return m_Name;
}

inline void ARBConfigVenue::SetName(std::string const& inName)
{
	m_Name = inName;
}

inline std::string const& ARBConfigVenue::GetLongName() const
{
	return m_LongName;
}

inline void ARBConfigVenue::SetLongName(std::string const& inName)
{
	m_LongName = inName;
}

inline std::string const& ARBConfigVenue::GetDesc() const
{
	return m_Desc;
}

inline void ARBConfigVenue::SetDesc(std::string const& inDesc)
{
	m_Desc = inDesc;
}

inline ARBConfigDivisionList const& ARBConfigVenue::GetDivisions() const
{
	return m_Divisions;
}

inline ARBConfigDivisionList& ARBConfigVenue::GetDivisions()
{
	return m_Divisions;
}

inline ARBConfigEventList const& ARBConfigVenue::GetEvents() const
{
	return m_Events;
}

inline ARBConfigEventList& ARBConfigVenue::GetEvents()
{
	return m_Events;
}

inline ARBConfigMultiQList const& ARBConfigVenue::GetMultiQs() const
{
	return m_MultiQs;
}

inline ARBConfigMultiQList& ARBConfigVenue::GetMultiQs()
{
	return m_MultiQs;
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
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfig& ioConfig,
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

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
	bool VerifyVenue(std::string const& inVenue) const;

	/**
	 * Verify a level exists.
	 * @param inVenue Venue level exists in.
	 * @param inDivision Division level exists in.
	 * @param inLevel Level to verify.
	 * @return Level exists.
	 */
	bool VerifyLevel(
			std::string const& inVenue,
			std::string const& inDivision,
			std::string const& inLevel) const;

	/**
	 * Verify an event exists.
	 * @param inVenue Venue event exists in.
	 * @param inDivision Division event exists in.
	 * @param inLevel Level event exists in.
	 * @param inEvent Event to verify.
	 * @return Event exists.
	 */
	bool VerifyEvent(
			std::string const& inVenue,
			std::string const& inDivision,
			std::string const& inLevel,
			std::string const& inEvent) const;

	/**
	 * Find a title by the complete name.
	 * This api is used to fix a problem introduced in v1.0.0.8.
	 * @param inVenue Venue title is in.
	 * @param inName Complete name of title to find.
	 * @param bAbbrevFirst Name is before or after Longname.
	 * @param outTitle Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindTitleCompleteName(
			std::string const& inVenue,
			std::string const& inName,
			bool bAbbrevFirst = true,
			ARBConfigTitle** outTitle = NULL) const;

	/**
	 * Find a title.
	 * @param inVenue Venue title is in.
	 * @param inTitle Name of title to find.
	 * @param outTitle Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindTitle(
			std::string const& inVenue,
			std::string const& inTitle,
			ARBConfigTitle** outTitle = NULL) const;

	/**
	 * Find the named venue.
	 * @param inVenue Venue to find.
	 * @param outVenue Object that was found.
	 * @return Whether the object was found.
	 */
	bool FindVenue(
			std::string const& inVenue,
			ARBConfigVenue** outVenue = NULL) const;

	/**
	 * Add a venue.
	 * @param inVenue Name of venue to add.
	 * @param outVenue Pointer to new object, NULL if name already exists or is empty.
	 * @return Whether the object was added.
	 */
	bool AddVenue(
			std::string const& inVenue,
			ARBConfigVenue** outVenue = NULL);

	/**
	 * Add a venue.
	 * @param inVenue Venue to add.
	 * @return Whether the object was added.
	 */
	bool AddVenue(ARBConfigVenue* inVenue);

	/**
	 * Delete a venue.
	 * @param inVenue Name of venue to delete.
	 * @return Number of venues deleted (0 or 1).
	 */
	int DeleteVenue(std::string const& inVenue);

	/**
	 * Find an event.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event name
	 * @param inDivision Division name
	 * @param inLevel True Level name (sublevel)
	 * @param inDate Date for requested scoring.
	 * @param outScoring Pointer to object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindEvent(
			std::string const& inVenue,
			std::string const& inEvent,
			std::string const& inDivision,
			std::string const& inLevel,
			ARBDate const& inDate,
			ARBConfigScoring** outScoring = NULL) const;
};
