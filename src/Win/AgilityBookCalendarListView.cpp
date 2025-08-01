/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CAgilityBookCalendarListView class
 * @author David Connet
 *
 * Revision History
 * 2015-04-22 Specifically use std::abs, on mac it used abs(int).
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-04 Ported to wxWidgets.
 * 2006-05-29 Sync cal view when item date changes.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-27 Add color coding to calendar for entries that need attention.
 * 2005-01-25 Remember the sort column between program invocations.
 * 2004-12-31 Make F1 invoke context help.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-06-24 Added a sort header image.
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-06-06 Added cut/copy/paste support.
 * 2004-04-15 Added Duplicate menu item.
 * 2004-04-06 Added simple sorting by column.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-12-27 Implemented Find/FindNext.
 * 2003-11-22 Update the view when creating or editing an entry.
 * 2003-11-21 Enabled consistent copy and select all (old copy moved).
 * 2003-08-30 Fixed bug when copying list to clipboard.
 * 2003-08-27 Cleaned up selection synchronization.
 * 2003-07-24 Calendar view didn't update when item deleted from list.
 */

#include "stdafx.h"
#include "AgilityBookCalendarListView.h"

#include "AgilityBook.h"
#include "AgilityBookCalendarView.h"
#include "AgilityBookDoc.h"
#include "AgilityBookMenu.h"
#include "AgilityBookPanels.h"
#include "ClipBoard.h"
#include "DlgAssignColumns.h"
#include "DlgCalendar.h"
#include "FilterOptions.h"
#include "ImageHelper.h"
#include "MainFrm.h"
#include "Print.h"
#include "RegItems.h"
#include "Wizard.h"

#include "ARB/ARBCalendar.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgFind.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/Logger.h"
#include <wx/config.h>
#include <wx/mstream.h>
#include <wx/settings.h>
#include <sstream>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookCalendarListViewData

class CAgilityBookCalendarListViewData : public CListData
{
public:
	CAgilityBookCalendarListViewData(CAgilityBookCalendarListView* pView, ARBCalendarPtr const& inCal)
		: m_pView(pView)
		, m_pCal(inCal)
	{
	}

	bool CanEdit() const
	{
		return true;
	}
	bool CanDelete() const
	{
		return true;
	}

	ARBCalendarPtr GetCalendar()
	{
		return m_pCal;
	}
	wxString OnNeedText(long iCol) const override;
	void OnNeedListItem(long iCol, wxListItem& info) const override;

private:
	bool HighlightOpeningNear(long iCol) const;
	bool HighlightClosingNear(long iCol) const;
	CAgilityBookCalendarListView* m_pView;
	ARBCalendarPtr m_pCal;
};


wxString CAgilityBookCalendarListViewData::OnNeedText(long iCol) const
{
	wxString str;
	if (m_pCal)
	{
		switch (m_pView->m_Columns[iCol])
		{
		case IO_CAL_START_DATE:
			str = m_pCal->GetStartDate().GetString();
			break;
		case IO_CAL_END_DATE:
			str = m_pCal->GetEndDate().GetString();
			break;
		case IO_CAL_LOCATION:
			str = m_pCal->GetLocation();
			break;
		case IO_CAL_CLUB:
			str = m_pCal->GetClub();
			break;
		case IO_CAL_VENUE:
			str = m_pCal->GetVenue();
			break;
		case IO_CAL_OPENS:
			if (m_pCal->GetOpeningDate().IsValid())
				str = m_pCal->GetOpeningDate().GetString();
			break;
		case IO_CAL_DRAWS:
			if (m_pCal->GetDrawDate().IsValid())
				str = m_pCal->GetDrawDate().GetString();
			break;
		case IO_CAL_CLOSES:
			if (m_pCal->GetClosingDate().IsValid())
				str = m_pCal->GetClosingDate().GetString();
			break;
		case IO_CAL_NOTES:
			str = StringUtil::Replace(m_pCal->GetNote(), L"\n", L" ");
			break;
		default:
			break;
		}
	}
	return str;
}


void CAgilityBookCalendarListViewData::OnNeedListItem(long iCol, wxListItem& info) const
{
	if (m_pCal)
	{
		info.SetMask(info.GetMask() | wxLIST_MASK_TEXT);
		info.SetText(OnNeedText(iCol));
		int idxImage = -1;
		if (IO_CAL_START_DATE == m_pView->m_Columns[iCol])
		{
			switch (m_pCal->GetEntered())
			{
			case ARBCalendarEntry::Not:
				if (m_pCal->IsTentative())
					idxImage = m_pView->m_imgTentative;
				else
					idxImage = m_pView->m_Ctrl->ImageEmpty();
				break;
			case ARBCalendarEntry::Planning:
				if (m_pCal->IsTentative())
					idxImage = m_pView->m_imgPlanTentative;
				else
					idxImage = m_pView->m_imgPlan;
				break;
			case ARBCalendarEntry::Pending:
				if (m_pCal->IsTentative())
					idxImage = m_pView->m_imgPendingTentative;
				else
					idxImage = m_pView->m_imgPending;
				break;
			case ARBCalendarEntry::Entered:
				if (m_pCal->IsTentative())
					idxImage = m_pView->m_imgEnteredTentative;
				else
					idxImage = m_pView->m_imgEntered;
				break;
			}
		}
		else if (IO_CAL_LOCATION == m_pView->m_Columns[iCol])
		{
			if (ARBCalendarEntry::Not == m_pCal->GetEntered())
				idxImage = m_pView->m_Ctrl->ImageEmpty();
			else
			{
				switch (m_pCal->GetAccommodation())
				{
				case ARBAccommodations::None:
					idxImage = m_pView->m_imgAccomNone;
					break;
				case ARBAccommodations::Todo:
					idxImage = m_pView->m_imgAccomTodo;
					break;
				case ARBAccommodations::Confirmed:
					idxImage = m_pView->m_imgAccomConfirm;
					break;
				}
			}
		}
		if (0 <= idxImage)
		{
			info.SetMask(info.GetMask() | wxLIST_MASK_IMAGE);
			info.SetImage(idxImage);
		}
		if (HighlightClosingNear(iCol))
		{
			info.SetTextColour(CAgilityBookOptions::CalendarColor(ARBCalColorItem::ClosingNear));
			info.SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		}
		else if (HighlightOpeningNear(iCol))
		{
			info.SetTextColour(CAgilityBookOptions::CalendarColor(ARBCalColorItem::OpeningNear));
			info.SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		}
	}
}


