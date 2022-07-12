#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Common interface for all views.
 * @author David Connet
 *
 * Revision History
 * 2009-01-01 Ported to wxWidgets.
 */

#include "AgilityBookDoc.h"
#include <wx/docview.h>
class wxListEvent;
class wxTreeEvent;


namespace dconSoft
{
class CBasePanel;
namespace ARBWin
{
class CListCtrl;
class CTreeCtrl;
} // namespace ARBWin


/// All views derive from this. We specifically do not want them calling
/// wxDocument::DeleteDocument when closing. An SDI document will be deleted
/// by the docmanager when closing.
class CAgilityBookBaseView : public wxView
{
	DECLARE_CLASS(CAgilityBookBaseView)

public:
	CAgilityBookDoc* GetDocument() const
	{
		return dynamic_cast<CAgilityBookDoc*>(wxView::GetDocument());
	}
	// Prevent the view from calling wxDocument::DeleteDocument
	// (wxView override)
	bool OnClose(bool /*deleteWindow*/) override
	{
		Activate(false);
		return true;
	}

	// These handle doc/view destruction so we can shut down properly.
	// (virtual starting here)
	virtual void DetachView()
	{
	}

	/**
	 * Used so the current view can report whether it is filtering or not.
	 */
	virtual bool IsFiltered() const
	{
		return false;
	}
	/**
	 * Get a status message for the current view.
	 */
	virtual bool GetMessage(std::wstring& msg) const
	{
		return false;
	}
	/**
	 * Get a second status message for the current view.
	 */
	virtual bool GetMessage2(std::wstring& msg) const
	{
		return false;
	}
};


/// All views except CTabView derive from this. CTabView is created via the
/// doc/view architecture. We create all the other views and attach them.
class CAgilityBookBaseExtraView : public CAgilityBookBaseView
{
	DECLARE_CLASS(CAgilityBookBaseExtraView)
	DECLARE_NO_COPY_IMPLEMENTED(CAgilityBookBaseExtraView)

public:
	CAgilityBookBaseExtraView(CTabView* pTabView, wxDocument* doc);
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
	virtual bool HasPrevPane() const
	{
		return false;
	}
	virtual bool PrevPane()
	{
		return false;
	}
	virtual bool HasNextPane() const
	{
		return false;
	}
	virtual bool NextPane()
	{
		return false;
	}

	// The base class implements, we want to make sure all these classes override
	virtual void DetachView() = 0;

	virtual bool AllowStatusContext(int field) const = 0;

	void OnActivateView(bool activate, wxView* activeView, wxView* deactiveView) override;

	virtual void DoActivateView(bool activate, wxView* activeView, wxView* deactiveView) = 0;

	void UpdateMessages();

protected:
	// Returns points in client coordinates
	bool GetMenuPosition(wxPoint& outPos, ARBWin::CListCtrl& ctrl, wxContextMenuEvent const& evt);
	bool GetMenuPosition(wxPoint& outPos, ARBWin::CListCtrl& ctrl, wxListEvent const& evt);
	bool GetMenuPosition(wxPoint& outPos, ARBWin::CTreeCtrl& ctrl, wxContextMenuEvent const& evt);
	bool GetMenuPosition(wxPoint& outPos, ARBWin::CTreeCtrl& ctrl, wxTreeEvent const& evt);
	bool GetMenuPosition(wxPoint& outPos, wxWindow const* ctrl, wxContextMenuEvent const& evt);

	CTabView* m_pTabView;
	CBasePanel* m_parentPanel;
	bool m_bIgnore;
};

} // namespace dconSoft
