/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
 * @brief implementation of the CAgilityBookDoc class
 * @author David Connet
 *
 * MFC document class, provides a binding between the real data in the
 * CAgilityRecordBook class, XML, and the MFC Doc-View architecture.
 *
 * Revision History
 * @li 2003-10-09 DRC Added option to not filter runs by selected trial.
 * @li 2003-09-15 DRC Fixed a bug where a trial created for more than one dog
 *                    at the same time actually only created one linked entry.
 * @li 2003-08-27 DRC Added view accessors for calendar, made them public so
 *                    I don't have to use UpdateAllViews. Added methods to allow
 *                    creating titles/trials/runs from the Run view.
 * @li 2003-08-25 DRC Added GetCurrentRun().
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include <fstream>

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTree.h"
#include "AgilityBookTreeData.h"
#include "AgilityBookViewCalendar.h"
#include "AgilityBookViewCalendarList.h"
#include "AgilityBookViewTraining.h"
#include "DlgCalendar.h"
#include "DlgConfigUpdate.h"
#include "DlgConfigure.h"
#include "DlgDog.h"
#include "DlgMessage.h"
#include "DlgOptions.h"
#include "DlgSelectDog.h"
#include "DlgTraining.h"
#include "DlgTrial.h"
#include "Element.h"
#include "MainFrm.h"
#include "TabView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgilityBookDoc

IMPLEMENT_DYNCREATE(CAgilityBookDoc, CDocument)

BEGIN_MESSAGE_MAP(CAgilityBookDoc, CDocument)
	//{{AFX_MSG_MAP(CAgilityBookDoc)
	ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT_CONFIG, OnUpdateFileImportConfig)
	ON_COMMAND(ID_FILE_IMPORT_CONFIG, OnFileImportConfig)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT, OnUpdateFileExport)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_COMMAND(ID_EDIT_CONFIGURATION, OnEditConfiguration)
	ON_COMMAND(ID_AGILITY_NEW_DOG, OnAgilityNewDog)
	ON_COMMAND(ID_AGILITY_NEW_CALENDAR, OnAgilityNewCalendar)
	ON_COMMAND(ID_AGILITY_NEW_TRAINING, OnAgilityNewTraining)
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SORTRUNS, OnUpdateViewSortruns)
	ON_COMMAND(ID_VIEW_SORTRUNS, OnViewSortruns)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RUNS_BY_TRIAL, OnUpdateViewRunsByTrial)
	ON_COMMAND(ID_VIEW_RUNS_BY_TRIAL, OnViewRunsByTrial)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CAgilityBookDoc::CAgilityBookDoc()
{
}

CAgilityBookDoc::~CAgilityBookDoc()
{
}

/**
 * Return the dog associated with the currently selected item in the tree.
 */
ARBDog* CAgilityBookDoc::GetCurrentDog()
{
	ARBDog* pDog = NULL;
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	if (pTree && pTree->GetCurrentTreeItem())
		pDog = pTree->GetCurrentTreeItem()->GetDog();
	return pDog;
}

/**
 * Return the trial associated with the currently selected item in the tree.
 */
ARBDogTrial* CAgilityBookDoc::GetCurrentTrial()
{
	ARBDogTrial* pTrial = NULL;
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	if (pTree && pTree->GetCurrentTreeItem())
		pTrial = pTree->GetCurrentTreeItem()->GetTrial();
	return pTrial;
}

/**
 * Return the run associated with the currently selected item in the tree.
 */
ARBDogRun* CAgilityBookDoc::GetCurrentRun()
{
	ARBDogRun* pRun = NULL;
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	if (pTree && pTree->GetCurrentTreeItem())
		pRun = pTree->GetCurrentTreeItem()->GetRun();
	return pRun;
}

/**
 * Called from the Runs view. Since the run is visible in that view and visible
 * runs are controlled by the selected item in the tree, 'pData' should never
 * be NULL.
 */
void CAgilityBookDoc::AddTitle(ARBDogRun* pSelectedRun)
{
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	CAgilityBookTreeData* pData = pTree->FindData(TVI_ROOT, pSelectedRun);
	if (pData)
	{
		pTree->GetTreeCtrl().EnsureVisible(pData->GetHTreeItem());
		if (pData->OnCmd(ID_AGILITY_NEW_TITLE, NULL))
			SetModifiedFlag(TRUE);
	}
}

