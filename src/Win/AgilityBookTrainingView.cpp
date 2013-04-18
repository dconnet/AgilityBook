/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CAgilityBookTrainingView class
 * @author David Connet
 *
 * Revision History
 * @li 2012-12-29 DRC Update training view properly after a paste.
 * @li 2011-12-22 DRC Switch to using Bind on wx2.9+.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-04 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-01-25 DRC Remember the sort column between program invocations.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-06-24 DRC Added a sort header image.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-04-15 DRC Added Duplicate menu item.
 * @li 2004-04-06 DRC Added simple sorting by column.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-12-27 DRC Implemented Find/FindNext.
 * @li 2003-12-14 DRC Re-sort items after editing an existing one.
 * @li 2003-09-21 DRC Created
 */

#include "stdafx.h"
#include "AgilityBookTrainingView.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookMenu.h"
#include "AgilityBookOptions.h"
#include "ClipBoard.h"
#include "DlgAssignColumns.h"
#include "DlgFind.h"
#include "DlgTraining.h"
#include "FilterOptions.h"
#include "ListData.h"
#include "MainFrm.h"
#include "Print.h"
#include "RegItems.h"

#include "ARB/ARBTraining.h"
#include "ARB/Element.h"
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookTrainingViewData

class CAgilityBookTrainingViewData : public CListData
{
public:
	CAgilityBookTrainingViewData(
			CAgilityBookTrainingView* pView,
			ARBTrainingPtr pTraining)
		: m_pView(pView)
		, m_pTraining(pTraining)
	{
	}
	~CAgilityBookTrainingViewData()
	{
	}

	bool CanEdit() const				{return true;}
	bool CanDelete() const				{return true;}

	ARBTrainingPtr GetTraining()		{return m_pTraining;}
	virtual std::wstring OnNeedText(long iCol) const;
	virtual void OnNeedListItem(long iCol, wxListItem& info) const;

private:
	CAgilityBookTrainingView* m_pView;
	ARBTrainingPtr m_pTraining;
};


std::wstring CAgilityBookTrainingViewData::OnNeedText(long iCol) const
{
	std::wstring str;
	if (m_pTraining)
	{
		switch (m_pView->m_Columns[iCol])
		{
		case IO_LOG_DATE:
			str = m_pTraining->GetDate().GetString();
			break;
		case IO_LOG_NAME:
			str = m_pTraining->GetName();
			break;
		case IO_LOG_SUBNAME:
			str = m_pTraining->GetSubName();
			break;
		case IO_LOG_NOTES:
			str = StringUtil::Replace(m_pTraining->GetNote(), L"\n", L" ");
			break;
		}
	}
	return str;
}


