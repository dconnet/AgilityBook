/*
 * Copyright © 2006 David Connet. All Rights Reserved.
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
 * @brief Wrapper classes for OpenOffice Calc, based on ATL.
 * @author David Connet
 *
 * Revision History
 * @li 2006-10-26 DRC Created
 */

//XIndexAccess : XElementAccess
//	long				getCount()
//	any					getByIndex([in]long nIndex)
//
//XTableColumns : XIndexAccess
//	void				insertByIndex([in]long nIndex, [in]long nCount)
//	void				removeByIndex([in]long nIndex, [in]long nCount)
//
//XTableRows : XIndexAccess
//	void				insertByIndex([in]long nIndex, [in]long nCount)
//	void				removeByIndex([in]long nIndex, [in]long nCount)
//
//XColumnRowRange
//	XTableColumns		getColumns()
//	XTableRows			getRows()
//
//CellContentType
//	EMPTY
//	VALUE
//	TEXT
//	FORMULA
//
//XCell
//	string				getFormula()
//	void				setFormula([in]string aFormula)
//	double				getValue()
//	void				setValue([in]double nValue)
//	CellContentType		getType()
//	long				getError()
//		[0==none]
//
//XCellRangeData
//	sequence<sequence<any>>	getDataArray()
//	void				setDataArray([in]sequence<sequence<any>> sArray)
//
//XCellRange
//	XCell				getCellByPosition([in]long nColumn, [in]long nRow)
//	XCellRange			getCellRangeByPosition([in]long nLeft, [in]long nTop, [in]long nRight, [in]long nBottom)
//	XCellRange			getCellRangeByName([in]string aRange)
//
//XSheetCellRange : XCellRange
//	XSpreadsheet		getSpreadsheet()
//
//XSheetCellCursor : XSheetCellRange
//	void				collapseToCurrentRegion()
//	void				collapseToCurrentArray()
//	void				collapseToMergedArea()
//	void				expandToEntireColumns()
//	void				expandToEntireRows()
//	void				collapseToSize([in]long nColumns, [in]long nRows)
//
//XUsedAreaCursor
//	void				gotoStartOfUsedArea([in]boolean bExpand)
//	void				gotoEndOfUsedArea([in]boolean bExpand)
//
//XSpreadsheet : XSheetCellRange
//	XSheetCellCursor	createCursor()
//	XSheetCellCursor	createCursorByRange([in]XSheetCellRange aRange)
//
//SheetCellCursor : XSheetCellCursor, XUsedAreaCursor
//
//XElementAccess
//	type				getElementType()
//	boolean				hasElements()
//
//XNameAccess : XElementAccess
//	any					getByName([in]string aName)
//	sequence<string>	getElementNames()
//	boolean				hasByNames([in]string aName)
//
//XNameReplace : XNameAccess
//	void				replaceByName([in]string aName, [in]any aElement)
//
//XNameContainer : XNameReplace
//	void				insertByName([in]string aName, [in]any aElement)
//	void				removeByName([in]string aName)
//
//XSpreadsheets : XNameContainer
//	void				insertNewByName([in]string aName, [in]short nPosition)
//	void				moveByName([in]string aName, [in]short nDestination)
//	void				copyByName([in]string aName, [in]string aCopy, [in]short nDestination)
//
//XSpreadsheetDocument
//	XSpreadsheets		getSheets()

namespace ooCalc
{

// XCellRange
class ooXCellRange : public CComDispatchDriver
{
public:
	ooXCellRange& operator=(IDispatch* rhs)
	{
		CComDispatchDriver::operator=(rhs);
		return *this;
	}

	CComDispatchDriver getColumns()
	{
		CComDispatchDriver columns;
		if (*this)
		{
			// XTableColumns
			CComVariant result;
			if (SUCCEEDED(Invoke0(L"getColumns", &result)))
				columns = result.pdispVal;
		}
		return columns;
	}

	bool setValue(double val)
	{
		if (!*this)
			return false;
		CComVariant var(val);
		CComVariant result;
		return SUCCEEDED(Invoke1(L"setValue", &var, &result));
	}

	bool setFormula(CString const& val)
	{
		if (!*this)
			return false;
		CComVariant var(val);
		CComVariant result;
		return SUCCEEDED(Invoke1(L"setFormula", &var, &result));
	}

	bool setDataArray(COleSafeArray& arr)
	{
		if (!*this)
			return false;
		CComVariant values(arr);
		arr.Detach();
		CComVariant result;
		return SUCCEEDED(Invoke1(L"setDataArray", &values, &result));
	}
};

// XSheetCellCursor
class ooXSheetCellCursor : public ooXCellRange
{
public:
	ooXSheetCellCursor& operator=(IDispatch* rhs)
	{
		CComDispatchDriver::operator=(rhs);
		return *this;
	}

	bool gotoStartOfUsedArea(bool b)
	{
		CComVariant param(b);
		CComVariant result;
		if (!SUCCEEDED(Invoke1(L"gotoStartOfUsedArea", &param, &result)))
			return false;
		return true;
	}

