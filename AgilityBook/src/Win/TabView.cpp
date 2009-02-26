/*
 * Copyright � 2002-2009 David Connet. All Rights Reserved.
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
 * @brief Tab view
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-06 DRC Ported to wxWidgets.
 * @li 2007-11-27 DRC Check that the html view was actually created.
 * @li 2004-06-24 DRC Removed sending initialupdate messages (redundant)
 * @li 2003-12-07 DRC Fixed a crash when opening a bad ARB file version.
 * @li 2003-09-21 DRC Added training log.
 */

#include "stdafx.h"
#include "TabView.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookPanels.h"
#include "AgilityBookTreeView.h"
#include "MainFrm.h"
#include <wx/choicebk.h>
#include <wx/config.h>
#include <wx/listbook.h>
#include <wx/notebook.h>
#include <wx/toolbook.h>
#include <wx/treebook.h>

#include "res/run.xpm"
#include "res/points.xpm"
#include "res/calendar.xpm"
#include "res/training.xpm"


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
	, m_type(GetDefaultBook())
	, m_orient(ID_ORIENT_TOP)
	, m_sizerFrame(NULL)
	, m_ctrlBook(NULL)
	, m_imageList(16,16)
	, m_bIgnoreEvents(false)
{
	m_imageList.Add(wxIcon(run_xpm));
	m_imageList.Add(wxIcon(points_xpm));
	m_imageList.Add(wxIcon(calendar_xpm));
	m_imageList.Add(wxIcon(training_xpm));
	m_type = wxConfig::Get()->Read(wxT("Settings/ViewType"), m_type);
	switch (m_type)
	{
	default:
		m_type = GetDefaultBook();
		break;
#if wxUSE_NOTEBOOK
	case ID_BOOK_NOTEBOOK:
#elif wxUSE_LISTBOOK
	case ID_BOOK_LISTBOOK:
#elif wxUSE_CHOICEBOOK
	case ID_BOOK_CHOICEBOOK:
#elif wxUSE_TREEBOOK
	case ID_BOOK_TREEBOOK:
#elif wxUSE_TOOLBOOK
	case ID_BOOK_TOOLBOOK:
#endif
		break;
	}
	m_orient = wxConfig::Get()->Read(wxT("Settings/ViewOrient"), m_orient);
	if (m_orient < ID_ORIENT_FIRST || m_orient >= ID_ORIENT_LAST)
		m_orient = ID_ORIENT_FIRST;
}


CTabView::~CTabView()
{
	if (m_ctrlBook)
	{
		wxConfig::Get()->Write(wxT("Settings/ViewType"), m_type);
		wxConfig::Get()->Write(wxT("Settings/ViewOrient"), m_orient);
		wxConfig::Get()->Write(wxT("Settings/View"), m_ctrlBook->GetSelection());
		m_ctrlBook->Destroy();
		m_ctrlBook = NULL;
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
		m_ctrlBook->Connect(m_ctrlBook->GetId(), wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler(CTabView::OnNotebookChanged), NULL, this);
		break;
#endif
#if wxUSE_LISTBOOK
	case ID_BOOK_LISTBOOK:
		m_ctrlBook = new wxListbook(m_frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, flags);
		m_ctrlBook->Connect(m_ctrlBook->GetId(), wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED, wxListbookEventHandler(CTabView::OnListbookChanged), NULL, this);
		break;
#endif
#if wxUSE_CHOICEBOOK
	case ID_BOOK_CHOICEBOOK:
		m_ctrlBook = new wxChoicebook(m_frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, flags);
		m_ctrlBook->Connect(m_ctrlBook->GetId(), wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED, wxChoicebookEventHandler(CTabView::OnChoicebookChanged), NULL, this);
		break;
#endif
#if wxUSE_TREEBOOK
	case ID_BOOK_TREEBOOK:
		m_ctrlBook = new wxTreebook(m_frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, flags);
		m_ctrlBook->Connect(m_ctrlBook->GetId(), wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED, wxTreebookEventHandler(CTabView::OnTreebookChanged), NULL, this);
		break;
#endif
#if wxUSE_TOOLBOOK
	case ID_BOOK_TOOLBOOK:
		m_ctrlBook = new wxToolbook(m_frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, flags);
		m_ctrlBook->Connect(m_ctrlBook->GetId(), wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGED, wxToolbookEventHandler(CTabView::OnToolbookChanged), NULL, this);
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
		sel = wxConfig::Get()->Read(wxT("Settings/View"), 0L);

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
	return m_ctrlBook->ChangeSelection(index);
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
		hint = reinterpret_cast<CUpdateHint*>(inHint);
	if (hint && hint->IsEqual(UPDATE_LANG_CHANGE))
	{
		m_ctrlBook->SetPageText(IDX_PANE_RUNS, _("IDS_RUNS"));
		m_ctrlBook->SetPageText(IDX_PANE_POINTS, _("IDS_POINTS"));
		m_ctrlBook->SetPageText(IDX_PANE_CALENDAR, _("IDS_CALENDAR"));
		m_ctrlBook->SetPageText(IDX_PANE_LOG, _("IDS_TRAINING"));
	}
}


void CTabView::OnBookCtrlChanged(wxBookCtrlBaseEvent& evt)
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
