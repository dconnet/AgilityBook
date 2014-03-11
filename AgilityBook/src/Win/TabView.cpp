/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Tab view
 * @author David Connet
 *
 * Revision History
 * 2012-01-07 Fix tab type/orientation persistence.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-08-03 Fix tab setting so view is properly activated.
 * 2009-01-06 Ported to wxWidgets.
 * 2007-11-27 Check that the html view was actually created.
 * 2004-06-24 Removed sending initialupdate messages (redundant)
 * 2003-12-07 Fixed a crash when opening a bad ARB file version.
 * 2003-09-21 Added training log.
 */

#include "stdafx.h"
#include "TabView.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookPanels.h"
#include "ImageManager.h"
#include "MainFrm.h"
#include "RegItems.h"
#include <wx/choicebk.h>
#include <wx/config.h>
#include <wx/listbook.h>
#include <wx/notebook.h>
#include <wx/treebook.h>
#include <wx/toolbook.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


IMPLEMENT_DYNAMIC_CLASS(CTabView, wxView)


static long GetDefaultBook()
{
#if wxUSE_NOTEBOOK
	return ID_BOOK_NOTEBOOK;
#elif wxUSE_LISTBOOK
	return ID_BOOK_LISTBOOK;
#elif wxUSE_CHOICEBOOK
	return ID_BOOK_CHOICEBOOK;
#elif wxUSE_TREEBOOK
	return ID_BOOK_TREEBOOK;
#elif wxUSE_TOOLBOOK
	return ID_BOOK_TOOLBOOK;
#elif
	#error "No books enabled in wxWidgets build!"
#endif
}


CTabView::CTabView()
	: m_frame(NULL)
	, m_type(GetDefaultBook() - ID_BOOK_FIRST)
	, m_orient(ID_ORIENT_TOP - ID_ORIENT_FIRST)
	, m_sizerFrame(NULL)
	, m_ctrlBook(NULL)
	, m_imageList(16,16)
	, m_bIgnoreEvents(false)
{
	m_imageList.Add(CImageManager::Get()->TabRuns());
	m_imageList.Add(CImageManager::Get()->TabPoints());
	m_imageList.Add(CImageManager::Get()->TabCalendar());
	m_imageList.Add(CImageManager::Get()->TabTraining());
	m_type = wxConfig::Get()->Read(CFG_SETTINGS_VIEWTYPE, m_type);
	if (m_type < 0 || m_type > ID_BOOK_LAST - ID_BOOK_FIRST)
	{
		// Backwards compatibility fix. Since menu ids might change by adding
		// a new item, we need to normalize. We don't know what the offset was.
		m_type = GetDefaultBook() - ID_BOOK_FIRST;
	}
	m_type += ID_BOOK_FIRST; // Restore to internal menu id.
	m_orient = wxConfig::Get()->Read(CFG_SETTINGS_VIEWORIENT, m_orient);
	if (m_orient < 0 || m_orient > ID_ORIENT_LAST - ID_ORIENT_FIRST)
	{
		// Backwards compatibility fix. Since menu ids might change by adding
		// a new item, we need to normalize. We don't know what the offset was.
		m_orient = ID_ORIENT_TOP - ID_ORIENT_FIRST;
	}
	m_orient += ID_ORIENT_FIRST; // Restore to internal menu id.
}


CTabView::~CTabView()
{
	if (m_ctrlBook)
	{
		wxConfig::Get()->Write(CFG_SETTINGS_VIEWTYPE, m_type - ID_BOOK_FIRST);
		wxConfig::Get()->Write(CFG_SETTINGS_VIEWORIENT, m_orient - ID_ORIENT_FIRST);
		wxConfig::Get()->Write(CFG_SETTINGS_VIEW, m_ctrlBook->GetSelection());
		m_ctrlBook->Destroy();
		m_ctrlBook = NULL;
	}
	if (m_frame && m_sizerFrame)
	{
		m_frame->SetSizer(NULL);
		m_sizerFrame = NULL;
	}
}


void CTabView::OnType(int id)
{
	m_type = id;
	RecreateBook(GetDocument(), 0, false);
}


void CTabView::OnOrient(int id)
{
	m_orient = id;
	RecreateBook(GetDocument(), 0, false);
}