void CAgilityBookTrainingViewData::OnNeedListItem(long iCol, wxListItem& info) const
{
	if (m_pTraining)
	{
		info.SetMask(info.GetMask() | wxLIST_MASK_TEXT);
		info.SetText(StringUtil::stringWX(OnNeedText(iCol)));
		if (0 == iCol)
		{
			info.SetMask(info.GetMask() | wxLIST_MASK_IMAGE);
			info.SetImage(m_pView->m_Ctrl->ImageEmpty());
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// List sorting

CAgilityBookTrainingView::CSortColumn::CSortColumn(std::vector<long>& inColumns)
	: m_Columns(inColumns)
	, m_iCol(1)
{
}


void CAgilityBookTrainingView::CSortColumn::Initialize()
{
	long realCol = IO_LOG_DATE;
	realCol = wxConfig::Get()->Read(CFG_SORTING_TRAINING, realCol);
	long neg = 1;
	if (0 > realCol)
	{
		neg = -1;
		realCol *= -1;
	}
	long col = LookupColumn(realCol);
	if (0 > m_iCol)
		col = LookupColumn(IO_LOG_DATE);
	m_iCol = col * neg;
}


void CAgilityBookTrainingView::CSortColumn::SetColumn(long iCol)
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
	long realCol = m_Columns[col-1] * neg;
	wxConfig::Get()->Write(CFG_SORTING_TRAINING, realCol);
}


long CAgilityBookTrainingView::CSortColumn::LookupColumn(long iCol) const
{
	size_t n = m_Columns.size();
	for (size_t i = 0; i < n; ++i)
	{
		if (m_Columns[i] == iCol)
		{
			return static_cast<long>(i+1);
		}
	}
	return -1;
}


// The wx functions take a 'long'. Which means we can't pass pointers on 64bit.
// So, we use a global. Since this is only used in one place, we don't have
// any threading issues.
static struct
{
	CAgilityBookTrainingView* pThis;
	int nCol;
} s_SortInfo;


#if wxCHECK_VERSION(2, 9, 4)
int wxCALLBACK CompareTraining(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
#else
int wxCALLBACK CompareTraining(long item1, long item2, long sortData)
#endif
{
	if (0 == s_SortInfo.nCol)
		return 0;
	CListDataPtr pRawItem1 = s_SortInfo.pThis->m_Ctrl->GetDataByData(static_cast<long>(item1));
	CListDataPtr pRawItem2 = s_SortInfo.pThis->m_Ctrl->GetDataByData(static_cast<long>(item2));
	CAgilityBookTrainingViewDataPtr pItem1 = std::tr1::dynamic_pointer_cast<CAgilityBookTrainingViewData, CListData>(pRawItem1);
	CAgilityBookTrainingViewDataPtr pItem2 = std::tr1::dynamic_pointer_cast<CAgilityBookTrainingViewData, CListData>(pRawItem2);
	int nRet = 0;
	int iCol = abs(s_SortInfo.nCol);
	switch (s_SortInfo.pThis->m_Columns[iCol-1])
	{
	case IO_LOG_DATE:
		if (pItem1->GetTraining()->GetDate() < pItem2->GetTraining()->GetDate())
			nRet = -1;
		else if (pItem1->GetTraining()->GetDate() > pItem2->GetTraining()->GetDate())
			nRet = 1;
		break;
	case IO_LOG_NAME:
		if (pItem1->GetTraining()->GetName() < pItem2->GetTraining()->GetName())
			nRet = -1;
		else if (pItem1->GetTraining()->GetName() > pItem2->GetTraining()->GetName())
			nRet = 1;
		break;
	case IO_LOG_SUBNAME:
		if (pItem1->GetTraining()->GetSubName() < pItem2->GetTraining()->GetSubName())
			nRet = -1;
		else if (pItem1->GetTraining()->GetSubName() > pItem2->GetTraining()->GetSubName())
			nRet = 1;
		break;
	case IO_LOG_NOTES:
		if (pItem1->GetTraining()->GetNote() < pItem2->GetTraining()->GetNote())
			nRet = -1;
		else if (pItem1->GetTraining()->GetNote() > pItem2->GetTraining()->GetNote())
			nRet = 1;
		break;
	}
	if (0 > s_SortInfo.nCol)
		nRet *= -1;
	return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// Find

bool CFindTraining::Search(CDlgFind* pDlg) const
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
	std::wstring search = Text();
	if (!MatchCase())
		search = StringUtil::ToLower(search);
	for (; !bFound && 0 <= index && index < m_pView->m_Ctrl->GetItemCount(); index += inc)
	{
		std::set<std::wstring> strings;
		if (SearchAll())
		{
			CAgilityBookTrainingViewDataPtr pData = m_pView->GetItemTrainingData(index);
			if (pData)
				pData->GetTraining()->GetSearchStrings(strings);
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
				strings.insert(StringUtil::stringW(info.GetText()));
			}
		}
		for (std::set<std::wstring>::iterator iter = strings.begin(); iter != strings.end(); ++iter)
		{
			std::wstring str((*iter));
			if (!MatchCase())
				str = StringUtil::ToLower(str);
			if (std::wstring::npos != str.find(search))
			{
				m_pView->m_Ctrl->Select(index, true);
				m_pView->m_Ctrl->Focus(index);
				bFound = true;
			}
		}
	}
	if (!bFound)
	{
		wxString msg = wxString::Format(_("IDS_CANNOT_FIND"), m_strSearch.c_str());
		wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
	}
	return bFound;
}

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(CAgilityBookTrainingView, CAgilityBookBaseExtraView)


BEGIN_EVENT_TABLE(CAgilityBookTrainingView, CAgilityBookBaseExtraView)
	EVT_CONTEXT_MENU(CAgilityBookTrainingView::OnViewContextMenu)
	EVT_UPDATE_UI(wxID_DUPLICATE, CAgilityBookTrainingView::OnViewUpdateCmd)
	EVT_MENU(wxID_DUPLICATE, CAgilityBookTrainingView::OnViewCmd)
	EVT_UPDATE_UI(wxID_CUT, CAgilityBookTrainingView::OnViewUpdateCmd)
	EVT_MENU(wxID_CUT, CAgilityBookTrainingView::OnViewCmd)
	EVT_UPDATE_UI(wxID_COPY, CAgilityBookTrainingView::OnViewUpdateCmd)
	EVT_MENU(wxID_COPY, CAgilityBookTrainingView::OnViewCmd)
	EVT_UPDATE_UI(wxID_PASTE, CAgilityBookTrainingView::OnViewUpdateCmd)
	EVT_MENU(wxID_PASTE, CAgilityBookTrainingView::OnViewCmd)
	EVT_UPDATE_UI(wxID_SELECTALL, CAgilityBookTrainingView::OnViewUpdateCmd)
	EVT_MENU(wxID_SELECTALL, CAgilityBookTrainingView::OnViewCmd)
	EVT_UPDATE_UI(wxID_FIND, CAgilityBookTrainingView::OnViewUpdateCmd)
	EVT_MENU(wxID_FIND, CAgilityBookTrainingView::OnViewCmd)
	EVT_UPDATE_UI(ID_EDIT_FIND_NEXT, CAgilityBookTrainingView::OnViewUpdateCmd)
	EVT_MENU(ID_EDIT_FIND_NEXT, CAgilityBookTrainingView::OnViewCmd)
	EVT_UPDATE_UI(ID_EDIT_FIND_PREVIOUS, CAgilityBookTrainingView::OnViewUpdateCmd)
	EVT_MENU(ID_EDIT_FIND_PREVIOUS, CAgilityBookTrainingView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_EDIT_TRAINING, CAgilityBookTrainingView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_EDIT_TRAINING, CAgilityBookTrainingView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_DELETE_TRAINING, CAgilityBookTrainingView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_DELETE_TRAINING, CAgilityBookTrainingView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_CUSTOMIZE, CAgilityBookTrainingView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_CUSTOMIZE, CAgilityBookTrainingView::OnViewCmd)
	EVT_MENU(wxID_PRINT, CAgilityBookTrainingView::OnPrint)
	EVT_MENU(wxID_PREVIEW, CAgilityBookTrainingView::OnPreview)
END_EVENT_TABLE()


#include "Platform/arbWarningPush.h"
CAgilityBookTrainingView::CAgilityBookTrainingView(
		CTabView* pTabView,
		wxDocument* doc)
	: CAgilityBookBaseExtraView(pTabView, doc)
	, m_Ctrl(NULL)
	, m_Columns()
	, m_Callback(this)
	, m_SortColumn(m_Columns)
{
}
#include "Platform/arbWarningPop.h"


CAgilityBookTrainingView::~CAgilityBookTrainingView()
{
}


bool CAgilityBookTrainingView::Create(
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
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler, CAgilityBookTrainingView::OnCtrlColumnClick);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler, CAgilityBookTrainingView::OnCtrlItemActivated);
	BIND_OR_CONNECT_CTRL(m_Ctrl, wxEVT_KEY_DOWN, wxKeyEventHandler, CAgilityBookTrainingView::OnCtrlKeyDown);
#if defined(__WXMAC__)
	m_Ctrl->SetDropTarget(new CFileDropTarget(doc->GetDocumentManager()));
#endif
	return CAgilityBookBaseExtraView::Create(parentView, parentCtrl, doc, flags, sizer, proportion, sizerFlags, border);
}