bool CAgilityBookCalendarListViewData::HighlightOpeningNear(long iCol) const
{
	bool bHighlight = false;
	int nearDays = CAgilityBookOptions::CalendarOpeningNear();
	if (0 <= iCol && 0 <= nearDays && ARBCalendarEntry::Planning == m_pCal->GetEntered()
		&& m_pCal->GetOpeningDate().IsValid())
	{
		ARBDate today = ARBDate::Today();
		// If 'interval' is less than 0, then the date has passed.
		long interval = m_pCal->GetOpeningDate() - today;
		if (interval <= nearDays)
		{
			bHighlight = true;
			if (0 > interval && m_pCal->GetClosingDate().IsValid() && m_pCal->GetClosingDate() < today)
				bHighlight = false;
		}
	}
	return bHighlight;
}


bool CAgilityBookCalendarListViewData::HighlightClosingNear(long iCol) const
{
	bool bHighlight = false;
	int nearDays = CAgilityBookOptions::CalendarClosingNear();
	if (0 <= iCol && 0 <= nearDays && ARBCalendarEntry::Planning == m_pCal->GetEntered()
		&& m_pCal->GetClosingDate().IsValid())
	{
		// If 'interval' is less than 0, then the date has passed.
		long interval = m_pCal->GetClosingDate() - ARBDate::Today();
		if (interval >= 0 && interval <= nearDays)
		{
			bHighlight = true;
		}
	}
	return bHighlight;
}

/////////////////////////////////////////////////////////////////////////////
// List sorting

CAgilityBookCalendarListView::CSortColumn::CSortColumn(std::vector<long>& inColumns)
	: m_Columns(inColumns)
	, m_iCol(1)
{
}


void CAgilityBookCalendarListView::CSortColumn::Initialize()
{
	long realCol = IO_CAL_START_DATE;
	realCol = wxConfig::Get()->Read(CFG_SORTING_CALENDAR, realCol);
	long neg = 1;
	if (0 > realCol)
	{
		neg = -1;
		realCol *= -1;
	}
	long col = LookupColumn(realCol);
	if (0 > m_iCol)
		col = LookupColumn(IO_CAL_START_DATE);
	m_iCol = col * neg;
}


void CAgilityBookCalendarListView::CSortColumn::SetColumn(long iCol)
{
	m_iCol = iCol;
	if (0 == iCol)
		return;
	long neg = 1;
	long col = iCol;
	if (0 > iCol)
	{
		neg = -1;
		col = iCol * -1;
	}
	long realCol = m_Columns[col - 1] * neg;
	wxConfig::Get()->Write(CFG_SORTING_CALENDAR, realCol);
}


long CAgilityBookCalendarListView::CSortColumn::LookupColumn(long iCol) const
{
	size_t n = m_Columns.size();
	for (size_t i = 0; i < n; ++i)
	{
		if (m_Columns[i] == iCol)
		{
			return static_cast<long>(i + 1);
		}
	}
	return -1;
}


struct CalListSortInfo : public SortInfo
{
	CAgilityBookCalendarListView* pThis;

	CalListSortInfo(CAgilityBookCalendarListView* This, int nCol)
		: SortInfo(nCol)
		, pThis(This)
	{
	}
};

