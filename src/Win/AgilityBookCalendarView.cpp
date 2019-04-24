/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CAgilityBookViewCalendar class
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2016-09-02 Add support for scrolling on touch (or mouse drag).
 * 2013-01-01 Allow the mouse wheel to scroll beyond last entry.
 *            Add better keyboard navigation on Mac.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-14 Removed wxScrollWindow - scroll wasn't used.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-10 Ported to wxWidgets.
 * 2007-12-21 Localize months/days (because of static link CRT)
 * 2007-01-13 Fixed a problem with scroll bar not appearing.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-12-31 Make F1 invoke context help.
 * 2004-10-04 Added div-by-0 tests.
 * 2004-09-28 Set the page scrolling size to 4 weeks.
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-11-22 Update the list when creating an entry.
 * 2003-11-21 Enabled copy and select all.
 * 2003-10-31 Cleaned up pagedown scrolling.
 * 2003-10-29 Support the mouse wheel for scrolling.
 * 2003-10-22 Right click sets the current date.
 * 2003-08-27 Cleaned up selection synchronization.
 * 2003-08-09 When dbl-clicking on a date, make sure all entries are
 *            visible - even if 'hide on entered' option is on.
 */

#include "stdafx.h"
#include "AgilityBookCalendarView.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookMenu.h"
#include "AgilityBookOptions.h"
#include "AgilityBookPanels.h"
#include "ClipBoard.h"
#include "DlgAssignColumns.h"
#include "DlgCalendar.h"
#include "DlgListCtrl.h"
#include "FilterOptions.h"
#include "MainFrm.h"
#include "TabView.h"

#include "ARB/ARBCalendar.h"
#include "ARBCommon/StringUtil.h"
#include <wx/dcbuffer.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

#define WEEKS_PER_PAGE		6
// In dlgunits
#define DAY_TEXT_INSET		1


class CAgilityBookCalendar : public wxWindow
{
	DECLARE_CLASS(CAgilityBookCalendar)
	DECLARE_NO_COPY_IMPLEMENTED(CAgilityBookCalendar)
public:
	CAgilityBookCalendar(
			CAgilityBookCalendarView* parentView,
			CBasePanel* parentOfView,
			wxWindow* parent);
	~CAgilityBookCalendar();

	int GetDayHeight();

	void OnDraw(wxDC* pDC);

	ARBDate CurrentDate() const		{return m_Current;}
	ARBDate FirstDate() const		{return m_First;}
	ARBDate LastDate() const		{return m_Last;}
	ARBDate FirstDayOfWeek(ARBDate const& inDate) const;
	ARBDate LastDayOfWeek(ARBDate const& inDate) const;
	ARBDate FirstDayOfVisibleMonth() const;

	bool SetCurrentDate(
			ARBDate const& date,
			bool bEnsureVisible);

	void Activate(bool activate);
	void RefreshCurrentDate();

	int NumEvents() const		{return static_cast<int>(m_Calendar.size());}

	void OnClick(wxPoint pt);
	void OnWheel(int wheel);
	bool CanCopy() const;
	void OnCopy();
	bool CanEdit() const;
	void OnEdit(CAgilityBookDoc* pDoc);
	bool CanDelete() const;
	void OnDelete(CAgilityBookDoc* pDoc);
	bool CanCreateEntry(CAgilityBookDoc* pDoc) const;
	void OnCreateEntry(CAgilityBookDoc* pDoc);

	void LoadData(CAgilityBookDoc* pDoc);
	void LoadColumns();

private:
	void GetWorkingAreas(
			wxDC* pDC,
			wxRect& outHeader,
			wxRect& outDaysOfWeek,
			wxRect& outCalendar,
			int& outDayWidth,
			int& outDayHeight,
			bool bIsPrinting);
	size_t GetEntriesOn(
			ARBDate const& date,
			std::vector<ARBCalendarPtr>& entries,
			bool bGetHidden) const;
	wxRect GetDateRect(ARBDate const& date);
	void GetDateFromPoint(
			wxPoint pt,
			ARBDate& date);

	CAgilityBookCalendarView* m_parentView;
	CBasePanel* m_parentPanel;
	std::vector<long> m_Columns;
	std::vector<ARBCalendarPtr> m_Calendar;
	std::vector<ARBCalendarPtr> m_CalendarHidden;
	ARBDate m_First;	///< First date, adjusted to Mon of that week.
	ARBDate m_Last;		///< Last trial date.
	int m_nMonths;
	int m_nCurOffset;	// Used during printing.
	ARBDate m_Current;	///< Currently selected date.
	wxFont m_fontMonthPrint;
	wxFont m_fontTextPrint;
	wxFont m_fontMonth;
	wxFont m_fontText;
	wxString m_Months[12];
	wxString m_Days[7];

	DECLARE_EVENT_TABLE()
	void OnPaint(wxPaintEvent& evt);
	void OnSetFocus(wxFocusEvent& evt);
	void OnKillFocus(wxFocusEvent& evt);
};


IMPLEMENT_CLASS(CAgilityBookCalendar, wxWindow)


BEGIN_EVENT_TABLE(CAgilityBookCalendar, wxWindow)
	EVT_PAINT(CAgilityBookCalendar::OnPaint)
	EVT_SET_FOCUS(CAgilityBookCalendar::OnSetFocus)
	EVT_KILL_FOCUS(CAgilityBookCalendar::OnKillFocus)
END_EVENT_TABLE()


CAgilityBookCalendar::CAgilityBookCalendar(
		CAgilityBookCalendarView* parentView,
		CBasePanel* parentOfView,
		wxWindow* parent)
	: m_parentView(parentView)
	, m_parentPanel(parentOfView)
	, m_Columns()
	, m_Calendar()
	, m_CalendarHidden()
	, m_First()
	, m_Last()
	, m_nMonths(0)
	, m_nCurOffset(0)
	, m_Current(ARBDate::Today())
	, m_fontMonthPrint()
	, m_fontTextPrint()
	, m_fontMonth()
	, m_fontText()
{
	if (!wxWindow::Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxWANTS_CHARS | wxFULL_REPAINT_ON_RESIZE))
		return;
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	SetInitialSize(wxDefaultSize);
}


CAgilityBookCalendar::~CAgilityBookCalendar()
{
}


int CAgilityBookCalendar::GetDayHeight()
{
	wxClientDC dc(this);

	wxRect rHeader, rDaysOfWeek, rCalendar;
	int width, height;
	GetWorkingAreas(&dc, rHeader, rDaysOfWeek, rCalendar, width, height, false);

	return height;
}


