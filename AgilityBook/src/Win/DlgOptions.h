#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgOptions class
 * @author David Connet
 *
 * Revision History
 * 2009-02-09 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-08-18 Separated options and filters into two dialogs.
 */

class CAgilityBookDoc;
class CDlgOptionsCalendar;
class CDlgOptionsFilter;
class CDlgOptionsPrint;
class CDlgOptionsProgram;


class CDlgOptions : public wxDialog
{
public:
	static int GetProgramPage()		{return 0;}
	static int GetFilterPage()		{return 1;}
	static int GetCalendarPage()	{return 2;}
	static int GetPrintPage()		{return 3;}

	CDlgOptions(
			CAgilityBookDoc* pDoc,
			wxWindow* pParentWnd = NULL,
			size_t iSelectPage = 0);

protected:
	CAgilityBookDoc* m_pDoc;
	CDlgOptionsProgram* m_pageProgram;
	CDlgOptionsFilter* m_pageFilter;
	CDlgOptionsCalendar* m_pageCal;
	CDlgOptionsPrint* m_pagePrint;

	DECLARE_EVENT_TABLE()
	void OnPageChanging(wxBookCtrlEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
