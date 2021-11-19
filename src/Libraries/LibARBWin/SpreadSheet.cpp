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
 * 2018-12-16 Convert to fmt.
 * 2018-10-11 Moved to Win LibARBWin
 * 2012-05-16 Fixed Calc format strings.
 * 2012-02-18 Added eSpreadSheetNumberNoZero
 * 2012-02-05 Added alignment and formatting.
 * 2012-02-04 Clear data in GetRowCol. Fix writing formulas.
 * 2012-01-14 Change creation to only create one worksheet.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-07-24 Removed option to export by array, completed wx port.
 * 2009-02-11 Ported to wxWidgets.
 * 2004-09-30 Created
 */

#include "stdafx.h"
#include "LibARBWin/SpreadSheet.h"

#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/StringUtil.h"
#include "fmt/xchar.h"

#if HAS_AUTOMATION
#include "LibARBWin/DlgProgress.h"
#include <wx/filename.h>
#include <wx/list.h>
#include <wx/msw/ole/automtn.h>
#include <wx/variant.h>

// illegal copy-initialization; more than one user-defined conversion has been implicitly applied
#pragma warning(disable : 4928)
#endif // HAS_AUTOMATION

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

// General note about data and formulas - in both Excel and Calc writing
// individual cells auto-formats (formulas work) and writing of arrays
// acts like raw data - even if you call the put_Formula method.

/////////////////////////////////////////////////////////////////////////////

#if HAS_AUTOMATION

class CWizardBaseImport : public ISpreadSheetImporter
{
public:
	CWizardBaseImport()
	{
	}

protected:
	std::wstring m_FileName;
};

/////////////////////////////////////////////////////////////////////////////

ARB_TYPEDEF(CWizardExcel)
ARB_TYPEDEF(CWizardExcelExport)
ARB_TYPEDEF(CWizardExcelImport)
ARB_TYPEDEF(CWizardCalc)
ARB_TYPEDEF(CWizardCalcExport)
ARB_TYPEDEF(CWizardCalcImport)

class CWizardExcel : public ISpreadSheet
{
	DECLARE_NO_COPY_IMPLEMENTED(CWizardExcel)
public:
	static CWizardExcelPtr Create();
	CWizardExcel();
	virtual ~CWizardExcel();

	ISpreadSheetExporterPtr GetExporter() const override;
	ISpreadSheetImporterPtr GetImporter() const override;

private:
	mutable wxAutomationObject m_App;
};


class CWizardExcelExport : public ISpreadSheetExporter
{
	DECLARE_NO_COPY_IMPLEMENTED(CWizardExcelExport)
protected:
	// Copied from C# code that used reflection.
	enum XlWBATemplate
	{
		xlWBATChart = -4109,
		xlWBATExcel4IntlMacroSheet = 4,
		xlWBATExcel4MacroSheet = 3,
		xlWBATWorksheet = -4167,
	};
	enum XlSheetType
	{
		xlChart = -4109,
		xlDialogSheet = -4116,
		xlExcel4IntlMacroSheet = 4,
		xlExcel4MacroSheet = 3,
		xlWorksheet = -4167,
	};
	// object.HorizontalAlignment = XlHAlign.type
	// similar to object.Font.Bold = 1
	enum XlHAlign
	{
		xlHAlignCenter = -4108,
		xlHAlignCenterAcrossSelection = 7,
		xlHAlignDistributed = -4117,
		xlHAlignFill = 5,
		xlHAlignGeneral = 1,
		xlHAlignJustify = -4130,
		xlHAlignLeft = -4131,
		xlHAlignRight = -4152,
	};

public:
	static CWizardExcelExportPtr Create(wxAutomationObject& ioApp);
	CWizardExcelExport(wxAutomationObject& ioApp);
	virtual ~CWizardExcelExport();

	wchar_t GetSumIfSeparator() const override;

	bool AllowAccess(bool bAllow) override;

	bool SetTextColor(long inRow, long inCol, wxColour inColor) override;
	bool SetBackColor(long inRow, long inCol, wxColour inColor) override;
	bool SetItalic(long inRow, long inCol, bool bItalic) override;
	bool SetBold(long inRow, long inCol, bool bBold) override;

	bool SetAlignment(long inRow, long inCol, ARBSpreadSheetAlign align) override;
	bool SetFormat(long inRow, long inCol, ARBSpreadSheetFormat format) override;
	bool SetFormat(long inRow, long inCol, std::wstring const& format) override;

	bool InsertData(long inRow, long inCol, double inData) override;
	bool InsertData(long inRow, long inCol, std::wstring const& inData, bool bFormula = false) override;

	bool AutoFit(long inColFrom, long inColTo) override;

private:
	wxAutomationObject& m_App;
	wxAutomationObject m_Worksheet;
};


