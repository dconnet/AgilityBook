#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBConfigEvent and ARBConfigEventList classes.
 * @author David Connet
 *
 * Revision History
 * 2016-06-17 Add support for Lifetime names.
 * 2013-09-03 Added short name.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-02 Added subnames to events.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-03-26 Added 'hasTable'. Used to set default when creating a run.
 * 2004-02-02 Added VerifyEvent.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-12-27 Changed FindEvent to take a date.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBConfigDivision.h"
#include "ARBConfigScoring.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"

#include <set>
class ARBDate;


/**
 * Event configuration information.
 */
class ARB_API ARBConfigEvent : public ARBBase
{
protected:
	ARBConfigEvent();
	ARBConfigEvent(ARBConfigEvent const& rhs);
	ARBConfigEvent(ARBConfigEvent&& rhs);

public:
	~ARBConfigEvent();
	static ARBConfigEventPtr New();
	ARBConfigEventPtr Clone() const;

	ARBConfigEvent& operator=(ARBConfigEvent const& rhs);
	ARBConfigEvent& operator=(ARBConfigEvent&& rhs);

	bool operator==(ARBConfigEvent const& rhs) const;
	bool operator!=(ARBConfigEvent const& rhs) const
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
	size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const override
	{
		return 0;
	}

	/**
	 * Load a event configuration.
	 * @pre inTree is the actual ARBConfigEvent element.
	 * @param inDivisions Used to verify division/level during load.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfigDivisionList const& inDivisions,
			ElementNodePtr const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigEvent element will be created in ioTree.
	 */
	bool Save(ElementNodePtr const& ioTree) const;

	/**
	 * Update this configuration from inEventNew.
	 * @param indent Indentation level for generating messages.
	 * @param inEventNew Event to merge.
	 * @param ioInfo Accumulated messages about changes that have happened.
	 * @return Whether or not changes have occurred.
	 */
	bool Update(
			int indent,
			ARBConfigEventPtr const& inEventNew,
			std::wstring& ioInfo);

	/**
	 * Find all the scoring methods that match.
	 * @param inDivision Division event is in.
	 * @param inLevel Level (NOT sublevel) event is in.
	 * @param inDate Date for matching methods.
	 * @param inTitlePoints Only return scoring methods that have title points.
	 * @param outList List of scoring methods found.
	 * @return Number of items found.
	 */
	size_t FindAllEvents(
			std::wstring const& inDivision,
			std::wstring const& inLevel,
			ARBDate const& inDate,
			bool inTitlePoints,
			ARBVector<ARBConfigScoringPtr>& outList) const
	{
		return m_Scoring.FindAllEvents(inDivision, inLevel, inDate, inTitlePoints, outList);
	}

	/**
	 * Verify a scoring method exists.
	 * @param inDivision Division event is in.
	 * @param inLevel Level event is in.
	 * @param inDate Date for matching methods.
	 * @return true if FindAllEvents() > 0.
	 */
	bool VerifyEvent(
			std::wstring const& inDivision,
			std::wstring const& inLevel,
			ARBDate const& inDate) const
	{
		return m_Scoring.VerifyEvent(inDivision, inLevel, inDate);
	}

