#pragma once

/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 *
 * @brief The main data class.
 * @author David Connet
 *
 * Revision History
 * @li 2003-10-13 DRC Made Time/CourseFaults common for all types of scoring.
 */

/**
 * These defines make up the structure of the xml.
 * Make sure these agree with the .dtd.
 */
#define TREE_BOOK							"AgilityBook"
#define TREE_CALENDAR						"Calendar"
#define TREE_TRAINING						"Training"
#define TREE_CONFIG							"Configuration"
#define TREE_VENUE							"Venue"
#define TREE_VENUE_DESC						"Desc"
#define TREE_DIVISION						"Division"
#define TREE_LEVEL							"Level"
#define TREE_TITLES							"Titles"
#define TREE_SUBLEVEL						"SubLevel"
#define TREE_EVENT							"Event"
#define TREE_EVENT_DESC						"Desc"
#define TREE_SCORING						"Scoring"
#define TREE_TITLE_POINTS					"TitlePoints"
#define TREE_FAULTTYPE						"FaultType"
#define TREE_OTHERPTS						"OtherPts"
#define TREE_DOG							"Dog"
#define TREE_REGNAME						"RegisteredName"
#define TREE_BREED							"Breed"
#define TREE_NOTE							"Note"
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
#define TREE_OTHER							"Other"
#define TREE_REF_RUN						"ReferenceRun"
#define TREE_REF_NAME						"Name"
#define TREE_REF_BREED						"Breed"
#define TREE_REF_SCORE						"ScoreOrFaults"
#define TREE_REF_NOTE						"Note"
#define ATTRIB_BOOK_VERSION					"Book"
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
#define ATTRIB_VENUE_NAME					"Name"
#define ATTRIB_OTHERPTS_NAME				"Name"
#define ATTRIB_OTHERPTS_COUNT				"Count"
#define ATTRIB_DIVISION_NAME				"Name"
#define ATTRIB_LEVEL_NAME					"Name"
#define ATTRIB_TITLES_NAME					"Name"
#define ATTRIB_SUBLEVEL_NAME				"Name"
#define ATTRIB_EVENT_NAME					"Name"
#define ATTRIB_EVENT_HASPARTNER				"hasPartner"
#define ATTRIB_SCORING_DIVISION				"Division"
#define ATTRIB_SCORING_LEVEL				"Level"
#define ATTRIB_SCORING_TYPE					"type"
#define ATTRIB_SCORING_DROPFRACTIONS		"dropFractions"
#define ATTRIB_SCORING_OPENINGPTS			"OpeningPts"
#define ATTRIB_SCORING_CLOSINGPTS			"ClosingPts"
#define ATTRIB_SCORING_SUPERQ				"superQ"
#define ATTRIB_SCORING_DOUBLEQ				"doubleQ"
#define ATTRIB_SCORING_MACHPTS				"machPts"
#define ATTRIB_TITLE_POINTS_POINTS			"Points"
#define ATTRIB_TITLE_POINTS_FAULTS			"Faults"
#define ATTRIB_DOG_CALLNAME					"CallName"
#define ATTRIB_DOG_DOB						"DOB"
#define ATTRIB_DOG_DECEASED					"Deceased"
#define ATTRIB_REG_NUM_VENUE				"Venue"
#define ATTRIB_REG_NUM_HEIGHT				"Height"
#define ATTRIB_REG_NUM_RECEIVED				"isReceived"
#define ATTRIB_TITLE_VENUE					"Venue"
#define ATTRIB_TITLE_NAME					"Name"
#define ATTRIB_TITLE_DATE					"Date"
#define ATTRIB_TITLE_RECEIVED				"isReceived"
#define ATTRIB_TRIAL_VERIFIED				"Verified"
#define ATTRIB_CLUB_VENUE					"Venue"
#define ATTRIB_RUN_DATE						"Date"
#define ATTRIB_RUN_DIVISION					"Division"
#define ATTRIB_RUN_LEVEL					"Level"
#define ATTRIB_RUN_HEIGHT					"Height"
#define ATTRIB_RUN_EVENT					"Event"
#define ATTRIB_PARTNER_HANDLER				"Handler"
#define ATTRIB_PARTNER_DOG					"Dog"
#define ATTRIB_PARTNER_REGNUM				"RegNum"
#define ATTRIB_SCORING_FAULTS				"CourseFaults"
#define ATTRIB_SCORING_TIME					"Time"
#define ATTRIB_BY_TIME_SCT					"SCT"
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

