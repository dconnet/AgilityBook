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
 * @brief interface of the CAgilityBookOptions class
 * @author David Connet
 *
 * Revision History
 */

#include <string>
#include <vector>
#include "ARBDate.h"
class ARBDogRun;
class ARBDogTitle;
class ARBDogTrial;

struct CFontInfo
{
	CString name;
	int size;
	bool italic;
	bool bold;
	void CreateFont(CFont& font, CDC* pDC = NULL);
	void CreateFont(const CFontDialog& dlg, CFont& font, CDC* pDC = NULL);
};

struct CVenueFilter
{
	std::string venue;
	std::string division;
	std::string level;
};

class CAgilityBookOptions
{
public:
	// Helper functions
	static bool IsDateVisible(const ARBDate& startDate, const ARBDate& endDate);
	static bool IsTitleVisible(
		const std::vector<CVenueFilter>& venues,
		const ARBDogTitle* pTitle);
	static bool IsVenueVisible(
		const std::vector<CVenueFilter>& venues,
		const std::string& venue);
	static bool IsTrialVisible(
		const std::vector<CVenueFilter>& venues,
		const ARBDogTrial* pTrial);
	static bool IsRunVisible(
		const std::vector<CVenueFilter>& venues,
		const ARBDogTrial* pTrial,
		const ARBDogRun* pRun);
	// Calendar options
	static bool AutoDeleteCalendarEntries();
	static void SetAutoDeleteCalendarEntries(bool bAuto);
	static bool ViewAllCalendarEntries();
	static void SetViewAllCalendarEntries(bool bView);
	static int DaysTillEntryIsPast();
	static void SetDaysTillEntryIsPast(int nDays);
	static bool HideOverlappingCalendarEntries();
	static void SetHideOverlappingCalendarEntries(bool bHide);
	static bool ViewCalendarAsList();
	static void GetCalendarDateFontInfo(CFontInfo& info, BOOL bPrinting);
	static void SetCalendarDateFontInfo(const CFontInfo& info, BOOL bPrinting);
	static void GetCalendarTextFontInfo(CFontInfo& info, BOOL bPrinting);
	static void SetCalendarTextFontInfo(const CFontInfo& info, BOOL bPrinting);
	static CSize GetCalendarEntrySize();
	static void SetCalendarEntrySize(const CSize& sz);
	// Common options
	static ARBDate::DayOfWeek GetFirstDayOfWeek();
	static void SetFirstDayOfWeek(ARBDate::DayOfWeek day);
	static void GetPrinterFontInfo(CFontInfo& info);
	static void SetPrinterFontInfo(const CFontInfo& info);
	// Runs/points options
	static bool GetNewestDatesFirst();
	static void SetNewestDatesFirst(bool bNewest);
	static bool GetViewAllDates();
	static void SetViewAllDates(bool bViewAll);
	static ARBDate GetStartFilterDate();
	static void SetStartFilterDate(const ARBDate& date);
	static bool GetStartFilterDateSet();
	static void SetStartFilterDateSet(bool bSet);
	static ARBDate GetEndFilterDate();
	static void SetEndFilterDate(const ARBDate& date);
	static bool GetEndFilterDateSet();
	static void SetEndFilterDateSet(bool bSet);
	static bool GetViewAllVenues();
	static void SetViewAllVenues(bool bViewAll);
	static void GetFilterVenue(std::vector<CVenueFilter>& venues);
	static void SetFilterVenue(const std::vector<CVenueFilter>& venues);
	// Last entered options
	static CString GetLastEnteredDivision();
	static void SetLastEnteredDivision(const char* inLast);
	static CString GetLastEnteredLevel();
	static void SetLastEnteredLevel(const char* inLast);
	static CString GetLastEnteredHeight();
	static void SetLastEnteredHeight(const char* inLast);
	static CString GetLastEnteredJudge();
	static void SetLastEnteredJudge(const char* inLast);
	static CString GetLastEnteredHandler();
	static void SetLastEnteredHandler(const char* inLast);
	// Special clipboard formats
	typedef enum
	{
		eFormatTrial,
		eFormatRun
	} eClipFormat;
	static UINT GetClipboardFormat(eClipFormat fmt);
};
