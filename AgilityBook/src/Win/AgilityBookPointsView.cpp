/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CAgilityBookPointsView class
 * @author David Connet
 *
 * Revision History
 * @li 2011-12-22 DRC Switch to using Bind on wx2.9+.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-04 DRC Ported to wxWidgets.
 * @li 2006-07-16 DRC Let user specify how to sort events within a venue.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-10-18 DRC Remember last selected item when reloading data.
 * @li 2005-10-14 DRC Added a context menu.
 * @li 2005-09-15 DRC Added code to filter multi-Qs by date (forgot it - oops!)
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-05-04 DRC Added subtotaling by division to lifetime points.
 * @li 2005-03-14 DRC Show a summary of lifetime points in the list viewer.
 * @li 2005-01-11 DRC Mark the document dirty when creating a title.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2004-12-03 DRC Show all lifetime points when filtering.
 * @li 2004-10-20 DRC Do not accumulate NA titling runs.
 * @li 2004-08-25 DRC Existing other pts were included in all other points.
 * @li 2004-08-12 DRC Allow creating a new title.
 * @li 2004-06-23 DRC Fixed a problem when getting the Q/NQ ratio when a filter
 *                    was in place. Trials that were hidden also removed any
 *                    runs that might have been applicable to the ratio.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 *                    Fix filtering on co-sanctioned trials.
 * @li 2004-05-27 DRC Lifetime point accumulation did not display the points
 *                    for existing runs.
 * @li 2004-05-20 DRC Add Dogs name and current date to report. Oops, just
 *                    realized the only thing that shouldn't be filtered on
 *                    runs is the Q-status (resets the last 2 changes).
 * @li 2004-05-16 DRC Do filter levels.
 * @li 2004-05-03 DRC Do not filter runs, only venues and titles.
 *                    Added percent qualifying.
 * @li 2004-02-02 DRC Added ExistingPoints accumulation.
 * @li 2004-01-05 DRC Header didn't generate properly if there were no titles.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-12-09 DRC Fixed problem tallying QQs when a 3rd run is present.
 * @li 2003-11-22 DRC Added more dog information into the report.
 * @li 2003-11-21 DRC Enabled copy and select all.
 * @li 2003-10-28 DRC Added '*' to Title if it has been received.
 * @li 2003-10-14 DRC Fixed problem inserting other point lines.
 * @li 2003-10-13 DRC Don't tally runs that have no titling points.
 * @li 2003-08-28 DRC Completed Other Points tallying
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 * @li 2003-06-11 DRC Accumulate points based on level, not sublevel, name.
 */

#include "stdafx.h"
#include "AgilityBookPointsView.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookMenu.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTreeData.h"
#include "ARBDogClub.h"
#include "ARBDogTrial.h"
#include "ARBTypes.h"
#include "ClipBoard.h"
#include "DlgPointsViewSort.h"
#include "FilterOptions.h"
#include "MainFrm.h"
#include "PointsData.h"
#include "Print.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


// Columns:
// 1: Venue
// 2: Reg-Number
// 1: <blank>
// 2: Date
// 3: Title
// 1: <blank>
// 2: Division
// 3: Level
// 4: Event
// 5: n Runs, n Judges, n Partners
// 6: n Qs (x %), (n clean), n Judges, n Partners
// 7: Points
// 8: SuperQs/SpeedPts(per run), total MQs/speedpts
// 9: SpeedPts (if SuperQs too) [possible, but an event like this doesn't exist]
//
// 1: Other Points
// 2: Name
// 3: Points total
#define MAX_COLUMNS		9

class CAgilityBookPointsViewData : public CListData
{
public:
	CAgilityBookPointsViewData(CPointsDataBasePtr inData, int imgIndex)
		: m_Data(inData)
		, m_imgIndex(imgIndex)
	{
	}
	virtual std::wstring OnNeedText(long index) const
	{
		return m_Data->OnNeedText(index);
	}
	virtual void OnNeedListItem(long iCol, wxListItem& info) const;
	virtual bool HasDetails() const
	{
		return m_Data->HasDetails();
	}
	virtual void Details() const
	{
		m_Data->Details();
	}
	virtual bool IsEqual(CPointsDataBasePtr inData)
	{
		return m_Data->IsEqual(inData);
	}

	CPointsDataBasePtr m_Data;
	int m_imgIndex;
};