void CAgilityBookDoc::AddTrial(ARBDogRun* pSelectedRun)
{
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	CAgilityBookTreeData* pData = pTree->FindData(TVI_ROOT, pSelectedRun);
	if (pData)
	{
		pTree->GetTreeCtrl().EnsureVisible(pData->GetHTreeItem());
		if (pData->OnCmd(ID_AGILITY_NEW_TRIAL, NULL))
			SetModifiedFlag(TRUE);
	}
}

void CAgilityBookDoc::AddRun(ARBDogRun* pSelectedRun)
{
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	CAgilityBookTreeData* pData = pTree->FindData(TVI_ROOT, pSelectedRun);
	if (pData)
	{
		pTree->GetTreeCtrl().EnsureVisible(pData->GetHTreeItem());
		if (pData->OnCmd(ID_AGILITY_NEW_RUN, NULL))
			SetModifiedFlag(TRUE);
	}
}

void CAgilityBookDoc::EditRun(ARBDogRun* pRun)
{
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	CAgilityBookTreeData* pData = pTree->FindData(TVI_ROOT, pRun);
	if (pData)
	{
		pTree->GetTreeCtrl().EnsureVisible(pData->GetHTreeItem());
		if (pData->OnCmd(ID_AGILITY_EDIT_RUN, NULL))
			SetModifiedFlag(TRUE);
	}
}

void CAgilityBookDoc::DeleteRun(ARBDogRun* pRun)
{
	CAgilityBookTree* pTree = GetTreeView();
	ASSERT(pTree);
	CAgilityBookTreeData* pData = pTree->FindData(TVI_ROOT, pRun);
	if (pData)
	{
		pTree->GetTreeCtrl().EnsureVisible(pData->GetHTreeItem());
		if (pData->OnCmd(ID_AGILITY_DELETE_RUN, NULL))
			SetModifiedFlag(TRUE);
	}
}

bool CAgilityBookDoc::CreateTrialFromCalendar(const ARBCalendar& cal, CTabView* pTabView)
{
	if (!GetConfig().GetVenues().FindVenue(cal.GetVenue()))
		return false;
	bool bCreated = false;
	ARBDogTrial* pTrial = new ARBDogTrial(cal);
	CDlgTrial dlg(this, pTrial);
	if (IDOK == dlg.DoModal())
	{
		std::vector<ARBDog*> dogs;
		CDlgSelectDog dlg(this, dogs);
		if (IDOK == dlg.DoModal() && 0 < dogs.size())
		{
			for (std::vector<ARBDog*>::iterator iter = dogs.begin(); iter != dogs.end(); ++iter)
			{
				bCreated = true;
				ARBDog* pDog = *iter;
				// If we're inserting this entry into more than one dog, we
				// MUST make a copy. Otherwise the trial will be the same trial
				// for both dogs and all changes will be reflected from one to
				// the other - until you save, exit and reload the program.
				ARBDogTrial* pCopyTrial = new ARBDogTrial(*pTrial);
				ARBDogTrial* pNewTrial = pDog->GetTrials().AddTrial(pCopyTrial);
				pCopyTrial->Release();
				SetModifiedFlag();
				UpdateAllViews(NULL, UPDATE_NEW_TRIAL, reinterpret_cast<CObject*>(pNewTrial));
			}
			pTabView->SetCurSel(0);
		}
	}
	pTrial->Release();
	return bCreated;
}

void CAgilityBookDoc::SortDates()
{
	bool bDescending = !CAgilityBookOptions::GetNewestDatesFirst();
	for (ARBDogList::iterator iterDogs = GetDogs().begin(); iterDogs != GetDogs().end(); ++iterDogs)
	{
		ARBDog* pDog = *iterDogs;
		pDog->GetTrials().sort(bDescending);
		for (ARBDogTrialList::iterator iterTrial = pDog->GetTrials().begin(); iterTrial != pDog->GetTrials().end(); ++iterTrial)
		{
			ARBDogTrial* pTrial = *iterTrial;
			pTrial->GetRuns().sort(bDescending);
		}
	}
}

