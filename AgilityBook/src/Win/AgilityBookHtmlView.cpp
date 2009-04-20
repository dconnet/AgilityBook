/*
 * Copyright © 2006-2009 David Connet. All Rights Reserved.
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
 * @brief interface of the CAgilityBookViewHtml class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2006-09-10 DRC Created
 */

#include "stdafx.h"
#include "AgilityBookHtmlView.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTreeData.h"
#include "ClipBoard.h"
#include "DlgPointsViewSort.h"
#include "FilterOptions.h"
#include "MainFrm.h"
#include "PointsData.h"
#include "Print.h"

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(CAgilityBookHtmlView, CAgilityBookBaseExtraView)


BEGIN_EVENT_TABLE(CAgilityBookHtmlView, CAgilityBookBaseExtraView)
	EVT_UPDATE_UI(wxID_COPY, CAgilityBookHtmlView::OnViewUpdateCmd)
	EVT_MENU(wxID_COPY, CAgilityBookHtmlView::OnViewCmd)
	EVT_UPDATE_UI(ID_AGILITY_NEW_TITLE, CAgilityBookHtmlView::OnViewUpdateCmd)
	EVT_MENU(ID_AGILITY_NEW_TITLE, CAgilityBookHtmlView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_POINTS_VIEW_SORT, CAgilityBookHtmlView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_POINTS_VIEW_SORT, CAgilityBookHtmlView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_HIDDEN, CAgilityBookHtmlView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_HIDDEN, CAgilityBookHtmlView::OnViewCmd)
	EVT_UPDATE_UI(ID_VIEW_LIFETIME_EVENTS, CAgilityBookHtmlView::OnViewUpdateCmd)
	EVT_MENU(ID_VIEW_LIFETIME_EVENTS, CAgilityBookHtmlView::OnViewCmd)
	EVT_MENU(wxID_PRINT, CAgilityBookHtmlView::OnPrint)
	EVT_MENU(wxID_PREVIEW, CAgilityBookHtmlView::OnPreview)
END_EVENT_TABLE()


CAgilityBookHtmlView::CAgilityBookHtmlView(
		CTabView* pTabView,
		wxDocument* doc)
	: CAgilityBookBaseExtraView(pTabView, doc)
	, m_Ctrl(NULL)
	, m_Items(new CPointsDataItems())
{
}


CAgilityBookHtmlView::~CAgilityBookHtmlView()
{
	m_Items->clear();
	delete m_Items;
}


bool CAgilityBookHtmlView::Create(
		CBasePanel* parentView,
		wxWindow* parentCtrl,
		wxDocument* doc,
		long flags,
		wxSizer* sizer,
		int proportion,
		int sizerFlags,
		int border)
{
	m_Ctrl = new wxHtmlWindow(parentCtrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxHW_SCROLLBAR_AUTO);
	m_Ctrl->Connect(wxEVT_COMMAND_HTML_LINK_CLICKED, wxHtmlLinkEventHandler(CAgilityBookHtmlView::OnCtrlLinkClicked), NULL, this);

	return CAgilityBookBaseExtraView::Create(parentView, parentCtrl, doc, flags, sizer, proportion, sizerFlags, border);
}


void CAgilityBookHtmlView::DetachView()
{
	// The control is actually owned by the panel, the view is not.
	m_Ctrl = NULL;
}


bool CAgilityBookHtmlView::IsFiltered() const
{
	return CFilterOptions::Options().IsFilterEnabled();
}


bool CAgilityBookHtmlView::GetMessage(wxString& msg) const
{
	msg = _("IDS_INDICATOR_BLANK");
	return true;
}


bool CAgilityBookHtmlView::GetMessage2(wxString& msg) const
{
	if (GetDocument()->GetCurrentDog())
	{
		msg = GetDocument()->GetCurrentDog()->GetCallName().c_str();
		return true;
	}
	else
	{
		msg.Empty();
		return false;
	}
}


bool CAgilityBookHtmlView::AllowStatusContext(int field) const
{
	return STATUS_DOG == field || STATUS_FILTERED == field;
}


bool CAgilityBookHtmlView::OnCreate(
		wxDocument* doc,
		long flags)
{
	return true;
}