void CAgilityBookPointsViewData::OnNeedListItem(long iCol, wxListItem& info) const
{
	if (m_Data)
	{
		info.SetMask(info.GetMask() | wxLIST_MASK_TEXT);
		info.SetText(StringUtil::stringWX(OnNeedText(iCol)));
		if (0 == iCol)
		{
			info.SetMask(info.GetMask() | wxLIST_MASK_IMAGE);
			info.SetImage(m_imgIndex);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(CAgilityBookPointsView, CAgilityBookBaseExtraView)


BEGIN_EVENT_TABLE(CAgilityBookPointsView, CAgilityBookBaseExtraView)
	EVT_CONTEXT_MENU(CAgilityBookPointsView::OnViewContextMenu)
	EVT_UPDATE_UI(wxID_COPY, CAgilityBookPointsView::OnViewUpdateCmd)
	EVT_MENU(wxID_COPY, CAgilityBookPointsView::OnViewCmd)
	EVT_UPDATE_UI(wxID_SELECTALL, CAgilityBookPointsView::OnViewUpdateCmd)
	EVT_MENU(wxID_SELECTALL, CAgilityBookPointsView::OnViewCmd)
	EVT_UPDATE_UI(ID_DETAILS, CAgilityBookPointsView::OnViewUpdateCmd)
	EVT_MENU(ID_DETAILS, CAgilityBookPointsView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_NEW_TITLE, CAgilityBookPointsView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_NEW_TITLE, CAgilityBookPointsView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_POINTS_VIEW_SORT, CAgilityBookPointsView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_POINTS_VIEW_SORT, CAgilityBookPointsView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_HIDDEN, CAgilityBookPointsView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_HIDDEN, CAgilityBookPointsView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_LIFETIME_EVENTS, CAgilityBookPointsView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_LIFETIME_EVENTS, CAgilityBookPointsView::OnViewCmd)
	EVT_MENU(wxID_PRINT, CAgilityBookPointsView::OnPrint)
	EVT_MENU(wxID_PREVIEW, CAgilityBookPointsView::OnPreview)
END_EVENT_TABLE()


CAgilityBookPointsView::CAgilityBookPointsView(
		CTabView* pTabView,
		wxDocument* doc)
	: CAgilityBookBaseExtraView(pTabView, doc)
	, m_Ctrl(NULL)
{
}


CAgilityBookPointsView::~CAgilityBookPointsView()
{
}


bool CAgilityBookPointsView::Create(
		CBasePanel* parentView,
		wxWindow* parentCtrl,
		wxDocument* doc,
		long flags,
		wxSizer* sizer,
		int proportion,
		int sizerFlags,
		int border)
{
	m_Ctrl = new CReportListCtrl(parentCtrl, false, CReportListCtrl::eNoSortHeader);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler, CAgilityBookPointsView::OnCtrlItemActivated);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_KEY_DOWN, wxKeyEventHandler, CAgilityBookPointsView::OnCtrlKeyDown);
#if defined(__WXMAC__)
	m_Ctrl->SetDropTarget(new CFileDropTarget(doc->GetDocumentManager()));
#endif
	return CAgilityBookBaseExtraView::Create(parentView, parentCtrl, doc, flags, sizer, proportion, sizerFlags, border);
}


void CAgilityBookPointsView::DetachView()
{
	// The control is actually owned by the panel, the view is not.
	m_Ctrl = NULL;
}


bool CAgilityBookPointsView::IsFiltered() const
{
	return CFilterOptions::Options().IsFilterEnabled();
}


bool CAgilityBookPointsView::GetMessage(std::wstring& msg) const
{
	msg = _("IDS_INDICATOR_BLANK");
	return true;
}


bool CAgilityBookPointsView::GetMessage2(std::wstring& msg) const
{
	if (GetDocument()->GetCurrentDog())
	{
		msg = GetDocument()->GetCurrentDog()->GetCallName();
		return true;
	}
	else
	{
		msg.clear();
		return false;
	}
}


bool CAgilityBookPointsView::AllowStatusContext(int field) const
{
	return STATUS_DOG == field || STATUS_FILTERED == field;
}


bool CAgilityBookPointsView::OnCreate(
		wxDocument* doc,
		long flags)
{
	SetupColumns();
	return true;
}


void CAgilityBookPointsView::DoActivateView(
		bool activate,
		wxView* activeView,
		wxView* deactiveView)
{
	if (m_Ctrl && activate)
		m_Ctrl->SetFocus();
}


void CAgilityBookPointsView::OnDraw(wxDC* dc)
{
}


void CAgilityBookPointsView::OnUpdate(
		wxView* sender,
		wxObject* inHint)
{
	CUpdateHint* hint = NULL;
	if (inHint)
		hint = wxDynamicCast(inHint, CUpdateHint);
	if (!hint || hint->IsSet(UPDATE_POINTS_VIEW)
	|| hint->IsEqual(UPDATE_CONFIG) || hint->IsEqual(UPDATE_OPTIONS))
	{
		LoadData();
	}
	else if (hint && hint->IsEqual(UPDATE_LANG_CHANGE))
	{
		SetupColumns();
		LoadData();
	}
}


void CAgilityBookPointsView::GetPrintLine(
		long item,
		std::vector<std::wstring>& line) const
{
	if (m_Ctrl)
		m_Ctrl->GetPrintLine(item, line);
}


CAgilityBookPointsViewDataPtr CAgilityBookPointsView::GetItemPointsData(long index) const
{
	if (!m_Ctrl)
		return CAgilityBookPointsViewDataPtr();
	return std::tr1::dynamic_pointer_cast<CAgilityBookPointsViewData, CListData>(m_Ctrl->GetData(index));
}


void CAgilityBookPointsView::SetupColumns()
{
	if (!m_Ctrl)
		return;
	int nColumnCount = m_Ctrl->GetColumnCount();
	for (int i = 0; i < nColumnCount; ++i)
		m_Ctrl->DeleteColumn(0);

	for (long iCol = 0; iCol < MAX_COLUMNS; ++iCol)
	{
		wxString str = L"";
		if (1 == iCol)
			str = _("IDS_TITLING_POINTS");
		int fmt = wxLIST_FORMAT_LEFT;
		m_Ctrl->InsertColumn(iCol, str, fmt);
	}
}


void CAgilityBookPointsView::LoadData()
{
	if (!m_Ctrl)
		return;

	wxBusyCursor wait;

	// Reduce flicker
	m_Ctrl->Freeze();

	// Get the current item.
	CPointsDataBasePtr curData;
	CAgilityBookPointsViewDataPtr pCurData = GetItemPointsData(m_Ctrl->GetFirstSelected());
	if (pCurData)
	{
		curData = pCurData->m_Data;
		curData.reset();
	}

	// Clear everything.
	m_Ctrl->DeleteAllItems();

	CPointsDataItems items;
	items.LoadData(GetDocument(), GetDocument()->GetCurrentDog());

	size_t nItems = items.NumLines();
	for (size_t nItem = 0; nItem < nItems; ++nItem)
	{
		CPointsDataBasePtr data = items.GetLine(nItem);
		if (data->IsVisible())
		{
			long item = m_Ctrl->InsertItem(std::make_shared<CAgilityBookPointsViewData>(data, m_Ctrl->ImageEmpty()));
			if (curData && data->IsEqual(curData))
			{
				m_Ctrl->Select(item);
				m_Ctrl->Focus(item);
			}
		}
	}

	int nColumnCount = m_Ctrl->GetColumnCount();
	for (int i = 0; i < nColumnCount; ++i)
		m_Ctrl->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);

	if (m_Ctrl->IsShownOnScreen())
		UpdateMessages();

	m_Ctrl->Thaw();
	m_Ctrl->Refresh();
}


