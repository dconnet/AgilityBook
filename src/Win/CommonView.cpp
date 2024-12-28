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

#include "stdafx.h"
#include "CommonView.h"

#include "AgilityBook.h"
#include "AgilityBookPanels.h"

#include "LibARBWin/Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBWin;

wxIMPLEMENT_CLASS(CAgilityBookBaseView, wxView)
wxIMPLEMENT_CLASS(CAgilityBookBaseExtraView, CAgilityBookBaseView)


CAgilityBookBaseExtraView::CAgilityBookBaseExtraView(CTabView* pTabView, wxDocument* doc)
	: m_pTabView(pTabView)
	, m_parentPanel(nullptr)
	, m_bIgnore(false)
{
	SetDocument(doc);
}


bool CAgilityBookBaseExtraView::Create(
	CBasePanel* parentOfView,
	wxWindow* parentOfCtrl,
	wxDocument* doc,
	long flags,
	wxSizer* sizer,
	int proportion,
	int sizerFlags,
	int border)
{
	m_parentPanel = parentOfView;
	if (!OnCreate(doc, flags))
		return false;
	sizer->Add(GetControl(), proportion, sizerFlags, border);
	return true;
}


void CAgilityBookBaseExtraView::OnActivateView(bool activate, wxView* activeView, wxView* deactiveView)
{
	// Derived controls may call SetFocus on their internal controls, which
	// may call wxView::Activate, which comes back in here. The internal
	// controls need to do this as focus may be set to them independently
	// of the view architecture.
	if (!m_bIgnore)
	{
		m_bIgnore = true;
		DoActivateView(activate, activeView, deactiveView);
		if (activate)
		{
			wxString msg;
			if (GetMessage(msg))
				wxGetApp().SetMessageText(msg, IsFiltered());
			if (GetMessage2(msg))
				wxGetApp().SetMessageText2(msg);
		}
		m_bIgnore = false;
	}
}


void CAgilityBookBaseExtraView::UpdateMessages()
{
	wxString msg;
	if (GetMessage(msg))
		wxGetApp().SetMessageText(msg, IsFiltered());
	if (GetMessage2(msg))
		wxGetApp().SetMessageText2(msg);
}


bool CAgilityBookBaseExtraView::GetMenuPosition(wxPoint& outPos, CListCtrl const& ctrl, wxContextMenuEvent const& evt)
{
	bool bOk = true;
	outPos = evt.GetPosition(); // in screen
	if (wxDefaultPosition == outPos)
	{
		// Adjust the menu position so it's on the selected item.
		wxRect rect;
		long index = ctrl.GetFirstSelected();
		if (0 > index || !ctrl.GetItemRect(index, rect))
		{
			rect = ctrl.GetScreenRect();
			outPos = ::wxGetMousePosition();
			if (!rect.Contains(outPos))
			{
				outPos.x = rect.GetLeft() + rect.GetWidth() / 3;
				outPos.y = rect.GetTop() + rect.GetHeight() / 2;
			}
			outPos = ctrl.ScreenToClient(outPos);
		}
		else
		{
			outPos.x = rect.GetLeft() + rect.GetWidth() / 3;
			outPos.y = rect.GetTop() + rect.GetHeight() / 2;
		}
	}
	else
	{
		outPos = ctrl.ScreenToClient(outPos);
		wxRect rect = ctrl.GetClientRect();
		bOk = rect.Contains(outPos);
	}
	return bOk;
}


bool CAgilityBookBaseExtraView::GetMenuPosition(wxPoint& outPos, CListCtrl const& ctrl, wxListEvent const& evt)
{
	bool bOk = true;
	outPos = evt.GetPoint(); // in client
	if (wxDefaultPosition == outPos)
	{
		// Adjust the menu position so it's on the selected item.
		wxRect rect;
		long index = ctrl.GetFirstSelected();
		if (0 > index || !ctrl.GetItemRect(index, rect))
		{
			rect = ctrl.GetScreenRect();
			outPos = ::wxGetMousePosition();
			if (!rect.Contains(outPos))
			{
				outPos.x = rect.GetLeft() + rect.GetWidth() / 3;
				outPos.y = rect.GetTop() + rect.GetHeight() / 2;
			}
			outPos = ctrl.ScreenToClient(outPos);
		}
		else
		{
			outPos.x = rect.GetLeft() + rect.GetWidth() / 3;
			outPos.y = rect.GetTop() + rect.GetHeight() / 2;
		}
	}
	else
	{
		wxRect rect = ctrl.GetClientRect();
		bOk = rect.Contains(outPos);
	}
	return bOk;
}


bool CAgilityBookBaseExtraView::GetMenuPosition(wxPoint& outPos, CTreeCtrl const& ctrl, wxContextMenuEvent const& evt)
{
	bool bOk = true;
	outPos = evt.GetPosition(); // in screen
	if (wxDefaultPosition == outPos)
	{
		// Adjust the menu position so it's on the selected item.
		wxRect rect;
		wxTreeItemId item = ctrl.GetSelection();
		if (!item.IsOk() || !ctrl.GetBoundingRect(item, rect))
		{
			rect = ctrl.GetScreenRect();
			outPos = ::wxGetMousePosition();
			if (!rect.Contains(outPos))
			{
				outPos.x = rect.GetLeft() + rect.GetWidth() / 3;
				outPos.y = rect.GetTop() + rect.GetHeight() / 2;
			}
			outPos = ctrl.ScreenToClient(outPos);
		}
		else
		{
			outPos.x = rect.GetLeft() + rect.GetWidth() / 3;
			outPos.y = rect.GetTop() + rect.GetHeight() / 2;
		}
	}
	else
	{
		outPos = ctrl.ScreenToClient(outPos);
		wxRect rect = ctrl.GetClientRect();
		bOk = rect.Contains(outPos);
	}
	return bOk;
}


bool CAgilityBookBaseExtraView::GetMenuPosition(wxPoint& outPos, CTreeCtrl const& ctrl, wxTreeEvent const& evt)
{
	bool bOk = true;
	outPos = evt.GetPoint(); // in client
	wxTreeItemId item = evt.GetItem();
	// Adjust the menu position so it's on the selected item.
	wxRect rect;
	if (wxDefaultPosition == outPos || !item.IsOk() || !ctrl.GetBoundingRect(item, rect))
	{
		rect = ctrl.GetScreenRect();
		outPos = ::wxGetMousePosition();
		if (!rect.Contains(outPos))
		{
			outPos.x = rect.GetLeft() + rect.GetWidth() / 3;
			outPos.y = rect.GetTop() + rect.GetHeight() / 2;
		}
		outPos = ctrl.ScreenToClient(outPos);
	}
	else
	{
		outPos.x = rect.GetLeft() + rect.GetWidth() / 3;
		outPos.y = rect.GetTop() + rect.GetHeight() / 2;
		rect = ctrl.GetClientRect();
		bOk = rect.Contains(outPos);
	}
	return bOk;
}


bool CAgilityBookBaseExtraView::GetMenuPosition(wxPoint& outPos, wxWindow const* ctrl, wxContextMenuEvent const& evt)
{
	return CMenuHelper::GetMenuPosition(outPos, ctrl, evt);
}

} // namespace dconSoft
