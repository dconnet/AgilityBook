/*
 * Copyright © 2003 David Connet. All Rights Reserved.
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
#include "DlgAssignColumns.h"
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
	m_Row = 1;
	m_Delim = _T(":");
	m_bTab = TRUE;
	m_bSpace = FALSE;
	m_bColon = FALSE;
	m_bSemicolon = FALSE;
	m_bComma = FALSE;
	m_bOther = FALSE;
	//}}AFX_DATA_INIT
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
	DDX_Check(pDX, IDC_IMPORT_DELIM_TAB, m_bTab);
	DDX_Check(pDX, IDC_IMPORT_DELIM_SPACE, m_bSpace);
	DDX_Check(pDX, IDC_IMPORT_DELIM_COLON, m_bColon);
	DDX_Check(pDX, IDC_IMPORT_DELIM_SEMI, m_bSemicolon);
	DDX_Check(pDX, IDC_IMPORT_DELIM_COMMA, m_bComma);
	DDX_Check(pDX, IDC_IMPORT_DELIM_OTHER, m_bOther);
	DDX_Text(pDX, IDC_WIZARD_DELIM, m_Delim);
	DDV_MaxChars(pDX, m_Delim, 1);
	DDX_Control(pDX, IDC_IMPORT_PREVIEW_FILE, m_ctrlPreviewFile);
	DDX_Control(pDX, IDC_WIZARD_PREVIEW, m_ctrlPreview);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWizardImport, CPropertyPage)
	//{{AFX_MSG_MAP(CWizardImport)
	ON_EN_KILLFOCUS(IDC_IMPORT_ROW, OnImportKillFocus)
	ON_NOTIFY(UDN_DELTAPOS, IDC_IMPORT_ROW_SPIN, OnDeltaposImportRowSpin)
	ON_BN_CLICKED(IDC_WIZARD_ASSIGN, OnImportAssign)
	ON_BN_CLICKED(IDC_IMPORT_FILE, OnImportFile)
	ON_BN_CLICKED(IDC_IMPORT_DELIM_COLON, OnImportDelim)
	ON_BN_CLICKED(IDC_IMPORT_DELIM_COMMA, OnImportDelim)
	ON_BN_CLICKED(IDC_IMPORT_DELIM_OTHER, OnImportDelim)
	ON_BN_CLICKED(IDC_IMPORT_DELIM_SEMI, OnImportDelim)
	ON_BN_CLICKED(IDC_IMPORT_DELIM_SPACE, OnImportDelim)
	ON_BN_CLICKED(IDC_IMPORT_DELIM_TAB, OnImportDelim)
	ON_EN_CHANGE(IDC_WIZARD_DELIM, OnImportDelim)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CWizardImport::UpdateButtons()
{
	DWORD dwWiz = PSWIZB_BACK;
	// Some test to make sure things are ready
	bool bOk = false;
	dwWiz |= (bOk ? PSWIZB_FINISH : PSWIZB_DISABLEDFINISH);
	m_pSheet->SetWizardButtons(dwWiz);
}

void CWizardImport::UpdatePreview()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWizardImport message handlers

BOOL CWizardImport::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_ctrlSpin.SetRange(100, 1);
	m_ctrlPreview.InsertColumn(0, "Todo");
	m_ctrlPreview.InsertItem(0, "This is still being developed.");
	m_ctrlPreview.InsertItem(1, "It doesn't work yet.");
	m_ctrlPreview.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
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

BOOL CWizardImport::OnWizardFinish() 
{
	return CPropertyPage::OnWizardFinish();
}

void CWizardImport::OnImportKillFocus()
{
	UpdateData(TRUE);
	UpdatePreview();
}

void CWizardImport::OnDeltaposImportRowSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// There is no "changed" msg, only the "changing".
	// So we'll just update the ddx variable ourselves.
	m_Row = pNMUpDown->iPos + pNMUpDown->iDelta;
	UpdatePreview();
	*pResult = 0;
}

void CWizardImport::OnImportDelim() 
{
	UpdateData(TRUE);
	UpdateButtons();
	UpdatePreview();
}

void CWizardImport::OnImportAssign() 
{
	UpdateData(TRUE);
	CDlgAssignColumns dlg(this);
	if (IDOK == dlg.DoModal())
	{
		UpdateButtons();
		UpdatePreview();
	}
}

void CWizardImport::OnImportFile() 
{
	UpdateData(TRUE);
	CString def, fname, filter;
	filter.LoadString(IDS_FILEEXT_FILTER_TXT);
	CFileDialog file(TRUE, "", "", OFN_FILEMUSTEXIST, filter, this);
	if (IDOK == file.DoModal())
	{
		m_FileName = file.GetPathName();
		CString str;
		str.FormatMessage(IDS_FILE_PREVIEW, (LPCTSTR)m_FileName);
		m_ctrlPreviewFile.SetWindowText(str);
		// TODO: Load data
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
