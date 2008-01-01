/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgCalendar class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-13 DRC Added direct access to Notes dialog.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-21 DRC Added Location/Club info fields.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgCalendar.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBCalendar.h"
#include "ARBConfig.h"
#include "DlgInfoJudge.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCalendar dialog

CDlgCalendar::CDlgCalendar(
		ARBCalendarPtr pCal,
		CAgilityBookDoc* pDoc,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgCalendar::IDD, pParent)
	, m_ctrlLocation(false)
	, m_ctrlVenue(false)
	, m_ctrlClub(false)
	, m_pCal(pCal)
	, m_pDoc(pDoc)
	, m_Span(0)
{
	//{{AFX_DATA_INIT(CDlgCalendar)
	m_dateStart = CTime::GetCurrentTime();
	m_dateEnd = CTime::GetCurrentTime() + CTimeSpan(1, 0, 0, 0);
	m_bTentative = m_pCal->IsTentative() ? TRUE : FALSE;
	m_dateOpens = CTime::GetCurrentTime();
	m_bOpeningUnknown = TRUE;
	m_dateDraws = CTime::GetCurrentTime();
	m_bDrawingUnknown = TRUE;
	m_dateCloses = CTime::GetCurrentTime();
	m_bClosingUnknown = TRUE;
	m_Entered = 0;
	m_OnlineUrl = m_pCal->GetOnlineURL().c_str();
	m_Accommodation = 0;
	m_PremiumUrl = m_pCal->GetPremiumURL().c_str();
	m_EMailSecAddr = m_pCal->GetSecEmail().c_str();
	m_Venue = m_pCal->GetVenue().c_str();
	m_Club = m_pCal->GetClub().c_str();
	m_Location = m_pCal->GetLocation().c_str();
	m_Notes = m_pCal->GetNote().c_str();
	//}}AFX_DATA_INIT
	if (m_pCal->GetStartDate().IsValid())
	{
		m_dateStart = CTime(m_pCal->GetStartDate().GetDate());
		if (m_pCal->GetEndDate().IsValid())
			m_Span = m_pCal->GetEndDate() - m_pCal->GetStartDate();
	}
	if (m_pCal->GetEndDate().IsValid())
		m_dateEnd = CTime(m_pCal->GetEndDate().GetDate());
	if (m_pCal->GetOpeningDate().IsValid())
	{
		m_dateOpens = CTime(m_pCal->GetOpeningDate().GetDate());
		m_bOpeningUnknown = FALSE;
	}
	if (m_pCal->GetDrawDate().IsValid())
	{
		m_dateDraws = CTime(m_pCal->GetDrawDate().GetDate());
		m_bDrawingUnknown = FALSE;
	}
	if (m_pCal->GetClosingDate().IsValid())
	{
		m_dateCloses = CTime(m_pCal->GetClosingDate().GetDate());
		m_bClosingUnknown = FALSE;
	}
	switch (m_pCal->GetEntered())
	{
	default:
		break;
	case ARBCalendar::ePlanning:
		m_Entered = 1;
		break;
	case ARBCalendar::eEntered:
		m_Entered = 2;
		break;
	}
	switch (m_pCal->GetAccommodation())
	{
	default:
		break;
	case ARBCalendar::eAccomTodo:
		m_Accommodation = 1;
		break;
	case ARBCalendar::eAccomConfirmed:
		m_Accommodation = 2;
		m_Confirmation = m_pCal->GetConfirmation().c_str();
		break;
	}
	m_Notes.Replace(_T("\n"), _T("\r\n"));
}


