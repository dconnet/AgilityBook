#pragma once

/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @brief ARBAgilityRecordBook class, global functions and defines.
 * @author David Connet
 *
 * This file also contains global function declarations and defines that
 * centralize most strings that would need globalization. The defines also
 * list all the elements and attributes from the DTD.
 *
 * Revision History
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

/*
 * These defines make up the structure of the xml.
 * Make sure these agree with the .dtd.
 *
 * Defines that are indented are the valid values for the attribute that
 * was just defined.
 */
#define TREE_BOOK							_T("AgilityBook")
#define TREE_CALENDAR						_T("Calendar")
#define TREE_TRAINING						_T("Training")
#define TREE_CONFIG							_T("Configuration")
#define TREE_ACTION							_T("Action")
#define TREE_VENUE							_T("Venue")
#define TREE_VENUE_DESC						_T("Desc")
#define TREE_MULTIQ							_T("MultiQ")
#define TREE_MULTIQ_ITEM					_T("MultiQItem")
#define TREE_DIVISION						_T("Division")
#define TREE_LEVEL							_T("Level")
#define TREE_TITLES							_T("Titles")
#define TREE_SUBLEVEL						_T("SubLevel")
#define TREE_EVENT							_T("Event")
#define TREE_EVENT_DESC						_T("Desc")
#define TREE_EVENT_SUBNAME					_T("SubName")
#define TREE_SCORING						_T("Scoring")
#define TREE_TITLE_POINTS					_T("TitlePoints")
#define TREE_LIFETIME_POINTS				_T("LifeTime")
#define TREE_FAULTTYPE						_T("FaultType")
#define TREE_OTHERPTS						_T("OtherPts")
#define TREE_INFO							_T("Info")
#define TREE_CLUBINFO						_T("ClubInfo")
#define TREE_JUDGEINFO						_T("JudgeInfo")
#define TREE_LOCATIONINFO					_T("LocationInfo")
#define TREE_DOG							_T("Dog")
#define TREE_REGNAME						_T("RegisteredName")
#define TREE_BREED							_T("Breed")
#define TREE_NOTE							_T("Note")
#define TREE_EXISTING_PTS					_T("ExistingPoints")
#define TREE_REG_NUM						_T("RegNum")
#define TREE_TITLE							_T("Title")
#define TREE_TRIAL							_T("Trial")
#define TREE_LOCATION						_T("Location")
#define TREE_CLUB							_T("Club")
#define TREE_RUN							_T("Run")
#define TREE_CONDITIONS						_T("Conditions")
#define TREE_JUDGE							_T("Judge")
#define TREE_HANDLER						_T("Handler")
#define TREE_PARTNER						_T("Partner")
#define TREE_BY_TIME						_T("ByTime")
#define TREE_BY_OPENCLOSE					_T("ByOpenClose")
#define TREE_BY_POINTS						_T("ByPoints")
#define TREE_PLACEMENT						_T("Placement")
#define TREE_PLACEMENT_OTHERPOINTS			_T("OtherPoints")
#define TREE_NOTES							_T("Notes")
#define TREE_FAULTS							_T("Faults")
#define TREE_CRCD							_T("CRCD")
#define TREE_CRCD_META						_T("CRCDMeta")
#define TREE_OTHER							_T("Other")
#define TREE_RUN_LINK						_T("Link")
#define TREE_REF_RUN						_T("ReferenceRun")
#define TREE_REF_NAME						_T("Name")
#define TREE_REF_BREED						_T("Breed")
#define TREE_REF_SCORE						_T("ScoreOrFaults")
#define TREE_REF_NOTE						_T("Note")
#define ATTRIB_BOOK_VERSION					_T("Book")
#define ATTRIB_BOOK_PGM_VERSION				_T("ver")
#define ATTRIB_CONFIG_VERSION				_T("version")
#define ATTRIB_CONFIG_UPDATE				_T("update")
#define ATTRIB_CAL_START					_T("DateStart")
#define ATTRIB_CAL_END						_T("DateEnd")
#define ATTRIB_CAL_OPENING					_T("DateOpening")
#define ATTRIB_CAL_CLOSING					_T("DateClosing")
#define ATTRIB_CAL_MAYBE					_T("isTentative")
#define ATTRIB_CAL_LOCATION					_T("Location")
#define ATTRIB_CAL_CLUB						_T("Club")
#define ATTRIB_CAL_VENUE					_T("Venue")
#define ATTRIB_CAL_ENTERED					_T("Entered")
#define ATTRIB_CAL_NOTE						_T("Note")
#define ATTRIB_TRAINING_DATE				_T("Date")
#define ATTRIB_TRAINING_NAME				_T("Name")
#define ATTRIB_TRAINING_SUBNAME				_T("SubName")
#define ATTRIB_ACTION_VERB					_T("Verb")
	#define ACTION_VERB_DELETE_TITLE			_T("DeleteTitle")
	#define ACTION_VERB_RENAME_TITLE			_T("RenameTitle")
