/*
 * Copyright © 2004-2007 David Connet. All Rights Reserved.
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
 * @li 2004-09-30 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "WizardExcel.h"

#include "DlgProgress.h"

#include "excel8.h"
#if _MSC_VER < 1300
#define CComVariant COleVariant
#else
#include "ooCalc.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	mutable Excel8::_Application m_App;
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
			COLORREF inColor);
	virtual bool SetBackColor(
			long inRow,
			long inCol,
			COLORREF inColor);
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
			COLORREF inColor);
	virtual bool SetBackColor(
			long inRow,
			long inCol,
			COLORREF inColor);
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
{
	// Get the ClassID from the ProgID.
	CLSID clsid;
	if (NOERROR != CLSIDFromProgID(L"Excel.Application", &clsid))
	{
		return;
	}

	// Get an interface to the running instance.
	//LPUNKNOWN lpUnk;
	//LPDISPATCH lpDispatch;
	//if (NOERROR == GetActiveObject(clsid, NULL, &lpUnk))
	//{
	//	HRESULT hr = lpUnk->QueryInterface(IID_IDispatch, (LPVOID*)&lpDispatch);
	//	lpUnk->Release();
	//	if (hr == NOERROR)
	//		m_App.AttachDispatch(lpDispatch, TRUE);
	//}

	// If dispatch ptr not attached yet, need to create one
	COleException e;
	if (m_App.m_lpDispatch == NULL && !m_App.CreateDispatch(clsid, &e))
	{
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
		COLORREF inColor)
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
		COLORREF inColor)
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
	if (NULL != m_Worksheet.m_lpDispatch)
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
		COLORREF inColor)
{
	// TODO
	return false;
}


bool CWizardCalcExport::SetBackColor(
		long inRow,
		long inCol,
		COLORREF inColor)
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
		fileName.Replace(_T("\\"), _T("/"));
		fileName = _T("file:///") + fileName;
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

/////////////////////////////////////////////////////////////////////////////

IWizardSpreadSheetPtr IWizardSpreadSheet::Create(eType inType)
{
	IWizardSpreadSheetPtr pInterface;
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
	return pInterface;
}


// static helper functions
long IWizardSpreadSheet::GetMaxRows()
{
	return 65536;
}


long IWizardSpreadSheet::GetMaxCols()
{
	return 256;
}


bool IWizardSpreadSheet::GetRowCol(
		long inRow,
		long inCol,
		CString& outCell)
{
	// Lookup tables are nice!
	static const TCHAR* const sc_ColumnNames[256] =
	{
		_T("A"), _T("B"), _T("C"), _T("D"), _T("E"), _T("F"), _T("G"), _T("H"),
		_T("I"), _T("J"), _T("K"), _T("L"), _T("M"), _T("N"), _T("O"), _T("P"),
		_T("Q"), _T("R"), _T("S"), _T("T"), _T("U"), _T("V"), _T("W"), _T("X"),
		_T("Y"), _T("Z"), _T("AA"),_T("AB"),_T("AC"),_T("AD"),_T("AE"),_T("AF"),
		_T("AG"),_T("AH"),_T("AI"),_T("AJ"),_T("AK"),_T("AL"),_T("AM"),_T("AN"),
		_T("AO"),_T("AP"),_T("AQ"),_T("AR"),_T("AS"),_T("AT"),_T("AU"),_T("AV"),
		_T("AW"),_T("AX"),_T("AY"),_T("AZ"),_T("BA"),_T("BB"),_T("BC"),_T("BD"),
		_T("BE"),_T("BF"),_T("BG"),_T("BH"),_T("BI"),_T("BJ"),_T("BK"),_T("BL"),
		_T("BM"),_T("BN"),_T("BO"),_T("BP"),_T("BQ"),_T("BR"),_T("BS"),_T("BT"),
		_T("BU"),_T("BV"),_T("BW"),_T("BX"),_T("BY"),_T("BZ"),_T("CA"),_T("CB"),
		_T("CC"),_T("CD"),_T("CE"),_T("CF"),_T("CG"),_T("CH"),_T("CI"),_T("CJ"),
		_T("CK"),_T("CL"),_T("CM"),_T("CN"),_T("CO"),_T("CP"),_T("CQ"),_T("CR"),
		_T("CS"),_T("CT"),_T("CU"),_T("CV"),_T("CW"),_T("CX"),_T("CY"),_T("CZ"),
		_T("DA"),_T("DB"),_T("DC"),_T("DD"),_T("DE"),_T("DF"),_T("DG"),_T("DH"),
		_T("DI"),_T("DJ"),_T("DK"),_T("DL"),_T("DM"),_T("DN"),_T("DO"),_T("DP"),
		_T("DQ"),_T("DR"),_T("DS"),_T("DT"),_T("DU"),_T("DV"),_T("DW"),_T("DX"),
		_T("DY"),_T("DZ"),_T("EA"),_T("EB"),_T("EC"),_T("ED"),_T("EE"),_T("EF"),
		_T("EG"),_T("EH"),_T("EI"),_T("EJ"),_T("EK"),_T("EL"),_T("EM"),_T("EN"),
		_T("EO"),_T("EP"),_T("EQ"),_T("ER"),_T("ES"),_T("ET"),_T("EU"),_T("EV"),
		_T("EW"),_T("EX"),_T("EY"),_T("EZ"),_T("FA"),_T("FB"),_T("FC"),_T("FD"),
		_T("FE"),_T("FF"),_T("FG"),_T("FH"),_T("FI"),_T("FJ"),_T("FK"),_T("FL"),
		_T("FM"),_T("FN"),_T("FO"),_T("FP"),_T("FQ"),_T("FR"),_T("FS"),_T("FT"),
		_T("FU"),_T("FV"),_T("FW"),_T("FX"),_T("FY"),_T("FZ"),_T("GA"),_T("GB"),
		_T("GC"),_T("GD"),_T("GE"),_T("GF"),_T("GG"),_T("GH"),_T("GI"),_T("GJ"),
		_T("GK"),_T("GL"),_T("GM"),_T("GN"),_T("GO"),_T("GP"),_T("GQ"),_T("GR"),
		_T("GS"),_T("GT"),_T("GU"),_T("GV"),_T("GW"),_T("GX"),_T("GY"),_T("GZ"),
		_T("HA"),_T("HB"),_T("HC"),_T("HD"),_T("HE"),_T("HF"),_T("HG"),_T("HH"),
		_T("HI"),_T("HJ"),_T("HK"),_T("HL"),_T("HM"),_T("HN"),_T("HO"),_T("HP"),
		_T("HQ"),_T("HR"),_T("HS"),_T("HT"),_T("HU"),_T("HV"),_T("HW"),_T("HX"),
		_T("HY"),_T("HZ"),_T("IA"),_T("IB"),_T("IC"),_T("ID"),_T("IE"),_T("IF"),
		_T("IG"),_T("IH"),_T("II"),_T("IJ"),_T("IK"),_T("IL"),_T("IM"),_T("IN"),
		_T("IO"),_T("IP"),_T("IQ"),_T("IR"),_T("IS"),_T("IT"),_T("IU"),_T("IV"),
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