/**
 * These defines list any special values.
 */
#define WILDCARD_DIVISION		"*"
#define WILDCARD_LEVEL			"*"

/**
 * Localization stuff
 *  There is some stuff in Element.cpp (errors due to XML parsing failures)
 */
#define UNKNOWN_VERSION			"Unknown document version"
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
#define INVALID_EVENT			"Unable to locate an event description in any of the venues of the affiliated clubs: "
#define INVALID_TITLE			"Title is not defined in the Venue description: "
#define VALID_VALUES			"Valid values: "
#define VALID_VALUES_BOOL		"Valid values: 'y', 'n'"
#define VALID_VALUES_ENTRY		"Valid values: 'E', 'P', 'N'"
#define VALID_VALUES_OTHERPT	"Valid values: All, AllByEvent, Level, LevelByEvent"
#define VALID_VALUES_SCORE		"Valid values: 'FaultsThenTime', 'Faults100ThenTime', 'Faults200ThenTime', 'OCScoreThenTime', 'ScoreThenTime', 'TimePlusFaults'"
#define TITLE_POINTS_NAME_FORMAT	"%hd points with %hd faults"
#define SCORE_STYLE_UNKNOWN			"Unknown"
#define SCORE_STYLE_FAULTSTIME		"Faults Then Time"
#define SCORE_STYLE_FAULTS100TIME	"100 Minus Faults Then Time"
#define SCORE_STYLE_FAULTS200TIME	"200 Minus Faults Then Time"
#define SCORE_STYLE_OCSCORETIME		"Opening/Closing Points Then Time"
#define SCORE_STYLE_SCORETIME		"Points Then Time"
#define SCORE_STYLE_TIMEPLUSFAULTS	"Time Plus Faults"
// Strings for formatting the information returned when updating configurations.
#define UPDATE_FORMAT_FAULTS	"Faults: %d added, %d identical\n"
#define UPDATE_FORMAT_OTHERPTS	"Other Points: %d added, %d updated, %d identical\n"
#define UPDATE_FORMAT_VENUES	"Venues: %d added, %d updated, %d identical"
#define UPDATE_FORMAT_DIVISIONS	"Divisions: %d added, %d updated, %d identical\n"
#define UPDATE_FORMAT_EVENTS	"Events: %d added, %d updated, %d identical\n"
#define UPDATE_FORMAT_LEVELS	"Levels: %d added, %d updated, %d identical\n"
#define UPDATE_FORMAT_TITLES	"Titles: %d added, %d updated, %d identical\n"
#define UPDATE_FORMAT_SUBLEVELS	"%d new sub-levels\n"
#define UPDATE_FORMAT_RULES		" rules: %d added, %d deleted, %d updated, %d identical\n"


#include <list>
#include <set>
#include "ARBCalendar.h"
#include "ARBConfig.h"
#include "ARBDog.h"
#include "ARBTraining.h"
class ARBConfigOtherPoints;
class CElement;

class ARBAgilityRecordBook
{
public:
	/**
	 * Return the current document version. This helps make sure an older
	 * version of the program doesn't read a newer version of the file.
	 */
	static short GetCurrentDocVersion();

	ARBAgilityRecordBook();
	~ARBAgilityRecordBook();
	void clear();

	/**
	 * Load a document. See CElement.h for more information on why we use it.
	 * The individual load flags allow us to load just a portion of a document.
	 * @pre If bDogs is true, bConfig must also be true or dogs won't load.
	 * @param inTree XML structure to convert into ARB.
	 * @param inCalendar Load calendar info.
	 * @param inTraining Load training info.
	 * @param inConfig Load config info.
	 * @param inDogs Load dog info.
	 * @return Success
	 */
	bool Load(
		const CElement& inTree,
		bool inCalendar,
		bool inTraining,
		bool inConfig,
		bool inDogs);
	bool Load(const CElement& inTree)
	{
		return Load(inTree, true, true, true, true);
	}
	/**
	 * Save a document.
	 * @param outTree XML structure to write ARB to.
	 */
	bool Save(CElement& outTree) const;

