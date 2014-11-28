#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Panels in tab control (views go in these)
 * @author David Connet
 *
 * Revision History
 * 2012-09-29 Strip the Runs View.
 * 2008-12-14 Created
 */

#include <vector>
class wxDocument;
class wxSplitterWindow;
class CAgilityBookBaseExtraView;
class CTabView;


class CBasePanel : public wxPanel
{
	DECLARE_NO_COPY_CLASS(CBasePanel)

public:
	CBasePanel(
			wxWindow* parent,
			std::wstring const& helpText);
	virtual ~CBasePanel();

	void ActivateView();
	bool ActivateView(int iSplit);

	// Used to remove the views and reattach when changing the tabview style
	void DetachViews(std::vector<CAgilityBookBaseExtraView*>& outViews);

	void OnActivateView(CAgilityBookBaseExtraView* activeView);

protected:
	void DetachViews();

	std::vector<CAgilityBookBaseExtraView*> m_views;
	CAgilityBookBaseExtraView* m_lastActiveView; // To determine view in splitters
};


class CAgilityBookPanelRuns : public CBasePanel
{
public:
	CAgilityBookPanelRuns(
			CTabView* pTabView,
			wxWindow* parent,
			wxDocument* doc,
			long flags,
			std::vector<CAgilityBookBaseExtraView*> const& inViews);
	~CAgilityBookPanelRuns();

private:
	bool m_bInit;
	wxSplitterWindow* m_splitter;
	void SplitterOnIdle(wxIdleEvent&);
};


class CAgilityBookPanelPoints : public CBasePanel
{
public:
	CAgilityBookPanelPoints(
			CTabView* pTabView,
			wxWindow* parent,
			wxDocument* doc,
			long flags,
			std::vector<CAgilityBookBaseExtraView*> const& inViews);
};


class CAgilityBookPanelCalendar : public CBasePanel
{
public:
	CAgilityBookPanelCalendar(
			CTabView* pTabView,
			wxWindow* parent,
			wxDocument* doc,
			long flags,
			std::vector<CAgilityBookBaseExtraView*> const& inViews);
	~CAgilityBookPanelCalendar();

private:
	bool m_bInit;
	wxSplitterWindow* m_splitter;
	void SplitterOnIdle(wxIdleEvent&);
};


class CAgilityBookPanelTraining : public CBasePanel
{
public:
	CAgilityBookPanelTraining(
			CTabView* pTabView,
			wxWindow* parent,
			wxDocument* doc,
			long flags,
			std::vector<CAgilityBookBaseExtraView*> const& inViews);
};