int wxCALLBACK CompareCalendar(CListDataPtr const& item1, CListDataPtr const& item2, SortInfo const* pSortInfo)
{
	CalListSortInfo const* pInfo = dynamic_cast<CalListSortInfo const*>(pSortInfo);
	assert(pInfo);

	if (0 == pInfo->nCol)
		return 0;
	CAgilityBookCalendarListViewDataPtr pItem1
		= std::dynamic_pointer_cast<CAgilityBookCalendarListViewData, CListData>(item1);
	CAgilityBookCalendarListViewDataPtr pItem2
		= std::dynamic_pointer_cast<CAgilityBookCalendarListViewData, CListData>(item2);
	int nRet = 0;
	int iCol = std::abs(pInfo->nCol);
	switch (pInfo->pThis->m_Columns[iCol - 1])
	{
	case IO_CAL_START_DATE:
		if (pItem1->GetCalendar()->GetStartDate() < pItem2->GetCalendar()->GetStartDate())
			nRet = -1;
		else if (pItem1->GetCalendar()->GetStartDate() > pItem2->GetCalendar()->GetStartDate())
			nRet = 1;
		break;
	case IO_CAL_END_DATE:
		if (pItem1->GetCalendar()->GetEndDate() < pItem2->GetCalendar()->GetEndDate())
			nRet = -1;
		else if (pItem1->GetCalendar()->GetEndDate() > pItem2->GetCalendar()->GetEndDate())
			nRet = 1;
		break;
	case IO_CAL_LOCATION:
		nRet = StringUtil::CompareNoCase(pItem1->GetCalendar()->GetLocation(), pItem2->GetCalendar()->GetLocation());
		break;
	case IO_CAL_CLUB:
		nRet = StringUtil::CompareNoCase(pItem1->GetCalendar()->GetClub(), pItem2->GetCalendar()->GetClub());
		break;
	case IO_CAL_VENUE:
		nRet = StringUtil::CompareNoCase(pItem1->GetCalendar()->GetVenue(), pItem2->GetCalendar()->GetVenue());
		break;
	case IO_CAL_OPENS:
	{
		bool bOk1 = pItem1->GetCalendar()->GetOpeningDate().IsValid();
		bool bOk2 = pItem2->GetCalendar()->GetOpeningDate().IsValid();
		if (bOk1 && bOk2)
		{
			if (pItem1->GetCalendar()->GetOpeningDate() < pItem2->GetCalendar()->GetOpeningDate())
				nRet = -1;
			else if (pItem1->GetCalendar()->GetOpeningDate() > pItem2->GetCalendar()->GetOpeningDate())
				nRet = 1;
		}
		else if (bOk1)
			nRet = 1;
		else if (bOk2)
			nRet = -1;
	}
	break;
	case IO_CAL_CLOSES:
	{
		bool bOk1 = pItem1->GetCalendar()->GetClosingDate().IsValid();
		bool bOk2 = pItem2->GetCalendar()->GetClosingDate().IsValid();
		if (bOk1 && bOk2)
		{
			if (pItem1->GetCalendar()->GetClosingDate() < pItem2->GetCalendar()->GetClosingDate())
				nRet = -1;
			else if (pItem1->GetCalendar()->GetClosingDate() > pItem2->GetCalendar()->GetClosingDate())
				nRet = 1;
		}
		else if (bOk1)
			nRet = 1;
		else if (bOk2)
			nRet = -1;
	}
	break;
	case IO_CAL_NOTES:
		nRet = StringUtil::CompareNoCase(pItem1->GetCalendar()->GetNote(), pItem2->GetCalendar()->GetNote());
		break;
	default:
		break;
	}
	if (0 > pInfo->nCol)
		nRet *= -1;
	return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// Find

CAgilityBookCalendarListView::CFindData::CFindData(CAgilityBookCalendarListView* pView)
	: m_pView(pView)
{
}


bool CAgilityBookCalendarListView::CFindData::Search(CDlgFind* pDlg)
{
	bool bFound = false;
	int inc = 1;
	if (!SearchDown())
		inc = -1;
	long index = m_pView->m_Ctrl->GetFirstSelected();
	if (0 <= index && index < m_pView->m_Ctrl->GetItemCount())
	{
		index += inc;
	}
	else if (0 > index && SearchDown())
		index = 0;
	else if (index >= m_pView->m_Ctrl->GetItemCount() && !SearchDown())
		index = m_pView->m_Ctrl->GetItemCount() - 1;
	for (; !bFound && 0 <= index && index < m_pView->m_Ctrl->GetItemCount(); index += inc)
	{
		std::set<wxString> strings;
		if (SearchAll())
		{
			CAgilityBookCalendarListViewDataPtr pData = m_pView->GetItemCalData(index);
			if (pData)
				pData->GetCalendar()->GetSearchStrings(strings);
		}
		else
		{
			long nColumns = m_pView->m_Ctrl->GetColumnCount();
			for (long i = 0; i < nColumns; ++i)
			{
				wxListItem info;
				info.SetId(index);
				info.SetMask(wxLIST_MASK_TEXT);
				info.SetColumn(i);
				m_pView->m_Ctrl->GetItem(info);
				strings.insert(info.GetText());
			}
		}
		for (auto const& str : strings)
		{
			if (Compare(str))
			{
				m_pView->m_Ctrl->SetSelection(index, true);
				bFound = true;
			}
		}
	}
	if (!bFound)
	{
		auto msg = wxString::Format(_("IDS_CANNOT_FIND"), m_search);
		wxMessageBox(msg, _("Agility Record Book"), wxOK | wxCENTRE | wxICON_INFORMATION);
	}
	return bFound;
}

/////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_CLASS(CAgilityBookCalendarListView, CAgilityBookBaseExtraView)


wxBEGIN_EVENT_TABLE(CAgilityBookCalendarListView, CAgilityBookBaseExtraView)
	EVT_CONTEXT_MENU(CAgilityBookCalendarListView::OnViewContextMenu)
	EVT_UPDATE_UI(wxID_DUPLICATE, CAgilityBookCalendarListView::OnViewUpdateCmd)
	EVT_MENU(wxID_DUPLICATE, CAgilityBookCalendarListView::OnViewCmd)
	EVT_UPDATE_UI(wxID_CUT, CAgilityBookCalendarListView::OnViewUpdateCmd)
	EVT_MENU(wxID_CUT, CAgilityBookCalendarListView::OnViewCmd)
	EVT_UPDATE_UI(wxID_COPY, CAgilityBookCalendarListView::OnViewUpdateCmd)
	EVT_MENU(wxID_COPY, CAgilityBookCalendarListView::OnViewCmd)
	EVT_UPDATE_UI(wxID_PASTE, CAgilityBookCalendarListView::OnViewUpdateCmd)
	EVT_MENU(wxID_PASTE, CAgilityBookCalendarListView::OnViewCmd)
	EVT_UPDATE_UI(wxID_DELETE, CAgilityBookCalendarListView::OnViewUpdateCmd)
	EVT_MENU(wxID_DELETE, CAgilityBookCalendarListView::OnViewCmd)
	EVT_UPDATE_UI(wxID_SELECTALL, CAgilityBookCalendarListView::OnViewUpdateCmd)
	EVT_MENU(wxID_SELECTALL, CAgilityBookCalendarListView::OnViewCmd)
	EVT_UPDATE_UI(wxID_FIND, CAgilityBookCalendarListView::OnViewUpdateCmd)
	EVT_MENU(wxID_FIND, CAgilityBookCalendarListView::OnViewCmd)
	EVT_UPDATE_UI(ID_EDIT_FIND_NEXT, CAgilityBookCalendarListView::OnViewUpdateCmd)
	EVT_MENU(ID_EDIT_FIND_NEXT, CAgilityBookCalendarListView::OnViewCmd)
	EVT_UPDATE_UI(ID_EDIT_FIND_PREVIOUS, CAgilityBookCalendarListView::OnViewUpdateCmd)
	EVT_MENU(ID_EDIT_FIND_PREVIOUS, CAgilityBookCalendarListView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_EDIT_CALENDAR, CAgilityBookCalendarListView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_EDIT_CALENDAR, CAgilityBookCalendarListView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_DELETE_CALENDAR, CAgilityBookCalendarListView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_DELETE_CALENDAR, CAgilityBookCalendarListView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_EXPORT_CALENDAR, CAgilityBookCalendarListView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_EXPORT_CALENDAR, CAgilityBookCalendarListView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_CREATEENTRY_CALENDAR, CAgilityBookCalendarListView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_CREATEENTRY_CALENDAR, CAgilityBookCalendarListView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_CUSTOMIZE, CAgilityBookCalendarListView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_CUSTOMIZE, CAgilityBookCalendarListView::OnViewCmd)
	EVT_MENU(wxID_PRINT, CAgilityBookCalendarListView::OnPrintView)
	EVT_MENU(wxID_PREVIEW, CAgilityBookCalendarListView::OnPreview)
wxEND_EVENT_TABLE()


#include "Platform/arbWarningPush.h"
CAgilityBookCalendarListView::CAgilityBookCalendarListView(CTabView* pTabView, wxDocument* doc)
	: CAgilityBookBaseExtraView(pTabView, doc)
	, m_Ctrl(nullptr)
	, m_imgTentative(-1)
	, m_imgPlan(-1)
	, m_imgPlanTentative(-1)
	, m_imgPending(-1)
	, m_imgPendingTentative(-1)
	, m_imgEntered(-1)
	, m_imgEnteredTentative(-1)
	, m_imgAccomNone(-1)
	, m_imgAccomTodo(-1)
	, m_imgAccomConfirm(-1)
	, m_bSuppressSelect(false)
	, m_Columns()
	, m_find(this)
	, m_SortColumn(m_Columns)
{
}
#include "Platform/arbWarningPop.h"


CAgilityBookCalendarListView::~CAgilityBookCalendarListView()
{
}


bool CAgilityBookCalendarListView::Create(
	CBasePanel* parentView,
	wxWindow* parentCtrl,
	wxDocument* doc,
	long flags,
	wxSizer* sizer,
	int proportion,
	int sizerFlags,
	int border)
{
	m_Ctrl = new CReportListCtrl(parentCtrl, false);
	m_Ctrl->Bind(wxEVT_SET_FOCUS, &CAgilityBookCalendarListView::OnCtrlSetFocus, this);
	m_Ctrl->Bind(wxEVT_COMMAND_LIST_COL_CLICK, &CAgilityBookCalendarListView::OnCtrlColumnClick, this);
	m_Ctrl->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, &CAgilityBookCalendarListView::OnCtrlItemSelected, this);
	m_Ctrl->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &CAgilityBookCalendarListView::OnCtrlItemActivated, this);
	m_Ctrl->Bind(wxEVT_KEY_DOWN, &CAgilityBookCalendarListView::OnCtrlKeyDown, this);
	m_imgTentative = m_Ctrl->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrQuestion));
	m_imgPlan = m_Ctrl->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrCheck));
	m_imgPlanTentative = m_Ctrl->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrCalPlanTentative));
	m_imgPending = m_Ctrl->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrCalPending));
	m_imgPendingTentative = m_Ctrl->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrCalPendingTentative));
	m_imgEntered = m_Ctrl->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrCalEntered));
	m_imgEnteredTentative = m_Ctrl->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrCalEnteredTentative));
	m_imgAccomNone = m_Ctrl->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrAccomNone));
	m_imgAccomTodo = m_Ctrl->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrAccomTodo));
	m_imgAccomConfirm = m_Ctrl->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrAccomConfirm));