	/**
	 * Create a default document: No dogs, default configuration.
	 */
	void Default();

	// Convenience functions that do some data accumulation.
	/**
	 * Get all club names in use from existing trials.
	 */
	size_t GetAllClubNames(std::set<std::string>& outClubs) const;
	/**
	 * Get all trial locations in use from existing trials.
	 */
	size_t GetAllTrialLocations(std::set<std::string>& outLocations) const;
	/**
	 * Get all heights in use from existing runs.
	 */
	size_t GetAllHeights(std::set<std::string>& outHeights) const;
	/**
	 * Get all judges in use from existing runs.
	 */
	size_t GetAllJudges(std::set<std::string>& outJudges) const;
	/**
	 * Get all handlers in use from existing runs.
	 */
	size_t GetAllHandlers(std::set<std::string>& outHandlers) const;
	/**
	 * Get all fault types in use from existing runs and configuration.
	 */
	size_t GetAllFaultTypes(std::set<std::string>& outFaults) const;

	const ARBCalendarList& GetCalendar() const;
	ARBCalendarList& GetCalendar();
	const ARBTrainingList& GetTraining() const;
	ARBTrainingList& GetTraining();
	const ARBConfig& GetConfig() const;
	ARBConfig& GetConfig();
	const ARBDogList& GetDogs() const;
	ARBDogList& GetDogs();

// Intentionally not implemented!
private:
	ARBAgilityRecordBook(const ARBAgilityRecordBook&);
	ARBAgilityRecordBook& operator=(const ARBAgilityRecordBook&);

private:
	ARBCalendarList m_Calendar;
	ARBTrainingList m_Training;
	ARBConfig m_Config;
	ARBDogList m_Dogs;
};

inline const ARBCalendarList& ARBAgilityRecordBook::GetCalendar() const
{
	return m_Calendar;
}

inline ARBCalendarList& ARBAgilityRecordBook::GetCalendar()
{
	return m_Calendar;
}

inline const ARBTrainingList& ARBAgilityRecordBook::GetTraining() const
{
	return m_Training;
}

inline ARBTrainingList& ARBAgilityRecordBook::GetTraining()
{
	return m_Training;
}

inline const ARBConfig& ARBAgilityRecordBook::GetConfig() const
{
	return m_Config;
}

inline ARBConfig& ARBAgilityRecordBook::GetConfig()
{
	return m_Config;
}

inline const ARBDogList& ARBAgilityRecordBook::GetDogs() const
{
	return m_Dogs;
}

inline ARBDogList& ARBAgilityRecordBook::GetDogs()
{
	return m_Dogs;
}

/////////////////////////////////////////////////////////////////////////////

//
// These error routines centralize platform dependent code.
// On windows, this prints using AfxMessageBox.
//

/**
 * Print an error message about invalid document structure.
 * This prints "Invalid document structure: " followed by a message.
 *
 * @param inMsg Additional error information.
 * @return None
 */
extern void ErrorInvalidDocStructure(const char* const inMsg);

/**
 * Print an error message about a missing required attribute.
 *
 * @param inElement Element containing missing attribute.
 * @param inAttrib Attribute name that is missing.
 * @param inMsg Additional error information.
 * @return None
 */
extern void ErrorMissingAttribute(
	const char* const inElement,
	const char* const inAttrib,
	const char* const inMsg = NULL);

/**
 * Print an error message about an invalid value in an attribute.
 *
 * @param inElement Element containing bad attribute.
 * @param inAttrib Attribute name whose value is bad.
 * @param inMsg Additional error information.
 * @return None
 */
extern void ErrorInvalidAttributeValue(
	const char* const inElement,
	const char* const inAttrib,
	const char* const inMsg = NULL);
