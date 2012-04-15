/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Spreadsheet interface
 * @author David Connet
 *
 * Revision History
 * @li 2012-02-18 Added eSpreadSheetNumberNoZero
 * @li 2012-02-05 Added alignment and formatting.
 * @li 2012-02-04 Clear data in GetRowCol. Fix writing formulas.
 * @li 2012-01-14 DRC Change creation to only create one worksheet.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-07-24 DRC Removed option to export by array, completed wx port.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2004-09-30 DRC Created
 */

#include "stdafx.h"
#include "WizardExcel.h"

#include "ARBString.h"
#include "ARBTypes.h"
#include <sstream>

#if HAS_AUTOMATION
#include "AgilityBook.h"
#include "DlgProgress.h"
#include <wx/filename.h>
#include <wx/list.h>
#include <wx/variant.h>
#include <wx/msw/ole/automtn.h>
#endif //HAS_AUTOMATION

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

// General note about data and formulas - in both Excel and Calc writing
// individual cells auto-formats (formulas work) and writing of arrays
// acts like raw data - even if you call the put_Formula method.

/////////////////////////////////////////////////////////////////////////////

ISpreadSheetExporter::~ISpreadSheetExporter()
{
}


ISpreadSheetImporter::~ISpreadSheetImporter()
{
}


ISpreadSheet::~ISpreadSheet()
{
}

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

class CWizardExcel : public ISpreadSheet
{
protected:
	CWizardExcel();
public:
	static CWizardExcel* Create();
	virtual ~CWizardExcel();

	virtual ISpreadSheetExporterPtr GetExporter() const;
	virtual ISpreadSheetImporterPtr GetImporter() const;

private:
	mutable wxAutomationObject m_App;
};


class CWizardExcelExport : public ISpreadSheetExporter
{
protected:
	CWizardExcelExport(wxAutomationObject& ioApp);
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
	static CWizardExcelExport* Create(wxAutomationObject& ioApp);
	virtual ~CWizardExcelExport();

	virtual bool AllowAccess(bool bAllow);

	virtual bool SetTextColor(
			long inRow,
			long inCol,
			wxColour inColor);
	virtual bool SetBackColor(
			long inRow,
			long inCol,
			wxColour inColor);
	virtual bool SetItalic(
			long inRow,
			long inCol,
			bool bItalic);
	virtual bool SetBold(
			long inRow,
			long inCol,
			bool bBold);

	virtual bool SetAlignment(
			long inRow,
			long inCol,
			ISpreadSheetExporter::eAlign align);
	virtual bool SetFormat(
			long inRow,
			long inCol,
			ISpreadSheetExporter::eFormat format);
	virtual bool SetFormat(
			long inRow,
			long inCol,
			std::wstring const& format);

	virtual bool InsertData(
			long inRow,
			long inCol,
			double inData);
	virtual bool InsertData(
			long inRow,
			long inCol,
			std::wstring const& inData,
			bool bFormula = false);

	virtual bool AutoFit(
			long inColFrom,
			long inColTo);

private:
	wxAutomationObject& m_App;
	wxAutomationObject m_Worksheet;
};


class CWizardExcelImport : public CWizardBaseImport
{
protected:
	CWizardExcelImport(wxAutomationObject& ioApp);
public:
	static CWizardExcelImport* Create(wxAutomationObject& ioApp);
	virtual ~CWizardExcelImport();

	virtual bool OpenFile(std::wstring const& inFilename);
	virtual bool GetData(
			std::vector< std::vector<std::wstring> >& outData,
			IDlgProgress* ioProgress);

private:
	wxAutomationObject& m_App;
	wxAutomationObject m_Worksheet;
};

/////////////////////////////////////////////////////////////////////////////

class CWizardCalc : public ISpreadSheet
{
protected:
	CWizardCalc();
public:
	static CWizardCalc* Create();
	virtual ~CWizardCalc();

	virtual ISpreadSheetExporterPtr GetExporter() const;
	virtual ISpreadSheetImporterPtr GetImporter() const;

private:
	mutable wxAutomationObject m_Manager;
	mutable wxAutomationObject m_Desktop;
};


class CWizardCalcExport : public ISpreadSheetExporter
{
protected:
	CWizardCalcExport(
			wxAutomationObject& ioDesktop);
public:
	static CWizardCalcExport* Create(
			wxAutomationObject& ioDesktop);
	virtual ~CWizardCalcExport();