#if defined(__WXMAC__)
	m_Ctrl->SetDropTarget(new CFileDropTarget(doc->GetDocumentManager()));
#endif
	return CAgilityBookBaseExtraView::Create(parentView, parentCtrl, doc, flags, sizer, proportion, sizerFlags, border);
}


bool CAgilityBookCalendarListView::PrevPane()
{
	if (m_parentPanel)
		return m_parentPanel->ActivateView(1);
	return false;
}


bool CAgilityBookCalendarListView::NextPane()
{
	if (m_parentPanel)
		return m_parentPanel->ActivateView(1);
	return false;
}


void CAgilityBookCalendarListView::DetachView()
{
	// The control is actually owned by the panel, the view is not.
	m_Ctrl = nullptr;
	m_imgTentative = -1;
	m_imgPlan = -1;
	m_imgPlanTentative = -1;
	m_imgPending = -1;
	m_imgPendingTentative = -1;
	m_imgEntered = -1;
	m_imgEnteredTentative = -1;
	m_imgAccomNone = -1;
	m_imgAccomTodo = -1;
	m_imgAccomConfirm = -1;
}


bool CAgilityBookCalendarListView::IsFiltered() const
{
	if (!CFilterOptions::Options().GetViewAllDates())
		return true;
	if (!CFilterOptions::Options().GetViewAllVenues())
		return true;
	CCalendarViewFilter filter = CFilterOptions::Options().FilterCalendarView();
	return filter.IsFiltered();
}


bool CAgilityBookCalendarListView::GetMessage(wxString& msg) const
{
	if (!m_Ctrl)
		return false;
	msg = wxString::Format(_("IDS_NUM_EVENTS"), m_Ctrl->GetItemCount());
	return true;
}


