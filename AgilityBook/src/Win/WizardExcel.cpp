/*
 * Copyright © 2004-2005 David Connet. All Rights Reserved.
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
using namespace Excel8;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CWizardExcelExport::CWizardExcelExport()
{
}
CWizardExcelExport::~CWizardExcelExport()
{
}

CWizardExcelImport::CWizardExcelImport()
{
}
CWizardExcelImport::~CWizardExcelImport()
{
}

/////////////////////////////////////////////////////////////////////////////

class CWizardExcelExportImpl : public CWizardExcelExport
{
public:
	CWizardExcelExportImpl(_Application& ioApp);
	virtual ~CWizardExcelExportImpl();
	bool IsOkay() const		{return NULL != m_Worksheet.m_lpDispatch;}

	// Safe array is ready for use
	virtual bool ArrayOkay() const	{return 0 < m_Rows && 0 < m_Cols;}
	// Create the safe array.
	virtual bool CreateArray(long inRows, long inCols);
	// Insert data into the safe array.
	virtual bool InsertArrayData(long inRow, long inCol, CString const& inData);
	// Copy the safe array to excel (safe array is reset)
	virtual bool ExportDataArray(long inRowTop = 0, long inColLeft = 0);
	virtual bool AllowAccess(bool bAllow);
	virtual bool InsertData(long nRow, long nCol, COleVariant const& inData);
	virtual bool InsertFormula(long inRowFrom, long inColFrom,
		long inRowTo, long inColTo,
		CString const& inFormula);

private:
	_Application& m_App;
	_Worksheet m_Worksheet;
	long m_Rows;
	long m_Cols;
	COleSafeArray m_Array;
	// Commonly used OLE variants.
	COleVariant covTrue;
	COleVariant covFalse;
	COleVariant covOptional;
};

CWizardExcelExportImpl::CWizardExcelExportImpl(_Application& ioApp)
	: m_App(ioApp)
	, m_Rows(0)
	, m_Cols(0)
	, covTrue(static_cast<short>(TRUE))
	, covFalse(static_cast<short>(FALSE))
	, covOptional(static_cast<long>(DISP_E_PARAMNOTFOUND), VT_ERROR)
{
	// Create a new workbook.
	Workbooks books = m_App.get_Workbooks();
	_Workbook book = books.Add(covOptional);
	// Get the first sheet.
	Worksheets sheets = book.get_Sheets();
	m_Worksheet = sheets.get_Item(COleVariant((short)1));
}

CWizardExcelExportImpl::~CWizardExcelExportImpl()
{
	if (IsOkay() && !m_App.get_Visible())
		m_App.Quit();
}

bool CWizardExcelExportImpl::CreateArray(long inRows, long inCols)
{
	if (ArrayOkay())
		return false;
	if (0 >= inRows || 0 >= inCols)
		return false;
	if (inRows >= CWizardExcel::GetMaxRows())
		inRows = CWizardExcel::GetMaxRows() - 1;
	if (inCols >= CWizardExcel::GetMaxCols())
		inCols = CWizardExcel::GetMaxCols() - 1;
	DWORD numElements[2];
	numElements[0] = m_Rows = inRows;
	numElements[1] = m_Cols = inCols;
	m_Array.Create(VT_BSTR, 2, numElements);
	return true;
}

bool CWizardExcelExportImpl::InsertArrayData(long inRow, long inCol, CString const& inData)
{
	if (!ArrayOkay())
		return false;
	if (0 > inRow || inRow >= m_Rows
	|| 0 > inCol || inCol >= m_Cols)
		return false;
	long index[2];
	index[0] = inRow;
	index[1] = inCol;
	VARIANT v;
	VariantInit(&v);
	v.vt = VT_BSTR;
	v.bstrVal = inData.AllocSysString();
	m_Array.PutElement(index, v.bstrVal);
	SysFreeString(v.bstrVal);
	VariantClear(&v);
	return true;
}

bool CWizardExcelExportImpl::ExportDataArray(long inRowTop, long inColLeft)
{
	if (!ArrayOkay())
		return false;

	CString cell1, cell2;
	if (!CWizardExcel::GetRowCol(inRowTop, inColLeft, cell1))
		return false;
	if (!CWizardExcel::GetRowCol(inRowTop + m_Rows - 1, inColLeft + m_Cols - 1, cell2))
		return false;

	m_App.put_UserControl(FALSE);

	Range range = m_Worksheet.get_Range(COleVariant(cell1), COleVariant(cell2));
	range.put_Value2(COleVariant(m_Array));
	m_Array.Detach();

	CWizardExcel::GetRowCol(inRowTop, inColLeft + m_Cols - 1, cell2);
	range = m_Worksheet.get_Range(COleVariant(cell1), COleVariant(cell2));
	Range cols = range.get_EntireColumn();
	cols.AutoFit();

	m_Rows = m_Cols = 0;
	m_App.put_Visible(TRUE);
	m_App.put_UserControl(TRUE);

	return true;
}

bool CWizardExcelExportImpl::AllowAccess(bool bAllow)
{
	if (bAllow)
	{
		m_App.put_Visible(TRUE);
		m_App.put_UserControl(TRUE);
	}
	else
	{
		m_App.put_UserControl(FALSE);
	}
	return true;
}

bool CWizardExcelExportImpl::InsertData(long inRow, long inCol, COleVariant const& inData)
{
	CString cell1;
	if (!CWizardExcel::GetRowCol(inRow, inCol, cell1))
		return false;
	Range range = m_Worksheet.get_Range(COleVariant(cell1), COleVariant(cell1));
	range.put_Value2(inData);
	return true;
}

bool CWizardExcelExportImpl::InsertFormula(long inRowFrom, long inColFrom,
	long inRowTo, long inColTo,
	CString const& inFormula)
{
	CString cell1, cell2;
	if (!CWizardExcel::GetRowCol(inRowFrom, inColFrom, cell1))
		return false;
	if (!CWizardExcel::GetRowCol(inRowTo, inColTo, cell2))
		return false;
	Range range = m_Worksheet.get_Range(COleVariant(cell1), COleVariant(cell2));
	range.put_Formula(COleVariant(inFormula));
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class CWizardExcelImportImpl : public CWizardExcelImport
{
public:
	CWizardExcelImportImpl(_Application& ioApp);
	virtual ~CWizardExcelImportImpl();
	bool IsOkay() const		{return NULL != m_Worksheet.m_lpDispatch;}

	virtual bool OpenFile(CString const& inFilename);
	virtual bool GetData(std::vector< std::vector<CString> >& outData, IDlgProgress* ioProgress);

private:
	_Application& m_App;
	_Worksheet m_Worksheet;
	CString m_FileName;
	// Commonly used OLE variants.
	COleVariant covTrue;
	COleVariant covFalse;
	COleVariant covOptional;
};

CWizardExcelImportImpl::CWizardExcelImportImpl(_Application& ioApp)
	: m_App(ioApp)
	, covTrue(static_cast<short>(TRUE))
	, covFalse(static_cast<short>(FALSE))
	, covOptional(static_cast<long>(DISP_E_PARAMNOTFOUND), VT_ERROR)
{
}

CWizardExcelImportImpl::~CWizardExcelImportImpl()
{
	if (IsOkay())
		m_App.Quit();
}

bool CWizardExcelImportImpl::OpenFile(CString const& inFilename)
{
	// Create a new workbook.
	Workbooks books = m_App.get_Workbooks();
	_Workbook book = books.Open(inFilename, covOptional, covTrue, covOptional,
		covOptional, covOptional, covOptional, covOptional,
		covOptional, covOptional, covOptional, covOptional,
		covOptional);
	if (NULL == book.m_lpDispatch)
		return false;
	m_FileName = inFilename;
	// Get the first sheet.
	Worksheets sheets = book.get_Sheets();
	m_Worksheet = sheets.get_Item(COleVariant((short)1));
	return IsOkay();
}

bool CWizardExcelImportImpl::GetData(std::vector< std::vector<CString> >& outData, IDlgProgress* ioProgress)
{
	outData.clear();
	if (!IsOkay())
		return false;
	Range range = m_Worksheet.get_UsedRange();
	long iRow = range.get_Row();
	long iCol = range.get_Column();
	long nRows = Range(range.get_Rows()).get_Count();
	long nCols = Range(range.get_Columns()).get_Count();
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
			msg.Format("Reading %ld rows and %ld columns", nRows, nCols);
			ioProgress->SetMessage(msg);
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
				if (!CWizardExcel::GetRowCol(iRow + iCellRow - 1, iCol + iCellCol - 1, cell1))
				{
					bAbort = true;
					break;
				}
				CString str;
#if _MSC_VER < 1300
				// VC6 can't translate a variant to cstring directly. sigh.
				COleVariant var = Range(range.get_Range(COleVariant(cell1), COleVariant(cell1))).get_Value();
				if (var.vt == VT_BSTR)
					str = var.bstrVal;
#else
				str = Range(range.get_Range(COleVariant(cell1), COleVariant(cell1))).get_Value();
#endif
				row.push_back(str);
			}
			outData.push_back(row);
		}
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CWizardExcelImpl

/**
 * Hide all OLE implmentation details.
 */
