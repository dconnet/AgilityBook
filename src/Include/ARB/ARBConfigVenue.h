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
 * 2016-01-06 Removed LifetimeName, added LifetimeNames.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2007-10-10 Added 'LifetimeName'
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-14 Moved 'Titles' to 'Venue'.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-02-02 Added VerifyEvent.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-12-27 Changed FindEvent to take a date.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBConfigDivision.h"
#include "ARBConfigEvent.h"
#include "ARBConfigLifetimeName.h"
#include "ARBConfigMultiQ.h"
#include "ARBConfigTitle.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"


namespace dconSoft
{
namespace ARBCommon
{
class ARBDate;
} // namespace ARBCommon
namespace ARB
{

/**
 * Venue configuration information.
 */
class ARB_API ARBConfigVenue : public ARBBase
{
protected:
	ARBConfigVenue();
	ARBConfigVenue(ARBConfigVenue const& rhs);
	ARBConfigVenue(ARBConfigVenue&& rhs);

public:
	~ARBConfigVenue();
	static ARBConfigVenuePtr New();
	ARBConfigVenuePtr Clone() const;

	ARBConfigVenue& operator=(ARBConfigVenue const& rhs);
	ARBConfigVenue& operator=(ARBConfigVenue&& rhs);

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
	wxString GetGenericName() const override
	{
		return m_Name;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<wxString>& ioStrings) const override
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
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigDivision element will be created in ioTree.
	 */
	bool Save(ARBCommon::ElementNodePtr const& ioTree) const;

	/**
	 * Update this configuration from inVenueNew.
	 * @param indent Indentation level for generating messages.
	 * @param inVenueNew Venue to merge.
	 * @param ioInfo Accumulated messages about changes that have happened.
	 * @return Whether or not changes have occurred.
	 */
	bool Update(int indent, ARBConfigVenuePtr const& inVenueNew, wxString& ioInfo);

	/*
	 * Getters/setters.
	 */
	wxString const& GetName() const
	{
		return m_Name;
	}
	void SetName(wxString const& inName)
	{
		m_Name = inName;
	}
	wxString const& GetLongName() const
	{
		return m_LongName;
	}
	void SetLongName(wxString const& inName)
	{
		m_LongName = inName;
	}
	wxString const& GetURL() const
	{
		return m_URL;
	}
	void SetURL(wxString const& inURL)
	{
		m_URL = inURL;
	}
	wxString const& GetDesc() const
	{
		return m_Desc;
	}
	void SetDesc(wxString const& inDesc)
	{
		m_Desc = inDesc;
	}
	ARBConfigLifetimeNameList const& GetLifetimeNames() const
	{
		return m_LifetimeNames;
	}
	ARBConfigLifetimeNameList& GetLifetimeNames()
	{
		return m_LifetimeNames;
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
	wxString m_Name;
	wxString m_LongName;
	wxString m_URL;
	wxString m_Desc;
	ARBConfigLifetimeNameList m_LifetimeNames;
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
class ARB_API ARBConfigVenueList : public ARBVector<ARBConfigVenuePtr>
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
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
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
	bool VerifyVenue(wxString const& inVenue) const
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
	bool VerifyMultiQ(wxString const& inVenue, wxString const& inMultiQ, bool inUseShortName = false) const;

	/**
	 * Verify a level exists.
	 * @param inVenue Venue level exists in.
	 * @param inDivision Division level exists in.
	 * @param inLevel Level to verify.
	 * @return Level exists.
	 */
	bool VerifyLevel(wxString const& inVenue, wxString const& inDivision, wxString const& inLevel) const;

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
		wxString const& inVenue,
		wxString const& inDivision,
		wxString const& inLevel,
		wxString const& inEvent,
		ARBCommon::ARBDate const& inDate) const;

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
		wxString const& inVenue,
		wxString const& inName,
		bool bAbbrevFirst = true,
		ARBConfigTitlePtr* outTitle = nullptr) const;

	/**
	 * Find a title.
	 * @param inVenue Venue title is in.
	 * @param inTitle Name of title to find.
	 * @param outTitle Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindTitle(wxString const& inVenue, wxString const& inTitle, ARBConfigTitlePtr* outTitle = nullptr) const;

	/**
	 * Delete a title.
	 * @param inTitle Name of title to delete.
	 * @return Whether title was deleted.
	 */
	bool DeleteTitle(wxString const& inTitle);

	/**
	 * Find the named venue.
	 * @param inVenue Venue to find.
	 * @param outVenue Object that was found.
	 * @return Whether the object was found.
	 */
	bool FindVenue(wxString const& inVenue, ARBConfigVenuePtr* outVenue = nullptr) const;

	/**
	 * Add a venue.
	 * @param inVenue Name of venue to add.
	 * @param outVenue Pointer to new object, NULL if name already exists or is empty.
	 * @return Whether the object was added.
	 */
	bool AddVenue(wxString const& inVenue, ARBConfigVenuePtr* outVenue = nullptr);

	/**
	 * Add a venue.
	 * @param inVenue Venue to add.
	 * @return Whether the object was added.
	 */
	bool AddVenue(ARBConfigVenuePtr const& inVenue);

	/**
	 * Delete a venue.
	 * @param inVenue Name of venue to delete.
	 * @return Number of venues deleted (0 or 1).
	 */
	int DeleteVenue(wxString const& inVenue);

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
		wxString const& inVenue,
		wxString const& inEvent,
		wxString const& inDivision,
		wxString const& inLevel,
		ARBCommon::ARBDate const& inDate,
		ARBConfigEventPtr* outEvent = nullptr,
		ARBConfigScoringPtr* outScoring = nullptr) const;
};

} // namespace ARB
} // namespace dconSoft