void CDlgCalendar::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCalendar)
	DDX_DateTimeCtrl(pDX, IDC_CAL_DATE_START, m_dateStart);
	DDX_DateTimeCtrl(pDX, IDC_CAL_DATE_END, m_dateEnd);
	DDX_Check(pDX, IDC_CAL_TENTATIVE, m_bTentative);
	DDX_DateTimeCtrl(pDX, IDC_CAL_DATE_OPENS, m_dateOpens);
	DDX_Check(pDX, IDC_CAL_DATE_OPENS_UNKNOWN, m_bOpeningUnknown);
	DDX_DateTimeCtrl(pDX, IDC_CAL_DATE_DRAWS, m_dateDraws);
	DDX_Check(pDX, IDC_CAL_DATE_DRAWS_UNKNOWN, m_bDrawingUnknown);
	DDX_DateTimeCtrl(pDX, IDC_CAL_DATE_CLOSES, m_dateCloses);
	DDX_Check(pDX, IDC_CAL_DATE_CLOSES_UNKNOWN, m_bClosingUnknown);
	DDX_Radio(pDX, IDC_CAL_ENTER_NOT, m_Entered);
	DDX_Control(pDX, IDC_CAL_ONLINE_ENTRY, m_ctrlOnlineUrlEntry);
	DDX_Text(pDX, IDC_CAL_ONLINE_URL, m_OnlineUrl);
	DDX_Control(pDX, IDC_CAL_ONLINE_URL, m_ctrlOnlineUrl);
	DDX_Radio(pDX, IDC_CAL_ACCOM_NONE, m_Accommodation);
	DDX_Text(pDX, IDC_CAL_ACCOM_CONFIRMATION, m_Confirmation);
	DDX_Control(pDX, IDC_CAL_ACCOM_CONFIRMATION, m_ctrlConfirmation);
	DDX_Control(pDX, IDC_CAL_PREMIUM_ENTRY, m_ctrlPremiumEntry);
	DDX_Text(pDX, IDC_CAL_PREMIUM_URL, m_PremiumUrl);
	DDX_Control(pDX, IDC_CAL_PREMIUM_URL, m_ctrlPremiumUrl);
	DDX_Control(pDX, IDC_CAL_EMAIL_SEC, m_ctrlEMailSec);
	DDX_Text(pDX, IDC_CAL_EMAIL_SEC_ADDR, m_EMailSecAddr);
	DDX_Control(pDX, IDC_CAL_EMAIL_SEC_ADDR, m_ctrlEMailSecAddr);
	DDX_Control(pDX, IDC_CAL_VENUE, m_ctrlVenue);
	DDX_CBString(pDX, IDC_CAL_VENUE, m_Venue);
	DDX_Control(pDX, IDC_CAL_CLUB, m_ctrlClub);
	DDX_CBString(pDX, IDC_CAL_CLUB, m_Club);
	DDX_Control(pDX, IDC_CAL_CLUB_NOTES, m_ctrlClubNotes);
	DDX_Control(pDX, IDC_CAL_CLUB_NOTE, m_ctrlClubInfo);
	DDX_Control(pDX, IDC_CAL_LOCATION, m_ctrlLocation);
	DDX_CBString(pDX, IDC_CAL_LOCATION, m_Location);
	DDX_Control(pDX, IDC_CAL_LOCATION_NOTES, m_ctrlLocationNotes);
	DDX_Control(pDX, IDC_CAL_LOCATION_NOTE, m_ctrlLocationInfo);
	DDX_Text(pDX, IDC_CAL_NOTES, m_Notes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCalendar, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgCalendar)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_CAL_DATE_START, OnDatetimechangeStart)
	ON_BN_CLICKED(IDC_CAL_DATE_OPENS_UNKNOWN, OnDateOpensUnknown)
	ON_BN_CLICKED(IDC_CAL_DATE_DRAWS_UNKNOWN, OnDateDrawsUnknown)
	ON_BN_CLICKED(IDC_CAL_DATE_CLOSES_UNKNOWN, OnDateClosesUnknown)
	ON_BN_CLICKED(IDC_CAL_ENTER_NOT, OnCalEntry)
	ON_BN_CLICKED(IDC_CAL_ENTER_PLANNING, OnCalEntry)
	ON_BN_CLICKED(IDC_CAL_ENTER_ENTERED, OnCalEntry)
	ON_EN_CHANGE(IDC_CAL_ONLINE_URL, OnEnChangeCalOnlineUrl)
	ON_BN_CLICKED(IDC_CAL_ONLINE_ENTRY, OnOnlineEntry)
	ON_EN_CHANGE(IDC_CAL_PREMIUM_URL, OnEnChangeCalPremiumUrl)
	ON_BN_CLICKED(IDC_CAL_PREMIUM_ENTRY, OnPremiumEntry)
	ON_EN_CHANGE(IDC_CAL_EMAIL_SEC_ADDR, OnEnChangeCalEmailSecAddr)
	ON_BN_CLICKED(IDC_CAL_EMAIL_SEC, OnEmailSec)
	ON_BN_CLICKED(IDC_CAL_ACCOM_NONE, OnAccommodation)
	ON_BN_CLICKED(IDC_CAL_ACCOM_NEEDED, OnAccommodation)
	ON_BN_CLICKED(IDC_CAL_ACCOM_MADE, OnAccommodation)
	ON_CBN_SELCHANGE(IDC_CAL_CLUB, OnSelchangeClub)
	ON_CBN_KILLFOCUS(IDC_CAL_CLUB, OnKillfocusClub)
	ON_BN_CLICKED(IDC_CAL_CLUB_NOTES, OnClubNotes)
	ON_CBN_SELCHANGE(IDC_CAL_LOCATION, OnSelchangeLocation)
	ON_CBN_KILLFOCUS(IDC_CAL_LOCATION, OnKillfocusLocation)
	ON_BN_CLICKED(IDC_CAL_LOCATION_NOTES, OnLocationNotes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgCalendar::UpdateLocationInfo(TCHAR const* pLocation)
{
	CString str;
	if (pLocation && *pLocation)
	{
		ARBInfoItemPtr pItem;
		if (m_pDoc->GetInfo().GetInfo(ARBInfo::eLocationInfo).FindItem(pLocation, &pItem))
		{
			str = pItem->GetComment().c_str();
			str.Replace(_T("\n"), _T("\r\n"));
		}
	}
	m_ctrlLocationInfo.SetWindowText(str);
}


void CDlgCalendar::ListLocations()
{
	set<tstring> locations;
	m_pDoc->GetAllTrialLocations(locations);
	tstring loc((LPCTSTR)m_Location);
	if (m_Location.IsEmpty())
		loc = m_pCal->GetLocation();
	m_ctrlLocation.ResetContent();
	for (set<tstring>::const_iterator iter = locations.begin(); iter != locations.end(); ++iter)
	{
		int index = m_ctrlLocation.AddString((*iter).c_str());
		if ((*iter) == loc)
		{
			m_ctrlLocation.SetCurSel(index);
			UpdateLocationInfo((*iter).c_str());
		}
	}
}


void CDlgCalendar::UpdateClubInfo(TCHAR const* pClub)
{
	CString str;
	if (pClub && *pClub)
	{
		ARBInfoItemPtr pItem;
		if (m_pDoc->GetInfo().GetInfo(ARBInfo::eClubInfo).FindItem(pClub, &pItem))
		{
			str = pItem->GetComment().c_str();
			str.Replace(_T("\n"), _T("\r\n"));
		}
	}
	m_ctrlClubInfo.SetWindowText(str);
}


void CDlgCalendar::ListClubs()
{
	set<tstring> clubs;
	m_pDoc->GetAllClubNames(clubs);
	tstring club((LPCTSTR)m_Club);
	if (m_Club.IsEmpty())
		club = m_pCal->GetClub();
	m_ctrlClub.ResetContent();
	for (set<tstring>::const_iterator iter = clubs.begin(); iter != clubs.end(); ++iter)
	{
		int index = m_ctrlClub.AddString((*iter).c_str());
		if ((*iter) == club)
		{
			m_ctrlClub.SetCurSel(index);
			UpdateClubInfo((*iter).c_str());
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgCalendar message handlers

BOOL CDlgCalendar::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	ListLocations();

	for (ARBConfigVenueList::const_iterator iterVenue = m_pDoc->GetConfig().GetVenues().begin();
		iterVenue != m_pDoc->GetConfig().GetVenues().end();
		++iterVenue)
	{
		ARBConfigVenuePtr pVenue = (*iterVenue);
		int index = m_ctrlVenue.AddString(pVenue->GetName().c_str());
		if (pVenue->GetName() == m_pCal->GetVenue())
			m_ctrlVenue.SetCurSel(index);
	}

	ListClubs();

	if (m_bOpeningUnknown)
		GetDlgItem(IDC_CAL_DATE_OPENS)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_CAL_DATE_OPENS)->EnableWindow(TRUE);
	if (m_bDrawingUnknown)
		GetDlgItem(IDC_CAL_DATE_DRAWS)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_CAL_DATE_DRAWS)->EnableWindow(TRUE);
	if (m_bClosingUnknown)
		GetDlgItem(IDC_CAL_DATE_CLOSES)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_CAL_DATE_CLOSES)->EnableWindow(TRUE);
	if (1 != m_Entered || m_OnlineUrl.IsEmpty())
		m_ctrlOnlineUrlEntry.EnableWindow(FALSE);
	if (m_PremiumUrl.IsEmpty())
		m_ctrlPremiumEntry.EnableWindow(FALSE);
	if (m_EMailSecAddr.IsEmpty())
		m_ctrlEMailSec.EnableWindow(FALSE);

	m_ctrlConfirmation.EnableWindow(2 == m_Accommodation);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgCalendar::OnDatetimechangeStart(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	if (0 < m_Span)
	{
		if (pDTChange->dwFlags == GDT_VALID)
		{
			UpdateData(TRUE);
			m_dateEnd = CTime(pDTChange->st) + CTimeSpan(m_Span, 0, 0, 0);
			UpdateData(FALSE);
		}
	}
	*pResult = 0;
}


void CDlgCalendar::OnDateOpensUnknown() 
{
	UpdateData(TRUE);
	if (m_bOpeningUnknown)
		GetDlgItem(IDC_CAL_DATE_OPENS)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_CAL_DATE_OPENS)->EnableWindow(TRUE);
}