class CIgnore
{
	DECLARE_NO_COPY_IMPLEMENTED(CIgnore);
public:
	CIgnore(bool& ignore) : m_Ignore(ignore) { m_Ignore = true; }
	~CIgnore()								{ reset(); }
	void reset()							{ m_Ignore = false; }
private:
	bool& m_Ignore;
};


void CTabView::RecreateBook(wxDocument* doc, long inFlags, bool bOnCreate)
{
	int flags;
	switch (m_orient)
	{
	default:
	case ID_ORIENT_TOP:
		flags = wxBK_TOP;
		break;
	case ID_ORIENT_BOTTOM:
		flags = wxBK_BOTTOM;
		break;
	case ID_ORIENT_LEFT:
		flags = wxBK_LEFT;
		break;
	case ID_ORIENT_RIGHT:
		flags = wxBK_RIGHT;
		break;
	}

	if (m_type == ID_BOOK_NOTEBOOK)
		flags |= wxNB_MULTILINE;

	wxBookCtrlBase *oldBook = m_ctrlBook;

	m_ctrlBook = NULL;
	CIgnore ignore(m_bIgnoreEvents);

	switch (m_type)
	{
#if wxUSE_NOTEBOOK
	case ID_BOOK_NOTEBOOK:
		m_ctrlBook = new wxNotebook(m_frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, flags);
		BIND_OR_CONNECT_CTRL(m_ctrlBook, wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler, CTabView::OnNotebookChanged);
		break;
#endif
#if wxUSE_LISTBOOK
	case ID_BOOK_LISTBOOK:
		m_ctrlBook = new wxListbook(m_frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, flags);
		BIND_OR_CONNECT_CTRL(m_ctrlBook, wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED, wxListbookEventHandler, CTabView::OnListbookChanged);
		break;
#endif
#if wxUSE_CHOICEBOOK
	case ID_BOOK_CHOICEBOOK:
		m_ctrlBook = new wxChoicebook(m_frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, flags);
		BIND_OR_CONNECT_CTRL(m_ctrlBook, wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED, wxChoicebookEventHandler, CTabView::OnChoicebookChanged);
		break;
#endif
#if wxUSE_TREEBOOK
	case ID_BOOK_TREEBOOK:
		m_ctrlBook = new wxTreebook(m_frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, flags);
		BIND_OR_CONNECT_CTRL(m_ctrlBook, wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED, wxTreebookEventHandler, CTabView::OnTreebookChanged);
		break;
#endif
#if wxUSE_TOOLBOOK
	case ID_BOOK_TOOLBOOK:
		m_ctrlBook = new wxToolbook(m_frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, flags);
		BIND_OR_CONNECT_CTRL(m_ctrlBook, wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGED, wxToolbookEventHandler, CTabView::OnToolbookChanged);
		break;
#endif
	}

	if (!m_ctrlBook)
		return;

	m_ctrlBook->SetImageList(&m_imageList);

	std::vector<CAgilityBookBaseExtraView*> views[4];
	int sel = wxNOT_FOUND;
	if (oldBook)
	{
		sel = oldBook->GetSelection();
		for (size_t i = 0; i < 4; ++i)
		{
			CBasePanel* panel = wxDynamicCast(oldBook->GetPage(i), CBasePanel);
			panel->DetachViews(views[i]);
		}
		m_sizerFrame->Detach(oldBook);
		oldBook->Destroy();
	}
	else
		sel = wxConfig::Get()->Read(CFG_SETTINGS_VIEW, 0L);

	CBasePanel* pages[4];
	m_ctrlBook->AddPage(
		(pages[IDX_PANE_RUNS] = new CAgilityBookPanelRuns(this, m_ctrlBook, doc, inFlags, views[IDX_PANE_RUNS])),
		_("IDS_RUNS"), false, IDX_PANE_RUNS);
	m_ctrlBook->AddPage(
		(pages[IDX_PANE_POINTS] = new CAgilityBookPanelPoints(this, m_ctrlBook, doc, inFlags, views[IDX_PANE_POINTS])),
		_("IDS_POINTS"), false, IDX_PANE_POINTS);
	m_ctrlBook->AddPage(
		(pages[IDX_PANE_CALENDAR] = new CAgilityBookPanelCalendar(this, m_ctrlBook, doc, inFlags, views[IDX_PANE_CALENDAR])),
		_("IDS_CALENDAR"), false, IDX_PANE_CALENDAR);
	m_ctrlBook->AddPage(
		(pages[IDX_PANE_LOG] = new CAgilityBookPanelTraining(this, m_ctrlBook, doc, inFlags, views[IDX_PANE_LOG])),
		_("IDS_TRAINING"), false, IDX_PANE_LOG);
	ignore.reset();

	if (sel != wxNOT_FOUND)
	{
		if (!bOnCreate)
			doc->UpdateAllViews();
		m_ctrlBook->ChangeSelection(sel);
		pages[sel]->ActivateView();
	}

	m_sizerFrame->Add(m_ctrlBook, 1, wxEXPAND | wxALL, 0);
	m_sizerFrame->Layout();
}


