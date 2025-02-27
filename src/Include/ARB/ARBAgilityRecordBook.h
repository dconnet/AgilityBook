#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBAgilityRecordBook class
 * @author David Connet
 *
 * Revision History
 * 2020-09-15 Remove CalSite from ARB.
 * 2014-06-09 Add access to write-only data for file-properties purpose.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2007-08-14 Separated DTD defines into ARBStructure.h
 * 2007-08-02 Added 'show' to 'Title', 'timestamp' to 'AgilityBook'
 *            Added 'CalSite' to config. 'SecEmail'/etc to 'Calendar'
 * 2007-04-22 Added 'Accom', 'Confirm' to 'Calendar', 'icon' to 'Venue'
 * 2007-02-27 Added 'defValue'.
 * 2007-01-03 Changed 'CRCDMeta' to 'CRCDMeta2'.
 * 2006-07-02 Added 'subtractTF'.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2006-02-08 Added 'RenameEvent'.
 * 2005-12-04 Added 'bonusPts' to 'Scoring','Run'(scoring)
 * 2005-10-26 Added 'Prefix' to 'Titles', 'update' to 'Configuration'.
 * 2005-04-26 Added 'ver' to 'AgilityBook'
 * 2005-01-11 Added 'Multiple' to 'Titles', 'instance' to 'Title'.
 * 2005-01-02 Added subnames to events.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-11-15 Added TimeFaults for T+F, ClubInfo, LocationInfo
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-03-31 Started adding auto-lifetime point accumulation.
 * 2004-03-30 Changed Table-in-YPS to hasTable, added to ConfigScoring.
 *            Added Link to runs.
 * 2004-02-18 Added 'DeleteTitle' configuration action.
 * 2004-02-14 Added TableInYPS in run scoring.
 * 2004-02-02 Added ARBDogExistingPoints items.
 * 2004-01-21 Added ARBConfigAction items.
 * 2003-12-07 Added Info section.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-10-31 Added options to Save() to allow partial saves.
 * 2003-10-13 Made Time/CourseFaults common for all types of scoring.
 */

#include "ARBCalendar.h"
#include "ARBConfig.h"
#include "ARBDog.h"
#include "ARBInfo.h"
#include "ARBStructure.h"
#include "ARBTraining.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"

#include <set>


namespace dconSoft
{
namespace ARB
{

enum class ARBFileInfo
{
	Book = 0,      // ARB document version
	OS = 1,        // OS name (with some info)0
	Platform = 2,  // Platform (arch)
	TimeStamp = 3, // File written
	Version = 4,   // Version of ARB that wrote file
	Max = 5,       // Version of ARB that wrote file
};


/**
 * The main data class.
 */
class ARB_API ARBAgilityRecordBook
{
public:
	/**
	 * Return the current document version. This helps make sure an older
	 * version of the program doesn't read a newer version of the file.
	 */
	static ARBCommon::ARBVersion const& GetCurrentDocVersion();

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
		ARBCommon::ElementNodePtr const& inTree,
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
	bool Load(ARBCommon::ElementNodePtr const& inTree, ARBErrorCallback& ioCallback)
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
		ARBCommon::ElementNodePtr const& outTree,
		wxString const& inPgmVer,
		bool inCalendar,
		bool inTraining,
		bool inConfig,
		bool inInfo,
		bool inDogs) const;

	/**
	 * Create a default document: No dogs, default configuration.
	 * @param inHandler Interface to deal with platform specific resource issues
	 * @post Clears all content, then creates default configuration.
	 */
	void Default(IARBConfigHandler const* inHandler);

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
	bool Update(int indent, ARBConfig const& inConfigNew, wxString& ioInfo, IConfigActionCallback& ioCallBack);

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
	size_t GetAllClubNames(std::set<wxString>& outClubs, bool bInfo, bool bVisibleOnly) const;

	/**
	 * Get all trial locations in use from existing trials and calendar entries.
	 * @param outLocations List of locations.
	 * @param bInfo Include locations from the ARBInfo.
	 * @param bVisibleOnly When including ARBInfo, only get visible items.
	 * @return Number of locations.
	 */
	size_t GetAllTrialLocations(std::set<wxString>& outLocations, bool bInfo, bool bVisibleOnly) const;

	/**
	 * Get all the subnames in use for the given event.
	 * @param inVenue Venue to find event in.
	 * @param inEvent Event info to match.
	 * @param outNames List of Sub-Names in use.
	 * @return Number of subnames.
	 */
	size_t GetAllEventSubNames(wxString const& inVenue, ARBConfigEventPtr const& inEvent, std::set<wxString>& outNames)
		const;

	/**
	 * Get all heights in use from existing runs.
	 * @param outHeights List of heights.
	 * @return Number of heights.
	 */
	size_t GetAllHeights(std::set<wxString>& outHeights) const;

	/**
	 * Get all callnames in use from existing runs.
	 * @param outNames List of names.
	 * @return Number of names.
	 */
	size_t GetAllCallNames(std::set<wxString>& outNames) const;

	/**
	 * Get all breeds in use from existing runs.
	 * @param outBreeds List of breeds.
	 * @return Number of breeds.
	 */
	size_t GetAllBreeds(std::set<wxString>& outBreeds) const;

	/**
	 * Get all judges in use from existing runs and Judges information.
	 * @param outJudges List of judges.
	 * @param bInfo Include judges from the ARBInfo.
	 * @param bVisibleOnly When including ARBInfo, only get visible items.
	 * @return Number of judges.
	 */
	size_t GetAllJudges(std::set<wxString>& outJudges, bool bInfo, bool bVisibleOnly) const;

	/**
	 * Get all handlers in use from existing runs.
	 * @param outHandlers List of handlers.
	 * @return Number of handlers.
	 */
	size_t GetAllHandlers(std::set<wxString>& outHandlers) const;

	/**
	 * Get all pairs partners in use from existing runs.
	 * @param outPartners List of handlers.
	 * @param outDogs List of dogs.
	 */
	void GetAllPartners(std::set<wxString>& outPartners, std::set<wxString>& outDogs) const;

	/**
	 * Get all fault types in use from existing runs and configuration.
	 * @param outFaults List of faults.
	 * @return Number of faults.
	 */
	size_t GetAllFaultTypes(std::set<wxString>& outFaults) const;

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

	/*
	 * Access to write-only data.
	 */
	wxString GetFileInfo(ARBFileInfo type) const
	{
		if (static_cast<size_t>(type) < m_FileInfo.size())
			return m_FileInfo[static_cast<size_t>(type)];
		return wxString();
	}

private:
	ARBCalendarList m_Calendar;
	ARBTrainingList m_Training;
	ARBConfig m_Config;
	ARBInfo m_Info;
	ARBDogList m_Dogs;
	mutable std::vector<wxString> m_FileInfo;

	DECLARE_NO_COPY_IMPLEMENTED(ARBAgilityRecordBook);
};

} // namespace ARB
} // namespace dconSoft