void CAgilityBookHtmlView::DoActivateView(
		bool activate,
		wxView* activeView,
		wxView* deactiveView)
{
	if (m_Ctrl && activate)
		m_Ctrl->SetFocus();
}


void CAgilityBookHtmlView::OnDraw(wxDC* dc)
{
	if (m_Ctrl && dc)
		m_Ctrl->OnDraw(*dc);
}


void CAgilityBookHtmlView::OnUpdate(
		wxView* sender,
		wxObject* inHint)
{
	CUpdateHint* hint = NULL;
	if (inHint)
		hint = reinterpret_cast<CUpdateHint*>(inHint);
	if (!hint || hint->IsSet(UPDATE_POINTS_VIEW)
	|| hint->IsEqual(UPDATE_CONFIG) || hint->IsEqual(UPDATE_OPTIONS)
	|| hint->IsEqual(UPDATE_LANG_CHANGE))
	{
		LoadData();
	}
}


wxString CAgilityBookHtmlView::RawHtml(bool bFragment) const
{
	ARBDate today(ARBDate::Today());
	otstringstream data;

	wxString title = _("IDS_TITLING_POINTS");

	data << wxT("<html>") << std::endl;
	if (!bFragment)
		data << wxT("<head><title>") << title.c_str() << wxT(" ")
			<< today.GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints))
			<< wxT("</title></head>")
			<< std::endl
			<< wxT("<body>")
			<< std::endl;

	size_t nItems = m_Items->NumLines();
	if (0 < nItems)
	{
		CPointsDataBasePtr item = m_Items->GetLine(0);
		data << item->GetHtml(0);
	}
	for (size_t nItem = 1; nItem < nItems; ++nItem)
	{
		CPointsDataBasePtr item = m_Items->GetLine(nItem);
		data << item->GetHtml(nItem);
	}
	if (!bFragment)
		data << wxT("</body></html>");
	data << std::endl;

	return data.str().c_str();
}


void CAgilityBookHtmlView::LoadData()
{
	wxBusyCursor wait;

	m_Items->LoadData(m_Ctrl, GetDocument(), GetDocument()->GetCurrentDog());
	wxString data = RawHtml(false);
	m_Ctrl->SetPage(data);

	if (m_Ctrl->IsShownOnScreen())
		UpdateMessages();
}


void CAgilityBookHtmlView::OnCtrlLinkClicked(wxHtmlLinkEvent& evt)
{
	static const wxString ProtocolABOUT(wxT("about:"));
	static const wxString ProtocolHTTP(wxT("http:"));
	static const wxString ProtocolHTTPS(wxT("https:"));
	static const wxString ProtocolARB(ARB_PROTOCOL);

	wxString url = evt.GetLinkInfo().GetHref();

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
		wxString index(url.substr(ProtocolARB.length()));
		if (!index.empty())
		{
			long nItem;
			if (index.ToLong(&nItem) && nItem < static_cast<long>(m_Items->NumLines()))
			{
				bDidIt = true;
				CPointsDataBasePtr item = m_Items->GetLine(nItem);
				item->Details();
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
		evt.Skip();
}


void CAgilityBookHtmlView::OnViewUpdateCmd(wxUpdateUIEvent& evt)
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


void CAgilityBookHtmlView::OnViewCmd(wxCommandEvent& evt)
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
				wxString data = RawHtml(true);
				clpData.AddData(eFormatHtml, data);
				clpData.AddData(m_Ctrl->ToText().c_str());
				clpData.CommitData();
			}
		}
		break;

	case ID_AGILITY_NEW_TITLE:
		if (GetDocument()->GetCurrentDog())
		{
			// Convenience! No duplicated code!
			CAgilityBookTreeDataDog data(GetDocument()->GetTreeView(), GetDocument()->GetCurrentDog());
			bool bModified = false;
			if (data.OnCmd(ID_AGILITY_NEW_TITLE, bModified, NULL))
			{
				if (bModified)
					GetDocument()->Modify(true);
			}
		}
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


void CAgilityBookHtmlView::OnPrint(wxCommandEvent& evt)
{
	wxString text(RawHtml(false));
	wxGetApp().GetHtmlPrinter()->PrintText(text);
}


void CAgilityBookHtmlView::OnPreview(wxCommandEvent& evt)
{
	wxString text(RawHtml(false));
	wxGetApp().GetHtmlPrinter()->PreviewText(text);
}