void CAgilityBookCalendar::OnDraw(wxDC* pDC)
{
	if (m_First.IsValid() && m_Last.IsValid())
	{
		// If it's not a wxAutoBufferedPaintDC, then assume we're printing (preview or printer)
		bool bIsPrinting = wxDynamicCast(pDC, wxAutoBufferedPaintDC) ? false : true;

		wxPen penNull(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW), 1, wxPENSTYLE_TRANSPARENT);
		wxPen penFrame(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), 1);
		wxPen penShadow(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW), 1);
		wxPen penHilite(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHILIGHT), 1);
		wxBrush brNull(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW), wxBRUSHSTYLE_TRANSPARENT);
		wxBrush brBtnFace(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
		wxBrush brHighlight(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
		wxBrush brMenu(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
		wxBrush brWorkspace(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));

		pDC->SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
		pDC->SetTextBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		pDC->SetBackgroundMode(wxTRANSPARENT);

		// Colors for current selected date
		wxColour clrNotEntered = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorNotEntered);
		wxColour clrPast = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorPast);
		wxColour clrPlanning = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorPlanning);
		wxColour clrPending = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorPending);
		wxColour clrEntered = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorEntered);
		wxColour clrOpening = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorOpening);
		wxColour clrClosing = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorClosing);

		// Figure out which month we're on.
		ARBDate curMonth = FirstDayOfVisibleMonth();
		curMonth = LastDayOfWeek(curMonth);
		std::wstring str = fmt::format(L"{} {}", m_Months[curMonth.GetMonth()-1].c_str(), curMonth.GetYear());

		wxRect rHeader;
		wxRect rWeekDays;
		wxRect rCalendar;
		int width;
		int height;

		GetWorkingAreas(pDC, rHeader, rWeekDays, rCalendar, width, height, bIsPrinting);

		// Print month text
		if (bIsPrinting)
			pDC->SetFont(m_fontMonthPrint);
		else
			pDC->SetFont(m_fontMonth);
		{
			wxDCClipper clip(*pDC, rHeader);
			pDC->DrawLabel(StringUtil::stringWX(str), rHeader, wxALIGN_TOP|wxALIGN_CENTRE_HORIZONTAL);
		}

		if (bIsPrinting)
			pDC->SetFont(m_fontTextPrint);
		else
			pDC->SetFont(m_fontText);

		// Fill the weekday area and frame
		pDC->SetPen(penFrame);
		pDC->SetBrush(brBtnFace);
		// Note: Rectangles work differently in wxWidgets than Windows. In
		// Windows, drawing a rectangle does not include the lower right,
		// in wxWindows, it does. (But lines do not)
		pDC->DrawRectangle(rWeekDays);

		// Draw the days-of-week boxes and text (but not frame - we delay
		// drawing frame till end because when printing, the off-month fill
		// can bleed onto lines)
		rWeekDays.width = width;
		wxRect rect(rWeekDays);
		// Tweak rect to only contain the working day area
		rect.Offset(1, 1);
		--rect.width;
		--rect.height;
		ARBDate weekStart = FirstDayOfWeek(m_First);
		int iDay;
		for (iDay = 0; iDay < 7; ++iDay)
		{
			{
				// dow is now the index of the day of week.
				int dow = (weekStart + iDay).GetDayOfWeek(CAgilityBookOptions::GetFirstDayOfWeek());
				// Now translate back to Sun==0
				dow = (dow + CAgilityBookOptions::GetFirstDayOfWeek()) % 7;
				wxDCClipper clip(*pDC, rect);
				pDC->DrawLabel(m_Days[dow], rect, wxALIGN_CENTRE);
			}
			pDC->SetPen(penShadow);
			pDC->DrawLine(rect.GetLeft() + 1, rect.GetBottom(), rect.GetRight(), rect.GetBottom());
			pDC->DrawLine(rect.GetRight(), rect.GetBottom(), rect.GetRight(), rect.GetTop());
			pDC->SetPen(penHilite);
			pDC->DrawLine(rect.GetRight(), rect.GetTop(), rect.GetLeft(), rect.GetTop());
			pDC->DrawLine(rect.GetLeft(), rect.GetTop(), rect.GetLeft(), rect.GetBottom() + 1);
			// Offset to next day.
			rect.Offset(width, 0);
		}

		// Draw the days
		int dayTextInset = wxDLG_UNIT_X(this, DAY_TEXT_INSET);
		ARBDate today = ARBDate::Today();
		ARBDate day = FirstDayOfWeek(curMonth);
		int iWeek;
		for (iWeek = 0; iWeek < WEEKS_PER_PAGE; ++iWeek)
		{
			rect.x = rCalendar.x + 1;
			rect.width = width - 1;
			rect.y = rCalendar.y + 1;
			rect.height = height - 1;
			rect.Offset(0, iWeek * height);
			rect.Inflate(-dayTextInset, -dayTextInset);
			for (iDay = 0; iDay < 7; ++iDay)
			{
				// Change background of non-current month days
				if (!bIsPrinting && m_Current == day)
				{
					wxRect r(rect);
					r.Inflate(dayTextInset + 1, dayTextInset + 1); // +1 for null pen
					if (wxWindow::DoFindFocus() == this)
						pDC->SetBrush(brHighlight);
					else
						pDC->SetBrush(brMenu);
					pDC->DrawRectangle(r);
				}
				else if (day.GetMonth() != curMonth.GetMonth())
				{
					wxRect r(rect);
					r.Inflate(dayTextInset + 1, dayTextInset + 1);
					pDC->SetBrush(brWorkspace);
					pDC->DrawRectangle(r);
				}

				pDC->SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
				if (!bIsPrinting)
				{
					if (m_Current == day)
					{
						if (wxWindow::DoFindFocus() == this)
							pDC->SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
						else
							pDC->SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUTEXT));
					}
				}

				// Display date (only day now, not full date)
				//wxString str(day.GetString());
				str = fmt::format(L"{}", day.GetDay());
				{
					wxDCClipper clip(*pDC, rect);
					pDC->DrawLabel(StringUtil::stringWX(str), rect, wxALIGN_TOP|wxALIGN_RIGHT);
				}

				// Display entries
				std::vector<ARBCalendarPtr> entries;
				if (0 < GetEntriesOn(day, entries, false))
				{
					wxRect r(rect);
					for (std::vector<ARBCalendarPtr>::iterator iter = entries.begin(); iter != entries.end(); ++iter)
					{
						r.y += pDC->GetCharHeight();
						r.height -= pDC->GetCharHeight();

						ARBCalendarPtr pCal = (*iter);
						str.clear();
						for (size_t iCol = 0; iCol < m_Columns.size(); ++iCol)
						{
							if (0 < iCol)
								str += L" ";
							switch (m_Columns[iCol])
							{
							case IO_CAL_LOCATION:
								if (pCal->GetLocation().empty())
									str += L"?";
								else
									str += pCal->GetLocation();
								break;
							case IO_CAL_CLUB:
								if (pCal->GetClub().empty())
									str += L"?";
								else
									str += pCal->GetClub();
								break;
							case IO_CAL_VENUE:
								if (pCal->GetVenue().empty())
									str += L"?";
								else
									str += pCal->GetVenue();
								break;
							case IO_CAL_NOTES:
								if (pCal->GetNote().empty())
									str += L"?";
								else
									str += pCal->GetNote();
								break;
							}
						}
						bool bReset = false;
						wxColour oldText = pDC->GetTextForeground();
						// Don't change the color on the selected day.
						// (That whole foreground/background thing)
						if (m_Current != day)
						{
							if (CAgilityBookOptions::ViewAllCalendarOpening()
							&& pCal->GetOpeningDate() == day)
							{
								bReset = true;
								pDC->SetTextForeground(clrOpening);
							}
							else if (CAgilityBookOptions::ViewAllCalendarClosing()
							&& pCal->GetClosingDate() == day)
							{
								bReset = true;
								pDC->SetTextForeground(clrClosing);
							}
							else if (pCal->IsBefore(today))
							{
								bReset = true;
								pDC->SetTextForeground(clrPast);
							}
							else switch (pCal->GetEntered())
							{
							case ARBCalendar::eNot:
								bReset = true;
								pDC->SetTextForeground(clrNotEntered);
								break;
							case ARBCalendar::ePlanning:
								bReset = true;
								pDC->SetTextForeground(clrPlanning);
								break;
							case ARBCalendar::ePending:
								bReset = true;
								pDC->SetTextForeground(clrPending);
								break;
							case ARBCalendar::eEntered:
								bReset = true;
								pDC->SetTextForeground(clrEntered);
								break;
							}
						}
						wxDCClipper clip(*pDC, r);
						pDC->DrawLabel(StringUtil::stringWX(str), r, wxALIGN_LEFT);
						if (bReset)
							pDC->SetTextForeground(oldText);
					}
				}

				// Get ready for next round
				rect.Offset(width, 0);
				++day;
			}
		}

		// Now draw all frames (do this last so the print preview looks ok)
		pDC->SetPen(penFrame);
		pDC->SetBrush(brNull);
		pDC->DrawRectangle(rCalendar);
		// Vertical lines
		for (iDay = 1; iDay < 7; ++iDay)
		{
			pDC->DrawLine(
				rCalendar.x + iDay * width, rWeekDays.y,
				rCalendar.x + iDay * width, rCalendar.GetBottom());
		}
		// Horizontal lines
		for (iWeek = 1; iWeek < WEEKS_PER_PAGE; ++iWeek)
		{
			pDC->DrawLine(
				rCalendar.x, rCalendar.y + iWeek * height,
				rCalendar.GetRight(), rCalendar.y + iWeek * height);
		}

		pDC->SetFont(wxNullFont);
		pDC->SetPen(wxNullPen);
		pDC->SetBrush(wxNullBrush);
	}
}