class CWizardExcelImpl
{
public:
	CWizardExcelImpl();
	bool IsOkay() const		{return NULL != m_App.m_lpDispatch;}
	CWizardExcelExport* GetExporter();
	CWizardExcelImport* GetImporter();

	_Application m_App;
};

CWizardExcelImpl::CWizardExcelImpl()
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

CWizardExcelExport* CWizardExcelImpl::GetExporter()
{
	return new CWizardExcelExportImpl(m_App);
}

CWizardExcelImport* CWizardExcelImpl::GetImporter()
{
	return new CWizardExcelImportImpl(m_App);
}

/////////////////////////////////////////////////////////////////////////////
// CWizardExcel

// static helper functions
long CWizardExcel::GetMaxRows()
{
	return 65536;
}
long CWizardExcel::GetMaxCols()
{
	return 256;
}
bool CWizardExcel::GetRowCol(long inRow, long inCol, CString& outCell)
{
	// Lookup tables are nice!
	static const char* const sc_ColumnNames[256] =
	{
		"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P",
		"Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "AA","AB","AC","AD","AE","AF",
		"AG","AH","AI","AJ","AK","AL","AM","AN","AO","AP","AQ","AR","AS","AT","AU","AV",
		"AW","AX","AY","AZ","BA","BB","BC","BD","BE","BF","BG","BH","BI","BJ","BK","BL",
		"BM","BN","BO","BP","BQ","BR","BS","BT","BU","BV","BW","BX","BY","BZ","CA","CB",
		"CC","CD","CE","CF","CG","CH","CI","CJ","CK","CL","CM","CN","CO","CP","CQ","CR",
		"CS","CT","CU","CV","CW","CX","CY","CZ","DA","DB","DC","DD","DE","DF","DG","DH",
		"DI","DJ","DK","DL","DM","DN","DO","DP","DQ","DR","DS","DT","DU","DV","DW","DX",
		"DY","DZ","EA","EB","EC","ED","EE","EF","EG","EH","EI","EJ","EK","EL","EM","EN",
		"EO","EP","EQ","ER","ES","ET","EU","EV","EW","EX","EY","EZ","FA","FB","FC","FD",
		"FE","FF","FG","FH","FI","FJ","FK","FL","FM","FN","FO","FP","FQ","FR","FS","FT",
		"FU","FV","FW","FX","FY","FZ","GA","GB","GC","GD","GE","GF","GG","GH","GI","GJ",
		"GK","GL","GM","GN","GO","GP","GQ","GR","GS","GT","GU","GV","GW","GX","GY","GZ",
		"HA","HB","HC","HD","HE","HF","HG","HH","HI","HJ","HK","HL","HM","HN","HO","HP",
		"HQ","HR","HS","HT","HU","HV","HW","HX","HY","HZ","IA","IB","IC","ID","IE","IF",
		"IG","IH","II","IJ","IK","IL","IM","IN","IO","IP","IQ","IR","IS","IT","IU","IV",
	};
	if (inRow < GetMaxRows() && inCol < GetMaxCols())
	{
		outCell.Format("%s%ld", sc_ColumnNames[inCol], inRow+1);
		return true;
	}
	else
	{
		outCell.Empty();
		return false;
	}
}

CWizardExcel::CWizardExcel()
	: m_Excel(NULL)
{
	m_Excel = new CWizardExcelImpl();
	if (m_Excel && !m_Excel->IsOkay())
	{
		delete m_Excel;
		m_Excel = NULL;
	}
}

CWizardExcel::~CWizardExcel()
{
	delete m_Excel;
}

bool CWizardExcel::IsAvailable() const
{
	return (m_Excel && m_Excel->IsOkay());
}

CWizardExcelExport* CWizardExcel::GetExporter()
{
	if (m_Excel)
		return m_Excel->GetExporter();
	return NULL;
}

CWizardExcelImport* CWizardExcel::GetImporter()
{
	if (m_Excel)
		return m_Excel->GetImporter();
	return NULL;
}
