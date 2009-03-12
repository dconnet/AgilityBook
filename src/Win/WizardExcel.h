#pragma once

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

#include <vector>
class IDlgProgress;
class IWizardExporter;
typedef tr1::shared_ptr<IWizardExporter> IWizardExporterPtr;
class IWizardImporter;
typedef tr1::shared_ptr<IWizardImporter> IWizardImporterPtr;
class IWizardSpreadSheet;
typedef tr1::shared_ptr<IWizardSpreadSheet> IWizardSpreadSheetPtr;

/**
 * Interface for exporting to a spreadsheet.
 */
class IWizardExporter
{
public:
	virtual ~IWizardExporter() = 0;

	virtual bool ArrayOkay() const = 0;
	virtual bool CreateArray(
			long inRows,
			long inCols) = 0;
	virtual bool InsertArrayData(
			long inRow,
			long inCol,
			wxString const& inData) = 0;
	virtual bool ExportDataArray(
			long inRowTop = 0,
			long inColLeft = 0) = 0;

	virtual bool AllowAccess(bool bAllow) = 0;

	virtual bool SetTextColor(
			long inRow,
			long inCol,
			wxColour inColor) = 0;
	virtual bool SetBackColor(
			long inRow,
			long inCol,
			wxColour inColor) = 0;
	virtual bool SetItalic(
			long inRow,
			long inCol,
			bool bItalic) = 0;
	virtual bool SetBold(
			long inRow,
			long inCol,
			bool bBold) = 0;

	virtual bool InsertData(
			long inRow,
			long inCol,
			double inData) = 0;
	virtual bool InsertData(
			long inRow,
			long inCol,
			wxString const& inData) = 0;

	virtual bool AutoFit(
			long inColFrom,
			long inColTo) = 0;
};

/**
 * Interface for importing from a spreadsheet.
 */
class IWizardImporter
{
public:
	virtual ~IWizardImporter() = 0;
	virtual bool OpenFile(wxString const& inFilename) = 0;
	virtual bool GetData(
			std::vector< std::vector<wxString> >& outData,
			IDlgProgress* ioProgress = NULL) = 0;
};

/////////////////////////////////////////////////////////////////////////////

class IWizardSpreadSheet
{
public:
	typedef enum
	{
		eMicrosoftExcel,
		eOpenOfficeCalc
	} eType;
	/**
	 * Create a new spreadsheet manager, must 'delete' the returned pointer.
	 */
	static IWizardSpreadSheetPtr Create(eType inType);

	/// Get the maximum number of rows Excel can handle.
	static long GetMaxRows();
	/// Get the maximum number of columns Excel can handle.
	static long GetMaxCols();
	/// Translate a given row/col into Excel notation: (0,0) -> "A1"
	static bool GetRowCol(
			long inRow,
			long inCol,
			wxString& outCell);

	virtual ~IWizardSpreadSheet() = 0;
	virtual IWizardExporterPtr GetExporter() const = 0;
	virtual IWizardImporterPtr GetImporter() const = 0;
};