#define ATTRIB_ACTION_VENUE					_T("Venue")
#define ATTRIB_ACTION_DIVISION				_T("Div")
#define ATTRIB_ACTION_OLDNAME				_T("OldName")
#define ATTRIB_ACTION_NEWNAME				_T("NewName")
#define ATTRIB_VENUE_NAME					_T("Name")
#define ATTRIB_VENUE_LONGNAME				_T("LongName")
#define ATTRIB_MULTIQ_NAME					_T("Name")
#define ATTRIB_MULTIQ_SHORTNAME				_T("SName")
#define ATTRIB_MULTIQ_VALID_FROM			_T("ValidFrom")
#define ATTRIB_MULTIQ_VALID_TO				_T("ValidTo")
#define ATTRIB_MULTIQ_ITEM_DIV				_T("Div")
#define ATTRIB_MULTIQ_ITEM_LEVEL			_T("Level")
#define ATTRIB_MULTIQ_ITEM_EVENT			_T("Event")
#define ATTRIB_OTHERPTS_NAME				_T("Name")
#define ATTRIB_OTHERPTS_COUNT				_T("Count")
#define ATTRIB_DIVISION_NAME				_T("Name")
#define ATTRIB_LEVEL_NAME					_T("Name")
#define ATTRIB_TITLES_NAME					_T("Name")
#define ATTRIB_TITLES_LONGNAME				_T("LongName")
#define ATTRIB_TITLES_MULTIPLE				_T("Multiple")
#define ATTRIB_TITLES_PREFIX				_T("Prefix")
#define ATTRIB_TITLES_VALIDFROM				_T("ValidFrom")
#define ATTRIB_TITLES_VALIDTO				_T("ValidTo")
#define ATTRIB_SUBLEVEL_NAME				_T("Name")
#define ATTRIB_EVENT_NAME					_T("Name")
#define ATTRIB_EVENT_HAS_TABLE				_T("hasTable")
#define ATTRIB_EVENT_HASPARTNER				_T("hasPartner")
#define ATTRIB_EVENT_HASSUBNAMES			_T("hasSubNames")
#define ATTRIB_SCORING_VALIDFROM			_T("ValidFrom")
#define ATTRIB_SCORING_VALIDTO				_T("ValidTo")
#define ATTRIB_SCORING_DIVISION				_T("Division")
#define ATTRIB_SCORING_LEVEL				_T("Level")
#define ATTRIB_SCORING_TYPE					_T("type")
#define ATTRIB_SCORING_DROPFRACTIONS		_T("dropFractions")
#define ATTRIB_SCORING_TIMEFAULTS_CLEAN_Q	_T("cleanQ")
#define ATTRIB_SCORING_TIMEFAULTS_UNDER		_T("underTF")
#define ATTRIB_SCORING_TIMEFAULTS_OVER		_T("overTF")
#define ATTRIB_SCORING_OPENINGPTS			_T("OpeningPts")
#define ATTRIB_SCORING_CLOSINGPTS			_T("ClosingPts")
#define ATTRIB_SCORING_SUPERQ				_T("superQ")
#define ATTRIB_SCORING_SPEEDPTS				_T("speedPts")
#define ATTRIB_SCORING_BONUSPTS				_T("bonusPts")
#define ATTRIB_TITLE_POINTS_POINTS			_T("Points")
#define ATTRIB_TITLE_POINTS_FAULTS			_T("Faults")
#define ATTRIB_LIFETIME_POINTS_POINTS		_T("Points")
#define ATTRIB_LIFETIME_POINTS_FAULTS		_T("Faults")
#define ATTRIB_INFO_NAME					_T("Name")
#define ATTRIB_DOG_CALLNAME					_T("CallName")
#define ATTRIB_DOG_DOB						_T("DOB")
#define ATTRIB_DOG_DECEASED					_T("Deceased")
#define ATTRIB_EXISTING_PTS_DATE			_T("Date")
#define ATTRIB_EXISTING_PTS_TYPE			_T("Type")
	#define EXISTING_PTS_TYPE_OTHER				_T("Other")
	#define EXISTING_PTS_TYPE_RUNS				_T("Run")
	#define EXISTING_PTS_TYPE_SPEED				_T("Speed")
	#define EXISTING_PTS_TYPE_MQ				_T("MQ")
	#define EXISTING_PTS_TYPE_SQ				_T("SQ")