void CAgilityBookTrainingView::DetachView()
{
	// The control is actually owned by the panel, the view is not.
	m_Ctrl = NULL;
}


void CAgilityBookTrainingView::SetCurrentDate(ARBDate const& inDate)
{
	if (!m_Ctrl)
		return;
	long index = -1;
	for (long i = 0; i < m_Ctrl->GetItemCount(); ++i)
	{
		CAgilityBookTrainingViewDataPtr pTraining = GetItemTrainingData(i);
		if (pTraining && pTraining->GetTraining()->GetDate() == inDate)
		{
			index = i;
			break;
		}
	}
	if (0 <= index)
	{
		m_Ctrl->Select(index, true);
		m_Ctrl->Focus(index);
	}
}


bool CAgilityBookTrainingView::IsFiltered() const
{
	if (CFilterOptions::Options().GetViewAllDates()
	&& CFilterOptions::Options().GetTrainingViewAllNames())
		return false;
	else
		return true;
}


bool CAgilityBookTrainingView::GetMessage(std::wstring& msg) const
{
	if (!m_Ctrl)
		return false;
	msg = StringUtil::stringW(wxString::Format(_("IDS_NUM_TRAINING"), m_Ctrl->GetItemCount()));
	return true;
}


bool CAgilityBookTrainingView::GetMessage2(std::wstring& msg) const
{
	msg = StringUtil::stringW(_("IDS_INDICATOR_BLANK"));
	return true;
}