	/**
	 * Find an event.
	 * @param inDivision Division event exists in.
	 * @param inLevel Level (NOT sublevel) event exists in.
	 * @param inDate Date for requested scoring.
	 * @param outScoring Pointer to object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindEvent(
			std::wstring const& inDivision,
			std::wstring const& inLevel,
			ARBDate const& inDate,
			ARBConfigScoringPtr* outScoring = nullptr) const
	{
		return m_Scoring.FindEvent(inDivision, inLevel, inDate, outScoring);
	}

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
	std::wstring const& GetShortName() const
	{
		return m_ShortName;
	}
	void SetShortName(std::wstring const& inName)
	{
		m_ShortName = inName;
	}
	std::wstring const& GetDesc() const
	{
		return m_Desc;
	}
	void SetDesc(std::wstring const& inDesc)
	{
		m_Desc = inDesc;
	}
	bool HasPartner() const
	{
		return m_bHasPartner;
	}
	void SetHasPartner(bool inHas)
	{
		m_bHasPartner = inHas;
	}
	ARBConfigScoringList const& GetScorings() const
	{
		return m_Scoring;
	}
	ARBConfigScoringList& GetScorings()
	{
		return m_Scoring;
	}

private:
	std::wstring m_Name;
	std::wstring m_ShortName;
	std::wstring m_Desc;
	bool m_bHasPartner;
	ARBConfigScoringList m_Scoring;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigEvent objects.
 */
class ARB_API ARBConfigEventList : public ARBVector<ARBConfigEventPtr>
{
public:
	/**
	 * Load an event configuration.
	 * @pre inTree is the actual ARBConfigEvent element.
	 * @param inDivisions Configurate division used for verification.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfigDivisionList const& inDivisions,
			ElementNodePtr const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Reorder the list based on 'inList'
	 * @param inList List we want to match order on.
	 */
	void ReorderBy(ARBConfigEventList const& inList);

	/**
	 * Verify an event exists.
	 * @param inEvent Event to verify.
	 * @param inDivision Division event exists in.
	 * @param inLevel Level event exists in.
	 * @param inDate Date for matching methods.
	 * @return Event exists.
	 */
	bool VerifyEvent(
			std::wstring const& inEvent,
			std::wstring const& inDivision,
			std::wstring const& inLevel,
			ARBDate const& inDate) const;

	/**
	 * Find an event.
	 * @param inEvent Event to find.
	 * @param inDivision Division event exists in.
	 * @param inLevel Level (NOT sublevel) event exists in.
	 * @param inDate Date for requested scoring.
	 * @param outEvent Pointer to object, NULL if not found.
	 * @param outScoring Pointer to object, NULL if not found.
	 * @return Whether the event was found.
	 */
	 bool FindEvent(
			std::wstring const& inEvent,
			std::wstring const& inDivision,
			std::wstring const& inLevel,
			ARBDate const& inDate,
			ARBConfigEventPtr* outEvent = nullptr,
			ARBConfigScoringPtr* outScoring = nullptr) const;

	/**
	 * Rename a division.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameDivision(
			std::wstring const& inOldDiv,
			std::wstring const& inNewDiv);

	/**
	 * Delete a division.
	 * @param inDiv Division name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteDivision(std::wstring const& inDiv);

	/**
	 * Rename a level.
	 * @param inOldDiv Division of old level.
	 * @param inOldLevel Level name being renamed.
	 * @param inNewLevel New level name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameLevel(
			std::wstring const& inOldDiv,
			std::wstring const& inOldLevel,
			std::wstring const& inNewLevel);

	/**
	 * Delete a level.
	 * @param inDiv Division of level being deleted.
	 * @param inLevel Level name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteLevel(
			std::wstring const& inDiv,
			std::wstring const& inLevel);

	/**
	 * Rename a lifetime name.
	 * @param inOldName Lifetime name being renamed.
	 * @param inNewName New Lifetime name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameLifetimeName(
			std::wstring const& inOldName,
			std::wstring const& inNewName);

	/**
	 * Delete a lifetime name.
	 * @param inName Name of lifetime object being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteLifetimeName(std::wstring const& inName);

	/**
	 * Find an event.
	 * @param inEvent Name of event to find.
	 * @param outEvent Pointer to object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindEvent(
			std::wstring const& inEvent,
			ARBConfigEventPtr* outEvent = nullptr) const;

	/**
	 * Add an event.
	 * @param inEvent Event to add.
	 * @return Whether the object was added.
	 */
	bool AddEvent(ARBConfigEventPtr const& inEvent);

	/**
	 * Delete an event.
	 * @param inEvent Name of event to delete.
	 * @return Whether event was deleted or not.
	 */
	bool DeleteEvent(std::wstring const& inEvent);
};
