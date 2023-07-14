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

#include "ARB/ARBCalendar.h"
#include "LibARBWin/Logger.h"
#include "LibARBWin/SpreadSheet.h"
#include <wx/wizard.h>
#include <vector>


namespace dconSoft
{
class CAgilityBookDoc;
class CWizardExport;
class CWizardImport;
class CWizardStart;


// These must agree with the order of items in sc_Items in WizardStart.cpp.
// They are simply for ease-of-coding.
constexpr int WIZ_IMPORT_RUNS = 0;
constexpr int WIZ_EXPORT_RUNS = 1;
constexpr int WIZ_IMPORT_CALENDAR = 2;
constexpr int WIZ_EXPORT_CALENDAR = 3;
constexpr int WIZ_EXPORT_CALENDAR_VCAL = 4;
constexpr int WIZ_EXPORT_CALENDAR_ICAL = 5;
constexpr int WIZ_EXPORT_CALENDAR_APPT = 6;
constexpr int WIZ_EXPORT_CALENDAR_TASK = 7;
constexpr int WIZ_IMPORT_LOG = 8;
constexpr int WIZ_EXPORT_LOG = 9;
constexpr int WIZ_IMPORT_CONFIGURATION = 10;
constexpr int WIZ_EXPORT_CONFIGURATION = 11;
constexpr int WIZ_EXPORT_DTD = 12;
constexpr int WIZ_EXPORT_XML = 13;
constexpr int WIZ_IMPORT_SETTINGS = 14;
constexpr int WIZ_EXPORT_SETTINGS = 15;

// Note: These numbers should not be changed - they are stored in the registry
constexpr long WIZARD_RADIO_EXCEL = 0L;
constexpr long WIZARD_RADIO_CALC = 3L;
constexpr long WIZARD_RADIO_SPREADSHEET = 1L;
constexpr long WIZARD_RADIO_ARB = 2L;


class CWizard : public wxWizard
{
public:
	CWizard(
		CAgilityBookDoc* pDoc,
		std::vector<ARB::ARBCalendarPtr>* pCalEntries = nullptr,
		wxWindow* pParent = nullptr);

	wxWizardPage* GetImportPage() const;
	wxWizardPage* GetExportPage() const;

	// Attributes
private:
	CAgilityBookDoc* m_pDoc;
	std::vector<ARB::ARBCalendarPtr>* m_CalEntries;
	CWizardStart* m_pageStart;
	CWizardImport* m_pageImport;
	CWizardExport* m_pageExport;
	ARBWin::ISpreadSheetPtr m_Excel;
	ARBWin::ISpreadSheetPtr m_Calc;
	int m_ImportExportItem;
	int m_ImportExportStyle;
	wxButton* m_Finish;
	ARBWin::CStackLogger m_trace;

	// Operations
public:
	/**
	 * Get selected entries for export. If NULL, we're exporting everything.
	 */
	std::vector<ARB::ARBCalendarPtr>* GetCalendarEntries() const
	{
		return m_CalEntries;
	}
	/**
	 * Get the Excel wrapper.
	 */
	ARBWin::ISpreadSheetPtr ExcelHelper()
	{
		return m_Excel;
	}
	/**
	 * Get the Excel wrapper.
	 */
	ARBWin::ISpreadSheetPtr CalcHelper()
	{
		return m_Calc;
	}
	/**
	 * This lets CWizardImport/Export know what data is being processed.
	 */
	int GetImportExportItem() const
	{
		return m_ImportExportItem;
	}
	int GetImportExportStyle() const
	{
		return m_ImportExportStyle;
	}
	/**
	 * Only CWizardStart should call this.
	 */
	void SetImportExportItem(int item, int style)
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

} // namespace dconSoft
