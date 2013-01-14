#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBConfigVenue and ARBConfigVenueList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2007-10-10 DRC Added 'LifetimeName'
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-02-02 DRC Added VerifyEvent.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBConfigDivision.h"
#include "ARBConfigEvent.h"
#include "ARBConfigMultiQ.h"
#include "ARBConfigTitle.h"
#include "ARBTypes.h"
class ARBDate;

/**
 * Venue configuration information.
 */
class ARBConfigVenue : public ARBBase
{
protected:
	ARBConfigVenue();
	ARBConfigVenue(ARBConfigVenue const& rhs);

public:
	~ARBConfigVenue();
	static ARBConfigVenuePtr New();
	ARBConfigVenuePtr Clone() const;

	ARBConfigVenue& operator=(ARBConfigVenue const& rhs);

	bool operator==(ARBConfigVenue const& rhs) const;
	bool operator!=(ARBConfigVenue const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Reset the contents of this object and all sub-objects.
	 */
	void clear();

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual std::wstring GetGenericName() const
	{
		return m_Name;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const
	{
		return 0;
	}

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
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigDivision element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

	/**
	 * Update this configuration from inVenueNew.
	 * @param indent Indentation level for generating messages.
	 * @param inVenueNew Venue to merge.
	 * @param ioInfo Accumulated messages about changes that have happened.
	 * @return Whether or not changes have occurred.
	 */
	bool Update(
			int indent,
			ARBConfigVenuePtr inVenueNew,
			std::wstring& ioInfo);

	/*
	 * Getters/setters.
	 */
	std::wstring const& GetName() const
	{
		return m_Name;
	}
	void SetName(std::wstring const& inName)
	{
		m_Name = inName;
	}
	std::wstring const& GetLongName() const
	{
		return m_LongName;
	}
	void SetLongName(std::wstring const& inName)
	{
		m_LongName = inName;
	}
	std::wstring const& GetURL() const
	{
		return m_URL;
	}
	void SetURL(std::wstring const& inURL)
	{
		m_URL = inURL;
	}
	std::wstring const& GetDesc() const
	{
		return m_Desc;
	}
	void SetDesc(std::wstring const& inDesc)
	{
		m_Desc = inDesc;
	}
	bool HasLifetimeName() const
	{
		return !m_LifetimeName.empty();
	}
	std::wstring GetLifetimeName() const
	{
		return m_LifetimeName;
	}
	void SetLifetimeName(std::wstring const& name)
	{
		m_LifetimeName = name;
	}
	short GetIcon() const
	{
		return m_idxIcon;
	}
	void SetIcon(short icon)
	{
		m_idxIcon = icon;
	}
	ARBConfigTitleList const& GetTitles() const
	{
		return m_Titles;
	}
	ARBConfigTitleList& GetTitles()
	{
		return m_Titles;
	}
	ARBConfigDivisionList const& GetDivisions() const
	{
		return m_Divisions;
	}
	ARBConfigDivisionList& GetDivisions()
	{
		return m_Divisions;
	}
	ARBConfigEventList const& GetEvents() const
	{
		return m_Events;
	}
	ARBConfigEventList& GetEvents()
	{
		return m_Events;
	}
	ARBConfigMultiQList const& GetMultiQs() const
	{
		return m_MultiQs;
	}
	ARBConfigMultiQList& GetMultiQs()
	{
		return m_MultiQs;
	}

private:
	std::wstring m_Name;
	std::wstring m_LongName;
	std::wstring m_URL;
	std::wstring m_Desc;
	std::wstring m_LifetimeName;
	short m_idxIcon;
	ARBConfigTitleList m_Titles;
	ARBConfigDivisionList m_Divisions;
	ARBConfigEventList m_Events;
	ARBConfigMultiQList m_MultiQs;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigVenue objects.
 */
class ARBConfigVenueList : public ARBVector<ARBConfigVenuePtr>
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
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Sort the list by name.
	 */
	void sort();

	/**
	 * Verify a venue exists.
	 * @param inVenue Venue to verify.
	 * @return Venue exists.
	 */
	bool VerifyVenue(std::wstring const& inVenue) const
	{
		return FindVenue(inVenue);
	}

	/**
	 * Verify a multiQ exists.
	 * @param inVenue Venue to verify.
	 * @param inMultiQ MultiQ to verify.
	 * @param inUseShortName Search by shortname instead of longname.
	 * @return MultiQ exists.
	 */
	bool VerifyMultiQ(
			std::wstring const& inVenue,
			std::wstring const& inMultiQ,
			bool inUseShortName = false) const;

	/**
	 * Verify a level exists.
	 * @param inVenue Venue level exists in.
	 * @param inDivision Division level exists in.
	 * @param inLevel Level to verify.
	 * @return Level exists.
	 */
	bool VerifyLevel(
			std::wstring const& inVenue,
			std::wstring const& inDivision,
			std::wstring const& inLevel) const;

	/**
	 * Verify an event exists.
	 * @param inVenue Venue event exists in.
	 * @param inDivision Division event exists in.
	 * @param inLevel Level event exists in.
	 * @param inEvent Event to verify.
	 * @param inDate Date for matching methods.
	 * @return Event exists.
	 */
	bool VerifyEvent(
			std::wstring const& inVenue,
			std::wstring const& inDivision,
			std::wstring const& inLevel,
			std::wstring const& inEvent,
			ARBDate const& inDate) const;

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
			std::wstring const& inVenue,
			std::wstring const& inName,
			bool bAbbrevFirst = true,
			ARBConfigTitlePtr* outTitle = NULL) const;

	/**
	 * Find a title.
	 * @param inVenue Venue title is in.
	 * @param inTitle Name of title to find.
	 * @param outTitle Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindTitle(
			std::wstring const& inVenue,
			std::wstring const& inTitle,
			ARBConfigTitlePtr* outTitle = NULL) const;

	/**
	 * Delete a title.
	 * @param inTitle Name of title to delete.
	 * @return Whether title was deleted.
	 */
	bool DeleteTitle(std::wstring const& inTitle);

	/**
	 * Find the named venue.
	 * @param inVenue Venue to find.
	 * @param outVenue Object that was found.
	 * @return Whether the object was found.
	 */
	bool FindVenue(
			std::wstring const& inVenue,
			ARBConfigVenuePtr* outVenue = NULL) const;

	/**
	 * Add a venue.
	 * @param inVenue Name of venue to add.
	 * @param outVenue Pointer to new object, NULL if name already exists or is empty.
	 * @return Whether the object was added.
	 */
	bool AddVenue(
			std::wstring const& inVenue,
			ARBConfigVenuePtr* outVenue = NULL);

	/**
	 * Add a venue.
	 * @param inVenue Venue to add.
	 * @return Whether the object was added.
	 */
	bool AddVenue(ARBConfigVenuePtr inVenue);

	/**
	 * Delete a venue.
	 * @param inVenue Name of venue to delete.
	 * @return Number of venues deleted (0 or 1).
	 */
	int DeleteVenue(std::wstring const& inVenue);

	/**
	 * Find an event.
	 * @param inVenue Venue event is in.
	 * @param inEvent Event name
	 * @param inDivision Division name
	 * @param inLevel True Level name (sublevel)
	 * @param inDate Date for requested scoring.
	 * @param outEvent Pointer to object, NULL if not found.
	 * @param outScoring Pointer to object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindEvent(
			std::wstring const& inVenue,
			std::wstring const& inEvent,
			std::wstring const& inDivision,
			std::wstring const& inLevel,
			ARBDate const& inDate,
			ARBConfigEventPtr* outEvent = NULL,
			ARBConfigScoringPtr* outScoring = NULL) const;
};
