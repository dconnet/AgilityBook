/*
 * Copyright © 2003-2004 David Connet. All Rights Reserved.
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

CWizardImport::CWizardImport(CWizard* pSheet, CAgilityBookDoc* pDoc)
	: CDlgBasePropertyPage(CWizardImport::IDD)
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
	case 0: delim = "\t"; break;
	case 1: delim = " "; break;
	case 2: delim = ":"; break;
	case 3: delim = ";"; break;
	case 4: delim = ","; break;
	case 5: delim = m_Delimiter; break;
	}
	return delim;
}

void CWizardImport::UpdateButtons()
{
	DWORD dwWiz = PSWIZB_BACK;
	// Some test to make sure things are ready
	bool bOk = false;
	if (1 == GetDelim().GetLength())
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
	m_ctrlAssign.EnableWindow(bEnable);
	dwWiz |= (bOk ? PSWIZB_FINISH : PSWIZB_DISABLEDFINISH);
	m_pSheet->SetWizardButtons(dwWiz);
}

static bool GetField(const CString& delim, CString& ioStr, CString& outFld, int curFld, INT_PTR maxFlds)
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
	CString delim = GetDelim();
	if (1 != delim.GetLength())
		return;
	m_ctrlPreview.DeleteAllItems();
	int nColumnCount = m_ctrlPreview.GetHeaderCtrl()->GetItemCount();
	int i;
	for (i = 0; i < nColumnCount; ++i)
		m_ctrlPreview.DeleteColumn(0);
	CAgilityBookOptions::ColumnOrder order = GetColumnInfo();
	size_t iCol;
	std::vector<int> columns[IO_TYPE_MAX];
	for (iCol = 0; iCol < IO_TYPE_MAX; ++iCol)
	{
		CDlgAssignColumns::GetColumnOrder(order, iCol, columns[iCol]);
	}

	CStringArray cols;
	switch (m_pSheet->GetImportExportItem())
	{
	default: break;
	case WIZ_IMPORT_RUNS:
		{
			for (iCol = 0; iCol < columns[IO_TYPE_RUNS_FAULTS_TIME].size(); ++iCol)
			{
				cols.Add(CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_RUNS_FAULTS_TIME][iCol]));
			}
			for (iCol = 0; iCol < columns[IO_TYPE_RUNS_TIME_FAULTS].size(); ++iCol)
			{
				CString str = CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_RUNS_TIME_FAULTS][iCol]);
				if (iCol >= static_cast<size_t>(cols.GetSize()))
					cols.Add(str);
				else
				{
					if (cols[iCol] != str && 0 < str.GetLength())
						cols[iCol] += "/" + str;
				}
			}
			for (iCol = 0; iCol < columns[IO_TYPE_RUNS_OPEN_CLOSE].size(); ++iCol)
			{
				CString str = CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_RUNS_OPEN_CLOSE][iCol]);
				if (iCol >= static_cast<size_t>(cols.GetSize()))
					cols.Add(str);
				else
				{
					if (cols[iCol] != str && 0 < str.GetLength())
						cols[iCol] += "/" + str;
				}
			}
			for (iCol = 0; iCol < columns[IO_TYPE_RUNS_POINTS].size(); ++iCol)
			{
				CString str = CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_RUNS_POINTS][iCol]);
				if (iCol >= static_cast<size_t>(cols.GetSize()))
					cols.Add(str);
				else
				{
					if (cols[iCol] != str && 0 < str.GetLength())
						cols[iCol] += "/" + str;
				}
			}
		}
		break;
	case WIZ_IMPORT_CALENDAR:
		for (iCol = 0; iCol < columns[IO_TYPE_CALENDAR].size(); ++iCol)
		{
			cols.Add(CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_CALENDAR][iCol]));
		}
		break;
	case WIZ_IMPORT_LOG:
		for (iCol = 0; iCol < columns[IO_TYPE_TRAINING].size(); ++iCol)
		{
			cols.Add(CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_TRAINING][iCol]));
		}
		break;
	}
	LV_COLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM;
	for (iCol = 0; iCol < static_cast<size_t>(cols.GetSize()); ++iCol)
	{
		CString str = cols[iCol];
		col.fmt = LVCFMT_LEFT;
		col.pszText = str.GetBuffer(0);
		col.iSubItem = static_cast<int>(iCol);
		m_ctrlPreview.InsertColumn(static_cast<int>(iCol), &col);
	}

	for (i = m_Row - 1; i < m_FileData.GetSize(); ++i)
	{
		CString str = m_FileData[i];
		CString fld;
		iCol = 0;
		while (GetField(delim, str, fld, static_cast<int>(iCol), cols.GetSize()))
		{
			if (0 == iCol)
				m_ctrlPreview.InsertItem(i-(m_Row-1), fld);
			else
				m_ctrlPreview.SetItemText(i-(m_Row-1), static_cast<int>(iCol), fld);
			++iCol;
		}
	}
	for (iCol = 0; iCol < static_cast<size_t>(cols.GetSize()); ++iCol)
		m_ctrlPreview.SetColumnWidth(static_cast<int>(iCol), LVSCW_AUTOSIZE_USEHEADER);
}

/////////////////////////////////////////////////////////////////////////////
// CWizardImport message handlers

BOOL CWizardImport::OnInitDialog() 
{
	CDlgBasePropertyPage::OnInitDialog();
	m_ctrlSpin.SetRange(100, 1);
	static struct
	{
		char const* pFormat;
		ARBDate::DateFormat format;
	} const sc_Dates[] =
	{
		{"MM-DD-YYYY", ARBDate::eDashMMDDYYYY},
		{"MM/DD/YYYY", ARBDate::eSlashMMDDYYYY},
		{"YYYY-MM-DD", ARBDate::eDashYYYYMMDD},
		{"YYYY/MM/DD", ARBDate::eSlashYYYYMMDD},
		{"DD-MM-YYYY", ARBDate::eDashDDMMYYYY},
		{"DD/MM/YYYY", ARBDate::eSlashDDMMYYYY},
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

static ARBDogRun* CreateRun(ARBDogRun* pRun, ARBConfigScoring const* pScoring)
{
	if (!pRun)
	{
		pRun = new ARBDogRun();
		pRun->GetScoring().SetType(ARBDogRunScoring::TranslateConfigScoring(pScoring->GetScoringStyle()),
			pScoring->DropFractions());
	}
	return pRun;
}

static std::string GetPrimaryVenue(std::string const& venues)
{
	std::string venue;
	std::string::size_type pos = venues.find('/');
	if (std::string::npos != pos)
		venue = venues.substr(0, pos);
	else
		venue = venues;
	return venue;
}

static void BreakLine(char inSep, std::string inStr, std::vector<std::string>& outFields)
{
	outFields.clear();
	std::string::size_type pos = inStr.find(inSep);
	while (std::string::npos != pos)
	{
		outFields.push_back(inStr.substr(0, pos));
		inStr = inStr.substr(pos+1);
		pos = inStr.find(inSep);
	}
	outFields.push_back(inStr);
}

static ARBCalendar* CreateCal(ARBCalendar* pCal)
{
	if (!pCal)
	{
		pCal = new ARBCalendar();
	}
	return pCal;
}

static ARBTraining* CreateLog(ARBTraining* pLog)
{
	if (!pLog)
	{
		pLog = new ARBTraining();
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
		return FALSE;
	CAgilityBookOptions::SetImportStartRow(m_Row);
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
	ARBDate::DateFormat format = static_cast<ARBDate::DateFormat>(m_ctrlDateFormat.GetItemData(index));
	CAgilityBookOptions::SetImportExportDateFormat(true, format);

	CAgilityBookOptions::ColumnOrder order = GetColumnInfo();
	size_t iCol;
	std::vector<int> columns[IO_TYPE_MAX];
	for (iCol = 0; iCol < IO_TYPE_MAX; ++iCol)
	{
		CDlgAssignColumns::GetColumnOrder(order, iCol, columns[iCol]);
	}
	CString errLog;
	int nAdded = 0;
	int nDuplicate = 0;
	int nSkipped = 0;
	int nColumns = m_ctrlPreview.GetHeaderCtrl()->GetItemCount();
	for (int nItem = 0; nItem < m_ctrlPreview.GetItemCount(); ++nItem)
	{
		std::vector<std::string> entry;
		entry.reserve(nColumns);
		for (int i = 0; i < nColumns; ++i)
		{
			CString str = m_ctrlPreview.GetItemText(nItem, i);
			str.Replace("\r\n", "\n");
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
				ARBConfigScoring const* pScoring = NULL;
				for (i = 0; !pScoring && i < 4; ++i)
				{
					if (0 <= idxVenue[i] && 0 <= idxEvent[i] && 0 <= idxDiv[i] && 0 <= idxLevel[i] && 0 <= idxDate[i])
					{
						pScoring = m_pDoc->GetConfig().GetVenues().FindEvent(
							GetPrimaryVenue(entry[idxVenue[i]]),
							entry[idxEvent[i]],
							entry[idxDiv[i]],
							entry[idxLevel[i]],
							ARBDate::FromString(entry[idxDate[i]], format));
					}
				}
				// It's conceivable that we could have more than one match.
				// But just don't worry about that... If data from various
				// rows/cols has that much overlap, it's just not worth it.
				if (!pScoring)
				{
					CString str;
					str.Format("Warning: Line %d: Skipped entry, unable to find a valid configuration entry\n",
						nItem + 1);
					errLog += str;
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

				std::string nameReg, nameCall;
				std::string trialVenue, trialClub, trialLocation, trialNotes;
				ARBDogRun* pRun = NULL;
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
								CString str;
								str.Format("ERROR: Line %d, Column %d: Invalid run date: %s\n",
									nItem + 1, iCol + 1, entry[iCol].c_str());
								errLog += str;
								if (pRun)
								{
									pRun->Release();
									pRun = NULL;
								}
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
						pRun->GetScoring().SetCourseFaults(static_cast<short>(atol(entry[iCol].c_str())));
						break;
					case IO_RUNS_TIME:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetTime(strtod(entry[iCol].c_str(), NULL));
						break;
					case IO_RUNS_YARDS:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetYards(strtod(entry[iCol].c_str(), NULL));
						break;
					case IO_RUNS_YPS:
						// Computed
						break;
					case IO_RUNS_SCT:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetSCT(strtod(entry[iCol].c_str(), NULL));
						break;
					case IO_RUNS_TOTAL_FAULTS:
						// Computed.
						break;
					case IO_RUNS_REQ_OPENING:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetNeedOpenPts(static_cast<short>(atol(entry[iCol].c_str())));
						break;
					case IO_RUNS_REQ_CLOSING:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetNeedClosePts(static_cast<short>(atol(entry[iCol].c_str())));
						break;
					case IO_RUNS_OPENING:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetOpenPts(static_cast<short>(atol(entry[iCol].c_str())));
						break;
					case IO_RUNS_CLOSING:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetClosePts(static_cast<short>(atol(entry[iCol].c_str())));
						break;
					case IO_RUNS_REQ_POINTS:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetNeedOpenPts(static_cast<short>(atol(entry[iCol].c_str())));
						break;
					case IO_RUNS_POINTS:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetOpenPts(static_cast<short>(atol(entry[iCol].c_str())));
						break;
					case IO_RUNS_PLACE:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetPlace(static_cast<short>(atol(entry[iCol].c_str())));
						break;
					case IO_RUNS_IN_CLASS:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetInClass(static_cast<short>(atol(entry[iCol].c_str())));
						break;
					case IO_RUNS_DOGSQD:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetDogsQd(static_cast<short>(atol(entry[iCol].c_str())));
						break;
					case IO_RUNS_Q:
						{
							pRun = CreateRun(pRun, pScoring);
							ARB_Q q;
							if ("QQ" == entry[iCol])
								entry[iCol] = "Q";
							std::string err;
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
							std::string str = pRun->GetNote();
							if (0 < str.length())
								str += "\n";
							str += entry[iCol];
							pRun->SetNote(str);
						}
						break;
					}
				}
				// Now that we've created a run, we have to put it somewhere.
				if (pRun)
				{
					// Find the dog
					ARBDog* pDog = m_pDoc->GetCurrentDog();
					if (0 < nameReg.length() || 0 < nameCall.length())
					{
						pDog = NULL;
						for (ARBDogList::iterator iterDog = m_pDoc->GetDogs().begin(); iterDog != m_pDoc->GetDogs().end(); ++iterDog)
						{
							ARBDog* pDogTmp = *iterDog;
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
							pDog = new ARBDog();
							if (0 < nameReg.length())
								pDog->SetRegisteredName(nameReg);
							if (0 < nameCall.length())
								pDog->SetCallName(nameCall);
							if (0 == nameCall.length() && 0 < nameReg.length())
								pDog->SetCallName(nameReg);
							m_pDoc->GetDogs().AddDog(pDog);
							pDog->Release();
							// pDog is still valid until it is actually removed from the doglist.
						}
					}
					if (!pDog)
					{
						if (m_pDoc->GetDogs().begin() == m_pDoc->GetDogs().end())
						{
							pDog = new ARBDog();
							pDog->SetCallName("?");
							m_pDoc->GetDogs().AddDog(pDog);
							pDog->Release();
						}
						else
							pDog = *(m_pDoc->GetDogs().begin());
					}
					ASSERT(NULL != pDog);

					// Find the trial
					std::vector<std::string> venues;
					std::vector<std::string> clubs;
					if (0 < trialVenue.length())
						BreakLine('/', trialVenue, venues);
					if (0 < trialClub.length())
						BreakLine('/', trialClub, clubs);
					if (clubs.size() < venues.size())
					{
						// Clubs and venues now agree so we can use them together easily.
						if (0 == clubs.size())
							clubs.push_back("?");
						while (clubs.size() < venues.size())
							clubs.push_back(clubs[clubs.size()-1]);
					}
					ARBDogTrial* pTrial = NULL;
					for (ARBDogTrialList::iterator iterTrial = pDog->GetTrials().begin(); iterTrial != pDog->GetTrials().end(); ++iterTrial)
					{
						ARBDogTrial* pTrialTmp = *iterTrial;
						if (pTrialTmp->GetClubs().size() == venues.size()
						&& pTrialTmp->GetLocation() == trialLocation
						&& pRun->GetDate().isBetween(pTrialTmp->GetRuns().GetStartDate(), pTrialTmp->GetRuns().GetEndDate()))
						{
							bool bOk = true;
							size_t i = 0;
							for (ARBDogClubList::iterator iterClub = pTrialTmp->GetClubs().begin(); iterClub != pTrialTmp->GetClubs().end(); ++iterClub)
							{
								ARBDogClub* pClub = new ARBDogClub();
								pClub->SetName(clubs[i]);
								pClub->SetVenue(venues[i]);
								if (*pClub != *(*(iterClub)))
								{
									bOk = false;
									break;
								}
								pClub->Release();
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
						pTrial = new ARBDogTrial();
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
						pTrial->Release();
					}
					pTrial->GetRuns().AddRun(pRun);
					pTrial->GetRuns().sort(true);
					pRun->Release();
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
				ARBCalendar* pCal = NULL;
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
								CString str;
								str.Format("ERROR: Line %d, Column %d: Invalid calendar start date: %s\n",
									nItem + 1, iCol + 1, entry[iCol].c_str());
								errLog += str;
								if (pCal)
								{
									pCal->Release();
									pCal = NULL;
								}
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
								CString str;
								str.Format("ERROR: Line %d, Column %d: Invalid calendar end date: %s\n",
									nItem + 1, iCol + 1, entry[iCol].c_str());
								errLog += str;
								if (pCal)
								{
									pCal->Release();
									pCal = NULL;
								}
								iCol = columns[IO_TYPE_CALENDAR].size();
							}
						}
						break;
					case IO_CAL_TENTATIVE:
						pCal = CreateCal(pCal);
						pCal->SetIsTentative(("?" == entry[iCol] || "y" == entry[iCol] || "Y" == entry[iCol]));
						break;
					case IO_CAL_ENTERED:
						if ("N" == entry[iCol])
						{
							pCal = CreateCal(pCal);
							pCal->SetEntered(ARBCalendar::eNot);
						}
						else if ("P" == entry[iCol] || "Planning" == entry[iCol])
						{
							pCal = CreateCal(pCal);
							pCal->SetEntered(ARBCalendar::ePlanning);
						}
						else if ("E" == entry[iCol] || "Entered" == entry[iCol])
						{
							pCal = CreateCal(pCal);
							pCal->SetEntered(ARBCalendar::eEntered);
						}
						else
						{
							CString str;
							str.Format("ERROR: Line %d, Column %d: Invalid calendar entered value: %s [N, P or E]\n",
								nItem + 1, iCol + 1, entry[iCol].c_str());
							errLog += str;
							if (pCal)
							{
								pCal->Release();
								pCal = NULL;
							}
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
								CString str;
								str.Format("ERROR: Line %d, Column %d: Invalid calendar opening date: %s\n",
									nItem + 1, iCol + 1, entry[iCol].c_str());
								errLog += str;
								if (pCal)
								{
									pCal->Release();
									pCal = NULL;
								}
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
								CString str;
								str.Format("ERROR: Line %d, Column %d: Invalid calendar closing date: %s\n",
									nItem + 1, iCol + 1, entry[iCol].c_str());
								errLog += str;
								if (pCal)
								{
									pCal->Release();
									pCal = NULL;
								}
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
					if (NULL == m_pDoc->GetCalendar().FindCalendar(pCal))
					{
						m_pDoc->GetCalendar().AddCalendar(pCal);
						m_pDoc->GetCalendar().sort();
						++nAdded;
					}
					else
						++nDuplicate;
					pCal->Release();
				}
				else
					++nSkipped;
			}
			break;

		case WIZ_IMPORT_LOG:
			{
				ARBTraining* pLog = NULL;
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
								CString str;
								str.Format("ERROR: Line %d, Column %d: Invalid training log date: %s\n",
									nItem + 1, iCol + 1, entry[iCol].c_str());
								errLog += str;
								if (pLog)
								{
									pLog->Release();
									pLog = NULL;
								}
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
					if (NULL == m_pDoc->GetTraining().FindTraining(pLog))
					{
						m_pDoc->GetTraining().AddTraining(pLog);
						m_pDoc->GetTraining().sort();
						++nAdded;
					}
					else
						++nDuplicate;
					pLog->Release();
				}
				else
					++nSkipped;
			}
			break;
		}
	}
	CString str;
	if (!errLog.IsEmpty())
		errLog += "\n";
	str.Format("%d entries added\n%d duplicates (not added)\n%d entries skipped",
		nAdded, nDuplicate, nSkipped);
	errLog += str;
	CDlgMessage dlg(errLog, 0, this);
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

void CWizardImport::OnDeltaposImportRowSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
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
	filter.LoadString(IDS_FILEEXT_FILTER_TXT);
	CFileDialog file(TRUE, "", "", OFN_FILEMUSTEXIST, filter, this);
	if (IDOK == file.DoModal())
	{
		m_FileName = file.GetPathName();
		CString str;
		str.FormatMessage(IDS_FILE_PREVIEW, (LPCTSTR)m_FileName);
		m_ctrlPreviewFile.SetWindowText(str);
		CWaitCursor wait;
		m_FileData.RemoveAll();
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
		UpdateButtons();
		UpdatePreview();
	}
}