void CAgilityBookDoc::ResetVisibility()
{
	std::vector<CVenueFilter> venues;
	CAgilityBookOptions::GetFilterVenue(venues);
	std::set<std::string> names;
	CAgilityBookOptions::GetTrainingFilterNames(names);

	for (ARBDogList::iterator iterDogs = GetDogs().begin(); iterDogs != GetDogs().end(); ++iterDogs)
	{
		ResetVisibility(venues, *iterDogs);
	}

	for (ARBTrainingList::iterator iterTraining = GetTraining().begin(); iterTraining != GetTraining().end(); ++iterTraining)
	{
		ResetVisibility(names, *iterTraining);
	}

// Currently, calendar entries are not filtered.
	//for (ARBCalendarList::iterator iterCal = GetCalendar().begin(); iterCal != GetCalendar().end(); ++iterCal)
	//{
	//	ARBCalendar* pCal = *iterCal;
	//}

	UpdateAllViews(NULL, UPDATE_OPTIONS);
}

void CAgilityBookDoc::ResetVisibility(std::vector<CVenueFilter>& venues, ARBDog* pDog)
{
	for (ARBDogTrialList::iterator iterTrial = pDog->GetTrials().begin(); iterTrial != pDog->GetTrials().end(); ++iterTrial)
		ResetVisibility(venues, *iterTrial);

	for (ARBDogTitleList::iterator iterTitle = pDog->GetTitles().begin(); iterTitle != pDog->GetTitles().end(); ++iterTitle)
		ResetVisibility(venues, *iterTitle);
}

void CAgilityBookDoc::ResetVisibility(std::vector<CVenueFilter>& venues, ARBDogTrial* pTrial)
{
	bool bVisTrial = CAgilityBookOptions::IsTrialVisible(venues, pTrial);
	pTrial->SetFiltered(!bVisTrial);
	if (bVisTrial)
	{
		int nVisible = 0;
		for (ARBDogRunList::iterator iterRun = pTrial->GetRuns().begin(); iterRun != pTrial->GetRuns().end(); ++iterRun)
		{
			ResetVisibility(venues, pTrial, *iterRun);
			if (!(*iterRun)->IsFiltered())
				++nVisible;
		}
		if (0 == nVisible && 0 < pTrial->GetRuns().size())
			pTrial->SetFiltered(true);
	}
}

void CAgilityBookDoc::ResetVisibility(std::vector<CVenueFilter>& venues, ARBDogTrial* pTrial, ARBDogRun* pRun)
{
	bool bVisRun = CAgilityBookOptions::IsRunVisible(venues, pTrial, pRun);
	pRun->SetFiltered(!bVisRun);
}

void CAgilityBookDoc::ResetVisibility(std::vector<CVenueFilter>& venues, ARBDogTitle* pTitle)
{
	bool bVisTitle = CAgilityBookOptions::IsTitleVisible(venues, pTitle);
	pTitle->SetFiltered(!bVisTitle);
}

void CAgilityBookDoc::ResetVisibility(std::set<std::string>& names, ARBTraining* pTraining)
{
	bool bVisTraining = CAgilityBookOptions::IsTrainingLogVisible(names, pTraining);
	pTraining->SetFiltered(!bVisTraining);
}

/**
 * Function to get the tree view. This is used internally and by the runs view.
 */
CAgilityBookTree* CAgilityBookDoc::GetTreeView() const
{
	POSITION pos = this->GetFirstViewPosition();
	while (NULL != pos)
	{
		CView* pView = GetNextView(pos);
		CAgilityBookTree* pView2 = DYNAMIC_DOWNCAST(CAgilityBookTree, pView);
		if (pView2)
			return pView2;
	}
	ASSERT(0);
	return NULL;
}

/**
 * Function to get the calendar list view. This is used by the calendar view.
 */
CAgilityBookViewCalendarList* CAgilityBookDoc::GetCalendarListView() const
{
	POSITION pos = this->GetFirstViewPosition();
	while (NULL != pos)
	{
		CView* pView = GetNextView(pos);
		CAgilityBookViewCalendarList* pView2 = DYNAMIC_DOWNCAST(CAgilityBookViewCalendarList, pView);
		if (pView2)
			return pView2;
	}
	ASSERT(0);
	return NULL;
}

/**
 * Function to get the calendar view. This is used by the calendar list view.
 */
CAgilityBookViewCalendar* CAgilityBookDoc::GetCalendarView() const
{
	POSITION pos = this->GetFirstViewPosition();
	while (NULL != pos)
	{
		CView* pView = GetNextView(pos);
		CAgilityBookViewCalendar* pView2 = DYNAMIC_DOWNCAST(CAgilityBookViewCalendar, pView);
		if (pView2)
			return pView2;
	}
	ASSERT(0);
	return NULL;
}

