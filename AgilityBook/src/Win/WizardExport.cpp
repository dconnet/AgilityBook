/*
 * Copyright � 2003-2004 David Connet. All Rights Reserved.
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
 * @brief Export Wizard
 * @author David Connet
 *
 * Revision History
 * @li 2004-09-07 DRC Time+Fault scoring shouldn't include time faults.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-05-10 DRC Place quotes are field on output as needed.
 * @li 2004-01-04 DRC Added date format specification.
 * @li 2003-12-30 DRC Fixed a bug exporting the training log.
 * @li 2003-12-10 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "WizardExport.h"
#include <fstream>

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookViewCalendarList.h"
#include "AgilityBookViewTraining.h"
#include "ARBConfigScoring.h"
#include "ARBDog.h"
#include "ARBDogRun.h"
#include "ARBDogTrial.h"
#include "DlgAssignColumns.h"
#include "Wizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizardExport property page

IMPLEMENT_DYNAMIC(CWizardExport, CDlgBasePropertyPage)

CWizardExport::CWizardExport(CWizard* pSheet, CAgilityBookDoc* pDoc)
	: CDlgBasePropertyPage(CWizardExport::IDD)
	, m_pSheet(pSheet)
	, m_pDoc(pDoc)
{
	//{{AFX_DATA_INIT(CWizardExport)
	m_Delim = -1;
	m_Delimiter = _T(":");
	//}}AFX_DATA_INIT
	int delim;
	CAgilityBookOptions::GetImportExportDelimiters(false, delim, m_Delimiter);
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

CWizardExport::~CWizardExport()
{
}

void CWizardExport::DoDataExchange(CDataExchange* pDX)
{
	CDlgBasePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardExport)
	DDX_Radio(pDX, IDC_WIZARD_EXPORT_DELIM_TAB, m_Delim);
	DDX_Text(pDX, IDC_WIZARD_EXPORT_DELIM, m_Delimiter);
	DDV_MaxChars(pDX, m_Delimiter, 1);
	DDX_Control(pDX, IDC_WIZARD_EXPORT_ASSIGN, m_ctrlAssign);
	DDX_Control(pDX, IDC_WIZARD_EXPORT_DATE, m_ctrlDateFormat);
	DDX_Control(pDX, IDC_WIZARD_EXPORT_PREVIEW, m_ctrlPreview);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWizardExport, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CWizardExport)
	ON_BN_CLICKED(IDC_WIZARD_EXPORT_DELIM_TAB, OnExportDelim)
	ON_BN_CLICKED(IDC_WIZARD_EXPORT_ASSIGN, OnExportAssign)
	ON_BN_CLICKED(IDC_WIZARD_EXPORT_DELIM_SPACE, OnExportDelim)
	ON_BN_CLICKED(IDC_WIZARD_EXPORT_DELIM_COLON, OnExportDelim)
	ON_BN_CLICKED(IDC_WIZARD_EXPORT_DELIM_SEMI, OnExportDelim)
	ON_BN_CLICKED(IDC_WIZARD_EXPORT_DELIM_COMMA, OnExportDelim)
	ON_BN_CLICKED(IDC_WIZARD_EXPORT_DELIM_OTHER, OnExportDelim)
	ON_EN_CHANGE(IDC_WIZARD_EXPORT_DELIM, OnExportDelim)
	ON_CBN_SELCHANGE(IDC_WIZARD_EXPORT_DATE, OnSelchangeDate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CAgilityBookOptions::ColumnOrder CWizardExport::GetColumnInfo() const
{
	CAgilityBookOptions::ColumnOrder order = CAgilityBookOptions::eUnknown;
	switch (m_pSheet->GetImportExportItem())
	{
	default: break;
	case WIZ_EXPORT_RUNS: order = CAgilityBookOptions::eRunsExport; break;
	case WIZ_EXPORT_CALENDAR: order = CAgilityBookOptions::eCalExport; break;
	case WIZ_EXPORT_LOG: order = CAgilityBookOptions::eLogExport; break;
	}
	return order;
}

CString CWizardExport::GetDelim() const
{
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle())
		return "";
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

CString CWizardExport::PrepFieldOutput(LPCTSTR inStr) const
{
	CString delim = GetDelim();
	bool bAddQuotes = false;
	CString fld(inStr);
	if (!delim.IsEmpty() && (0 <= fld.Find(delim) || 0 <= fld.Find('"')))
	{
		bAddQuotes = true;
		if (0 <= fld.Find('"'))
			fld.Replace("\"", "\"\"");
	}
	if (bAddQuotes)
	{
		fld = "\"" + fld;
		fld += "\"";
	}
	return fld;
}

void CWizardExport::UpdateButtons()
{
	DWORD dwWiz = PSWIZB_BACK;
	// Some tests to make sure things are ready
	bool bOk = false;
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle())
		bOk = true;
	else if (1 == GetDelim().GetLength())
		bOk = true;
	BOOL bEnable = FALSE;
	CAgilityBookOptions::ColumnOrder order = CAgilityBookOptions::eUnknown;
	switch (m_pSheet->GetImportExportItem())
	{
	default:
		break;
	case WIZ_EXPORT_RUNS:
		order = CAgilityBookOptions::eRunsExport;
		break;
	case WIZ_EXPORT_CALENDAR:
		order = CAgilityBookOptions::eCalExport;
		break;
	case WIZ_EXPORT_LOG:
		order = CAgilityBookOptions::eLogExport;
		break;
	}
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
    GetDlgItem(IDC_WIZARD_EXPORT_DELIM_GROUP)->ShowWindow(nCmdShow);
    GetDlgItem(IDC_WIZARD_EXPORT_DELIM_TAB)->ShowWindow(nCmdShow);
    GetDlgItem(IDC_WIZARD_EXPORT_DELIM_SPACE)->ShowWindow(nCmdShow);
    GetDlgItem(IDC_WIZARD_EXPORT_DELIM_COLON)->ShowWindow(nCmdShow);
    GetDlgItem(IDC_WIZARD_EXPORT_DELIM_SEMI)->ShowWindow(nCmdShow);
    GetDlgItem(IDC_WIZARD_EXPORT_DELIM_COMMA)->ShowWindow(nCmdShow);
    GetDlgItem(IDC_WIZARD_EXPORT_DELIM_OTHER)->ShowWindow(nCmdShow);
    GetDlgItem(IDC_WIZARD_EXPORT_DELIM)->ShowWindow(nCmdShow);
	m_ctrlAssign.EnableWindow(bEnable);
	dwWiz |= (bOk ? PSWIZB_FINISH : PSWIZB_DISABLEDFINISH);
	m_pSheet->SetWizardButtons(dwWiz);
}

CString CWizardExport::AddPreviewData(int inLine, int inCol, CString inData)
{
	// TODO: Add option to allow CRs?
	inData.Replace("\n", " ");
	CString data;
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle())
	{
		if (0 == inCol)
			m_ctrlPreview.InsertItem(inLine, inData);
		else
			m_ctrlPreview.SetItemText(inLine, inCol, inData);
	}
	else
	{
		if (0 < inCol)
			data += GetDelim();
		data += PrepFieldOutput(inData);
	}
	return data;
}

void CWizardExport::UpdatePreview()
{
	CWaitCursor wait;

	// Clear existing preview data.
	m_ctrlPreview.DeleteAllItems();
	int nColumnCount = m_ctrlPreview.GetHeaderCtrl()->GetItemCount();
	int iCol;
	for (iCol = 0; iCol < nColumnCount; ++iCol)
		m_ctrlPreview.DeleteColumn(0);

	// Get export info.
	ARBDate::DateFormat format = ARBDate::eSlashMDY;
	int idxDateFormat = m_ctrlDateFormat.GetCurSel();
	if (CB_ERR != idxDateFormat)
		format = static_cast<ARBDate::DateFormat>(m_ctrlDateFormat.GetItemData(idxDateFormat));
	CString delim = GetDelim();
	if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle()
	&& delim.IsEmpty())
	{
		m_ctrlPreview.InsertColumn(0, "");
		m_ctrlPreview.InsertItem(0, "No delimiter specified. Unable to preview data.");
		m_ctrlPreview.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
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
	default:
		break;
	case WIZ_EXPORT_RUNS:
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
						cols[iCol] += "/" + str;
				}
			}
		}
		break;
	case WIZ_EXPORT_CALENDAR:
		for (index = 0; index < columns[IO_TYPE_CALENDAR].size(); ++index)
		{
			CString str = CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_CALENDAR][index]);
			cols.Add(str);
		}
		break;
	case WIZ_EXPORT_LOG:
		for (index = 0; index < columns[IO_TYPE_TRAINING].size(); ++index)
		{
			CString str = CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_TRAINING][index]);
			cols.Add(str);
		}
	}

	// Now start writing the data.
	int iLine = 0;
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle())
	{
		for (iCol = 0; iCol < cols.GetSize(); ++iCol)
			m_ctrlPreview.InsertColumn(iCol, "");
		m_ctrlPreview.InsertItem(iLine, cols[0]);
		for (iCol = 1; iCol < cols.GetSize(); ++iCol)
			m_ctrlPreview.SetItemText(iLine, iCol, cols[iCol]);
		++iLine;
	}
	else
	{
		m_ctrlPreview.InsertColumn(0, "");
		CString data;
		for (iCol = 0; iCol < cols.GetSize(); ++iCol)
		{
			if (0 < iCol)
				data += delim;
			data += PrepFieldOutput(cols[iCol]);
		}
		if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle())
			m_ctrlPreview.InsertItem(iLine, data);
		++iLine;
	}

	switch (m_pSheet->GetImportExportItem())
	{
	default:
		break;

	case WIZ_EXPORT_RUNS:
		{
			for (ARBDogList::const_iterator iterDog = m_pDoc->GetDogs().begin(); iterDog != m_pDoc->GetDogs().end(); ++iterDog)
			{
				ARBDog const* pDog = *iterDog;
				for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin(); iterTrial != pDog->GetTrials().end(); ++iterTrial)
				{
					ARBDogTrial const* pTrial = *iterTrial;
					for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin(); iterRun != pTrial->GetRuns().end(); ++iterRun)
					{
						ARBDogRun const* pRun = *iterRun;
						ARBConfigScoring const* pScoring = NULL;
						if (pTrial->GetClubs().GetPrimaryClub())
							pScoring = m_pDoc->GetConfig().GetVenues().FindEvent(
								pTrial->GetClubs().GetPrimaryClub()->GetVenue(),
								pRun->GetEvent(),
								pRun->GetDivision(),
								pRun->GetLevel(),
								pRun->GetDate());
						ASSERT(pScoring);
						if (pScoring)
						{
							int idxType = -1;
							switch (pScoring->GetScoringStyle())
							{
							case ARBConfigScoring::eFaultsThenTime:
							case ARBConfigScoring::eFaults100ThenTime:
							case ARBConfigScoring::eFaults200ThenTime:
								idxType = IO_TYPE_RUNS_FAULTS_TIME;
								break;
							case ARBConfigScoring::eOCScoreThenTime:
								idxType = IO_TYPE_RUNS_OPEN_CLOSE;
								break;
							case ARBConfigScoring::eScoreThenTime:
								idxType = IO_TYPE_RUNS_POINTS;
								break;
							case ARBConfigScoring::eTimePlusFaults:
								idxType = IO_TYPE_RUNS_TIME_FAULTS;
								break;
							}
							ASSERT(-1 != idxType);
							if (0 <= idxType)
							{
								CString data;
								for (int idx = 0; idx < static_cast<int>(columns[idxType].size()); ++idx)
								{
									switch (columns[idxType][idx])
									{
									case IO_RUNS_REG_NAME:
										data += AddPreviewData(iLine, idx, pDog->GetRegisteredName().c_str());
										break;
									case IO_RUNS_CALL_NAME:
										data += AddPreviewData(iLine, idx, pDog->GetCallName().c_str());
										break;
									case IO_RUNS_DATE:
										data += AddPreviewData(iLine, idx, pRun->GetDate().GetString(format).c_str());
										break;
									case IO_RUNS_VENUE:
										{
											CString fld;
											int i = 0;
											for (ARBDogClubList::const_iterator iter = pTrial->GetClubs().begin();
												iter != pTrial->GetClubs().end();
												++iter, ++i)
											{
												if (0 < i)
													fld += "/";
												fld += (*iter)->GetVenue().c_str();
											}
											data += AddPreviewData(iLine, idx, fld);
										}
										break;
									case IO_RUNS_CLUB:
										{
											CString fld;
											int i = 0;
											for (ARBDogClubList::const_iterator iter = pTrial->GetClubs().begin();
												iter != pTrial->GetClubs().end();
												++iter, ++i)
											{
												if (0 < i)
													fld += "/";
												fld += (*iter)->GetName().c_str();
											}
											data += AddPreviewData(iLine, idx, fld);
										}
										break;
									case IO_RUNS_LOCATION:
										data += AddPreviewData(iLine, idx, pTrial->GetLocation().c_str());
										break;
									case IO_RUNS_TRIAL_NOTES:
										data += AddPreviewData(iLine, idx, pTrial->GetNote().c_str());
										break;
									case IO_RUNS_DIVISION:
										data += AddPreviewData(iLine, idx, pRun->GetDivision().c_str());
										break;
									case IO_RUNS_LEVEL:
										data += AddPreviewData(iLine, idx, pRun->GetLevel().c_str());
										break;
									case IO_RUNS_EVENT:
										data += AddPreviewData(iLine, idx, pRun->GetEvent().c_str());
										break;
									case IO_RUNS_HEIGHT:
										data += AddPreviewData(iLine, idx, pRun->GetHeight().c_str());
										break;
									case IO_RUNS_JUDGE:
										data += AddPreviewData(iLine, idx, pRun->GetJudge().c_str());
										break;
									case IO_RUNS_HANDLER:
										data += AddPreviewData(iLine, idx, pRun->GetHandler().c_str());
										break;
									case IO_RUNS_CONDITIONS:
										data += AddPreviewData(iLine, idx, pRun->GetConditions().c_str());
										break;
									case IO_RUNS_COURSE_FAULTS:
										{
											CString str;
											str.Format("%hd", pRun->GetScoring().GetCourseFaults());
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_TIME:
										data += AddPreviewData(iLine, idx, pRun->GetScoring().GetTime().str().c_str());
										break;
									case IO_RUNS_YARDS:
										{
											CString str;
											str.Format("%.3f", pRun->GetScoring().GetYards());
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_YPS:
										{
											double yps;
											if (pRun->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), yps))
											{
												CString str;
												str.Format("%.3f", yps);
												data += AddPreviewData(iLine, idx, str);
											}
										}
										break;
									case IO_RUNS_SCT:
										data += AddPreviewData(iLine, idx, pRun->GetScoring().GetSCT().str().c_str());
										break;
									case IO_RUNS_TOTAL_FAULTS:
										{
											if (ARBDogRunScoring::eTypeByTime == pRun->GetScoring().GetType())
											{
												CString str;
												double faults = pRun->GetScoring().GetCourseFaults() + pRun->GetScoring().GetTimeFaults(pScoring);
												str.Format("%.3f", faults);
												data += AddPreviewData(iLine, idx, str);
											}
										}
										break;
									case IO_RUNS_REQ_OPENING:
										{
											CString str;
											str.Format("%hd", pRun->GetScoring().GetNeedOpenPts());
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_REQ_CLOSING:
										{
											CString str;
											str.Format("%hd", pRun->GetScoring().GetNeedClosePts());
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_OPENING:
										{
											CString str;
											str.Format("%hd", pRun->GetScoring().GetOpenPts());
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_CLOSING:
										{
											CString str;
											str.Format("%hd", pRun->GetScoring().GetClosePts());
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_REQ_POINTS:
										{
											CString str;
											str.Format("%hd", pRun->GetScoring().GetNeedOpenPts());
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_POINTS:
										{
											CString str;
											str.Format("%hd", pRun->GetScoring().GetOpenPts());
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_PLACE:
										{
											CString str;
											short place = pRun->GetPlace();
											if (0 > place)
												str = "?";
											else if (0 == place)
												str = "-";
											else
												str.Format("%hd", place);
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_IN_CLASS:
										{
											CString str;
											short inClass = pRun->GetInClass();
											if (0 >= inClass)
												str = "?";
											else
												str.Format("%hd", inClass);
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_DOGSQD:
										{
											CString str;
											short qd = pRun->GetDogsQd();
											if (0 > qd)
												str = "?";
											else
												str.Format("%hd", qd);
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_Q:
										{
											CString str = pRun->GetQ().str().c_str();
											if (pRun->GetQ().Qualified())
											{
												if (pTrial->HasQQ(
													pRun->GetDate(),
													m_pDoc->GetConfig(),
													pRun->GetDivision(),
													pRun->GetLevel()))
												{
													str.LoadString(IDS_QQ);
												}
												if (ARB_Q::eQ_SuperQ == pRun->GetQ())
													str.LoadString(IDS_SQ);
											}
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_SCORE:
										if (pRun->GetQ().Qualified()
										|| ARB_Q::eQ_NQ == pRun->GetQ())
										{
											data += AddPreviewData(iLine, idx, pRun->GetScore(pScoring).str().c_str());
										}
										break;
									case IO_RUNS_TITLE_POINTS:
										{
											CString str;
											short pts = 0;
											if (pRun->GetQ().Qualified())
												pts = pRun->GetTitlePoints(pScoring);
											str.Format("%hd", pts);
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_COMMENTS:
										data += AddPreviewData(iLine, idx, pRun->GetNote().c_str());
										break;
									case IO_RUNS_FAULTS:
										{
											CString fld;
											int i = 0;
											for (ARBDogFaultList::const_iterator iter = pRun->GetFaults().begin();
												iter != pRun->GetFaults().end();
												++iter)
											{
												if (0 < i)
													fld += "/";
												fld += (*iter).c_str();
											}
											data += AddPreviewData(iLine, idx, fld);
										}
										break;
									}
								}
								if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle())
									m_ctrlPreview.InsertItem(iLine, data);
								++iLine;
							}
						}
					}
				}
			}
		}
		break;

	case WIZ_EXPORT_CALENDAR:
		{
			for (ARBCalendarList::const_iterator iterCal = m_pDoc->GetCalendar().begin(); iterCal != m_pDoc->GetCalendar().end(); ++iterCal)
			{
				CString data;
				ARBCalendar const* pCal = *iterCal;
				for (int idx = 0; idx < static_cast<int>(columns[IO_TYPE_CALENDAR].size()); ++idx)
				{
					ARBDate date;
					if (0 < idx)
						data += delim;
					switch (columns[IO_TYPE_CALENDAR][idx])
					{
					case IO_CAL_START_DATE:
						data += AddPreviewData(iLine, idx, pCal->GetStartDate().GetString(format).c_str());
						break;
					case IO_CAL_END_DATE:
						data += AddPreviewData(iLine, idx, pCal->GetEndDate().GetString(format).c_str());
						break;
					case IO_CAL_TENTATIVE:
						if (pCal->IsTentative())
							data += AddPreviewData(iLine, idx, "?");
						break;
					case IO_CAL_ENTERED:
						switch (pCal->GetEntered())
						{
						default:
						case ARBCalendar::eNot:
							break;
						case ARBCalendar::eEntered:
							data += AddPreviewData(iLine, idx, "Entered");
							break;
						case ARBCalendar::ePlanning:
							data += AddPreviewData(iLine, idx, "Planning");
							break;
						}
						break;
					case IO_CAL_LOCATION:
						data += AddPreviewData(iLine, idx, pCal->GetLocation().c_str());
						break;
					case IO_CAL_CLUB:
						data += AddPreviewData(iLine, idx, pCal->GetClub().c_str());
						break;
					case IO_CAL_VENUE:
						data += AddPreviewData(iLine, idx, pCal->GetVenue().c_str());
						break;
					case IO_CAL_OPENS:
						date = pCal->GetOpeningDate();
						if (date.IsValid())
							data += AddPreviewData(iLine, idx, date.GetString(format).c_str());
						break;
					case IO_CAL_CLOSES:
						date = pCal->GetClosingDate();
						if (date.IsValid())
							data += AddPreviewData(iLine, idx, date.GetString(format).c_str());
						break;
					case IO_CAL_NOTES:
						data += AddPreviewData(iLine, idx, pCal->GetNote().c_str());
						break;
					}
				}
				if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle())
					m_ctrlPreview.InsertItem(iLine, data);
				++iLine;
			}
		}
		break;

	case WIZ_EXPORT_LOG:
		{
			for (ARBTrainingList::const_iterator iterLog = m_pDoc->GetTraining().begin(); iterLog != m_pDoc->GetTraining().end(); ++iterLog)
			{
				CString data;
				ARBTraining const* pLog = *iterLog;
				for (int idx = 0; idx < static_cast<int>(columns[IO_TYPE_TRAINING].size()); ++idx)
				{
					if (0 < idx)
						data += delim;
					switch (columns[IO_TYPE_TRAINING][idx])
					{
					case IO_LOG_DATE:
						data += AddPreviewData(iLine, idx, pLog->GetDate().GetString(format).c_str());
						break;
					case IO_LOG_NAME:
						data += AddPreviewData(iLine, idx, pLog->GetName().c_str());
						break;
					case IO_LOG_SUBNAME:
						data += AddPreviewData(iLine, idx, pLog->GetSubName().c_str());
						break;
					case IO_LOG_NOTES:
						data += AddPreviewData(iLine, idx, pLog->GetNote().c_str());
						break;
					}
				}
				if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle())
					m_ctrlPreview.InsertItem(iLine, data);
				++iLine;
			}
		}
		break;
	}
	for (iCol = 0; iCol < cols.GetSize(); ++iCol)
		m_ctrlPreview.SetColumnWidth(iCol, LVSCW_AUTOSIZE_USEHEADER);
}

/////////////////////////////////////////////////////////////////////////////
// CWizardExport message handlers

BOOL CWizardExport::OnInitDialog() 
{
	CDlgBasePropertyPage::OnInitDialog();
	static struct
	{
		char const* pFormat;
		ARBDate::DateFormat format;
	} const sc_Dates[] =
	{
		{"M-D-Y", ARBDate::eDashMDY},
		{"M/D/Y", ARBDate::eSlashMDY},
		{"Y-M-D", ARBDate::eDashYMD},
		{"Y/M/D", ARBDate::eSlashYMD},
		{"D-M-Y", ARBDate::eDashDMY},
		{"D/M/Y", ARBDate::eSlashDMY},
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
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWizardExport::OnSetActive() 
{
	UpdateButtons();
	UpdatePreview();
	return CDlgBasePropertyPage::OnSetActive();
}

LRESULT CWizardExport::OnWizardBack()
{
	return IDD_WIZARD_START;
}

BOOL CWizardExport::OnWizardFinish() 
{
	UpdateData(TRUE);
	int index = m_ctrlDateFormat.GetCurSel();
	if (CB_ERR == index)
		return FALSE;
	UpdatePreview();
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
		CAgilityBookOptions::SetImportExportDelimiters(false, delim, m_Delimiter);
	}
	ARBDate::DateFormat format = static_cast<ARBDate::DateFormat>(m_ctrlDateFormat.GetItemData(index));
	CAgilityBookOptions::SetImportExportDateFormat(true, format);

	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle())
	{
		int nColumnCount = m_ctrlPreview.GetHeaderCtrl()->GetItemCount();
		// TODO: Write data to excel
		for (int i = 0; i < m_ctrlPreview.GetItemCount(); ++i)
		{
			for (int iCol = 0; iCol < nColumnCount; ++iCol)
			{
				CString line = m_ctrlPreview.GetItemText(i, iCol);
			}
		}
		return CDlgBasePropertyPage::OnWizardFinish();
	}
	else
	{
		CString filter;
		filter.LoadString(IDS_FILEEXT_FILTER_TXT);
		CFileDialog file(FALSE, "", "", OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST, filter, this);
		if (IDOK == file.DoModal())
		{
			std::ofstream output(file.GetFileName(), std::ios::out);
			output.exceptions(std::ios_base::badbit);
			if (output.is_open())
			{
				for (int i = 0; i < m_ctrlPreview.GetItemCount(); ++i)
				{
					CString line = m_ctrlPreview.GetItemText(i, 0);
					output << (LPCTSTR)line << std::endl;
				}
				output.close();
			}
			return CDlgBasePropertyPage::OnWizardFinish();
		}
		else
			return FALSE;
	}
}

void CWizardExport::OnExportDelim() 
{
	UpdateData(TRUE);
	UpdateButtons();
	UpdatePreview();
}

void CWizardExport::OnExportAssign() 
{
	UpdateData(TRUE);
	CAgilityBookOptions::ColumnOrder order = CAgilityBookOptions::eUnknown;
	switch (m_pSheet->GetImportExportItem())
	{
	default: break;
	case WIZ_EXPORT_RUNS: order = CAgilityBookOptions::eRunsExport; break;
	case WIZ_EXPORT_CALENDAR: order = CAgilityBookOptions::eCalExport; break;
	case WIZ_EXPORT_LOG: order = CAgilityBookOptions::eLogExport; break;
	}
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

void CWizardExport::OnSelchangeDate() 
{
	UpdatePreview();
}