void CDlgCalendar::OnDateDrawsUnknown() 
{
	UpdateData(TRUE);
	if (m_bDrawingUnknown)
		GetDlgItem(IDC_CAL_DATE_DRAWS)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_CAL_DATE_DRAWS)->EnableWindow(TRUE);
}


void CDlgCalendar::OnDateClosesUnknown() 
{
	UpdateData(TRUE);
	if (m_bClosingUnknown)
		GetDlgItem(IDC_CAL_DATE_CLOSES)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_CAL_DATE_CLOSES)->EnableWindow(TRUE);
}


void CDlgCalendar::OnCalEntry()
{
	UpdateData(TRUE);
	if (1 != m_Entered || m_OnlineUrl.IsEmpty())
		m_ctrlOnlineUrlEntry.EnableWindow(FALSE);
	else
		m_ctrlOnlineUrlEntry.EnableWindow(TRUE);
}


void CDlgCalendar::OnEnChangeCalOnlineUrl()
{
	CString s;
	m_ctrlOnlineUrl.GetWindowText(s);
	if (1 != m_Entered || s.IsEmpty())
		m_ctrlOnlineUrlEntry.EnableWindow(FALSE);
	else
		m_ctrlOnlineUrlEntry.EnableWindow(TRUE);
}


void CDlgCalendar::OnOnlineEntry()
{
	UpdateData(TRUE);
	RunCommand(m_OnlineUrl);
}


