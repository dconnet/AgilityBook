#pragma once

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
 * @brief Tab view
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-06 DRC Ported to wxWidgets.
 */

#include "CommonView.h"
#include <wx/choicebk.h>
#include <wx/docview.h>
#include <wx/imaglist.h>
#include <wx/listbook.h>
#include <wx/notebook.h>
#include <wx/toolbook.h>
#include <wx/treebook.h>
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

	void OnBookCtrlChanged(wxBookCtrlBaseEvent& evt);
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
};
