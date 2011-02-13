/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CAgilityBookViewCalendar class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-14 DRC Removed wxScrollWindow - scroll wasn't used.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-10 DRC Ported to wxWidgets.
 * @li 2007-12-21 DRC Localize months/days (because of static link CRT)
 * @li 2007-01-13 DRC Fixed a problem with scroll bar not appearing.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2004-10-04 DRC Added div-by-0 tests.
 * @li 2004-09-28 DRC Set the page scrolling size to 4 weeks.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-11-22 DRC Update the list when creating an entry.
 * @li 2003-11-21 DRC Enabled copy and select all.
 * @li 2003-10-31 DRC Cleaned up pagedown scrolling.
 * @li 2003-10-29 DRC Support the mouse wheel for scrolling.
 * @li 2003-10-22 DRC Right click sets the current date.
 * @li 2003-08-27 DRC Cleaned up selection synchronization.
 * @li 2003-08-09 DRC When dbl-clicking on a date, make sure all entries are
 *                    visible - even if 'hide on entered' option is on.
 */

#include "stdafx.h"
#include "AgilityBookCalendarView.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookMenu.h"
#include "AgilityBookOptions.h"
#include "AgilityBookPanels.h"
#include "ARBCalendar.h"
#include "ClipBoard.h"
#include "DlgAssignColumns.h"
#include "DlgCalendar.h"
#include "DlgListCtrl.h"
#include "FilterOptions.h"
#include "MainFrm.h"
#include "TabView.h"
#include <wx/dcbuffer.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

#define WEEKS_PER_PAGE		6
#define DAY_TEXT_INSET		2


class CAgilityBookCalendar : public wxWindow
{
	DECLARE_CLASS(CAgilityBookCalendar)
public:
	CAgilityBookCalendar(
			CAgilityBookCalendarView* parentView,
			CBasePanel* parentOfView,
			wxWindow* parent);
	~CAgilityBookCalendar();

