/*
 * Copyright � 2003 David Connet. All Rights Reserved.
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

IMPLEMENT_DYNAMIC(CWizardImport, CPropertyPage)

CWizardImport::CWizardImport(CWizard* pSheet, CAgilityBookDoc* pDoc)
	: CPropertyPage(CWizardImport::IDD)
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
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardImport)
	DDX_Text(pDX, IDC_IMPORT_ROW, m_Row);
	DDV_MinMaxInt(pDX, m_Row, 1, 100);
	DDX_Control(pDX, IDC_IMPORT_ROW_SPIN, m_ctrlSpin);
	DDX_Radio(pDX, IDC_WIZARD_DELIM_TAB, m_Delim);
	DDX_Text(pDX, IDC_WIZARD_DELIM, m_Delimiter);
	DDV_MaxChars(pDX, m_Delimiter, 1);
	DDX_Control(pDX, IDC_IMPORT_PREVIEW_FILE, m_ctrlPreviewFile);
	DDX_Control(pDX, IDC_WIZARD_ASSIGN, m_ctrlAssign);
	DDX_Control(pDX, IDC_WIZARD_PREVIEW, m_ctrlPreview);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWizardImport, CPropertyPage)
	//{{AFX_MSG_MAP(CWizardImport)
	ON_EN_KILLFOCUS(IDC_IMPORT_ROW, OnImportKillFocus)
	ON_NOTIFY(UDN_DELTAPOS, IDC_IMPORT_ROW_SPIN, OnDeltaposImportRowSpin)
	ON_BN_CLICKED(IDC_WIZARD_ASSIGN, OnImportAssign)
	ON_BN_CLICKED(IDC_IMPORT_FILE, OnImportFile)
	ON_BN_CLICKED(IDC_WIZARD_DELIM_TAB, OnImportDelim)
	ON_BN_CLICKED(IDC_WIZARD_DELIM_SPACE, OnImportDelim)
	ON_BN_CLICKED(IDC_WIZARD_DELIM_COLON, OnImportDelim)
	ON_BN_CLICKED(IDC_WIZARD_DELIM_SEMI, OnImportDelim)
	ON_BN_CLICKED(IDC_WIZARD_DELIM_COMMA, OnImportDelim)
	ON_BN_CLICKED(IDC_WIZARD_DELIM_OTHER, OnImportDelim)
	ON_EN_CHANGE(IDC_WIZARD_DELIM, OnImportDelim)
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
		iCol = 0;
		int pos;
		while (0 <= (pos = str.Find(delim)) && iCol < static_cast<size_t>(cols.GetSize()))
		{
			CString data = str.Left(pos);
			str = str.Mid(pos+1);
			if (0 == iCol)
				m_ctrlPreview.InsertItem(i-(m_Row-1), data);
			else
				m_ctrlPreview.SetItemText(i-(m_Row-1), static_cast<int>(iCol), data);
			++iCol;
		}
		if (!str.IsEmpty() && iCol < static_cast<size_t>(cols.GetSize()))
			m_ctrlPreview.SetItemText(i-(m_Row-1), static_cast<int>(iCol), str);
	}
	for (iCol = 0; iCol < static_cast<size_t>(cols.GetSize()); ++iCol)
		m_ctrlPreview.SetColumnWidth(static_cast<int>(iCol), LVSCW_AUTOSIZE_USEHEADER);
}

/////////////////////////////////////////////////////////////////////////////
// CWizardImport message handlers

BOOL CWizardImport::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_ctrlSpin.SetRange(100, 1);
	UpdatePreview();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWizardImport::OnSetActive() 
{
	UpdateButtons();
	BOOL rc = CPropertyPage::OnSetActive();
	if (m_FileName.IsEmpty())
		OnImportFile();
	return rc;
}

LRESULT CWizardImport::OnWizardBack()
{
	return IDD_WIZARD_START;
}

static ARBDogRun* CreateRun(ARBDogRun* pRun)
{
	if (!pRun)
	{
		pRun = new ARBDogRun();
	}
	return pRun;
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
			++nSkipped;
			break;

		case WIZ_IMPORT_CALENDAR:
			{
				ARBCalendar* pCal = NULL;
				for (iCol = 0; iCol < static_cast<int>(columns[IO_TYPE_CALENDAR].size()); ++iCol)
				{
					if (0 == entry[iCol].length())
						continue;
					switch (columns[IO_TYPE_CALENDAR][iCol])
					{
					case IO_CAL_START_DATE:
						{
							ARBDate date = ARBDate::FromString(entry[iCol]);
							if (date.IsValid())
							{
								pCal = CreateCal(pCal);
								pCal->SetStartDate(date);
							}
							else
							{
								CString str;
								str.Format("ERROR: Line %d, Column %d: Invalid calendar start date: %s\n",
									nItem, iCol, entry[iCol].c_str());
								errLog += str;
							}
						}
						break;
					case IO_CAL_END_DATE:
						{
							ARBDate date = ARBDate::FromString(entry[iCol]);
							if (date.IsValid())
							{
								pCal = CreateCal(pCal);
								pCal->SetEndDate(date);
							}
							else
							{
								CString str;
								str.Format("ERROR: Line %d, Column %d: Invalid calendar end date: %s\n",
									nItem, iCol, entry[iCol].c_str());
								errLog += str;
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
								nItem, iCol, entry[iCol].c_str());
							errLog += str;
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
							ARBDate date = ARBDate::FromString(entry[iCol]);
							if (date.IsValid())
							{
								pCal = CreateCal(pCal);
								pCal->SetOpeningDate(date);
							}
							else
							{
								CString str;
								str.Format("ERROR: Line %d, Column %d: Invalid calendar opening date: %s\n",
									nItem, iCol, entry[iCol].c_str());
								errLog += str;
							}
						}
						break;
					case IO_CAL_CLOSES:
						{
							ARBDate date = ARBDate::FromString(entry[iCol]);
							if (date.IsValid())
							{
								pCal = CreateCal(pCal);
								pCal->SetClosingDate(date);
							}
							else
							{
								CString str;
								str.Format("ERROR: Line %d, Column %d: Invalid calendar closing date: %s\n",
									nItem, iCol, entry[iCol].c_str());
								errLog += str;
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
				for (iCol = 0; iCol < entry.size() && iCol < static_cast<int>(columns[IO_TYPE_TRAINING].size()); ++iCol)
				{
					if (0 == entry[iCol].length())
						continue;
					switch (columns[IO_TYPE_TRAINING][iCol])
					{
					case IO_LOG_DATE:
						{
							ARBDate date = ARBDate::FromString(entry[iCol]);
							if (date.IsValid())
							{
								pLog = CreateLog(pLog);
								pLog->SetDate(date);
							}
							else
							{
								CString str;
								str.Format("ERROR: Line %d, Column %d: Invalid training log date: %s\n",
									nItem, iCol, entry[iCol].c_str());
								errLog += str;
							}
						}
						break;
					case IO_LOG_NAME:
						pLog = CreateLog(pLog);
						pLog->SetName(entry[iCol]);
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

	return CPropertyPage::OnWizardFinish();
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

#if 0
// I used this code to load my own data - maybe I can tweak it for use here.
{
	//ARBDog* pDog = GetCurrentDog();
	//if (pDog)
	//	bEnable = TRUE;
	// @todo: Write file import code
The following code will import - but there's no safety and it can/will
corrupt the file. But there's some ideas here that may be of future use...
	ARBDog* pDog = GetCurrentDog();
	HTREEITEM hDog = GetTreeView()->GetCurrentTreeItem()->GetHTreeItem();
	if (!pDog)
		return;
	//Make sure any strings end up in the resource file.
	CString def, fname, filter;
	def.LoadString(IDS_FILEEXT_DEF_TXT);
	fname.LoadString(IDS_FILEEXT_FNAME_TXT);
	filter.LoadString(IDS_FILEEXT_FILTER_TXT);
	CFileDialog file(TRUE, def, fname, OFN_FILEMUSTEXIST, filter);
	if (IDOK == file.DoModal())
	{
		TRY
		{
			CStdioFile input(file.GetFileName(), CFile::modeRead | CFile::typeBinary);
			CString buffer;
			int line = 0;
			ARBDogTrial* pTrial = NULL;
			HTREEITEM hTrial = NULL;
			while (input.ReadString(buffer))
			{
				++line;
				buffer.TrimRight();
				if (!buffer.IsEmpty())
				{
					//0   :1   :2   :3  :4    :5    :6       :7    :8       :9        :10  :11         :12          :13   :14 :15:16   :17     :18    :19
					//Date:Show:Club:Org:Class:Level:Division:Judge:SCT     :Yards    :Time:TimeFaults :CourseFaults:Total:YPS:Q :Place:InClass:Points:Notes
					//    :    :    :   :     :     :        :     :Open Pts:Close Pts:Time:Opening Pts:Closing Pts :...
					//TRIAL:Place:Club1:Venue1[:Club2:Venue2...]
					CStringArray fields;
					LPCTSTR pos = LPCTSTR(buffer);
					LPCTSTR npos = _tcschr(pos, '\t');
					while (npos)
					{
						CString str(pos, static_cast<int>(npos - pos));
						fields.Add(str);
						pos = npos + 1;
						npos = _tcschr(pos, '\t');
					}
					if (pos)
					{
						CString str(pos);
						fields.Add(str);
					}
					if (fields[0] == "TRIAL")
					{
						ARBDogTrial* pNewTrial = new ARBDogTrial;
						pNewTrial->SetLocation((LPCTSTR)fields[1]);
						for (int i = 2; i < fields.GetSize(); i += 2)
						{
							pNewTrial->GetClubs().AddClub((LPCTSTR)fields[i], (LPCTSTR)fields[i+1]);
						}
						pTrial = pDog->GetTrials().AddTrial(pNewTrial);
						pNewTrial->Release();
						hTrial = GetTreeView()->InsertTrial(pTrial, hDog);
					}
					else if (pTrial)
					{
						ARBDogRun* pRun = new ARBDogRun;
						pRun->SetHeight("22");
						ARBDate date;
						date.Load((LPCTSTR)fields[0], 1);
						pRun->SetDate(date);
						if (fields.GetSize() > 4 && !fields[4].IsEmpty())
							pRun->SetEvent((LPCSTR)fields[4]);
						if (fields.GetSize() > 5 && !fields[5].IsEmpty())
							pRun->SetLevel((LPCSTR)fields[5]);
						if (fields.GetSize() > 6 && !fields[6].IsEmpty())
							pRun->SetDivision((LPCSTR)fields[6]);
						if (fields.GetSize() > 7 && !fields[7].IsEmpty())
							pRun->SetJudge((LPCTSTR)fields[7]);
						ARB_Q q;
						if (fields.GetSize() > 15 && !fields[15].IsEmpty())
						{
							q.Load((LPCTSTR)fields[15], 1);
						}
						if (fields.GetSize() > 10 && !fields[10].IsEmpty())
						{
							if (fields[10] == "NT")
								q.Load("E", 1);
							else
								pRun->GetScoring().SetTime(atof(fields[10]));
						}
						if (fields[4] == "Gamblers")
						{
							pRun->GetScoring().SetType(ARBDogRunScoring::eTypeByOpenClose, false);
							if (fields.GetSize() > 8 && !fields[8].IsEmpty())
								pRun->GetScoring().SetNeedOpenPts(atoi(fields[8]));
							if (fields.GetSize() > 9 && !fields[9].IsEmpty())
								pRun->GetScoring().SetNeedClosePts(atoi(fields[9]));
							if (fields.GetSize() > 11 && !fields[11].IsEmpty())
								pRun->GetScoring().SetOpenPts(atoi(fields[11]));
							if (fields.GetSize() > 12 && !fields[12].IsEmpty())
								pRun->GetScoring().SetClosePts(atoi(fields[12]));
						}
						else if (fields[4] == "Snooker")
						{
							pRun->GetScoring().SetType(ARBDogRunScoring::eTypeByPoints, false);
							pRun->GetScoring().SetNeedOpenPts(37);
							int score = 0;
							if (fields.GetSize() > 11 && !fields[11].IsEmpty())
								score += atoi(fields[11]);
							if (fields.GetSize() > 12 && !fields[12].IsEmpty())
								score += atoi(fields[12]);
							pRun->GetScoring().SetOpenPts(score);
						}
						else
						{
							pRun->GetScoring().SetType(ARBDogRunScoring::eTypeByTime, false);
							if (fields.GetSize() > 8 && !fields[8].IsEmpty())
								pRun->GetScoring().SetSCT(atof(fields[8]));
							if (fields.GetSize() > 9 && !fields[9].IsEmpty())
								pRun->GetScoring().SetYards(atof(fields[9]));
							if (fields.GetSize() > 12 && !fields[12].IsEmpty())
							{
								if (fields[12] == "E")
									q.Load("E", 1);
								else
									pRun->GetScoring().SetCourseFaults(atoi(fields[12]));
							}
						}
						pRun->SetQ(q);
						if (fields.GetSize() > 16 && !fields[16].IsEmpty())
							pRun->SetPlace(atoi(fields[16]));
						if (fields.GetSize() > 17 && !fields[17].IsEmpty())
							pRun->SetInClass(atoi(fields[17]));
						if (fields.GetSize() > 19 && !fields[19].IsEmpty())
							pRun->SetNote((LPCTSTR)fields[19]);
						ARBDogRun* pInsert = pTrial->GetRuns().AddRun(pRun);
						pRun->Release();
						GetTreeView()->InsertRun(pInsert, hTrial);
					}
				}
			}
			UpdateAllViews(NULL, 0);
			TRACE("%d lines\n", line);
		}
		CATCH (CFileException, e)
		{
		}
		END_CATCH
	}
}
#endif