class CWizardExcelImport : public CWizardBaseImport
{
	DECLARE_NO_COPY_IMPLEMENTED(CWizardExcelImport)
public:
	static CWizardExcelImportPtr Create(wxAutomationObject& ioApp);
	CWizardExcelImport(wxAutomationObject& ioApp);
	virtual ~CWizardExcelImport();

	bool OpenFile(std::wstring const& inFilename) override;
	bool GetData(std::vector<std::vector<std::wstring>>& outData, IDlgProgress* ioProgress) override;

private:
	wxAutomationObject& m_App;
	wxAutomationObject m_Worksheet;
};

/////////////////////////////////////////////////////////////////////////////

class CWizardCalc : public ISpreadSheet
{
	DECLARE_NO_COPY_IMPLEMENTED(CWizardCalc)
public:
	static CWizardCalcPtr Create();
	CWizardCalc();
	virtual ~CWizardCalc();

	ISpreadSheetExporterPtr GetExporter() const override;
	ISpreadSheetImporterPtr GetImporter() const override;

private:
	mutable wxAutomationObject m_Manager;
	mutable wxAutomationObject m_Desktop;
};


class CWizardCalcExport : public ISpreadSheetExporter
{
	DECLARE_NO_COPY_IMPLEMENTED(CWizardCalcExport)
public:
	static CWizardCalcExportPtr Create(wxAutomationObject& ioManager, wxAutomationObject& ioDesktop);
	CWizardCalcExport(wxAutomationObject& ioManager, wxAutomationObject& ioDesktop);
	virtual ~CWizardCalcExport();

	wchar_t GetSumIfSeparator() const override;

	bool AllowAccess(bool bAllow) override;

	bool SetTextColor(long inRow, long inCol, wxColour inColor) override;
	bool SetBackColor(long inRow, long inCol, wxColour inColor) override;
	bool SetItalic(long inRow, long inCol, bool bItalic) override;
	bool SetBold(long inRow, long inCol, bool bBold) override;

	bool SetAlignment(long inRow, long inCol, ARBSpreadSheetAlign align) override;
	bool SetFormat(long inRow, long inCol, ARBSpreadSheetFormat format) override;
	bool SetFormat(long inRow, long inCol, std::wstring const& format) override;

	bool InsertData(long inRow, long inCol, double inData) override;
	bool InsertData(long inRow, long inCol, std::wstring const& inData, bool bFormula = false) override;

	bool AutoFit(long inColFrom, long inColTo) override;

private:
	bool CreateWorksheet();

	wxAutomationObject& m_Manager;
	wxAutomationObject& m_Desktop;
	wxAutomationObject m_Document;
	wxAutomationObject m_Worksheet;
};


class CWizardCalcImport : public CWizardBaseImport
{
	DECLARE_NO_COPY_IMPLEMENTED(CWizardCalcImport)
public:
	static CWizardCalcImportPtr Create(wxAutomationObject& ioDesktop);
	CWizardCalcImport(wxAutomationObject& ioDesktop);
	virtual ~CWizardCalcImport();

	bool OpenFile(std::wstring const& inFilename) override;
	bool GetData(std::vector<std::vector<std::wstring>>& outData, IDlgProgress* ioProgress) override;

private:
	wxAutomationObject& m_Desktop;
	wxAutomationObject m_Document;
	wxAutomationObject m_Worksheet;
};

/////////////////////////////////////////////////////////////////////////////

CWizardExcelPtr CWizardExcel::Create()
{
	wxBusyCursor wait;
	CWizardExcelPtr pExcel = std::make_shared<CWizardExcel>();
	if (pExcel)
	{
		bool bKill = false;
		if (!pExcel->m_App.GetDispatchPtr())
			bKill = true;
		else
		{
			if (!pExcel->GetExporter() || !pExcel->GetImporter())
				bKill = true;
		}
		if (bKill)
			pExcel.reset();
	}
	return pExcel;
}


CWizardExcel::CWizardExcel()
{
	wxLogNull logSuppressor;
	if (!m_App.GetInstance(L"Excel.Application"))
	{
		if (!m_App.CreateInstance(L"Excel.Application"))
			return;
	}
}


CWizardExcel::~CWizardExcel()
{
}


ISpreadSheetExporterPtr CWizardExcel::GetExporter() const
{
	return ISpreadSheetExporterPtr(CWizardExcelExport::Create(m_App));
}


ISpreadSheetImporterPtr CWizardExcel::GetImporter() const
{
	return ISpreadSheetImporterPtr(CWizardExcelImport::Create(m_App));
}

/////////////////////////////////////////////////////////////////////////////

CWizardExcelExportPtr CWizardExcelExport::Create(wxAutomationObject& ioApp)
{
	CWizardExcelExportPtr ptr;
	if (ioApp.GetDispatchPtr())
		ptr = std::make_shared<CWizardExcelExport>(ioApp);
	return ptr;
}


