#pragma once

/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @brief Common interface for all views.
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-01 DRC Ported to wxWidgets.
 */

#include "AgilityBookDoc.h"
#include <wx/docview.h>
class CBasePanel;
class wxListEvent;
class wxListView;
class wxTreeCtrl;
class wxTreeEvent;


/// All views derive from this. We specifically do not want them calling
/// wxDocument::DeleteDocument when closing. An SDI document will be deleted
/// by the docmanager when closing.
class CAgilityBookBaseView : public wxView
{
	DECLARE_CLASS(CAgilityBookBaseView)

public:
	CAgilityBookDoc* GetDocument() const
	{
		return (CAgilityBookDoc*)wxView::GetDocument();
	}
	// Prevent the view from calling wxDocument::DeleteDocument
	// (wxView override)
    virtual bool OnClose(bool /*deleteWindow*/)
	{
		Activate(false);
		return true;
	}

	// These handle doc/view destruction so we can shut down properly.
	// (virtual starting here)
	virtual void DetachView() {}

	/**
	 * Used so the current view can report whether it is filtering or not.
	 */
	virtual bool IsFiltered() const {return false;}
	/**
	 * Get a status message for the current view.
	 */
	virtual bool GetMessage(wxString& msg) const {return false;}
	/**
	 * Get a second status message for the current view.
	 */
	virtual bool GetMessage2(wxString& msg) const {return false;}
};


/// All views except CTabView derive from this. CTabView is created via the
/// doc/view architecture. We create all the other views and attach them.
class CAgilityBookBaseExtraView : public CAgilityBookBaseView
{
	DECLARE_CLASS(CAgilityBookBaseExtraView)
	CAgilityBookBaseExtraView(
			CTabView* pTabView,
			wxDocument* doc);

public:
	/// Derived class must call this at end of their work.
	virtual bool Create(
			CBasePanel* parentOfView,
			wxWindow* parentOfCtrl,
			wxDocument* doc,
			long flags,
			wxSizer* sizer,
			int proportion = 0,
			int sizerFlags = 0,
			int border = 0);

	virtual wxWindow* GetControl() = 0;
	virtual bool HasPrevPane() const	{return false;}
	virtual bool PrevPane()				{return false;}
	virtual bool HasNextPane() const	{return false;}
	virtual bool NextPane()				{return false;}

	// The base class implements, we want to make sure all these classes override
	virtual void DetachView() = 0;

	virtual bool AllowStatusContext(int field) const = 0;

	virtual void OnActivateView(
			bool activate,
			wxView* activeView,
			wxView* deactiveView);

	virtual void DoActivateView(
			bool activate,
			wxView* activeView,
			wxView* deactiveView) = 0;

	void UpdateMessages();

protected:
	// Returns points in client coordinates
	bool GetMenuPosition(
			wxPoint& outPos,
			wxListView& ctrl,
			wxContextMenuEvent const& evt);
	bool GetMenuPosition(
			wxPoint& outPos,
			wxListView& ctrl,
			wxListEvent const& evt);
	bool GetMenuPosition(
			wxPoint& outPos,
			wxTreeCtrl& ctrl,
			wxContextMenuEvent const& evt);
	bool GetMenuPosition(
			wxPoint& outPos,
			wxTreeCtrl& ctrl,
			wxTreeEvent const& evt);
	bool GetMenuPosition(
			wxPoint& outPos,
			wxWindow* ctrl,
			wxContextMenuEvent const& evt);

	CTabView* m_pTabView;
	CBasePanel* m_parentPanel;
	bool m_bIgnore;
};