	//virtual wxSize DoGetBestSize() const	{return wxSize(100, 100);}

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


void CAgilityBookCalendar::OnDraw(wxDC* pDC)
{
	if (m_First.IsValid() && m_Last.IsValid())
	{
		// If it's not a wxBufferedPaintDC, then assume we're printing (preview or printer)
		bool bIsPrinting = wxDynamicCast(pDC, wxBufferedPaintDC) ? false : true;

		wxPen penNull(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW), 1, wxTRANSPARENT);
		wxPen penFrame(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT), 1);
		wxPen penShadow(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW), 1);
		wxPen penHilite(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHILIGHT), 1);
		wxBrush brNull(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW), wxTRANSPARENT);
		wxBrush brBtnFace(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
		wxBrush brHighlight(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
		wxBrush brMenu(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
		wxBrush brWorkspace(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));

		pDC->SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
		pDC->SetTextBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		pDC->SetBackgroundMode(wxTRANSPARENT);

		// Colors for current selected date
		CCalendarViewFilter filter = CFilterOptions::Options().FilterCalendarView();
		wxColour clrNotEntered = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorNotEntered);
		wxColour clrPlanning = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorPlanning);
		wxColour clrPending = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorPending);
		wxColour clrEntered = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorEntered);
		wxColour clrOpening = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorOpening);
		wxColour clrClosing = CAgilityBookOptions::CalendarColor(CAgilityBookOptions::eCalColorClosing);

		// Figure out which month we're on.
		ARBDate curMonth = FirstDayOfVisibleMonth();
		curMonth = LastDayOfWeek(curMonth);
		wxString str = wxString::Format(wxT("%s %d"), m_Months[curMonth.GetMonth()-1].c_str(), curMonth.GetYear());

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
			pDC->DrawLabel(str, rHeader, wxALIGN_TOP|wxALIGN_CENTRE_HORIZONTAL);
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
		ARBDate day = FirstDayOfWeek(curMonth);
		int iWeek;
		for (iWeek = 0; iWeek < WEEKS_PER_PAGE; ++iWeek)
		{
			rect.x = rCalendar.x + 1;
			rect.width = width - 1;
			rect.y = rCalendar.y + 1;
			rect.height = height - 1;
			rect.Offset(0, iWeek * height);
			rect.Inflate(-DAY_TEXT_INSET, -DAY_TEXT_INSET);
			for (iDay = 0; iDay < 7; ++iDay)
			{
				// Change background of non-current month days
				if (!bIsPrinting && m_Current == day)
				{
					wxRect r(rect);
					r.Inflate(DAY_TEXT_INSET + 1, DAY_TEXT_INSET + 1); // +1 for null pen
					if (wxWindow::DoFindFocus() == this)
						pDC->SetBrush(brHighlight);
					else
						pDC->SetBrush(brMenu);
					pDC->DrawRectangle(r);
				}
				else if (day.GetMonth() != curMonth.GetMonth())
				{
					wxRect r(rect);
					r.Inflate(DAY_TEXT_INSET + 1, DAY_TEXT_INSET + 1);
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
				str = wxString::Format(wxT("%d"), day.GetDay());
				{
					wxDCClipper clip(*pDC, rect);
					pDC->DrawLabel(str, rect, wxALIGN_TOP|wxALIGN_RIGHT);
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
								str += wxT(" ");
							switch (m_Columns[iCol])
							{
							case IO_CAL_LOCATION:
								if (pCal->GetLocation().empty())
									str += wxT("?");
								else
									str += pCal->GetLocation();
								break;
							case IO_CAL_CLUB:
								if (pCal->GetClub().empty())
									str += wxT("?");
								else
									str += pCal->GetClub();
								break;
							case IO_CAL_VENUE:
								if (pCal->GetVenue().empty())
									str += wxT("?");
								else
									str += pCal->GetVenue();
								break;
							case IO_CAL_NOTES:
								if (pCal->GetNote().empty())
									str += wxT("?");
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
						pDC->DrawLabel(str, r, wxALIGN_LEFT);
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
		if (m_nCurOffset + 1 < m_nMonths)
		{
			++m_nCurOffset;
			Refresh();
			Update();
		}
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

	static const wxChar* scColumns[] =
	{
		wxT("IDS_COL_START_DATE"),
		wxT("IDS_COL_END_DATE"),
		wxT("IDS_COL_VENUE"),
		wxT("IDS_COL_LOCATION"),
		wxT("IDS_COL_CLUB"),
		wxT("IDS_COL_OPENS"),
		wxT("IDS_COL_CLOSES"),
		wxT("IDS_COL_NOTES"),
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

	int index;
	size_t maxLen[scNumColumns];
	wxString columns[scNumColumns];
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
	// The header
	wxString data = wxString::Format(wxT(" %*s - %-*s %-*s %-*s %-*s %*s - %-*s %-*s"),
		maxLen[COL_START_DATE], columns[COL_START_DATE].c_str(),
		maxLen[COL_END_DATE], columns[COL_END_DATE].c_str(),
		maxLen[COL_VENUE], columns[COL_VENUE].c_str(),
		maxLen[COL_LOCATION], columns[COL_LOCATION].c_str(),
		maxLen[COL_CLUB], columns[COL_CLUB].c_str(),
		maxLen[COL_OPENS], columns[COL_OPENS].c_str(),
		maxLen[COL_CLOSES], columns[COL_CLOSES].c_str(),
		maxLen[COL_NOTES], columns[COL_NOTES].c_str());
	data.Trim();
	data += wxT("\n");

	// The data
	for (iter = m_Calendar.begin(); iter != m_Calendar.end(); ++iter)
	{
		ARBCalendarPtr cal = *iter;
		wxString items[scNumColumns];
		items[COL_START_DATE] = cal->GetStartDate().GetString();
		items[COL_END_DATE] = cal->GetEndDate().GetString();
		items[COL_LOCATION] = cal->GetLocation();
		items[COL_CLUB] = cal->GetClub();
		items[COL_VENUE] = cal->GetVenue();
		items[COL_OPENS] = cal->GetOpeningDate().GetString();
		items[COL_CLOSES] = cal->GetClosingDate().GetString();
		wxString tmp = cal->GetNote();
		tmp.Replace(wxT("\n"), wxT(" "));
		items[COL_NOTES] = tmp;
		wxString tentative(wxT("  "));
		if (cal->IsTentative())
			tentative = wxT("? ");
		wxString str = wxString::Format(wxT("%s%*s - %-*s %-*s %-*s %-*s %*s%s%-*s %-*s"),
			tentative.c_str(),
			maxLen[COL_START_DATE], items[COL_START_DATE].c_str(),
			maxLen[COL_END_DATE], items[COL_END_DATE].c_str(),
			maxLen[COL_VENUE], items[COL_VENUE].c_str(),
			maxLen[COL_LOCATION], items[COL_LOCATION].c_str(),
			maxLen[COL_CLUB], items[COL_CLUB].c_str(),
			maxLen[COL_OPENS], items[COL_OPENS].c_str(),
			(0 < items[COL_OPENS].length() || 0 < items[COL_CLOSES].length()) ? wxT(" - ") : wxT("   "),
			maxLen[COL_CLOSES], items[COL_CLOSES].c_str(),
			maxLen[COL_NOTES], items[COL_NOTES].c_str());
		str.Trim();
		data += str + wxT("\n");
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
	int width, height;
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
	int width, height;
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
	wxBufferedPaintDC dc(this);
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
	EVT_UPDATE_UI(ID_AGILITY_NEW_CALENDAR, CAgilityBookCalendarView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_NEW_CALENDAR, CAgilityBookCalendarView::OnViewCmd)
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
	, m_Ctrl(NULL)
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
	m_Ctrl->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(CAgilityBookCalendarView::OnCtrlMouseEvent), NULL, this);
	m_Ctrl->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(CAgilityBookCalendarView::OnCtrlMouseEvent), NULL, this);
	m_Ctrl->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(CAgilityBookCalendarView::OnCtrlMouseEvent), NULL, this);
#if wxUSE_MOUSEWHEEL
	m_Ctrl->Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(CAgilityBookCalendarView::OnCtrlMouseWheel), NULL, this);
#endif
	m_Ctrl->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(CAgilityBookCalendarView::OnCtrlKeyDown), NULL, this);
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
	m_Ctrl = NULL;
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


bool CAgilityBookCalendarView::GetMessage(wxString& msg) const
{
	if (!m_Ctrl)
		return false;
	msg = wxString::Format(_("IDS_NUM_EVENTS"), m_Ctrl->NumEvents());
	return true;
}


bool CAgilityBookCalendarView::GetMessage2(wxString& msg) const
{
	msg = _("IDS_INDICATOR_BLANK");
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
	if (!m_Ctrl)
		return;
	CUpdateHint* hint = NULL;
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
		wxMenu* menu = CreatePopup(IdMenuCalendar);
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
	if (evt.ButtonDown(wxMOUSE_BTN_LEFT) || evt.ButtonDown(wxMOUSE_BTN_RIGHT))
	{
		m_Ctrl->OnClick(evt.GetPosition());
	}
	else if (evt.ButtonDClick(wxMOUSE_BTN_LEFT))
	{
		m_Ctrl->OnEdit(GetDocument());
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


void CAgilityBookCalendarView::OnCtrlKeyDown(wxKeyEvent& evt)
{
	if (!m_Ctrl)
		return;
	ARBDate date = m_Ctrl->CurrentDate();
	if (!date.IsValid())
		return;
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
		--date;
		break;
	case WXK_RIGHT:
	case WXK_NUMPAD_RIGHT:
		++date;
		break;
	case WXK_UP:
	case WXK_NUMPAD_UP:
		date -= 7;
		break;
	case WXK_DOWN:
	case WXK_NUMPAD_DOWN:
		date += 7;
		break;
	case WXK_PAGEUP:
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
	case WXK_PAGEDOWN:
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
	case WXK_HOME:
		if (evt.CmdDown())
			date = m_Ctrl->FirstDate();
		else
			date = m_Ctrl->FirstDayOfWeek(date);
		break;
	case WXK_END:
		if (evt.CmdDown())
			date = m_Ctrl->LastDate();
		else
			date = m_Ctrl->LastDayOfWeek(date);
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
		case ID_AGILITY_NEW_CALENDAR:
			bEnable = true;
			break;
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
	case ID_AGILITY_NEW_CALENDAR:
		{
			ARBCalendarPtr cal(ARBCalendar::New());
			if (m_Ctrl->CurrentDate().IsValid())
			{
				cal->SetStartDate(m_Ctrl->CurrentDate());
				cal->SetEndDate(m_Ctrl->CurrentDate() + 1);
			}
			CDlgCalendar dlg(cal, GetDocument());
			if (wxID_OK == dlg.ShowModal())
			{
				if (!(CAgilityBookOptions::AutoDeleteCalendarEntries() && cal->GetEndDate() < ARBDate::Today()))
				{
					m_Ctrl->SetCurrentDate(cal->GetStartDate(), false);
					GetDocument()->Book().GetCalendar().AddCalendar(cal);
					GetDocument()->Book().GetCalendar().sort();
					LoadData();
					GetDocument()->Modify(true);
					CUpdateHint hint(UPDATE_CALENDAR_VIEW);
					GetDocument()->UpdateAllViews(this, &hint);
				}
			}
		}
		break;
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