CWizardExcelExport::CWizardExcelExport(wxAutomationObject& ioApp)
	: m_App(ioApp)
	, m_Worksheet()
{
	// Create a new workbook.
	wxAutomationObject book = m_App.CallMethod(L"Workbooks.Add", xlWBATWorksheet);
	// Get the first sheet.
	wxAutomationObject sheets = book.GetProperty(L"Sheets");
	wxVariant args[1];
	args[0] = wxVariant(static_cast<short>(1));
	sheets.GetObject(m_Worksheet, L"Item", 1, args);

	// wx doesn't support IDispatch as a variant
	// sheets.CallMethod(L"Add", wxNullVariant, m_Worksheet, wxNullVariant, xlWorksheet);
	// This also doesn't work
	// sheets.CallMethod(L"Add", wxNullVariant, wxNullVariant, wxNullVariant, xlWorksheet);
	// The following python works:
	// import win32com.client
	// app = win32com.client.Dispatch('Excel.Application')
	// app.Visible = True
	// book = app.Workbooks.Add(-4167)
	// sheet = book.Sheets(1)
	// sheet.Name = 'Test'
	// sheet2 = book.Sheets.Add(None, sheet, None, -4167)
	// sheetFirst = book.Sheets.Add(None, None, None, -4167)
}


CWizardExcelExport::~CWizardExcelExport()
{
	if (m_Worksheet.GetDispatchPtr() && !m_App.GetProperty(L"Visible").GetBool())
		m_App.CallMethod(L"Quit");
}


wchar_t CWizardExcelExport::GetSumIfSeparator() const
{
	return L',';
}


bool CWizardExcelExport::AllowAccess(bool bAllow)
{
	if (bAllow)
	{
		m_App.PutProperty(L"UserControl", true);
		m_App.PutProperty(L"Visible", true);
	}
	else
		m_App.PutProperty(L"UserControl", false);
	return true;
}


bool CWizardExcelExport::SetTextColor(long inRow, long inCol, wxColour inColor)
{
	std::wstring cell1;
	if (!ISpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, L"Range", 2, args);
	wxAutomationObject font;
	range.GetObject(font, L"Font");
	font.PutProperty(L"Color", (long)inColor.GetPixel());
	return true;
}


bool CWizardExcelExport::SetBackColor(long inRow, long inCol, wxColour inColor)
{
	std::wstring cell1;
	if (!ISpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, L"Range", 2, args);
	wxAutomationObject interior;
	range.GetObject(interior, L"Interior");
	interior.PutProperty(L"Color", (long)inColor.GetPixel());
	return true;
}


bool CWizardExcelExport::SetItalic(long inRow, long inCol, bool bItalic)
{
	std::wstring cell1;
	if (!ISpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, L"Range", 2, args);
	wxAutomationObject font;
	range.GetObject(font, L"Font");
	font.PutProperty(L"Italic", static_cast<short>(bItalic));
	return true;
}


bool CWizardExcelExport::SetBold(long inRow, long inCol, bool bBold)
{
	std::wstring cell1;
	if (!ISpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, L"Range", 2, args);
	wxAutomationObject font;
	range.GetObject(font, L"Font");
	font.PutProperty(L"Bold", static_cast<short>(bBold));
	return true;
}


bool CWizardExcelExport::SetAlignment(long inRow, long inCol, ARBSpreadSheetAlign align)
{
	if (ARBSpreadSheetAlign::None == align)
		return true;
	std::wstring cell1;
	if (!ISpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, L"Range", 2, args);
	short val = xlHAlignGeneral;
	switch (align)
	{
	case ARBSpreadSheetAlign::None:
	case ARBSpreadSheetAlign::General:
		val = xlHAlignGeneral;
		break;
	case ARBSpreadSheetAlign::Left:
		val = xlHAlignLeft;
		break;
	case ARBSpreadSheetAlign::Right:
		val = xlHAlignRight;
		break;
	case ARBSpreadSheetAlign::Center:
		val = xlHAlignCenter;
		break;
	}
	range.PutProperty(L"HorizontalAlignment", val);
	return true;
}


