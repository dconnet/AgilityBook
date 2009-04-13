/*
 * Copyright © 2004-2009 David Connet. All Rights Reserved.
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
 * @brief Import/Export Wizard for Excel
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2004-09-30 DRC Created
 */

#include "stdafx.h"
#include "WizardExcel.h"

#pragma message PRAGMA_MESSAGE("TODO: Implement CWizardExcel")
//#if defined(WIN32) && defined(wxUSE_OLE_AUTOMATION)
//#define HAS_AUTOMATION	1
//#else
#define HAS_AUTOMATION	0
//#endif

#include "ARBTypes.h"
#if HAS_AUTOMATION
#include "AgilityBook.h"
#include "DlgProgress.h"
#include <wx/variant.h>
#include <wx/msw/ole/automtn.h>

// Note: Can't use these headers as they use MFC
//#include "excel8.h"
//#if _MSC_VER < 1300
//#define CComVariant COleVariant
//#else
//#include "ooCalc.h"
//#endif
#endif //HAS_AUTOMATION

// General note about data and formulas - in both Excel and Calc writing
// individual cells auto-formats (formulas work) and writing of arrays
// acts like raw data - even if you call the put_Formula method.

/////////////////////////////////////////////////////////////////////////////

IWizardExporter::~IWizardExporter()
{
}


IWizardImporter::~IWizardImporter()
{
}


IWizardSpreadSheet::~IWizardSpreadSheet()
{
}

/////////////////////////////////////////////////////////////////////////////

#if HAS_AUTOMATION
class CWizardBaseExport : public IWizardExporter
{
protected:
	CWizardBaseExport(bool bInvert)
		: m_bInvert(bInvert)
		, m_Rows(0)
		, m_Cols(0)
		, covTrue(static_cast<short>(TRUE))
		, covOptional(static_cast<long>(DISP_E_PARAMNOTFOUND), VT_ERROR)
	{
	}

public:
	// Safe array is ready for use
	virtual bool ArrayOkay() const
	{
		return 0 < m_Rows && 0 < m_Cols;
	}
	// Create the safe array.
	virtual bool CreateArray(
			long inRows,
			long inCols);
	// Insert data into the safe array.
	virtual bool InsertArrayData(
			long inRow,
			long inCol,
			CString const& inData);

protected:
	bool m_bInvert; // OOCalc needs the array inverted in order to insert it
	long m_Rows;
	long m_Cols;
	COleSafeArray m_Array;
	// Commonly used OLE variants.
	CComVariant covTrue;
	CComVariant covOptional;
};


bool CWizardBaseExport::CreateArray(
		long inRows,
		long inCols)
{
	if (ArrayOkay())
		return false;
	if (0 >= inRows || 0 >= inCols)
		return false;
	if (inRows >= IWizardSpreadSheet::GetMaxRows())
		inRows = IWizardSpreadSheet::GetMaxRows() - 1;
	if (inCols >= IWizardSpreadSheet::GetMaxCols())
		inCols = IWizardSpreadSheet::GetMaxCols() - 1;
	DWORD numElements[2];
	int nRow = 0;
	int nCol = 1;
	if (m_bInvert)
	{
		nRow = 1;
		nCol = 0;
	}
	numElements[nRow] = m_Rows = inRows;
	numElements[nCol] = m_Cols = inCols;
	m_Array.Create(VT_BSTR, 2, numElements);
	return true;
}


