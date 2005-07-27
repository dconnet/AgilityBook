#pragma once

/*
 * Copyright © 2005 David Connet. All Rights Reserved.
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
#include <string>
#include "ARBBase.h"
#include "ARBConfigDivision.h"
#include "ARBDate.h"
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
	ARBConfigMultiQ(ARBConfigMultiQ const& rhs);
	ARBConfigMultiQ& operator=(ARBConfigMultiQ const& rhs);
	bool operator==(ARBConfigMultiQ const& rhs) const;
	bool operator!=(ARBConfigMultiQ const& rhs) const;

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
	 * @param inRuns Runs to check.
	 * @return There is a match.
	 */
	bool Match(ARBVectorBase<ARBDogRun> const& inRuns) const;

	/**
	 * Rename a division.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameDivision(
			std::string const& inOldDiv,
			std::string const& inNewDiv);

	/**
	 * Delete a division.
	 * @param inDiv Division name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteDivision(std::string const& inDiv);

	/**
	 * Rename a level.
	 * @param inOldLevel Level name being renamed.
	 * @param inNewLevel New level name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameLevel(
			std::string const& inOldLevel,
			std::string const& inNewLevel);

	/**
	 * Delete a level.
	 * @param inLevel Level name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteLevel(std::string const& inLevel);

	/**
	 * Rename an event.
	 * @param inOldEvent Event name being renamed.
	 * @param inNewEvent New event name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameEvent(
			std::string const& inOldEvent,
			std::string const& inNewEvent);

	/**
	 * Delete an event.
	 * @param inEvent Event name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteEvent(std::string const& inEvent);

	/**
	 * Add an event.
	 * @param inDiv Division of event.
	 * @param inLevel Level of event.
	 * @param inEvent Event.
	 * @return Whether object was added.
	 */
	bool AddEvent(
			std::string const& inDiv,
			std::string const& inLevel,
			std::string const& inEvent);

	/*
	 * Getters/setters.
	 */
	std::string GetName() const;
	void SetName(std::string const& inName);
	std::string GetShortName() const;
	void SetShortName(std::string const& inName);
	ARBDate GetValidFrom() const;
	void SetValidFrom(ARBDate const& inDate);
	ARBDate GetValidTo() const;
	void SetValidTo(ARBDate const& inDate);
	size_t GetNumEvents() const;

private:
	~ARBConfigMultiQ();

	struct MultiQItem
	{
		std::string m_Div;
		std::string m_Level;
		std::string m_Event;
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

	std::string m_Name;
	std::string m_ShortName;
	ARBDate m_ValidFrom;
	ARBDate m_ValidTo;
	std::set<MultiQItem> m_Items;
};

inline std::string ARBConfigMultiQ::GetName() const
{
	return m_Name;
}

inline void ARBConfigMultiQ::SetName(std::string const& inName)
{
	m_Name = inName;
}

inline std::string ARBConfigMultiQ::GetShortName() const
{
	return m_ShortName;
}

inline void ARBConfigMultiQ::SetShortName(std::string const& inName)
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

inline size_t ARBConfigMultiQ::GetNumEvents() const
{
	return m_Items.size();
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigMultiQ objects.
 */
class ARBConfigMultiQList : public ARBVector<ARBConfigMultiQ>
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
	 * Find a MultiQ.
	 * @param inMultiQ Multi-Q definition to find
	 * @param outMultiQ Pointer to object, NULL if not found.
	 */
	bool FindMultiQ(
		ARBConfigMultiQ const& inMultiQ,
		ARBConfigMultiQ** outMultiQ = NULL);

	/**
	 * Rename a division.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameDivision(
			std::string const& inOldDiv,
			std::string const& inNewDiv);

	/**
	 * Delete a division.
	 * @param inDiv Division name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteDivision(std::string const& inDiv);

	/**
	 * Rename a level.
	 * @param inOldLevel Level name being renamed.
	 * @param inNewLevel New level name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameLevel(
			std::string const& inOldLevel,
			std::string const& inNewLevel);

	/**
	 * Delete a level.
	 * @param inLevel Level name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteLevel(std::string const& inLevel);

	/**
	 * Rename an event.
	 * @param inOldEvent Event name being renamed.
	 * @param inNewEvent New event name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameEvent(
			std::string const& inOldEvent,
			std::string const& inNewEvent);

	/**
	 * Delete an event.
	 * @param inEvent Event name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteEvent(std::string const& inEvent);

	/**
	 * Add a MultiQ.
	 * @param inMultiQ MultiQ to add.
	 * @return Whether the object was added.
	 */
	bool AddMultiQ(ARBConfigMultiQ* inMultiQ);

	/**
	 * Delete an MultiQ.
	 * @param inMultiQ Name of MultiQ to delete.
	 * @return Whether object was deleted or not.
	 */
	bool DeleteMultiQ(ARBConfigMultiQ* inMultiQ);
};