bool CWizardExcelExport::SetFormat(long inRow, long inCol, ARBSpreadSheetFormat format)
{
	std::wstring sFormat;

	/*
	 * Good tips on what to change
	 * http://www.tek-tips.com/faqs.cfm?fid=6715
	 *
	 * general:
	 * number,2dec,1000sep,(redneg): #,##0.00_);[Red](#,##0.00)
	 * number,2dec,1000sep,(neg): #,##0.00_);(#,##0.00)
	 * number,2dec,-neg: 0.00
	 * number,2dec,1000sep,-neg: #,##0.00
	 * currency: $#,##0.00_);[Red]($#,##0.00)
	 * currency,left-aligned $, "-" when 0
	 * custom: _($* #,##0.00_);[Red]_($* (#,##0.00);_(* "-"??_);_(@_)
	 *
	 * (range).NumberFormat = "formatstr"
	 *
	 * align (also on Range)
	 * HorizontalAlignment = xlHAlignCenter
	 *
	 * borders (Range)
	 * Borders(xlEdgeBottom).LineStyle = xlDouble
	 * Borders(xlEdgeBottom).Weight = xlThick
	 * Borders(xlEdgeBottom).ColorIndex = 1
	 * Borders(xlEdgeBottom).LineStyle = xlContinuous
	 */

	switch (format)
	{
	case ARBSpreadSheetFormat::Text:
		sFormat = L"@";
		break;
	case ARBSpreadSheetFormat::Currency:
		sFormat = L"_($* #,##0.00_);[Red]_($* (#,##0.00);_(* \"-\"??_);_(@_)";
		break;
	case ARBSpreadSheetFormat::Number:
		sFormat = L"#,##0_);[Red](#,##0)";
		break;
	case ARBSpreadSheetFormat::NumberNoZero:
		sFormat = L"#,##0_);[Red](#,##0);_(* \"\"??_);_(@_)";
		break;
	case ARBSpreadSheetFormat::Date:
		sFormat = L"d/m/yyyy";
		break;
	}
	return SetFormat(inRow, inCol, sFormat);
}


bool CWizardExcelExport::SetFormat(long inRow, long inCol, std::wstring const& format)
{
	std::wstring cell1;
	if (!ISpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, L"Range", 2, args);
	range.PutProperty(L"NumberFormat", format.c_str());
	return true;
}


bool CWizardExcelExport::InsertData(long inRow, long inCol, double inData)
{
	std::wstring cell1;
	if (!ISpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, L"Range", 2, args);
	range.PutProperty(L"Value2", inData);
	return true;
}


bool CWizardExcelExport::InsertData(long inRow, long inCol, std::wstring const& inData, bool bFormula)
{
	std::wstring cell1;
	if (!ISpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, L"Range", 2, args);
	std::wstring prop = bFormula ? L"Formula" : L"Value2";
	if (0 < inData.length() && '=' == inData[0] && !bFormula)
	{
		std::wstring data = std::wstring(L"'") + inData;
		range.PutProperty(prop, data.c_str());
	}
	else
		range.PutProperty(prop, inData.c_str());
	return true;
}


bool CWizardExcelExport::AutoFit(long inColFrom, long inColTo)
{
	std::wstring cell1, cell2;
	if (!ISpreadSheet::GetRowCol(0, inColFrom, cell1))
		return false;
	if (!ISpreadSheet::GetRowCol(0, inColTo, cell2))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell2;
	m_Worksheet.GetObject(range, L"Range", 2, args);
	wxAutomationObject cols;
	range.GetObject(cols, L"EntireColumn");
	cols.CallMethod(L"AutoFit");
	return true;
}

/////////////////////////////////////////////////////////////////////////////

CWizardExcelImportPtr CWizardExcelImport::Create(wxAutomationObject& ioApp)
{
	CWizardExcelImportPtr ptr;
	if (ioApp.GetDispatchPtr())
		ptr = std::make_shared<CWizardExcelImport>(ioApp);
	return ptr;
}

CWizardExcelImport::CWizardExcelImport(wxAutomationObject& ioApp)
	: m_App(ioApp)
{
}


CWizardExcelImport::~CWizardExcelImport()
{
	if (m_Worksheet.GetDispatchPtr())
		m_App.CallMethod(L"Quit");
}


bool CWizardExcelImport::OpenFile(std::wstring const& inFilename)
{
	wxVariant bk = m_App.CallMethod(L"Workbooks.Open", inFilename.c_str());
	wxAutomationObject book((WXIDISPATCH*)bk.GetVoidPtr());
	if (!book.GetDispatchPtr())
		return false;
	m_FileName = inFilename;
	// Get the first sheet.
	wxAutomationObject sheets = book.GetProperty(L"Sheets");
	wxVariant args2[1];
	args2[0] = wxVariant(static_cast<short>(1));
	sheets.GetObject(m_Worksheet, L"Item", 1, args2);
	return !!m_Worksheet.GetDispatchPtr();
}


