#pragma once

/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @brief ARBConfigEvent and ARBConfigEventList classes.
 * @author David Connet
 *
 * Revision History
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

#include <set>
#include "ARBBase.h"
#include "ARBConfigDivision.h"
#include "ARBConfigScoring.h"
#include "ARBTypes.h"
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
	virtual tstring GetGenericName() const
	{
		return m_Name;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const
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
			tstring& ioInfo);

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
			tstring const& inDivision,
			tstring const& inLevel,
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
			tstring const& inDivision,
			tstring const& inLevel,
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
			tstring const& inDivision,
			tstring const& inLevel,
			ARBDate const& inDate,
			ARBConfigScoringPtr* outScoring = NULL) const
	{
		return m_Scoring.FindEvent(inDivision, inLevel, inDate, outScoring);
	}

	/*
	 * Getters/setters.
	 */
	tstring const& GetName() const
	{
		return m_Name;
	}
	void SetName(tstring const& inName)
	{
		m_Name = inName;
	}
	tstring const& GetDesc() const
	{
		return m_Desc;
	}
	void SetDesc(tstring const& inDesc)
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
	size_t GetSubNames(std::set<tstring>& outNames) const;
	void SetSubNames(std::set<tstring> const& inNames);
	ARBConfigScoringList const& GetScorings() const
	{
		return m_Scoring;
	}
	ARBConfigScoringList& GetScorings()
	{
		return m_Scoring;
	}

private:
	tstring m_Name;
	tstring m_Desc;
	bool m_bTable;
	bool m_bHasPartner;
	bool m_bHasSubNames;
	std::set<tstring> m_SubNames;
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
			tstring const& inEvent,
			tstring const& inDivision,
			tstring const& inLevel,
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
			tstring const& inEvent,
			tstring const& inDivision,
			tstring const& inLevel,
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
			tstring const& inOldDiv,
			tstring const& inNewDiv);

	/**
	 * Delete a division.
	 * @param inDiv Division name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteDivision(tstring const& inDiv);

	/**
	 * Rename a level.
	 * @param inOldDiv Division of old level.
	 * @param inOldLevel Level name being renamed.
	 * @param inNewLevel New level name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameLevel(
			tstring const& inOldDiv,
			tstring const& inOldLevel,
			tstring const& inNewLevel);

	/**
	 * Delete a level.
	 * @param inDiv Division of level being deleted.
	 * @param inLevel Level name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteLevel(
			tstring const& inDiv,
			tstring const& inLevel);

	/**
	 * Find an event.
	 * @param inEvent Name of event to find.
	 * @param outEvent Pointer to object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindEvent(
			tstring const& inEvent,
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
	bool DeleteEvent(tstring const& inEvent);
};
