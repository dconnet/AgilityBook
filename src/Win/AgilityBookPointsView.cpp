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
		ScrollToAnchor(m_tag);

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

	if (url.length() > ProtocolABOUT.length()
	&& 0 == ProtocolABOUT.CmpNoCase(url.substr(0, ProtocolABOUT.length())))
	{
		// Let About thru (for "about:blank")
		bOpen = true;
	}
	else if (url.length() > ProtocolARB.length()
	&& 0 == ProtocolARB.CmpNoCase(url.substr(0, ProtocolARB.length())))
	{
		// Our special internal link
		// Remember, spaces are now %20. Other special chars may
		// need fixing too. Just don't use those in our links.
		bool bDidIt = false;
		std::wstring index(StringUtil::stringW(url.substr(ProtocolARB.length())));
		if (!index.empty())
		{
			long nItem;
			if (StringUtil::ToCLong(index, nItem) && nItem < static_cast<long>(m_pView->m_Items->NumLines()))
			{
				bDidIt = true;
				CPointsDataBasePtr item = m_pView->m_Items->GetLine(nItem);
				m_tag = fmt::format(L"ref{}", index);
				item->Details();
				m_tag.clear();
			}
		}
		if (!bDidIt)
		{
			wxBell();
		}
	}
	else if ((url.length() > ProtocolHTTP.length()
	&& 0 == ProtocolHTTP.CmpNoCase(url.substr(0, ProtocolHTTP.length())))
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

IMPLEMENT_CLASS(CAgilityBookPointsView, CAgilityBookBaseExtraView)


BEGIN_EVENT_TABLE(CAgilityBookPointsView, CAgilityBookBaseExtraView)
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
	EVT_MENU(wxID_PRINT, CAgilityBookPointsView::OnPrint)
	EVT_MENU(wxID_PREVIEW, CAgilityBookPointsView::OnPreview)
END_EVENT_TABLE()


CAgilityBookPointsView::CAgilityBookPointsView(
		CTabView* pTabView,
		wxDocument* doc)
	: CAgilityBookBaseExtraView(pTabView, doc)
	, m_Ctrl(nullptr)
	, m_Items(std::make_unique<CPointsDataItems>())
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
	m_Ctrl = new CHtmlWindow(this, parentCtrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxHW_SCROLLBAR_AUTO);
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


bool CAgilityBookPointsView::OnCreate(
		wxDocument* doc,
		long flags)
{
	return true;
}


void CAgilityBookPointsView::DoActivateView(
		bool activate,
		wxView* activeView,
		wxView* deactiveView)
{
	if (m_Ctrl && activate)
		m_Ctrl->SetFocus();
}


void CAgilityBookPointsView::OnDraw(wxDC* dc)
{
	if (m_Ctrl && dc)
		m_Ctrl->OnDraw(*dc);
}


void CAgilityBookPointsView::OnUpdate(
		wxView* sender,
		wxObject* inHint)
{
	STACK_TRACE(stack, L"CAgilityBookPointsView::OnUpdate");

	CUpdateHint* hint = nullptr;
	if (inHint)
		hint = wxDynamicCast(inHint, CUpdateHint);
	if (!hint || hint->IsSet(UPDATE_POINTS_VIEW)
	|| hint->IsEqual(UPDATE_CONFIG) || hint->IsEqual(UPDATE_OPTIONS)
	|| hint->IsEqual(UPDATE_LANG_CHANGE))
	{
		LoadData();
	}
}


wxString CAgilityBookPointsView::RawHtml(
		bool bFragment,
		bool bNoInternalLinks) const
{
	fmt::wmemory_buffer data;

	size_t nItems = m_Items->NumLines();
	if (!bFragment)
	{
		fmt::format_to(data, L"<html>\n");
		if (0 < nItems)
		{
			CPointsDataBasePtr item = m_Items->GetLine(0);
			fmt::format_to(data, L"{}", item->GetHtml(0, bNoInternalLinks));
		}
		fmt::format_to(data, L"<body>\n");
	}

	for (size_t nItem = 1; nItem < nItems; ++nItem)
	{
		CPointsDataBasePtr item = m_Items->GetLine(nItem);
		fmt::format_to(data, L"{}", item->GetHtml(nItem, bNoInternalLinks));
	}
	if (!bFragment)
		fmt::format_to(data, L"</body></html>");
	fmt::format_to(data, L"\n");

	return StringUtil::stringWX(fmt::to_string(data));
}


void CAgilityBookPointsView::LoadData()
{
	STACK_TRACE(stack, L"CAgilityBookPointsView::LoadData");

	wxBusyCursor wait;

	m_Items->LoadData(GetDocument(), GetDocument()->GetCurrentDog());
	wxString data = RawHtml(false, false);
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
				wxString data = RawHtml(true, true);
				clpData.AddData(eFormatHtml, StringUtil::stringW(data));
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


void CAgilityBookPointsView::OnPrint(wxCommandEvent& evt)
{
	wxString text(RawHtml(false, true));
	wxGetApp().GetHtmlPrinter()->PrintText(text);
}


void CAgilityBookPointsView::OnPreview(wxCommandEvent& evt)
{
	wxString text(RawHtml(false, true));
	wxGetApp().GetHtmlPrinter()->PreviewText(text);
}