bool CAgilityBookCalendarListView::GetMessage2(wxString& msg) const
{
	msg = _("IDS_INDICATOR_BLANK");
	return true;
}


bool CAgilityBookCalendarListView::AllowStatusContext(int field) const
{
	return STATUS_FILTERED == field;
}


bool CAgilityBookCalendarListView::OnCreate(wxDocument* doc, long flags)
{
	SetupColumns();
	return true;
}


void CAgilityBookCalendarListView::DoActivateView(bool activate, wxView* activeView, wxView* deactiveView)
{
	if (m_Ctrl && activate && wxWindow::DoFindFocus() != m_Ctrl)
		m_Ctrl->SetFocus();
}


void CAgilityBookCalendarListView::OnDraw(wxDC* dc)
{
}


void CAgilityBookCalendarListView::OnUpdate(wxView* sender, wxObject* inHint)
{
	STACK_TRACE(stack, L"CAgilityBookCalendarListView::OnUpdate");

	CUpdateHint* hint = nullptr;
	if (inHint)
		hint = wxDynamicCast(inHint, CUpdateHint);
	if (!hint || hint->IsSet(UPDATE_CALENDAR_VIEW) || hint->IsEqual(UPDATE_OPTIONS))
	{
		m_Ctrl->UpdateAlternateRowColor(CAgilityBookOptions::UseAlternateRowColor());
		LoadData();
	}
	else if (hint && (hint->IsEqual(UPDATE_LANG_CHANGE) || hint->IsEqual(UPDATE_CUSTOMIZE)))
	{
		SetupColumns();
		LoadData();
	}
}


void CAgilityBookCalendarListView::GetPrintLine(long item, std::vector<wxString>& line) const
{
	if (m_Ctrl)
		m_Ctrl->GetPrintLine(item, line);
}


CAgilityBookCalendarListViewDataPtr CAgilityBookCalendarListView::GetItemCalData(long index) const
{
	if (!m_Ctrl)
		return CAgilityBookCalendarListViewDataPtr();
	return std::dynamic_pointer_cast<CAgilityBookCalendarListViewData, CListData>(m_Ctrl->GetData(index));
}


void CAgilityBookCalendarListView::SetupColumns()
{
	if (!m_Ctrl)
		return;
	int nColumnCount = m_Ctrl->GetColumnCount();
	for (int i = 0; i < nColumnCount; ++i)
		m_Ctrl->DeleteColumn(0);
	if (CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_CALENDAR_LIST, m_Columns))
	{
		for (size_t iCol = 0; iCol < m_Columns.size(); ++iCol)
		{
			wxString str = CDlgAssignColumns::GetNameFromColumnID(m_Columns[iCol]);
			int fmt = CDlgAssignColumns::GetFormatFromColumnID(m_Columns[iCol]);
			m_Ctrl->InsertColumn(static_cast<long>(iCol), str, fmt);
		}
		m_SortColumn.Initialize();
	}
}


void CAgilityBookCalendarListView::LoadData()
{
	if (!m_Ctrl)
		return;

	STACK_TRACE(stack, L"CAgilityBookCalendarListView::LoadData");

	m_bSuppressSelect = true;

	// Remember what's selected.
	ARBCalendarPtr pCurCal;
	CAgilityBookCalendarListViewDataPtr pCurData = GetItemCalData(m_Ctrl->GetFirstSelected());
	if (pCurData)
	{
		pCurCal = pCurData->GetCalendar();
		pCurData.reset();
	}

	// Reduce flicker
	m_Ctrl->Freeze();

	// Clear everything.
	m_Ctrl->DeleteAllItems();

	ARBDate today(ARBDate::Today());
	today -= CAgilityBookOptions::DaysTillEntryIsPast();
	bool bViewAll = CAgilityBookOptions::ViewAllCalendarEntries();
	bool bHide = CAgilityBookOptions::HideOverlappingCalendarEntries();

	// Add items.
	int i = 0;
	std::vector<ARBCalendarPtr> entered;
	if (bHide)
		GetDocument()->Book().GetCalendar().GetAllEntered(entered);
	CCalendarViewFilter filter = CFilterOptions::Options().FilterCalendarView();
	for (ARBCalendarList::iterator iter = GetDocument()->Book().GetCalendar().begin();
		 iter != GetDocument()->Book().GetCalendar().end();
		 ++iter)
	{
		ARBCalendarPtr pCal = (*iter);
		if (pCal->IsFiltered())
			continue;
		// Additional filtering
		if (!((ARBCalendarEntry::Not == pCal->GetEntered() && filter.ViewNotEntered())
			  || (ARBCalendarEntry::Planning == pCal->GetEntered() && filter.ViewPlanning())
			  || ((ARBCalendarEntry::Pending == pCal->GetEntered() || ARBCalendarEntry::Entered == pCal->GetEntered())
				  && filter.ViewEntered())))
			continue;
		if (!bViewAll)
		{
			if (pCal->IsBefore(today))
				continue;
		}
		if (bHide
			&& !(ARBCalendarEntry::Pending == pCal->GetEntered() || ARBCalendarEntry::Entered == pCal->GetEntered()))
		{
			bool bSuppress = false;
			for (std::vector<ARBCalendarPtr>::const_iterator iterE = entered.begin();
				 !bSuppress && iterE != entered.end();
				 ++iterE)
			{
				ARBCalendarPtr pEntered = (*iterE);
				if (pCal != pEntered && pCal->IsRangeOverlapped(pEntered->GetStartDate(), pEntered->GetEndDate()))
				{
					bSuppress = true;
				}
			}
			if (bSuppress)
				continue;
		}
		long index = m_Ctrl->InsertItem(std::make_shared<CAgilityBookCalendarListViewData>(this, pCal));
		// We may have modified the entry, so don't do a full equality test.
		// Just check the start/end date, location, club and venue. This allows
		// us to modify the opens/closes dates, notes and entry status.
		// Note: This is only important when editing the entry from the other
		// calendar view! If we edit locally, this is not a problem since we
		// just modified our own entry.
		if (pCurCal)
		{
			if (*pCurCal == *pCal
				|| (pCurCal->GetStartDate() == pCal->GetStartDate() && pCurCal->GetEndDate() == pCal->GetEndDate()
					&& pCurCal->GetLocation() == pCal->GetLocation() && pCurCal->GetClub() == pCal->GetClub()
					&& pCurCal->GetVenue() == pCal->GetVenue()))
			{
				m_Ctrl->Select(index, true);
			}
		}
		++i;
	}
	int nColumnCount = m_Ctrl->GetColumnCount();
	for (int iCol = 0; iCol < nColumnCount; ++iCol)
		m_Ctrl->SetColumnWidth(iCol, wxLIST_AUTOSIZE_USEHEADER);

	if (m_Ctrl->IsShownOnScreen())
		UpdateMessages();

	CalListSortInfo sortInfo(this, m_SortColumn.GetColumn());
	m_Ctrl->SortItems(CompareCalendar, &sortInfo);
	m_Ctrl->SetColumnSort(std::abs(m_SortColumn.GetColumn()) - 1, m_SortColumn.GetColumn());
	// Now make sure the selected item is visible.
	if (0 <= m_Ctrl->GetFirstSelected())
		m_Ctrl->Focus(m_Ctrl->GetFirstSelected());

	// Cleanup.
	m_Ctrl->Thaw();
	m_Ctrl->Refresh();

	m_bSuppressSelect = false;
}


