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
#define TREE_BOOK							"AgilityBook"
#define TREE_CALENDAR						"Calendar"
#define TREE_TRAINING						"Training"
#define TREE_CONFIG							"Configuration"
#define TREE_ACTION							"Action"
#define TREE_VENUE							"Venue"
#define TREE_VENUE_DESC						"Desc"
#define TREE_DIVISION						"Division"
#define TREE_LEVEL							"Level"
#define TREE_TITLES							"Titles"
#define TREE_SUBLEVEL						"SubLevel"
#define TREE_EVENT							"Event"
#define TREE_EVENT_DESC						"Desc"
#define TREE_EVENT_SUBNAME					"SubName"
#define TREE_SCORING						"Scoring"
#define TREE_TITLE_POINTS					"TitlePoints"
#define TREE_LIFETIME_POINTS				"LifeTime"
#define TREE_FAULTTYPE						"FaultType"
#define TREE_OTHERPTS						"OtherPts"
#define TREE_INFO							"Info"
#define TREE_CLUBINFO						"ClubInfo"
#define TREE_JUDGEINFO						"JudgeInfo"
#define TREE_LOCATIONINFO					"LocationInfo"
#define TREE_DOG							"Dog"
#define TREE_REGNAME						"RegisteredName"
#define TREE_BREED							"Breed"
#define TREE_NOTE							"Note"
#define TREE_EXISTING_PTS					"ExistingPoints"
#define TREE_REG_NUM						"RegNum"
#define TREE_TITLE							"Title"
#define TREE_TRIAL							"Trial"
#define TREE_LOCATION						"Location"
#define TREE_CLUB							"Club"
#define TREE_RUN							"Run"
#define TREE_CONDITIONS						"Conditions"
#define TREE_JUDGE							"Judge"
#define TREE_HANDLER						"Handler"
#define TREE_PARTNER						"Partner"
#define TREE_BY_TIME						"ByTime"
#define TREE_BY_OPENCLOSE					"ByOpenClose"
#define TREE_BY_POINTS						"ByPoints"
#define TREE_PLACEMENT						"Placement"
#define TREE_PLACEMENT_OTHERPOINTS			"OtherPoints"
#define TREE_NOTES							"Notes"
#define TREE_FAULTS							"Faults"
#define TREE_CRCD							"CRCD"
#define TREE_CRCD_META						"CRCDMeta"
#define TREE_OTHER							"Other"
#define TREE_RUN_LINK						"Link"
#define TREE_REF_RUN						"ReferenceRun"
#define TREE_REF_NAME						"Name"
#define TREE_REF_BREED						"Breed"
#define TREE_REF_SCORE						"ScoreOrFaults"
#define TREE_REF_NOTE						"Note"
#define ATTRIB_BOOK_VERSION					"Book"
#define ATTRIB_BOOK_PGM_VERSION				"ver"
#define ATTRIB_CONFIG_VERSION				"version"
#define ATTRIB_CAL_START					"DateStart"
#define ATTRIB_CAL_END						"DateEnd"
#define ATTRIB_CAL_OPENING					"DateOpening"
#define ATTRIB_CAL_CLOSING					"DateClosing"
#define ATTRIB_CAL_MAYBE					"isTentative"
#define ATTRIB_CAL_LOCATION					"Location"
#define ATTRIB_CAL_CLUB						"Club"
#define ATTRIB_CAL_VENUE					"Venue"
#define ATTRIB_CAL_ENTERED					"Entered"
#define ATTRIB_CAL_NOTE						"Note"
#define ATTRIB_TRAINING_DATE				"Date"
#define ATTRIB_TRAINING_NAME				"Name"
#define ATTRIB_TRAINING_SUBNAME				"SubName"
#define ATTRIB_ACTION_VERB					"Verb"
	#define ACTION_VERB_DELETE_TITLE			"DeleteTitle"
	#define ACTION_VERB_RENAME_TITLE			"RenameTitle"
