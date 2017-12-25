#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgOptionsPrint class
 * @author David Connet
 *
 * Revision History
 * 2009-02-11 Ported to wxWidgets.
 * 2006-11-22 Moved printing to new page.
 */

#include "AgilityBookOptions.h"
#include <wx/cmndata.h>


class CDlgOptionsPrint : public wxPanel
{
public:
	CDlgOptionsPrint(wxWindow* pParent);

	void Save();

private:
	CFontInfo m_fontPrintInfo;
	wxFont m_fontPrint;
	wxPrintDialogData m_PrintData;
	bool m_MetricSizes;
	double m_PageRunWidth;
	double m_PageRunHeight;
	double m_Left;
	double m_Right;
	double m_Top;
	double m_Bottom;
	wxStaticText* m_ctrlFontPrint;
	wxRadioBox* m_Orientation;
	wxRadioBox* m_Metric;
	int m_idxIn;
	int m_idxMM;

	void OnFontPrint(wxCommandEvent& evt);
	void OnUnitsChange(wxCommandEvent& evt);
};