void CAgilityBookPointsView::OnCtrlItemActivated(wxListEvent& evt)
{
	OnCmd(ID_DETAILS);
}


void CAgilityBookPointsView::OnCtrlKeyDown(wxKeyEvent& evt)
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
		OnCmd(ID_DETAILS);
		break;
	}
}


void CAgilityBookPointsView::OnViewContextMenu(wxContextMenuEvent& evt)
{
	if (!m_Ctrl)
		return;
	wxPoint point;
	if (GetMenuPosition(point, *m_Ctrl, evt))
	{
		wxMenu* menu = CreatePopup(IdMenuPoints);
		m_Ctrl->PopupMenu(menu, point);
		delete menu;
	}
	else
		evt.Skip();
}


void CAgilityBookPointsView::OnViewUpdateCmd(wxUpdateUIEvent& evt)
{
	switch (evt.GetId())
	{
	case wxID_COPY:
		evt.Enable(true);
		break;
	case wxID_SELECTALL:
		evt.Enable(m_Ctrl->CanSelectAll());
		break;
	case ID_DETAILS:
		evt.Enable(1 == m_Ctrl->GetSelectedItemCount());
		break;
	case ID_AGILITY_NEW_TITLE:
		evt.Enable(GetDocument()->GetCurrentDog() ? true : false);
		break;
	case ID_VIEW_POINTS_VIEW_SORT:
	case ID_VIEW_HIDDEN:
	case ID_VIEW_LIFETIME_EVENTS:
		evt.Enable(true);
		break;
	}
}


