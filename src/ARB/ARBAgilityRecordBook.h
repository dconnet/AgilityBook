#pragma once

/*
 * Copyright � 2002-2008 David Connet. All Rights Reserved.
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
 * @brief ARBAgilityRecordBook class
 * @author David Connet
 *
 * Revision History
 * @li 2007-08-14 DRC Separated DTD defines into ARBStructure.h
 * @li 2007-08-02 DRC Added 'show' to 'Title', 'timestamp' to 'AgilityBook'
 *                    Added 'CalSite' to config. 'SecEmail'/etc to 'Calendar'
 * @li 2007-04-22 DRC Added 'Accom', 'Confirm' to 'Calendar', 'icon' to 'Venue'
 * @li 2007-02-27 DRC Added 'defValue'.
 * @li 2007-01-03 DRC Changed 'CRCDMeta' to 'CRCDMeta2'.
 * @li 2006-07-02 DRC Added 'subtractTF'.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2006-02-08 DRC Added 'RenameEvent'.
 * @li 2005-12-04 DRC Added 'bonusPts' to 'Scoring','Run'(scoring)
 * @li 2005-10-26 DRC Added 'Prefix' to 'Titles', 'update' to 'Configuration'.
 * @li 2005-04-26 DRC Added 'ver' to 'AgilityBook'
 * @li 2005-01-11 DRC Added 'Multiple' to 'Titles', 'instance' to 'Title'.
 * @li 2005-01-02 DRC Added subnames to events.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-11-15 DRC Added TimeFaults for T+F, ClubInfo, LocationInfo
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-03-31 DRC Started adding auto-lifetime point accumulation.
 * @li 2004-03-30 DRC Changed Table-in-YPS to hasTable, added to ConfigScoring.
 *                    Added Link to runs.
 * @li 2004-02-18 DRC Added 'DeleteTitle' configuration action.
 * @li 2004-02-14 DRC Added TableInYPS in run scoring.
 * @li 2004-02-02 DRC Added ARBDogExistingPoints items.
 * @li 2004-01-21 DRC Added ARBConfigAction items.
 * @li 2003-12-07 DRC Added Info section.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-10-31 DRC Added options to Save() to allow partial saves.
 * @li 2003-10-13 DRC Made Time/CourseFaults common for all types of scoring.
 */

#include "ARBCalendar.h"
#include "ARBConfig.h"
#include "ARBDog.h"
#include "ARBInfo.h"
#include "ARBStructure.h"
#include "ARBTraining.h"
#include "ARBTypes.h"
#include <set>

/**
 * The main data class.
 */
class ARBAgilityRecordBook
{
public:
	/**
	 * Return the current document version. This helps make sure an older
	 * version of the program doesn't read a newer version of the file.
	 */
	static ARBVersion const& GetCurrentDocVersion();

	ARBAgilityRecordBook();
	~ARBAgilityRecordBook();

	/**
	 * Reset the contents of this object and all sub-objects.
	 * @post All content cleared, including configuration.
	 */
	void clear();

	/**
	 * Load a document. See Element.h for more information on why we use it.
	 * The individual load flags allow us to load just a portion of a document.
	 * @pre If bDogs is true, bConfig must also be true or dogs won't load.
	 * @param inTree XML structure to convert into ARB.
	 * @param inCalendar Load calendar info.
	 * @param inTraining Load training info.
	 * @param inConfig Load config info.
	 * @param inInfo Load the Info (judges) info.
	 * @param inDogs Load dog info.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
		ElementNodePtr inTree,
		bool inCalendar,
		bool inTraining,
		bool inConfig,
		bool inInfo,
		bool inDogs,
		ARBErrorCallback& ioCallback);

	/**
	 * Load an entire document.
	 * @param inTree XML structure to convert into ARB.
	 * @param ioCallback Error processing callback
	 * @return Success
	 */
	bool Load(ElementNodePtr inTree, ARBErrorCallback& ioCallback)
	{
		return Load(inTree, true, true, true, true, true, ioCallback);
	}

	/**
	 * Save a document.
	 * @param outTree XML structure to write ARB to.
	 * @param inPgmVer Program version.
	 * @param inCalendar Save calendar info.
	 * @param inTraining Save training info.
	 * @param inConfig Save config info.
	 * @param inInfo Save the Info (judges) info.
	 * @param inDogs Save dog info, implies inConfig.
	 * @return Success
	 */
	bool Save(
		ElementNodePtr outTree,
		tstring const& inPgmVer,
		bool inCalendar,
		bool inTraining,
		bool inConfig,
		bool inInfo,
		bool inDogs) const;

