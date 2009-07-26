#pragma once

/*
 * Copyright (c) 2003-2009 David Connet. All Rights Reserved.
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
 * @brief Import/Export Wizard
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-10 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2003-12-10 DRC Created
 */

#include "ARBCalendar.h"
#include "WizardExcel.h"
#include <vector>
#include <wx/wizard.h>
class CAgilityBookDoc;
class CWizardExport;
class CWizardImport;
class CWizardStart;

// These must agree with the order of items in sc_Items in WizardStart.cpp.
// They are simply for ease-of-coding.
#define WIZ_IMPORT_RUNS				0
#define WIZ_EXPORT_RUNS				1
#define WIZ_IMPORT_CALENDAR			2
#define WIZ_EXPORT_CALENDAR			3
#define WIZ_EXPORT_CALENDAR_VCAL	4
#define WIZ_EXPORT_CALENDAR_ICAL	5
#define WIZ_EXPORT_CALENDAR_APPT	6
#define WIZ_EXPORT_CALENDAR_TASK	7
#define WIZ_IMPORT_LOG				8
#define WIZ_EXPORT_LOG				9
#define WIZ_IMPORT_CONFIGURATION	10
#define WIZ_EXPORT_CONFIGURATION	11
#define WIZ_EXPORT_DTD				12
#define WIZ_EXPORT_XML				13

// Note: These numbers should not be changed - they are stored in the registry
#define WIZARD_RADIO_EXCEL			0L
#define WIZARD_RADIO_CALC			3L
#define WIZARD_RADIO_SPREADSHEET	1L
#define WIZARD_RADIO_ARB			2L

class CWizard : public wxWizard
{
public:
	CWizard(
			CAgilityBookDoc* pDoc,
			std::vector<ARBCalendarPtr>* pCalEntries = NULL,
			wxWindow* pParent = NULL);

	wxWizardPage* GetImportPage() const;
	wxWizardPage* GetExportPage() const;

// Attributes
private:
	CAgilityBookDoc* m_pDoc;
	std::vector<ARBCalendarPtr>* m_CalEntries;
	CWizardStart* m_pageStart;
	CWizardImport* m_pageImport;
	CWizardExport* m_pageExport;
	IWizardSpreadSheetPtr m_Excel;
	IWizardSpreadSheetPtr m_Calc;
	int m_ImportExportItem;
	int m_ImportExportStyle;
	wxButton* m_Finish;

// Operations
public:
	/**
	 * Get selected entries for export. If NULL, we're exporting everything.
	 */
	std::vector<ARBCalendarPtr>* GetCalendarEntries() const
	{
		return m_CalEntries;
	}
	/**
	 * Get the Excel wrapper.
	 */
	IWizardSpreadSheetPtr ExcelHelper()			{return m_Excel;}
	/**
	 * Get the Excel wrapper.
	 */
	IWizardSpreadSheetPtr CalcHelper()			{return m_Calc;}
	/**
	 * This lets CWizardImport/Export know what data is being processed.
	 */
	int GetImportExportItem() const				{return m_ImportExportItem;}
	int GetImportExportStyle() const			{return m_ImportExportStyle;}
	/**
	 * Only CWizardStart should call this.
	 */
	void SetImportExportItem(
			int item,
			int style)
	{
		m_ImportExportItem = item;
		m_ImportExportStyle = style;
	}
	/**
	 * Called by WizardStart
	 */
	void ResetData();

	void UpdateButtons(bool enableNextOrFinish);
};