void CAgilityBookCalendarListView::OnCtrlSetFocus(wxFocusEvent& evt)
{
	// We need this as clicking directly in the control does not change
	// the active view - we have to do it. Conversely, setting the active
	// view does not change the focused window, so we have to do that too.
	if (GetDocumentManager()->GetCurrentView() != this)
	{
		m_parentPanel->OnActivateView(this);
		Activate(true);
	}
	evt.Skip();
}


void CAgilityBookCalendarListView::OnCtrlColumnClick(wxListEvent& evt)
{
	m_Ctrl->SetColumnSort(std::abs(m_SortColumn.GetColumn()) - 1, 0);
	int nBackwards = 1;
	if (m_SortColumn.GetColumn() == evt.GetColumn() + 1)
		nBackwards = -1;
	m_SortColumn.SetColumn((evt.GetColumn() + 1) * nBackwards);
	CalListSortInfo sortInfo(this, m_SortColumn.GetColumn());
	m_Ctrl->SortItems(CompareCalendar, &sortInfo);
	m_Ctrl->SetColumnSort(std::abs(m_SortColumn.GetColumn()) - 1, m_SortColumn.GetColumn());
}


void CAgilityBookCalendarListView::OnCtrlItemSelected(wxListEvent& evt)
{
	if (m_Ctrl && !m_bSuppressSelect && 1 == m_Ctrl->GetSelectedItemCount())
	{
		CAgilityBookCalendarListViewDataPtr pData = GetItemCalData(evt.GetIndex());
		if (pData && pData->GetCalendar() && pData->GetCalendar()->GetStartDate().IsValid())
		{
			GetDocument()->SetCalenderDate(pData->GetCalendar()->GetStartDate());
		}
	}
}


void CAgilityBookCalendarListView::OnCtrlItemActivated(wxListEvent& evt)
{
	OnCmd(ID_AGILITY_EDIT_CALENDAR);
}


void CAgilityBookCalendarListView::OnCtrlKeyDown(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		evt.Skip();
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
	case WXK_RETURN:
	case WXK_NUMPAD_ENTER:
		OnCmd(ID_AGILITY_EDIT_CALENDAR);
		break;
	}
}


void CAgilityBookCalendarListView::OnViewContextMenu(wxContextMenuEvent& evt)
{
	CLogger::Log(L"MENU: CAgilityBookCalendarListView::OnViewContextMenu");
	if (!m_Ctrl)
		return;
	wxPoint point;
	if (GetMenuPosition(point, *m_Ctrl, evt))
	{
		wxMenu* menu = wxGetApp().GetMenus().CreatePopupMenu(m_Ctrl, IdMenuCalendar);
		m_Ctrl->PopupMenu(menu, point);
		delete menu;
	}
	else
		evt.Skip();
}