bool CAgilityBookPointsView::OnCmd(int id)
{
	bool bHandled = true;
	switch (id)
	{
	default:
		bHandled = false;
		break;

	case wxID_COPY:
		{
			std::vector<long> indices;
			if (0 == m_Ctrl->GetSelection(indices))
			{
				for (long i = 0; i < m_Ctrl->GetItemCount(); ++i)
					indices.push_back(i);
			}

			CClipboardDataWriter clpData;
			if (!clpData.isOkay())
				return true;

			std::wstring data;
			std::wstring html;
			CClipboardDataTable table(data, html);

			// Take care of the header, but only if more than one line is selected.
			if (1 < indices.size()
			|| indices.size() == static_cast<size_t>(m_Ctrl->GetItemCount()))
			{
				std::vector<std::wstring> line;
				m_Ctrl->GetPrintLine(-1, line);
				table.StartLine();
				for (int i = 0; i < static_cast<int>(line.size()); ++i)
				{
					table.Cell(i, line[i]);
				}
				table.EndLine();
			}

			// Now all the data.
			for (std::vector<long>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
			{
				std::vector<std::wstring> line;
				m_Ctrl->GetPrintLine((*iter), line);
				table.StartLine();
				for (int i = 0; i < static_cast<int>(line.size()); ++i)
				{
					table.Cell(i, line[i]);
				}
				table.EndLine();
			}

			clpData.AddData(table);
			clpData.CommitData();
		}
		break;

	case wxID_SELECTALL:
		m_Ctrl->SelectAll();
		break;

	case ID_DETAILS:
		{
			CAgilityBookPointsViewDataPtr pData = GetItemPointsData(m_Ctrl->GetFirstSelected());
			if (pData)
				pData->Details();
		}
		break;

	case ID_AGILITY_NEW_TITLE:
		if (GetDocument()->GetCurrentDog())
		{
			// Convenience! No duplicated code!
			CAgilityBookTreeDataDog data(GetDocument()->GetTreeModel(), GetDocument()->GetCurrentDog());
			bool bModified = false;
			if (data.OnCmd(ID_AGILITY_NEW_TITLE, bModified, NULL))
			{
				if (bModified)
					GetDocument()->Modify(true);
			}
		}
		break;

	case ID_VIEW_POINTS_VIEW_SORT:
		{
			CDlgPointsViewSort dlg(m_Ctrl);
			if (wxID_OK == dlg.ShowModal())
				LoadData();
		}
		break;

	case ID_VIEW_HIDDEN:
		{
			CAgilityBookOptions::SetViewHiddenTitles(!CAgilityBookOptions::GetViewHiddenTitles());
			std::vector<CVenueFilter> venues;
			CFilterOptions::Options().GetFilterVenue(venues);
			for (ARBDogList::iterator iterDogs = GetDocument()->Book().GetDogs().begin();
					iterDogs != GetDocument()->Book().GetDogs().end();
					++iterDogs)
			{
				for (ARBDogTitleList::iterator iterTitle = (*iterDogs)->GetTitles().begin();
						iterTitle != (*iterDogs)->GetTitles().end();
						++iterTitle)
				{
					GetDocument()->ResetVisibility(venues, *iterTitle);
				}
			}
			LoadData();
		}
		break;

	case ID_VIEW_LIFETIME_EVENTS:
		CAgilityBookOptions::SetViewLifetimePointsByEvent(!CAgilityBookOptions::GetViewLifetimePointsByEvent());
		LoadData();
		break;
	}
	return bHandled;
}


void CAgilityBookPointsView::OnViewCmd(wxCommandEvent& evt)
{
	OnCmd(evt.GetId());
}


void CAgilityBookPointsView::OnPrint(wxCommandEvent& evt)
{
	wxGetApp().GetHtmlPrinter()->PrintText(StringUtil::stringWX(m_Ctrl->GetPrintDataAsHtmlTable()));
}


void CAgilityBookPointsView::OnPreview(wxCommandEvent& evt)
{
	wxGetApp().GetHtmlPrinter()->PreviewText(StringUtil::stringWX(m_Ctrl->GetPrintDataAsHtmlTable()));
}
