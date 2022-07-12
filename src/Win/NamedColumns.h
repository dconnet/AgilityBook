#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CNamedColumns class
 * @author David Connet
 *
 * Revision History
 * 2020-04-24 Moved defines from DlgAssignColumns to here.
 * 2010-03-28 Created.
 */

#include "AgilityBookOptions.h"
#include <vector>


namespace dconSoft
{

/*
 * If types/columns are added/removed, do not change these numbers.
 * These numbers are stored in a users registry for the order.
 * If a column is removed, then the number must be reserved.
 * (types are stored as a key name "colN", the others are the value of the key)
 */
constexpr size_t IO_TYPE_RUNS_FAULTS_TIME = 0;
constexpr size_t IO_TYPE_RUNS_TIME_FAULTS = 1;
constexpr size_t IO_TYPE_RUNS_OPEN_CLOSE = 2;
constexpr size_t IO_TYPE_RUNS_POINTS = 3;
constexpr size_t IO_TYPE_CALENDAR = 4;
constexpr size_t IO_TYPE_TRAINING = 5;
constexpr size_t IO_TYPE_VIEW_TREE_DOG = 6; // Tree/Run and TreeList
constexpr size_t IO_TYPE_VIEW_TREE_TRIAL = 7;
constexpr size_t IO_TYPE_VIEW_TREE_RUN = 8;
constexpr size_t IO_TYPE_VIEW_RUNS_LIST = 9;
constexpr size_t IO_TYPE_VIEW_CALENDAR_LIST = 10;
constexpr size_t IO_TYPE_VIEW_TRAINING_LIST = 11;
constexpr size_t IO_TYPE_CALENDAR_APPT = 12;
constexpr size_t IO_TYPE_CALENDAR_TASK = 13;
constexpr size_t IO_TYPE_VIEW_CALENDAR = 14;
constexpr size_t IO_TYPE_MAX = 15;

constexpr size_t IO_RUNS_REG_NAME = 0;
constexpr size_t IO_RUNS_CALL_NAME = 1;
constexpr size_t IO_RUNS_DATE = 2;
constexpr size_t IO_RUNS_VENUE = 3;
constexpr size_t IO_RUNS_CLUB = 4;
constexpr size_t IO_RUNS_LOCATION = 5;
constexpr size_t IO_RUNS_TRIAL_NOTES = 6;
constexpr size_t IO_RUNS_DIVISION = 7;
constexpr size_t IO_RUNS_LEVEL = 8;
constexpr size_t IO_RUNS_EVENT = 9;
constexpr size_t IO_RUNS_HEIGHT = 10;
constexpr size_t IO_RUNS_JUDGE = 11;
constexpr size_t IO_RUNS_HANDLER = 12;
constexpr size_t IO_RUNS_CONDITIONS = 13;
constexpr size_t IO_RUNS_COURSE_FAULTS = 14;
constexpr size_t IO_RUNS_TIME = 15;
constexpr size_t IO_RUNS_YARDS = 16;
constexpr size_t IO_RUNS_YPS = 17;
constexpr size_t IO_RUNS_SCT = 18;
constexpr size_t IO_RUNS_TOTAL_FAULTS = 19;
constexpr size_t IO_RUNS_REQ_OPENING = 20;
constexpr size_t IO_RUNS_REQ_CLOSING = 21;
constexpr size_t IO_RUNS_OPENING = 22;
constexpr size_t IO_RUNS_CLOSING = 23;
constexpr size_t IO_RUNS_REQ_POINTS = 24;
constexpr size_t IO_RUNS_POINTS = 25;
constexpr size_t IO_RUNS_PLACE = 26;
constexpr size_t IO_RUNS_IN_CLASS = 27;
constexpr size_t IO_RUNS_DOGSQD = 28;
constexpr size_t IO_RUNS_Q = 29;
constexpr size_t IO_RUNS_SCORE = 30;
constexpr size_t IO_RUNS_TITLE_POINTS = 31;
constexpr size_t IO_RUNS_COMMENTS = 32;
constexpr size_t IO_RUNS_FAULTS = 33;
constexpr size_t IO_CAL_START_DATE = 34;
constexpr size_t IO_CAL_END_DATE = 35;
constexpr size_t IO_CAL_TENTATIVE = 36;
constexpr size_t IO_CAL_ENTERED = 37;
constexpr size_t IO_CAL_LOCATION = 38;
constexpr size_t IO_CAL_CLUB = 39;
constexpr size_t IO_CAL_VENUE = 40;
constexpr size_t IO_CAL_OPENS = 41;
constexpr size_t IO_CAL_CLOSES = 42;
constexpr size_t IO_CAL_NOTES = 43;
constexpr size_t IO_LOG_DATE = 44;
constexpr size_t IO_LOG_NAME = 45;
constexpr size_t IO_LOG_NOTES = 46;
constexpr size_t IO_TREE_DOG_REGNAME = 47;
constexpr size_t IO_TREE_DOG_CALLNAME = 48;
constexpr size_t IO_TREE_DOG_BREED = 49;
constexpr size_t IO_TREE_DOG_DOB = 50;
constexpr size_t IO_TREE_DOG_AGE = 51;
constexpr size_t IO_TREE_TRIAL_START = 52;
constexpr size_t IO_TREE_TRIAL_END = 53;
constexpr size_t IO_TREE_TRIAL_CLUB = 54;
constexpr size_t IO_TREE_TRIAL_VENUE = 55;
constexpr size_t IO_TREE_TRIAL_LOCATION = 56;
constexpr size_t IO_TREE_TRIAL_NOTES = 57;
constexpr size_t IO_TREE_RUN_DATE = 58;
constexpr size_t IO_TREE_RUN_Q = 59;
constexpr size_t IO_TREE_RUN_EVENT = 60;
constexpr size_t IO_TREE_RUN_DIVISION = 61;
constexpr size_t IO_TREE_RUN_LEVEL = 62;
constexpr size_t IO_TREE_RUN_HEIGHT = 63;
constexpr size_t IO_LOG_SUBNAME = 64;
constexpr size_t IO_TREE_TRIAL_VERIFIED = 65;
constexpr size_t IO_RUNS_SPEED = 66;
constexpr size_t IO_CAL_APPT_SUBJECT = 67;
constexpr size_t IO_CAL_APPT_START_DATE = 68;
constexpr size_t IO_CAL_APPT_START_TIME = 69;
constexpr size_t IO_CAL_APPT_END_DATE = 70;
constexpr size_t IO_CAL_APPT_END_TIME = 71;
constexpr size_t IO_CAL_APPT_ALLDAY = 72;
constexpr size_t IO_CAL_APPT_REMINDER = 73;
constexpr size_t IO_CAL_APPT_REMINDER_DATE = 74;
constexpr size_t IO_CAL_APPT_REMINDER_TIME = 75;
constexpr size_t IO_CAL_APPT_ORGANIZER = 76;
constexpr size_t IO_CAL_APPT_REQ_ATTENDEES = 77;
constexpr size_t IO_CAL_APPT_OPT_ATTENDEES = 78;
constexpr size_t IO_CAL_APPT_RESOURCES = 79;
constexpr size_t IO_CAL_APPT_BILLING = 80;
constexpr size_t IO_CAL_APPT_CATEGORIES = 81;
constexpr size_t IO_CAL_APPT_DESCRIPTION = 82;
constexpr size_t IO_CAL_APPT_LOCATION = 83;
constexpr size_t IO_CAL_APPT_MILEAGE = 84;
constexpr size_t IO_CAL_APPT_PRIORITY = 85;
constexpr size_t IO_CAL_APPT_PRIVATE = 86;
constexpr size_t IO_CAL_APPT_SENSITIVITY = 87;
constexpr size_t IO_CAL_APPT_SHOW_TIME_AS = 88;
constexpr size_t IO_CAL_TASK_SUBJECT = 89;
constexpr size_t IO_CAL_TASK_START_DATE = 90;
constexpr size_t IO_CAL_TASK_DUE_DATE = 91;
constexpr size_t IO_CAL_TASK_REMINDER = 92;
constexpr size_t IO_CAL_TASK_REMINDER_DATE = 93;
constexpr size_t IO_CAL_TASK_REMINDER_TIME = 94;
constexpr size_t IO_CAL_TASK_COMPLETED_DATE = 95;
constexpr size_t IO_CAL_TASK_COMPLETE = 96;
constexpr size_t IO_CAL_TASK_TOTAL_WORK = 97;
constexpr size_t IO_CAL_TASK_ACTUAL_WORK = 98;
constexpr size_t IO_CAL_TASK_BILLING = 99;
constexpr size_t IO_CAL_TASK_CATEGORIES = 100;
constexpr size_t IO_CAL_TASK_COMPANIES = 101;
constexpr size_t IO_CAL_TASK_CONTACTS = 102;
constexpr size_t IO_CAL_TASK_MILEAGE = 103;
constexpr size_t IO_CAL_TASK_NOTES = 104;
constexpr size_t IO_CAL_TASK_PRIORITY = 105;
constexpr size_t IO_CAL_TASK_PRIVATE = 106;
constexpr size_t IO_CAL_TASK_ROLE = 107;
constexpr size_t IO_CAL_TASK_SCH_PRIORITY = 108;
constexpr size_t IO_CAL_TASK_SENSITIVITY = 109;
constexpr size_t IO_CAL_TASK_STATUS = 110;
constexpr size_t IO_RUNS_MIN_YPS = 111;
constexpr size_t IO_CAL_DRAWS = 112;
constexpr size_t IO_RUNS_OBSTACLES = 113;
constexpr size_t IO_RUNS_OPS = 114;
constexpr size_t IO_RUNS_SUBNAME = 115;
constexpr size_t IO_MAX = 116;


class CNamedColumns
{
private:
	CAgilityBookOptions::ColumnOrder m_eOrder;
	std::vector<long> m_Columns[IO_TYPE_MAX];

	int m_numConfigs;
	std::wstring m_curConfig;
	struct CNamedColumnsData
	{
		std::wstring configName;
		std::vector<long> m_Columns[IO_TYPE_MAX];
	};
	std::vector<CNamedColumnsData> m_Configs;

	std::vector<CNamedColumnsData>::iterator FindConfig(std::wstring const& inName);

public:
	CNamedColumns(CAgilityBookOptions::ColumnOrder eOrder);
	void Save();

	CAgilityBookOptions::ColumnOrder Order() const
	{
		return m_eOrder;
	}
	std::wstring GetCurrentConfig() const
	{
		return m_curConfig;
	}
	std::vector<long> const& Columns(int idx) const
	{
		return m_Columns[idx];
	}

	size_t GetAllConfigNames(std::vector<std::wstring>& outNames) const;
	bool SetCurrentConfig(std::wstring const& inName);
	bool AddConfig(std::wstring const& inName); ///< Returns if a new one was added
	bool DeleteConfig(std::wstring const& inName);

	void ResetDefault(int idxType);
	void SetColumn(int idxType, std::vector<long> const& columns);
};

} // namespace dconSoft
