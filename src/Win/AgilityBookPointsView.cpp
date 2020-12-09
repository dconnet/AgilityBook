/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CAgilityBookViewHtml class
 * @author David Connet
 *
 * Revision History
 * 2019-05-28 Suppress href warning when hiding a title.
 * 2019-05-04 Reworked PointsData usage.
 * 2018-12-16 Convert to fmt.
 * 2017-08-20 Alter how header is generated/handled.
 * 2014-04-23 Scroll to position of clicked link on page load.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-09 Ported to wxWidgets.
 * 2006-09-10 Created
 */

#include "stdafx.h"
#include "AgilityBookPointsView.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ClipBoard.h"
#include "DlgPointsViewSort.h"
#include "FilterOptions.h"
#include "MainFrm.h"
#include "PointsData.h"
#include "Print.h"

#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

bool CHtmlWindow::SetPage(const wxString& source)
{
	bool rc = wxHtmlWindow::SetPage(source);

	if (rc && !m_tag.empty())
	{
		wxLogNull log; // Suppress missing tag warning
		ScrollToAnchor(m_tag);
	}

	return rc;
}

void CHtmlWindow::OnLinkClicked(const wxHtmlLinkInfo& link)
{
	const wxString ProtocolABOUT(L"about:");
	const wxString ProtocolHTTP(L"http:");
	const wxString ProtocolHTTPS(L"https:");
	const wxString ProtocolARB(ARB_PROTOCOL);

	wxString url = link.GetHref();

	bool bOpen = false;

	if (url.length() > ProtocolABOUT.length() && 0 == ProtocolABOUT.CmpNoCase(url.substr(0, ProtocolABOUT.length())))
	{
		// Let About thru (for "about:blank")
		bOpen = true;
	}
	else if (url.length() > ProtocolARB.length() && 0 == ProtocolARB.CmpNoCase(url.substr(0, ProtocolARB.length())))
	{
		// Our special internal link
		// Remember, spaces are now %20. Other special chars may
		// need fixing too. Just don't use those in our links.
		bool bDidIt = false;
		std::wstring index(StringUtil::stringW(url.substr(ProtocolARB.length())));
		if (!index.empty())
		{
			m_tag = index;
			bDidIt = m_pView->m_Items->Details(index);
			m_tag.clear();
		}
		if (!bDidIt)
		{
			wxBell();
		}
	}
	else if (
		(url.length() > ProtocolHTTP.length() && 0 == ProtocolHTTP.CmpNoCase(url.substr(0, ProtocolHTTP.length())))
		|| (url.length() > ProtocolHTTPS.length()
			&& 0 == ProtocolHTTPS.CmpNoCase(url.substr(0, ProtocolHTTPS.length()))))
	{
		// Don't allow links to replace us.
		wxLaunchDefaultBrowser(url);
		// Note, using 'target="new"' in the html href tag will cause the new
		// window to open in IE, which is not necessarily the default browser.
	}
	else
	{
		// Don't allow any other types of links.
		//TRACE("Preventing navigation to '%s'\n", lpszURL);
		wxBell();
	}
	if (bOpen)
		wxHtmlWindow::OnLinkClicked(link);
}

/////////////////////////////////////////////////////////////////////////////

wxIMPLEMENT_CLASS(CAgilityBookPointsView, CAgilityBookBaseExtraView)


wxBEGIN_EVENT_TABLE(CAgilityBookPointsView, CAgilityBookBaseExtraView)
	EVT_UPDATE_UI(wxID_COPY, CAgilityBookPointsView::OnViewUpdateCmd)
	EVT_MENU(wxID_COPY, CAgilityBookPointsView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_NEW_TITLE, CAgilityBookPointsView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_NEW_TITLE, CAgilityBookPointsView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_POINTS_VIEW_SORT, CAgilityBookPointsView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_POINTS_VIEW_SORT, CAgilityBookPointsView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_HIDDEN, CAgilityBookPointsView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_HIDDEN, CAgilityBookPointsView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_LIFETIME_EVENTS, CAgilityBookPointsView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_LIFETIME_EVENTS, CAgilityBookPointsView::OnViewCmd)
	EVT_MENU(wxID_PRINT, CAgilityBookPointsView::OnPrintView)
	EVT_MENU(wxID_PREVIEW, CAgilityBookPointsView::OnPreview)
wxEND_EVENT_TABLE()


CAgilityBookPointsView::CAgilityBookPointsView(CTabView* pTabView, wxDocument* doc)
	: CAgilityBookBaseExtraView(pTabView, doc)
	, m_Ctrl(nullptr)
	, m_Items(std::make_unique<CPointsDataItems>(GetDocument()))
{
}


CAgilityBookPointsView::~CAgilityBookPointsView()
{
	m_Items->clear();
}


bool CAgilityBookPointsView::Create(
	CBasePanel* parentView,
	wxWindow* parentCtrl,
	wxDocument* doc,
	long flags,
	wxSizer* sizer,
	int proportion,
	int sizerFlags,
	int border)
{
	m_Ctrl = new CHtmlWindow(
		this,
		parentCtrl,
		wxID_ANY,
		wxDefaultPosition,
		wxDefaultSize,
		wxNO_BORDER | wxHW_SCROLLBAR_AUTO);
#if defined(__WXMAC__)
	m_Ctrl->SetDropTarget(new CFileDropTarget(doc->GetDocumentManager()));
#endif
	return CAgilityBookBaseExtraView::Create(parentView, parentCtrl, doc, flags, sizer, proportion, sizerFlags, border);
}


