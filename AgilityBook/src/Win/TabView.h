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
class CMainFrame;

#define IDX_PANE_RUNS		0
#define IDX_PANE_POINTS		1
#define IDX_PANE_CALENDAR	2
#define IDX_PANE_LOG		3


class CTabView : public CAgilityBookBaseView
{
	DECLARE_DYNAMIC_CLASS(CTabView)

public:
	CTabView();
	~CTabView();
	bool ShowPointsAsHtml(bool bHtml);
	int GetCurTab() const;
	int SetCurTab(int index);
	virtual void OnChangeFilename();
	virtual bool OnCreate(wxDocument* doc, long flags);
	virtual void OnDraw(wxDC* dc);
	virtual void OnUpdate(wxView* sender, wxObject* inHint = NULL);

	// Menu ids
	int GetType() const		{return m_type;}
	int GetOrient() const	{return m_orient;}

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
#if wxCHECK_VERSION(3, 0, 0)
#if wxUSE_NOTEBOOK
	void OnNotebookChanged(wxBookCtrlEvent& evt)	{OnBookCtrlChanged(evt);}
#endif
#if wxUSE_CHOICEBOOK
	void OnChoicebookChanged(wxBookCtrlEvent& evt)	{OnBookCtrlChanged(evt);}
#endif
#if wxUSE_LISTBOOK
	void OnListbookChanged(wxBookCtrlEvent& evt)	{OnBookCtrlChanged(evt);}
#endif
#if wxUSE_TREEBOOK
	void OnTreebookChanged(wxBookCtrlEvent& evt)	{OnBookCtrlChanged(evt);}
#endif
#if wxUSE_TOOLBOOK
	void OnToolbookChanged(wxBookCtrlEvent& evt)	{OnBookCtrlChanged(evt);}
#endif
#else
#if wxUSE_NOTEBOOK
	void OnNotebookChanged(wxNotebookEvent& evt)		{OnBookCtrlChanged(evt);}
#endif
#if wxUSE_CHOICEBOOK
	void OnChoicebookChanged(wxChoicebookEvent& evt)	{OnBookCtrlChanged(evt);}
#endif
#if wxUSE_LISTBOOK
	void OnListbookChanged(wxListbookEvent& evt)		{OnBookCtrlChanged(evt);}
#endif
#if wxUSE_TREEBOOK
	void OnTreebookChanged(wxTreebookEvent& evt)		{OnBookCtrlChanged(evt);}
#endif
#if wxUSE_TOOLBOOK
	void OnToolbookChanged(wxToolbookEvent& evt)		{OnBookCtrlChanged(evt);}
#endif
#endif
};