	virtual bool AllowAccess(bool bAllow);

	virtual bool SetTextColor(
			long inRow,
			long inCol,
			wxColour inColor);
	virtual bool SetBackColor(
			long inRow,
			long inCol,
			wxColour inColor);
	virtual bool SetItalic(
			long inRow,
			long inCol,
			bool bItalic);
	virtual bool SetBold(
			long inRow,
			long inCol,
			bool bBold);

	virtual bool SetAlignment(
			long inRow,
			long inCol,
			ISpreadSheetExporter::eAlign align);
	virtual bool SetFormat(
			long inRow,
			long inCol,
			ISpreadSheetExporter::eFormat format);
	virtual bool SetFormat(
			long inRow,
			long inCol,
			std::wstring const& format);

	virtual bool InsertData(
			long inRow,
			long inCol,
			double inData);
	virtual bool InsertData(
			long inRow,
			long inCol,
			std::wstring const& inData,
			bool bFormula = false);

	virtual bool AutoFit(
			long inColFrom,
			long inColTo);

private:
	bool CreateWorksheet();

	wxAutomationObject& m_Desktop;
	wxAutomationObject m_Document;
	wxAutomationObject m_Worksheet;
};


class CWizardCalcImport : public CWizardBaseImport
{
protected:
	CWizardCalcImport(
			wxAutomationObject& ioDesktop);
public:
	static CWizardCalcImport* Create(
			wxAutomationObject& ioDesktop);
	virtual ~CWizardCalcImport();

	virtual bool OpenFile(std::wstring const& inFilename);
	virtual bool GetData(
			std::vector< std::vector<std::wstring> >& outData,
			IDlgProgress* ioProgress);

private:
	wxAutomationObject& m_Desktop;
	wxAutomationObject m_Document;
	wxAutomationObject m_Worksheet;
};

/////////////////////////////////////////////////////////////////////////////

CWizardExcel* CWizardExcel::Create()
{
	wxBusyCursor wait;
	CWizardExcel* pExcel = new CWizardExcel();
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
		{
			delete pExcel;
			pExcel = NULL;
		}
	}
	return pExcel;
}


