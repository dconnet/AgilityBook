#pragma once

/*
 * Copyright © 2005-2006 David Connet. All Rights Reserved.
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
 * @brief ARBConfigMultiQItem, ARBConfigMultiQ and ARBConfigMultiQList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2005-07-15 DRC Created.
 */

#include <set>
#include "ARBBase.h"
#include "ARBConfigDivision.h"
#include "ARBDate.h"
#include "ARBTypes.h"
#include "ARBVector.h"
class ARBDogRun;
class ARBErrorCallback;
class ARBVersion;
class Element;

/**
 * MultiQ configuration information.
 */
class ARBConfigMultiQ : public ARBBase
{
public:
	ARBConfigMultiQ();
	~ARBConfigMultiQ();
	ARBConfigMultiQ(ARBConfigMultiQ const& rhs);
	ARBConfigMultiQ& operator=(ARBConfigMultiQ const& rhs);
	ARBConfigMultiQPtr Clone() const;

	bool operator==(ARBConfigMultiQ const& rhs) const;
	bool operator!=(ARBConfigMultiQ const& rhs) const;

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual ARBString GetGenericName() const;

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<ARBString>& ioStrings) const;

	/**
	 * Load a multiQ configuration.
	 * @pre inTree is the actual ARBConfigMultiQ element.
	 * @param inVenue Used to verify division/level/event during load.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfigVenue const& inVenue,
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigMultiQ element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Does this multi-q configuration match the given set of runs?
	 * @param ioRuns Runs to check.
	 * @param outRuns Runs that were removed from ioRuns.
	 * @return There is a match. Matching runs are removed from the vector.
	 */
	bool Match(
			std::vector<ARBDogRunPtr>& ioRuns,
			std::vector<ARBDogRunPtr>& outRuns) const;

	/**
	 * Rename a division.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameDivision(
			ARBString const& inOldDiv,
			ARBString const& inNewDiv);

	/**
	 * Delete a division.
	 * @param inDiv Division name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteDivision(ARBString const& inDiv);

	/**
	 * Rename a level.
	 * @param inDiv Division level is in.
	 * @param inOldLevel Level name being renamed.
	 * @param inNewLevel New level name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameLevel(
			ARBString const& inDiv,
			ARBString const& inOldLevel,
			ARBString const& inNewLevel);

	/**
	 * Delete a level.
	 * @param inLevel Level name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteLevel(ARBString const& inLevel);

	/**
	 * Rename an event.
	 * @param inOldEvent Event name being renamed.
	 * @param inNewEvent New event name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameEvent(
			ARBString const& inOldEvent,
			ARBString const& inNewEvent);

	/**
	 * Delete an event.
	 * @param inEvent Event name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteEvent(ARBString const& inEvent);

	/**
	 * Add an item.
	 * @param inDiv Division of event.
	 * @param inLevel Level of event.
	 * @param inEvent Event.
	 * @return Whether object was added.
	 */
	bool AddItem(
			ARBString const& inDiv,
			ARBString const& inLevel,
			ARBString const& inEvent);

	/**
	 * Remove an item.
	 * @param inDiv Division of event.
	 * @param inLevel Level of event.
	 * @param inEvent Event.
	 * @return Whether object was removed.
	 */
	bool RemoveItem(
			ARBString const& inDiv,
			ARBString const& inLevel,
			ARBString const& inEvent);

	/**
	 * Remove all configuration items.
	 */
	bool RemoveAllItems();

	/*
	 * Getters/setters.
	 */
	ARBString GetName() const;
	void SetName(ARBString const& inName);
	ARBString GetShortName() const;
	void SetShortName(ARBString const& inName);
	ARBDate GetValidFrom() const;
	void SetValidFrom(ARBDate const& inDate);
	ARBDate GetValidTo() const;
	void SetValidTo(ARBDate const& inDate);
	size_t GetNumItems() const;
	bool GetItem(
			size_t inIndex,
			ARBString& outDivision,
			ARBString& outLevel,
			ARBString& outEvent) const;