void CDlgCalendar::OnEnChangeCalPremiumUrl()
{
	CString s;
	m_ctrlPremiumUrl.GetWindowText(s);
	if (s.IsEmpty())
		m_ctrlPremiumEntry.EnableWindow(FALSE);
	else
		m_ctrlPremiumEntry.EnableWindow(TRUE);
}


void CDlgCalendar::OnPremiumEntry()
{
	UpdateData(TRUE);
	RunCommand(m_PremiumUrl);
}


void CDlgCalendar::OnEnChangeCalEmailSecAddr()
{
	CString s;
	m_ctrlEMailSecAddr.GetWindowText(s);
	if (s.IsEmpty())
		m_ctrlEMailSec.EnableWindow(FALSE);
	else
		m_ctrlEMailSec.EnableWindow(TRUE);
}


void CDlgCalendar::OnEmailSec()
{
	UpdateData(TRUE);
	CString s(m_EMailSecAddr);
	if (0 != m_EMailSecAddr.Find(_T("mailto:")))
		s = _T("mailto:") + m_EMailSecAddr;
	RunCommand(s);
}


void CDlgCalendar::OnAccommodation()
{
	UpdateData(TRUE);
	m_ctrlConfirmation.EnableWindow(2 == m_Accommodation);
}


void CDlgCalendar::OnSelchangeClub()
{
	int idx = m_ctrlClub.GetCurSel();
	CString str;
	if (CB_ERR != idx)
		m_ctrlClub.GetLBText(idx, str);
	UpdateClubInfo((LPCTSTR)str);
}


void CDlgCalendar::OnKillfocusClub()
{
	UpdateData(TRUE);
	UpdateClubInfo((LPCTSTR)m_Club);
}


void CDlgCalendar::OnClubNotes()
{
	UpdateData(TRUE);
	m_Club.TrimRight();
	m_Club.TrimLeft();
	CDlgInfoJudge dlg(m_pDoc, ARBInfo::eClubInfo, (LPCTSTR)m_Club, this);
	if (IDOK == dlg.DoModal())
	{
		ListClubs();
	}
}


void CDlgCalendar::OnSelchangeLocation()
{
	int idx = m_ctrlLocation.GetCurSel();
	CString str;
	if (CB_ERR != idx)
		m_ctrlLocation.GetLBText(idx, str);
	UpdateLocationInfo((LPCTSTR)str);
}


void CDlgCalendar::OnKillfocusLocation()
{
	UpdateData(TRUE);
	UpdateLocationInfo((LPCTSTR)m_Location);
}