bool CWizardBaseExport::InsertArrayData(
		long inRow,
		long inCol,
		CString const& inData)
{
	if (!ArrayOkay())
		return false;
	if (0 > inRow || inRow >= m_Rows
	|| 0 > inCol || inCol >= m_Cols)
		return false;
	long index[2];
	int nRow = 0;
	int nCol = 1;
	if (m_bInvert)
	{
		nRow = 1;
		nCol = 0;
	}
	index[nRow] = inRow;
	index[nCol] = inCol;
	CComVariant v(inData);
	m_Array.PutElement(index, v.bstrVal);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class CWizardBaseImport : public IWizardImporter
{
public:
	CWizardBaseImport()
		: covTrue(static_cast<short>(TRUE))
		, covOptional(static_cast<long>(DISP_E_PARAMNOTFOUND), VT_ERROR)
	{
	}

protected:
	CString m_FileName;
	// Commonly used OLE variants.
	CComVariant covTrue;
	CComVariant covOptional;
};

/////////////////////////////////////////////////////////////////////////////

class CWizardExcel : public IWizardSpreadSheet
{
protected:
	CWizardExcel();
public:
	static CWizardExcel* Create();
	virtual ~CWizardExcel();

	virtual IWizardExporterPtr GetExporter() const;
	virtual IWizardImporterPtr GetImporter() const;

private:
	bool m_Ok;
	mutable wxAutomationObject m_App; //Excel8::_Application
};


class CWizardExcelExport : public CWizardBaseExport
{
protected:
	CWizardExcelExport(Excel8::_Application& ioApp);
public:
	static CWizardExcelExport* Create(Excel8::_Application& ioApp);
	virtual ~CWizardExcelExport();

	// Copy the safe array to excel (safe array is reset)
	virtual bool ExportDataArray(
			long inRowTop = 0,
			long inColLeft = 0);

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

	virtual bool InsertData(
			long inRow,
			long inCol,
			double inData);
	virtual bool InsertData(
			long inRow,
			long inCol,
			CString const& inData);

	virtual bool AutoFit(
			long inColFrom,
			long inColTo);

private:
	Excel8::_Application& m_App;
	Excel8::_Worksheet m_Worksheet;
};


class CWizardExcelImport : public CWizardBaseImport
{
protected:
	CWizardExcelImport(Excel8::_Application& ioApp);
public:
	static CWizardExcelImport* Create(Excel8::_Application& ioApp);
	virtual ~CWizardExcelImport();

	virtual bool OpenFile(CString const& inFilename);
	virtual bool GetData(
			std::vector< std::vector<CString> >& outData,
			IDlgProgress* ioProgress);

private:
	Excel8::_Application& m_App;
	Excel8::_Worksheet m_Worksheet;
};

/////////////////////////////////////////////////////////////////////////////

#if _MSC_VER < 1300

class CWizardCalc : public IWizardSpreadSheet
{
public:
	static CWizardCalc* Create();
};


#else

class CWizardCalc : public IWizardSpreadSheet
{
protected:
	CWizardCalc();
public:
	static CWizardCalc* Create();
	virtual ~CWizardCalc();

	virtual IWizardExporterPtr GetExporter() const;
	virtual IWizardImporterPtr GetImporter() const;

private:
	mutable ooCalc::CComManagerDriver m_Manager;
};


class CWizardCalcExport : public CWizardBaseExport
{
protected:
	CWizardCalcExport(
			ooCalc::CComManagerDriver& ioManager);
public:
	static CWizardCalcExport* Create(
			ooCalc::CComManagerDriver& ioManager);
	virtual ~CWizardCalcExport();

	// Copy the safe array to excel (safe array is reset)
	virtual bool ExportDataArray(
			long inRowTop = 0,
			long inColLeft = 0);

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

	virtual bool InsertData(
			long inRow,
			long inCol,
			double inData);
	virtual bool InsertData(
			long inRow,
			long inCol,
			CString const& inData);

	virtual bool AutoFit(
			long inColFrom,
			long inColTo);

private:
	ooCalc::ooXSpreadsheet GetWorksheet();

	ooCalc::CComManagerDriver& m_Manager;
	ooCalc::ooXSpreadsheetDocument m_Document;
	ooCalc::ooXSpreadsheet m_Worksheet;
};


class CWizardCalcImport : public CWizardBaseImport
{
protected:
	CWizardCalcImport(
			ooCalc::CComManagerDriver& ioManager);
public:
	static CWizardCalcImport* Create(
			ooCalc::CComManagerDriver& ioManager);
	virtual ~CWizardCalcImport();

	virtual bool OpenFile(CString const& inFilename);
	virtual bool GetData(
			std::vector< std::vector<CString> >& outData,
			IDlgProgress* ioProgress);

private:
	ooCalc::CComManagerDriver& m_Manager;
	ooCalc::ooXSpreadsheetDocument m_Document;
	ooCalc::ooXSpreadsheet m_Worksheet;
};

#endif

/////////////////////////////////////////////////////////////////////////////

CWizardExcel* CWizardExcel::Create()
{
	CWaitCursor wait;
	CWizardExcel* pExcel = new CWizardExcel();
	if (pExcel)
	{
		bool bKill = false;
		if (NULL == pExcel->m_App.m_lpDispatch)
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
	: m_Ok(false)
{
	if (!m_App.GetInstance(wxT("Excel.Application")))
	{
		if (!m_App.CreateInstance(wxT("Excel.Application")))
			return;
	}
}


CWizardExcel::~CWizardExcel()
{
}


IWizardExporterPtr CWizardExcel::GetExporter() const
{
	return IWizardExporterPtr(CWizardExcelExport::Create(m_App));
}


IWizardImporterPtr CWizardExcel::GetImporter() const
{
	return IWizardImporterPtr(CWizardExcelImport::Create(m_App));
}

/////////////////////////////////////////////////////////////////////////////

CWizardExcelExport* CWizardExcelExport::Create(Excel8::_Application& ioApp)
{
	return new CWizardExcelExport(ioApp);
}


CWizardExcelExport::CWizardExcelExport(Excel8::_Application& ioApp)
	: CWizardBaseExport(false)
	, m_App(ioApp)
{
	try
	{
		// Create a new workbook.
		Excel8::Workbooks books = m_App.get_Workbooks();
		Excel8::_Workbook book = books.Add(covOptional);
		// Get the first sheet.
		Excel8::Worksheets sheets = book.get_Sheets();
		m_Worksheet = sheets.get_Item(CComVariant((short)1));
	}
	catch (COleDispatchException* ex)
	{
		ex->Delete();
	}
}


CWizardExcelExport::~CWizardExcelExport()
{
	try
	{
		if (NULL != m_Worksheet.m_lpDispatch && !m_App.get_Visible())
			m_App.Quit();
	}
	catch (COleDispatchException* ex)
	{
		ex->Delete();
	}
}


bool CWizardExcelExport::ExportDataArray(
		long inRowTop,
		long inColLeft)
{
	if (!ArrayOkay())
		return false;

	CString cell1, cell2;
	if (!IWizardSpreadSheet::GetRowCol(inRowTop, inColLeft, cell1))
		return false;
	if (!IWizardSpreadSheet::GetRowCol(inRowTop + m_Rows - 1, inColLeft + m_Cols - 1, cell2))
		return false;

	try
	{
		Excel8::Range range = m_Worksheet.get_Range(CComVariant(cell1), CComVariant(cell2));
		range.put_Value2(CComVariant(m_Array));
		m_Array.Detach();

		IWizardSpreadSheet::GetRowCol(inRowTop, inColLeft + m_Cols - 1, cell2);
		range = m_Worksheet.get_Range(CComVariant(cell1), CComVariant(cell2));
		Excel8::Range cols = range.get_EntireColumn();
		cols.AutoFit();

		m_Rows = m_Cols = 0;

		return true;
	}
	catch (COleDispatchException* ex)
	{
		ex->Delete();
	}
	return false;
}


bool CWizardExcelExport::AllowAccess(bool bAllow)
{
	try
	{
		if (bAllow)
		{
			m_App.put_UserControl(TRUE);
			m_App.put_Visible(TRUE);
		}
		else
			m_App.put_UserControl(FALSE);
		return true;
	}
	catch (COleDispatchException* ex)
	{
		ex->Delete();
	}
	return false;
}


bool CWizardExcelExport::SetTextColor(
		long inRow,
		long inCol,
		wxColour inColor)
{
	CString cell1;
	if (!IWizardSpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	try
	{
		Excel8::Range range = m_Worksheet.get_Range(CComVariant(cell1), CComVariant(cell1));
		Excel8::Font font = range.get_Font();
		font.put_Color(COleVariant((LONG)inColor));
		return true;
	}
	catch (COleDispatchException* ex)
	{
		ex->Delete();
	}
	return false;
}


bool CWizardExcelExport::SetBackColor(
		long inRow,
		long inCol,
		wxColour inColor)
{
	CString cell1;
	if (!IWizardSpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	try
	{
		Excel8::Range range = m_Worksheet.get_Range(CComVariant(cell1), CComVariant(cell1));
		Excel8::Interior interior = range.get_Interior();
		interior.put_Color(COleVariant((long)inColor));
		return true;
	}
	catch (COleDispatchException* ex)
	{
		ex->Delete();
	}
	return false;
}


bool CWizardExcelExport::SetItalic(
		long inRow,
		long inCol,
		bool bItalic)
{
	CString cell1;
	if (!IWizardSpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	try
	{
		Excel8::Range range = m_Worksheet.get_Range(CComVariant(cell1), CComVariant(cell1));
		Excel8::Font font = range.get_Font();
		font.put_Italic(COleVariant(static_cast<short>(bItalic)));
		return true;
	}
	catch (COleDispatchException* ex)
	{
		ex->Delete();
	}
	return false;
}


bool CWizardExcelExport::SetBold(
		long inRow,
		long inCol,
		bool bBold)
{
	CString cell1;
	if (!IWizardSpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	try
	{
		Excel8::Range range = m_Worksheet.get_Range(CComVariant(cell1), CComVariant(cell1));
		Excel8::Font font = range.get_Font();
		font.put_Bold(COleVariant(static_cast<short>(bBold)));
		return true;
	}
	catch (COleDispatchException* ex)
	{
		ex->Delete();
	}
	return false;
}


bool CWizardExcelExport::InsertData(
		long inRow,
		long inCol,
		double inData)
{
	CString cell1;
	if (!IWizardSpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	try
	{
		Excel8::Range range = m_Worksheet.get_Range(CComVariant(cell1), CComVariant(cell1));
		range.put_Value2(CComVariant(inData));
		return true;
	}
	catch (COleDispatchException* ex)
	{
		ex->Delete();
	}
	return false;
}


bool CWizardExcelExport::InsertData(
		long inRow,
		long inCol,
		CString const& inData)
{
	CString cell1;
	if (!IWizardSpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	try
	{
		Excel8::Range range = m_Worksheet.get_Range(CComVariant(cell1), CComVariant(cell1));
		if (0 < inData.GetLength() && '=' == inData[0])
		{
			CString data = CString('\'') + inData;
			range.put_Value2(CComVariant(data));
		}
		else
			range.put_Value2(CComVariant(inData));
		return true;
	}
	catch (COleDispatchException* ex)
	{
		ex->Delete();
	}
	return false;
}


bool CWizardExcelExport::AutoFit(
		long inColFrom,
		long inColTo)
{
	CString cell1, cell2;
	if (!IWizardSpreadSheet::GetRowCol(0, inColFrom, cell1))
		return false;
	if (!IWizardSpreadSheet::GetRowCol(0, inColTo, cell2))
		return false;
	try
	{
		Excel8::Range range = m_Worksheet.get_Range(CComVariant(cell1), CComVariant(cell2));
		Excel8::Range cols = range.get_EntireColumn();
		cols.AutoFit();
		return true;
	}
	catch (COleDispatchException* ex)
	{
		ex->Delete();
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////

CWizardExcelImport* CWizardExcelImport::Create(Excel8::_Application& ioApp)
{
	return new CWizardExcelImport(ioApp);
}


CWizardExcelImport::CWizardExcelImport(Excel8::_Application& ioApp)
	: m_App(ioApp)
{
}


CWizardExcelImport::~CWizardExcelImport()
{
	try
	{
		if (NULL != m_Worksheet.m_lpDispatch)
			m_App.Quit();
	}
	catch (COleDispatchException* ex)
	{
		ex->Delete();
	}
}


bool CWizardExcelImport::OpenFile(CString const& inFilename)
{
	try
	{
		// Create a new workbook.
		Excel8::Workbooks books = m_App.get_Workbooks();
		Excel8::_Workbook book = books.Open(inFilename, covOptional, covTrue, covOptional,
			covOptional, covOptional, covOptional, covOptional,
			covOptional, covOptional, covOptional, covOptional,
			covOptional);
		if (NULL == book.m_lpDispatch)
			return false;
		m_FileName = inFilename;
		// Get the first sheet.
		Excel8::Worksheets sheets = book.get_Sheets();
		m_Worksheet = sheets.get_Item(CComVariant((short)1));
	}
	catch (COleDispatchException* ex)
	{
		ex->Delete();
	}
	return NULL != m_Worksheet.m_lpDispatch;
}


bool CWizardExcelImport::GetData(
		std::vector< std::vector<CString> >& outData,
		IDlgProgress* ioProgress)
{
	outData.clear();
	if (NULL == m_Worksheet.m_lpDispatch)
		return false;
	try
	{
		Excel8::Range range = m_Worksheet.get_UsedRange();
		long iRow = range.get_Row();
		long iCol = range.get_Column();
		long nRows = Excel8::Range(range.get_Rows()).get_Count();
		long nCols = Excel8::Range(range.get_Columns()).get_Count();
		if (0 < nRows && 0 < nCols)
		{
			if (ioProgress)
			{
				CString msg;
				int nPath = m_FileName.ReverseFind('\\');
				if (0 < nPath)
					msg = m_FileName.Mid(nPath+1);
				else
					msg = m_FileName;
				ioProgress->SetCaption(msg);
				CString str;
				str.FormatMessage(IDS_READING_ROWSCOLS, nRows, nCols);
				ioProgress->SetMessage(str);
				ioProgress->SetNumProgressBars(1);
				ioProgress->SetRange(1, 0, nRows);
				ioProgress->Show();
			}
			bool bAbort = false;
			outData.reserve(nRows);
			for (int iCellRow = 0; !bAbort && iCellRow < nRows; ++iCellRow)
			{
				if (ioProgress)
					ioProgress->StepIt(1);
				std::vector<CString> row;
				row.reserve(nCols);
				for (int iCellCol = 0; iCellCol < nCols; ++iCellCol)
				{
					CString cell1;
					if (!IWizardSpreadSheet::GetRowCol(iRow + iCellRow - 1, iCol + iCellCol - 1, cell1))
					{
						bAbort = true;
						break;
					}
					CString str;
#if _MSC_VER < 1300
					// VC6 can't translate a variant to cstring directly. sigh.
					COleVariant var = Excel8::Range(range.get_Range(COleVariant(cell1), COleVariant(cell1))).get_Value();
					if (var.vt == VT_BSTR)
						str = var.bstrVal;
#else
					str = Excel8::Range(range.get_Range(CComVariant(cell1), CComVariant(cell1))).get_Value();
#endif
					row.push_back(str);
				}
				outData.push_back(row);
			}
		}
		return true;
	}
	catch (COleDispatchException* ex)
	{
		ex->Delete();
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////

CWizardCalc* CWizardCalc::Create()
{
#if _MSC_VER < 1300
	return NULL;
#else
	CWaitCursor wait;
	CWizardCalc* pCalc = new CWizardCalc();
	if (pCalc)
	{
		bool bKill = false;
		if (!pCalc->m_Manager)
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
#endif
}


#if _MSC_VER >= 1300

CWizardCalc::CWizardCalc()
{
	if (!SUCCEEDED(m_Manager.CoCreateInstance(L"com.sun.star.ServiceManager")))
		m_Manager.Release();
}


CWizardCalc::~CWizardCalc()
{
}


IWizardExporterPtr CWizardCalc::GetExporter() const
{
	return IWizardExporterPtr(CWizardCalcExport::Create(m_Manager));
}


IWizardImporterPtr CWizardCalc::GetImporter() const
{
	return IWizardImporterPtr(CWizardCalcImport::Create(m_Manager));
}

#endif

/////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1300

CWizardCalcExport* CWizardCalcExport::Create(
		ooCalc::CComManagerDriver& ioManager)
{
	return new CWizardCalcExport(ioManager);
}


CWizardCalcExport::CWizardCalcExport(
		ooCalc::CComManagerDriver& ioManager)
	: CWizardBaseExport(true)
	, m_Manager(ioManager)
{
}


CWizardCalcExport::~CWizardCalcExport()
{
}


ooCalc::ooXSpreadsheet CWizardCalcExport::GetWorksheet()
{
	if (!m_Document)
	{
		CComVariant param1(L"private:factory/scalc");
		m_Document = m_Manager.loadComponentFromURL(param1);
	}
	if (!m_Worksheet)
	{
		m_Worksheet = m_Document.getSheet(0);
	}
	return m_Worksheet;
}


bool CWizardCalcExport::ExportDataArray(
		long inRowTop,
		long inColLeft)
{
	if (!ArrayOkay())
		return false;

	ooCalc::ooXSpreadsheet worksheet = GetWorksheet();
	if (!worksheet)
		return false;

	ooCalc::ooXCellRange range = worksheet.getCellRangeByPosition(
			inColLeft, inRowTop,
			inColLeft + m_Cols - 1, inRowTop + m_Rows - 1);
	if (!range)
		return false;

	if (!range.setDataArray(m_Array))
		return false;

	CComDispatchDriver columns = range.getColumns();
	if (!columns)
		return false;
	columns.PutPropertyByName(L"OptimalWidth", &covTrue);

	m_Rows = m_Cols = 0;

	return true;
}


bool CWizardCalcExport::AllowAccess(bool bAllow)
{
	// TODO
	return false;
}


bool CWizardCalcExport::SetTextColor(
		long inRow,
		long inCol,
		wxColour inColor)
{
	// TODO
	return false;
}


bool CWizardCalcExport::SetBackColor(
		long inRow,
		long inCol,
		wxColour inColor)
{
	// TODO
	return false;
}


bool CWizardCalcExport::SetItalic(
		long inRow,
		long inCol,
		bool bItalic)
{
	// TODO
	return false;
}


bool CWizardCalcExport::SetBold(
		long inRow,
		long inCol,
		bool bBold)
{
	// TODO
	return false;
}


bool CWizardCalcExport::InsertData(
		long inRow,
		long inCol,
		double inData)
{
	ooCalc::ooXSpreadsheet worksheet = GetWorksheet();
	if (!worksheet)
		return false;
	ooCalc::ooXCell cell = worksheet.getCellByPosition(inCol, inRow);
	if (!cell)
		return false;
	return cell.setValue(inData);
}


bool CWizardCalcExport::InsertData(
		long inRow,
		long inCol,
		CString const& inData)
{
	ooCalc::ooXSpreadsheet worksheet = GetWorksheet();
	if (!worksheet)
		return false;
	ooCalc::ooXCell cell = worksheet.getCellByPosition(inCol, inRow);
	if (!cell)
		return false;
	return cell.setFormula(inData);
}


bool CWizardCalcExport::AutoFit(
		long inColFrom,
		long inColTo)
{
	ooCalc::ooXSpreadsheet worksheet = GetWorksheet();
	if (!worksheet)
		return false;
	ooCalc::ooXCellRange range = worksheet.getCellRangeByPosition(
			inColFrom, 0,
			inColTo, 0);
	if (!range)
		return false;
	CComDispatchDriver columns = range.getColumns();
	if (!columns)
		return false;
	columns.PutPropertyByName(L"OptimalWidth", &covTrue);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

CWizardCalcImport* CWizardCalcImport::Create(
		ooCalc::CComManagerDriver& ioManager)
{
	return new CWizardCalcImport(ioManager);
}


CWizardCalcImport::CWizardCalcImport(
		ooCalc::CComManagerDriver& ioManager)
	: m_Manager(ioManager)
{
}


CWizardCalcImport::~CWizardCalcImport()
{
	m_Document.dispose();
}


bool CWizardCalcImport::OpenFile(CString const& inFilename)
{
	if (!m_Document)
	{
		CString fileName(inFilename);
		fileName.Replace(wxT("\\"), wxT("/"));
		fileName = wxT("file:///") + fileName;
		CComVariant param1(fileName);
		m_Document = m_Manager.loadComponentFromURL(param1);
	}
	if (!m_Worksheet)
		m_Worksheet = m_Document.getSheet(0);
	if (!m_Worksheet)
		return false;
	m_FileName = inFilename;
	return true;
}


bool CWizardCalcImport::GetData(
		std::vector< std::vector<CString> >& outData,
		IDlgProgress* ioProgress)
{
	outData.clear();
	if (!m_Worksheet)
		return false;

	ooCalc::ooXSheetCellCursor cursor = m_Worksheet.createCursor();
	if (cursor)
	{
		// Set up the data area
		if (!cursor.gotoStartOfUsedArea(false))
			return false;
		if (!cursor.gotoEndOfUsedArea(true))
			return false;
	}
	COleSafeArray dataAll = cursor.getDataArray();

	DWORD dim = dataAll.GetDim();
	// Should be only 1 dimension
	// Open office sets the data as an array of arrays.
	if (1 == dim)
	{
		bool bAbort = false;

		long lbound, ubound;
		dataAll.GetLBound(1, &lbound);
		dataAll.GetUBound(1, &ubound);
		long nRows = ubound - lbound + 1;
		outData.reserve(nRows);

		if (ioProgress)
		{
			CString msg;
			int nPath = m_FileName.ReverseFind('\\');
			if (0 < nPath)
				msg = m_FileName.Mid(nPath+1);
			else
				msg = m_FileName;
			ioProgress->SetCaption(msg);
			CString str;
			str.FormatMessage(IDS_READING_ROWS, nRows);
			ioProgress->SetMessage(str);
			ioProgress->SetNumProgressBars(1);
			ioProgress->SetRange(1, 0, nRows);
			ioProgress->Show();
		}

		for (long r = lbound; !bAbort && r <= ubound; ++r)
		{
			if (ioProgress)
				ioProgress->StepIt(1);

			long index[2];
			index[0] = r;
			index[1] = 0;
			VARIANT v;
			dataAll.GetElement(index, &v);
			COleSafeArray dataRow(v);
			dim = dataRow.GetDim();

			// Again, should only be one.
			if (1 == dim)
			{
				long lbound2, ubound2;
				dataRow.GetLBound(1, &lbound2);
				dataRow.GetUBound(1, &ubound2);
				long nCols = ubound2 - lbound2 + 1;
				std::vector<CString> row;
				row.reserve(nCols);
				for (long r2 = lbound2; r2 <= ubound2; ++r2)
				{
					index[0] = r2;
					index[1] = 0;
					VARIANT v2;
					dataRow.GetElement(index, &v2);
					CComVariant col(v2);
					CString str(col);
					row.push_back(str);
				}
				outData.push_back(row);
			}
		}
	}
	return true;
}

#endif

#endif //HAS_AUTOMATION

/////////////////////////////////////////////////////////////////////////////

IWizardSpreadSheetPtr IWizardSpreadSheet::Create(eType inType)
{
	IWizardSpreadSheetPtr pInterface;
#if HAS_AUTOMATION
	switch (inType)
	{
	default:
		break;
	case eMicrosoftExcel:
		pInterface = IWizardSpreadSheetPtr(CWizardExcel::Create());
		break;
	case eOpenOfficeCalc:
		pInterface = IWizardSpreadSheetPtr(CWizardCalc::Create());
		break;
	}
#endif
	return pInterface;
}


// static helper functions
long IWizardSpreadSheet::GetMaxRows()
{
	return 65536; // Excel limits (I believe later versions of excel/calc may expand this, but we support older versions too)
}


long IWizardSpreadSheet::GetMaxCols()
{
	return 256; // Excel limits
}


bool IWizardSpreadSheet::GetRowCol(
		long inRow,
		long inCol,
		wxString& outCell)
{
	// Lookup tables are nice!
	static const wxChar* const sc_ColumnNames[256] =
	{
		wxT("A"), wxT("B"), wxT("C"), wxT("D"), wxT("E"), wxT("F"), wxT("G"), wxT("H"),
		wxT("I"), wxT("J"), wxT("K"), wxT("L"), wxT("M"), wxT("N"), wxT("O"), wxT("P"),
		wxT("Q"), wxT("R"), wxT("S"), wxT("T"), wxT("U"), wxT("V"), wxT("W"), wxT("X"),
		wxT("Y"), wxT("Z"), wxT("AA"),wxT("AB"),wxT("AC"),wxT("AD"),wxT("AE"),wxT("AF"),
		wxT("AG"),wxT("AH"),wxT("AI"),wxT("AJ"),wxT("AK"),wxT("AL"),wxT("AM"),wxT("AN"),
		wxT("AO"),wxT("AP"),wxT("AQ"),wxT("AR"),wxT("AS"),wxT("AT"),wxT("AU"),wxT("AV"),
		wxT("AW"),wxT("AX"),wxT("AY"),wxT("AZ"),wxT("BA"),wxT("BB"),wxT("BC"),wxT("BD"),
		wxT("BE"),wxT("BF"),wxT("BG"),wxT("BH"),wxT("BI"),wxT("BJ"),wxT("BK"),wxT("BL"),
		wxT("BM"),wxT("BN"),wxT("BO"),wxT("BP"),wxT("BQ"),wxT("BR"),wxT("BS"),wxT("BT"),
		wxT("BU"),wxT("BV"),wxT("BW"),wxT("BX"),wxT("BY"),wxT("BZ"),wxT("CA"),wxT("CB"),
		wxT("CC"),wxT("CD"),wxT("CE"),wxT("CF"),wxT("CG"),wxT("CH"),wxT("CI"),wxT("CJ"),
		wxT("CK"),wxT("CL"),wxT("CM"),wxT("CN"),wxT("CO"),wxT("CP"),wxT("CQ"),wxT("CR"),
		wxT("CS"),wxT("CT"),wxT("CU"),wxT("CV"),wxT("CW"),wxT("CX"),wxT("CY"),wxT("CZ"),
		wxT("DA"),wxT("DB"),wxT("DC"),wxT("DD"),wxT("DE"),wxT("DF"),wxT("DG"),wxT("DH"),
		wxT("DI"),wxT("DJ"),wxT("DK"),wxT("DL"),wxT("DM"),wxT("DN"),wxT("DO"),wxT("DP"),
		wxT("DQ"),wxT("DR"),wxT("DS"),wxT("DT"),wxT("DU"),wxT("DV"),wxT("DW"),wxT("DX"),
		wxT("DY"),wxT("DZ"),wxT("EA"),wxT("EB"),wxT("EC"),wxT("ED"),wxT("EE"),wxT("EF"),
		wxT("EG"),wxT("EH"),wxT("EI"),wxT("EJ"),wxT("EK"),wxT("EL"),wxT("EM"),wxT("EN"),
		wxT("EO"),wxT("EP"),wxT("EQ"),wxT("ER"),wxT("ES"),wxT("ET"),wxT("EU"),wxT("EV"),
		wxT("EW"),wxT("EX"),wxT("EY"),wxT("EZ"),wxT("FA"),wxT("FB"),wxT("FC"),wxT("FD"),
		wxT("FE"),wxT("FF"),wxT("FG"),wxT("FH"),wxT("FI"),wxT("FJ"),wxT("FK"),wxT("FL"),
		wxT("FM"),wxT("FN"),wxT("FO"),wxT("FP"),wxT("FQ"),wxT("FR"),wxT("FS"),wxT("FT"),
		wxT("FU"),wxT("FV"),wxT("FW"),wxT("FX"),wxT("FY"),wxT("FZ"),wxT("GA"),wxT("GB"),
		wxT("GC"),wxT("GD"),wxT("GE"),wxT("GF"),wxT("GG"),wxT("GH"),wxT("GI"),wxT("GJ"),
		wxT("GK"),wxT("GL"),wxT("GM"),wxT("GN"),wxT("GO"),wxT("GP"),wxT("GQ"),wxT("GR"),
		wxT("GS"),wxT("GT"),wxT("GU"),wxT("GV"),wxT("GW"),wxT("GX"),wxT("GY"),wxT("GZ"),
		wxT("HA"),wxT("HB"),wxT("HC"),wxT("HD"),wxT("HE"),wxT("HF"),wxT("HG"),wxT("HH"),
		wxT("HI"),wxT("HJ"),wxT("HK"),wxT("HL"),wxT("HM"),wxT("HN"),wxT("HO"),wxT("HP"),
		wxT("HQ"),wxT("HR"),wxT("HS"),wxT("HT"),wxT("HU"),wxT("HV"),wxT("HW"),wxT("HX"),
		wxT("HY"),wxT("HZ"),wxT("IA"),wxT("IB"),wxT("IC"),wxT("ID"),wxT("IE"),wxT("IF"),
		wxT("IG"),wxT("IH"),wxT("II"),wxT("IJ"),wxT("IK"),wxT("IL"),wxT("IM"),wxT("IN"),
		wxT("IO"),wxT("IP"),wxT("IQ"),wxT("IR"),wxT("IS"),wxT("IT"),wxT("IU"),wxT("IV"),
	};
	if (inRow < GetMaxRows() && inCol < GetMaxCols())
	{
		otstringstream str;
		str << sc_ColumnNames[inCol] << inRow + 1;
		outCell = str.str().c_str();
		return true;
	}
	else
	{
		outCell.Empty();
		return false;
	}
}