#define ATTRIB_ACTION_VENUE					"Venue"
#define ATTRIB_ACTION_OLDNAME				"OldName"
#define ATTRIB_ACTION_NEWNAME				"NewName"
#define ATTRIB_VENUE_NAME					"Name"
#define ATTRIB_VENUE_LONGNAME				"LongName"
#define ATTRIB_OTHERPTS_NAME				"Name"
#define ATTRIB_OTHERPTS_COUNT				"Count"
#define ATTRIB_DIVISION_NAME				"Name"
#define ATTRIB_LEVEL_NAME					"Name"
#define ATTRIB_TITLES_NAME					"Name"
#define ATTRIB_TITLES_LONGNAME				"LongName"
#define ATTRIB_TITLES_MULTIPLE				"Multiple"
#define ATTRIB_SUBLEVEL_NAME				"Name"
#define ATTRIB_EVENT_NAME					"Name"
#define ATTRIB_EVENT_HAS_TABLE				"hasTable"
#define ATTRIB_EVENT_HASPARTNER				"hasPartner"
#define ATTRIB_EVENT_HASSUBNAMES			"hasSubNames"
#define ATTRIB_SCORING_VALIDFROM			"ValidFrom"
#define ATTRIB_SCORING_VALIDTO				"ValidTo"
#define ATTRIB_SCORING_DIVISION				"Division"
#define ATTRIB_SCORING_LEVEL				"Level"
#define ATTRIB_SCORING_TYPE					"type"
#define ATTRIB_SCORING_DROPFRACTIONS		"dropFractions"
#define ATTRIB_SCORING_TIMEFAULTS_UNDER		"underTF"
#define ATTRIB_SCORING_TIMEFAULTS_OVER		"overTF"
#define ATTRIB_SCORING_OPENINGPTS			"OpeningPts"
#define ATTRIB_SCORING_CLOSINGPTS			"ClosingPts"
#define ATTRIB_SCORING_SUPERQ				"superQ"
#define ATTRIB_SCORING_DOUBLEQ				"doubleQ"
#define ATTRIB_SCORING_SPEEDPTS				"speedPts"
#define ATTRIB_TITLE_POINTS_POINTS			"Points"
#define ATTRIB_TITLE_POINTS_FAULTS			"Faults"
#define ATTRIB_LIFETIME_POINTS_POINTS		"Points"
#define ATTRIB_LIFETIME_POINTS_FAULTS		"Faults"
#define ATTRIB_INFO_NAME					"Name"
#define ATTRIB_DOG_CALLNAME					"CallName"
#define ATTRIB_DOG_DOB						"DOB"
#define ATTRIB_DOG_DECEASED					"Deceased"
#define ATTRIB_EXISTING_PTS_DATE			"Date"
#define ATTRIB_EXISTING_PTS_TYPE			"Type"
	#define EXISTING_PTS_TYPE_OTHER				"Other"
	#define EXISTING_PTS_TYPE_RUNS				"Run"
	#define EXISTING_PTS_TYPE_SPEED				"Speed"
	#define EXISTING_PTS_TYPE_QQ				"QQ"
	#define EXISTING_PTS_TYPE_SQ				"SQ"