ARBDate CAgilityBookCalendar::FirstDayOfWeek(ARBDate const& inDate) const
{
	ARBDate date(inDate);
	date -= date.GetDayOfWeek(CAgilityBookOptions::GetFirstDayOfWeek());
	return date;
}


ARBDate CAgilityBookCalendar::LastDayOfWeek(ARBDate const& inDate) const
{
	ARBDate date(inDate);
	date += 6 - date.GetDayOfWeek(CAgilityBookOptions::GetFirstDayOfWeek());
	return date;
}


ARBDate CAgilityBookCalendar::FirstDayOfVisibleMonth() const
{
	ARBDate date = FirstDayOfWeek(m_First);
	if (0 < m_nCurOffset)
	{
		date = LastDayOfWeek(date); // Go to end-of-week (make sure month is correct)
		// Push forward offset months
		int y = date.GetYear();
		int m = date.GetMonth();
		y += m_nCurOffset / 12;
		m += m_nCurOffset % 12;
		if (m > 12)
		{
			++y;
			m -= 12;
		}
		// Set to first day of month
		date.SetDate(y, m, 1);
		// Then fix 1st day of week.
		date = FirstDayOfWeek(date);
	}
	return date;
}


bool CAgilityBookCalendar::SetCurrentDate(
		ARBDate const& date,
		bool bEnsureVisible)
{
	bool bSet = false;
	ARBDate dateFirstVisible = FirstDayOfWeek(m_First);
	ARBDate dateLastVisible(m_Last.GetYear(), m_Last.GetMonth(), 1);
	dateLastVisible = FirstDayOfWeek(dateLastVisible);
	dateLastVisible += WEEKS_PER_PAGE * 7 - 1;
	// TODO
	// It appears letting the end date run free is ok.
	// However, freeing the start date doesn't work as well.
	// The date changes, but the calendar won't scroll up.
	if (date != m_Current
	&& dateFirstVisible <= date /*&& date <= dateLastVisible*/)
	{
		bSet = true;
		// Invalidate the current date.
		if (m_Current.IsValid())
		{
			wxRect r = GetDateRect(m_Current);
			RefreshRect(r);
		}
		// Now handle the new date.
		m_Current = date;
		if (m_Current.IsValid())
		{
			bool bInvalidate = true;
			if (bEnsureVisible)
			{
				ARBDate dateTop = FirstDayOfVisibleMonth();
				ARBDate dateBottom = dateTop + WEEKS_PER_PAGE * 7 - 1;
				if (!m_Current.isBetween(dateTop, dateBottom))
				{
					m_nCurOffset = (m_Current.GetYear() - m_First.GetYear()) * 12 + m_Current.GetMonth() - m_First.GetMonth();
					Refresh();
					bInvalidate = false;
				}
			}
			if (bInvalidate)
			{
				wxRect r = GetDateRect(m_Current);
				RefreshRect(r);
			}
		}
	}
	return bSet;
}


void CAgilityBookCalendar::Activate(bool activate)
{
	RefreshCurrentDate();
}


void CAgilityBookCalendar::RefreshCurrentDate()
{
	if (m_Current.IsValid())
	{
		wxRect r = GetDateRect(m_Current);
		RefreshRect(r);
		Update();
	}
}


void CAgilityBookCalendar::OnClick(wxPoint pt)
{
	ARBDate date;
	GetDateFromPoint(pt, date);
	SetCurrentDate(date, false);
}


void CAgilityBookCalendar::OnWheel(int wheel)
{
	if (0 < wheel)
	{
		if (0 < m_nCurOffset)
		{
			--m_nCurOffset;
			Refresh();
			Update();
		}
	}
	else
	{
		++m_nCurOffset;
		Refresh();
		Update();
	}
}


bool CAgilityBookCalendar::CanCopy() const
{
	return 0 < m_Calendar.size();
}