CWizardExcel::CWizardExcel()
{
	wxLogNull logSuppressor;
	if (!m_App.GetInstance(wxT("Excel.Application")))
	{
		if (!m_App.CreateInstance(wxT("Excel.Application")))
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

CWizardExcelExport* CWizardExcelExport::Create(wxAutomationObject& ioApp)
{
	if (ioApp.GetDispatchPtr())
		return new CWizardExcelExport(ioApp);
	return NULL;
}


CWizardExcelExport::CWizardExcelExport(wxAutomationObject& ioApp)
	: m_App(ioApp)
	, m_Worksheet()
{
	// Create a new workbook.
	wxAutomationObject book = m_App.CallMethod(wxT("Workbooks.Add"), xlWBATWorksheet);
	// Get the first sheet.
	wxAutomationObject sheets = book.GetProperty(wxT("Sheets"));
	wxVariant args[1];
	args[0] = wxVariant((short)1);
	sheets.GetObject(m_Worksheet, wxT("Item"), 1, args);

	// wx doesn't support IDispatch as a variant
	//sheets.CallMethod(wxT("Add"), wxNullVariant, m_Worksheet, wxNullVariant, xlWorksheet);
	// This also doesn't work
	//sheets.CallMethod(wxT("Add"), wxNullVariant, wxNullVariant, wxNullVariant, xlWorksheet);
	// The following python works:
	//import win32com.client
	//app = win32com.client.Dispatch('Excel.Application')
	//app.Visible = True
	//book = app.Workbooks.Add(-4167)
	//sheet = book.Sheets(1)
	//sheet.Name = 'Test'
	//sheet2 = book.Sheets.Add(None, sheet, None, -4167)
	//sheetFirst = book.Sheets.Add(None, None, None, -4167)
}


CWizardExcelExport::~CWizardExcelExport()
{
	if (NULL != m_Worksheet.GetDispatchPtr() && !m_App.GetProperty(wxT("Visible")).GetBool())
		m_App.CallMethod(wxT("Quit"));
}


bool CWizardExcelExport::AllowAccess(bool bAllow)
{
	if (bAllow)
	{
		m_App.PutProperty(wxT("UserControl"), true);
		m_App.PutProperty(wxT("Visible"), true);
	}
	else
		m_App.PutProperty(wxT("UserControl"), false);
	return true;
}


bool CWizardExcelExport::SetTextColor(
		long inRow,
		long inCol,
		wxColour inColor)
{
	std::wstring cell1;
	if (!ISpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, wxT("Range"), 2, args);
	wxAutomationObject font;
	range.GetObject(font, wxT("Font"));
	font.PutProperty(wxT("Color"), (long)inColor.GetPixel());
	return true;
}


bool CWizardExcelExport::SetBackColor(
		long inRow,
		long inCol,
		wxColour inColor)
{
	std::wstring cell1;
	if (!ISpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, wxT("Range"), 2, args);
	wxAutomationObject interior;
	range.GetObject(interior, wxT("Interior"));
	interior.PutProperty(wxT("Color"), (long)inColor.GetPixel());
	return true;
}


bool CWizardExcelExport::SetItalic(
		long inRow,
		long inCol,
		bool bItalic)
{
	std::wstring cell1;
	if (!ISpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, wxT("Range"), 2, args);
	wxAutomationObject font;
	range.GetObject(font, wxT("Font"));
	font.PutProperty(wxT("Italic"), static_cast<short>(bItalic));
	return true;
}


bool CWizardExcelExport::SetBold(
		long inRow,
		long inCol,
		bool bBold)
{
	std::wstring cell1;
	if (!ISpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, wxT("Range"), 2, args);
	wxAutomationObject font;
	range.GetObject(font, wxT("Font"));
	font.PutProperty(wxT("Bold"), static_cast<short>(bBold));
	return true;
}


bool CWizardExcelExport::SetAlignment(
		long inRow,
		long inCol,
		ISpreadSheetExporter::eAlign align)
{
	if (ISpreadSheetExporter::eSpreadSheetNone == align)
		return true;
	std::wstring cell1;
	if (!ISpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, wxT("Range"), 2, args);
	short val = xlHAlignGeneral;
	switch (align)
	{
	default:
	case ISpreadSheetExporter::eSpreadSheetGeneral:
		val = xlHAlignGeneral;
		break;
	case ISpreadSheetExporter::eSpreadSheetLeft:
		val = xlHAlignLeft;
		break;
	case ISpreadSheetExporter::eSpreadSheetRight:
		val = xlHAlignRight;
		break;
	case ISpreadSheetExporter::eSpreadSheetCenter:
		val = xlHAlignCenter;
		break;
	}
	range.PutProperty(wxT("HorizontalAlignment"), val);
	return true;
}


bool CWizardExcelExport::SetFormat(
		long inRow,
		long inCol,
		ISpreadSheetExporter::eFormat format)
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
	default:
	case ISpreadSheetExporter::eSpreadSheetText:
		sFormat = L"@";
		break;
	case ISpreadSheetExporter::eSpreadSheetCurrency:
		sFormat = L"_($* #,##0.00_);[Red]_($* (#,##0.00);_(* \"-\"??_);_(@_)";
		break;
	case ISpreadSheetExporter::eSpreadSheetNumber:
		sFormat = L"#,##0_);[Red](#,##0)";
		break;
	case ISpreadSheetExporter::eSpreadSheetNumberNoZero:
		sFormat = L"#,##0_);[Red](#,##0);_(* \"\"??_);_(@_)";
		break;
	case ISpreadSheetExporter::eSpreadSheetDate:
		sFormat = L"d/m/yyyy";
		break;
	}
	return SetFormat(inRow, inCol, sFormat);
}


bool CWizardExcelExport::SetFormat(
		long inRow,
		long inCol,
		std::wstring const& format)
{
	std::wstring cell1;
	if (!ISpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, wxT("Range"), 2, args);
	range.PutProperty(wxT("NumberFormat"), format.c_str());
	return true;
}


bool CWizardExcelExport::InsertData(
		long inRow,
		long inCol,
		double inData)
{
	std::wstring cell1;
	if (!ISpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, wxT("Range"), 2, args);
	range.PutProperty(wxT("Value2"), inData);
	return true;
}


bool CWizardExcelExport::InsertData(
		long inRow,
		long inCol,
		std::wstring const& inData,
		bool bFormula)
{
	std::wstring cell1;
	if (!ISpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, wxT("Range"), 2, args);
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


bool CWizardExcelExport::AutoFit(
		long inColFrom,
		long inColTo)
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
	m_Worksheet.GetObject(range, wxT("Range"), 2, args);
	wxAutomationObject cols;
	range.GetObject(cols, wxT("EntireColumn"));
	cols.CallMethod(wxT("AutoFit"));
	return true;
}