#define ATTRIB_EXISTING_PTS_OTHER			"Other"
#define ATTRIB_EXISTING_PTS_VENUE			"Venue"
#define ATTRIB_EXISTING_PTS_DIV				"Div"
#define ATTRIB_EXISTING_PTS_LEVEL			"Level"
#define ATTRIB_EXISTING_PTS_EVENT			"Event"
#define ATTRIB_EXISTING_PTS_SUBNAME			"SubName"
#define ATTRIB_EXISTING_PTS_POINTS			"Pts"
#define ATTRIB_REG_NUM_VENUE				"Venue"
#define ATTRIB_REG_NUM_NUMBER				"Number"
#define ATTRIB_REG_NUM_HEIGHT				"Height"
#define ATTRIB_REG_NUM_RECEIVED				"isReceived"
#define ATTRIB_TITLE_VENUE					"Venue"
#define ATTRIB_TITLE_NAME					"Name"
#define ATTRIB_TITLE_DATE					"Date"
#define ATTRIB_TITLE_INSTANCE				"instance"
#define ATTRIB_TITLE_RECEIVED				"isReceived"
#define ATTRIB_TITLE_HIDDEN					"isHidden"
#define ATTRIB_TRIAL_VERIFIED				"Verified"
#define ATTRIB_CLUB_VENUE					"Venue"
#define ATTRIB_RUN_DATE						"Date"
#define ATTRIB_RUN_DIVISION					"Division"
#define ATTRIB_RUN_LEVEL					"Level"
#define ATTRIB_RUN_HEIGHT					"Height"
#define ATTRIB_RUN_EVENT					"Event"
#define ATTRIB_RUN_SUBNAME					"SubName"
#define ATTRIB_PARTNER_HANDLER				"Handler"
#define ATTRIB_PARTNER_DOG					"Dog"
#define ATTRIB_PARTNER_REGNUM				"RegNum"
#define ATTRIB_SCORING_FAULTS				"CourseFaults"
#define ATTRIB_SCORING_TIME					"Time"
#define ATTRIB_SCORING_HAS_TABLE			"hasTable"
#define ATTRIB_SCORING_TF_MULTIPLIER		"timeFault"
#define ATTRIB_SCORING_SCT					"SCT"
#define ATTRIB_SCORING_SCT2					"SCT2"
#define ATTRIB_BY_TIME_YARDS				"Yards"
#define ATTRIB_BY_OPENCLOSE_NEEDOPEN		"NeedOpenPts"
#define ATTRIB_BY_OPENCLOSE_NEEDCLOSE		"NeedClosePts"
#define ATTRIB_BY_OPENCLOSE_GOTOPEN			"OpenPts"
#define ATTRIB_BY_OPENCLOSE_GOTCLOSE		"ClosePts"
#define ATTRIB_BY_POINTS_NEED				"NeedPts"
#define ATTRIB_BY_POINTS_GOT				"Points"
#define ATTRIB_PLACEMENT_Q					"Q"
#define ATTRIB_PLACEMENT_PLACE				"Place"
#define ATTRIB_PLACEMENT_INCLASS			"InClass"
#define ATTRIB_PLACEMENT_DOGSQD				"DogsQd"
#define ATTRIB_PLACEMENT_OTHERPOINTS_NAME	"Name"
#define ATTRIB_PLACEMENT_OTHERPOINTS_POINTS	"Points"
#define ATTRIB_REF_RUN_Q					"Q"
#define ATTRIB_REF_RUN_PLACE				"Place"
#define ATTRIB_REF_RUN_TIME					"Time"
#define ATTRIB_REF_RUN_HEIGHT				"Height"

/*
 * These defines list any special values.
 */
#define WILDCARD_DIVISION		"*"
#define WILDCARD_LEVEL			"*"

/*
 * Localization stuff
 *  There is some stuff in:
 *    Element.cpp (errors due to XML parsing failures)
 */

// Used when loading data.
#define UNKNOWN_VERSION			"Unknown document version"
#define WARNING_NEWER_DOC		"Warning: The file you are loading was created by a newer version of this program. Saving this file with this version of the program will result in a loss of data.\n\nAre you sure you want to continue?"
#define INVALID_DOC_STRUCTURE	"Invalid document structure"
#define INVALID_FILE_FORMAT		"Invalid file format: '"
#define INVALID_FILE_MISSING_ATTRIB	"' is missing required attribute '"
#define INVALID_FILE_BAD_ATTRIB	"' has an invalid value for the attribute '"
#define INVALID_DATE			"Invalid date: "
#define INVALID_VALUE			"Invalid value: "
#define INVALID_DIV_LEVEL		"Division/level pair has not been defined: "
#define INVALID_ROOT			"'" TREE_BOOK "' must be the root element."
#define MISSING_CONFIG			"Missing '" TREE_CONFIG "' section."
#define INVALID_CONFIG			"Only one '" TREE_CONFIG "' section is allowed."
#define INVALID_VENUE_CONFIG	"All '" TREE_DIVISION "' elements must come before '" TREE_EVENT "' elements."
#define INVALID_VENUE_NAME		"Invalid venue name: "
#define INVALID_OTHER_PTS_NAME	"Invalid Other Points name: "
#define INVALID_EVENT			"Unable to locate an event description in any of the venues of the affiliated clubs: "
#define INVALID_TITLE			"Title is not defined in the Venue description: "
#define VALID_VALUES			"Valid values: "
#define VALID_VALUES_BOOL		"Valid values: 'y', 'n'"
#define VALID_VALUES_ENTRY		"Valid values: 'E', 'P', 'N'"
#define VALID_VALUES_OTHERPT	"Valid values: 'All', 'AllByEvent', 'Level', 'LevelByEvent'"
#define VALID_VALUES_SCORE		"Valid values: 'FaultsThenTime', 'Faults100ThenTime', 'Faults200ThenTime', 'OCScoreThenTime', 'ScoreThenTime', 'TimePlusFaults'"