void CAgilityBookCalendarListView::OnViewUpdateCmd(wxUpdateUIEvent& evt)
{
	switch (evt.GetId())
	{
	case wxID_DUPLICATE:
		evt.Enable(0 < m_Ctrl->GetSelectedItemCount());
		break;
	case wxID_CUT:
		evt.Enable(0 < m_Ctrl->GetSelectedItemCount());
		break;
	case wxID_COPY:
		evt.Enable(0 < m_Ctrl->GetSelectedItemCount());
		break;
	case wxID_PASTE:
		evt.Enable(CClipboardDataReader::IsFormatAvailable(ARBClipFormat::Calendar));
		break;
	case wxID_FIND:
	case ID_EDIT_FIND_NEXT:
	case ID_EDIT_FIND_PREVIOUS:
		evt.Enable(true);
		break;
	case wxID_SELECTALL:
		evt.Enable(m_Ctrl->CanSelectAll());
		break;
	case ID_AGILITY_EDIT_CALENDAR:
		evt.Enable(1 == m_Ctrl->GetSelectedItemCount());
		break;
	case wxID_DELETE:
	case ID_AGILITY_DELETE_CALENDAR:
	{
		bool bEnable = false;
		std::vector<long> indices;
		if (0 < m_Ctrl->GetSelection(indices))
		{
			for (std::vector<long>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
			{
				CAgilityBookCalendarListViewDataPtr pData = GetItemCalData(*iter);
				if (pData && pData->CanDelete())
				{
					bEnable = true;
					break;
				}
			}
		}
		evt.Enable(bEnable);
	}
	break;
	case ID_AGILITY_EXPORT_CALENDAR:
		evt.Enable(0 < m_Ctrl->GetSelectedItemCount());
		break;
	case ID_AGILITY_CREATEENTRY_CALENDAR:
		evt.Enable(1 == m_Ctrl->GetSelectedItemCount());
		break;
	case ID_VIEW_CUSTOMIZE:
		evt.Enable(true);
		break;
	default:
		break;
	}
}


bool CAgilityBookCalendarListView::OnCmd(int id)
{
	if (!m_Ctrl)
		return false;
	bool bHandled = true;
	switch (id)
	{
	default:
		bHandled = false;
		break;

	case wxID_DUPLICATE:
	{
		std::vector<long> indices;
		if (0 < m_Ctrl->GetSelection(indices))
		{
			std::vector<CAgilityBookCalendarListViewDataPtr> items;
			for (std::vector<long>::iterator iterData = indices.begin(); iterData != indices.end(); ++iterData)
			{
				CAgilityBookCalendarListViewDataPtr pData = GetItemCalData(*iterData);
				if (pData)
					items.push_back(pData);
			}
			int nNewIsNotVisible = 0;
			for (std::vector<CAgilityBookCalendarListViewDataPtr>::iterator iter = items.begin(); iter != items.end();
				 ++iter)
			{
				// We need to warn the user if the duplicated entry is not visible.
				// This will happen if the source is marked as entered and they have
				// selected the option to hide dates.
				ARBCalendarPtr cal = (*iter)->GetCalendar()->Clone();
				if (((*iter)->GetCalendar()->GetEntered() == ARBCalendarEntry::Pending
					 || (*iter)->GetCalendar()->GetEntered() == ARBCalendarEntry::Entered)
					&& CAgilityBookOptions::HideOverlappingCalendarEntries())
				{
					++nNewIsNotVisible;
				}
				cal->SetEntered(ARBCalendarEntry::Not);
				GetDocument()->Book().GetCalendar().AddCalendar(cal);
			}
			GetDocument()->Book().GetCalendar().sort();
			LoadData();
			GetDocument()->Modify(true);
			CUpdateHint hint(UPDATE_CALENDAR_VIEW);
			GetDocument()->UpdateAllViews(this, &hint);
			if (0 < nNewIsNotVisible)
			{
				auto msg = wxString::Format(_("IDS_NOT_VISIBLE"), nNewIsNotVisible);
				wxMessageBox(msg, _("Agility Record Book"), wxOK | wxCENTRE | wxICON_WARNING);
			}
		}
	}
	break;

	case wxID_CUT:
		OnCmd(wxID_COPY);
		OnCmd(ID_AGILITY_DELETE_CALENDAR);
		break;

	case wxID_COPY:
	{
		std::vector<long> indices;
		if (0 < m_Ctrl->GetSelection(indices))
		{
			CClipboardDataWriter clpData;
			if (!clpData.isOkay())
				return true;

			wxString data;
			wxString html;
			CClipboardDataTable table(data, html);

			// Take care of the header, but only if more than one line is selected.
			if (1 < indices.size() || indices.size() == static_cast<size_t>(m_Ctrl->GetItemCount()))
			{
				std::vector<wxString> line;
				GetPrintLine(-1, line);
				table.StartLine();
				for (int i = 0; i < static_cast<int>(line.size()); ++i)
				{
					table.Cell(i, line[i]);
				}
				table.EndLine();
			}

			std::stringstream iCal;
			ElementNodePtr tree(ElementNode::New(CLIPDATA));

			// Now all the data.
			int nWarning = CAgilityBookOptions::CalendarOpeningNear();
			ICalendarPtr iCalendar = ICalendar::iCalendarBegin(iCal, 2);
			for (std::vector<long>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
			{
				CAgilityBookCalendarListViewDataPtr pData = GetItemCalData(*iter);
				if (pData)
				{
					pData->GetCalendar()->Save(tree);
					pData->GetCalendar()->iCalendar(iCalendar, nWarning);
				}
				std::vector<wxString> line;
				GetPrintLine((*iter), line);
				table.StartLine();
				for (int i = 0; i < static_cast<int>(line.size()); ++i)
				{
					table.Cell(i, line[i]);
				}
				table.EndLine();
			}
			iCalendar.reset();

			clpData.AddData(ARBClipFormat::Calendar, tree);
			clpData.AddData(table);
			clpData.AddData(ARBClipFormat::iCalendar, iCal.str());
			clpData.CommitData();
		}
	}
	break;

	case wxID_PASTE:
	{
		bool bLoaded = false;
		ElementNodePtr tree(ElementNode::New());
		CClipboardDataReader clpData;
		if (clpData.GetData(ARBClipFormat::Calendar, tree))
		{
			if (CLIPDATA == tree->GetName())
			{
				for (int i = 0; i < tree->GetElementCount(); ++i)
				{
					ElementNodePtr element = tree->GetElementNode(i);
					if (!element)
						continue;
					if (element->GetName() == TREE_CALENDAR)
					{
						ARBCalendarPtr pCal(ARBCalendar::New());
						CErrorCallback err;
						if (pCal->Load(element, ARBAgilityRecordBook::GetCurrentDocVersion(), err))
						{
							bLoaded = true;
							GetDocument()->Book().GetCalendar().AddCalendar(pCal);
						}
					}
				}
			}
		}
		clpData.Close();
		if (bLoaded)
		{
			GetDocument()->Book().GetCalendar().sort();
			LoadData();
			GetDocument()->Modify(true);
			CUpdateHint hint(UPDATE_CALENDAR_VIEW);
			GetDocument()->UpdateAllViews(this, &hint);
		}
	}
	break;

	case wxID_SELECTALL:
		m_Ctrl->SelectAll();
		break;

	case wxID_FIND:
	{
		CDlgFind dlg(m_find, m_Ctrl);
		dlg.ShowModal();
	}
	break;

	case ID_EDIT_FIND_NEXT:
	{
		m_find.SearchDown(true);
		if (m_find.Text().empty())
			OnCmd(wxID_FIND);
		else
			m_find.Search(nullptr);
	}
	break;

	case ID_EDIT_FIND_PREVIOUS:
	{
		m_find.SearchDown(false);
		if (m_find.Text().empty())
			OnCmd(wxID_FIND);
		else
			m_find.Search(nullptr);
	}
	break;

	case ID_AGILITY_EDIT_CALENDAR:
	{
		CAgilityBookCalendarListViewDataPtr pData = GetItemCalData(m_Ctrl->GetFirstSelected());
		if (pData && pData->CanEdit())
		{
			ARBDate oldDate = pData->GetCalendar()->GetStartDate();
			CDlgCalendar dlg(pData->GetCalendar(), GetDocument());
			if (wxID_OK == dlg.ShowModal())
			{
				if (CAgilityBookOptions::AutoDeleteCalendarEntries()
					&& pData->GetCalendar()->GetEndDate() < ARBDate::Today())
				{
					ARBDate today(ARBDate::Today());
					today -= CAgilityBookOptions::DaysTillEntryIsPast();
					GetDocument()->Book().GetCalendar().TrimEntries(today);
				}
				GetDocument()->Book().GetCalendar().sort();
				if (oldDate != pData->GetCalendar()->GetStartDate())
				{
					GetDocument()->SetCalenderDate(pData->GetCalendar()->GetStartDate());
				}
				LoadData();
				GetDocument()->Modify(true);
				CUpdateHint hint(UPDATE_CALENDAR_VIEW);
				GetDocument()->UpdateAllViews(this, &hint);
			}
		}
	}
	break;

	case wxID_DELETE:
	case ID_AGILITY_DELETE_CALENDAR:
	{
		std::vector<long> indices;
		if (0 < m_Ctrl->GetSelection(indices))
		{
			std::vector<CAgilityBookCalendarListViewDataPtr> items;
			for (std::vector<long>::iterator iterData = indices.begin(); iterData != indices.end(); ++iterData)
			{
				CAgilityBookCalendarListViewDataPtr pData = GetItemCalData(*iterData);
				if (pData && pData->CanDelete())
					items.push_back(pData);
			}
			for (std::vector<CAgilityBookCalendarListViewDataPtr>::iterator iter = items.begin(); iter != items.end();
				 ++iter)
			{
				GetDocument()->Book().GetCalendar().DeleteCalendar((*iter)->GetCalendar());
			}
			if (0 < items.size())
			{
				LoadData();
				long index = indices[0];
				if (index >= m_Ctrl->GetItemCount())
					index = m_Ctrl->GetItemCount() - 1;
				m_Ctrl->SetSelection(index);
				GetDocument()->Modify(true);
				CUpdateHint hint(UPDATE_CALENDAR_VIEW);
				GetDocument()->UpdateAllViews(this, &hint);
			}
		}
	}
	break;

	case ID_AGILITY_EXPORT_CALENDAR:
	{
		std::vector<long> indices;
		if (0 < m_Ctrl->GetSelection(indices))
		{
			std::vector<ARBCalendarPtr> items;
			for (std::vector<long>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
			{
				CAgilityBookCalendarListViewDataPtr pData = GetItemCalData(*iter);
				if (pData)
				{
					ARBCalendarPtr pCal = pData->GetCalendar();
					items.push_back(pCal);
				}
			}
			std::vector<ARBCalendarPtr>* exportItems = nullptr;
			if (0 < items.size())
				exportItems = &items;
			new CWizard(GetDocument(), exportItems);
		}
	}
	break;

	case ID_AGILITY_CREATEENTRY_CALENDAR:
	{
		if (0 == GetDocument()->Book().GetDogs().size())
			return true;
		CAgilityBookCalendarListViewDataPtr pData = GetItemCalData(m_Ctrl->GetFirstSelected());
		if (pData)
		{
			ARBCalendarPtr pCal = pData->GetCalendar();
			GetDocument()->CreateTrialFromCalendar(*pCal, m_pTabView);
		}
	}
	break;

	case ID_VIEW_CUSTOMIZE:
	{
		CDlgAssignColumns dlg(CAgilityBookOptions::eView, m_Ctrl, GetDocument(), IO_TYPE_VIEW_CALENDAR_LIST);
		dlg.ShowModal();
	}
	break;
	}
	return bHandled;
}


void CAgilityBookCalendarListView::OnViewCmd(wxCommandEvent& evt)
{
	CLogger::Log(wxString::Format(L"MENU: CAgilityBookCalendarListView::OnViewCmd %d", evt.GetId()));
	OnCmd(evt.GetId());
}


void CAgilityBookCalendarListView::OnPrintView(wxCommandEvent& evt)
{
	CLogger::Log(L"MENU: CAgilityBookCalendarListView::OnPrintView");
	wxGetApp().GetHtmlPrinter()->PrintText(m_Ctrl->GetPrintDataAsHtmlTable());
}


void CAgilityBookCalendarListView::OnPreview(wxCommandEvent& evt)
{
	CLogger::Log(L"MENU: CAgilityBookCalendarListView::OnPreview");
	wxGetApp().GetHtmlPrinter()->PreviewText(m_Ctrl->GetPrintDataAsHtmlTable());
}

} // namespace dconSoft
