#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBDogClub and ARBDogClubList classes.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-12-27 Changed FindEvent to take a date.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"
class ARBDate;


/**
 * Dog club.
 */
class ARB_API ARBDogClub : public ARBBase
{
protected:
	ARBDogClub();
	ARBDogClub(ARBDogClub const& rhs);
	ARBDogClub(ARBDogClub&& rhs);

public:
	~ARBDogClub();
	static ARBDogClubPtr New();
	ARBDogClubPtr Clone() const;

	ARBDogClub& operator=(ARBDogClub const& rhs);
	ARBDogClub& operator=(ARBDogClub&& rhs);

	bool operator==(ARBDogClub const& rhs) const;
	bool operator!=(ARBDogClub const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	std::wstring GetGenericName() const override
	{
		return m_Name;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const override;

	/**
	 * Load a club.
	 * @pre inTree is the actual ARBDogClub element.
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
	 * Perform any fix ups - specifically, co-sanctioning.
	 * @param clubList The parent list container.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool PostLoad(ARBDogClubList const& clubList, ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogClub element will be created in ioTree.
	 */
	bool Save(ElementNodePtr const& ioTree) const;

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
	std::wstring const& GetVenue() const
	{
		return m_Venue;
	}
	void SetVenue(std::wstring const& inVenue)
	{
		m_Venue = inVenue;
	}
	// For Co-sanctioning
	ARBDogClubPtr GetPrimaryClub() const
	{
		return m_PrimaryClub.lock();
	}
	void SetPrimaryClub(ARBDogClubPtr const& inCoSanction)
	{
		m_PrimaryClub = inCoSanction;
	}

private:
	std::wstring m_Name;
	std::wstring m_Venue;
	std::wstring m_PrimaryClubVenue; // Only used during loading.
	ARBDogClubWPtr m_PrimaryClub;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogClub objects, used in a trial object.
 * @note The order within the club list is important. It is used to denote
 *       the primary club in a trial. The primary club is the club that is
 *       used to establish the rules used in entering a run.
 */
class ARB_API ARBDogClubList : public ARBVector<ARBDogClubPtr>
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
	 * Perform any fix ups - specifically, co-sanctioning.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool PostLoad(ARBErrorCallback& ioCallback);

	/**
	 * Get the main club for the trial (used for icon and trial generic name)
	 * @return Main club for trial
	 */
	ARBDogClubPtr GetMainClub() const;

	/**
	 * Get the primary club for a cosanctioning pair, used to establish rules.
	 * @param inVenue Venue of club to find.
	 * @param outClub Pointer to first club.
	 * @return Whether there is a primary club.
	 */
	bool GetPrimaryClub(std::wstring const& inVenue, ARBDogClubPtr* outClub = nullptr) const;

	/**
	 * Get the list of all clubs.
	 * @param bNames Get club names or venues?
	 * @param bIncCosanction Should co-sanctioning clubs be included?
	 * @return All clubs.
	 */
	std::wstring GetClubList(bool bNames, bool bIncCosanction = true) const;

	/**
	 * Get the list of clubs for a run.
	 * @param bNames Get club names or venues?
	 * @param inRun Run to get clubs for.
	 * @return Primary and possible cosanctioning club.
	 */
	std::wstring GetClubList(bool bNames, ARBDogRunPtr inRun) const;

	/**
	 * Find the first cosanctioning club for a primary club.
	 * If the club is a cosanctioning club and not a primary,
	 * no results are found.
	 * @param inClub The primary club.
	 * @return The cosanctioning club.
	 *         That club's GetPrimaryClub should (better!) return inClub.
	 */
	ARBDogClubPtr FindCoSanctioningClub(ARBDogClubPtr inClub) const;

	/**
	 * Find the index of the club.
	 * Should only be called by ARBDogRun to save data.
	 * @param inClub Club to find.
	 * @param outIndex Direct index of club.
	 * @return If club was found.
	 */
	bool FindClubIndex(ARBDogClubPtr const& inClub, size_t& outIndex) const;

	/**
	 * Find a club
	 * @param inName Name of club to find.
	 * @param inVenue Venue of club.
	 * @param outClub Found club.
	 * @return Whether the club was found.
	 */
	bool FindClub(std::wstring const& inName, std::wstring const& inVenue, ARBDogClubPtr* outClub = nullptr) const;

	/**
	 * Find the first scoring style to match.
	 * Club order is critical as we'll search the venues by club order.
	 * @param inConfig Configuration for looking up information.
	 * @param inEvent Event to search for.
	 * @param inDivision Division to search for.
	 * @param inLevel Level to search for.
	 * @param inDate Date of event.
	 * @param ioCallback Error processing callback.
	 * @param outEvent Pointer to object, NULL if not found.
	 * @param outScoring Pointer to object, NULL if not found.
	 * @return Whether an event was found.
	 */
	bool FindEvent(
		ARBConfig const& inConfig,
		std::wstring const& inEvent,
		std::wstring const& inDivision,
		std::wstring const& inLevel,
		ARBDate const& inDate,
		ARBErrorCallback& ioCallback,
		ARBConfigEventPtr* outEvent = nullptr,
		ARBConfigScoringPtr* outScoring = nullptr) const;

	/**
	 * Find a club that uses the specified venue.
	 * @param inVenue Venue to search for.
	 * @param outClub Found club.
	 * @return Whether the club was found.
	 */
	bool FindVenue(std::wstring const& inVenue, ARBDogClubPtr* outClub = nullptr) const;

	/**
	 * Add a club.
	 * @param inName Name of club to add.
	 * @param inVenue Venue of club.
	 * @param outClub Added club.
	 * @return Whether the club was added.
	 */
	bool AddClub(std::wstring const& inName, std::wstring const& inVenue, ARBDogClubPtr* outClub = nullptr);

	/**
	 * Delete a club.
	 * @param inName Name of club to delete.
	 * @param inVenue Venue of club.
	 * @return Whether club was deleted.
	 */
	bool DeleteClub(std::wstring const& inName, std::wstring const& inVenue);
};
