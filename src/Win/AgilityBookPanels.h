#pragma once

/*
 * Copyright © 2008-2009 David Connet. All Rights Reserved.
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
 * @brief Panels in tab control (views go in these)
 * @author David Connet
 *
 * Revision History
 * @li 2008-12-14 DRC Created
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
			wxString const& helpText);
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
