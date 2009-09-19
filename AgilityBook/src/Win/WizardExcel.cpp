/*
 * Copyright (c) 2004-2009 David Connet. All Rights Reserved.
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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-07-24 DRC Removed option to export by array, completed wx port.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2004-09-30 DRC Created
 */

#include "stdafx.h"
#include "WizardExcel.h"

#if defined(WIN32) && wxUSE_OLE_AUTOMATION
#define HAS_AUTOMATION	1
#else
#define HAS_AUTOMATION	0
#endif

#include "ARBTypes.h"
#if HAS_AUTOMATION
#include "AgilityBook.h"
#include "DlgProgress.h"
#include <wx/filename.h>
#include <wx/list.h>
#include <wx/variant.h>
#include <wx/msw/ole/automtn.h>
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

class CWizardBaseImport : public IWizardImporter
{
public:
	CWizardBaseImport()
	{
	}

protected:
	wxString m_FileName;
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
	mutable wxAutomationObject m_App;
};


class CWizardExcelExport : public IWizardExporter
{
protected:
	CWizardExcelExport(wxAutomationObject& ioApp);
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

	virtual bool InsertData(
			long inRow,
			long inCol,
			double inData);
	virtual bool InsertData(
			long inRow,
			long inCol,
			wxString const& inData,
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

	virtual bool OpenFile(wxString const& inFilename);
	virtual bool GetData(
			std::vector< std::vector<wxString> >& outData,
			IDlgProgress* ioProgress);

private:
	wxAutomationObject& m_App;
	wxAutomationObject m_Worksheet;
};

/////////////////////////////////////////////////////////////////////////////

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
	mutable wxAutomationObject m_Manager;
	mutable wxAutomationObject m_Desktop;
};


class CWizardCalcExport : public IWizardExporter
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

	virtual bool InsertData(
			long inRow,
			long inCol,
			double inData);
	virtual bool InsertData(
			long inRow,
			long inCol,
			wxString const& inData,
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

	virtual bool OpenFile(wxString const& inFilename);
	virtual bool GetData(
			std::vector< std::vector<wxString> >& outData,
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


IWizardExporterPtr CWizardExcel::GetExporter() const
{
	return IWizardExporterPtr(CWizardExcelExport::Create(m_App));
}


IWizardImporterPtr CWizardExcel::GetImporter() const
{
	return IWizardImporterPtr(CWizardExcelImport::Create(m_App));
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
{
	// Create a new workbook.
	wxAutomationObject book = m_App.CallMethod(wxT("Workbooks.Add"));
	// Get the first sheet.
	wxAutomationObject sheets = book.GetProperty(wxT("Sheets"));
	wxVariant args[1];
	args[0] = wxVariant((short)1);
	sheets.GetObject(m_Worksheet, wxT("Item"), 1, args);
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
	wxString cell1;
	if (!IWizardSpreadSheet::GetRowCol(inRow, inCol, cell1))
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
	wxString cell1;
	if (!IWizardSpreadSheet::GetRowCol(inRow, inCol, cell1))
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
	wxString cell1;
	if (!IWizardSpreadSheet::GetRowCol(inRow, inCol, cell1))
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
	wxString cell1;
	if (!IWizardSpreadSheet::GetRowCol(inRow, inCol, cell1))
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


bool CWizardExcelExport::InsertData(
		long inRow,
		long inCol,
		double inData)
{
	wxString cell1;
	if (!IWizardSpreadSheet::GetRowCol(inRow, inCol, cell1))
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
		wxString const& inData,
		bool bFormula)
{
	wxString cell1;
	if (!IWizardSpreadSheet::GetRowCol(inRow, inCol, cell1))
		return false;
	wxAutomationObject range;
	wxVariant args[2];
	args[0] = cell1;
	args[1] = cell1;
	m_Worksheet.GetObject(range, wxT("Range"), 2, args);
	wxString property = bFormula ? wxT("Formula") : wxT("Value2");
	if (0 < inData.length() && '=' == inData[0])
	{
		wxString data = wxString(wxT("'")) + inData;
		range.PutProperty(property, data);
	}
	else
		range.PutProperty(property, inData);
	return true;
}


bool CWizardExcelExport::AutoFit(
		long inColFrom,
		long inColTo)
{
	wxString cell1, cell2;
	if (!IWizardSpreadSheet::GetRowCol(0, inColFrom, cell1))
		return false;
	if (!IWizardSpreadSheet::GetRowCol(0, inColTo, cell2))
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


bool CWizardExcelImport::OpenFile(wxString const& inFilename)
{
	wxVariant bk = m_App.CallMethod(wxT("Workbooks.Open"), inFilename);
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
		std::vector< std::vector<wxString> >& outData,
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
			wxString msg = filename.GetFullPath();
			ioProgress->SetCaption(msg);
			wxString str = wxString::Format(_("IDS_READING_ROWSCOLS"), nRows, nCols);
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
			std::vector<wxString> row;
			row.reserve(nCols);
			for (int iCellCol = 0; iCellCol < nCols; ++iCellCol)
			{
				wxString cell1;
				if (!IWizardSpreadSheet::GetRowCol(iRow + iCellRow - 1, iCol + iCellCol - 1, cell1))
				{
					bAbort = true;
					break;
				}
				wxAutomationObject range2;
				wxVariant args[2];
				args[0] = cell1;
				args[1] = cell1;
				range.GetObject(range2, wxT("Range"), 2, args);
				row.push_back(range2.GetProperty(wxT("Value")).GetString());
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


IWizardExporterPtr CWizardCalc::GetExporter() const
{
	return IWizardExporterPtr(CWizardCalcExport::Create(m_Desktop));
}


IWizardImporterPtr CWizardCalc::GetImporter() const
{
	return IWizardImporterPtr(CWizardCalcImport::Create(m_Desktop));
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
		wxString const& inData,
		bool bFormula)
{
	if (!CreateWorksheet())
		return false;
	wxAutomationObject cell(m_Worksheet.CallMethod(wxT("getCellByPosition"), inCol, inRow));
	if (!cell.GetDispatchPtr())
		return false;
	wxString property = bFormula ? wxT("Formula") : wxT("String");
	return cell.PutProperty(property, inData);
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


bool CWizardCalcImport::OpenFile(wxString const& inFilename)
{
	if (!m_Document.GetDispatchPtr())
	{
		// 'Calc' doesn't take kindly to wxWidgets FileNameToURL syntax.
		// Since this is windows only, screw it, just format it the way calc likes.
		wxString fileName(inFilename);
		fileName.Replace(_T("\\"), _T("/"));
		fileName = _T("file:///") + fileName;
		wxVariant args;
		args.NullList();
		wxVariant file = m_Desktop.CallMethod(wxT("loadComponentFromURL"), fileName, wxT("_blank"), 0, args);
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
		std::vector< std::vector<wxString> >& outData,
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
		wxString msg = filename.GetFullPath();
		ioProgress->SetCaption(msg);
		wxString str = wxString::Format(_("IDS_READING_ROWS"), nRows);
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
		std::vector<wxString> row;
		row.reserve(nCols);
		for (int iCol = 0; !bAbort && iCol < nCols; ++iCol)
		{
			row.push_back(vRow[iCol].GetString());
		}
		outData.push_back(row);
	}
	return true;
}

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
		outCell << sc_ColumnNames[inCol] << inRow + 1;
		return true;
	}
	else
	{
		outCell.Empty();
		return false;
	}
}