void CAgilityBookCalendar::OnCopy()
{
	CClipboardDataWriter clpData;
	if (!clpData.isOkay())
		return;

	static const wchar_t* scColumns[] =
	{
		arbT("IDS_COL_START_DATE"),
		arbT("IDS_COL_END_DATE"),
		arbT("IDS_COL_VENUE"),
		arbT("IDS_COL_LOCATION"),
		arbT("IDS_COL_CLUB"),
		arbT("IDS_COL_OPENS"),
		arbT("IDS_COL_CLOSES"),
		arbT("IDS_COL_NOTES"),
	};
	static int const scNumColumns = sizeof(scColumns) / sizeof(scColumns[0]);
#define COL_START_DATE	0
#define COL_END_DATE	1
#define COL_VENUE		2
#define COL_LOCATION	3
#define COL_CLUB		4
#define COL_OPENS		5
#define COL_CLOSES		6
#define COL_NOTES		7

	int index = 0;
	size_t maxLen[scNumColumns] = { 0 };
	std::wstring columns[scNumColumns];
	for (index = 0; index < scNumColumns; ++index)
	{
		wxString str = wxGetTranslation(scColumns[index]);
		maxLen[index] = str.length();
		columns[index] = str;
	}
	std::vector<ARBCalendarPtr>::const_iterator iter;
	for (iter = m_Calendar.begin(); iter != m_Calendar.end(); ++iter)
	{
		ARBCalendarPtr cal = *iter;
		size_t len = cal->GetStartDate().GetString().length();
		if (len > maxLen[COL_START_DATE])
			maxLen[COL_START_DATE] = len;
		len = cal->GetEndDate().GetString().length();
		if (len > maxLen[COL_END_DATE])
			maxLen[COL_END_DATE] = len;
		len = cal->GetLocation().length();
		if (len > maxLen[COL_LOCATION])
			maxLen[COL_LOCATION] = len;
		len = cal->GetClub().length();
		if (len > maxLen[COL_CLUB])
			maxLen[COL_CLUB] = len;
		len = cal->GetVenue().length();
		if (len > maxLen[COL_VENUE])
			maxLen[COL_VENUE] = len;
		len = cal->GetOpeningDate().GetString().length();
		if (len > maxLen[COL_OPENS])
			maxLen[COL_OPENS] = len;
		len = cal->GetClosingDate().GetString().length();
		if (len > maxLen[COL_CLOSES])
			maxLen[COL_CLOSES] = len;
		len = cal->GetNote().length();
		if (len > maxLen[COL_NOTES])
			maxLen[COL_NOTES] = len;
	}

	const std::wstring fmtspec(L"{}{:>{}} - {:{}} {:{}} {:{}} {:{}} {:>{}}{}{:{}} {:{}}");
	const std::wstring col1(L"  ");
	const std::wstring opencloseSep(L"   ");
	const std::wstring opencloseSep2(L" - ");

	// The header
	std::wstring data = fmt::format(fmtspec,
		col1,
		columns[COL_START_DATE], maxLen[COL_START_DATE],
		columns[COL_END_DATE], maxLen[COL_END_DATE],
		columns[COL_VENUE], maxLen[COL_VENUE],
		columns[COL_LOCATION], maxLen[COL_LOCATION],
		columns[COL_CLUB], maxLen[COL_CLUB],
		columns[COL_OPENS], maxLen[COL_OPENS],
		opencloseSep,
		columns[COL_CLOSES], maxLen[COL_CLOSES],
		columns[COL_NOTES], maxLen[COL_NOTES]);
	data = StringUtil::TrimRight(data) + L"\n";

	// The data
	for (iter = m_Calendar.begin(); iter != m_Calendar.end(); ++iter)
	{
		ARBCalendarPtr cal = *iter;
		std::wstring items[scNumColumns];
		items[COL_START_DATE] = cal->GetStartDate().GetString();
		items[COL_END_DATE] = cal->GetEndDate().GetString();
		items[COL_LOCATION] = cal->GetLocation();
		items[COL_CLUB] = cal->GetClub();
		items[COL_VENUE] = cal->GetVenue();
		items[COL_OPENS] = cal->GetOpeningDate().GetString();
		items[COL_CLOSES] = cal->GetClosingDate().GetString();
		items[COL_NOTES] = StringUtil::Replace(cal->GetNote(), L"\n", L" ");
		wxString tentative(L"  ");
		if (cal->IsTentative())
			tentative = L"? ";
		assert(col1.length() == tentative.length());
		std::wstring str = fmt::format(fmtspec,
			tentative.c_str(),
			items[COL_START_DATE], maxLen[COL_START_DATE],
			items[COL_END_DATE], maxLen[COL_END_DATE],
			items[COL_VENUE], maxLen[COL_VENUE],
			items[COL_LOCATION], maxLen[COL_LOCATION],
			items[COL_CLUB], maxLen[COL_CLUB],
			items[COL_OPENS], maxLen[COL_OPENS],
			(0 < items[COL_OPENS].length() || 0 < items[COL_CLOSES].length()) ? opencloseSep2 : opencloseSep,
			items[COL_CLOSES], maxLen[COL_CLOSES],
			items[COL_NOTES], maxLen[COL_NOTES]);
		data += StringUtil::TrimRight(str) + L"\n";
	}
	clpData.AddData(data);
	clpData.CommitData();
}


bool CAgilityBookCalendar::CanEdit() const
{
	return m_Current.IsValid();
}


void CAgilityBookCalendar::OnEdit(CAgilityBookDoc* pDoc)
{
	if (m_Current.IsValid())
	{
		CTabView* pView = pDoc->GetTabView();
		std::vector<ARBCalendarPtr> entries;
		GetEntriesOn(m_Current, entries, true);
		CDlgListCtrl dlg(pDoc, m_Current, &entries, pView, this);
		if (wxID_OK == dlg.ShowModal())
		{
			m_parentView->LoadData();
			pDoc->Modify(true);
			// DlgListCtrl calls UpdateAllViews.
		}
	}
}

bool CAgilityBookCalendar::CanDelete() const
{
	std::vector<ARBCalendarPtr> entries;
	GetEntriesOn(m_Current, entries, true);
	return 0 < entries.size();
}


void CAgilityBookCalendar::OnDelete(CAgilityBookDoc* pDoc)
{
	std::vector<ARBCalendarPtr> entries;
	GetEntriesOn(m_Current, entries, true);
	if (0 < entries.size())
		OnEdit(pDoc);
}


bool CAgilityBookCalendar::CanCreateEntry(CAgilityBookDoc* pDoc) const
{
	std::vector<ARBCalendarPtr> entries;
	GetEntriesOn(m_Current, entries, false);
	if (1 == entries.size())
	{
		// Make sure the venue is recognized.
		ARBCalendarPtr pCal = *(entries.begin());
		return pDoc->Book().GetConfig().GetVenues().FindVenue(pCal->GetVenue());
	}
	else if (1 < entries.size())
		return true;
	return false;
}


void CAgilityBookCalendar::OnCreateEntry(CAgilityBookDoc* pDoc)
{
	std::vector<ARBCalendarPtr> entries;
	GetEntriesOn(m_Current, entries, false);
	if (1 == entries.size())
	{
		ARBCalendarPtr pCal = *(entries.begin());
		CTabView* pView = pDoc->GetTabView();
		if (pDoc->CreateTrialFromCalendar(*pCal, pView))
			pDoc->Modify(true);
	}
	else if (1 < entries.size())
		OnEdit(pDoc);
}