#define ATTRIB_EXISTING_PTS_OTHER			_T("Other")
#define ATTRIB_EXISTING_PTS_VENUE			_T("Venue")
#define ATTRIB_EXISTING_PTS_MULTIQ			_T("MultiQ")
#define ATTRIB_EXISTING_PTS_DIV				_T("Div")
#define ATTRIB_EXISTING_PTS_LEVEL			_T("Level")
#define ATTRIB_EXISTING_PTS_EVENT			_T("Event")
#define ATTRIB_EXISTING_PTS_SUBNAME			_T("SubName")
#define ATTRIB_EXISTING_PTS_POINTS			_T("Pts")
#define ATTRIB_REG_NUM_VENUE				_T("Venue")
#define ATTRIB_REG_NUM_NUMBER				_T("Number")
#define ATTRIB_REG_NUM_HEIGHT				_T("Height")
#define ATTRIB_REG_NUM_RECEIVED				_T("isReceived")
#define ATTRIB_TITLE_VENUE					_T("Venue")
#define ATTRIB_TITLE_NAME					_T("Name")
#define ATTRIB_TITLE_DATE					_T("Date")
#define ATTRIB_TITLE_INSTANCE				_T("instance")
#define ATTRIB_TITLE_RECEIVED				_T("isReceived")
#define ATTRIB_TITLE_HIDDEN					_T("isHidden")
#define ATTRIB_TRIAL_VERIFIED				_T("Verified")
#define ATTRIB_CLUB_VENUE					_T("Venue")
#define ATTRIB_RUN_DATE						_T("Date")
#define ATTRIB_RUN_DIVISION					_T("Division")
#define ATTRIB_RUN_LEVEL					_T("Level")
#define ATTRIB_RUN_HEIGHT					_T("Height")
#define ATTRIB_RUN_EVENT					_T("Event")
#define ATTRIB_RUN_SUBNAME					_T("SubName")
#define ATTRIB_PARTNER_HANDLER				_T("Handler")
#define ATTRIB_PARTNER_DOG					_T("Dog")
#define ATTRIB_PARTNER_REGNUM				_T("RegNum")
#define ATTRIB_SCORING_FAULTS				_T("CourseFaults")
#define ATTRIB_SCORING_TIME					_T("Time")
#define ATTRIB_SCORING_HAS_TABLE			_T("hasTable")
#define ATTRIB_SCORING_TF_MULTIPLIER		_T("timeFault")
#define ATTRIB_SCORING_SCT					_T("SCT")
#define ATTRIB_SCORING_SCT2					_T("SCT2")
#define ATTRIB_BY_TIME_YARDS				_T("Yards")
#define ATTRIB_BY_OPENCLOSE_NEEDOPEN		_T("NeedOpenPts")
#define ATTRIB_BY_OPENCLOSE_NEEDCLOSE		_T("NeedClosePts")
#define ATTRIB_BY_OPENCLOSE_GOTOPEN			_T("OpenPts")
#define ATTRIB_BY_OPENCLOSE_GOTCLOSE		_T("ClosePts")
#define ATTRIB_BY_POINTS_NEED				_T("NeedPts")
#define ATTRIB_BY_POINTS_GOT				_T("Points")
#define ATTRIB_PLACEMENT_Q					_T("Q")
#define ATTRIB_PLACEMENT_PLACE				_T("Place")
#define ATTRIB_PLACEMENT_INCLASS			_T("InClass")
#define ATTRIB_PLACEMENT_DOGSQD				_T("DogsQd")
#define ATTRIB_PLACEMENT_OTHERPOINTS_NAME	_T("Name")
#define ATTRIB_PLACEMENT_OTHERPOINTS_POINTS	_T("Points")
#define ATTRIB_REF_RUN_Q					_T("Q")
#define ATTRIB_REF_RUN_PLACE				_T("Place")
#define ATTRIB_REF_RUN_TIME					_T("Time")
#define ATTRIB_REF_RUN_HEIGHT				_T("Height")

/*
 * These defines list any special values.
 */
#define WILDCARD_DIVISION		_T("*")
#define WILDCARD_LEVEL			_T("*")