/////////////////////////////////////////////////////////////////////////////

CWizardExcelImport* CWizardExcelImport::Create(wxAutomationObject& ioApp)
{
	if (ioApp.GetDispatchPtr())
		return new CWizardExcelImport(ioApp);
	return NULL;
}

CWizardExcelImport::CWizardExcelImport(wxAutomationObject& ioApp)
	: m_App(ioApp)
{
}


CWizardExcelImport::~CWizardExcelImport()
{
	if (NULL != m_Worksheet.GetDispatchPtr())
		m_App.CallMethod(wxT("Quit"));
}


bool CWizardExcelImport::OpenFile(std::wstring const& inFilename)
{
	wxVariant bk = m_App.CallMethod(wxT("Workbooks.Open"), inFilename.c_str());
	wxAutomationObject book((WXIDISPATCH*)bk.GetVoidPtr());
	if (NULL == book.GetDispatchPtr())
		return false;
	m_FileName = inFilename;
	// Get the first sheet.
	wxAutomationObject sheets = book.GetProperty(wxT("Sheets"));
	wxVariant args2[1];
	args2[0] = wxVariant((short)1);
	sheets.GetObject(m_Worksheet, wxT("Item"), 1, args2);
	return NULL != m_Worksheet.GetDispatchPtr();
}


bool CWizardExcelImport::GetData(
		std::vector< std::vector<std::wstring> >& outData,
		IDlgProgress* ioProgress)
{
	outData.clear();
	if (NULL == m_Worksheet.GetDispatchPtr())
		return false;
	wxAutomationObject range;
	m_Worksheet.GetObject(range, wxT("UsedRange"));
	long iRow = range.GetProperty(wxT("Row")).GetLong();
	long iCol = range.GetProperty(wxT("Column")).GetLong();
	wxAutomationObject rangeRows, rangeColumns;
	range.GetObject(rangeRows, wxT("Rows"));
	range.GetObject(rangeColumns, wxT("Columns"));
	long nRows = rangeRows.GetProperty(wxT("Count")).GetLong();
	long nCols = rangeColumns.GetProperty(wxT("Count")).GetLong();
	if (0 < nRows && 0 < nCols)
	{
		if (ioProgress)
		{
			wxFileName filename(m_FileName);
			std::wstring msg = filename.GetFullPath();
			ioProgress->SetCaption(msg);
			std::wstring str = wxString::Format(_("IDS_READING_ROWSCOLS"),
				static_cast<int>(nRows),
				static_cast<int>(nCols));
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
				range.GetObject(range2, wxT("Range"), 2, args);
				row.push_back(StringUtil::stringW(range2.GetProperty(wxT("Value")).GetString()));
			}
			outData.push_back(row);
		}
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////

CWizardCalc* CWizardCalc::Create()
{
	wxBusyCursor wait;
	CWizardCalc* pCalc = new CWizardCalc();
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
		{
			delete pCalc;
			pCalc = NULL;
		}
	}
	return pCalc;
}


CWizardCalc::CWizardCalc()
{
	wxLogNull logSuppressor;
	if (!m_Manager.GetInstance(wxT("com.sun.star.ServiceManager")))
	{
		if (!m_Manager.CreateInstance(wxT("com.sun.star.ServiceManager")))
			return;
	}
	m_Desktop.SetDispatchPtr(m_Manager.CallMethod(wxT("createInstance"), wxT("com.sun.star.frame.Desktop")));
}


CWizardCalc::~CWizardCalc()
{
}


ISpreadSheetExporterPtr CWizardCalc::GetExporter() const
{
	return ISpreadSheetExporterPtr(CWizardCalcExport::Create(m_Desktop));
}


ISpreadSheetImporterPtr CWizardCalc::GetImporter() const
{
	return ISpreadSheetImporterPtr(CWizardCalcImport::Create(m_Desktop));
}

/////////////////////////////////////////////////////////////////////////////

CWizardCalcExport* CWizardCalcExport::Create(
		wxAutomationObject& ioDesktop)
{
	return new CWizardCalcExport(ioDesktop);
}