	/**
	 * Create a default document: No dogs, default configuration.
	 * @post Clears all content, then creates default configuration.
	 */
	void Default();

	/**
	 * Update this configuration from inConfigNew.
	 * @param indent Indentation level for generating messages.
	 * @param inConfigNew Configuration to merge.
	 * @param ioInfo Accumulated messages about changes that have happened.
	 * @param ioCallBack Callback object for informing about deleted objects.
	 * @return Whether or not changes have occurred.
	 * @post Sometimes there are side affects from a configuration update.
	 *       Existing runs may be modified or deleted.
	 */
	bool Update(
			int indent,
			ARBConfig const& inConfigNew,
			otstringstream& ioInfo,
			IConfigActionCallback& ioCallBack);

	//
	// Convenience functions that do some data accumulation.
	//

	/**
	 * Get all club names in use from existing trials and calendar entries.
	 * @param outClubs List of clubs.
	 * @param bInfo Include clubs from the ARBInfo.
	 * @param bVisibleOnly When including ARBInfo, only get visible items.
	 * @return Number of clubs.
	 */
	size_t GetAllClubNames(
			std::set<tstring>& outClubs,
			bool bInfo,
			bool bVisibleOnly) const;

	/**
	 * Get all trial locations in use from existing trials and calendar entries.
	 * @param outLocations List of locations.
	 * @param bInfo Include locations from the ARBInfo.
	 * @param bVisibleOnly When including ARBInfo, only get visible items.
	 * @return Number of locations.
	 */
	size_t GetAllTrialLocations(
			std::set<tstring>& outLocations,
			bool bInfo,
			bool bVisibleOnly) const;

	/**
	 * Get all the subnames in use for the given event.
	 * @param inVenue Venue to find event in.
	 * @param inEvent Event info to match.
	 * @param outNames List of Sub-Names in use.
	 * @return Number of subnames.
	 */
	size_t GetAllEventSubNames(
			tstring const& inVenue,
			ARBConfigEventPtr inEvent,
			std::set<tstring>& outNames) const;

	/**
	 * Get all heights in use from existing runs.
	 * @param outHeights List of heights.
	 * @return Number of heights.
	 */
	size_t GetAllHeights(std::set<tstring>& outHeights) const;

	/**
	 * Get all callnames in use from existing runs.
	 * @param outNames List of names.
	 * @return Number of names.
	 */
	size_t GetAllCallNames(std::set<tstring>& outNames) const;

	/**
	 * Get all breeds in use from existing runs.
	 * @param outBreeds List of breeds.
	 * @return Number of breeds.
	 */
	size_t GetAllBreeds(std::set<tstring>& outBreeds) const;

	/**
	 * Get all judges in use from existing runs and Judges information.
	 * @param outJudges List of judges.
	 * @param bInfo Include judges from the ARBInfo.
	 * @param bVisibleOnly When including ARBInfo, only get visible items.
	 * @return Number of judges.
	 */
	size_t GetAllJudges(
			std::set<tstring>& outJudges,
			bool bInfo,
			bool bVisibleOnly) const;

	/**
	 * Get all handlers in use from existing runs.
	 * @param outHandlers List of handlers.
	 * @return Number of handlers.
	 */
	size_t GetAllHandlers(std::set<tstring>& outHandlers) const;

	/**
	 * Get all pairs partners in use from existing runs.
	 * @param outPartners List of handlers.
	 * @param outDogs List of dogs.
	 */
	void GetAllPartners(
			std::set<tstring>& outPartners,
			std::set<tstring>& outDogs) const;

	/**
	 * Get all fault types in use from existing runs and configuration.
	 * @param outFaults List of faults.
	 * @return Number of faults.
	 */
	size_t GetAllFaultTypes(std::set<tstring>& outFaults) const;

	/*
	 * Getters.
	 */
	ARBCalendarList const& GetCalendar() const;
	ARBCalendarList& GetCalendar();
	ARBTrainingList const& GetTraining() const;
	ARBTrainingList& GetTraining();
	ARBConfig const& GetConfig() const;
	ARBConfig& GetConfig();
	ARBInfo const& GetInfo() const;
	ARBInfo& GetInfo();
	ARBDogList const& GetDogs() const;
	ARBDogList& GetDogs();

// Intentionally not implemented!
private:
	ARBAgilityRecordBook(ARBAgilityRecordBook const&);
	ARBAgilityRecordBook& operator=(ARBAgilityRecordBook const&);

private:
	ARBCalendarList m_Calendar;
	ARBTrainingList m_Training;
	ARBConfig m_Config;
	ARBInfo m_Info;
	ARBDogList m_Dogs;
};
