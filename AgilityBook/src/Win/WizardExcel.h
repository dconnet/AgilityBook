#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief Import/Export Wizard for Excel
 * @author David Connet
 *
 * Revision History
 * @li 2009-07-24 DRC Removed option to export by array.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2004-09-30 DRC Created
 */

#include <vector>
class IDlgProgress;
class IWizardExporter;
typedef std::tr1::shared_ptr<IWizardExporter> IWizardExporterPtr;
class IWizardImporter;
typedef std::tr1::shared_ptr<IWizardImporter> IWizardImporterPtr;
class IWizardSpreadSheet;
typedef std::tr1::shared_ptr<IWizardSpreadSheet> IWizardSpreadSheetPtr;

/**
 * Interface for exporting to a spreadsheet.
 */
class IWizardExporter
{
public:
	virtual ~IWizardExporter() = 0;

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
			wxString const& inData,
			bool bFormula = false) = 0;

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