CWizardCalcExport::CWizardCalcExport(
		wxAutomationObject& ioDesktop)
	: m_Desktop(ioDesktop)
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
		wxVariant retval = m_Desktop.CallMethod(wxT("loadComponentFromURL"), wxT("private:factory/scalc"), wxT("_blank"), 0, args);
		if (!retval.IsNull())
		{
			m_Document.SetDispatchPtr(retval);
		}
	}
	if (m_Document.GetDispatchPtr() && !m_Worksheet.GetDispatchPtr())
	{
		wxAutomationObject sheets = m_Document.CallMethod(wxT("getSheets"));
		m_Worksheet.SetDispatchPtr(sheets.CallMethod(wxT("getByIndex"), 0));
	}
	return m_Worksheet.GetDispatchPtr() != NULL;
}


bool CWizardCalcExport::AllowAccess(bool bAllow)
{
	// Calc doesn't seem to support controlling user access like Excel does
	return false;
}


bool CWizardCalcExport::SetTextColor(
		long inRow,
		long inCol,
		wxColour inColor)
{
	wxAutomationObject cell(m_Worksheet.CallMethod(wxT("getCellByPosition"), inCol, inRow));
	if (!cell.GetDispatchPtr())
		return false;
	return cell.PutProperty(wxT("CharColor"), static_cast<long>(inColor.GetPixel()));
}


bool CWizardCalcExport::SetBackColor(
		long inRow,
		long inCol,
		wxColour inColor)
{
	wxAutomationObject cell(m_Worksheet.CallMethod(wxT("getCellByPosition"), inCol, inRow));
	if (!cell.GetDispatchPtr())
		return false; 
	return cell.PutProperty(wxT("CellBackColor"), static_cast<long>(inColor.GetPixel()));
}


bool CWizardCalcExport::SetItalic(
		long inRow,
		long inCol,
		bool bItalic)
{
	wxAutomationObject cell(m_Worksheet.CallMethod(wxT("getCellByPosition"), inCol, inRow));
	if (!cell.GetDispatchPtr())
		return false;
	return cell.PutProperty(wxT("CharPosture"), bItalic ? 1L : 0L);
}


bool CWizardCalcExport::SetBold(
		long inRow,
		long inCol,
		bool bBold)
{
	wxAutomationObject cell(m_Worksheet.CallMethod(wxT("getCellByPosition"), inCol, inRow));
	if (!cell.GetDispatchPtr())
		return false;
	return cell.PutProperty(wxT("CharWeight"), bBold ? 150L : 100L);
}


bool CWizardCalcExport::SetAlignment(
		long inRow,
		long inCol,
		ISpreadSheetExporter::eAlign align)
{
#pragma PRAGMA_TODO("Enable alignment in Calc")
	return false;
}


bool CWizardCalcExport::SetFormat(
		long inRow,
		long inCol,
		ISpreadSheetExporter::eFormat format)
{
#pragma PRAGMA_TODO("Are the formatting strings the same as excel?")
	std::wstring sFormat;
	switch (format)
	{
	default:
	case ISpreadSheetExporter::eSpreadSheetText:
		sFormat = L"@";
		break;
	case ISpreadSheetExporter::eSpreadSheetCurrency:
		sFormat = L"_($* #,##0.00_);[Red]_($* (#,##0.00);_(* \"-\"??_);_(@_)";
		break;
	case ISpreadSheetExporter::eSpreadSheetNumber:
		sFormat = L"#,##0.00;[Red](#,##0.00)";
		break;
	case ISpreadSheetExporter::eSpreadSheetDate:
		sFormat = L"d/m/yyyy";
		break;
	}
	return SetFormat(inRow, inCol, sFormat);
}


bool CWizardCalcExport::SetFormat(
		long inRow,
		long inCol,
		std::wstring const& format)
{
#pragma PRAGMA_TODO("Enable formatting in Calc")
	return false;
}


bool CWizardCalcExport::InsertData(
		long inRow,
		long inCol,
		double inData)
{
	if (!CreateWorksheet())
		return false;
	wxAutomationObject cell(m_Worksheet.CallMethod(wxT("getCellByPosition"), inCol, inRow));
	if (!cell.GetDispatchPtr())
		return false;
	return cell.PutProperty(wxT("Value"), inData);
}