void CAgilityBookCalendar::LoadData(CAgilityBookDoc* pDoc)
{
	STACK_TRACE(stack, L"CAgilityBookCalendar::LoadData");

	Freeze();

	// Clear everything.
	m_Calendar.clear();
	m_CalendarHidden.clear();
	m_Last = ARBDate::Today();
	m_First = ARBDate::Today();
	LoadColumns();

	ARBDate today(ARBDate::Today());
	today -= CAgilityBookOptions::DaysTillEntryIsPast();
	bool bViewAll = CAgilityBookOptions::ViewAllCalendarEntries();
	bool bHide = CAgilityBookOptions::HideOverlappingCalendarEntries();
	CCalendarViewFilter filter = CFilterOptions::Options().FilterCalendarView();

	// Add items.
	std::vector<ARBCalendarPtr> entered;
	if (bHide)
		pDoc->Book().GetCalendar().GetAllEntered(entered);
	for (ARBCalendarList::iterator iter = pDoc->Book().GetCalendar().begin();
		iter != pDoc->Book().GetCalendar().end();
		++iter)
	{
		ARBCalendarPtr pCal = (*iter);
		if (pCal->IsFiltered())
			continue;
		bool bSuppress = false;
		if (!bViewAll)
		{
			if (pCal->IsBefore(today))
				bSuppress = true;
		}
		if (!bSuppress && bHide)
		{
			for (std::vector<ARBCalendarPtr>::const_iterator iterE = entered.begin();
				!bSuppress && iterE != entered.end();
				++iterE)
			{
				ARBCalendarPtr pEntered = (*iterE);
				if (pCal != pEntered
				&& pCal->IsRangeOverlapped(pEntered->GetStartDate(), pEntered->GetEndDate()))
				{
					bSuppress = true;
				}
			}
		}
		if (bSuppress)
			m_CalendarHidden.push_back(pCal);
		else
		{
			bool bAdd = false;
			ARBDate f, l;
			if (filter.ViewNotEntered()
			|| filter.ViewPlanning()
			|| filter.ViewEntered())
			{
				bAdd = true;
				f = pCal->GetStartDate();
				l = pCal->GetEndDate();
			}
			if (ARBCalendar::ePlanning == pCal->GetEntered()
			&& filter.ViewPlanning())
			{
				if (CAgilityBookOptions::ViewAllCalendarOpening()
				&& pCal->GetOpeningDate().IsValid())
				{
					bAdd = true;
					if (!f.IsValid() || pCal->GetOpeningDate() < f)
						f = pCal->GetOpeningDate();
					if (!l.IsValid() || pCal->GetOpeningDate() > l)
						l = pCal->GetOpeningDate();
				}
				if (CAgilityBookOptions::ViewAllCalendarClosing()
				&& pCal->GetClosingDate().IsValid())
				{
					bAdd = true;
					if (!f.IsValid() || pCal->GetClosingDate() < f)
						f = pCal->GetClosingDate();
					if (!l.IsValid() || pCal->GetClosingDate() > l)
						l = pCal->GetClosingDate();
				}
			}
			if (f.IsValid() && (!m_First.IsValid() || f < m_First))
				m_First = f;
			if (l.IsValid() && (!m_Last.IsValid() || l > m_Last))
				m_Last = l;
			if (bAdd)
				m_Calendar.push_back(pCal);
		}
	}
	if (!m_Current.IsValid() || m_Current < m_First)
		m_Current = ARBDate::Today();
	// Set to first day of month
	m_First.SetDate(m_First.GetYear(), m_First.GetMonth(), 1);
	// Set of last day of month
	if (12 == m_Last.GetMonth())
		m_Last.SetDate(m_Last.GetYear() + 1, 1, 1);
	else
		m_Last.SetDate(m_Last.GetYear(), m_Last.GetMonth() + 1, 1);
	--m_Last;
	m_nMonths = (m_Last.GetYear() - m_First.GetYear()) * 12 + m_Last.GetMonth() - m_First.GetMonth() + 1;

	// Now initialize all our drawing stuff.
	CFontInfo fontInfo;
	CAgilityBookOptions::GetPrinterFontInfo(fontInfo);
	fontInfo.CreateFont(m_fontTextPrint);
	fontInfo.size *= 3;
	fontInfo.CreateFont(m_fontMonthPrint);
	CAgilityBookOptions::GetCalendarFontInfo(fontInfo);
	fontInfo.CreateFont(m_fontText);
	fontInfo.size *= 3;
	fontInfo.CreateFont(m_fontMonth);

	// Make sure the current date is visible.
	ARBDate date(m_Current);
	m_Current.clear();
	SetCurrentDate(date, true);

	Thaw();
	Refresh();
}


void CAgilityBookCalendar::LoadColumns()
{
	CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_CALENDAR, m_Columns);
	m_Months[0] = _("January");
	m_Months[1] = _("February");
	m_Months[2] = _("March");
	m_Months[3] = _("April");
	m_Months[4] = _("May");
	m_Months[5] = _("June");
	m_Months[6] = _("July");
	m_Months[7] = _("August");
	m_Months[8] = _("September");
	m_Months[9] = _("October");
	m_Months[10] = _("November");
	m_Months[11] = _("December");
	m_Days[0] = _("Sunday");
	m_Days[1] = _("Monday");
	m_Days[2] = _("Tuesday");
	m_Days[3] = _("Wednesday");
	m_Days[4] = _("Thursday");
	m_Days[5] = _("Friday");
	m_Days[6] = _("Saturday");
	Refresh();
}


void CAgilityBookCalendar::GetWorkingAreas(
		wxDC* pDC,
		wxRect& outHeader,
		wxRect& outDaysOfWeek,
		wxRect& outCalendar,
		int& outDayWidth,
		int& outDayHeight,
		bool bIsPrinting)
{
	wxFont oldFont = pDC->GetFont();

	wxRect rClient = GetClientRect();
	if (bIsPrinting)
	{
		wxSize sz = pDC->GetSize();
		rClient.width = pDC->DeviceToLogicalXRel(sz.x);
		rClient.height = pDC->DeviceToLogicalYRel(sz.y);
		long leftMargin, rightMargin, topMargin, bottomMargin;
		CAgilityBookOptions::GetPrinterMargins(false, leftMargin, rightMargin, topMargin, bottomMargin, pDC);
		rClient.x += leftMargin;
		rClient.y += topMargin;
		rClient.width -= (leftMargin + rightMargin);
		rClient.height -= (topMargin + bottomMargin);
	}

	// Print month text
	if (bIsPrinting)
		pDC->SetFont(m_fontMonthPrint);
	else
		pDC->SetFont(m_fontMonth);
	outHeader = rClient;
	outHeader.height = pDC->GetCharHeight() + 3;

	rClient.y += outHeader.height;
	rClient.height -= outHeader.height;

	// Figure out working areas (weekdays, days).
	outDaysOfWeek = rClient;
	if (bIsPrinting)
		pDC->SetFont(m_fontTextPrint);
	else
		pDC->SetFont(m_fontText);
	outDaysOfWeek.height = pDC->GetCharHeight() + 2;

	rClient.y += outDaysOfWeek.height;
	rClient.height -= outDaysOfWeek.height;

	outCalendar = rClient;

	// Compute how big a day is. The rectangle includes the bounding rect
	// which goes on the top-left. The bottom-right is handled by the
	// adjacent day, with the "-1" below taking care of the final bounding.
	outDayWidth = (outCalendar.width - 1) / 7;
	if (bIsPrinting)
	{
		// Offset to center when printing.
		outCalendar.x += (outCalendar.width - (outDayWidth * 7 + 1)) / 2;
		outDaysOfWeek.x = outCalendar.x;
	}
	outDaysOfWeek.width = outCalendar.width = outDayWidth * 7 + 1;
	outDayHeight = (outCalendar.height - 1) / WEEKS_PER_PAGE;
	outCalendar.height = outDayHeight * WEEKS_PER_PAGE + 1;

	pDC->SetFont(oldFont);
}