// Strings for formatting the information returned when updating configurations.
// - Changed to inline functions in order to remove sprintf style formatting.
#include <sstream>
#include <string>
inline std::string UPDATE_FORMAT_FAULTS(int nNew, int nSkipped)
{
	std::ostringstream buffer;
	buffer << "Faults: " << nNew << " added, " << nSkipped << " identical" << std::endl;
	return buffer.str();
}
inline std::string UPDATE_FORMAT_OTHERPTS(int nNew, int nUpdated, int nSkipped)
{
	std::ostringstream buffer;
	buffer << "Other Points: " << nNew << " added, " << nUpdated << " updated, " << nSkipped << " identical" << std::endl;
	return buffer.str();
}
inline std::string UPDATE_FORMAT_VENUES(int nNew, int nUpdated, int nSkipped)
{
	std::ostringstream buffer;
	buffer << "Venues: " << nNew << " added, " << nUpdated << " updated, " << nSkipped << " identical" << std::endl;
	return buffer.str();
}
inline std::string UPDATE_FORMAT_DIVISIONS(int nAdded, int nChanged, int nSkipped)
{
	std::ostringstream buffer;
	buffer << "Divisions: " << nAdded << " added, " << nChanged << " updated, " << nSkipped << " identical" << std::endl;
	return buffer.str();
}
inline std::string UPDATE_FORMAT_EVENTS(int nAdded, int nChanged, int nSkipped)
{
	std::ostringstream buffer;
	buffer << "Events: " << nAdded << " added, " << nChanged << " updated, " << nSkipped << " identical" << std::endl;
	return buffer.str();
}
inline std::string UPDATE_FORMAT_LEVELS(int nAdded, int nChanged, int nSkipped)
{
	std::ostringstream buffer;
	buffer << "Levels: " << nAdded << " added, " << nChanged << " updated, " << nSkipped << " identical" << std::endl;
	return buffer.str();
}
inline std::string UPDATE_FORMAT_TITLES(int nAdded, int nChanged, int nSkipped)
{
	std::ostringstream buffer;
	buffer << "Titles: " << nAdded << " added, " << nChanged << " updated, " << nSkipped << " identical" << std::endl;
	return buffer.str();
}
inline std::string UPDATE_FORMAT_SUBLEVELS(int nAdded)
{
	std::ostringstream buffer;
	buffer << nAdded << " new sub-levels" << std::endl;
	return buffer.str();
}
inline std::string UPDATE_FORMAT_RULES(int nAdded, int nDeleted, int nChanged, int nSkipped)
{
	std::ostringstream buffer;
	buffer << " rules: " << nAdded << " added, " << nDeleted << " deleted, " << nChanged << " updated, " << nSkipped << " identical" << std::endl;
	return buffer.str();
}

// Used in ARBConfigTitlePoints.cpp
inline std::string TITLE_POINTS_NAME_FORMAT(short points, short faults)
{
	std::ostringstream buffer;
	buffer << points << " points with " << faults << " faults";
	return buffer.str();
}
inline std::string LIFETIME_POINTS_NAME_FORMAT(short points, short faults)
{
	std::ostringstream buffer;
	buffer << points << " lifetime points with " << faults << " faults";
	return buffer.str();
}

// Used in ARBConfigScoring.cpp
#define SCORE_STYLE_UNKNOWN			"Unknown"
#define SCORE_STYLE_FAULTSTIME		"Faults Then Time"
#define SCORE_STYLE_FAULTS100TIME	"100 Minus Faults Then Time"
#define SCORE_STYLE_FAULTS200TIME	"200 Minus Faults Then Time"
#define SCORE_STYLE_OCSCORETIME		"Opening/Closing Points Then Time"
#define SCORE_STYLE_SCORETIME		"Points Then Time"
#define SCORE_STYLE_TIMEPLUSFAULTS	"Time Plus Faults"