/*
 * Localization stuff
 *  There is some stuff in:
 *    Element.cpp (errors due to XML parsing failures)
 */

// Used when loading data.
#define UNKNOWN_VERSION			_T("Unknown document version")
#define WARNING_NEWER_DOC		_T("Warning: The file you are loading was created by a newer version of this program. Saving this file with this version of the program will result in a loss of data.\n\nAre you sure you want to continue?")
#define INVALID_DOC_STRUCTURE	_T("Invalid document structure")
#define INVALID_FILE_FORMAT		_T("Invalid file format: '")
#define INVALID_FILE_MISSING_ATTRIB	_T("' is missing required attribute '")
#define INVALID_FILE_BAD_ATTRIB	_T("' has an invalid value for the attribute '")
#define INVALID_DATE			_T("Invalid date: ")
#define INVALID_VALUE			_T("Invalid value: ")
#define INVALID_DIV_LEVEL		_T("Division/level pair has not been defined: ")
#define INVALID_EVENT_NAME		_T("Event has not been defined: ")
#define INVALID_ROOT			_T("'") TREE_BOOK _T("' must be the root element.")
#define MISSING_CONFIG			_T("Missing '") TREE_CONFIG _T("' section.")
#define INVALID_CONFIG			_T("Only one '") TREE_CONFIG _T("' section is allowed.")
#define INVALID_VENUE_CONFIG	_T("All '") TREE_DIVISION _T("' elements must come before '") TREE_EVENT _T("' elements.")
#define INVALID_DIV_NAME		_T("Invalid division name: ")
#define INVALID_VENUE_NAME		_T("Invalid venue name: ")
#define INVALID_OTHER_PTS_NAME	_T("Invalid Other Points name: ")
#define INVALID_MULTIQ_NAME		_T("Invalid Multiple Q name: ")
#define INVALID_MULTIQ_CONVERSION	_T("Unable to convert Existing QQ Points")
#define INVALID_EVENT			_T("Unable to locate an event description in any of the venues of the affiliated clubs: ")
#define INVALID_TITLE			_T("Title is not defined in the Venue description: ")
#define VALID_VALUES			_T("Valid values: ")
#define VALID_VALUES_BOOL		_T("Valid values: 'y', 'n'")
#define VALID_VALUES_ENTRY		_T("Valid values: 'E', 'P', 'N'")
#define VALID_VALUES_OTHERPT	_T("Valid values: 'All', 'AllByEvent', 'Level', 'LevelByEvent'")
#define VALID_VALUES_SCORE		_T("Valid values: 'FaultsThenTime', 'Faults100ThenTime', 'Faults200ThenTime', 'OCScoreThenTime', 'ScoreThenTime', 'TimePlusFaults'")

// Strings for formatting the information returned when updating configurations.
// - Changed to inline functions in order to remove sprintf style formatting.
#include "ARBTypes.h"
inline ARBString UPDATE_FORMAT_FAULTS(int nNew, int nSkipped)
{
	ARBostringstream buffer;
	buffer << _T("Faults: ") << nNew << _T(" added, ") << nSkipped << _T(" identical") << std::endl;
	return buffer.str();
}
inline ARBString UPDATE_FORMAT_OTHERPTS(int nNew, int nUpdated, int nSkipped)
{
	ARBostringstream buffer;
	buffer << _T("Other Points: ") << nNew << _T(" added, ") << nUpdated << _T(" updated, ") << nSkipped << _T(" identical") << std::endl;
	return buffer.str();
}
inline ARBString UPDATE_FORMAT_VENUES(int nNew, int nUpdated, int nSkipped)
{
	ARBostringstream buffer;
	buffer << _T("Venues: ") << nNew << _T(" added, ") << nUpdated << _T(" updated, ") << nSkipped << _T(" identical") << std::endl;
	return buffer.str();
}
inline ARBString UPDATE_FORMAT_DIVISIONS(int nAdded, int nChanged, int nSkipped)
{
	ARBostringstream buffer;
	buffer << _T("Divisions: ") << nAdded << _T(" added, ") << nChanged << _T(" updated, ") << nSkipped << _T(" identical") << std::endl;
	return buffer.str();
}
inline ARBString UPDATE_FORMAT_EVENTS(int nAdded, int nChanged, int nSkipped)
{
	ARBostringstream buffer;
	buffer << _T("Events: ") << nAdded << _T(" added, ") << nChanged << _T(" updated, ") << nSkipped << _T(" identical") << std::endl;
	return buffer.str();
}
inline ARBString UPDATE_FORMAT_MULTIQS(int nAdded, int nDeleted, int nSkipped)
{
	ARBostringstream buffer;
	buffer << _T("MultiQs: ") << nAdded << _T(" added, ") << nDeleted << _T(" deleted, ") << nSkipped << _T(" identical") << std::endl;
	return buffer.str();
}
inline ARBString UPDATE_FORMAT_LEVELS(int nAdded, int nChanged, int nSkipped)
{
	ARBostringstream buffer;
	buffer << _T("Levels: ") << nAdded << _T(" added, ") << nChanged << _T(" updated, ") << nSkipped << _T(" identical") << std::endl;
	return buffer.str();
}
inline ARBString UPDATE_FORMAT_TITLES(int nAdded, int nChanged, int nSkipped)
{
	ARBostringstream buffer;
	buffer << _T("Titles: ") << nAdded << _T(" added, ") << nChanged << _T(" updated, ") << nSkipped << _T(" identical") << std::endl;
	return buffer.str();
}
inline ARBString UPDATE_FORMAT_SUBLEVELS(int nAdded)
{
	ARBostringstream buffer;
	buffer << nAdded << _T(" new sub-levels") << std::endl;
	return buffer.str();
}
inline ARBString UPDATE_FORMAT_RULES(int nAdded, int nDeleted, int nChanged, int nSkipped)
{
	ARBostringstream buffer;
	buffer << _T(" rules: ") << nAdded << _T(" added, ") << nDeleted << _T(" deleted, ") << nChanged << _T(" updated, ") << nSkipped << _T(" identical") << std::endl;
	return buffer.str();
}

