#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
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
#define TREE_BOOK							wxT("AgilityBook")
#define TREE_CALENDAR						wxT("Calendar")
#define TREE_TRAINING						wxT("Training")
#define TREE_CONFIG							wxT("Configuration")
#define TREE_ACTION							wxT("Action")
#define TREE_VENUE							wxT("Venue")
#define TREE_VENUE_DESC						wxT("Desc")
#define TREE_TITLES							wxT("Titles")
#define TREE_MULTIQ							wxT("MultiQ")
#define TREE_MULTIQ_ITEM					wxT("MultiQItem")
#define TREE_DIVISION						wxT("Division")
#define TREE_LEVEL							wxT("Level")
#define TREE_SUBLEVEL						wxT("SubLevel")
#define TREE_EVENT							wxT("Event")
#define TREE_EVENT_DESC						wxT("Desc")
#define TREE_EVENT_SUBNAME					wxT("SubName")
#define TREE_SCORING						wxT("Scoring")
#define TREE_PLACE_INFO						wxT("PlaceInfo")
#define TREE_TITLE_POINTS					wxT("TitlePoints")
#define TREE_LIFETIME_POINTS				wxT("LifeTime")
#define TREE_PLACEMENTS						wxT("Placements")
#define TREE_CALSITE						wxT("CalSite")
#define TREE_CALSITE_DESC					wxT("Desc")
#define TREE_LOCCODE						wxT("LocCode")
#define TREE_VENUECODE						wxT("VenueCode")
#define TREE_FAULTTYPE						wxT("FaultType")
#define TREE_OTHERPTS						wxT("OtherPts")
#define TREE_INFO							wxT("Info")
#define TREE_CLUBINFO						wxT("ClubInfo")
#define TREE_JUDGEINFO						wxT("JudgeInfo")
#define TREE_LOCATIONINFO					wxT("LocationInfo")
#define TREE_DOG							wxT("Dog")
#define TREE_REGNAME						wxT("RegisteredName")
#define TREE_BREED							wxT("Breed")
#define TREE_NOTE							wxT("Note")
#define TREE_EXISTING_PTS					wxT("ExistingPoints")
#define TREE_REG_NUM						wxT("RegNum")
#define TREE_TITLE							wxT("Title")
#define TREE_TRIAL							wxT("Trial")
#define TREE_LOCATION						wxT("Location")
#define TREE_CLUB							wxT("Club")
#define TREE_RUN							wxT("Run")
#define TREE_CONDITIONS						wxT("Conditions")
#define TREE_JUDGE							wxT("Judge")
#define TREE_HANDLER						wxT("Handler")
#define TREE_PARTNER						wxT("Partner")
#define TREE_BY_TIME						wxT("ByTime")
#define TREE_BY_OPENCLOSE					wxT("ByOpenClose")
#define TREE_BY_POINTS						wxT("ByPoints")
#define TREE_PLACEMENT						wxT("Placement")
#define TREE_PLACEMENT_OTHERPOINTS			wxT("OtherPoints")
#define TREE_NOTES							wxT("Notes")
#define TREE_FAULTS							wxT("Faults")
#define TREE_CRCD							wxT("CRCD")
#define TREE_CRCD_META						wxT("CRCDMeta")
#define TREE_CRCD_META2						wxT("CRCDMeta2")
#define TREE_OTHER							wxT("Other")
#define TREE_RUN_LINK						wxT("Link")
#define TREE_REF_RUN						wxT("ReferenceRun")
#define TREE_REF_NAME						wxT("Name")
#define TREE_REF_BREED						wxT("Breed")
#define TREE_REF_SCORE						wxT("ScoreOrFaults")
#define TREE_REF_NOTE						wxT("Note")
#define ATTRIB_BOOK_VERSION					wxT("Book")
#define ATTRIB_BOOK_PGM_VERSION				wxT("ver")
#define ATTRIB_BOOK_TIMESTAMP				wxT("timestamp")
#define ATTRIB_CONFIG_VERSION				wxT("version")
#define ATTRIB_CONFIG_UPDATE				wxT("update")
#define ATTRIB_CAL_START					wxT("DateStart")
#define ATTRIB_CAL_END						wxT("DateEnd")
#define ATTRIB_CAL_OPENING					wxT("DateOpening")
#define ATTRIB_CAL_DRAW						wxT("DateDraw")
#define ATTRIB_CAL_CLOSING					wxT("DateClosing")
#define ATTRIB_CAL_MAYBE					wxT("isTentative")
#define ATTRIB_CAL_LOCATION					wxT("Location")
#define ATTRIB_CAL_CLUB						wxT("Club")
#define ATTRIB_CAL_VENUE					wxT("Venue")
#define ATTRIB_CAL_ENTERED					wxT("Entered")
#define ATTRIB_CAL_ACCOMMODATION			wxT("Acc")
#define ATTRIB_CAL_CONFIRMATION				wxT("Confirm")
#define ATTRIB_CAL_SECEMAIL					wxT("SecEmail")
#define ATTRIB_CAL_PREMIUMURL				wxT("PremiumURL")
#define ATTRIB_CAL_ONLINEURL				wxT("OnlineURL")
#define ATTRIB_CAL_NOTE						wxT("Note")
#define ATTRIB_TRAINING_DATE				wxT("Date")
#define ATTRIB_TRAINING_NAME				wxT("Name")
#define ATTRIB_TRAINING_SUBNAME				wxT("SubName")
#define ATTRIB_ACTION_VERB					wxT("Verb")
#define ATTRIB_ACTION_VENUE					wxT("Venue")
#define ATTRIB_ACTION_DIVISION				wxT("Div")
#define ATTRIB_ACTION_OLDNAME				wxT("OldName")
#define ATTRIB_ACTION_NEWNAME				wxT("NewName")
#define ATTRIB_VENUE_NAME					wxT("Name")
#define ATTRIB_VENUE_LONGNAME				wxT("LongName")
#define ATTRIB_VENUE_URL					wxT("URL")
#define ATTRIB_VENUE_ICON					wxT("icon")
#define ATTRIB_VENUE_LIFETIME_NAME			wxT("LifetimeName")
#define ATTRIB_MULTIQ_NAME					wxT("Name")
#define ATTRIB_MULTIQ_SHORTNAME				wxT("SName")
#define ATTRIB_MULTIQ_VALID_FROM			wxT("ValidFrom")
#define ATTRIB_MULTIQ_VALID_TO				wxT("ValidTo")
#define ATTRIB_MULTIQ_ITEM_DIV				wxT("Div")
#define ATTRIB_MULTIQ_ITEM_LEVEL			wxT("Level")
#define ATTRIB_MULTIQ_ITEM_EVENT			wxT("Event")
#define ATTRIB_CALSITE_NAME					wxT("name")
#define ATTRIB_CALSITE_SEARCH				wxT("search")
#define ATTRIB_CALSITE_HELP					wxT("help")
#define ATTRIB_LOCCODE_CODE					wxT("code")
#define ATTRIB_LOCCODE_NAME					wxT("name")
#define ATTRIB_VENUECODE_CODE				wxT("code")
#define ATTRIB_VENUECODE_VENUE				wxT("venue")
#define ATTRIB_OTHERPTS_NAME				wxT("Name")
#define ATTRIB_OTHERPTS_COUNT				wxT("Count")
#define ATTRIB_OTHERPTS_DEFAULT				wxT("defValue")
#define ATTRIB_DIVISION_NAME				wxT("Name")
#define ATTRIB_LEVEL_NAME					wxT("Name")
#define ATTRIB_TITLES_NAME					wxT("Name")
#define ATTRIB_TITLES_LONGNAME				wxT("LongName")
#define ATTRIB_TITLES_MULTIPLE				wxT("Multiple")
#define ATTRIB_TITLES_MULTIPLE_STYLE		wxT("Style")
#define ATTRIB_TITLES_PREFIX				wxT("Prefix")
#define ATTRIB_TITLES_VALIDFROM				wxT("ValidFrom")
#define ATTRIB_TITLES_VALIDTO				wxT("ValidTo")
#define ATTRIB_SUBLEVEL_NAME				wxT("Name")
#define ATTRIB_EVENT_NAME					wxT("Name")
#define ATTRIB_EVENT_HAS_TABLE				wxT("hasTable")
#define ATTRIB_EVENT_HASPARTNER				wxT("hasPartner")
#define ATTRIB_EVENT_HASSUBNAMES			wxT("hasSubNames")
#define ATTRIB_SCORING_VALIDFROM			wxT("ValidFrom")
#define ATTRIB_SCORING_VALIDTO				wxT("ValidTo")
#define ATTRIB_SCORING_DIVISION				wxT("Division")
#define ATTRIB_SCORING_LEVEL				wxT("Level")
#define ATTRIB_SCORING_TYPE					wxT("type")
#define ATTRIB_SCORING_DROPFRACTIONS		wxT("dropFractions")
#define ATTRIB_SCORING_TIMEFAULTS_CLEAN_Q	wxT("cleanQ")
#define ATTRIB_SCORING_TIMEFAULTS_UNDER		wxT("underTF")
#define ATTRIB_SCORING_TIMEFAULTS_OVER		wxT("overTF")
#define ATTRIB_SCORING_SUBTRACT_TIMEFAULTS	wxT("subtractTF")
#define ATTRIB_SCORING_OPENINGPTS			wxT("OpeningPts")
#define ATTRIB_SCORING_CLOSINGPTS			wxT("ClosingPts")
#define ATTRIB_SCORING_SUPERQ				wxT("superQ")
#define ATTRIB_SCORING_SPEEDPTS				wxT("speedPts")
#define ATTRIB_SCORING_BONUSPTS				wxT("bonusPts")
#define ATTRIB_SCORING_OBSTACLES			wxT("obstacles")
#define ATTRIB_PLACE_INFO_PLACE				wxT("Place")
#define ATTRIB_PLACE_INFO_VALUE				wxT("Value")
#define ATTRIB_PLACE_INFO_MUSTQ				wxT("MustQ")
#define ATTRIB_TITLE_POINTS_POINTS			wxT("Points")
#define ATTRIB_TITLE_POINTS_FAULTS			wxT("Faults")
#define ATTRIB_LIFETIME_POINTS_POINTS		wxT("Points")
#define ATTRIB_LIFETIME_POINTS_FAULTS		wxT("Faults")
#define ATTRIB_INFO_NAME					wxT("Name")
#define ATTRIB_INFO_VISIBLE					wxT("Visible")
#define ATTRIB_DOG_CALLNAME					wxT("CallName")
#define ATTRIB_DOG_DOB						wxT("DOB")
#define ATTRIB_DOG_DECEASED					wxT("Deceased")
#define ATTRIB_EXISTING_PTS_DATE			wxT("Date")
#define ATTRIB_EXISTING_PTS_TYPE			wxT("Type")
#define ATTRIB_EXISTING_PTS_OTHER			wxT("Other")
#define ATTRIB_EXISTING_PTS_VENUE			wxT("Venue")
#define ATTRIB_EXISTING_PTS_MULTIQ			wxT("MultiQ")
#define ATTRIB_EXISTING_PTS_DIV				wxT("Div")
#define ATTRIB_EXISTING_PTS_LEVEL			wxT("Level")
#define ATTRIB_EXISTING_PTS_EVENT			wxT("Event")
#define ATTRIB_EXISTING_PTS_SUBNAME			wxT("SubName")
#define ATTRIB_EXISTING_PTS_POINTS			wxT("Pts")
#define ATTRIB_REG_NUM_VENUE				wxT("Venue")
#define ATTRIB_REG_NUM_NUMBER				wxT("Number")
#define ATTRIB_REG_NUM_HEIGHT				wxT("Height")
#define ATTRIB_REG_NUM_RECEIVED				wxT("isReceived")
#define ATTRIB_TITLE_VENUE					wxT("Venue")
#define ATTRIB_TITLE_NAME					wxT("Name")
#define ATTRIB_TITLE_DATE					wxT("Date")
#define ATTRIB_TITLE_INSTANCE_SHOW			wxT("show")
#define ATTRIB_TITLE_INSTANCE				wxT("instance")
#define ATTRIB_TITLE_STYLE					wxT("style")
#define ATTRIB_TITLE_RECEIVED				wxT("isReceived")
#define ATTRIB_TITLE_HIDDEN					wxT("isHidden")
#define ATTRIB_TRIAL_VERIFIED				wxT("Verified")
#define ATTRIB_CLUB_VENUE					wxT("Venue")
#define ATTRIB_RUN_DATE						wxT("Date")
#define ATTRIB_RUN_DIVISION					wxT("Division")
#define ATTRIB_RUN_LEVEL					wxT("Level")
#define ATTRIB_RUN_HEIGHT					wxT("Height")
#define ATTRIB_RUN_EVENT					wxT("Event")
#define ATTRIB_RUN_SUBNAME					wxT("SubName")
#define ATTRIB_PARTNER_HANDLER				wxT("Handler")
#define ATTRIB_PARTNER_DOG					wxT("Dog")
#define ATTRIB_PARTNER_REGNUM				wxT("RegNum")
#define ATTRIB_SCORING_FAULTS				wxT("CourseFaults")
#define ATTRIB_SCORING_TIME					wxT("Time")
#define ATTRIB_SCORING_HAS_TABLE			wxT("hasTable")
#define ATTRIB_SCORING_TF_MULTIPLIER		wxT("timeFault")
#define ATTRIB_SCORING_SCT					wxT("SCT")
#define ATTRIB_SCORING_SCT2					wxT("SCT2")
#define ATTRIB_BY_TIME_YARDS				wxT("Yards")
#define ATTRIB_BY_OPENCLOSE_NEEDOPEN		wxT("NeedOpenPts")
#define ATTRIB_BY_OPENCLOSE_NEEDCLOSE		wxT("NeedClosePts")
#define ATTRIB_BY_OPENCLOSE_GOTOPEN			wxT("OpenPts")
#define ATTRIB_BY_OPENCLOSE_GOTCLOSE		wxT("ClosePts")
#define ATTRIB_BY_POINTS_NEED				wxT("NeedPts")
#define ATTRIB_BY_POINTS_GOT				wxT("Points")
#define ATTRIB_PLACEMENT_Q					wxT("Q")
#define ATTRIB_QTYPE_NA							wxT("NA")
#define ATTRIB_QTYPE_Q							wxT("Q")
#define ATTRIB_QTYPE_NQ							wxT("NQ")
#define ATTRIB_QTYPE_E							wxT("E")
#define ATTRIB_QTYPE_DNR						wxT("DNR")
#define ATTRIB_QTYPE_SQ							wxT("SQ")
#define ATTRIB_PLACEMENT_PLACE				wxT("Place")
#define ATTRIB_PLACEMENT_INCLASS			wxT("InClass")
#define ATTRIB_PLACEMENT_DOGSQD				wxT("DogsQd")
#define ATTRIB_PLACEMENT_OTHERPOINTS_NAME	wxT("Name")
#define ATTRIB_PLACEMENT_OTHERPOINTS_POINTS	wxT("Points")
#define ATTRIB_REF_RUN_Q					wxT("Q")
#define ATTRIB_REF_RUN_PLACE				wxT("Place")
#define ATTRIB_REF_RUN_TIME					wxT("Time")
#define ATTRIB_REF_RUN_HEIGHT				wxT("Height")

/*
 * These defines list any special values.
 */
#define WILDCARD_DIVISION		wxT("*")
#define WILDCARD_LEVEL			wxT("*")