bool CWizardExcelImport::GetData(std::vector<std::vector<std::wstring>>& outData, IDlgProgress* ioProgress)
{
	outData.clear();
	if (!m_Worksheet.GetDispatchPtr())
		return false;
	wxAutomationObject range;
	m_Worksheet.GetObject(range, L"UsedRange");
	long iRow = range.GetProperty(L"Row").GetLong();
	long iCol = range.GetProperty(L"Column").GetLong();
	wxAutomationObject rangeRows, rangeColumns;
	range.GetObject(rangeRows, L"Rows");
	range.GetObject(rangeColumns, L"Columns");
	long nRows = rangeRows.GetProperty(L"Count").GetLong();
	long nCols = rangeColumns.GetProperty(L"Count").GetLong();
	if (0 < nRows && 0 < nCols)
	{
		if (ioProgress)
		{
			wxFileName filename(m_FileName);
			std::wstring msg = filename.GetFullPath();
			ioProgress->SetCaption(msg);
			std::wstring str = fmt::format(_("Reading {0} rows and {1} columns").wx_str(), nRows, nCols);
			ioProgress->SetMessage(str);
			ioProgress->SetRange(1, nRows);
			ioProgress->ShowProgress();
		}
		bool bAbort = false;
		outData.reserve(nRows);
		for (int iCellRow = 0; !bAbort && iCellRow < nRows; ++iCellRow)
		{
			if (ioProgress)
			{
				ioProgress->StepIt(1);
				if (ioProgress->HasCanceled())
					bAbort = true;
			}
			std::vector<std::wstring> row;
			row.reserve(nCols);
			for (int iCellCol = 0; iCellCol < nCols; ++iCellCol)
			{
				std::wstring cell1;
				if (!ISpreadSheet::GetRowCol(iRow + iCellRow - 1, iCol + iCellCol - 1, cell1))
				{
					bAbort = true;
					break;
				}
				wxAutomationObject range2;
				wxVariant args[2];
				args[0] = cell1;
				args[1] = cell1;
				range.GetObject(range2, L"Range", 2, args);
				row.push_back(StringUtil::stringW(range2.GetProperty(L"Value").GetString()));
			}
			outData.push_back(row);
		}
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////

CWizardCalcPtr CWizardCalc::Create()
{
	wxBusyCursor wait;
	CWizardCalcPtr pCalc = std::make_shared<CWizardCalc>();
	if (pCalc)
	{
		bool bKill = false;
		if (!pCalc->m_Desktop.GetDispatchPtr())
			bKill = true;
		else
		{
			if (!pCalc->GetExporter() || !pCalc->GetImporter())
				bKill = true;
		}
		if (bKill)
			pCalc.reset();
	}
	return pCalc;
}


CWizardCalc::CWizardCalc()
{
	wxLogNull logSuppressor;
	if (!m_Manager.GetInstance(L"com.sun.star.ServiceManager"))
	{
		if (!m_Manager.CreateInstance(L"com.sun.star.ServiceManager"))
			return;
	}
	m_Desktop.SetDispatchPtr(m_Manager.CallMethod(L"createInstance", L"com.sun.star.frame.Desktop"));
}


CWizardCalc::~CWizardCalc()
{
}


ISpreadSheetExporterPtr CWizardCalc::GetExporter() const
{
	return ISpreadSheetExporterPtr(CWizardCalcExport::Create(m_Manager, m_Desktop));
}


ISpreadSheetImporterPtr CWizardCalc::GetImporter() const
{
	return ISpreadSheetImporterPtr(CWizardCalcImport::Create(m_Desktop));
}

/////////////////////////////////////////////////////////////////////////////

CWizardCalcExportPtr CWizardCalcExport::Create(wxAutomationObject& ioManager, wxAutomationObject& ioDesktop)
{
	return std::make_shared<CWizardCalcExport>(ioManager, ioDesktop);
}


CWizardCalcExport::CWizardCalcExport(wxAutomationObject& ioManager, wxAutomationObject& ioDesktop)
	: m_Manager(ioManager)
	, m_Desktop(ioDesktop)
	, m_Document()
	, m_Worksheet()
{
}


CWizardCalcExport::~CWizardCalcExport()
{
}


bool CWizardCalcExport::CreateWorksheet()
{
	if (m_Desktop.GetDispatchPtr() && !m_Document.GetDispatchPtr())
	{
		wxVariant args;
		args.NullList();
		wxVariant retval = m_Desktop.CallMethod(L"loadComponentFromURL", L"private:factory/scalc", L"_blank", 0, args);
		if (!retval.IsNull())
		{
			m_Document.SetDispatchPtr(retval);
		}
	}
	if (m_Document.GetDispatchPtr() && !m_Worksheet.GetDispatchPtr())
	{
		wxAutomationObject sheets = m_Document.CallMethod(L"getSheets");
		m_Worksheet.SetDispatchPtr(sheets.CallMethod(L"getByIndex", 0));
	}
	return !!m_Worksheet.GetDispatchPtr();
}


wchar_t CWizardCalcExport::GetSumIfSeparator() const
{
	return L';';
}


bool CWizardCalcExport::AllowAccess(bool bAllow)
{
	// Calc doesn't seem to support controlling user access like Excel does
	return false;
}


bool CWizardCalcExport::SetTextColor(long inRow, long inCol, wxColour inColor)
{
	wxAutomationObject cell(m_Worksheet.CallMethod(L"getCellByPosition", inCol, inRow));
	if (!cell.GetDispatchPtr())
		return false;
	return cell.PutProperty(L"CharColor", static_cast<long>(inColor.GetPixel()));
}


bool CWizardCalcExport::SetBackColor(long inRow, long inCol, wxColour inColor)
{
	wxAutomationObject cell(m_Worksheet.CallMethod(L"getCellByPosition", inCol, inRow));
	if (!cell.GetDispatchPtr())
		return false;
	return cell.PutProperty(L"CellBackColor", static_cast<long>(inColor.GetPixel()));
}


bool CWizardCalcExport::SetItalic(long inRow, long inCol, bool bItalic)
{
	wxAutomationObject cell(m_Worksheet.CallMethod(L"getCellByPosition", inCol, inRow));
	if (!cell.GetDispatchPtr())
		return false;
	return cell.PutProperty(L"CharPosture", bItalic ? 1L : 0L);
}


bool CWizardCalcExport::SetBold(long inRow, long inCol, bool bBold)
{
	wxAutomationObject cell(m_Worksheet.CallMethod(L"getCellByPosition", inCol, inRow));
	if (!cell.GetDispatchPtr())
		return false;
	return cell.PutProperty(L"CharWeight", bBold ? 150L : 100L);
}


bool CWizardCalcExport::SetAlignment(long inRow, long inCol, ARBSpreadSheetAlign align)
{
	if (ARBSpreadSheetAlign::None == align)
		return true;
	wxAutomationObject cell(m_Worksheet.CallMethod(L"getCellByPosition", inCol, inRow));
	if (!cell.GetDispatchPtr())
		return false;

	// com.sun.star.table.CellHoriJustify.STANDARD
	long val = 0;
	switch (align)
	{
	case ARBSpreadSheetAlign::None:
	case ARBSpreadSheetAlign::General:
		// com.sun.star.table.CellHoriJustify.STANDARD
		val = 0;
		break;
	case ARBSpreadSheetAlign::Left:
		// com.sun.star.table.CellHoriJustify.LEFT
		val = 1;
		break;
	case ARBSpreadSheetAlign::Center:
		// com.sun.star.table.CellHoriJustify.CENTER
		val = 2;
		break;
	case ARBSpreadSheetAlign::Right:
		// com.sun.star.table.CellHoriJustify.RIGHT
		val = 3;
		break;
	}
	return cell.PutProperty(L"HoriJustify", val);
}


bool CWizardCalcExport::SetFormat(long inRow, long inCol, ARBSpreadSheetFormat format)
{
	std::wstring sFormat;
	switch (format)
	{
	case ARBSpreadSheetFormat::Text:
	case ARBSpreadSheetFormat::NumberNoZero:
		sFormat = L"@";
		break;
	case ARBSpreadSheetFormat::Currency:
		sFormat = L"_($#,##0.00_);[RED]_($(#,##0.00);_(\"-\"??_);_(@_)";
		break;
	case ARBSpreadSheetFormat::Number:
		sFormat = L"#,##0_);[RED](#,##0)";
		break;
	case ARBSpreadSheetFormat::Date:
		sFormat = L"DD/MM/YYYY";
		break;
	}
	return SetFormat(inRow, inCol, sFormat);
}


bool CWizardCalcExport::SetFormat(long inRow, long inCol, std::wstring const& format)
{
	if (!CreateWorksheet())
		return false;
	wxAutomationObject cell(m_Worksheet.CallMethod(L"getCellByPosition", inCol, inRow));
	if (!cell.GetDispatchPtr())
		return false;

	wxAutomationObject numberFormats(m_Document.CallMethod(L"getNumberFormats"));
	if (!numberFormats.GetDispatchPtr())
		return false;

	wxAutomationObject localSettings;
	{
		// This creation keeps throwing some weird invalid property value
		// messages. Doesn't seem to do anything, so ignore them.
		wxLogNull logSuppressor;
		localSettings.SetDispatchPtr(m_Manager.CallMethod(L"createInstance", L"com.sun.star.lang.Locale"));
	}
	localSettings.PutProperty(L"Language", wxString(L"en"));
	localSettings.PutProperty(L"Country", wxString(L"US"));

	wxVariant numberFormatId
		= numberFormats.CallMethod(L"queryKey", format.c_str(), localSettings.GetDispatchPtr(), true);
	if (numberFormatId.GetInteger() == -1)
		numberFormatId = numberFormats.CallMethod(L"addNew", format.c_str(), localSettings.GetDispatchPtr());

	return cell.PutProperty(L"NumberFormat", numberFormatId);
}


bool CWizardCalcExport::InsertData(long inRow, long inCol, double inData)
{
	if (!CreateWorksheet())
		return false;
	wxAutomationObject cell(m_Worksheet.CallMethod(L"getCellByPosition", inCol, inRow));
	if (!cell.GetDispatchPtr())
		return false;
	return cell.PutProperty(L"Value", inData);
}


bool CWizardCalcExport::InsertData(long inRow, long inCol, std::wstring const& inData, bool bFormula)
{
	if (!CreateWorksheet())
		return false;
	wxAutomationObject cell(m_Worksheet.CallMethod(L"getCellByPosition", inCol, inRow));
	if (!cell.GetDispatchPtr())
		return false;
	std::wstring property = bFormula ? L"Formula" : L"String";
	return cell.PutProperty(property, inData.c_str());
}


bool CWizardCalcExport::AutoFit(long inColFrom, long inColTo)
{
	if (!CreateWorksheet())
		return false;
	wxAutomationObject range(m_Worksheet.CallMethod(L"getCellRangeByPosition", inColFrom, 0, inColTo, 0));
	if (!range.GetDispatchPtr())
		return false;
	range.PutProperty(L"Columns.OptimalWidth", true);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

CWizardCalcImportPtr CWizardCalcImport::Create(wxAutomationObject& ioDesktop)
{
	return std::make_shared<CWizardCalcImport>(ioDesktop);
}


CWizardCalcImport::CWizardCalcImport(wxAutomationObject& ioDesktop)
	: m_Desktop(ioDesktop)
{
}


CWizardCalcImport::~CWizardCalcImport()
{
}


bool CWizardCalcImport::OpenFile(std::wstring const& inFilename)
{
	if (!m_Document.GetDispatchPtr())
	{
		// 'Calc' doesn't take kindly to wxWidgets FileNameToURL syntax.
		// Since this is windows only, screw it, just format it the way calc likes.
		std::wstring fileName;
		fileName = StringUtil::Replace(inFilename, L"\\", L"/");
		fileName = L"file:///" + fileName;
		wxVariant args;
		args.NullList();
		wxVariant file = m_Desktop.CallMethod(L"loadComponentFromURL", fileName.c_str(), L"_blank", 0, args);
		if (file.IsNull())
			return false;
		m_Document.SetDispatchPtr(file);
	}
	if (!m_Worksheet.GetDispatchPtr())
	{
		wxAutomationObject sheets = m_Document.CallMethod(L"getSheets");
		m_Worksheet.SetDispatchPtr(sheets.CallMethod(L"getByIndex", 0));
	}
	if (!m_Worksheet.GetDispatchPtr())
		return false;
	m_FileName = inFilename;
	return true;
}


bool CWizardCalcImport::GetData(std::vector<std::vector<std::wstring>>& outData, IDlgProgress* ioProgress)
{
	outData.clear();
	if (!m_Worksheet.GetDispatchPtr())
		return false;

	wxAutomationObject cursor = m_Worksheet.CallMethod(L"createCursor");
	if (!cursor.GetDispatchPtr())
		return false;

	// Set up the data area
	// Ignore failure codes
	// - these are returning failure, but appear to be working correctly.
	cursor.CallMethod(L"gotoStartOfUsedArea", 0);
	cursor.CallMethod(L"gotoEndOfUsedArea", 1);

	// This should be a list of lists.
	wxVariant dataAll = cursor.CallMethod(L"getDataArray");
	if (!dataAll.IsType(L"list"))
		return false;
	bool bAbort = false;
	int nRows = static_cast<int>(dataAll.GetCount());
	outData.reserve(nRows);
	if (ioProgress)
	{
		wxFileName filename(m_FileName);
		std::wstring msg = filename.GetFullPath();
		ioProgress->SetCaption(msg);
		std::wstring str = fmt::format(_("Reading {} rows").wx_str(), nRows);
		ioProgress->SetMessage(str);
		ioProgress->SetRange(1, nRows);
		ioProgress->ShowProgress();
	}
	for (int iRow = 0; !bAbort && iRow < nRows; ++iRow)
	{
		if (ioProgress)
		{
			ioProgress->StepIt(1);
			if (ioProgress->HasCanceled())
				bAbort = true;
		}
		wxVariant vRow = dataAll[iRow];
		if (!vRow.IsType(L"list"))
			continue;
		int nCols = static_cast<int>(vRow.GetCount());
		std::vector<std::wstring> row;
		row.reserve(nCols);
		for (int iCol = 0; !bAbort && iCol < nCols; ++iCol)
		{
			row.push_back(StringUtil::stringW(vRow[iCol].GetString()));
		}
		outData.push_back(row);
	}
	return true;
}

#endif // HAS_AUTOMATION

/////////////////////////////////////////////////////////////////////////////

ISpreadSheetPtr ISpreadSheet::Create(ARBSpreadSheetType inType)
{
	ISpreadSheetPtr pInterface;
#if HAS_AUTOMATION
	switch (inType)
	{
	case ARBSpreadSheetType::MicrosoftExcel:
		pInterface = ISpreadSheetPtr(CWizardExcel::Create());
		break;
	case ARBSpreadSheetType::OpenOfficeCalc:
		pInterface = ISpreadSheetPtr(CWizardCalc::Create());
		break;
	}
#endif
	return pInterface;
}


// static helper functions
long ISpreadSheet::GetMaxRows()
{
	return 65536; // Excel limits (I believe later versions of excel/calc may expand this, but we support older versions
				  // too)
}


long ISpreadSheet::GetMaxCols()
{
	return 256; // Excel limits
}


bool ISpreadSheet::GetRowCol(long inRow, long inCol, std::wstring& outCell)
{
	// Lookup tables are nice!
	static const wchar_t* const sc_ColumnNames[256] = {
		// clang-format off
		L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H",
		L"I", L"J", L"K", L"L", L"M", L"N", L"O", L"P",
		L"Q", L"R", L"S", L"T", L"U", L"V", L"W", L"X",
		L"Y", L"Z", L"AA",L"AB",L"AC",L"AD",L"AE",L"AF",
		L"AG",L"AH",L"AI",L"AJ",L"AK",L"AL",L"AM",L"AN",
		L"AO",L"AP",L"AQ",L"AR",L"AS",L"AT",L"AU",L"AV",
		L"AW",L"AX",L"AY",L"AZ",L"BA",L"BB",L"BC",L"BD",
		L"BE",L"BF",L"BG",L"BH",L"BI",L"BJ",L"BK",L"BL",
		L"BM",L"BN",L"BO",L"BP",L"BQ",L"BR",L"BS",L"BT",
		L"BU",L"BV",L"BW",L"BX",L"BY",L"BZ",L"CA",L"CB",
		L"CC",L"CD",L"CE",L"CF",L"CG",L"CH",L"CI",L"CJ",
		L"CK",L"CL",L"CM",L"CN",L"CO",L"CP",L"CQ",L"CR",
		L"CS",L"CT",L"CU",L"CV",L"CW",L"CX",L"CY",L"CZ",
		L"DA",L"DB",L"DC",L"DD",L"DE",L"DF",L"DG",L"DH",
		L"DI",L"DJ",L"DK",L"DL",L"DM",L"DN",L"DO",L"DP",
		L"DQ",L"DR",L"DS",L"DT",L"DU",L"DV",L"DW",L"DX",
		L"DY",L"DZ",L"EA",L"EB",L"EC",L"ED",L"EE",L"EF",
		L"EG",L"EH",L"EI",L"EJ",L"EK",L"EL",L"EM",L"EN",
		L"EO",L"EP",L"EQ",L"ER",L"ES",L"ET",L"EU",L"EV",
		L"EW",L"EX",L"EY",L"EZ",L"FA",L"FB",L"FC",L"FD",
		L"FE",L"FF",L"FG",L"FH",L"FI",L"FJ",L"FK",L"FL",
		L"FM",L"FN",L"FO",L"FP",L"FQ",L"FR",L"FS",L"FT",
		L"FU",L"FV",L"FW",L"FX",L"FY",L"FZ",L"GA",L"GB",
		L"GC",L"GD",L"GE",L"GF",L"GG",L"GH",L"GI",L"GJ",
		L"GK",L"GL",L"GM",L"GN",L"GO",L"GP",L"GQ",L"GR",
		L"GS",L"GT",L"GU",L"GV",L"GW",L"GX",L"GY",L"GZ",
		L"HA",L"HB",L"HC",L"HD",L"HE",L"HF",L"HG",L"HH",
		L"HI",L"HJ",L"HK",L"HL",L"HM",L"HN",L"HO",L"HP",
		L"HQ",L"HR",L"HS",L"HT",L"HU",L"HV",L"HW",L"HX",
		L"HY",L"HZ",L"IA",L"IB",L"IC",L"ID",L"IE",L"IF",
		L"IG",L"IH",L"II",L"IJ",L"IK",L"IL",L"IM",L"IN",
		L"IO",L"IP",L"IQ",L"IR",L"IS",L"IT",L"IU",L"IV",
		// clang-format on
	};
	bool bOk = false;
	fmt::wmemory_buffer output;
	outCell.clear();
	if (inRow < GetMaxRows() && inCol < GetMaxCols())
	{
		fmt::format_to(std::back_inserter(output), L"{}{}", sc_ColumnNames[inCol], inRow + 1);
		bOk = true;
	}
	outCell = fmt::to_string(output);
	return bOk;
}