size_t CAgilityBookCalendar::GetEntriesOn(
		ARBDate const& date,
		std::vector<ARBCalendarPtr>& entries,
		bool bGetHidden) const
{
	entries.clear();
	CCalendarViewFilter filter = CFilterOptions::Options().FilterCalendarView();
	for (std::vector<ARBCalendarPtr>::const_iterator iter = m_Calendar.begin();
		iter != m_Calendar.end();
		++iter)
	{
		ARBCalendarPtr pCal = *iter;
		if (pCal->InRange(date)
		&& ((ARBCalendar::eNot == pCal->GetEntered() && filter.ViewNotEntered())
		|| (ARBCalendar::ePlanning == pCal->GetEntered() && filter.ViewPlanning())
		|| ((ARBCalendar::ePending == pCal->GetEntered() || ARBCalendar::eEntered == pCal->GetEntered())
		&& filter.ViewEntered())))
		{
			entries.push_back((*iter));
		}
		// Only show opening/closing dates if we're planning on entering
		else if (ARBCalendar::ePlanning == pCal->GetEntered()
		&& filter.ViewPlanning())
		{
			if ((CAgilityBookOptions::ViewAllCalendarOpening()
			&& pCal->GetOpeningDate() == date)
			|| (CAgilityBookOptions::ViewAllCalendarClosing()
			&& pCal->GetClosingDate() == date))
				entries.push_back((*iter));
		}
	}
	if (bGetHidden)
	{
		for (std::vector<ARBCalendarPtr>::const_iterator iter = m_CalendarHidden.begin();
			iter != m_CalendarHidden.end();
			++iter)
		{
			if ((*iter)->InRange(date))
				entries.push_back((*iter));
		}
	}
	return entries.size();
}


/**
 * Returns the working rect for a date. Borders are handled entirely separately.
 * Note, this is used ONLY on the UI side, not while printing.
 */
wxRect CAgilityBookCalendar::GetDateRect(ARBDate const& date)
{
	if (!date.IsValid())
		return wxRect(0,0,0,0);

	ARBDate visible1 = FirstDayOfVisibleMonth();
	ARBDate visible2(visible1);
	visible2 += WEEKS_PER_PAGE * 7 - 1;

	if (!date.isBetween(visible1, visible2))
		return wxRect(0,0,0,0);

	int span = date - visible1;
	int x = span % 7;
	int y = span / 7;

	wxClientDC dc(this);

	wxRect rHeader, rDaysOfWeek, rCalendar;
	int width = 0, height = 0;
	GetWorkingAreas(&dc, rHeader, rDaysOfWeek, rCalendar, width, height, false);

	wxRect r(rCalendar.x + 1, rCalendar.y + 1, width - 1, height - 1);
	r.Offset(x * width, y * height);
	return r;
}


void CAgilityBookCalendar::GetDateFromPoint(
		wxPoint pt,
		ARBDate& date)
{
	date.clear();

	wxClientDC dc(this);

	wxRect rHeader, rDaysOfWeek, rCalendar;
	int width = 0, height = 0;
	GetWorkingAreas(&dc, rHeader, rDaysOfWeek, rCalendar, width, height, false);

	if (rCalendar.Contains(pt))
	{
		int x = (pt.x - rCalendar.x) / width;
		int y = (pt.y - rCalendar.y) / height;

		date = FirstDayOfVisibleMonth();
		date += x + y * 7;
	}
}


void CAgilityBookCalendar::OnPaint(wxPaintEvent& evt)
{
	wxAutoBufferedPaintDC dc(this);
	PrepareDC(dc);
	dc.SetBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	dc.Clear();
	OnDraw(&dc);
}


void CAgilityBookCalendar::OnSetFocus(wxFocusEvent& evt)
{
	RefreshCurrentDate();
	// We need this as clicking directly in the control does not change
	// the active view - we have to do it. Conversely, setting the active
	// view does not change the focused window, so we have to do that too.
	if (m_parentView->GetDocumentManager()->GetCurrentView() != m_parentView)
	{
		m_parentPanel->OnActivateView(m_parentView);
		m_parentView->Activate(true);
	}
	evt.Skip();
}


void CAgilityBookCalendar::OnKillFocus(wxFocusEvent& evt)
{
	RefreshCurrentDate();
	evt.Skip();
}

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(CAgilityBookCalendarView, CAgilityBookBaseExtraView)


BEGIN_EVENT_TABLE(CAgilityBookCalendarView, CAgilityBookBaseExtraView)
	EVT_CONTEXT_MENU(CAgilityBookCalendarView::OnContextMenu)
	EVT_UPDATE_UI(wxID_DUPLICATE, CAgilityBookCalendarView::OnViewUpdateCmd)
	EVT_MENU(wxID_DUPLICATE, CAgilityBookCalendarView::OnViewCmd)
	EVT_UPDATE_UI(wxID_CUT, CAgilityBookCalendarView::OnViewUpdateCmd)
	EVT_MENU(wxID_CUT, CAgilityBookCalendarView::OnViewCmd)
	EVT_UPDATE_UI(wxID_COPY, CAgilityBookCalendarView::OnViewUpdateCmd)
	EVT_MENU(wxID_COPY, CAgilityBookCalendarView::OnViewCmd)
	EVT_UPDATE_UI(wxID_PASTE, CAgilityBookCalendarView::OnViewUpdateCmd)
	EVT_MENU(wxID_PASTE, CAgilityBookCalendarView::OnViewCmd)
	EVT_UPDATE_UI(wxID_DELETE, CAgilityBookCalendarView::OnViewUpdateCmd)
	EVT_MENU(wxID_DELETE, CAgilityBookCalendarView::OnViewCmd)
	EVT_UPDATE_UI(wxID_SELECTALL, CAgilityBookCalendarView::OnViewUpdateCmd)
	EVT_MENU(wxID_SELECTALL, CAgilityBookCalendarView::OnViewCmd)
	EVT_UPDATE_UI(wxID_FIND, CAgilityBookCalendarView::OnViewUpdateCmd)
	EVT_MENU(wxID_FIND, CAgilityBookCalendarView::OnViewCmd)
	EVT_UPDATE_UI(ID_EDIT_FIND_NEXT, CAgilityBookCalendarView::OnViewUpdateCmd)
	EVT_MENU(ID_EDIT_FIND_NEXT, CAgilityBookCalendarView::OnViewCmd)
	EVT_UPDATE_UI(ID_EDIT_FIND_PREVIOUS, CAgilityBookCalendarView::OnViewUpdateCmd)
	EVT_MENU(ID_EDIT_FIND_PREVIOUS, CAgilityBookCalendarView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_EDIT_CALENDAR, CAgilityBookCalendarView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_EDIT_CALENDAR, CAgilityBookCalendarView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_DELETE_CALENDAR, CAgilityBookCalendarView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_DELETE_CALENDAR, CAgilityBookCalendarView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_EXPORT_CALENDAR, CAgilityBookCalendarView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_EXPORT_CALENDAR, CAgilityBookCalendarView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_CREATEENTRY_CALENDAR, CAgilityBookCalendarView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_CREATEENTRY_CALENDAR, CAgilityBookCalendarView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_CUSTOMIZE, CAgilityBookCalendarView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_CUSTOMIZE, CAgilityBookCalendarView::OnViewCmd)
END_EVENT_TABLE()


CAgilityBookCalendarView::CAgilityBookCalendarView(
		CTabView* pTabView,
		wxDocument* doc)
	: CAgilityBookBaseExtraView(pTabView, doc)
	, m_Ctrl(nullptr)
	, m_bTracking(false)
	, m_lastPt(0, 0)
	, m_motionDelta(0)
{
}