// Used in ARBConfigTitlePoints.cpp
inline ARBString TITLE_POINTS_NAME_FORMAT(double points, short faults)
{
	ARBostringstream buffer;
	buffer << points << _T(" points with ") << faults << _T(" faults");
	return buffer.str();
}
inline ARBString LIFETIME_POINTS_NAME_FORMAT(double points, short faults)
{
	ARBostringstream buffer;
	buffer << points << _T(" lifetime points with ") << faults << _T(" faults");
	return buffer.str();
}

// Used in ARBConfigScoring.cpp
#define SCORE_STYLE_UNKNOWN			_T("Unknown")
#define SCORE_STYLE_FAULTSTIME		_T("Faults Then Time")
#define SCORE_STYLE_FAULTS100TIME	_T("100 Minus Faults Then Time")
#define SCORE_STYLE_FAULTS200TIME	_T("200 Minus Faults Then Time")
#define SCORE_STYLE_OCSCORETIME		_T("Opening/Closing Points Then Time")
#define SCORE_STYLE_SCORETIME		_T("Points Then Time")
#define SCORE_STYLE_TIMEPLUSFAULTS	_T("Time Plus Faults")

// Used in ARBTypes.cpp
#define ARBQ_TYPE_NA				_T("NA")
#define ARBQ_TYPE_Q					_T("Q")
#define ARBQ_TYPE_NQ				_T("NQ")
#define ARBQ_TYPE_E					_T("E")
#define ARBQ_TYPE_SQ				_T("SQ")

// Used in ARBDogExistingPoints.cpp
#define EXISTING_POINTS_OTHER		_T("OtherPoints")
#define EXISTING_POINTS_RUN			_T("Run")
#define EXISTING_POINTS_SPEED		_T("Speed")
#define EXISTING_POINTS_MQ			_T("Multiple Q")
#define EXISTING_POINTS_SQ			_T("SuperQ")


#include <list>
#include <set>
#include "ARBCalendar.h"
#include "ARBConfig.h"
#include "ARBDog.h"
#include "ARBInfo.h"
#include "ARBTraining.h"
class ARBConfigOtherPoints;
class ARBVersion;
class Element;

/**
 * Error callback class.
 */
class ARBErrorCallback
{
public:
	ARBErrorCallback(ARBString& ioErrMsg)
		: m_ErrMsg(ioErrMsg)
	{
	}
	virtual ~ARBErrorCallback() {}

	/**
	 * Error message that requires a user response.
	 * @param pMsg Message to display to user.
	 * @return True to continue, false to abort.
	 */
	virtual bool OnError(TCHAR const* const pMsg)
	{
		return false;
	}

