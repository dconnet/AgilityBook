#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Tab view
 * @author David Connet
 *
 * Revision History
 * 2012-01-07 Fix tab type/orientation persistence.
 * 2009-01-06 Ported to wxWidgets.
 */

#include "CommonView.h"
#include <wx/docview.h>
#include <wx/imaglist.h>


namespace dconSoft
{
class CMainFrame;

constexpr int IDX_PANE_RUNS = 0;
constexpr int IDX_PANE_POINTS = 1;
constexpr int IDX_PANE_CALENDAR = 2;
constexpr int IDX_PANE_LOG = 3;


class CTabView : public CAgilityBookBaseView
{
	DECLARE_DYNAMIC_CLASS(CTabView)
	DECLARE_NO_COPY_IMPLEMENTED(CTabView)

public:
	CTabView();
	~CTabView();
	bool ShowPointsAsHtml(bool bHtml);
	int GetCurTab() const;
	int SetCurTab(int index);
	void OnChangeFilename() override;
	bool OnCreate(wxDocument* doc, long flags) override;
	void OnDraw(wxDC* dc) override;
	void OnUpdate(wxView* sender, wxObject* inHint = nullptr) override;

	// Menu ids
	int GetType() const
	{
		return m_type;
	}
	int GetOrient() const
	{
		return m_orient;
	}

	void OnType(int id);
	void OnOrient(int id);

private:
	void RecreateBook(wxDocument* doc, long flags, bool bOnCreate);

	CMainFrame* m_frame;
	long m_type;
	long m_orient;
	wxBoxSizer* m_sizerFrame;
	wxBookCtrlBase* m_ctrlBook;
	wxImageList m_imageList;
	bool m_bIgnoreEvents;

	void OnBookCtrlChanged(wxBookCtrlEvent& evt);
#if wxUSE_NOTEBOOK
	void OnNotebookChanged(wxBookCtrlEvent& evt)
	{
		OnBookCtrlChanged(evt);
	}
#endif
#if wxUSE_CHOICEBOOK
	void OnChoicebookChanged(wxBookCtrlEvent& evt)
	{
		OnBookCtrlChanged(evt);
	}
#endif
#if wxUSE_LISTBOOK
	void OnListbookChanged(wxBookCtrlEvent& evt)
	{
		OnBookCtrlChanged(evt);
	}
#endif
#if wxUSE_TREEBOOK
	void OnTreebookChanged(wxBookCtrlEvent& evt)
	{
		OnBookCtrlChanged(evt);
	}
#endif
#if wxUSE_TOOLBOOK
	void OnToolbookChanged(wxBookCtrlEvent& evt)
	{
		OnBookCtrlChanged(evt);
	}
#endif
};

} // namespace dconSoft