CAgilityBookCalendarView::~CAgilityBookCalendarView()
{
}


bool CAgilityBookCalendarView::Create(
		CBasePanel* parentView,
		wxWindow* parentCtrl,
		wxDocument* doc,
		long flags,
		wxSizer* sizer,
		int proportion,
		int sizerFlags,
		int border)
{
	m_Ctrl = new CAgilityBookCalendar(this, parentView, parentCtrl);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_LEFT_DOWN, wxMouseEventHandler, CAgilityBookCalendarView::OnCtrlMouseEvent);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_LEFT_UP, wxMouseEventHandler, CAgilityBookCalendarView::OnCtrlMouseEvent);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_RIGHT_DOWN, wxMouseEventHandler, CAgilityBookCalendarView::OnCtrlMouseEvent);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_LEFT_DCLICK, wxMouseEventHandler, CAgilityBookCalendarView::OnCtrlMouseEvent);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_LEAVE_WINDOW, wxMouseEventHandler, CAgilityBookCalendarView::OnCtrlMouseMove);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_MOTION, wxMouseEventHandler, CAgilityBookCalendarView::OnCtrlMouseMove);
#if wxUSE_MOUSEWHEEL
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_MOUSEWHEEL, wxMouseEventHandler, CAgilityBookCalendarView::OnCtrlMouseWheel);
#endif
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_CHAR, wxKeyEventHandler, CAgilityBookCalendarView::OnCtrlChar);
#if defined(__WXMAC__)
	m_Ctrl->SetDropTarget(new CFileDropTarget(doc->GetDocumentManager()));
#endif
	return CAgilityBookBaseExtraView::Create(parentView, parentCtrl, doc, flags, sizer, proportion, sizerFlags, border);
}


wxWindow* CAgilityBookCalendarView::GetControl()
{
	return m_Ctrl;
}


bool CAgilityBookCalendarView::PrevPane()
{
	if (m_parentPanel)
		return m_parentPanel->ActivateView(0);
	return false;
}


bool CAgilityBookCalendarView::NextPane()
{
	if (m_parentPanel)
		return m_parentPanel->ActivateView(0);
	return false;
}


void CAgilityBookCalendarView::DetachView()
{
	// The control is actually owned by the panel, the view is not.
	m_Ctrl = nullptr;
}


ARBDate CAgilityBookCalendarView::GetCurrentDate() const
{
	return m_Ctrl ? m_Ctrl->CurrentDate() : ARBDate();
}


bool CAgilityBookCalendarView::SetCurrentDate(
		ARBDate const& date,
		bool bEnsureVisible)
{
	return m_Ctrl ? m_Ctrl->SetCurrentDate(date, bEnsureVisible) : false;
}


bool CAgilityBookCalendarView::IsFiltered() const
{
	if (!CFilterOptions::Options().GetViewAllDates())
		return true;
	if (!CFilterOptions::Options().GetViewAllVenues())
		return true;
	CCalendarViewFilter filter = CFilterOptions::Options().FilterCalendarView();
	return filter.IsFiltered();
}


bool CAgilityBookCalendarView::GetMessage(std::wstring& msg) const
{
	if (!m_Ctrl)
		return false;
	msg = fmt::format(_("IDS_NUM_EVENTS").wx_str(), m_Ctrl->NumEvents());
	return true;
}


bool CAgilityBookCalendarView::GetMessage2(std::wstring& msg) const
{
	msg = StringUtil::stringW(_("IDS_INDICATOR_BLANK"));
	return true;
}


bool CAgilityBookCalendarView::AllowStatusContext(int field) const
{
	return STATUS_FILTERED == field;
}


bool CAgilityBookCalendarView::OnCreate(
		wxDocument* doc,
		long flags)
{
	return true;
}


void CAgilityBookCalendarView::DoActivateView(
		bool activate,
		wxView* activeView,
		wxView* deactiveView)
{
	if (m_Ctrl)
	{
		if (activate && wxWindow::DoFindFocus() != m_Ctrl)
			m_Ctrl->SetFocus();
		m_Ctrl->Activate(activate);
	}
}


void CAgilityBookCalendarView::OnDraw(wxDC* dc)
{
	if (m_Ctrl && dc)
		m_Ctrl->OnDraw(dc);
}


void CAgilityBookCalendarView::OnUpdate(
		wxView* sender,
		wxObject* inHint)
{
	STACK_TRACE(stack, L"CAgilityBookCalendarView::OnUpdate");

	if (!m_Ctrl)
		return;
	CUpdateHint* hint = nullptr;
	if (inHint)
		hint = wxDynamicCast(inHint, CUpdateHint);
	if (!hint || hint->IsSet(UPDATE_CALENDAR_VIEW)
	|| hint->IsEqual(UPDATE_OPTIONS))
	{
		LoadData();
	}
	else if (hint && (hint->IsEqual(UPDATE_CUSTOMIZE)
	|| hint->IsEqual(UPDATE_LANG_CHANGE)))
	{
		m_Ctrl->LoadColumns();
	}
}


void CAgilityBookCalendarView::LoadData()
{
	if (m_Ctrl)
	{
		m_Ctrl->LoadData(GetDocument());
		if (m_Ctrl->IsShownOnScreen())
			UpdateMessages();
	}
}


void CAgilityBookCalendarView::OnContextMenu(wxContextMenuEvent& evt)
{
	if (!m_Ctrl)
		return;
	wxPoint point;
	if (GetMenuPosition(point, m_Ctrl, evt))
	{
		wxMenu* menu = wxGetApp().GetMenus().CreatePopupMenu(m_Ctrl, IdMenuCalendar);
		m_Ctrl->PopupMenu(menu, point);
		delete menu;
	}
	else
		evt.Skip();
}


void CAgilityBookCalendarView::OnCtrlMouseEvent(wxMouseEvent& evt)
{
	if (!m_Ctrl)
		return;
	if (wxWindow::DoFindFocus() != m_Ctrl && evt.ButtonDown())
	{
		m_Ctrl->SetFocus();
	}
	if (evt.LeftDown() || evt.RightDown())
	{
		m_bTracking = true;
		m_lastPt = evt.GetPosition();
		m_motionDelta = m_Ctrl->GetDayHeight();
		m_Ctrl->OnClick(evt.GetPosition());
	}
	else if (evt.LeftUp())
	{
		m_bTracking = false;
		m_lastPt.x = m_lastPt.y = 0;
	}
	else if (evt.LeftDClick())
	{
		m_Ctrl->OnEdit(GetDocument());
	}
	evt.Skip();
}


void CAgilityBookCalendarView::OnCtrlMouseMove(wxMouseEvent& evt)
{
	if (!m_Ctrl)
		return;
	if (evt.Leaving())
	{
		m_bTracking = false;
		m_lastPt.x = m_lastPt.y = 0;
	}
	else if (m_bTracking && evt.Dragging())
	{
		wxPoint pt = evt.GetPosition();
		if (0 != m_lastPt.x && 0 != m_lastPt.y)
		{
			int diff = m_lastPt.y - pt.y;
			if (abs(diff) > m_motionDelta)
			{
				m_lastPt = pt;
				m_Ctrl->OnWheel(diff);
			}
		}
		else
			m_lastPt = pt;
	}
	evt.Skip();
}


