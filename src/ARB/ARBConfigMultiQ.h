#pragma once

/*
 * Copyright (c) 2005-2009 David Connet. All Rights Reserved.
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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-07-15 DRC Created.
 */

#include <set>
#include "ARBBase.h"
#include "ARBDate.h"
#include "ARBTypes.h"

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
	virtual wxString GetGenericName() const
	{
		return m_ShortName;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<wxString>& ioStrings) const
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
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigMultiQ element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

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
			wxString const& inOldDiv,
			wxString const& inNewDiv);

	/**
	 * Delete a division.
	 * @param inDiv Division name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteDivision(wxString const& inDiv);

	/**
	 * Rename a level.
	 * @param inDiv Division level is in.
	 * @param inOldLevel Level name being renamed.
	 * @param inNewLevel New level name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameLevel(
			wxString const& inDiv,
			wxString const& inOldLevel,
			wxString const& inNewLevel);

	/**
	 * Delete a level.
	 * @param inLevel Level name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteLevel(wxString const& inLevel);

	/**
	 * Rename an event.
	 * @param inOldEvent Event name being renamed.
	 * @param inNewEvent New event name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameEvent(
			wxString const& inOldEvent,
			wxString const& inNewEvent);

	/**
	 * Delete an event.
	 * @param inEvent Event name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteEvent(wxString const& inEvent);

	/**
	 * Add an item.
	 * @param inDiv Division of event.
	 * @param inLevel Level of event.
	 * @param inEvent Event.
	 * @return Whether object was added.
	 */
	bool AddItem(
			wxString const& inDiv,
			wxString const& inLevel,
			wxString const& inEvent);

	/**
	 * Remove an item.
	 * @param inDiv Division of event.
	 * @param inLevel Level of event.
	 * @param inEvent Event.
	 * @return Whether object was removed.
	 */
	bool RemoveItem(
			wxString const& inDiv,
			wxString const& inLevel,
			wxString const& inEvent);

	/**
	 * Remove all configuration items.
	 */
	bool RemoveAllItems();

	/*
	 * Getters/setters.
	 */
	wxString GetName() const
	{
		return m_Name;
	}
	void SetName(wxString const& inName)
	{
		m_Name = inName;
	}
	wxString GetShortName() const
	{
		return m_ShortName;
	}
	void SetShortName(wxString const& inName)
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
			wxString& outDivision,
			wxString& outLevel,
			wxString& outEvent) const;

private:
	struct MultiQItem
	{
		wxString m_Div;
		wxString m_Level;
		wxString m_Event;
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

	wxString m_Name;
	wxString m_ShortName;
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
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Find a MultiQ by name.
	 * @param inName Multi-Q to find
	 * @param inUseShortName Search by shortname instead of longname.
	 * @param outMultiQ Pointer to object, NULL if not found.
	 */
	bool FindMultiQ(
			wxString const& inName,
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
			wxString const& inOldDiv,
			wxString const& inNewDiv);

	/**
	 * Delete a division.
	 * @param inDiv Division name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteDivision(wxString const& inDiv);

	/**
	 * Rename a level.
	 * @param inDiv Division level is in.
	 * @param inOldLevel Level name being renamed.
	 * @param inNewLevel New level name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameLevel(
			wxString const& inDiv,
			wxString const& inOldLevel,
			wxString const& inNewLevel);

	/**
	 * Delete a level.
	 * @param inLevel Level name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteLevel(wxString const& inLevel);

	/**
	 * Rename an event.
	 * @param inOldEvent Event name being renamed.
	 * @param inNewEvent New event name.
	 * @return Number of ARBConfigScoring items updated.
	 */
	int RenameEvent(
			wxString const& inOldEvent,
			wxString const& inNewEvent);

	/**
	 * Delete an event.
	 * @param inEvent Event name being deleted.
	 * @return Number of ARBConfigScoring items removed.
	 */
	int DeleteEvent(wxString const& inEvent);

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
