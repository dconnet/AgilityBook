#pragma once

/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-03-26 DRC Added 'hasTable'. Used to set default when creating a run.
 * @li 2004-02-02 DRC Added VerifyEvent.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <string>
#include "ARBBase.h"
#include "ARBConfigDivision.h"
#include "ARBConfigScoring.h"
#include "ARBVector.h"
class ARBErrorCallback;
class ARBVersion;
class Element;

/**
 * Event configuration information.
 */
class ARBConfigEvent : public ARBBase
{
public:
	ARBConfigEvent();
	ARBConfigEvent(ARBConfigEvent const& rhs);
	ARBConfigEvent& operator=(ARBConfigEvent const& rhs);
	bool operator==(ARBConfigEvent const& rhs) const;
	bool operator!=(ARBConfigEvent const& rhs) const;

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
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigEvent element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Update this configuration from inEventNew.
	 * @param indent Indentation level for generating messages.
	 * @param inEventNew Event to merge.
	 * @param ioInfo Accumulated messages about changes that have happened.
	 * @return Whether or not changes have occurred.
	 */
	bool Update(int indent, ARBConfigEvent const* inEventNew, std::string& ioInfo);

	/**
	 * Find all the scoring methods that match.
	 * @param inDivision Division event is in.
	 * @param inLevel Level (NOT sublevel) event is in.
	 * @param inTitlePoints Only return scoring methods that have title points.
	 * @param outList List of scoring methods found.
	 * @return Number of items found.
	 */
	size_t FindAllEvents(
		std::string const& inDivision,
		std::string const& inLevel,
		bool inTitlePoints,
		std::vector<ARBConfigScoring const*>& outList) const;

	/**
	 * Verify a scoring method exists.
	 * @param inDivision Division event is in.
	 * @param inLevel Level event is in.
	 * @return true if FindAllEvents() > 0.
	 */
	bool VerifyEvent(
		std::string const& inDivision,
		std::string const& inLevel) const;

	/**
	 * Find an event.
	 * @param inDivision Division event exists in.
	 * @param inLevel Level (NOT sublevel) event exists in.
	 * @param inDate Date for requested scoring.
	 * @return Pointer to object, NULL if not found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBConfigScoring const* FindEvent(
		std::string const& inDivision,
		std::string const& inLevel,
		ARBDate const& inDate) const;

	/*
	 * Getters/setters.
	 */
	std::string const& GetName() const;
	void SetName(std::string const& inName);
	std::string const& GetDesc() const;
	void SetDesc(std::string const& inDesc);
	bool HasTable() const;
	void SetHasTable(bool inBool);
	bool HasPartner() const;
	void SetHasPartner(bool inHas);
	ARBConfigScoringList const& GetScorings() const;
	ARBConfigScoringList& GetScorings();

private:
	~ARBConfigEvent();
	std::string m_Name;
	std::string m_Desc;
	bool m_bTable;
	bool m_bHasPartner;
	ARBConfigScoringList m_Scoring;
};

inline std::string ARBConfigEvent::GetGenericName() const
{
	return m_Name;
}

inline std::string const& ARBConfigEvent::GetName() const
{
	return m_Name;
}

inline void ARBConfigEvent::SetName(std::string const& inName)
{
	m_Name = inName;
}

inline std::string const& ARBConfigEvent::GetDesc() const
{
	return m_Desc;
}

inline void ARBConfigEvent::SetDesc(std::string const& inDesc)
{
	m_Desc = inDesc;
}

inline bool ARBConfigEvent::HasTable() const
{
	return m_bTable;
}

inline void ARBConfigEvent::SetHasTable(bool inBool)
{
	m_bTable = inBool;
}

inline bool ARBConfigEvent::HasPartner() const
{
	return m_bHasPartner;
}

inline void ARBConfigEvent::SetHasPartner(bool inHas)
{
	m_bHasPartner = inHas;
}

inline ARBConfigScoringList const& ARBConfigEvent::GetScorings() const
{
	return m_Scoring;
}

inline ARBConfigScoringList& ARBConfigEvent::GetScorings()
{
	return m_Scoring;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigEvent objects.
 */
class ARBConfigEventList : public ARBVector<ARBConfigEvent>
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
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Verify an event exists.
	 * @param inEvent Event to verify.
	 * @param inDivision Division event exists in.
	 * @param inLevel Level event exists in.
	 * @return Event exists.
	 */
	bool VerifyEvent(
		std::string const& inEvent,
		std::string const& inDivision,
		std::string const& inLevel) const;

	/**
	 * Find an event.
	 * @param inEvent Event to find.
	 * @param inDivision Division event exists in.
	 * @param inLevel Level event exists in.
	 * @param inDate Date for requested scoring.
	 * @return Pointer to object, NULL if not found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBConfigScoring const* FindEvent(
		std::string const& inEvent,
		std::string const& inDivision,
		std::string const& inLevel,
		ARBDate const& inDate) const;

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
	 * Find an event.
	 * @param inEvent Name of event to find.
	 * @return Pointer to object, NULL if not found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBConfigEvent const* FindEvent(std::string const& inEvent) const;
	ARBConfigEvent* FindEvent(std::string const& inEvent);

	/**
	 * Add an event.
	 * @param inEvent Event to add.
	 * @return Pointer to object, NULL if name already exists or is empty.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 *       The pointer is added to the list and its ref count is incremented.
	 */
	ARBConfigEvent* AddEvent(ARBConfigEvent* inEvent);

	/**
	 * Delete an event.
	 * @param inEvent Name of event to delete.
	 * @return Whether event was deleted or not.
	 */
	bool DeleteEvent(std::string const& inEvent);
};