#if wxUSE_MOUSEWHEEL
void CAgilityBookCalendarView::OnCtrlMouseWheel(wxMouseEvent& evt)
{
	if (m_Ctrl)
		m_Ctrl->OnWheel(evt.GetWheelRotation());
	evt.Skip();
}
#endif


enum CalendarMotion
{
	eMotionNone,
	eMotionDayLast,
	eMotionDayNext,
	eMotionWeekLast,
	eMotionWeekNext,
	eMotionMonthLast,
	eMotionMonthNext,
	eMotionWeekStart,
	eMotionWeekEnd,
	eMotionCalStart,
	eMotionCalEnd
};


void CAgilityBookCalendarView::OnCtrlChar(wxKeyEvent& evt)
{
	if (!m_Ctrl)
		return;
	ARBDate date = m_Ctrl->CurrentDate();
	if (!date.IsValid())
		return;
	CalendarMotion motion = eMotionNone;
	switch (evt.GetKeyCode())
	{
	default:
		evt.Skip();
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
	case WXK_RETURN:
	case WXK_NUMPAD_ENTER:
		m_Ctrl->OnEdit(GetDocument());
		break;
	case WXK_LEFT:
	case WXK_NUMPAD_LEFT:
		motion = eMotionDayLast;
		break;
	case WXK_RIGHT:
	case WXK_NUMPAD_RIGHT:
		motion = eMotionDayNext;
		break;
	case WXK_UP:
	case WXK_NUMPAD_UP:
#if defined(__WXMAC__)
		motion = (wxMOD_CMD == evt.GetModifiers()) ? eMotionCalStart : eMotionWeekLast;
#else
		motion = eMotionWeekLast;
#endif
		break;
	case WXK_DOWN:
	case WXK_NUMPAD_DOWN:
#if defined(__WXMAC__)
		motion = (wxMOD_CMD == evt.GetModifiers()) ? eMotionCalEnd : eMotionWeekNext;
#else
		motion = eMotionWeekNext;
#endif
		break;
	case WXK_PAGEUP:
		// Note: The mac synthesizes PAGEUP with fn+<up>
		motion = eMotionMonthLast;
		break;
	case WXK_PAGEDOWN:
		// Note: The mac synthesizes PAGEDOWN with fn+<dn>
		motion = eMotionMonthNext;
		break;
	case WXK_HOME:
		// Note: The mac synthesizes HOME with fn+<left>,
		// But don't support fn+control+left as ctrl+home since ctrl+a is also
		// synthesized into HOME - hence ctrl+a == ctrl+home which is wrong.
#if defined(__WXMAC__)
		motion = eMotionWeekStart;
#else
		motion = (wxMOD_CONTROL == evt.GetModifiers()) ? eMotionCalStart : eMotionWeekStart;
#endif
		break;
	case WXK_END:
		// Note: The mac synthesizes END with fn+<right>
#if defined(__WXMAC__)
		motion = eMotionWeekEnd;
#else
		motion = (wxMOD_CONTROL == evt.GetModifiers()) ? eMotionCalEnd : eMotionWeekEnd;
#endif
		break;
	}
	switch (motion)
	{
	default:
		break;
	case eMotionDayLast:
		--date;
		break;
	case eMotionDayNext:
		++date;
		break;
	case eMotionWeekLast:
		date -= 7;
		break;
	case eMotionWeekNext:
		date += 7;
		break;
	case eMotionMonthLast:
		if (1 == date.GetMonth())
		{
			date.SetDate(date.GetYear() - 1, 12, date.GetDay());
		}
		else
		{
			// If we failed, it's probably because of end-of-month differences (28/30/31)
			// (Jan & Dec = 31, so no need to deal with above)
			if (!date.SetDate(date.GetYear(), date.GetMonth() - 1, date.GetDay(), false))
			{
				date.SetDate(date.GetYear(), date.GetMonth(), 1);
				--date;
			}
		}
		break;
	case eMotionMonthNext:
		if (12 == date.GetMonth())
		{
			date.SetDate(date.GetYear() + 1, 1, date.GetDay());
		}
		else
		{
			// If we failed, it's probably because of end-of-month differences (28/30/31)
			// (Jan & Dec = 31, so no need to deal with above)
			if (!date.SetDate(date.GetYear(), date.GetMonth() + 1, date.GetDay(), false))
			{
				date.SetDate(date.GetYear(), date.GetMonth() + 2, 1);
				--date;
			}
		}
		break;
	case eMotionWeekStart:
		date = m_Ctrl->FirstDayOfWeek(date);
		break;
	case eMotionWeekEnd:
		date = m_Ctrl->LastDayOfWeek(date);
		break;
	case eMotionCalStart:
		date = m_Ctrl->FirstDate();
		break;
	case eMotionCalEnd:
		date = m_Ctrl->LastDate();
		break;
	}
	if (date != m_Ctrl->CurrentDate())
	{
		m_Ctrl->SetCurrentDate(date, true);
	}
}


void CAgilityBookCalendarView::OnViewUpdateCmd(wxUpdateUIEvent& evt)
{
	bool bEnable = false;
	if (m_Ctrl)
	{
		switch (evt.GetId())
		{
		case wxID_DUPLICATE:
		case wxID_CUT:
			break;
		case wxID_COPY:
			bEnable = m_Ctrl->CanCopy();
			break;
		case wxID_PASTE:
		case wxID_FIND:
		case ID_EDIT_FIND_NEXT:
		case ID_EDIT_FIND_PREVIOUS:
		case wxID_SELECTALL:
			break;
		case ID_AGILITY_EDIT_CALENDAR:
			bEnable = m_Ctrl->CanEdit();
			break;
		case wxID_DELETE:
		case ID_AGILITY_DELETE_CALENDAR:
			bEnable = m_Ctrl->CanDelete();
			break;
		case ID_AGILITY_EXPORT_CALENDAR:
			break;
		case ID_AGILITY_CREATEENTRY_CALENDAR:
			bEnable = m_Ctrl->CanCreateEntry(GetDocument());
			break;
		case ID_VIEW_CUSTOMIZE:
			bEnable = true;
			break;
		}
	}
	evt.Enable(bEnable);
}


void CAgilityBookCalendarView::OnViewCmd(wxCommandEvent& evt)
{
	if (!m_Ctrl)
		return;
	switch (evt.GetId())
	{
	case wxID_COPY:
		m_Ctrl->OnCopy();
		break;
	case ID_AGILITY_EDIT_CALENDAR:
		m_Ctrl->OnEdit(GetDocument());
		break;
	case wxID_DELETE:
	case ID_AGILITY_DELETE_CALENDAR:
		m_Ctrl->OnDelete(GetDocument());
		break;
	case ID_AGILITY_CREATEENTRY_CALENDAR:
		m_Ctrl->OnCreateEntry(GetDocument());
		break;
	case ID_VIEW_CUSTOMIZE:
		{
			CDlgAssignColumns dlg(CAgilityBookOptions::eView, m_Ctrl, GetDocument(), IO_TYPE_VIEW_CALENDAR);
			dlg.ShowModal();
		}
		break;
	}
}
