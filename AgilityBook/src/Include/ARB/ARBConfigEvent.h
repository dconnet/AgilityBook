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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-02 DRC Added subnames to events.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-03-26 DRC Added 'hasTable'. Used to set default when creating a run.
 * @li 2004-02-02 DRC Added VerifyEvent.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARB/ARBBase.h"
#include "ARB/ARBConfigDivision.h"
#include "ARB/ARBConfigScoring.h"
#include "ARB/ARBTypes2.h"
#include <set>
class ARBDate;


/**
 * Event configuration information.
 */
class ARBConfigEvent : public ARBBase
{
protected:
	ARBConfigEvent();
	ARBConfigEvent(ARBConfigEvent const& rhs);

public:
	~ARBConfigEvent();
	static ARBConfigEventPtr New();
	ARBConfigEventPtr Clone() const;

	ARBConfigEvent& operator=(ARBConfigEvent const& rhs);

	bool operator==(ARBConfigEvent const& rhs) const;
	bool operator!=(ARBConfigEvent const& rhs) const
	{
		return !operator==(rhs);
	}

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
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigEvent element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

	/**
	 * Update this configuration from inEventNew.
	 * @param indent Indentation level for generating messages.
	 * @param inEventNew Event to merge.
	 * @param ioInfo Accumulated messages about changes that have happened.
	 * @return Whether or not changes have occurred.
	 */
	bool Update(
			int indent,
			ARBConfigEventPtr inEventNew,
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
			ARBConfigScoringPtr* outScoring = NULL) const
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
	std::wstring const& GetDesc() const
	{
		return m_Desc;
	}
	void SetDesc(std::wstring const& inDesc)
	{
		m_Desc = inDesc;
	}
	bool HasTable() const
	{
		return m_bTable;
	}
	void SetHasTable(bool inBool)
	{
		m_bTable = inBool;
	}
	bool HasPartner() const
	{
		return m_bHasPartner;
	}
	void SetHasPartner(bool inHas)
	{
		m_bHasPartner = inHas;
	}
	bool HasSubNames() const
	{
		return m_bHasSubNames;
	}
	void SetHasSubNames(bool inHas)
	{
		m_bHasSubNames = inHas;
	}
	size_t GetSubNames(std::set<std::wstring>& outNames) const;
	void SetSubNames(std::set<std::wstring> const& inNames);
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
	std::wstring m_Desc;
	bool m_bTable;
	bool m_bHasPartner;
	bool m_bHasSubNames;
	std::set<std::wstring> m_SubNames;
	ARBConfigScoringList m_Scoring;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigEvent objects.
 */
class ARBConfigEventList : public ARBVector<ARBConfigEventPtr>
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
			ElementNodePtr inTree,
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
	 * @param inLevel Level event exists in.
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
			ARBConfigEventPtr* outEvent = NULL,
			ARBConfigScoringPtr* outScoring = NULL) const;

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
	 * Find an event.
	 * @param inEvent Name of event to find.
	 * @param outEvent Pointer to object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindEvent(
			std::wstring const& inEvent,
			ARBConfigEventPtr* outEvent = NULL) const;

	/**
	 * Add an event.
	 * @param inEvent Event to add.
	 * @return Whether the object was added.
	 */
	bool AddEvent(ARBConfigEventPtr inEvent);

	/**
	 * Delete an event.
	 * @param inEvent Name of event to delete.
	 * @return Whether event was deleted or not.
	 */
	bool DeleteEvent(std::wstring const& inEvent);
};
