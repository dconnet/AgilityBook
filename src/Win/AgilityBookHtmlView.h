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
 * 2014-04-23 Scroll to position of clicked link on page load.
 * 2009-02-09 Ported to wxWidgets.
 * 2006-09-10 Created
 */

#include "CommonView.h"
#include <wx/docview.h>
class CAgilityBookHtmlView;
class CHtmlWindow;
class CPointsDataItems;


class CHtmlWindow : public wxHtmlWindow
{
public:
	CHtmlWindow(
			CAgilityBookHtmlView* pView,
			wxWindow *parent,
			wxWindowID id = wxID_ANY,
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize,
			long style = wxHW_DEFAULT_STYLE,
			const wxString& name = wxT("htmlWindow"))
		: wxHtmlWindow(parent, id, pos, size, style, name)
		, m_pView(pView)
	{
	}

	bool SetPage(const wxString& source) override;
	void OnLinkClicked(const wxHtmlLinkInfo& link) override;

private:
	CAgilityBookHtmlView* m_pView;
	std::wstring m_tag;
};


class CAgilityBookHtmlView : public CAgilityBookBaseExtraView
{
	friend class CHtmlWindow;
	DECLARE_DYNAMIC_CLASS(CAgilityBookHtmlView)
	DECLARE_NO_COPY_IMPLEMENTED(CAgilityBookHtmlView)

public:
	CAgilityBookHtmlView(
			CTabView* pTabView,
			wxDocument* doc);
	~CAgilityBookHtmlView();

	bool Create(
			CBasePanel* parentView,
			wxWindow* parentCtrl,
			wxDocument* doc,
			long flags,
			wxSizer* sizer,
			int proportion = 0,
			int sizerFlags = 0,
			int border = 0) override;
	wxWindow* GetControl() override		{return m_Ctrl;}
	void DetachView() override;

	bool IsFiltered() const override;
	bool GetMessage(std::wstring& msg) const override;
	bool GetMessage2(std::wstring& msg) const override;
	bool AllowStatusContext(int field) const override;

	bool OnCreate(
			wxDocument* doc,
			long flags) override;
	void DoActivateView(
			bool activate,
			wxView* activeView,
			wxView* deactiveView) override;
	void OnDraw(wxDC* dc) override;
	void OnUpdate(
			wxView* sender,
			wxObject* inHint = nullptr) override;

private:
	wxString RawHtml(
			bool bFragment,
			bool bNoInternalLinks) const;
	void LoadData();

	CHtmlWindow* m_Ctrl;
	std::unique_ptr<CPointsDataItems> m_Items;

	DECLARE_EVENT_TABLE()
	void OnViewUpdateCmd(wxUpdateUIEvent& evt);
	void OnViewCmd(wxCommandEvent& evt);
	void OnPrint(wxCommandEvent& evt);
	void OnPreview(wxCommandEvent& evt);
};
