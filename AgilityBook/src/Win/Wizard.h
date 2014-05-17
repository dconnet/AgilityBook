#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Import/Export Wizard
 * @author David Connet
 *
 * Revision History
 * 2009-02-10 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2003-12-10 Created
 */

#include "SpreadSheet.h"
#include "ARB/ARBCalendar.h"
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
#define WIZ_IMPORT_SETTINGS			14
#define WIZ_EXPORT_SETTINGS			15

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
			std::vector<ARBCalendarPtr>* pCalEntries = nullptr,
			wxWindow* pParent = nullptr);

	wxWizardPage* GetImportPage() const;
	wxWizardPage* GetExportPage() const;

// Attributes
private:
	CAgilityBookDoc* m_pDoc;
	std::vector<ARBCalendarPtr>* m_CalEntries;
	CWizardStart* m_pageStart;
	CWizardImport* m_pageImport;
	CWizardExport* m_pageExport;
	ISpreadSheetPtr m_Excel;
	ISpreadSheetPtr m_Calc;
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
	ISpreadSheetPtr ExcelHelper()			{return m_Excel;}
	/**
	 * Get the Excel wrapper.
	 */
	ISpreadSheetPtr CalcHelper()			{return m_Calc;}
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
