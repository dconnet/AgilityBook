/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Panels in tab control (views go in these)
 * @author David Connet
 *
 * Revision History
 * @li 2011-12-22 DRC Switch to using Bind on wx2.9+.
 * @li 2011-02-12 DRC Don't save sash position if it hasn't been initialized.
 * @li 2009-07-25 DRC Set a minimum splitter width.
 * @li 2008-12-14 DRC Created
 */

#include "stdafx.h"
#include "AgilityBookPanels.h"

#include "AgilityBookCalendarListView.h"
#include "AgilityBookCalendarView.h"
#include "AgilityBookHtmlView.h"
#include "AgilityBookOptions.h"
#include "AgilityBookPointsView.h"
#include "AgilityBookRunsView.h"
#include "AgilityBookTrainingView.h"
#include "AgilityBookTreeView.h"
#include "CommonView.h"
#include "RegItems.h"

#include "ARB/StringUtil.h"
#include <wx/config.h>
#include <wx/object.h>
#include <wx/splitter.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

// Default splitter widths
#define DEFAULT_RUN_WIDTH	200L
#define MIN_RUN_WIDTH		50L
#define DEFAULT_CAL_WIDTH	300L
#define MIN_CAL_WIDTH		50L


CBasePanel::CBasePanel(wxWindow* parent, std::wstring const& helpText)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER)
	, m_views()
	, m_lastActiveView(NULL)
{
#if wxUSE_HELP
	if (!helpText.empty())
		SetHelpText(helpText.c_str());
#endif
}


CBasePanel::~CBasePanel()
{
	DetachViews();
}


void CBasePanel::ActivateView()
{
	if (m_lastActiveView)
		m_lastActiveView->Activate(true);
	else if (0 < m_views.size())
		m_views[0]->Activate(true);
}


bool CBasePanel::ActivateView(int iSplit)
{
	if (0 <= iSplit && iSplit < static_cast<int>(m_views.size()))
	{
		m_lastActiveView = m_views[iSplit];
		ActivateView();
		return true;
	}
	return false;
}


void CBasePanel::DetachViews(std::vector<CAgilityBookBaseExtraView*>& outViews)
{
	outViews = m_views;
	DetachViews();
}


void CBasePanel::OnActivateView(CAgilityBookBaseExtraView* activeView)
{
	m_lastActiveView = activeView;
}


void CBasePanel::DetachViews()
{
	if (0 < m_views.size())
	{
		for (std::vector<CAgilityBookBaseExtraView*>::iterator iter = m_views.begin(); iter != m_views.end(); ++iter)
		{
			(*iter)->DetachView();
		}
		m_views.clear();
		m_lastActiveView = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////

CAgilityBookPanelRuns::CAgilityBookPanelRuns(
		CTabView* pTabView,
		wxWindow *parent,
		wxDocument* doc,
		long flags,
		std::vector<CAgilityBookBaseExtraView*> const& inViews)
	: CBasePanel(parent, StringUtil::stringW(_("PanelRuns")))
	, m_bInit(false)
	, m_splitter(NULL)
{
	m_views = inViews;
	bool bAttachViews = m_views.empty();

	m_splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D);
	BIND_OR_CONNECT_CTRL(m_splitter, wxEVT_IDLE, wxIdleEventHandler, CAgilityBookPanelRuns::SplitterOnIdle);

	wxPanel* panel1 = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);

	if (bAttachViews)
	{
		CAgilityBookBaseExtraView* pView = new CAgilityBookTreeView(pTabView, doc);
		assert(pView->HasNextPane() && pView->HasPrevPane());
		m_views.push_back(pView);
	}
	m_views[0]->Create(this, panel1, doc, flags, bSizer1, 1, wxALL|wxEXPAND, 0);

	panel1->SetSizer(bSizer1);
	panel1->Layout();
	bSizer1->Fit(panel1);

	wxPanel* panel2 = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
	if (bAttachViews)
	{
		CAgilityBookBaseExtraView* pView = new CAgilityBookRunsView(pTabView, doc);
		assert(pView->HasNextPane() && pView->HasPrevPane());
		m_views.push_back(pView);
	}
	m_views[1]->Create(this, panel2, doc, flags, bSizer2, 1, wxALL|wxEXPAND, 0);
	panel2->SetSizer(bSizer2);
	panel2->Layout();
	bSizer2->Fit(panel2);

	m_splitter->SplitVertically(panel1, panel2);

	wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);
	sizerPanel->Add(m_splitter, 1, wxEXPAND, 0);
	SetSizer(sizerPanel);
}


CAgilityBookPanelRuns::~CAgilityBookPanelRuns()
{
	if (m_bInit)
	{
		wxConfig::Get()->Write(CFG_SETTINGS_SPLITCX, m_splitter->GetSashPosition());
	}
}


void CAgilityBookPanelRuns::SplitterOnIdle(wxIdleEvent&)
{
	long cx = wxConfig::Get()->Read(CFG_SETTINGS_SPLITCX, DEFAULT_RUN_WIDTH);
	if (cx < MIN_RUN_WIDTH)
		cx = MIN_RUN_WIDTH;
	m_splitter->SetSashPosition(cx);
	m_bInit = true;
	UNBIND_OR_DISCONNECT_CTRL(m_splitter, wxEVT_IDLE, wxIdleEventHandler, CAgilityBookPanelRuns::SplitterOnIdle);
}