bool CAgilityBookTrainingView::AllowStatusContext(int field) const
{
	return STATUS_FILTERED == field;
}


bool CAgilityBookTrainingView::OnCreate(
		wxDocument* doc,
		long flags)
{
	SetupColumns();
	return true;
}


void CAgilityBookTrainingView::DoActivateView(
		bool activate,
		wxView* activeView,
		wxView* deactiveView)
{
	if (m_Ctrl && activate)
		m_Ctrl->SetFocus();
}


void CAgilityBookTrainingView::OnDraw(wxDC* dc)
{
}


void CAgilityBookTrainingView::OnUpdate(
		wxView* sender,
		wxObject* inHint)
{
	if (!m_Ctrl)
		return;
	CUpdateHint* hint = NULL;
	if (inHint)
		hint = wxDynamicCast(inHint, CUpdateHint);
	if (!hint || hint->IsSet(UPDATE_TRAINING_VIEW)
	|| hint->IsEqual(UPDATE_OPTIONS))
	{
		LoadData();
	}
	else if (hint && (hint->IsEqual(UPDATE_LANG_CHANGE) || hint->IsEqual(UPDATE_CUSTOMIZE)))
	{
		SetupColumns();
		LoadData();
	}
}


void CAgilityBookTrainingView::GetPrintLine(
		long item,
		std::vector<std::wstring>& line) const
{
	if (m_Ctrl)
		m_Ctrl->GetPrintLine(item, line);
}


CAgilityBookTrainingViewDataPtr CAgilityBookTrainingView::GetItemTrainingData(long index) const
{
	if (!m_Ctrl)
		return CAgilityBookTrainingViewDataPtr();
	return std::tr1::dynamic_pointer_cast<CAgilityBookTrainingViewData, CListData>(m_Ctrl->GetData(index));
}


void CAgilityBookTrainingView::SetupColumns()
{
	if (!m_Ctrl)
		return;
	int nColumnCount = m_Ctrl->GetColumnCount();
	for (int i = 0; i < nColumnCount; ++i)
		m_Ctrl->DeleteColumn(0);
	if (CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::eView, IO_TYPE_VIEW_TRAINING_LIST, m_Columns))
	{
		for (size_t iCol = 0; iCol < m_Columns.size(); ++iCol)
		{
			std::wstring str = CDlgAssignColumns::GetNameFromColumnID(m_Columns[iCol]);
			int fmt = CDlgAssignColumns::GetFormatFromColumnID(m_Columns[iCol]);
			m_Ctrl->InsertColumn(static_cast<long>(iCol), StringUtil::stringWX(str), fmt);
		}
		m_SortColumn.Initialize();
	}
}


void CAgilityBookTrainingView::LoadData()
{
	if (!m_Ctrl)
		return;

	// Remember what's selected.
	ARBTrainingPtr pCurTraining;
	CAgilityBookTrainingViewDataPtr pCurData = GetItemTrainingData(m_Ctrl->GetFirstSelected());
	if (pCurData)
	{
		pCurTraining = pCurData->GetTraining();
		pCurData.reset();
	}

	// Reduce flicker
	m_Ctrl->Freeze();

	// Clear everything.
	m_Ctrl->DeleteAllItems();

	// Add items.
	int i = 0;
	for (ARBTrainingList::iterator iter = GetDocument()->Book().GetTraining().begin();
	iter != GetDocument()->Book().GetTraining().end();
	++iter)
	{
		ARBTrainingPtr pTraining = (*iter);
		if (pTraining->IsFiltered())
			continue;
		long index = m_Ctrl->InsertItem(std::make_shared<CAgilityBookTrainingViewData>(this, pTraining));
		// We may have modified the entry, so don't do a full equality test.
		// Just check the start/end date, location, club and venue. This allows
		// us to modify the opens/closes dates, notes and entry status.
		// Note: This is only important when editing the entry from the other
		// calendar view! If we edit locally, this is not a problem since we
		// just modified our own entry.
		if (pCurTraining)
		{
			if (*pCurTraining == *pTraining
			|| pCurTraining->GetDate() == pTraining->GetDate())
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

	s_SortInfo.pThis = this;
	s_SortInfo.nCol = m_SortColumn.GetColumn();
	m_Ctrl->SortItems(CompareTraining, 0);
	m_Ctrl->SetColumnSort(abs(m_SortColumn.GetColumn())-1, m_SortColumn.GetColumn());
	// Now make sure the selected item is visible.
	if (0 <= m_Ctrl->GetFirstSelected())
		m_Ctrl->Focus(m_Ctrl->GetFirstSelected());

	// Cleanup.
	m_Ctrl->Thaw();
	m_Ctrl->Refresh();
}


void CAgilityBookTrainingView::OnCtrlColumnClick(wxListEvent& evt)
{
	if (!m_Ctrl)
		return;
	m_Ctrl->SetColumnSort(abs(m_SortColumn.GetColumn())-1, 0);
	int nBackwards = 1;
	if (m_SortColumn.GetColumn() == evt.GetColumn() + 1)
		nBackwards = -1;
	m_SortColumn.SetColumn((evt.GetColumn() + 1) * nBackwards);
	s_SortInfo.pThis = this;
	s_SortInfo.nCol = m_SortColumn.GetColumn();
	m_Ctrl->SortItems(CompareTraining, 0);
	m_Ctrl->SetColumnSort(abs(m_SortColumn.GetColumn())-1, m_SortColumn.GetColumn());
}


void CAgilityBookTrainingView::OnCtrlItemActivated(wxListEvent& evt)
{
	OnCmd(ID_AGILITY_EDIT_TRAINING);
}


void CAgilityBookTrainingView::OnCtrlKeyDown(wxKeyEvent& evt)
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
		OnCmd(ID_AGILITY_EDIT_TRAINING);
		break;
	}
}


