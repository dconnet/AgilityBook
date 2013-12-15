#pragma once

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
 * 2009-02-09 Ported to wxWidgets.
 * 2006-09-10 Created
 */

#include "CommonView.h"
#include <wx/docview.h>
class CPointsDataItems;


class CAgilityBookHtmlView : public CAgilityBookBaseExtraView
{
	DECLARE_DYNAMIC_CLASS(CAgilityBookHtmlView)
	CAgilityBookHtmlView(
			CTabView* pTabView,
			wxDocument* doc);

public:
	~CAgilityBookHtmlView();

	virtual bool Create(
			CBasePanel* parentView,
			wxWindow* parentCtrl,
			wxDocument* doc,
			long flags,
			wxSizer* sizer,
			int proportion = 0,
			int sizerFlags = 0,
			int border = 0);
	virtual wxWindow* GetControl()		{return m_Ctrl;}
	virtual void DetachView();

	virtual bool IsFiltered() const;
	virtual bool GetMessage(std::wstring& msg) const;
	virtual bool GetMessage2(std::wstring& msg) const;
	virtual bool AllowStatusContext(int field) const;

	virtual bool OnCreate(
			wxDocument* doc,
			long flags);
	virtual void DoActivateView(
			bool activate,
			wxView* activeView,
			wxView* deactiveView);
	virtual void OnDraw(wxDC* dc);
	virtual void OnUpdate(
			wxView* sender,
			wxObject* inHint = NULL);

private:
	wxString RawHtml(
			bool bFragment,
			bool bNoInternalLinks) const;
	void LoadData();

	wxHtmlWindow* m_Ctrl;
	CPointsDataItems* m_Items;

	DECLARE_EVENT_TABLE()
	void OnCtrlLinkClicked(wxHtmlLinkEvent& evt);
	void OnViewUpdateCmd(wxUpdateUIEvent& evt);
	void OnViewCmd(wxCommandEvent& evt);
	void OnPrint(wxCommandEvent& evt);
	void OnPreview(wxCommandEvent& evt);
};
