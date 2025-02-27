#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBDogTitle and ARBDogTitleList classes.
 * @author David Connet
 *
 * Revision History
 * 2013-01-13 Added new recurring title suffix style.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-14 Moved 'Titles' to 'Venue'.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-10 Allow titles to be optionally entered multiple times.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-03-13 Added ability to hide titles, including unearned ones.
 *            An unearned title has an invalid date.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBConfigTitle.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"

#include "ARBCommon/ARBDate.h"


namespace dconSoft
{
namespace ARB
{

/**
 * Titles a dog has earned.
 */
class ARB_API ARBDogTitle
	: public ARBBase
	, protected ARBTitleInstance
{
protected:
	ARBDogTitle();
	ARBDogTitle(ARBDogTitle const& rhs);
	ARBDogTitle(ARBDogTitle&& rhs);

public:
	~ARBDogTitle();
	static ARBDogTitlePtr New();
	ARBDogTitlePtr Clone() const;

	ARBDogTitle& operator=(ARBDogTitle const& rhs);
	ARBDogTitle& operator=(ARBDogTitle&& rhs);

	bool operator==(ARBDogTitle const& rhs) const;
	bool operator!=(ARBDogTitle const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	wxString GetGenericName() const override;

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<wxString>& ioStrings) const override;

	/**
	 * Load a title.
	 * @pre inTree is the actual ARBDogTitle element.
	 * @param inConfig Configuration for looking up information.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
		ARBConfig const& inConfig,
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogTitle element will be created in ioTree.
	 */
	bool Save(ARBCommon::ElementNodePtr const& ioTree) const;

	/*
	 * Getters/setters.
	 */
	ARBCommon::ARBDate const& GetDate() const
	{
		return m_Date;
	}
	/**
	 * @post Setting the date also affects the hidden value. An invalid date
	 *       will hide the entry, a valid date will unhide.
	 */
	void SetDate(ARBCommon::ARBDate const& inDate)
	{
		m_Date = inDate;
		if (!m_Date.IsValid())
			m_bHidden = true;
	}
	wxString const& GetVenue() const
	{
		return m_Venue;
	}
	void SetVenue(wxString const& inVenue)
	{
		m_Venue = inVenue;
	}
	wxString const& GetRawName() const
	{
		return m_Name;
	}
	short GetInstance() const
	{
		return m_Instance;
	}
	short GetInstanceIncrement() const
	{
		return m_MultipleIncrement;
	}
	bool ShowInstanceOne() const
	{
		return 1 == m_Instance && m_bShowInstanceOne;
	}
	ARBTitleStyle GetStyle() const
	{
		return m_MultipleStyle;
	}
	ARBTitleSeparator GetStyleSeparator() const
	{
		return m_MultipleSeparator;
	}
	void SetName(wxString const& inName, short inInstance, ARBConfigTitlePtr const& inConfigTitle);
	void Rename(wxString const& inName)
	{
		m_Name = inName;
	}
	bool GetReceived() const
	{
		return m_bReceived;
	}
	void SetReceived(bool inReceived)
	{
		m_bReceived = inReceived;
	}
	bool IsHidden() const
	{
		return m_bHidden;
	}
	void SetHidden(bool bHidden)
	{
		if (m_Date.IsValid())
			m_bHidden = bHidden;
		else
			m_bHidden = true;
	}

private:
	ARBCommon::ARBDate m_Date;
	wxString m_Venue;
	wxString m_Name;
	short m_Instance;
	unsigned short m_MultipleStartAt;
	short m_MultipleIncrement;
	bool m_bShowInstanceOne;
	ARBTitleStyle m_MultipleStyle;
	ARBTitleSeparator m_MultipleSeparator;
	bool m_bReceived;
	bool m_bHidden;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogTitle objects.
 */
class ARB_API ARBDogTitleList : public ARBVector<ARBDogTitlePtr>
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
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Sort the list by date.
	 */
	void sort();

	/**
	 * Get the number of titles in a venue.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @return Number of objects.
	 */
	int NumTitlesInVenue(wxString const& inVenue) const;

	/**
	 * Find a title
	 * @param inVenue Venue name.
	 * @param inTitle Name of title.
	 * @param outTitle Pointer to found title.
	 */
	bool FindTitle(wxString const& inVenue, wxString const& inTitle, ARBDogTitlePtr* outTitle = nullptr) const;

	/**
	 * Find the maximum instance of a title
	 * @param inVenue Venue name.
	 * @param inTitle Name of title.
	 * @param outMissing Return a list of missing instances.
	 */
	short FindMaxInstance(wxString const& inVenue, wxString const& inTitle, std::vector<short>* outMissing = nullptr)
		const;

	/**
	 * Rename a venue, rename any dependent objects.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items changed.
	 */
	int RenameVenue(wxString const& inOldVenue, wxString const& inNewVenue);

	/**
	 * Delete a venue, remove any dependent objects.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(wxString const& inVenue);

	/**
	 * Number of titles in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue title is in.
	 * @param inTitle Title to tally.
	 * @return Number of objects.
	 */
	int NumTitlesInUse(wxString const& inVenue, wxString const& inTitle) const;

	/**
	 * Rename a title, rename any dependent objects.
	 * @param inVenue Venue title is in.
	 * @param inOldTitle Title name being renamed.
	 * @param inNewTitle New title name.
	 * @return Number of items changed.
	 */
	int RenameTitle(wxString const& inVenue, wxString const& inOldTitle, wxString const& inNewTitle);

	/**
	 * Add a title.
	 * @param inTitle Title to add.
	 * @return Whether the object was added.
	 */
	bool AddTitle(ARBDogTitlePtr const& inTitle);

	/**
	 * Delete a title, remove any dependent objects.
	 * @param inVenue Venue that has title being deleted.
	 * @param inTitle Title being deleted.
	 * @return Was it removed?
	 */
	bool DeleteTitle(ARBConfigVenuePtr const& inVenue, ARBDogTitlePtr const& inTitle);
};

} // namespace ARB
} // namespace dconSoft