bool CTabView::ShowPointsAsHtml(bool bHtml)
{
	if (m_ctrlBook)
	{
		wxBusyCursor wait;
		CAgilityBookDoc* pDoc = GetDocument();
		int sel = m_ctrlBook->GetSelection();
		CBasePanel* panel = wxDynamicCast(m_ctrlBook->GetPage(IDX_PANE_POINTS), CBasePanel);
		std::vector<CAgilityBookBaseExtraView*> views;
		panel->DetachViews(views);
		for (std::vector<CAgilityBookBaseExtraView*>::iterator iView = views.begin();
			iView != views.end();
			++iView)
		{
			pDoc->RemoveView(*iView);
			delete *iView;
		}
		views.clear();
		m_ctrlBook->DeletePage(IDX_PANE_POINTS);
		CBasePanel* page = new CAgilityBookPanelPoints(this, m_ctrlBook, pDoc, 0, views);
		m_ctrlBook->InsertPage(IDX_PANE_POINTS, page, _("IDS_POINTS"), false, IDX_PANE_POINTS);
		CUpdateHint hint(UPDATE_POINTS_VIEW);
		pDoc->UpdateAllViews(this, &hint);
		if (sel == IDX_PANE_POINTS)
		{
			m_ctrlBook->ChangeSelection(sel);
			page->ActivateView();
		}
		return true;
	}
	return false;
}


int CTabView::GetCurTab() const
{
	if (!m_ctrlBook)
		return -1;
	return m_ctrlBook->GetSelection();
}


int CTabView::SetCurTab(int index)
{
	if (!m_ctrlBook)
		return -1;
	return m_ctrlBook->SetSelection(index);
}


void CTabView::OnChangeFilename()
{
	// The default simply puts the filename in the caption.
	// I want the application name too.
	m_frame->SetLabel(GetDocument()->GetDocumentManager()->MakeFrameTitle(GetDocument()));
}


bool CTabView::OnCreate(wxDocument* doc, long flags)
{
	m_frame = wxDynamicCast(wxGetApp().GetTopWindow(), CMainFrame);
	SetFrame(m_frame);
	assert(m_frame);
	m_sizerFrame = new wxBoxSizer(wxVERTICAL);
	m_frame->SetSizer(m_sizerFrame);
	RecreateBook(doc, flags, true);
	// On initial creation, this isn't needed. But if we open/create another
	// document, the tabview isn't visible until the frame resizes.
	m_frame->SendSizeEvent();
	return wxView::OnCreate(doc, flags);
}


void CTabView::OnDraw(wxDC* dc)
{
}


void CTabView::OnUpdate(wxView* sender, wxObject* inHint)
{
	CUpdateHint* hint = NULL;
	if (inHint)
		hint = wxDynamicCast(inHint, CUpdateHint);
	if (hint && hint->IsEqual(UPDATE_LANG_CHANGE))
	{
		m_ctrlBook->SetPageText(IDX_PANE_RUNS, _("IDS_RUNS"));
		m_ctrlBook->SetPageText(IDX_PANE_POINTS, _("IDS_POINTS"));
		m_ctrlBook->SetPageText(IDX_PANE_CALENDAR, _("IDS_CALENDAR"));
		m_ctrlBook->SetPageText(IDX_PANE_LOG, _("IDS_TRAINING"));
	}
}


void CTabView::OnBookCtrlChanged(wxBookCtrlEvent& evt)
{
	if (!m_bIgnoreEvents)
	{
		CBasePanel* panel = wxDynamicCast(m_ctrlBook->GetPage(evt.GetSelection()), CBasePanel);
		if (panel)
		{
			m_ctrlBook->ChangeSelection(evt.GetSelection());
			panel->ActivateView();
			// Swallow the event - or the base control will set the focus to the
			// page - which just undid our focus setting above
			return;
		}
	}
	evt.Skip();
}
