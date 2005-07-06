/*
 * Copyright © 2003-2005 David Connet. All Rights Reserved.
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
 * @brief Import/Export Wizard
 * @author David Connet
 *
 * Revision History
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-10 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-08-21 DRC Added ability to import dog/trial/run/judge info.
 * @li 2004-02-26 DRC Moved configuration update logic to the doc.
 * @li 2004-02-18 DRC Added 'DeleteTitle' configuration action.
 * @li 2004-01-26 DRC Display errors on non-fatal load.
 * @li 2004-01-21 DRC Implemented Action items in configuration update.
 * @li 2003-12-10 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "WizardStart.h"
#include <fstream>

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "DlgMessage.h"
#include "Element.h"
#include "Wizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizardStart property page

IMPLEMENT_DYNAMIC(CWizardStart, CDlgBasePropertyPage)

CWizardStart::CWizardStart(
		CWizard* pSheet,
		CAgilityBookDoc* pDoc)
	: CDlgBasePropertyPage(CWizardStart::IDD)
	, m_pSheet(pSheet)
	, m_pDoc(pDoc)
{
	//{{AFX_DATA_INIT(CWizardStart)
	m_Style = WIZARD_RADIO_EXCEL;
	//}}AFX_DATA_INIT
	if (!m_pSheet->ExcelHelper().IsAvailable())
		m_Style = WIZARD_RADIO_SPREADSHEET;
}

CWizardStart::~CWizardStart()
{
}

void CWizardStart::DoDataExchange(CDataExchange* pDX)
{
	CDlgBasePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardStart)
	DDX_Radio(pDX, IDC_WIZARD_START_EXCEL, m_Style);
	DDX_Control(pDX, IDC_WIZARD_START_LIST, m_ctrlList);
	DDX_Control(pDX, IDC_WIZARD_START_DESCRIPTION, m_ctrlDesc);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWizardStart, CDlgBasePropertyPage)
	//{{AFX_MSG_MAP(CWizardStart)
	ON_LBN_SELCHANGE(IDC_WIZARD_START_LIST, OnSelchangeExportList)
	ON_LBN_DBLCLK(IDC_WIZARD_START_LIST, OnDblclkExportList)
	ON_BN_CLICKED(IDC_WIZARD_START_EXCEL, OnWizardStyle)
	ON_BN_CLICKED(IDC_WIZARD_START_SPREADSHEET, OnWizardStyle)
	ON_BN_CLICKED(IDC_WIZARD_START_ARB, OnWizardStyle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

static struct
{
	// For integrity checking. See Wizard.h.
	int index;
	struct
	{
		// The next button for the wizard. Also used when dbl-click on item.
		// PSWIZB_FINISH infers that nextPage is -1 (and visa-versa)
		// This also means that we handle this case in our Finsh code below.
		int nButton;
		// -1: Prevent page from changing
		// 0: Automatic next page
		// other: ID of dialog
		LRESULT nextPage;
		// Listing (NULL denotes no entry)
		char const* item;
		// Description shown when listing is selected.
		char const* desc;
	} data[3]; // Data must agree with WIZARD_RADIO_* defines.
} const sc_Items[] =
{
	{WIZ_IMPORT_RUNS,
	{
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT,
			"Import Trials and Runs",
			"Import trial and run information from Excel.\n\nWhen importing, each run must have a valid (as defined in the Configuration) Venue, Event, Division and Level. Also, if a trial is dual-sanctioned, the '/' character is assumed to separate the list of venue names and club names."
		},
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT,
			"Import Trials and Runs",
			"Import trial and run information from a spreadsheet. Data must be in a text format.\n\nWhen importing, each run must have a valid (as defined in the Configuration) Venue, Event, Division and Level. Also, if a trial is dual-sanctioned, the '/' character is assumed to separate the list of venue names and club names."
		},
		{PSWIZB_FINISH, -1,
			"Import Dogs, Trials, Runs and Judges",
			"Import dog, trial, run and judge information from Agility Record Book."
		}
	} },
	{WIZ_EXPORT_RUNS,
	{
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			"Export Runs",
			"Export trial and run information to Excel."
		},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			"Export Runs",
			"Export trial and run information so it can be imported into a spreadsheet."
		},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL}
	} },
	{WIZ_IMPORT_CALENDAR,
	{
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT,
			"Import Calendar",
			"Import a calendar listing from Excel."
		},
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT,
			"Import Calendar",
			"Import a calendar listing from a spreadsheet."
		},
		{PSWIZB_FINISH, -1,
			"Import Calendar",
			"Import a calendar listing that was exported from Agility Record Book."
		}
	} },
	{WIZ_EXPORT_CALENDAR,
	{
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			"Export Calendar",
			"Export your calendar listing to Excel."
		},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			"Export Calendar",
			"Export your calendar listing to a spreadsheet."
		},
		{PSWIZB_FINISH, -1,
			"Export Calendar",
			"Export your calendar listing so it can be imported into Agility Record Book."
		}
	} },
	{WIZ_EXPORT_CALENDAR_ICAL,
	{
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_FINISH, -1,
			"Export Calendar (iCalendar)",
			"Export calendar entries in iCalendar (.ics) format so they can be imported into other programs."
		},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL}
	} },
	{WIZ_EXPORT_CALENDAR_APPT,
	{
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			"Export Calendar (MS Outlook Appointment)",
			"Export calendar entries to Excel so they can be imported into Microsoft Outlook as Appointments."
		},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			"Export Calendar (MS Outlook Appointment)",
			"Export calendar entries to a spreadsheet so they can be imported into Microsoft Outlook as Appointments."
		},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL}
	} },
	{WIZ_EXPORT_CALENDAR_TASK,
	{
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			"Export Calendar (MS Outlook Task)",
			"Export calendar entries to Excel so they can be imported into Microsoft Outlook as Tasks. Only Calendar entries that at marked as 'Planning' will be exported."
		},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			"Export Calendar (MS Outlook Task)",
			"Export calendar entries to a spreadsheet so they can be imported into Microsoft Outlook as Tasks. Only Calendar entries that at marked as 'Planning' will be exported."
		},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL}
	} },
	{WIZ_IMPORT_LOG,
	{
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT,
			"Import Training Log",
			"Import a Training Log from Excel."
		},
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT,
			"Import Training Log",
			"Import a Training Log from a spreadsheet."
		},
		{PSWIZB_FINISH, -1,
			"Import Training Log",
			"Import a Training Log that was exported from Agility Record Book."
		}
	} },
	{WIZ_EXPORT_LOG,
	{
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			"Export Training Log",
			"Export your Training Log to Excel."
		},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			"Export Training Log",
			"Export your Training Log to a spreadsheet."
		},
		{PSWIZB_FINISH, -1,
			"Export Training Log",
			"Export your Training Log so it can be imported into Agility Record Book."
		}
	} },
	{WIZ_IMPORT_CONFIGURATION,
	{
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_FINISH, -1,
			"Import Configuration",
			"Update your configuration to support new and/or updated venues."
		}
	} },
	{WIZ_EXPORT_CONFIGURATION,
	{
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_FINISH, -1,
			"Export Configuration",
			"Export your configuration so it can be imported into Agility Record Book."
		}
	} },
	{WIZ_EXPORT_DTD,
	{
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_FINISH, -1,
			"Export DTD",
			"Export the Document Type Definition. This data describes the XML format of the data file."
		}
	} },
	{WIZ_EXPORT_XML,
	{
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_FINISH, -1,
			"Export File as XML",
			"Export your data file as an XML file. The DTD will be contained within this file."
		}
	} },
};
static int const sc_nItems = sizeof(sc_Items) / sizeof(sc_Items[0]);

void CWizardStart::UpdateList()
{
	m_ctrlList.ResetContent();
	m_ctrlDesc.SetWindowText("");
	for (int i = 0; i < sc_nItems; ++i)
	{
		ASSERT(sc_Items[i].index == i);
		bool bAdd = true;
		if (m_pSheet->GetCalendarEntries()
		&& sc_Items[i].index != WIZ_EXPORT_CALENDAR_ICAL
		&& sc_Items[i].index != WIZ_EXPORT_CALENDAR_APPT
		&& sc_Items[i].index != WIZ_EXPORT_CALENDAR_TASK)
			bAdd = false;
		if (bAdd && NULL == sc_Items[i].data[m_Style].item)
			bAdd = false;
		int index = LB_ERR;
		if (bAdd)
			index = m_ctrlList.AddString(sc_Items[i].data[m_Style].item);
		if (LB_ERR != index)
			m_ctrlList.SetItemData(index, i);
	}
	m_ctrlList.SetCurSel(-1);
	UpdateButtons();
}

void CWizardStart::UpdateButtons()
{
	DWORD dwWiz = 0;
	int index = m_ctrlList.GetCurSel();
	if (-1 != m_Style && LB_ERR != index)
	{
		int data = static_cast<int>(m_ctrlList.GetItemData(index));
		dwWiz |= sc_Items[data].data[m_Style].nButton;
	}
	m_pSheet->SetWizardButtons(dwWiz);
}

/////////////////////////////////////////////////////////////////////////////
// CWizardStart message handlers

BOOL CWizardStart::OnInitDialog() 
{
	CDlgBasePropertyPage::OnInitDialog();
	if (!m_pSheet->ExcelHelper().IsAvailable())
		GetDlgItem(IDC_WIZARD_START_EXCEL)->EnableWindow(FALSE);
	UpdateList();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWizardStart::OnSetActive() 
{
	UpdateButtons();
	return CDlgBasePropertyPage::OnSetActive();
}

LRESULT CWizardStart::OnWizardNext() 
{
	LRESULT nextPage = -1;
	int index = m_ctrlList.GetCurSel();
	if (-1 != m_Style && LB_ERR != index)
	{
		int data = static_cast<int>(m_ctrlList.GetItemData(index));
		nextPage = sc_Items[data].data[m_Style].nextPage;
		m_pSheet->SetImportExportItem(data, m_Style);
	}
	return nextPage;
}

BOOL CWizardStart::OnWizardFinish() 
{
	bool bOk = false;
	int index = m_ctrlList.GetCurSel();
	if (-1 != m_Style && LB_ERR != index)
	{
		int data = static_cast<int>(m_ctrlList.GetItemData(index));
		switch (data)
		{
		default:
			break;

		case WIZ_IMPORT_RUNS:
			{
				CString def, fname, filter;
				def.LoadString(IDS_FILEEXT_DEF_ARB);
				fname.LoadString(IDS_FILEEXT_FNAME_ARB);
				filter.LoadString(IDS_FILEEXT_FILTER_ARB);
				CFileDialog file(TRUE, def, fname, OFN_FILEMUSTEXIST, filter, this);
				if (IDOK == file.DoModal())
				{
					AfxGetMainWnd()->UpdateWindow();
					CWaitCursor wait;
					std::string errMsg;
					Element tree;
					if (!tree.LoadXMLFile(file.GetPathName(), errMsg))
					{
						CString msg;
						msg.LoadString(AFX_IDP_FAILED_TO_OPEN_DOC);
						if (0 < errMsg.length())
						{
							msg += "\n\n";
							msg += errMsg.c_str();
						}
						AfxMessageBox(msg, MB_ICONEXCLAMATION);
					}
					else
					{
						CErrorCallback err;
						ARBAgilityRecordBook book;
						if (book.Load(tree, false, false, true, true, true, err))
						{
							if (0 < err.m_ErrMsg.length())
								AfxMessageBox(err.m_ErrMsg.c_str(), MB_ICONINFORMATION);
							int countDog = 0;
							int countRegNums = 0;
							int countExistingPts = 0;
							int countTitles = 0;
							int countTrials = 0;
							int countClubs = 0;
							int countJudges = 0;
							int countLocations = 0;
							for (ARBDogList::iterator iterDog = book.GetDogs().begin();
								iterDog != book.GetDogs().end();
								++iterDog)
							{
								ARBDog* pDog = *iterDog;
								ARBDog* pExisting = NULL;
								for (ARBDogList::iterator iterDog = m_pDoc->GetDogs().begin();
									iterDog != m_pDoc->GetDogs().end();
									++iterDog)
								{
									if ((*iterDog)->GetCallName() == pDog->GetCallName())
									{
										pExisting = *iterDog;
										break;
									}
								}
								if (!pExisting)
								{
									++countDog;
									m_pDoc->GetDogs().AddDog(pDog);
								}
								else if (*pExisting != *pDog)
								{
									// If the dog exists, only update the
									// existing points, registration numbers,
									// titles and trials.
									if (pExisting->GetRegNums() != pDog->GetRegNums())
									{
										for (ARBDogRegNumList::iterator iter = pDog->GetRegNums().begin();
											iter != pDog->GetRegNums().end();
											++iter)
										{
											if (!pExisting->GetRegNums().FindRegNum((*iter)->GetVenue()))
											{
												++countRegNums;
												pExisting->GetRegNums().AddRegNum((*iter));
											}
										}
									}
									if (pExisting->GetExistingPoints() != pDog->GetExistingPoints())
									{
										for (ARBDogExistingPointsList::iterator iter = pDog->GetExistingPoints().begin();
											iter != pDog->GetExistingPoints().end();
											++iter)
										{
											bool bFound = false;
											for (ARBDogExistingPointsList::iterator iter2 = pExisting->GetExistingPoints().begin();
												iter2 != pExisting->GetExistingPoints().end();
												++iter2)
											{
												if (*(*iter) == *(*iter2))
												{
													bFound = true;
													break;
												}
											}
											if (!bFound)
											{
												++countExistingPts;
												pExisting->GetExistingPoints().AddExistingPoints((*iter));
											}
										}
									}
									if (pExisting->GetTitles() != pDog->GetTitles())
									{
										for (ARBDogTitleList::iterator iter = pDog->GetTitles().begin();
											iter != pDog->GetTitles().end();
											++iter)
										{
											if (!pExisting->GetTitles().FindTitle((*iter)->GetVenue(), (*iter)->GetRawName()))
											{
												++countTitles;
												ARBDogTitle* pNewTitle = new ARBDogTitle(*(*iter));
												pExisting->GetTitles().AddTitle(pNewTitle);
												pNewTitle->Release();
											}
										}
									}
									if (pExisting->GetTrials() != pDog->GetTrials())
									{
										for (ARBDogTrialList::iterator iter = pDog->GetTrials().begin();
											iter != pDog->GetTrials().end();
											++iter)
										{
											bool bFound = false;
											for (ARBDogTrialList::iterator iter2 = pExisting->GetTrials().begin();
												iter2 != pExisting->GetTrials().end();
												++iter2)
											{
												if (*(*iter) == *(*iter2))
												{
													bFound = true;
													break;
												}
											}
											if (!bFound)
											{
												++countTrials;
												pExisting->GetTrials().AddTrial((*iter));
											}
										}
									}
								}
							}
							for (ARBInfoItemList::const_iterator iterClub = book.GetInfo().GetInfo(ARBInfo::eClubInfo).begin();
								iterClub != book.GetInfo().GetInfo(ARBInfo::eClubInfo).end();
								++iterClub)
							{
								ARBInfoItem* pClub = *iterClub;
								// If this fails, it already exists.
								if (m_pDoc->GetARB().GetInfo().GetInfo(ARBInfo::eClubInfo).AddItem(pClub))
								{
									++countClubs;
								}
							}
							for (ARBInfoItemList::const_iterator iterJudge = book.GetInfo().GetInfo(ARBInfo::eJudgeInfo).begin();
								iterJudge != book.GetInfo().GetInfo(ARBInfo::eJudgeInfo).end();
								++iterJudge)
							{
								ARBInfoItem* pJudge = *iterJudge;
								// If this fails, it already exists.
								if (m_pDoc->GetARB().GetInfo().GetInfo(ARBInfo::eJudgeInfo).AddItem(pJudge))
								{
									++countJudges;
								}
							}
							for (ARBInfoItemList::const_iterator iterLocation = book.GetInfo().GetInfo(ARBInfo::eLocationInfo).begin();
								iterLocation != book.GetInfo().GetInfo(ARBInfo::eLocationInfo).end();
								++iterLocation)
							{
								ARBInfoItem* pLocation = *iterLocation;
								// If this fails, it already exists.
								if (m_pDoc->GetARB().GetInfo().GetInfo(ARBInfo::eLocationInfo).AddItem(pLocation))
								{
									++countLocations;
								}
							}
							if (0 < countDog
							|| 0 < countRegNums
							|| 0 < countExistingPts
							|| 0 < countTitles
							|| 0 < countTrials)
							{
								m_pDoc->UpdateAllViews(NULL, UPDATE_ALL_VIEW);
								m_pDoc->SetModifiedFlag();
							}
							if (0 < countClubs)
							{
								std::set<std::string> namesInUse;
								m_pDoc->GetAllClubNames(namesInUse, false);
								m_pDoc->GetARB().GetInfo().GetInfo(ARBInfo::eClubInfo).CondenseContent(namesInUse);
							}
							if (0 < countJudges)
							{
								std::set<std::string> namesInUse;
								m_pDoc->GetAllJudges(namesInUse, false);
								m_pDoc->GetARB().GetInfo().GetInfo(ARBInfo::eJudgeInfo).CondenseContent(namesInUse);
							}
							if (0 < countLocations)
							{
								std::set<std::string> namesInUse;
								m_pDoc->GetAllTrialLocations(namesInUse, false);
								m_pDoc->GetARB().GetInfo().GetInfo(ARBInfo::eLocationInfo).CondenseContent(namesInUse);
							}
							//"Added %1!d! new dogs, updated %2!d! dogs and added %3!d! judges."
							CString str("Added ");
							bool bAdded = false;
							if (0 < countDog)
							{
								if (bAdded)
									str += ", ";
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_DOGS, countDog);
								str += str2;
							}
							if (0 < countRegNums)
							{
								if (bAdded)
									str += ", ";
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_REGNUMS, countRegNums);
								str += str2;
							}
							if (0 < countExistingPts)
							{
								if (bAdded)
									str += ", ";
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_EXISTINGPTS, countExistingPts);
								str += str2;
							}
							if (0 < countTitles)
							{
								if (bAdded)
									str += ", ";
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_TITLES, countTitles);
								str += str2;
							}
							if (0 < countTrials)
							{
								if (bAdded)
									str += ", ";
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_TRIALS, countTrials);
								str += str2;
							}
							if (0 < countClubs)
							{
								if (bAdded)
									str += ", ";
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_CLUBS, countClubs);
								str += str2;
							}
							if (0 < countJudges)
							{
								if (bAdded)
									str += ", ";
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_JUDGES, countJudges);
								str += str2;
							}
							if (0 < countLocations)
							{
								if (bAdded)
									str += ", ";
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_LOCATIONS, countLocations);
								str += str2;
							}
							AfxMessageBox(str, MB_ICONINFORMATION);
							bOk = true;
						}
						else if (0 < err.m_ErrMsg.length())
							AfxMessageBox(err.m_ErrMsg.c_str(), MB_ICONWARNING);
					}
				}
			}
			break;

		case WIZ_IMPORT_CALENDAR:
			{
				CString def, fname, filter;
				def.LoadString(IDS_FILEEXT_DEF_ARB);
				fname.LoadString(IDS_FILEEXT_FNAME_ARB);
				filter.LoadString(IDS_FILEEXT_FILTER_ARB);
				CFileDialog file(TRUE, def, fname, OFN_FILEMUSTEXIST, filter, this);
				if (IDOK == file.DoModal())
				{
					AfxGetMainWnd()->UpdateWindow();
					CWaitCursor wait;
					std::string errMsg;
					Element tree;
					if (!tree.LoadXMLFile(file.GetPathName(), errMsg))
					{
						CString msg;
						msg.LoadString(AFX_IDP_FAILED_TO_OPEN_DOC);
						if (0 < errMsg.length())
						{
							msg += "\n\n";
							msg += errMsg.c_str();
						}
						AfxMessageBox(msg, MB_ICONEXCLAMATION);
					}
					else
					{
						CErrorCallback err;
						ARBAgilityRecordBook book;
						if (book.Load(tree, true, false, false, false, false, err))
						{
							if (0 < err.m_ErrMsg.length())
								AfxMessageBox(err.m_ErrMsg.c_str(), MB_ICONINFORMATION);
							int count = 0;
							for (ARBCalendarList::iterator iter = book.GetCalendar().begin(); iter != book.GetCalendar().end(); ++iter)
							{
								ARBCalendar* cal = *iter;
								if (!m_pDoc->GetARB().GetCalendar().FindCalendar(cal))
								{
									if (!(CAgilityBookOptions::AutoDeleteCalendarEntries() && cal->GetEndDate() < ARBDate::Today()))
									{
										m_pDoc->GetARB().GetCalendar().AddCalendar(cal);
										++count;
									}
								}
							}
							if (0 < count)
							{
								m_pDoc->GetARB().GetCalendar().sort();
								m_pDoc->UpdateAllViews(NULL, UPDATE_CALENDAR_VIEW);
								m_pDoc->SetModifiedFlag();
							}
							CString str;
							str.FormatMessage(IDS_ADDED_CAL_ITEMS, count);
							AfxMessageBox(str, MB_ICONINFORMATION);
							bOk = true;
						}
						else if (0 < err.m_ErrMsg.length())
							AfxMessageBox(err.m_ErrMsg.c_str(), MB_ICONWARNING);
					}
				}
			}
			break;

		case WIZ_EXPORT_CALENDAR:
			{
				CString def, fname, filter;
				def.LoadString(IDS_FILEEXT_DEF_ARB);
				fname.LoadString(IDS_FILEEXT_FNAME_ARB);
				filter.LoadString(IDS_FILEEXT_FILTER_ARB);
				CFileDialog file(FALSE, def, fname, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST, filter, this);
				if (IDOK == file.DoModal())
				{
					CVersionNum ver;
					std::string verstr = (LPCTSTR)ver.GetVersionString();
					Element tree;
					if (m_pDoc->GetARB().Save(tree, verstr, true, false, false, false, false))
					{
						std::ofstream output(file.GetFileName(), std::ios::out | std::ios::binary);
						output.exceptions(std::ios_base::badbit);
						if (output.is_open())
						{
							tree.SaveXML(output);
							output.close();
						}
					}
					bOk = true;
				}
			}
			break;

		case WIZ_EXPORT_CALENDAR_ICAL:
			{
				CString def, fname, filter;
				def.LoadString(IDS_FILEEXT_DEF_ICS);
				fname.LoadString(IDS_FILEEXT_FNAME_ICS);
				filter.LoadString(IDS_FILEEXT_FILTER_ICS);
				CFileDialog file(FALSE, def, fname, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST, filter, this);
				if (IDOK == file.DoModal())
				{
					ARBVectorBase<ARBCalendar> allEntries;
					ARBVectorBase<ARBCalendar>* entries = m_pSheet->GetCalendarEntries();
					if (!entries)
					{
						allEntries.reserve(m_pDoc->GetCalendar().size());
						for (ARBCalendarList::const_iterator iterCal = m_pDoc->GetCalendar().begin(); iterCal != m_pDoc->GetCalendar().end(); ++iterCal)
						{
							(*iterCal)->AddRef();
							allEntries.push_back(*iterCal);
						}
						entries = &allEntries;
					}
					std::ofstream output(file.GetFileName(), std::ios::out | std::ios::binary);
					output.exceptions(std::ios_base::badbit);
					if (output.is_open())
					{
						int nWarning = CAgilityBookOptions::CalendarOpeningNear();
						ARBCalendar::iCalendarBegin(output);
						for (ARBVectorBase<ARBCalendar>::const_iterator iterCal = entries->begin(); iterCal != entries->end(); ++iterCal)
						{
							ARBCalendar const* pCal = *iterCal;
							pCal->iCalendar(output, nWarning);
						}
						ARBCalendar::iCalendarEnd(output);
						output.close();
					}
					bOk = true;
				}
			}
			break;

		case WIZ_IMPORT_LOG:
			{
				CString def, fname, filter;
				def.LoadString(IDS_FILEEXT_DEF_ARB);
				fname.LoadString(IDS_FILEEXT_FNAME_ARB);
				filter.LoadString(IDS_FILEEXT_FILTER_ARB);
				CFileDialog file(TRUE, def, fname, OFN_FILEMUSTEXIST, filter, this);
				if (IDOK == file.DoModal())
				{
					AfxGetMainWnd()->UpdateWindow();
					CWaitCursor wait;
					std::string errMsg;
					Element tree;
					if (!tree.LoadXMLFile(file.GetPathName(), errMsg))
					{
						CString msg;
						msg.LoadString(AFX_IDP_FAILED_TO_OPEN_DOC);
						if (0 < errMsg.length())
						{
							msg += "\n\n";
							msg += errMsg.c_str();
						}
						AfxMessageBox(msg, MB_ICONEXCLAMATION);
					}
					else
					{
						CErrorCallback err;
						ARBAgilityRecordBook book;
						if (book.Load(tree, false, true, false, false, false, err))
						{
							if (0 < err.m_ErrMsg.length())
								AfxMessageBox(err.m_ErrMsg.c_str(), MB_ICONINFORMATION);
							int count = 0;
							for (ARBTrainingList::iterator iter = book.GetTraining().begin(); iter != book.GetTraining().end(); ++iter)
							{
								ARBTraining* item = *iter;
								if (!m_pDoc->GetARB().GetTraining().FindTraining(item))
								{
									m_pDoc->GetARB().GetTraining().AddTraining(item);
									++count;
								}
							}
							if (0 < count)
							{
								m_pDoc->GetTraining().sort();
								m_pDoc->UpdateAllViews(NULL, UPDATE_TRAINING_VIEW);
								m_pDoc->SetModifiedFlag();
							}
							CString str;
							str.FormatMessage(IDS_ADDED_TRAINING_ITEMS, count);
							AfxMessageBox(str, MB_ICONINFORMATION);
							bOk = true;
						}
						else if (0 < err.m_ErrMsg.length())
							AfxMessageBox(err.m_ErrMsg.c_str(), MB_ICONWARNING);
					}
				}
			}
			break;

		case WIZ_EXPORT_LOG:
			{
				CString def, fname, filter;
				def.LoadString(IDS_FILEEXT_DEF_ARB);
				fname.LoadString(IDS_FILEEXT_FNAME_ARB);
				filter.LoadString(IDS_FILEEXT_FILTER_ARB);
				CFileDialog file(FALSE, def, fname, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST, filter, this);
				if (IDOK == file.DoModal())
				{
					CVersionNum ver;
					std::string verstr = (LPCTSTR)ver.GetVersionString();
					Element tree;
					if (m_pDoc->GetARB().Save(tree, verstr, false, true, false, false, false))
					{
						std::ofstream output(file.GetFileName(), std::ios::out | std::ios::binary);
						output.exceptions(std::ios_base::badbit);
						if (output.is_open())
						{
							tree.SaveXML(output);
							output.close();
						}
					}
					bOk = true;
				}
			}
			break;

		case WIZ_IMPORT_CONFIGURATION:
			bOk = m_pDoc->ImportConfiguration(false);
			if (bOk)
				m_pDoc->SetModifiedFlag();
			break;

		case WIZ_EXPORT_CONFIGURATION:
			{
				CString def, fname, filter;
				def.LoadString(IDS_FILEEXT_DEF_ARB);
				fname.LoadString(IDS_FILEEXT_FNAME_ARB);
				filter.LoadString(IDS_FILEEXT_FILTER_ARB);
				CFileDialog file(FALSE, def, fname, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST, filter, this);
				if (IDOK == file.DoModal())
				{
					CVersionNum ver;
					std::string verstr = (LPCTSTR)ver.GetVersionString();
					Element tree;
					if (m_pDoc->GetARB().Save(tree, verstr, false, false, true, false, false))
					{
						std::ofstream output(file.GetFileName(), std::ios::out | std::ios::binary);
						output.exceptions(std::ios_base::badbit);
						if (output.is_open())
						{
							tree.SaveXML(output);
							output.close();
						}
					}
					bOk = true;
				}
			}
			break;

		case WIZ_EXPORT_DTD:
			{
				CString def, fname, filter;
				def.LoadString(IDS_FILEEXT_DEF_DTD);
				filter.LoadString(IDS_FILEEXT_FILTER_DTD);
				CFileDialog file(FALSE, def, "AgilityRecordBook.dtd", OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST, filter, this);
				if (IDOK == file.DoModal())
				{
					CStdioFile output(file.GetFileName(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
					std::string dtd = ARBConfig::GetDTD();
					output.WriteString(dtd.c_str());
					output.Close();
					bOk = true;
				}
			}
			break;

		case WIZ_EXPORT_XML:
			{
				CString name = m_pDoc->GetPathName();
				if (name.IsEmpty())
					name = "AgilityRecordBook.xml";
				else
					name = name.Left(name.ReverseFind('.')) + ".xml";
				CString def, fname, filter;
				def.LoadString(IDS_FILEEXT_DEF_XML);
				filter.LoadString(IDS_FILEEXT_FILTER_XML);
				CFileDialog file(FALSE, def, name, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST, filter, this);
				if (IDOK == file.DoModal())
				{
					CVersionNum ver;
					std::string verstr = (LPCTSTR)ver.GetVersionString();
					Element tree;
					if (m_pDoc->GetARB().Save(tree, verstr, true, true, true, true, true))
					{
						std::ofstream output(file.GetFileName(), std::ios::out | std::ios::binary);
						output.exceptions(std::ios_base::badbit);
						if (output.is_open())
						{
							std::string dtd = ARBConfig::GetDTD();
							tree.SaveXML(output, &dtd);
							output.close();
						}
					}
					bOk = true;
				}
			}
			break;
		}
	}
	if (bOk)
		return CDlgBasePropertyPage::OnWizardFinish();
	else
		return FALSE;
}

void CWizardStart::OnWizardStyle() 
{
	UpdateData(TRUE);
	m_pSheet->ResetData();
	UpdateList();
}

void CWizardStart::OnSelchangeExportList() 
{
	CString str;
	int index = m_ctrlList.GetCurSel();
	if (-1 != m_Style && LB_ERR != index)
	{
		str = sc_Items[m_ctrlList.GetItemData(index)].data[m_Style].desc;
	}
	m_ctrlDesc.SetWindowText(str);
	UpdateButtons();
}

void CWizardStart::OnDblclkExportList() 
{
	int index = m_ctrlList.GetCurSel();
	if (-1 != m_Style && LB_ERR != index)
	{
		switch (sc_Items[m_ctrlList.GetItemData(index)].data[m_Style].nButton)
		{
		default:
			break;
		case PSWIZB_FINISH:
			m_pSheet->PressButton(PSBTN_FINISH);
			break;
		case PSWIZB_NEXT:
			m_pSheet->PressButton(PSBTN_NEXT);
			break;
		}
	}
}