// Used in ARBTypes.cpp
#define ARBQ_TYPE_NA				"NA"
#define ARBQ_TYPE_Q					"Q"
#define ARBQ_TYPE_NQ				"NQ"
#define ARBQ_TYPE_E					"E"
#define ARBQ_TYPE_SQ				"SQ"

// Used in ARBDogExistingPoints.cpp
#define EXISTING_POINTS_OTHER		"OtherPoints"
#define EXISTING_POINTS_RUN			"Run"
#define EXISTING_POINTS_SPEED		"Speed"
#define EXISTING_POINTS_QQ			"Double Q"
#define EXISTING_POINTS_SQ			"SuperQ"


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
	ARBErrorCallback(std::string& ioErrMsg)
		: m_ErrMsg(ioErrMsg)
	{
	}
	virtual ~ARBErrorCallback() {}

	/**
	 * Error message that requires a user response.
	 * @param pMsg Message to display to user.
	 * @return True to continue, false to abort.
	 */
	virtual bool OnError(char const* const pMsg)
	{
		return false;
	}

	/**
	 * Log an error message.
	 */
	virtual void LogMessage(std::string const& inMsg)
	{
		m_ErrMsg += inMsg;
	}

protected:
	std::string& m_ErrMsg;
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
		std::string const& inPgmVer,
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
	size_t GetAllClubNames(std::set<std::string>& outClubs, bool bInfo = true) const;

	/**
	 * Get all trial locations in use from existing trials and calendar entries.
	 * @param outLocations List of locations.
	 * @param bInfo Include locations from the ARBInfo.
	 * @return Number of locations.
	 */
	size_t GetAllTrialLocations(std::set<std::string>& outLocations, bool bInfo = true) const;

	/**
	 * Get all the subnames in use for the given event.
	 * @param inVenue Venue to find event in.
	 * @param inEvent Event info to match.
	 * @param outNames List of Sub-Names in use.
	 * @return Number of subnames.
	 */
	size_t GetAllEventSubNames(std::string const& inVenue, ARBConfigEvent const* inEvent, std::set<std::string>& outNames) const;

	/**
	 * Get all heights in use from existing runs.
	 * @param outHeights List of heights.
	 * @return Number of heights.
	 */
	size_t GetAllHeights(std::set<std::string>& outHeights) const;

	/**
	 * Get all judges in use from existing runs and Judges information.
	 * @param outJudges List of judges.
	 * @param bInfo Include judges from the ARBInfo.
	 * @return Number of judges.
	 */
	size_t GetAllJudges(std::set<std::string>& outJudges, bool bInfo = true) const;

	/**
	 * Get all handlers in use from existing runs.
	 * @param outHandlers List of handlers.
	 * @return Number of handlers.
	 */
	size_t GetAllHandlers(std::set<std::string>& outHandlers) const;

	/**
	 * Get all fault types in use from existing runs and configuration.
	 * @param outFaults List of faults.
	 * @return Number of faults.
	 */
	size_t GetAllFaultTypes(std::set<std::string>& outFaults) const;

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
extern std::string ErrorInvalidDocStructure(char const* const inMsg);

/**
 * Return an error message about a missing required attribute.
 *  
 * @param inElement Element containing missing attribute.
 * @param inAttrib Attribute name that is missing.
 * @param inMsg Additional error information.
 * @return Message with newline.
 */
extern std::string ErrorMissingAttribute(
	char const* const inElement,
	char const* const inAttrib,
	char const* const inMsg = NULL);

/**
 * Return an error message about an invalid value in an attribute.
 *
 * @param inElement Element containing bad attribute.
 * @param inAttrib Attribute name whose value is bad.
 * @param inMsg Additional error information.
 * @return Message with newline.
 */
extern std::string ErrorInvalidAttributeValue(
	char const* const inElement,
	char const* const inAttrib,
	char const* const inMsg = NULL);