private:
	struct MultiQItem
	{
		ARBString m_Div;
		ARBString m_Level;
		ARBString m_Event;
		bool operator<(MultiQItem const& rhs) const
		{
			if (m_Div < rhs.m_Div)
				return true;
			if (m_Level < rhs.m_Level)
				return true;
			if (m_Event < rhs.m_Event)
				return true;
			return false;
		}
		bool operator==(MultiQItem const& rhs) const
		{
			return m_Div == rhs.m_Div && m_Level == rhs.m_Level && m_Event == rhs.m_Event;
		}
	};

	ARBString m_Name;
	ARBString m_ShortName;
	ARBDate m_ValidFrom;
	ARBDate m_ValidTo;
	std::set<MultiQItem> m_Items;
};

inline ARBString ARBConfigMultiQ::GetName() const
{
	return m_Name;
}

inline void ARBConfigMultiQ::SetName(ARBString const& inName)
{
	m_Name = inName;
}

inline ARBString ARBConfigMultiQ::GetShortName() const
{
	return m_ShortName;
}

inline void ARBConfigMultiQ::SetShortName(ARBString const& inName)
{
	m_ShortName = inName;
}

inline ARBDate ARBConfigMultiQ::GetValidFrom() const
{
	return m_ValidFrom;
}

inline void ARBConfigMultiQ::SetValidFrom(ARBDate const& inDate)
{
	m_ValidFrom = inDate;
}

inline ARBDate ARBConfigMultiQ::GetValidTo() const
{
	return m_ValidTo;
}

inline void ARBConfigMultiQ::SetValidTo(ARBDate const& inDate)
{
	m_ValidTo = inDate;
}

inline size_t ARBConfigMultiQ::GetNumItems() const
{
	return m_Items.size();
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigMultiQ objects.
 */
class ARBConfigMultiQList : public ARBVector<ARBConfigMultiQPtr>
{
public:
	/**
	 * Load a multiQ configuration.
	 * @pre inTree is the actual ARBConfigMultiQ element.
	 * @param inVenue Used to verify division/level/event during load.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfigVenue const& inVenue,
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Find a MultiQ by name.
	 * @param inName Multi-Q to find
	 * @param inUseShortName Search by shortname instead of longname.
	 * @param outMultiQ Pointer to object, NULL if not found.
	 */
	bool FindMultiQ(
			ARBString const& inName,
			bool inUseShortName = false,
			ARBConfigMultiQPtr* outMultiQ = NULL) const;

	/**
	 * Find a MultiQ by value.
	 * @param inMultiQ Multi-Q definition to find
	 * @param outMultiQ Pointer to object, NULL if not found.
	 */
	bool FindMultiQ(
			ARBConfigMultiQ const& inMultiQ,
			ARBConfigMultiQPtr* outMultiQ = NULL) const;

	/**
	 * Rename a division.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameDivision(
			ARBString const& inOldDiv,
			ARBString const& inNewDiv);

	/**
	 * Delete a division.
	 * @param inDiv Division name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteDivision(ARBString const& inDiv);

	/**
	 * Rename a level.
	 * @param inDiv Division level is in.
	 * @param inOldLevel Level name being renamed.
	 * @param inNewLevel New level name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameLevel(
			ARBString const& inDiv,
			ARBString const& inOldLevel,
			ARBString const& inNewLevel);

	/**
	 * Delete a level.
	 * @param inLevel Level name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteLevel(ARBString const& inLevel);

	/**
	 * Rename an event.
	 * @param inOldEvent Event name being renamed.
	 * @param inNewEvent New event name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameEvent(
			ARBString const& inOldEvent,
			ARBString const& inNewEvent);

	/**
	 * Delete an event.
	 * @param inEvent Event name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteEvent(ARBString const& inEvent);

	/**
	 * Add a MultiQ.
	 * @param inMultiQ MultiQ to add.
	 * @return Whether the object was added.
	 */
	bool AddMultiQ(ARBConfigMultiQPtr inMultiQ);

	/**
	 * Delete an MultiQ.
	 * @param inMultiQ Name of MultiQ to delete.
	 * @return Whether object was deleted or not.
	 */
	bool DeleteMultiQ(ARBConfigMultiQPtr inMultiQ);
};