void CAgilityBookTrainingView::OnViewContextMenu(wxContextMenuEvent& evt)
{
	if (!m_Ctrl)
		return;
	wxPoint point;
	if (GetMenuPosition(point, *m_Ctrl, evt))
	{
		wxMenu* menu = CreatePopup(IdMenuTraining);
		m_Ctrl->PopupMenu(menu, point);
		delete menu;
	}
	else
		evt.Skip();
}


void CAgilityBookTrainingView::OnViewUpdateCmd(wxUpdateUIEvent& evt)
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
		evt.Enable(CClipboardDataReader::IsFormatAvailable(eFormatLog));
		break;
	case wxID_SELECTALL:
		evt.Enable(m_Ctrl->CanSelectAll());
		break;
	case wxID_FIND:
		evt.Enable(true);
		break;
	case ID_EDIT_FIND_NEXT:
		evt.Enable(true);
		break;
	case ID_EDIT_FIND_PREVIOUS:
		evt.Enable(true);
		break;
	case ID_AGILITY_EDIT_TRAINING:
		evt.Enable(1 == m_Ctrl->GetSelectedItemCount());
		break;
	case ID_AGILITY_DELETE_TRAINING:
		evt.Enable(0 < m_Ctrl->GetSelectedItemCount());
		break;
	case ID_VIEW_CUSTOMIZE:
		evt.Enable(true);
		break;
	}
}


