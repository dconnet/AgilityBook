/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
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

#include "stdafx.h"
#include "CommonView.h"

#include "AgilityBook.h"
#include "AgilityBookPanels.h"
#include "CheckTreeCtrl.h"
#include "ListCtrl.h"


IMPLEMENT_CLASS(CAgilityBookBaseView, wxView)
IMPLEMENT_CLASS(CAgilityBookBaseExtraView, CAgilityBookBaseView)


CAgilityBookBaseExtraView::CAgilityBookBaseExtraView(
		CTabView* pTabView,
		wxDocument* doc)
	: m_pTabView(pTabView)
	, m_parentPanel(NULL)
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


void CAgilityBookBaseExtraView::OnActivateView(
		bool activate,
		wxView* activeView,
		wxView* deactiveView)
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


bool CAgilityBookBaseExtraView::GetMenuPosition(
		wxPoint& outPos,
		CListCtrl& ctrl,
		wxContextMenuEvent const& evt)
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


bool CAgilityBookBaseExtraView::GetMenuPosition(
		wxPoint& outPos,
		CListCtrl& ctrl,
		wxListEvent const& evt)
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


bool CAgilityBookBaseExtraView::GetMenuPosition(
		wxPoint& outPos,
		CTreeCtrl& ctrl,
		wxContextMenuEvent const& evt)
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


bool CAgilityBookBaseExtraView::GetMenuPosition(
		wxPoint& outPos,
		CTreeCtrl& ctrl,
		wxTreeEvent const& evt)
{
	bool bOk = true;
	outPos = evt.GetPoint(); // in client
	wxTreeItemId item = evt.GetItem();
	// Adjust the menu position so it's on the selected item.
	wxRect rect;
	if (wxDefaultPosition == outPos || !item.IsOk()
	|| !ctrl.GetBoundingRect(item, rect))
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


bool CAgilityBookBaseExtraView::GetMenuPosition(
		wxPoint& outPos,
		wxWindow* ctrl,
		wxContextMenuEvent const& evt)
{
	outPos = evt.GetPosition(); // in screen
	if (!ctrl)
		return false;
	bool bOk = true;
	if (wxDefaultPosition == outPos)
	{
		wxRect rect = ctrl->GetScreenRect();
		outPos = ::wxGetMousePosition();
		if (!rect.Contains(outPos))
		{
			outPos.x = rect.GetLeft() + rect.GetWidth() / 3;
			outPos.y = rect.GetTop() + rect.GetHeight() / 2;
		}
		outPos = ctrl->ScreenToClient(outPos);
	}
	else
	{
		outPos = ctrl->ScreenToClient(outPos);
		wxRect rect = ctrl->GetClientRect();
		bOk = rect.Contains(outPos);
	}
	return bOk;
}