bool CWizardCalcExport::InsertData(
		long inRow,
		long inCol,
		std::wstring const& inData,
		bool bFormula)
{
	if (!CreateWorksheet())
		return false;
	wxAutomationObject cell(m_Worksheet.CallMethod(wxT("getCellByPosition"), inCol, inRow));
	if (!cell.GetDispatchPtr())
		return false;
	std::wstring property = bFormula ? wxT("Formula") : wxT("String");
	return cell.PutProperty(property, inData.c_str());
}


bool CWizardCalcExport::AutoFit(
		long inColFrom,
		long inColTo)
{
	if (!CreateWorksheet())
		return false;
	wxAutomationObject range(m_Worksheet.CallMethod(wxT("getCellRangeByPosition"), inColFrom, 0, inColTo, 0));
	if (!range.GetDispatchPtr())
		return false;
	range.PutProperty(wxT("Columns.OptimalWidth"), true);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

CWizardCalcImport* CWizardCalcImport::Create(
		wxAutomationObject& ioDesktop)
{
	return new CWizardCalcImport(ioDesktop);
}


CWizardCalcImport::CWizardCalcImport(
		wxAutomationObject& ioDesktop)
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
		wxVariant file = m_Desktop.CallMethod(wxT("loadComponentFromURL"), fileName.c_str(), wxT("_blank"), 0, args);
		if (file.IsNull())
			return false;
		m_Document.SetDispatchPtr(file);
	}
	if (!m_Worksheet.GetDispatchPtr())
	{
		wxAutomationObject sheets = m_Document.CallMethod(wxT("getSheets"));
		m_Worksheet.SetDispatchPtr(sheets.CallMethod(wxT("getByIndex"), 0));
	}
	if (!m_Worksheet.GetDispatchPtr())
		return false;
	m_FileName = inFilename;
	return true;
}


bool CWizardCalcImport::GetData(
		std::vector< std::vector<std::wstring> >& outData,
		IDlgProgress* ioProgress)
{
	outData.clear();
	if (!m_Worksheet.GetDispatchPtr())
		return false;

	wxAutomationObject cursor = m_Worksheet.CallMethod(wxT("createCursor"));
	if (!cursor.GetDispatchPtr())
		return false;

	// Set up the data area
	// Ignore failure codes
	// - these are returning failure, but appear to be working correctly.
	cursor.CallMethod(wxT("gotoStartOfUsedArea"), 0);
	cursor.CallMethod(wxT("gotoEndOfUsedArea"), 1);

	// This should be a list of lists.
	wxVariant dataAll = cursor.CallMethod(wxT("getDataArray"));
	if (!dataAll.IsType(wxT("list")))
		return false;
	bool bAbort = false;
	int nRows = static_cast<int>(dataAll.GetCount());
	outData.reserve(nRows);
	if (ioProgress)
	{
		wxFileName filename(m_FileName);
		std::wstring msg = filename.GetFullPath();
		ioProgress->SetCaption(msg);
		std::wstring str = wxString::Format(_("IDS_READING_ROWS"), nRows);
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
		if (!vRow.IsType(wxT("list")))
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

#endif //HAS_AUTOMATION

/////////////////////////////////////////////////////////////////////////////

ISpreadSheetPtr ISpreadSheet::Create(eType inType)
{
	ISpreadSheetPtr pInterface;
#if HAS_AUTOMATION
	switch (inType)
	{
	default:
		break;
	case eMicrosoftExcel:
		pInterface = ISpreadSheetPtr(CWizardExcel::Create());
		break;
	case eOpenOfficeCalc:
		pInterface = ISpreadSheetPtr(CWizardCalc::Create());
		break;
	}
#endif
	return pInterface;
}


// static helper functions
long ISpreadSheet::GetMaxRows()
{
	return 65536; // Excel limits (I believe later versions of excel/calc may expand this, but we support older versions too)
}


long ISpreadSheet::GetMaxCols()
{
	return 256; // Excel limits
}


bool ISpreadSheet::GetRowCol(
		long inRow,
		long inCol,
		std::wstring& outCell)
{
	// Lookup tables are nice!
	static const wchar_t* const sc_ColumnNames[256] =
	{
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
	};
	bool bOk = false;
	std::wostringstream output;
	outCell.clear();
	if (inRow < GetMaxRows() && inCol < GetMaxCols())
	{
		output << sc_ColumnNames[inCol] << inRow + 1;
		bOk = true;
	}
	outCell = output.str();
	return bOk;
}