void CDlgCalendar::OnLocationNotes()
{
	UpdateData(TRUE);
	m_Location.TrimRight();
	m_Location.TrimLeft();
	CDlgInfoJudge dlg(m_pDoc, ARBInfo::eLocationInfo, (LPCTSTR)m_Location, this);
	if (IDOK == dlg.DoModal())
	{
		ListLocations();
	}
}


void CDlgCalendar::OnOK()
{
	if (!UpdateData(TRUE))
		return;
	m_Venue.TrimRight();
	m_Venue.TrimLeft();
	m_Club.TrimRight();
	m_Club.TrimLeft();
	m_Location.TrimRight();
	m_Location.TrimLeft();
	m_Notes.TrimRight();
	m_Notes.TrimLeft();

	ARBDate startDate(m_dateStart.GetYear(), m_dateStart.GetMonth(), m_dateStart.GetDay());
	ARBDate endDate(m_dateEnd.GetYear(), m_dateEnd.GetMonth(), m_dateEnd.GetDay());
	if (startDate > endDate)
	{
		ARBDate temp = startDate;
		startDate = endDate;
		endDate = temp;
	}
	ARBDate openingDate(m_dateOpens.GetYear(), m_dateOpens.GetMonth(), m_dateOpens.GetDay());
	ARBDate drawingDate(m_dateDraws.GetYear(), m_dateDraws.GetMonth(), m_dateDraws.GetDay());
	ARBDate closingDate(m_dateCloses.GetYear(), m_dateCloses.GetMonth(), m_dateCloses.GetDay());
	if (!m_bOpeningUnknown && !m_bClosingUnknown && openingDate > closingDate)
	{
		ARBDate temp = openingDate;
		openingDate = closingDate;
		closingDate = temp;
	}
	if (!m_bDrawingUnknown)
	{
		if (!m_bOpeningUnknown && openingDate > drawingDate)
		{
			AfxMessageBox(IDS_BAD_DRAWDATE);
			GotoDlgCtrl(GetDlgItem(IDC_CAL_DATE_DRAWS));
			return;
		}
		if (!m_bClosingUnknown && drawingDate > closingDate)
		{
			AfxMessageBox(IDS_BAD_DRAWDATE);
			GotoDlgCtrl(GetDlgItem(IDC_CAL_DATE_DRAWS));
			return;
		}
	}
	ARBDate today(ARBDate::Today());
	today -= CAgilityBookOptions::DaysTillEntryIsPast();
	if (CAgilityBookOptions::AutoDeleteCalendarEntries() && endDate < today)
	{
		if (IDYES != AfxMessageBox(IDS_AUTODELETE_CAL, MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2))
			return;
	}

	m_pCal->SetStartDate(startDate);
	m_pCal->SetEndDate(endDate);
	switch (m_Entered)
	{
	default:
		m_pCal->SetEntered(ARBCalendar::eNot);
		break;
	case 1:
		m_pCal->SetEntered(ARBCalendar::ePlanning);
		break;
	case 2:
		m_pCal->SetEntered(ARBCalendar::eEntered);
		break;
	}
	m_pCal->SetIsTentative(m_bTentative ? true : false);
	m_pCal->SetLocation((LPCTSTR)m_Location);
	m_pCal->SetVenue((LPCTSTR)m_Venue);
	m_pCal->SetClub((LPCTSTR)m_Club);
	if (m_bOpeningUnknown)
		openingDate.clear();
	if (m_bDrawingUnknown)
		drawingDate.clear();
	if (m_bClosingUnknown)
		closingDate.clear();
	m_pCal->SetOpeningDate(openingDate);
	m_pCal->SetDrawDate(drawingDate);
	m_pCal->SetClosingDate(closingDate);
	m_pCal->SetOnlineURL((LPCTSTR)m_OnlineUrl);
	m_pCal->SetPremiumURL((LPCTSTR)m_PremiumUrl);
	m_pCal->SetSecEmail((LPCTSTR)m_EMailSecAddr);
	switch (m_Accommodation)
	{
	default:
		m_pCal->SetAccommodation(ARBCalendar::eAccomNone);
		break;
	case 1:
		m_pCal->SetAccommodation(ARBCalendar::eAccomTodo);
		break;
	case 2:
		m_pCal->SetAccommodation(ARBCalendar::eAccomConfirmed);
		m_pCal->SetConfirmation((LPCTSTR)m_Confirmation);
		break;
	}
	m_Notes.Replace(_T("\r\n"), _T("\n"));
	m_pCal->SetNote((LPCTSTR)m_Notes);

	CDlgBaseDialog::OnOK();
}
