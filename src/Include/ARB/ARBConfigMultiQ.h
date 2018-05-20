#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBConfigMultiQItem, ARBConfigMultiQ and ARBConfigMultiQList classes.
 * @author David Connet
 *
 * Revision History
 * 2012-11-21 Fix MultiQItem sorting (wasn't antisymetric).
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-07-15 Created.
 */

#include "ARB/ARBBase.h"
#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"
#include <set>


/**
 * MultiQ configuration information.
 */
class ARBConfigMultiQ : public ARBBase
{
protected:
	ARBConfigMultiQ();
	ARBConfigMultiQ(ARBConfigMultiQ const& rhs);

public:
	~ARBConfigMultiQ();
	static ARBConfigMultiQPtr New();
	ARBConfigMultiQPtr Clone() const;

	ARBConfigMultiQ& operator=(ARBConfigMultiQ const& rhs);

	bool operator==(ARBConfigMultiQ const& rhs) const;
	bool operator!=(ARBConfigMultiQ const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual std::wstring GetGenericName() const
	{
		return m_ShortName;
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
			ElementNodePtr const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigMultiQ element will be created in ioTree.
	 */
	bool Save(ElementNodePtr const& ioTree) const;

	/**
	 * Does this multi-q configuration match the given set of runs?
	 * @param inRuns Runs to check.
	 * @param outRuns Runs that were removed from inRuns.
	 * @return There is a match.
	 */
	bool Match(
			std::vector<ARBDogRunPtr> const& inRuns,
			std::vector<ARBDogRunPtr>& outRuns) const;

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
	 * @param inDiv Division level is in.
	 * @param inOldLevel Level name being renamed.
	 * @param inNewLevel New level name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameLevel(
			std::wstring const& inDiv,
			std::wstring const& inOldLevel,
			std::wstring const& inNewLevel);

	/**
	 * Delete a level.
	 * @param inLevel Level name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteLevel(std::wstring const& inLevel);

	/**
	 * Rename an event.
	 * @param inOldEvent Event name being renamed.
	 * @param inNewEvent New event name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameEvent(
			std::wstring const& inOldEvent,
			std::wstring const& inNewEvent);

	/**
	 * Delete an event.
	 * @param inEvent Event name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteEvent(std::wstring const& inEvent);

	/**
	 * Add an item.
	 * @param inDiv Division of event.
	 * @param inLevel Level of event.
	 * @param inEvent Event.
	 * @return Whether object was added.
	 */
	bool AddItem(
			std::wstring const& inDiv,
			std::wstring const& inLevel,
			std::wstring const& inEvent);

	/**
	 * Remove an item.
	 * @param inDiv Division of event.
	 * @param inLevel Level of event.
	 * @param inEvent Event.
	 * @return Whether object was removed.
	 */
	bool RemoveItem(
			std::wstring const& inDiv,
			std::wstring const& inLevel,
			std::wstring const& inEvent);

	/**
	 * Remove all configuration items.
	 */
	bool RemoveAllItems();

	/*
	 * Getters/setters.
	 */
	std::wstring GetName() const
	{
		return m_Name;
	}
	void SetName(std::wstring const& inName)
	{
		m_Name = inName;
	}
	std::wstring GetShortName() const
	{
		return m_ShortName;
	}
	void SetShortName(std::wstring const& inName)
	{
		m_ShortName = inName;
	}
	ARBDate GetValidFrom() const
	{
		return m_ValidFrom;
	}
	void SetValidFrom(ARBDate const& inDate)
	{
		m_ValidFrom = inDate;
	}
	ARBDate GetValidTo() const
	{
		return m_ValidTo;
	}
	void SetValidTo(ARBDate const& inDate)
	{
		m_ValidTo = inDate;
	}
	size_t GetNumItems() const
	{
		return m_Items.size();
	}
	bool GetItem(
			size_t inIndex,
			std::wstring& outDivision,
			std::wstring& outLevel,
			std::wstring& outEvent) const;

private:
	struct MultiQItem
	{
		std::wstring m_Div;
		std::wstring m_Level;
		std::wstring m_Event;
		bool operator<(MultiQItem const& rhs) const
		{
			if (m_Div != rhs.m_Div)
				return (m_Div < rhs.m_Div);
			if (m_Level != rhs.m_Level)
				return (m_Level < rhs.m_Level);
			return (m_Event < rhs.m_Event);
		}
		bool operator==(MultiQItem const& rhs) const
		{
			return m_Div == rhs.m_Div && m_Level == rhs.m_Level && m_Event == rhs.m_Event;
		}
	};

	std::wstring m_Name;
	std::wstring m_ShortName;
	ARBDate m_ValidFrom;
	ARBDate m_ValidTo;
	std::set<MultiQItem> m_Items;
};

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
			ElementNodePtr const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Find a MultiQ by name.
	 * @param inName Multi-Q to find
	 * @param inUseShortName Search by shortname instead of longname.
	 * @param outMultiQ Pointer to object, NULL if not found.
	 */
	bool FindMultiQ(
			std::wstring const& inName,
			bool inUseShortName = false,
			ARBConfigMultiQPtr* outMultiQ = nullptr) const;

	/**
	 * Find a MultiQ by value.
	 * @param inMultiQ Multi-Q definition to find
	 * @param outMultiQ Pointer to object, NULL if not found.
	 */
	bool FindMultiQ(
			ARBConfigMultiQ const& inMultiQ,
			ARBConfigMultiQPtr* outMultiQ = nullptr) const;

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
	 * @param inDiv Division level is in.
	 * @param inOldLevel Level name being renamed.
	 * @param inNewLevel New level name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameLevel(
			std::wstring const& inDiv,
			std::wstring const& inOldLevel,
			std::wstring const& inNewLevel);

	/**
	 * Delete a level.
	 * @param inLevel Level name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteLevel(std::wstring const& inLevel);

	/**
	 * Rename an event.
	 * @param inOldEvent Event name being renamed.
	 * @param inNewEvent New event name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameEvent(
			std::wstring const& inOldEvent,
			std::wstring const& inNewEvent);

	/**
	 * Delete an event.
	 * @param inEvent Event name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteEvent(std::wstring const& inEvent);

	/**
	 * Add a MultiQ.
	 * @param inMultiQ MultiQ to add.
	 * @return Whether the object was added.
	 */
	bool AddMultiQ(ARBConfigMultiQPtr const& inMultiQ);

	/**
	 * Delete an MultiQ.
	 * @param inMultiQ Name of MultiQ to delete.
	 * @return Whether object was deleted or not.
	 */
	bool DeleteMultiQ(ARBConfigMultiQPtr const& inMultiQ);
};
