#pragma once

/*
 * Copyright � 2002-2007 David Connet. All Rights Reserved.
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
 * @brief Defines for elements and attributes from the DTD
 * @author David Connet
 *
 * Revision History
 * @li 2007-08-14 DRC Separated from ARBAgilityRecordBook.h
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
#define TREE_TITLES							_T("Titles")
#define TREE_MULTIQ							_T("MultiQ")
#define TREE_MULTIQ_ITEM					_T("MultiQItem")
#define TREE_DIVISION						_T("Division")
#define TREE_LEVEL							_T("Level")
#define TREE_SUBLEVEL						_T("SubLevel")
#define TREE_EVENT							_T("Event")
#define TREE_EVENT_DESC						_T("Desc")
#define TREE_EVENT_SUBNAME					_T("SubName")
#define TREE_SCORING						_T("Scoring")
#define TREE_PLACE_INFO						_T("PlaceInfo")
#define TREE_TITLE_POINTS					_T("TitlePoints")
#define TREE_LIFETIME_POINTS				_T("LifeTime")
#define TREE_PLACEMENTS						_T("Placements")
#define TREE_CALSITE						_T("CalSite")
#define TREE_LOCCODE						_T("LocCode")
#define TREE_VENUECODE						_T("VenueCode")
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
#define TREE_CRCD_META2						_T("CRCDMeta2")
#define TREE_OTHER							_T("Other")
#define TREE_RUN_LINK						_T("Link")
#define TREE_REF_RUN						_T("ReferenceRun")
#define TREE_REF_NAME						_T("Name")
#define TREE_REF_BREED						_T("Breed")
#define TREE_REF_SCORE						_T("ScoreOrFaults")
#define TREE_REF_NOTE						_T("Note")
#define ATTRIB_BOOK_VERSION					_T("Book")
#define ATTRIB_BOOK_PGM_VERSION				_T("ver")
#define ATTRIB_BOOK_TIMESTAMP				_T("timestamp")
#define ATTRIB_CONFIG_VERSION				_T("version")
#define ATTRIB_CONFIG_UPDATE				_T("update")
#define ATTRIB_CAL_START					_T("DateStart")
#define ATTRIB_CAL_END						_T("DateEnd")
#define ATTRIB_CAL_OPENING					_T("DateOpening")
#define ATTRIB_CAL_DRAW						_T("DateDraw")
#define ATTRIB_CAL_CLOSING					_T("DateClosing")
#define ATTRIB_CAL_MAYBE					_T("isTentative")
#define ATTRIB_CAL_LOCATION					_T("Location")
#define ATTRIB_CAL_CLUB						_T("Club")
#define ATTRIB_CAL_VENUE					_T("Venue")
#define ATTRIB_CAL_ENTERED					_T("Entered")
#define ATTRIB_CAL_ACCOMMODATION			_T("Acc")
#define ATTRIB_CAL_CONFIRMATION				_T("Confirm")
#define ATTRIB_CAL_SECEMAIL					_T("SecEmail")
#define ATTRIB_CAL_PREMIUMURL				_T("PremiumURL")
#define ATTRIB_CAL_ONLINEURL				_T("OnlineURL")
#define ATTRIB_CAL_NOTE						_T("Note")
#define ATTRIB_TRAINING_DATE				_T("Date")
#define ATTRIB_TRAINING_NAME				_T("Name")
#define ATTRIB_TRAINING_SUBNAME				_T("SubName")
#define ATTRIB_ACTION_VERB					_T("Verb")
	#define ACTION_VERB_DELETE_TITLE			_T("DeleteTitle")
	#define ACTION_VERB_RENAME_TITLE			_T("RenameTitle")
	#define ACTION_VERB_DELETE_EVENT			_T("DeleteEvent")
	#define ACTION_VERB_RENAME_EVENT			_T("RenameEvent")
	#define ACTION_VERB_RENAME_DIV				_T("RenameDivision")
#define ATTRIB_ACTION_VENUE					_T("Venue")
#define ATTRIB_ACTION_DIVISION				_T("Div")
#define ATTRIB_ACTION_OLDNAME				_T("OldName")
#define ATTRIB_ACTION_NEWNAME				_T("NewName")
#define ATTRIB_VENUE_NAME					_T("Name")
#define ATTRIB_VENUE_LONGNAME				_T("LongName")
#define ATTRIB_VENUE_URL					_T("URL")
#define ATTRIB_VENUE_ICON					_T("icon")
#define ATTRIB_MULTIQ_NAME					_T("Name")
#define ATTRIB_MULTIQ_SHORTNAME				_T("SName")
#define ATTRIB_MULTIQ_VALID_FROM			_T("ValidFrom")
#define ATTRIB_MULTIQ_VALID_TO				_T("ValidTo")
#define ATTRIB_MULTIQ_ITEM_DIV				_T("Div")
#define ATTRIB_MULTIQ_ITEM_LEVEL			_T("Level")
#define ATTRIB_MULTIQ_ITEM_EVENT			_T("Event")
#define ATTRIB_CALSITE_SEARCH				_T("search")
#define ATTRIB_CALSITE_HELP					_T("help")
#define ATTRIB_LOCCODE_CODE					_T("code")
#define ATTRIB_LOCCODE_NAME					_T("name")
#define ATTRIB_VENUECODE_CODE				_T("code")
#define ATTRIB_VENUECODE_VENUE				_T("venue")
#define ATTRIB_OTHERPTS_NAME				_T("Name")
#define ATTRIB_OTHERPTS_COUNT				_T("Count")
	#define OTHERPTS_COUNT_ALL					_T("All")
	#define OTHERPTS_COUNT_ALLBYEVENT			_T("AllByEvent")
	#define OTHERPTS_COUNT_LEVEL				_T("Level")
	#define OTHERPTS_COUNT_LEVELBYEVENT			_T("LevelByEvent")
#define ATTRIB_OTHERPTS_DEFAULT				_T("defValue")
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
	#define SCORING_TYPE_FT						_T("FaultsThenTime")
	#define SCORING_TYPE_FT100					_T("Faults100ThenTime")
	#define SCORING_TYPE_FT200					_T("Faults200ThenTime")
	#define SCORING_TYPE_OCT					_T("OCScoreThenTime")
	#define SCORING_TYPE_ST						_T("ScoreThenTime")
	#define SCORING_TYPE_TF						_T("TimePlusFaults")
#define ATTRIB_SCORING_DROPFRACTIONS		_T("dropFractions")
#define ATTRIB_SCORING_TIMEFAULTS_CLEAN_Q	_T("cleanQ")
#define ATTRIB_SCORING_TIMEFAULTS_UNDER		_T("underTF")
#define ATTRIB_SCORING_TIMEFAULTS_OVER		_T("overTF")
#define ATTRIB_SCORING_SUBTRACT_TIMEFAULTS	_T("subtractTF")
#define ATTRIB_SCORING_OPENINGPTS			_T("OpeningPts")
#define ATTRIB_SCORING_CLOSINGPTS			_T("ClosingPts")
#define ATTRIB_SCORING_SUPERQ				_T("superQ")
#define ATTRIB_SCORING_SPEEDPTS				_T("speedPts")
#define ATTRIB_SCORING_BONUSPTS				_T("bonusPts")
#define ATTRIB_PLACE_INFO_PLACE				_T("Place")
#define ATTRIB_PLACE_INFO_VALUE				_T("Value")
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
#define ATTRIB_TITLE_INSTANCE_SHOW			_T("show")
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