	/**
	 * Log an error message.
	 */
	virtual void LogMessage(ARBString const& inMsg)
	{
		m_ErrMsg += inMsg;
	}

protected:
	ARBString& m_ErrMsg;
};

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
		Element const& inTree,
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
	bool Load(Element const& inTree, ARBErrorCallback& ioCallback)
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
		Element& outTree,
		ARBString const& inPgmVer,
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

	//
	// Convenience functions that do some data accumulation.
	//

	/**
	 * Get all club names in use from existing trials and calendar entries.
	 * @param outClubs List of clubs.
	 * @param bInfo Include clubs from the ARBInfo.
	 * @return Number of clubs.
	 */
	size_t GetAllClubNames(std::set<ARBString>& outClubs, bool bInfo = true) const;

	/**
	 * Get all trial locations in use from existing trials and calendar entries.
	 * @param outLocations List of locations.
	 * @param bInfo Include locations from the ARBInfo.
	 * @return Number of locations.
	 */
	size_t GetAllTrialLocations(std::set<ARBString>& outLocations, bool bInfo = true) const;

	/**
	 * Get all the subnames in use for the given event.
	 * @param inVenue Venue to find event in.
	 * @param inEvent Event info to match.
	 * @param outNames List of Sub-Names in use.
	 * @return Number of subnames.
	 */
	size_t GetAllEventSubNames(ARBString const& inVenue, ARBConfigEvent const* inEvent, std::set<ARBString>& outNames) const;

	/**
	 * Get all heights in use from existing runs.
	 * @param outHeights List of heights.
	 * @return Number of heights.
	 */
	size_t GetAllHeights(std::set<ARBString>& outHeights) const;

	/**
	 * Get all judges in use from existing runs and Judges information.
	 * @param outJudges List of judges.
	 * @param bInfo Include judges from the ARBInfo.
	 * @return Number of judges.
	 */
	size_t GetAllJudges(std::set<ARBString>& outJudges, bool bInfo = true) const;

	/**
	 * Get all handlers in use from existing runs.
	 * @param outHandlers List of handlers.
	 * @return Number of handlers.
	 */
	size_t GetAllHandlers(std::set<ARBString>& outHandlers) const;

	/**
	 * Get all fault types in use from existing runs and configuration.
	 * @param outFaults List of faults.
	 * @return Number of faults.
	 */
	size_t GetAllFaultTypes(std::set<ARBString>& outFaults) const;

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

inline ARBCalendarList const& ARBAgilityRecordBook::GetCalendar() const
{
	return m_Calendar;
}

inline ARBCalendarList& ARBAgilityRecordBook::GetCalendar()
{
	return m_Calendar;
}

inline ARBTrainingList const& ARBAgilityRecordBook::GetTraining() const
{
	return m_Training;
}

inline ARBTrainingList& ARBAgilityRecordBook::GetTraining()
{
	return m_Training;
}

inline ARBConfig const& ARBAgilityRecordBook::GetConfig() const
{
	return m_Config;
}

inline ARBConfig& ARBAgilityRecordBook::GetConfig()
{
	return m_Config;
}

inline ARBInfo const& ARBAgilityRecordBook::GetInfo() const
{
	return m_Info;
}

inline ARBInfo& ARBAgilityRecordBook::GetInfo()
{
	return m_Info;
}

inline ARBDogList const& ARBAgilityRecordBook::GetDogs() const
{
	return m_Dogs;
}

inline ARBDogList& ARBAgilityRecordBook::GetDogs()
{
	return m_Dogs;
}

/////////////////////////////////////////////////////////////////////////////
// Global functions

/**
 * Return an error message about invalid document structure.
 *
 * @param inMsg Additional error information.
 * @return Message with newline.
 */
extern ARBString ErrorInvalidDocStructure(TCHAR const* const inMsg);

/**
 * Return an error message about a missing required attribute.
 *
 * @param inElement Element containing missing attribute.
 * @param inAttrib Attribute name that is missing.
 * @param inMsg Additional error information.
 * @return Message with newline.
 */
extern ARBString ErrorMissingAttribute(
	TCHAR const* const inElement,
	TCHAR const* const inAttrib,
	TCHAR const* const inMsg = NULL);

/**
 * Return an error message about an invalid value in an attribute.
 *
 * @param inElement Element containing bad attribute.
 * @param inAttrib Attribute name whose value is bad.
 * @param inMsg Additional error information.
 * @return Message with newline.
 */
extern ARBString ErrorInvalidAttributeValue(
	TCHAR const* const inElement,
	TCHAR const* const inAttrib,
	TCHAR const* const inMsg = NULL);