/**
 * MFC method to delete contents of current document.
 */
void CAgilityBookDoc::DeleteContents()
{
	m_Records.clear();
	CDocument::DeleteContents();
	SetModifiedFlag(FALSE);
}

/**
 * MFC method to create a new empty document.
 */
BOOL CAgilityBookDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	AfxGetApp()->WriteProfileString("Settings", "LastFile", _T(""));
	m_Records.Default();
	return TRUE;
}

/**
 * Since we are not using the CArchive serialization (via Serialize()), we
 * have to provide a full implementation of the open and save routines.
 * We do not want the CArchive stuff since we're using Xerces to read the XML.
 */
BOOL CAgilityBookDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CFileStatus status;
	if (!CFile::GetStatus(lpszPathName, status))
	{
		AfxGetApp()->WriteProfileString("Settings", "LastFile", _T(""));
		ReportSaveLoadException(lpszPathName, NULL, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}

	CWaitCursor wait;

	// Do standard CDocument stuff.
	if (IsModified())
	{
		TRACE0("Warning: OnOpenDocument replaces an unsaved document.\n");
	}
	DeleteContents();

	// We need a char*, even in UNICODE.
#if _MSC_VER < 1300
	// But since I'm not compiling in unicode...
	CString source(lpszPathName);
#else
	CStringA source(lpszPathName);
#endif
	CElement tree;
	// Translate the XML to a tree form.
	if (!tree.LoadXMLFile((const char*)source))
	{
		AfxGetApp()->WriteProfileString("Settings", "LastFile", _T(""));
		ReportSaveLoadException(lpszPathName, NULL, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}
	// Translate the tree to a class structure.
	if (!m_Records.Load(tree))
	{
		AfxGetApp()->WriteProfileString("Settings", "LastFile", _T(""));
		ReportSaveLoadException(lpszPathName, NULL, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}
	SortDates();

	ResetVisibility();

	SetModifiedFlag(FALSE);     // start off with unmodified

	if (CAgilityBookOptions::AutoDeleteCalendarEntries())
	{
		ARBDate today(ARBDate::Today());
		today -= CAgilityBookOptions::DaysTillEntryIsPast();
		if (0 < m_Records.GetCalendar().TrimEntries(today))
			SetModifiedFlag(TRUE);
	}

	AfxGetApp()->WriteProfileString("Settings", "LastFile", lpszPathName);
	return TRUE;
}

/**
 * MFC method to save a document.
 */
BOOL CAgilityBookDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CWaitCursor wait;

	BOOL bOk = FALSE;
	CElement tree;
	// First, we have to push all the class data into a tree.
	if (m_Records.Save(tree))
	{
		// Then we can stream that tree out as XML.
		std::ofstream output(lpszPathName, std::ios::out | std::ios::binary);
		output.exceptions(std::ios_base::badbit);
		if (output.is_open())
		{
			if (tree.SaveXML(output))
			{
				AfxGetApp()->WriteProfileString("Settings", "LastFile", lpszPathName);
				bOk = TRUE;
				SetModifiedFlag(FALSE);
			}
			output.close();
		}
		else
		{
			CString errMsg;
			errMsg.FormatMessage(IDS_MIN_RESOLUTION, lpszPathName);
			AfxMessageBox(errMsg, MB_OK | MB_ICONEXCLAMATION);
		}
	}
	return bOk;
}

#ifdef _DEBUG
// CAgilityBookDoc diagnostics
void CAgilityBookDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAgilityBookDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// CAgilityBookDoc commands

void CAgilityBookDoc::OnUpdateFileImportConfig(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CAgilityBookDoc::OnFileImportConfig()
{
	CDlgConfigUpdate dlg;
	if (IDOK == dlg.DoModal())
	{
		ARBConfig& update = dlg.GetConfig();
		CString msg;
		msg = GetConfig().Update(0, update).c_str();
		if (0 < msg.GetLength())
		{
			CDlgMessage dlg(msg, 0);
			dlg.DoModal();
			SetModifiedFlag();
			UpdateAllViews(NULL, UPDATE_CONFIG);
		}
		else
			AfxMessageBox(IDS_CONFIG_NO_UPDATE, MB_ICONINFORMATION);
	}
}

#if 0
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
	CFileDialog file(TRUE, "txt", "*.txt", OFN_FILEMUSTEXIST, "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||");
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

void CAgilityBookDoc::OnUpdateFileExport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

void CAgilityBookDoc::OnFileExport()
{
	// @todo: Write file export code
}

void CAgilityBookDoc::OnEditConfiguration()
{
	CDlgConfigure config(this, m_Records);
	config.DoModal();
}

void CAgilityBookDoc::OnAgilityNewDog()
{
	ARBDog* dog = new ARBDog;
	CDlgDog dlg(GetConfig(), dog);
	if (IDOK == dlg.DoModal())
	{
		CAgilityBookTree* pTree = NULL;
		POSITION pos = GetFirstViewPosition();
		while (NULL != pos)
		{
			CView* pView = GetNextView(pos);
			if (DYNAMIC_DOWNCAST(CAgilityBookTree, pView))
			{
				pTree = reinterpret_cast<CAgilityBookTree*>(pView);
				break;
			}
		}
		if (pTree)
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->SetCurTab(0);
			SetModifiedFlag();
			ARBDog* pNewDog = GetDogs().AddDog(dog);
			pTree->InsertDog(pNewDog, true);
		}
	}
	dog->Release();
}

void CAgilityBookDoc::OnAgilityNewCalendar()
{
	ARBCalendar* cal = new ARBCalendar;
	CDlgCalendar dlg(cal, this);
	if (IDOK == dlg.DoModal())
	{
		if (!(CAgilityBookOptions::AutoDeleteCalendarEntries() && cal->GetEndDate() < ARBDate::Today()))
		{
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			pFrame->SetCurTab(2);
			GetCalendar().AddCalendar(cal);
			GetCalendar().sort();
			SetModifiedFlag();
			UpdateAllViews(NULL, UPDATE_CALENDAR_VIEW);
			POSITION pos = GetFirstViewPosition();
			while (NULL != pos)
			{
				CView* pView = GetNextView(pos);
				if (DYNAMIC_DOWNCAST(CAgilityBookViewCalendar, pView))
				{
					reinterpret_cast<CAgilityBookViewCalendar*>(pView)->SetCurrentDate(cal->GetStartDate(), true);
					break;
				}
			}
		}
	}
	cal->Release();
}

void CAgilityBookDoc::OnAgilityNewTraining()
{
	ARBTraining* training = new ARBTraining();
	CDlgTraining dlg(training, this);
	if (IDOK == dlg.DoModal())
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->SetCurTab(3);
		GetTraining().AddTraining(training);
		GetTraining().sort();
		SetModifiedFlag();
		UpdateAllViews(NULL, UPDATE_TRAINING_VIEW);
		POSITION pos = GetFirstViewPosition();
		while (NULL != pos)
		{
			CView* pView = GetNextView(pos);
			if (DYNAMIC_DOWNCAST(CAgilityBookViewTraining, pView))
			{
				reinterpret_cast<CAgilityBookViewTraining*>(pView)->SetCurrentDate(training->GetDate());
				break;
			}
		}
	}
	training->Release();
}

void CAgilityBookDoc::OnViewOptions()
{
	int nPage = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (2 == pFrame->GetCurTab())
		nPage = 1;
	else if (3 == pFrame->GetCurTab())
		nPage = 2;
	CDlgOptions options(this, AfxGetMainWnd(), nPage);
	options.DoModal();
}

void CAgilityBookDoc::OnUpdateViewSortruns(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(CAgilityBookOptions::GetNewestDatesFirst() ? 1 : 0);
}

void CAgilityBookDoc::OnViewSortruns() 
{
	CAgilityBookOptions::SetNewestDatesFirst(!CAgilityBookOptions::GetNewestDatesFirst());
	SortDates();
	UpdateAllViews(NULL, UPDATE_OPTIONS);
}

void CAgilityBookDoc::OnUpdateViewRunsByTrial(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CAgilityBookOptions::GetViewRunsByTrial() ? 1 : 0);
}

void CAgilityBookDoc::OnViewRunsByTrial()
{
	CAgilityBookOptions::SetViewRunsByTrial(!CAgilityBookOptions::GetViewRunsByTrial());
	UpdateAllViews(NULL, UPDATE_OPTIONS);
}
