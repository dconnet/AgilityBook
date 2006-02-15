/*
 * Copyright © 2003-2006 David Connet. All Rights Reserved.
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
 * @brief Import Wizard
 * @author David Connet
 *
 * Revision History
 * @li 2005-08-08 DRC Added validation during import to make sure names are
 *                    correct.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-10 DRC Only sort runs one way, the UI handles everything else.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-05-10 DRC Process quoted fields in input (like Excel).
 * @li 2004-04-15 DRC If imported data contained a header that we parsed as
 *                    data, the program faulted trying to release a null ptr.
 * @li 2004-03-22 DRC Fixed line/col number reporting (off by one)
 * @li 2004-01-04 DRC Implemented import, except for trials/runs.
 * @li 2003-12-10 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "WizardImport.h"
#include <fstream>

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "DlgAssignColumns.h"
#include "DlgMessage.h"
#include "DlgProgress.h"
#include "Element.h"
#include "Wizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizardImport property page

IMPLEMENT_DYNAMIC(CWizardImport, CDlgBasePropertyPage)

CWizardImport::CWizardImport(
		CWizard* pSheet,
		CAgilityBookDoc* pDoc)
	: CDlgBasePropertyPage(CWizardImport::IDD)
	, m_ctrlPreview(false)
	, m_pSheet(pSheet)
	, m_pDoc(pDoc)
{
	//{{AFX_DATA_INIT(CWizardImport)
	m_Row = CAgilityBookOptions::GetImportStartRow();
	m_Delim = -1;
	m_Delimiter = _T(":");
	//}}AFX_DATA_INIT
	int delim;
	CAgilityBookOptions::GetImportExportDelimiters(true, delim, m_Delimiter);
	switch (delim)
	{
	default:
	case CAgilityBookOptions::eDelimTab:		m_Delim = 0; break;
	case CAgilityBookOptions::eDelimSpace:		m_Delim = 1; break;
	case CAgilityBookOptions::eDelimColon:		m_Delim = 2; break;
	case CAgilityBookOptions::eDelimSemicolon:	m_Delim = 3; break;
	case CAgilityBookOptions::eDelimComma:		m_Delim = 4; break;
	case CAgilityBookOptions::eDelimOther:		m_Delim = 5; break;
	}
}

CWizardImport::~CWizardImport()
{
}

void CWizardImport::ResetData()
{
	m_FileName.Empty();
	m_FileData.RemoveAll();
	m_ExcelData.clear();
}

void CWizardImport::DoDataExchange(CDataExchange* pDX)
{
	CDlgBasePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardImport)
	DDX_Text(pDX, IDC_WIZARD_IMPORT_ROW, m_Row);
	DDV_MinMaxInt(pDX, m_Row, 1, 100);
	DDX_Control(pDX, IDC_WIZARD_IMPORT_ROW_SPIN, m_ctrlSpin);
	DDX_Radio(pDX, IDC_WIZARD_IMPORT_DELIM_TAB, m_Delim);
	DDX_Text(pDX, IDC_WIZARD_IMPORT_DELIM, m_Delimiter);
	DDV_MaxChars(pDX, m_Delimiter, 1);
	DDX_Control(pDX, IDC_WIZARD_IMPORT_ASSIGN, m_ctrlAssign);
	DDX_Control(pDX, IDC_WIZARD_IMPORT_DATE, m_ctrlDateFormat);
	DDX_Control(pDX, IDC_WIZARD_IMPORT_PREVIEW_FILE, m_ctrlPreviewFile);
	DDX_Control(pDX, IDC_WIZARD_IMPORT_PREVIEW, m_ctrlPreview);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWizardImport, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CWizardImport)
	ON_EN_KILLFOCUS(IDC_WIZARD_IMPORT_ROW, OnImportKillFocus)
	ON_NOTIFY(UDN_DELTAPOS, IDC_WIZARD_IMPORT_ROW_SPIN, OnDeltaposImportRowSpin)
	ON_BN_CLICKED(IDC_WIZARD_IMPORT_ASSIGN, OnImportAssign)
	ON_BN_CLICKED(IDC_WIZARD_IMPORT_FILE, OnImportFile)
	ON_BN_CLICKED(IDC_WIZARD_IMPORT_DELIM_TAB, OnImportDelim)
	ON_BN_CLICKED(IDC_WIZARD_IMPORT_DELIM_SPACE, OnImportDelim)
	ON_BN_CLICKED(IDC_WIZARD_IMPORT_DELIM_COLON, OnImportDelim)
	ON_BN_CLICKED(IDC_WIZARD_IMPORT_DELIM_SEMI, OnImportDelim)
	ON_BN_CLICKED(IDC_WIZARD_IMPORT_DELIM_COMMA, OnImportDelim)
	ON_BN_CLICKED(IDC_WIZARD_IMPORT_DELIM_OTHER, OnImportDelim)
	ON_EN_CHANGE(IDC_WIZARD_IMPORT_DELIM, OnImportDelim)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CAgilityBookOptions::ColumnOrder CWizardImport::GetColumnInfo() const
{
	CAgilityBookOptions::ColumnOrder order = CAgilityBookOptions::eUnknown;
	switch (m_pSheet->GetImportExportItem())
	{
	default: break;
	case WIZ_IMPORT_RUNS: order = CAgilityBookOptions::eRunsImport; break;
	case WIZ_IMPORT_CALENDAR: order = CAgilityBookOptions::eCalImport; break;
	case WIZ_IMPORT_LOG: order = CAgilityBookOptions::eLogImport; break;
	}
	return order;
}

CString CWizardImport::GetDelim() const
{
	CString delim;
	switch (m_Delim)
	{
	default: break;
	case 0: delim = _T("\t"); break;
	case 1: delim = _T(" "); break;
	case 2: delim = _T(":"); break;
	case 3: delim = _T(";"); break;
	case 4: delim = _T(","); break;
	case 5: delim = m_Delimiter; break;
	}
	return delim;
}

void CWizardImport::UpdateButtons()
{
	DWORD dwWiz = PSWIZB_BACK;
	// Some test to make sure things are ready
	bool bOk = false;
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle())
		bOk = true;
	else if (1 == GetDelim().GetLength())
		bOk = true;
	BOOL bEnable = FALSE;
	CAgilityBookOptions::ColumnOrder order = GetColumnInfo();
	if (CAgilityBookOptions::eUnknown != order)
	{
		bEnable = TRUE;
		if (bOk)
		{
			for (size_t i = 0; bOk && i < IO_TYPE_MAX; ++i)
			{
				std::vector<int> columns;
				if (CDlgAssignColumns::GetColumnOrder(order, i, columns))
				{
					if (0 == columns.size())
						bOk = false;
				}
			}
		}
	}
	int nCmdShow = SW_SHOW;
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle())
		nCmdShow = SW_HIDE;
    GetDlgItem(IDC_WIZARD_IMPORT_DELIM_GROUP)->ShowWindow(nCmdShow);
    GetDlgItem(IDC_WIZARD_IMPORT_DELIM_TAB)->ShowWindow(nCmdShow);
    GetDlgItem(IDC_WIZARD_IMPORT_DELIM_SPACE)->ShowWindow(nCmdShow);
    GetDlgItem(IDC_WIZARD_IMPORT_DELIM_COLON)->ShowWindow(nCmdShow);
    GetDlgItem(IDC_WIZARD_IMPORT_DELIM_SEMI)->ShowWindow(nCmdShow);
    GetDlgItem(IDC_WIZARD_IMPORT_DELIM_COMMA)->ShowWindow(nCmdShow);
    GetDlgItem(IDC_WIZARD_IMPORT_DELIM_OTHER)->ShowWindow(nCmdShow);
    GetDlgItem(IDC_WIZARD_IMPORT_DELIM)->ShowWindow(nCmdShow);
	m_ctrlAssign.EnableWindow(bEnable);
	dwWiz |= (bOk ? PSWIZB_FINISH : PSWIZB_DISABLEDFINISH);
	m_pSheet->SetWizardButtons(dwWiz);
}

static bool GetField(
		const CString& delim,
		CString& ioStr,
		CString& outFld,
		int curFld,
		INT_PTR maxFlds)
{
	outFld.Empty();
	if (ioStr.IsEmpty())
		return false;

	ASSERT(delim.GetLength() == 1);

	int pos;
	int nLen = ioStr.GetLength();
	int quoteCount = 0;
	for (pos = 0; pos < nLen; ++pos)
	{
		if ('"' == ioStr[pos])
			++quoteCount;
		else
			break;
	}
	bool inQuote = false;
	if (quoteCount % 2 == 1)
	{
		inQuote = true;
		pos = 1;
	}
	else
		pos = 0;

	for (; pos < nLen; ++pos)
	{
		if (!inQuote && curFld < maxFlds - 1 && delim[0] == ioStr[pos])
		{
			ioStr = ioStr.Mid(pos+1);
			break;
		}
		if ('"' == ioStr[pos])
		{
			if (pos < nLen + 1 && '"' == ioStr[pos+1])
			{
				outFld += ioStr[pos];
				++pos;
			}
			else
				inQuote = false;
		}
		else
			outFld += ioStr[pos];
	}
	if (curFld >= maxFlds - 1)
		ioStr.Empty();
	return true;
}

void CWizardImport::UpdatePreview()
{
	CWaitCursor wait;
	m_ctrlPreview.SetRedraw(FALSE);

	// Clear existing preview data.
	m_ctrlPreview.DeleteAllItems();
	int nColumnCount = m_ctrlPreview.GetHeaderCtrl()->GetItemCount();
	int iCol;
	for (iCol = 0; iCol < nColumnCount; ++iCol)
		m_ctrlPreview.DeleteColumn(0);

	CString delim = GetDelim();
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle())
	{
	}
	else if (1 != delim.GetLength())
	{
		m_ctrlPreview.SetRedraw(TRUE);
		m_ctrlPreview.Invalidate();
		return;
	}

	// Get export columns.
	CAgilityBookOptions::ColumnOrder order = GetColumnInfo();
	size_t index;
	std::vector<int> columns[IO_TYPE_MAX];
	for (index = 0; index < IO_TYPE_MAX; ++index)
	{
		CDlgAssignColumns::GetColumnOrder(order, index, columns[index]);
	}

	// Now generate the header information.
	CStringArray cols;
	switch (m_pSheet->GetImportExportItem())
	{
	default: break;
	case WIZ_IMPORT_RUNS:
		for (index = 0; index < IO_TYPE_MAX; ++index)
		{
			if (0 == columns[index].size())
				continue;
			for (iCol = 0; iCol < static_cast<int>(columns[index].size()); ++iCol)
			{
				CString str = CDlgAssignColumns::GetNameFromColumnID(columns[index][iCol]);
				if (iCol >= cols.GetSize())
					cols.Add(str);
				else
				{
					if (cols[iCol] != str && 0 < str.GetLength())
						cols[iCol] += _T("/") + str;
				}
			}
		}
		break;
	case WIZ_IMPORT_CALENDAR:
		for (index = 0; index < columns[IO_TYPE_CALENDAR].size(); ++index)
		{
			cols.Add(CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_CALENDAR][index]));
		}
		break;
	case WIZ_IMPORT_LOG:
		for (index = 0; index < columns[IO_TYPE_TRAINING].size(); ++index)
		{
			cols.Add(CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_TRAINING][index]));
		}
		break;
	}
	LV_COLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM;
	for (iCol = 0; iCol < cols.GetSize(); ++iCol)
	{
		CString str = cols[iCol];
		col.fmt = LVCFMT_LEFT;
		col.pszText = str.GetBuffer(0);
		col.iSubItem = iCol;
		m_ctrlPreview.InsertColumn(iCol, &col);
	}

	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle())
	{
		int iLine = 0;
		for (std::vector< std::vector<CString> >::const_iterator iter = m_ExcelData.begin();
			iter != m_ExcelData.end();
			++iter, ++iLine)
		{
			if (iLine >= m_Row - 1)
			{
				std::vector<CString> const& rowData = *iter;
				int iCol = 0;
				int iCurLine = -1;
				for (std::vector<CString>::const_iterator iter2 = rowData.begin();
					iter2 != rowData.end() && iCol < cols.GetSize();
					++iter2, ++iCol)
				{
					CString const& str = *iter2;
					if (0 == iCol)
						iCurLine = m_ctrlPreview.InsertItem(iLine, str);
					else
						m_ctrlPreview.SetItemText(iCurLine, iCol, str);
				}
			}
		}
	}
	else
	{
		for (int iLine = m_Row - 1; iLine < m_FileData.GetSize(); ++iLine)
		{
			CString str = m_FileData[iLine];
			CString fld;
			iCol = 0;
			while (GetField(delim, str, fld, iCol, cols.GetSize()))
			{
				if (0 == iCol)
					m_ctrlPreview.InsertItem(iLine-(m_Row-1), fld);
				else
					m_ctrlPreview.SetItemText(iLine-(m_Row-1), iCol, fld);
				++iCol;
			}
		}
	}
	for (iCol = 0; iCol < cols.GetSize(); ++iCol)
		m_ctrlPreview.SetColumnWidth(iCol, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlPreview.SetRedraw(TRUE);
	m_ctrlPreview.Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// CWizardImport message handlers

BOOL CWizardImport::OnInitDialog() 
{
	CDlgBasePropertyPage::OnInitDialog();
	m_ctrlPreview.SetExtendedStyle(m_ctrlPreview.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_ctrlSpin.SetRange(100, 1);
	static struct
	{
		TCHAR const* pFormat;
		ARBDate::DateFormat format;
	} const sc_Dates[] =
	{
		{_T("MM-DD-YYYY"), ARBDate::eDashMMDDYYYY},
		{_T("MM/DD/YYYY"), ARBDate::eSlashMMDDYYYY},
		{_T("YYYY-MM-DD"), ARBDate::eDashYYYYMMDD},
		{_T("YYYY/MM/DD"), ARBDate::eSlashYYYYMMDD},
		{_T("DD-MM-YYYY"), ARBDate::eDashDDMMYYYY},
		{_T("DD/MM/YYYY"), ARBDate::eSlashDDMMYYYY},
	};
	ARBDate::DateFormat format;
	CAgilityBookOptions::GetImportExportDateFormat(true, format);
	static int const sc_nDates = sizeof(sc_Dates) / sizeof(sc_Dates[0]);
	for (int i = 0; i < sc_nDates; ++i)
	{
		int index = m_ctrlDateFormat.AddString(sc_Dates[i].pFormat);
		m_ctrlDateFormat.SetItemData(index, static_cast<DWORD>(sc_Dates[i].format));
		if (sc_Dates[i].format == format)
			m_ctrlDateFormat.SetCurSel(index);
	}
	UpdatePreview();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWizardImport::OnSetActive() 
{
	UpdateButtons();
	BOOL rc = CDlgBasePropertyPage::OnSetActive();
	if (m_FileName.IsEmpty())
		OnImportFile();
	return rc;
}

LRESULT CWizardImport::OnWizardBack()
{
	return IDD_WIZARD_START;
}

static ARBDogRunPtr CreateRun(
		ARBDogRunPtr pRun,
		ARBConfigScoringPtr pScoring)
{
	if (!pRun)
	{
		pRun = ARBDogRunPtr(ARBDogRun::New());
		pRun->GetScoring().SetType(ARBDogRunScoring::TranslateConfigScoring(pScoring->GetScoringStyle()),
			pScoring->DropFractions());
	}
	return pRun;
}

static ARBString GetPrimaryVenue(ARBString const& venues)
{
	ARBString venue;
	ARBString::size_type pos = venues.find('/');
	if (ARBString::npos != pos)
		venue = venues.substr(0, pos);
	else
		venue = venues;
	return venue;
}

static void BreakLine(
		char inSep,
		ARBString inStr,
		std::vector<ARBString>& outFields)
{
	outFields.clear();
	ARBString::size_type pos = inStr.find(inSep);
	while (ARBString::npos != pos)
	{
		outFields.push_back(inStr.substr(0, pos));
		inStr = inStr.substr(pos+1);
		pos = inStr.find(inSep);
	}
	outFields.push_back(inStr);
}

static ARBCalendarPtr CreateCal(ARBCalendarPtr pCal)
{
	if (!pCal)
	{
		pCal = ARBCalendarPtr(ARBCalendar::New());
	}
	return pCal;
}

static ARBTrainingPtr CreateLog(ARBTrainingPtr pLog)
{
	if (!pLog)
	{
		pLog = ARBTrainingPtr(ARBTraining::New());
		pLog->SetDate(ARBDate::Today());
	}
	return pLog;
}

BOOL CWizardImport::OnWizardFinish() 
{
	if (!UpdateData(TRUE))
		return FALSE;
	int index = m_ctrlDateFormat.GetCurSel();
	if (CB_ERR == index)
	{
		AfxMessageBox(_T("Please select a date format"));
		GotoDlgCtrl(GetDlgItem(IDC_WIZARD_IMPORT_DATE));
		return FALSE;
	}
	CAgilityBookOptions::SetImportStartRow(m_Row);
	if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle())
	{
		int delim;
		switch (m_Delim)
		{
		default:
		case 0: delim = CAgilityBookOptions::eDelimTab; break;
		case 1: delim = CAgilityBookOptions::eDelimSpace; break;
		case 2: delim = CAgilityBookOptions::eDelimColon; break;
		case 3: delim = CAgilityBookOptions::eDelimSemicolon; break;
		case 4: delim = CAgilityBookOptions::eDelimComma; break;
		case 5: delim = CAgilityBookOptions::eDelimOther; break;
		}
		CAgilityBookOptions::SetImportExportDelimiters(true, delim, m_Delimiter);
	}
	ARBDate::DateFormat format = static_cast<ARBDate::DateFormat>(m_ctrlDateFormat.GetItemData(index));
	CAgilityBookOptions::SetImportExportDateFormat(true, format);

	CWaitCursor wait;
	CAgilityBookOptions::ColumnOrder order = GetColumnInfo();
	size_t iCol;
	std::vector<int> columns[IO_TYPE_MAX];
	for (iCol = 0; iCol < IO_TYPE_MAX; ++iCol)
	{
		CDlgAssignColumns::GetColumnOrder(order, iCol, columns[iCol]);
	}
	ARBostringstream errLog;
	int nAdded = 0;
	int nDuplicate = 0;
	int nSkipped = 0;
	int nColumns = m_ctrlPreview.GetHeaderCtrl()->GetItemCount();
	for (int nItem = 0; nItem < m_ctrlPreview.GetItemCount(); ++nItem)
	{
		std::vector<ARBString> entry;
		entry.reserve(nColumns);
		for (int i = 0; i < nColumns; ++i)
		{
			CString str = m_ctrlPreview.GetItemText(nItem, i);
			str.Replace(_T("\r\n"), _T("\n"));
			entry.push_back((LPCTSTR)str);
		}
		switch (m_pSheet->GetImportExportItem())
		{
		default: break;

		case WIZ_IMPORT_RUNS:
			{
				int const colIdx[4] =
				{
					IO_TYPE_RUNS_FAULTS_TIME,
					IO_TYPE_RUNS_TIME_FAULTS,
					IO_TYPE_RUNS_OPEN_CLOSE,
					IO_TYPE_RUNS_POINTS
				};
				int idxDate[4];
				int idxVenue[4];
				int idxClub[4];
				int idxDiv[4];
				int idxLevel[4];
				int idxEvent[4];
				int i;
				for (i = 0; i < 4; ++i)
				{
					idxDate[i] = idxVenue[i] = idxClub[i] = idxDiv[i] = idxLevel[i] = idxEvent[i] = -1;
				}
				for (i = 0; i < 4; ++i)
				{
					for (iCol = 0; iCol < columns[colIdx[i]].size(); ++iCol)
					{
						if (IO_RUNS_DATE == columns[colIdx[i]][iCol])
							idxDate[i] = static_cast<int>(iCol);
						else if (IO_RUNS_VENUE == columns[colIdx[i]][iCol])
							idxVenue[i] = static_cast<int>(iCol);
						else if (IO_RUNS_CLUB == columns[colIdx[i]][iCol])
							idxClub[i] = static_cast<int>(iCol);
						else if (IO_RUNS_DIVISION == columns[colIdx[i]][iCol])
							idxDiv[i] = static_cast<int>(iCol);
						else if (IO_RUNS_LEVEL == columns[colIdx[i]][iCol])
							idxLevel[i] = static_cast<int>(iCol);
						else if (IO_RUNS_EVENT == columns[colIdx[i]][iCol])
							idxEvent[i] = static_cast<int>(iCol);
					}
				}
				ARBConfigScoringPtr pScoring;
				for (i = 0; !pScoring && i < 4; ++i)
				{
					if (0 <= idxVenue[i] && 0 <= idxEvent[i] && 0 <= idxDiv[i] && 0 <= idxLevel[i] && 0 <= idxDate[i])
					{
						m_pDoc->GetConfig().GetVenues().FindEvent(
							GetPrimaryVenue(entry[idxVenue[i]]),
							entry[idxEvent[i]],
							entry[idxDiv[i]],
							entry[idxLevel[i]],
							ARBDate::FromString(entry[idxDate[i]], format),
							&pScoring);
					}
				}
				// It's conceivable that we could have more than one match.
				// But just don't worry about that... If data from various
				// rows/cols has that much overlap, it's just not worth it.
				if (!pScoring)
				{
					errLog << _T("Warning: Line ")
						<< nItem + 1
						<< _T(": Skipped entry, unable to find a valid configuration entry\n");
					++nSkipped;
					continue;
				}
				i = -1;
				switch (pScoring->GetScoringStyle())
				{
				case ARBConfigScoring::eFaultsThenTime:
				case ARBConfigScoring::eFaults100ThenTime:
				case ARBConfigScoring::eFaults200ThenTime:
					i = IO_TYPE_RUNS_FAULTS_TIME;
					break;
				case ARBConfigScoring::eOCScoreThenTime:
					i = IO_TYPE_RUNS_OPEN_CLOSE;
					break;
				case ARBConfigScoring::eScoreThenTime:
					i = IO_TYPE_RUNS_POINTS;
					break;
				case ARBConfigScoring::eTimePlusFaults:
					i = IO_TYPE_RUNS_TIME_FAULTS;
					break;
				}
				ASSERT(0 <= i);

				ARBString nameReg, nameCall;
				ARBString trialVenue, trialClub, trialLocation, trialNotes;
				ARBDogRunPtr pRun;
				for (iCol = 0; iCol < entry.size() && iCol < columns[i].size(); ++iCol)
				{
					if (0 == entry[iCol].length())
						continue;
					switch (columns[i][iCol])
					{
					case IO_RUNS_REG_NAME:
						nameReg = entry[iCol];
						break;
					case IO_RUNS_CALL_NAME:
						nameCall = entry[iCol];
						break;
					case IO_RUNS_DATE:
						{
							ARBDate date = ARBDate::FromString(entry[iCol], format);
							if (date.IsValid())
							{
								pRun = CreateRun(pRun, pScoring);
								pRun->SetDate(date);
							}
							else
							{
								errLog << _T("ERROR: Line ")
									<< nItem + 1
									<< _T(", Column ")
#if _MSC_VER >= 1300 && _MSC_VER < 1400 // VC7 casting warning
									<< static_cast<UINT>(iCol + 1)
#else
									<< iCol + 1
#endif
									<< _T(": Invalid run date: ")
									<< entry[iCol]
									<< _T("\n");
								if (pRun)
									pRun.reset();
								iCol = columns[i].size();
							}
						}
						break;
					case IO_RUNS_VENUE:
						trialVenue = entry[iCol];
						break;
					case IO_RUNS_CLUB:
						trialClub = entry[iCol];
						break;
					case IO_RUNS_LOCATION:
						trialLocation = entry[iCol];
						break;
					case IO_RUNS_TRIAL_NOTES:
						trialNotes = entry[iCol];
						break;
					case IO_RUNS_DIVISION:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetDivision(entry[iCol]);
						break;
					case IO_RUNS_LEVEL:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetLevel(entry[iCol]);
						break;
					case IO_RUNS_EVENT:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetEvent(entry[iCol]);
						break;
					case IO_RUNS_HEIGHT:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetHeight(entry[iCol]);
						break;
					case IO_RUNS_JUDGE:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetJudge(entry[iCol]);
						break;
					case IO_RUNS_HANDLER:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetHandler(entry[iCol]);
						break;
					case IO_RUNS_CONDITIONS:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetConditions(entry[iCol]);
						break;
					case IO_RUNS_COURSE_FAULTS:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetCourseFaults(static_cast<short>(_tstol(entry[iCol].c_str())));
						break;
					case IO_RUNS_TIME:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetTime(_tcstod(entry[iCol].c_str(), NULL));
						break;
					case IO_RUNS_YARDS:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetYards(_tcstod(entry[iCol].c_str(), NULL));
						break;
					case IO_RUNS_YPS:
						// Computed
						break;
					case IO_RUNS_SCT:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetSCT(_tcstod(entry[iCol].c_str(), NULL));
						break;
					case IO_RUNS_TOTAL_FAULTS:
						// Computed.
						break;
					case IO_RUNS_REQ_OPENING:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetNeedOpenPts(static_cast<short>(_tstol(entry[iCol].c_str())));
						break;
					case IO_RUNS_REQ_CLOSING:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetNeedClosePts(static_cast<short>(_tstol(entry[iCol].c_str())));
						break;
					case IO_RUNS_OPENING:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetOpenPts(static_cast<short>(_tstol(entry[iCol].c_str())));
						break;
					case IO_RUNS_CLOSING:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetClosePts(static_cast<short>(_tstol(entry[iCol].c_str())));
						break;
					case IO_RUNS_REQ_POINTS:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetNeedOpenPts(static_cast<short>(_tstol(entry[iCol].c_str())));
						break;
					case IO_RUNS_POINTS:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetOpenPts(static_cast<short>(_tstol(entry[iCol].c_str())));
						break;
					case IO_RUNS_PLACE:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetPlace(static_cast<short>(_tstol(entry[iCol].c_str())));
						break;
					case IO_RUNS_IN_CLASS:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetInClass(static_cast<short>(_tstol(entry[iCol].c_str())));
						break;
					case IO_RUNS_DOGSQD:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetDogsQd(static_cast<short>(_tstol(entry[iCol].c_str())));
						break;
					case IO_RUNS_Q:
						{
							pRun = CreateRun(pRun, pScoring);
							ARB_Q q;
							if (_T("QQ") == entry[iCol])
								entry[iCol] = _T("Q");
							CErrorCallback err;
							q.Load(entry[iCol], ARBAgilityRecordBook::GetCurrentDocVersion(), err);
							pRun->SetQ(q);
						}
						break;
					case IO_RUNS_SCORE:
						// This is computed.
						break;
					case IO_RUNS_TITLE_POINTS:
						// These are computed.
						break;
					case IO_RUNS_COMMENTS:
					case IO_RUNS_FAULTS:
						{
							pRun = CreateRun(pRun, pScoring);
							ARBString str = pRun->GetNote();
							if (0 < str.length())
								str += _T("\n");
							str += entry[iCol];
							pRun->SetNote(str);
						}
						break;
					}
				}
				pScoring.reset();

				// Verify data: venue is not necessarily valid!
				// When importing multiple types of data and only one type was
				// actually set, the other types will attempt to import because
				// we matched pScoring on that first entry. But when we
				// actually parse the data, the venue name is not what we
				// matched for pScoring! So check v/d/l/e again.
				if (pRun)
				{
					if (!m_pDoc->GetConfig().GetVenues().FindVenue(trialVenue))
					{
						errLog << _T("Warning: Line ")
							<< nItem + 1
							<< _T(": Skipped entry, invalid venue name: ")
							<< trialVenue
							<< _T("\n");
						pRun.reset();
					}
					else if (!m_pDoc->GetConfig().GetVenues().FindEvent(
						trialVenue,
						pRun->GetEvent(),
						pRun->GetDivision(),
						pRun->GetLevel(),
						pRun->GetDate()))
					{
						errLog << _T("Warning: Line ")
							<< nItem + 1
							<< _T(": Skipped entry, unable to find a valid configuration entry\n");
						pRun.reset();
					}
				}

				// Now that we've created a run, we have to put it somewhere.
				if (pRun)
				{
					// Find the dog
					ARBDogPtr pDog = m_pDoc->GetCurrentDog();
					if (0 < nameReg.length() || 0 < nameCall.length())
					{
						pDog.reset();
						for (ARBDogList::iterator iterDog = m_pDoc->GetDogs().begin(); iterDog != m_pDoc->GetDogs().end(); ++iterDog)
						{
							ARBDogPtr pDogTmp = *iterDog;
							if ((0 < nameReg.length() && pDogTmp->GetRegisteredName() == nameReg
							&& 0 < nameCall.length() && pDogTmp->GetCallName() == nameCall)
							|| (0 < nameReg.length() && pDogTmp->GetRegisteredName() == nameReg
							&& 0 == nameCall.length())
							|| (0 == nameReg.length()
							&& 0 < nameCall.length() && pDogTmp->GetCallName() == nameCall))
							{
								pDog = pDogTmp;
								break;
							}
						}
						// Not found, create it.
						if (!pDog)
						{
							pDog = ARBDogPtr(ARBDog::New());
							if (0 < nameReg.length())
								pDog->SetRegisteredName(nameReg);
							if (0 < nameCall.length())
								pDog->SetCallName(nameCall);
							if (0 == nameCall.length() && 0 < nameReg.length())
								pDog->SetCallName(nameReg);
							m_pDoc->GetDogs().AddDog(pDog);
						}
					}
					if (!pDog)
					{
						if (m_pDoc->GetDogs().begin() == m_pDoc->GetDogs().end())
						{
							pDog = ARBDogPtr(ARBDog::New());
							pDog->SetCallName(_T("?"));
							m_pDoc->GetDogs().AddDog(pDog);
						}
						else
							pDog = *(m_pDoc->GetDogs().begin());
					}
					ASSERT(pDog);

					// Find the trial
					std::vector<ARBString> venues;
					std::vector<ARBString> clubs;
					if (0 < trialVenue.length())
						BreakLine('/', trialVenue, venues);
					if (0 < trialClub.length())
						BreakLine('/', trialClub, clubs);
					if (clubs.size() < venues.size())
					{
						// Clubs and venues now agree so we can use them together easily.
						if (0 == clubs.size())
							clubs.push_back(_T("?"));
						while (clubs.size() < venues.size())
							clubs.push_back(clubs[clubs.size()-1]);
					}
					ARBDogTrialPtr pTrial;
					for (ARBDogTrialList::iterator iterTrial = pDog->GetTrials().begin(); iterTrial != pDog->GetTrials().end(); ++iterTrial)
					{
						ARBDogTrialPtr pTrialTmp = *iterTrial;
						if (pTrialTmp->GetClubs().size() == venues.size()
						&& pTrialTmp->GetLocation() == trialLocation
						&& pRun->GetDate().isBetween(pTrialTmp->GetRuns().GetStartDate(), pTrialTmp->GetRuns().GetEndDate()))
						{
							bool bOk = true;
							size_t i = 0;
							for (ARBDogClubList::iterator iterClub = pTrialTmp->GetClubs().begin(); iterClub != pTrialTmp->GetClubs().end(); ++iterClub)
							{
								ARBDogClubPtr pClub(ARBDogClub::New());
								pClub->SetName(clubs[i]);
								pClub->SetVenue(venues[i]);
								if (*pClub != *(*(iterClub)))
								{
									bOk = false;
									break;
								}
								++i;
							}
							if (bOk)
							{
								pTrial = pTrialTmp;
								break;
							}
						}
					}
					if (!pTrial)
					{
						// Couldn't find a trial, so make one.
						pTrial = ARBDogTrialPtr(ARBDogTrial::New());
						pDog->GetTrials().AddTrial(pTrial);
						pDog->GetTrials().sort(true);
						for (size_t i = 0; i < venues.size(); ++i)
						{
							pTrial->GetClubs().AddClub(clubs[i], venues[i]);
						}
						if (0 < trialLocation.length())
							pTrial->SetLocation(trialLocation);
						if (0 < trialNotes.length())
							pTrial->SetNote(trialNotes);
					}
					pTrial->GetRuns().AddRun(pRun);
					pTrial->SetMultiQs(m_pDoc->GetConfig());
					pTrial->GetRuns().sort();
					++nAdded;
				}
				else
				{
					++nSkipped;
				}
			}
			break;

		case WIZ_IMPORT_CALENDAR:
			{
				ARBCalendarPtr pCal;
				for (iCol = 0; iCol < columns[IO_TYPE_CALENDAR].size(); ++iCol)
				{
					if (0 == entry[iCol].length())
						continue;
					switch (columns[IO_TYPE_CALENDAR][iCol])
					{
					case IO_CAL_START_DATE:
						{
							ARBDate date = ARBDate::FromString(entry[iCol], format);
							if (date.IsValid())
							{
								pCal = CreateCal(pCal);
								pCal->SetStartDate(date);
							}
							else
							{
								errLog << _T("ERROR: Line ")
									<< nItem + 1
									<< _T(", Column ")
#if _MSC_VER >= 1300 && _MSC_VER < 1400 // VC7 casting warning
									<< static_cast<UINT>(iCol + 1)
#else
									<< iCol + 1
#endif
									<< _T(": Invalid calendar start date: ")
									<< entry[iCol]
									<< _T("\n");
								if (pCal)
									pCal.reset();
								iCol = columns[IO_TYPE_CALENDAR].size();
							}
						}
						break;
					case IO_CAL_END_DATE:
						{
							ARBDate date = ARBDate::FromString(entry[iCol], format);
							if (date.IsValid())
							{
								pCal = CreateCal(pCal);
								pCal->SetEndDate(date);
							}
							else
							{
								errLog << _T("ERROR: Line ")
									<< nItem + 1
									<< _T(", Column ")
#if _MSC_VER >= 1300 && _MSC_VER < 1400 // VC7 casting warning
									<< static_cast<UINT>(iCol + 1)
#else
									<< iCol + 1
#endif
									<< _T(": Invalid calendar end date: ")
									<< entry[iCol]
									<< _T("\n");
								if (pCal)
									pCal.reset();
								iCol = columns[IO_TYPE_CALENDAR].size();
							}
						}
						break;
					case IO_CAL_TENTATIVE:
						pCal = CreateCal(pCal);
						pCal->SetIsTentative((_T("?") == entry[iCol] || _T("y") == entry[iCol] || _T("Y") == entry[iCol]));
						break;
					case IO_CAL_ENTERED:
						if (_T("N") == entry[iCol])
						{
							pCal = CreateCal(pCal);
							pCal->SetEntered(ARBCalendar::eNot);
						}
						else if (_T("P") == entry[iCol] || _T("Planning") == entry[iCol])
						{
							pCal = CreateCal(pCal);
							pCal->SetEntered(ARBCalendar::ePlanning);
						}
						else if (_T("E") == entry[iCol] || _T("Entered") == entry[iCol])
						{
							pCal = CreateCal(pCal);
							pCal->SetEntered(ARBCalendar::eEntered);
						}
						else
						{
							errLog << _T("ERROR: Line ")
								<< nItem + 1
								<< _T(", Column ")
#if _MSC_VER >= 1300 && _MSC_VER < 1400 // VC7 casting warning
								<< static_cast<UINT>(iCol + 1)
#else
								<< iCol + 1
#endif
								<< _T(": Invalid calendar entered value: ")
								<< entry[iCol]
								<< _T(" [N, P or E]\n");
							if (pCal)
								pCal.reset();
							iCol = columns[IO_TYPE_CALENDAR].size();
						}
						break;
					case IO_CAL_LOCATION:
						pCal = CreateCal(pCal);
						pCal->SetLocation(entry[iCol]);
						break;
					case IO_CAL_CLUB:
						pCal = CreateCal(pCal);
						pCal->SetClub(entry[iCol]);
						break;
					case IO_CAL_VENUE:
						pCal = CreateCal(pCal);
						pCal->SetVenue(entry[iCol]);
						break;
					case IO_CAL_OPENS:
						{
							ARBDate date = ARBDate::FromString(entry[iCol], format);
							if (date.IsValid())
							{
								pCal = CreateCal(pCal);
								pCal->SetOpeningDate(date);
							}
							else
							{
								errLog << _T("ERROR: Line ")
									<< nItem + 1
									<< _T(", Column ")
#if _MSC_VER >= 1300 && _MSC_VER < 1400 // VC7 casting warning
									<< static_cast<UINT>(iCol + 1)
#else
									<< iCol + 1
#endif
									<< _T(": Invalid calendar opening date: ")
									<< entry[iCol]
									<< _T("\n");
								if (pCal)
									pCal.reset();
								iCol = columns[IO_TYPE_CALENDAR].size();
							}
						}
						break;
					case IO_CAL_CLOSES:
						{
							ARBDate date = ARBDate::FromString(entry[iCol], format);
							if (date.IsValid())
							{
								pCal = CreateCal(pCal);
								pCal->SetClosingDate(date);
							}
							else
							{
								errLog << _T("ERROR: Line ")
									<< nItem + 1
									<< _T(", Column ")
#if _MSC_VER >= 1300 && _MSC_VER < 1400 // VC7 casting warning
									<< static_cast<UINT>(iCol + 1)
#else
									<< iCol + 1
#endif
									<< _T(": Invalid calendar closing date: ")
									<< entry[iCol]
									<< _T("\n");
								if (pCal)
									pCal.reset();
								iCol = columns[IO_TYPE_CALENDAR].size();
							}
						}
						break;
					case IO_CAL_NOTES:
						pCal = CreateCal(pCal);
						pCal->SetNote(entry[iCol]);
						break;
					}
				}
				if (pCal)
				{
					if (!m_pDoc->GetCalendar().FindCalendar(pCal))
					{
						m_pDoc->GetCalendar().AddCalendar(pCal);
						m_pDoc->GetCalendar().sort();
						++nAdded;
					}
					else
						++nDuplicate;
				}
				else
					++nSkipped;
			}
			break;

		case WIZ_IMPORT_LOG:
			{
				ARBTrainingPtr pLog;
				for (iCol = 0; iCol < entry.size() && iCol < columns[IO_TYPE_TRAINING].size(); ++iCol)
				{
					if (0 == entry[iCol].length())
						continue;
					switch (columns[IO_TYPE_TRAINING][iCol])
					{
					case IO_LOG_DATE:
						{
							ARBDate date = ARBDate::FromString(entry[iCol], format);
							if (date.IsValid())
							{
								pLog = CreateLog(pLog);
								pLog->SetDate(date);
							}
							else
							{
								errLog << _T("ERROR: Line ")
									<< nItem + 1
									<< _T(", Column ")
#if _MSC_VER >= 1300 && _MSC_VER < 1400 // VC7 casting warning
									<< static_cast<UINT>(iCol + 1)
#else
									<< iCol + 1
#endif
									<< _T(": Invalid training log date: ")
									<< entry[iCol]
									<< _T("\n");
								if (pLog)
									pLog.reset();
								iCol = columns[IO_TYPE_TRAINING].size();
							}
						}
						break;
					case IO_LOG_NAME:
						pLog = CreateLog(pLog);
						pLog->SetName(entry[iCol]);
						break;
					case IO_LOG_SUBNAME:
						pLog = CreateLog(pLog);
						pLog->SetSubName(entry[iCol]);
						break;
					case IO_LOG_NOTES:
						pLog = CreateLog(pLog);
						pLog->SetNote(entry[iCol]);
						break;
					}
				}
				if (pLog)
				{
					if (!m_pDoc->GetTraining().FindTraining(pLog))
					{
						m_pDoc->GetTraining().AddTraining(pLog);
						m_pDoc->GetTraining().sort();
						++nAdded;
					}
					else
						++nDuplicate;
				}
				else
					++nSkipped;
			}
			break;
		}
	}
	if (0 < errLog.tellp())
		errLog << _T("\n");
	errLog << nAdded << _T(" entries added\n")
		<< nDuplicate << _T(" duplicates (not added)\n")
		<< nSkipped << _T(" entries skipped");
	CDlgMessage dlg(errLog.str().c_str(), 0, this);
	dlg.DoModal();
	if (0 < nAdded)
	{
		m_pDoc->SetModifiedFlag(TRUE);
		m_pDoc->UpdateAllViews(NULL, UPDATE_ALL_VIEW);
	}

	return CDlgBasePropertyPage::OnWizardFinish();
}

void CWizardImport::OnImportKillFocus()
{
	if (!UpdateData(TRUE))
		return;
	int row = m_Row;
	if (row != m_Row)
		UpdatePreview();
}

void CWizardImport::OnDeltaposImportRowSpin(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = reinterpret_cast<NM_UPDOWN*>(pNMHDR);
	// There is no "changed" msg, only the "changing".
	// So we'll just update the ddx variable ourselves.
	if (m_Row != pNMUpDown->iPos + pNMUpDown->iDelta
	&& 0 < pNMUpDown->iPos + pNMUpDown->iDelta)
	{
		m_Row = pNMUpDown->iPos + pNMUpDown->iDelta;
		UpdatePreview();
	}
	*pResult = 0;
}

void CWizardImport::OnImportDelim() 
{
	if (!UpdateData(TRUE))
		return;
	UpdateButtons();
	UpdatePreview();
}

void CWizardImport::OnImportAssign() 
{
	if (!UpdateData(TRUE))
		return;
	CAgilityBookOptions::ColumnOrder order = GetColumnInfo();
	if (CAgilityBookOptions::eUnknown != order)
	{
		CDlgAssignColumns dlg(order, this);
		if (IDOK == dlg.DoModal())
		{
			UpdateButtons();
			UpdatePreview();
		}
	}
}

void CWizardImport::OnImportFile() 
{
	if (!UpdateData(TRUE))
		return;
	CString filter;
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle())
		filter.LoadString(IDS_FILEEXT_FILTER_XLS);
	else
		filter.LoadString(IDS_FILEEXT_FILTER_TXT);
	CFileDialog file(TRUE, _T(""), _T(""), OFN_FILEMUSTEXIST, filter, this);
	if (IDOK == file.DoModal())
	{
		m_FileName = file.GetPathName();
		CString str;
		str.FormatMessage(IDS_FILE_PREVIEW, (LPCTSTR)m_FileName);
		m_ctrlPreviewFile.SetWindowText(str);
		CWaitCursor wait;
		m_FileData.RemoveAll();
		m_ExcelData.clear();
		if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle())
		{
			CWizardExcelImport* pImporter = m_pSheet->ExcelHelper().GetImporter();
			if (pImporter)
			{
				IDlgProgress* pProgress = IDlgProgress::CreateProgress(this);
				if (pImporter->OpenFile(m_FileName))
				{
					pImporter->GetData(m_ExcelData, pProgress);
				}
				pProgress->Dismiss();
				delete pImporter;
			}
		}
		else
		{
			CStdioFile file;
			if (file.Open(m_FileName, CFile::modeRead | CFile::typeText))
			{
				CString str;
				while (file.ReadString(str))
				{
					str.TrimRight();
					m_FileData.Add(str);
				}
				file.Close();
			}
		}
		UpdateButtons();
		UpdatePreview();
	}
}