/////////////////////////////////////////////////////////////////////////////

CAgilityBookPanelPoints::CAgilityBookPanelPoints(
		CTabView* pTabView,
		wxWindow* parent,
		wxDocument* doc,
		long flags,
		std::vector<CAgilityBookBaseExtraView*> const& inViews)
	: CBasePanel(parent, StringUtil::stringW(_("PanelPoints")))
{
	m_views = inViews;
	bool bAttachViews = m_views.empty();

	wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);

	if (bAttachViews)
	{
		CAgilityBookBaseExtraView* pView;
		if (CAgilityBookOptions::ShowHtmlPoints())
			pView = new CAgilityBookHtmlView(pTabView, doc);
		else
			pView = new CAgilityBookPointsView(pTabView, doc);
		assert(!pView->HasNextPane() && !pView->HasPrevPane());
		m_views.push_back(pView);
	}
	m_views[0]->Create(this, this, doc, flags, sizerPanel, 1, wxEXPAND, 0);
	SetSizer(sizerPanel);
}

/////////////////////////////////////////////////////////////////////////////

CAgilityBookPanelCalendar::CAgilityBookPanelCalendar(
		CTabView* pTabView,
		wxWindow* parent,
		wxDocument* doc,
		long flags,
		std::vector<CAgilityBookBaseExtraView*> const& inViews)
	: CBasePanel(parent, StringUtil::stringW(_("PanelCal")))
	, m_bInit(false)
	, m_splitter(NULL)
{
	m_views = inViews;
	bool bAttachViews = m_views.empty();

	m_splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D);
	BIND_OR_CONNECT_CTRL(m_splitter, wxEVT_IDLE, wxIdleEventHandler, CAgilityBookPanelCalendar::SplitterOnIdle);

	wxPanel* panel1 = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer1 = new wxBoxSizer(wxVERTICAL);
	if (bAttachViews)
	{
		CAgilityBookBaseExtraView* pView = new CAgilityBookCalendarListView(pTabView, doc);
		assert(pView->HasNextPane() && pView->HasPrevPane());
		m_views.push_back(pView);
	}
	m_views[0]->Create(this, panel1, doc, flags, bSizer1, 1, wxALL|wxEXPAND, 0);
	panel1->SetSizer(bSizer1);
	panel1->Layout();
	bSizer1->Fit(panel1);

	wxPanel* panel2 = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer* bSizer2 = new wxBoxSizer(wxVERTICAL);
	if (bAttachViews)
	{
		CAgilityBookBaseExtraView* pView = new CAgilityBookCalendarView(pTabView, doc);
		assert(pView->HasNextPane() && pView->HasPrevPane());
		m_views.push_back(pView);
	}
	m_views[1]->Create(this, panel2, doc, flags, bSizer2, 1, wxALL|wxEXPAND, 0);
	panel2->SetSizer(bSizer2);
	panel2->Layout();
	bSizer2->Fit(panel2);

	m_splitter->SplitVertically(panel1, panel2);

	wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);
	sizerPanel->Add(m_splitter, 1, wxEXPAND, 0);
	SetSizer(sizerPanel);
}


CAgilityBookPanelCalendar::~CAgilityBookPanelCalendar()
{
	if (m_bInit)
	{
		wxConfig::Get()->Write(CFG_SETTINGS_SPLITCX2, m_splitter->GetSashPosition());
	}
}


void CAgilityBookPanelCalendar::SplitterOnIdle(wxIdleEvent&)
{
	long cx = wxConfig::Get()->Read(CFG_SETTINGS_SPLITCX2, DEFAULT_CAL_WIDTH);
	if (cx < MIN_CAL_WIDTH)
		cx = MIN_CAL_WIDTH;
	m_splitter->SetSashPosition(cx);
	m_bInit = true;
	UNBIND_OR_DISCONNECT_CTRL(m_splitter, wxEVT_IDLE, wxIdleEventHandler, CAgilityBookPanelCalendar::SplitterOnIdle);
}

/////////////////////////////////////////////////////////////////////////////

CAgilityBookPanelTraining::CAgilityBookPanelTraining(
		CTabView* pTabView,
		wxWindow* parent,
		wxDocument* doc,
		long flags,
		std::vector<CAgilityBookBaseExtraView*> const& inViews)
	: CBasePanel(parent, StringUtil::stringW(_("PanelTraining")))
{
	m_views = inViews;
	bool bAttachViews = m_views.empty();

	wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);
	if (bAttachViews)
	{
		CAgilityBookBaseExtraView* pView = new CAgilityBookTrainingView(pTabView, doc);
		assert(!pView->HasNextPane() && !pView->HasPrevPane());
		m_views.push_back(pView);
	}
	m_views[0]->Create(this, this, doc, flags, sizerPanel, 1, wxEXPAND, 0);
	SetSizer(sizerPanel);
}
