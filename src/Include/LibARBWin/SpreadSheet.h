#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Spreadsheet interface
 * @author David Connet
 *
 * Revision History
 * 2018-10-11 Moved to Win LibARBWin
 * 2012-02-18 Added eSpreadSheetNumberNoZero
 * 2012-02-05 Added alignment and formatting.
 * 2009-07-24 Removed option to export by array.
 * 2009-02-11 Ported to wxWidgets.
 * 2004-09-30 Created
 */

#if defined(__WXMSW__) && wxUSE_OLE_AUTOMATION
#define HAS_AUTOMATION 1
#else
#define HAS_AUTOMATION 0
#endif

#include "LibwxARBWin.h"

#include "ARBCommon/ARBTypes.h"
#include <string>
#include <vector>
class IDlgProgress;

ARB_TYPEDEF(ISpreadSheet)
ARB_TYPEDEF(ISpreadSheetExporter)
ARB_TYPEDEF(ISpreadSheetImporter)


enum class ARBSpreadSheetAlign
{
	None,
	General,
	Left,
	Right,
	Center,
};
enum class ARBSpreadSheetFormat
{
	Text,         ///< General text
	Currency,     ///< Currency, "$  (redneg)", 0 == "-"
	Number,       ///< 0dec number, 1000sep, (redneg)
	NumberNoZero, ///< 0dec number, 1000sep, (redneg), 0 == ""
	Date,         ///< m/d/yyyy
};
enum class ARBSpreadSheetType
{
	MicrosoftExcel,
	OpenOfficeCalc
};


/**
 * Interface for exporting to a spreadsheet.
 */
class ARBWIN_API ISpreadSheetExporter
{
public:
	virtual ~ISpreadSheetExporter() = default;

	virtual wchar_t GetSumIfSeparator() const = 0;

	virtual bool AllowAccess(bool bAllow) = 0;

	virtual bool SetTextColor(long inRow, long inCol, wxColour inColor) = 0;
	virtual bool SetBackColor(long inRow, long inCol, wxColour inColor) = 0;
	virtual bool SetItalic(long inRow, long inCol, bool bItalic) = 0;
	virtual bool SetBold(long inRow, long inCol, bool bBold) = 0;

	virtual bool SetAlignment(long inRow, long inCol, ARBSpreadSheetAlign align) = 0;
	virtual bool SetFormat(long inRow, long inCol, ARBSpreadSheetFormat format) = 0;
	// Direct formatting
	virtual bool SetFormat(long inRow, long inCol, std::wstring const& format) = 0;

	virtual bool InsertData(long inRow, long inCol, double inData) = 0;
	virtual bool InsertData(long inRow, long inCol, std::wstring const& inData, bool bFormula = false) = 0;

	virtual bool AutoFit(long inColFrom, long inColTo) = 0;
};

/**
 * Interface for importing from a spreadsheet.
 */
class ARBWIN_API ISpreadSheetImporter
{
public:
	virtual ~ISpreadSheetImporter() = default;
	virtual bool OpenFile(std::wstring const& inFilename) = 0;
	virtual bool GetData(std::vector<std::vector<std::wstring>>& outData, IDlgProgress* ioProgress = nullptr) = 0;
};

/////////////////////////////////////////////////////////////////////////////

class ARBWIN_API ISpreadSheet
{
public:
	/**
	 * Create a new spreadsheet manager, must 'delete' the returned pointer.
	 */
	static ISpreadSheetPtr Create(ARBSpreadSheetType inType);

	virtual ~ISpreadSheet() = default;

	/// Get the maximum number of rows Excel can handle.
	static long GetMaxRows();
	/// Get the maximum number of columns Excel can handle.
	static long GetMaxCols();
	/// Translate a given row/col into Excel notation: (0,0) -> "A1"
	static bool GetRowCol(long inRow, long inCol, std::wstring& outCell);

	virtual ISpreadSheetExporterPtr GetExporter() const = 0;
	virtual ISpreadSheetImporterPtr GetImporter() const = 0;
};