void CAgilityBookPointsView::DetachView()
{
	// The control is actually owned by the panel, the view is not.
	m_Ctrl = nullptr;
}


bool CAgilityBookPointsView::IsFiltered() const
{
	return CFilterOptions::Options().IsFilterEnabled();
}


bool CAgilityBookPointsView::GetMessage(std::wstring& msg) const
{
	msg = _("IDS_INDICATOR_BLANK");
	return true;
}


bool CAgilityBookPointsView::GetMessage2(std::wstring& msg) const
{
	if (GetDocument()->GetCurrentDog())
	{
		msg = GetDocument()->GetCurrentDog()->GetCallName();
		return true;
	}
	else
	{
		msg.clear();
		return false;
	}
}


bool CAgilityBookPointsView::AllowStatusContext(int field) const
{
	return STATUS_DOG == field || STATUS_FILTERED == field;
}


bool CAgilityBookPointsView::OnCreate(wxDocument* doc, long flags)
{
	return true;
}


void CAgilityBookPointsView::DoActivateView(bool activate, wxView* activeView, wxView* deactiveView)
{
	if (m_Ctrl && activate)
		m_Ctrl->SetFocus();
}


void CAgilityBookPointsView::OnDraw(wxDC* dc)
{
	if (m_Ctrl && dc)
		m_Ctrl->OnDraw(*dc);
}


void CAgilityBookPointsView::OnUpdate(wxView* sender, wxObject* inHint)
{
	STACK_TRACE(stack, L"CAgilityBookPointsView::OnUpdate");

	CUpdateHint* hint = nullptr;
	if (inHint)
		hint = wxDynamicCast(inHint, CUpdateHint);
	if (!hint || hint->IsSet(UPDATE_POINTS_VIEW) || hint->IsEqual(UPDATE_CONFIG) || hint->IsEqual(UPDATE_OPTIONS)
		|| hint->IsEqual(UPDATE_LANG_CHANGE))
	{
		LoadData();
	}
}


void CAgilityBookPointsView::LoadData()
{
	STACK_TRACE(stack, L"CAgilityBookPointsView::LoadData");

	wxBusyCursor wait;

	m_Items->LoadData(GetDocument()->GetCurrentDog());
	wxString data = m_Items->GetHtml(false, false);
	m_Ctrl->SetPage(data);

	if (m_Ctrl->IsShownOnScreen())
		UpdateMessages();
}


void CAgilityBookPointsView::OnViewUpdateCmd(wxUpdateUIEvent& evt)
{
	switch (evt.GetId())
	{
	case wxID_COPY:
		evt.Enable(true);
		break;
	case ID_AGILITY_NEW_TITLE:
		evt.Enable(GetDocument()->GetCurrentDog() ? true : false);
		break;
	case ID_VIEW_POINTS_VIEW_SORT:
	case ID_VIEW_HIDDEN:
	case ID_VIEW_LIFETIME_EVENTS:
		evt.Enable(true);
		break;
	}
}


void CAgilityBookPointsView::OnViewCmd(wxCommandEvent& evt)
{
	if (!m_Ctrl)
		return;
	switch (evt.GetId())
	{
	case wxID_COPY:
	{
		CClipboardDataWriter clpData;
		if (clpData.isOkay())
		{
			wxString data = m_Items->GetHtml(true, true);
			clpData.AddData(ARBClipFormat::Html, StringUtil::stringW(data));
			clpData.AddData(StringUtil::stringW(m_Ctrl->ToText()));
			clpData.CommitData();
		}
	}
	break;

	case ID_AGILITY_NEW_TITLE:
		GetDocument()->AddTitle(GetDocument()->GetCurrentDog());
		break;

	case ID_VIEW_POINTS_VIEW_SORT:
	{
		CDlgPointsViewSort dlg(m_Ctrl);
		if (wxID_OK == dlg.ShowModal())
			LoadData();
	}
	break;

	case ID_VIEW_HIDDEN:
	{
		CAgilityBookOptions::SetViewHiddenTitles(!CAgilityBookOptions::GetViewHiddenTitles());
		std::vector<CVenueFilter> venues;
		CFilterOptions::Options().GetFilterVenue(venues);
		for (ARBDogList::iterator iterDogs = GetDocument()->Book().GetDogs().begin();
			 iterDogs != GetDocument()->Book().GetDogs().end();
			 ++iterDogs)
		{
			for (ARBDogTitleList::iterator iterTitle = (*iterDogs)->GetTitles().begin();
				 iterTitle != (*iterDogs)->GetTitles().end();
				 ++iterTitle)
			{
				GetDocument()->ResetVisibility(venues, *iterTitle);
			}
		}
		LoadData();
	}
	break;

	case ID_VIEW_LIFETIME_EVENTS:
	{
		CAgilityBookOptions::SetViewLifetimePointsByEvent(!CAgilityBookOptions::GetViewLifetimePointsByEvent());
		LoadData();
	}
	break;
	}
}


void CAgilityBookPointsView::OnPrintView(wxCommandEvent& evt)
{
	wxString text(m_Items->GetHtml(false, true));
	wxGetApp().GetHtmlPrinter()->PrintText(text);
}


void CAgilityBookPointsView::OnPreview(wxCommandEvent& evt)
{
	wxString text(m_Items->GetHtml(false, true));
	wxGetApp().GetHtmlPrinter()->PreviewText(text);
}