bool CAgilityBookTrainingView::OnCmd(int id)
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
				std::vector<CAgilityBookTrainingViewDataPtr> items;
				for (std::vector<long>::iterator iterData = indices.begin(); iterData != indices.end(); ++iterData)
				{
					CAgilityBookTrainingViewDataPtr pData = GetItemTrainingData(*iterData);
					if (pData)
						items.push_back(pData);
				}
				ARBDate date;
				for (std::vector<CAgilityBookTrainingViewDataPtr>::iterator iter = items.begin(); iter != items.end(); ++iter)
				{
					// Currently, we don't need to worry if this is visible. The only filtering
					// is on name/date. So they can see the item that's being duped, which means
					// the new one is visible too.
					ARBTrainingPtr training = (*iter)->GetTraining()->Clone();
					GetDocument()->Book().GetTraining().AddTraining(training);
					GetDocument()->Book().GetTraining().sort();
					date = training->GetDate();
				}
				if (0 < items.size())
				{
					LoadData();
					GetDocument()->Modify(true);
					SetCurrentDate(date);
				}
			}
		}
		break;

	case wxID_CUT:
		OnCmd(wxID_COPY);
		OnCmd(ID_AGILITY_DELETE_TRAINING);
		break;

	case wxID_COPY:
		{
			std::vector<long> indices;
			if (0 < m_Ctrl->GetSelection(indices))
			{
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

				ElementNodePtr tree(ElementNode::New(CLIPDATA));

				// Now all the data.
				for (std::vector<long>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
				{
					CAgilityBookTrainingViewDataPtr pData = GetItemTrainingData(*iter);
					if (pData)
						pData->GetTraining()->Save(tree);
					std::vector<std::wstring> line;
					m_Ctrl->GetPrintLine((*iter), line);
					table.StartLine();
					for (int i = 0; i < static_cast<int>(line.size()); ++i)
					{
						table.Cell(i, line[i]);
					}
					table.EndLine();
				}

				clpData.AddData(eFormatLog, tree);
				clpData.AddData(table);
				clpData.CommitData();
			}
		}
		break;

	case wxID_PASTE:
		{
			bool bLoaded = false;
			ElementNodePtr tree(ElementNode::New());
			CClipboardDataReader clpData;
			if (clpData.GetData(eFormatLog, tree))
			{
				if (CLIPDATA == tree->GetName())
				{
					for (int i = 0; i < tree->GetElementCount(); ++i)
					{
						ElementNodePtr element = tree->GetElementNode(i);
						if (!element)
							continue;
						if (element->GetName() == TREE_TRAINING)
						{
							ARBTrainingPtr pLog(ARBTraining::New());
							CErrorCallback err;
							if (pLog->Load(element, ARBAgilityRecordBook::GetCurrentDocVersion(), err))
							{
								bLoaded = true;
								GetDocument()->Book().GetTraining().AddTraining(pLog);
							}
						}
					}
				}
			}
			clpData.Close();
			if (bLoaded)
			{
				GetDocument()->Book().GetTraining().sort();
				LoadData();
				GetDocument()->Modify(true);
			}
		}
		break;

	case wxID_SELECTALL:
		m_Ctrl->SelectAll();
		break;

	case wxID_FIND:
		{
			CDlgFind dlg(m_Callback, m_Ctrl);
			dlg.ShowModal();
		}
		break;

	case ID_EDIT_FIND_NEXT:
		{
			m_Callback.SearchDown(true);
			if (m_Callback.Text().empty())
				OnCmd(wxID_FIND);
			else
				m_Callback.Search(NULL);
		}
		break;

	case ID_EDIT_FIND_PREVIOUS:
		{
			m_Callback.SearchDown(false);
			if (m_Callback.Text().empty())
				OnCmd(wxID_FIND);
			else
				m_Callback.Search(NULL);
		}
		break;

	case ID_AGILITY_EDIT_TRAINING:
		{
			CAgilityBookTrainingViewDataPtr pData = GetItemTrainingData(m_Ctrl->GetFirstSelected());
			if (pData)
			{
				CDlgTraining dlg(pData->GetTraining(), GetDocument());
				if (wxID_OK == dlg.ShowModal())
				{
					GetDocument()->Book().GetTraining().sort();
					LoadData();
					GetDocument()->Modify(true);
					m_Ctrl->Refresh();
					int nColumnCount = m_Ctrl->GetColumnCount();
					for (int i = 0; i < nColumnCount; ++i)
						m_Ctrl->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
				}
			}
		}
		break;

	case ID_AGILITY_DELETE_TRAINING:
		{
			std::vector<long> indices;
			if (0 < m_Ctrl->GetSelection(indices))
			{
				std::vector<CAgilityBookTrainingViewDataPtr> items;
				for (std::vector<long>::iterator iter = indices.begin(); iter != indices.end(); ++iter)
				{
					CAgilityBookTrainingViewDataPtr pData = GetItemTrainingData(*iter);
					if (pData)
						GetDocument()->Book().GetTraining().DeleteTraining(pData->GetTraining());
				}
				GetDocument()->Modify(true);
				LoadData();
			}
		}
		break;

	case ID_VIEW_CUSTOMIZE:
		{
			CDlgAssignColumns dlg(CAgilityBookOptions::eView, m_Ctrl, GetDocument(), IO_TYPE_VIEW_TRAINING_LIST);
			dlg.ShowModal();
		}
		break;
	}
	return bHandled;
}


void CAgilityBookTrainingView::OnViewCmd(wxCommandEvent& evt)
{
	OnCmd(evt.GetId());
}


void CAgilityBookTrainingView::OnPrint(wxCommandEvent& evt)
{
	wxGetApp().GetHtmlPrinter()->PrintText(StringUtil::stringWX(m_Ctrl->GetPrintDataAsHtmlTable()));
}


void CAgilityBookTrainingView::OnPreview(wxCommandEvent& evt)
{
	wxGetApp().GetHtmlPrinter()->PreviewText(StringUtil::stringWX(m_Ctrl->GetPrintDataAsHtmlTable()));
}
