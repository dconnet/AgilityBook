#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgDog class
 * @author David Connet
 *
 * Revision History
 * 2021-01-07 Separated panes from DlgDog.cpp
 * 2009-03-14 Merged DlgRun* into here.
 * 2009-02-09 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-02-03 Broke dialog up into pages.
 * 2003-08-18 Added a deceased date for a dog.
 */

#include "ARB/ARBTypes2.h"
class CAgilityBookDoc;
class CDlgDogPanelPoints;
class CDlgDogPanelProp;
class CDlgDogPanelRegNums;
class CDlgDogPanelTitles;
class CReportListCtrl;
class wxDateEvent;
class wxDatePickerCtrl;


class CDlgDog : public wxDialog
{
	DECLARE_NO_COPY_IMPLEMENTED(CDlgDog)
public:
	CDlgDog(CAgilityBookDoc* pDoc, ARBDogPtr const& inDog, wxWindow* pParent = nullptr, int iSelectPage = 0);

private:
	CAgilityBookDoc* m_pDoc;
	ARBDogPtr m_pDog;
	bool m_viewHidden;

	CDlgDogPanelProp* m_panelProps;
	CDlgDogPanelTitles* m_panelTitles;
	CDlgDogPanelRegNums* m_panelRegNums;
	CDlgDogPanelPoints* m_panelPoints;

	DECLARE_ON_INIT()
	DECLARE_EVENT_TABLE()
	void OnPageChanging(wxBookCtrlEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