	bool gotoEndOfUsedArea(bool b)
	{
		CComVariant param(b);
		CComVariant result;
		if (!SUCCEEDED(Invoke1(L"gotoEndOfUsedArea", &param, &result)))
			return false;
		return true;
	}

	COleSafeArray getDataArray()
	{
		COleSafeArray data;
		VARIANT var;
		if (SUCCEEDED(Invoke0(L"getDataArray", &var)))
			data.Attach(var);
		return data;
	}
};

class CComWorksheetDriver : public CComDispatchDriver
{
public:
	CComWorksheetDriver& operator=(IDispatch* rhs)
	{
		CComDispatchDriver::operator=(rhs);
		return *this;
	}

	ooXSheetCellCursor createCursor()
	{
		ooXSheetCellCursor cursor;
		if (*this)
		{
			CComVariant result;
			if (SUCCEEDED(Invoke0(L"createCursor", &result)))
				cursor = result.pdispVal;
		}
		return cursor;
	}

	ooXCellRange getCellByPosition(long nCol, long nRow)
	{
		ooXCellRange range;
		if (*this)
		{
			CComVariant col(nCol);
			CComVariant row(nRow);
			CComVariant result;
			if (SUCCEEDED(Invoke2(L"getCellByPosition", &col, &row, &result)))
				range = result.pdispVal;
		}
		return range;
	}

	ooXCellRange getCellRangeByPosition(long left, long top, long right, long bottom)
	{
		ooXCellRange range;
		if (*this)
		{
			VARIANT params[4];
			for (int i = 0; i < 4; ++i)
				VariantInit(&params[i]);
			params[3].vt = VT_I4; //left
			params[3].lVal = left;
			params[2].vt = VT_I4; //top
			params[2].lVal = top;
			params[1].vt = VT_I4; //right
			params[1].lVal = right;
			params[0].vt = VT_I4; //bottom
			params[0].lVal = bottom;

			CComVariant result;
			if (SUCCEEDED(InvokeN(L"getCellRangeByPosition", params, 4, &result)))
				range = result.pdispVal;
		}
		return range;
	}
};

class CComDocumentDriver : public CComDispatchDriver
{
public:
	CComDocumentDriver& operator=(IDispatch* rhs)
	{
		CComDispatchDriver::operator=(rhs);
		return *this;
	}

	CComWorksheetDriver getSheet(int index)
	{
		CComWorksheetDriver worksheet;
		if (*this)
		{
			CComVariant result;
			HRESULT hr = Invoke0(L"getSheets", &result);
			if (SUCCEEDED(hr))
			{
				// XSpreadsheets
				CComDispatchDriver sheets = result.pdispVal;
				CComVariant sheet(index);
				hr = sheets.Invoke1(L"getByIndex", &sheet, &result);
				if (SUCCEEDED(hr))
				{
					// XSpreadsheet
					worksheet = result.pdispVal;
				}
			}
		}
		return worksheet;
	}

	void dispose()
	{
		if (*this)
		{
			CComVariant result;
			Invoke0(L"dispose", &result);
		}
	}
};

class CComDesktopDriver : public CComDispatchDriver
{
public:
	CComDesktopDriver& operator=(IDispatch* rhs)
	{
		CComDispatchDriver::operator=(rhs);
		return *this;
	}

	CComDocumentDriver loadComponentFromURL(CComVariant param1)
	{
		CComDocumentDriver document;
		if (*this)
		{
			CComVariant param2(L"_blank");
			VARIANT params[4];
			int i;
			for (i = 0; i < 4; ++i)
				VariantInit(&params[i]);
			params[3].vt = param1.vt;
			params[3].bstrVal = param1.bstrVal;
			params[2].vt = param1.vt;
			params[2].bstrVal = param2.bstrVal;
			params[1].vt = VT_I4;
			params[1].lVal = 0;
			params[0].vt = VT_ARRAY | VT_VARIANT;
			params[0].parray = NULL;
			CComVariant result;
			HRESULT hr = InvokeN(L"loadComponentFromURL", params, 4, &result);
			if (SUCCEEDED(hr))
			{
				// ::com::sun::star::sheet::
				// XSpreadsheetDocument
				document = result.pdispVal;
			}
			for (i = 0; i < 4; ++i)
				VariantClear(&params[i]);
		}
		return document;
	}
};

class CComManagerDriver : public CComDispatchDriver
{
public:
	CComManagerDriver()
	{
	}

	CComManagerDriver& operator=(IDispatch* rhs)
	{
		CComDispatchDriver::operator=(rhs);
		return *this;
	}

	CComDesktopDriver createDesktop()
	{
		CComDesktopDriver desktop;
		CComVariant param1(L"com.sun.star.frame.Desktop");
		CComVariant result;
		if (SUCCEEDED(Invoke1(L"createInstance", &param1, &result)))
		{
			desktop = result.pdispVal;
		}
		if (!desktop)
		{
			Release();
		}
		return desktop;
	}
};

};
