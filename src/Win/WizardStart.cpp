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
 * @brief Import/Export Wizard
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-08-09 DRC When importing ARB files, update regnum and titles
 *                    that are different.
 * @li 2005-07-14 DRC Remember selected export item.
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

// Registry settings in "Last"
#define LAST_SECTION	_T("Last")
#define LAST_STYLE			_T("WizStyle")
#define LAST_STYLEITEM		_T("WizSubStyle") // A number will be appended

// Note: For this to work, the IDC_WIZARD_START_* radio buttons should be in
// sequential order. If not, then the logic below must be fudged. The WIZARD_
// codes are written to the registry, so we can't change them. But as items
// are added/removed from the radio list, the radio button dlg data must be
// adjusted. The structures below that control the wizard are setup in radio
// button order (the value of m_Style). So these 2 functions translate windows
// radio codes to registry codes and back.
// (In the past, we kept the wizard codes the same as the radio buttons - that
// was fine until the open office button was added right after the excel one)
static int TransWizardToDlg(int wizCode)
{
	switch (wizCode)
	{
	default:
		ASSERT(0);
		// fall thru
	case WIZARD_RADIO_EXCEL:
		return IDC_WIZARD_START_EXCEL - IDC_WIZARD_START_EXCEL;
	case WIZARD_RADIO_OPENOFFICE:
		return IDC_WIZARD_START_OPENOFFICE - IDC_WIZARD_START_EXCEL;
	case WIZARD_RADIO_SPREADSHEET:
		return IDC_WIZARD_START_SPREADSHEET - IDC_WIZARD_START_EXCEL;
	case WIZARD_RADIO_ARB:
		return IDC_WIZARD_START_ARB - IDC_WIZARD_START_EXCEL;
	}
}

static int TransDlgToWizard(int radioBtn)
{
	switch (radioBtn + IDC_WIZARD_START_EXCEL)
	{
	default:
		ASSERT(0);
		// fall thru
	case IDC_WIZARD_START_EXCEL:
		return WIZARD_RADIO_EXCEL;
	case IDC_WIZARD_START_OPENOFFICE:
		return WIZARD_RADIO_OPENOFFICE;
	case IDC_WIZARD_START_SPREADSHEET:
		return WIZARD_RADIO_SPREADSHEET;
	case IDC_WIZARD_START_ARB:
		return WIZARD_RADIO_ARB;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWizardStart property page

IMPLEMENT_DYNAMIC(CWizardStart, CDlgBasePropertyPage)

CWizardStart::CWizardStart(
		CWizard* pSheet,
		CAgilityBookDoc* pDoc)
	: CDlgBasePropertyPage(CWizardStart::IDD)
	, m_ctrlList(false)
	, m_pSheet(pSheet)
	, m_pDoc(pDoc)
{
	//{{AFX_DATA_INIT(CWizardStart)
	m_Style = TransWizardToDlg(WIZARD_RADIO_EXCEL);
	//}}AFX_DATA_INIT
	int wiz = AfxGetApp()->GetProfileInt(LAST_SECTION, LAST_STYLE, m_Style);
	m_Style = TransWizardToDlg(wiz);
	if (WIZARD_RADIO_EXCEL == wiz && !m_pSheet->ExcelHelper())
		m_Style = TransWizardToDlg(WIZARD_RADIO_SPREADSHEET);
	if (WIZARD_RADIO_OPENOFFICE == wiz && !m_pSheet->OpenOfficeHelper())
		m_Style = TransWizardToDlg(WIZARD_RADIO_SPREADSHEET);
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
	ON_BN_CLICKED(IDC_WIZARD_START_OPENOFFICE, OnWizardStyle)
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
		TCHAR const* item;
		// Description shown when listing is selected.
		TCHAR const* desc;
	} data[4]; // Data must agree with radio buttons.
} const sc_Items[] =
{
	{WIZ_IMPORT_RUNS,
	{
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT,
			_T("Import Trials and Runs"),
			_T("Import trial and run information from Microsoft Excel.\n\nWhen importing, each run must have a valid (as defined in the Configuration) Venue, Event, Division and Level. Also, if a trial is dual-sanctioned, the '/' character is assumed to separate the list of venue names and club names.")
		},
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT,
			_T("Import Trials and Runs"),
			_T("Import trial and run information from OpenOffice Calc.\n\nWhen importing, each run must have a valid (as defined in the Configuration) Venue, Event, Division and Level. Also, if a trial is dual-sanctioned, the '/' character is assumed to separate the list of venue names and club names.")
		},
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT,
			_T("Import Trials and Runs"),
			_T("Import trial and run information from a spreadsheet. Data must be in a text format.\n\nWhen importing, each run must have a valid (as defined in the Configuration) Venue, Event, Division and Level. Also, if a trial is dual-sanctioned, the '/' character is assumed to separate the list of venue names and club names.")
		},
		{PSWIZB_FINISH, -1,
			_T("Import Dogs, Trials, Runs and Judges"),
			_T("Import dog, trial, run and judge information from Agility Record Book.")
		}
	} },
	{WIZ_EXPORT_RUNS,
	{
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			_T("Export Runs"),
			_T("Export trial and run information to Microsoft Excel.")
		},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			_T("Export Runs"),
			_T("Export trial and run information to OpenOffice Calc.")
		},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			_T("Export Runs"),
			_T("Export trial and run information so it can be imported into a spreadsheet.")
		},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL}
	} },
	{WIZ_IMPORT_CALENDAR,
	{
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT,
			_T("Import Calendar"),
			_T("Import a calendar listing from Microsoft Excel.")
		},
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT,
			_T("Import Calendar"),
			_T("Import a calendar listing from OpenOffice Calc.")
		},
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT,
			_T("Import Calendar"),
			_T("Import a calendar listing from a spreadsheet.")
		},
		{PSWIZB_FINISH, -1,
			_T("Import Calendar"),
			_T("Import a calendar listing that was exported from Agility Record Book.")
		}
	} },
	{WIZ_EXPORT_CALENDAR,
	{
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			_T("Export Calendar"),
			_T("Export your calendar listing to Microsoft Excel.")
		},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			_T("Export Calendar"),
			_T("Export your calendar listing to OpenOffice Calc.")
		},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			_T("Export Calendar"),
			_T("Export your calendar listing to a spreadsheet.")
		},
		{PSWIZB_FINISH, -1,
			_T("Export Calendar"),
			_T("Export your calendar listing so it can be imported into Agility Record Book.")
		}
	} },
	{WIZ_EXPORT_CALENDAR_VCAL,
	{
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_FINISH, -1,
			_T("Export Calendar (vCalendar)"),
			_T("Export calendar entries in vCalendar (.vcs) format so they can be imported into other programs.")
		},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL}
	} },
	{WIZ_EXPORT_CALENDAR_ICAL,
	{
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_FINISH, -1,
			_T("Export Calendar (iCalendar)"),
			_T("Export calendar entries in iCalendar (.ics) format so they can be imported into other programs.")
		},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL}
	} },
	{WIZ_EXPORT_CALENDAR_APPT,
	{
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			_T("Export Calendar (MS Outlook Appointment)"),
			_T("Export calendar entries to Excel so they can be imported into Microsoft Outlook as Appointments.")
		},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			_T("Export Calendar (MS Outlook Appointment)"),
			_T("Export calendar entries to a spreadsheet so they can be imported into Microsoft Outlook as Appointments.")
		},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL}
	} },
	{WIZ_EXPORT_CALENDAR_TASK,
	{
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			_T("Export Calendar (MS Outlook Task)"),
			_T("Export calendar entries to Excel so they can be imported into Microsoft Outlook as Tasks. Only Calendar entries that at marked as 'Planning' will be exported.")
		},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			_T("Export Calendar (MS Outlook Task)"),
			_T("Export calendar entries to a spreadsheet so they can be imported into Microsoft Outlook as Tasks. Only Calendar entries that at marked as 'Planning' will be exported.")
		},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL}
	} },
	{WIZ_IMPORT_LOG,
	{
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT,
			_T("Import Training Log"),
			_T("Import a Training Log from Microsoft Excel.")
		},
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT,
			_T("Import Training Log"),
			_T("Import a Training Log from OpenOffice Calc.")
		},
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT,
			_T("Import Training Log"),
			_T("Import a Training Log from a spreadsheet.")
		},
		{PSWIZB_FINISH, -1,
			_T("Import Training Log"),
			_T("Import a Training Log that was exported from Agility Record Book.")
		}
	} },
	{WIZ_EXPORT_LOG,
	{
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			_T("Export Training Log"),
			_T("Export your Training Log to Microsoft Excel.")
		},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			_T("Export Training Log"),
			_T("Export your Training Log to OpenOffice Calc.")
		},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT,
			_T("Export Training Log"),
			_T("Export your Training Log to a spreadsheet.")
		},
		{PSWIZB_FINISH, -1,
			_T("Export Training Log"),
			_T("Export your Training Log so it can be imported into Agility Record Book.")
		}
	} },
	{WIZ_IMPORT_CONFIGURATION,
	{
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_FINISH, -1,
			_T("Import Configuration"),
			_T("Update your configuration to support new and/or updated venues.")
		}
	} },
	{WIZ_EXPORT_CONFIGURATION,
	{
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_FINISH, -1,
			_T("Export Configuration"),
			_T("Export your configuration so it can be imported into Agility Record Book.")
		}
	} },
	{WIZ_EXPORT_DTD,
	{
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_FINISH, -1,
			_T("Export DTD"),
			_T("Export the Document Type Definition. This data describes the XML format of the data file.")
		}
	} },
	{WIZ_EXPORT_XML,
	{
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_DISABLEDFINISH, -1, NULL, NULL},
		{PSWIZB_FINISH, -1,
			_T("Export File as XML"),
			_T("Export your data file as an XML file. The DTD will be contained within this file.")
		}
	} },
};
static int const sc_nItems = sizeof(sc_Items) / sizeof(sc_Items[0]);

void CWizardStart::UpdateList()
{
	m_ctrlList.ResetContent();
	m_ctrlDesc.SetWindowText(_T(""));
	for (int i = 0; i < sc_nItems; ++i)
	{
		ASSERT(sc_Items[i].index == i);
		bool bAdd = true;
		if (m_pSheet->GetCalendarEntries()
		&& sc_Items[i].index != WIZ_EXPORT_CALENDAR_VCAL
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
	ARBostringstream str;
	str << LAST_STYLEITEM << m_Style;
	int idx = AfxGetApp()->GetProfileInt(LAST_SECTION, str.str().c_str(), -1);
	m_ctrlList.SetCurSel(idx);
	OnSelchangeExportList();
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
	if (!m_pSheet->ExcelHelper())
		GetDlgItem(IDC_WIZARD_START_EXCEL)->EnableWindow(FALSE);
#if 0
	if (!m_pSheet->OpenOfficeHelper())
		GetDlgItem(IDC_WIZARD_START_OPENOFFICE)->EnableWindow(FALSE);
#endif
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
		m_pSheet->SetImportExportItem(data, TransDlgToWizard(m_Style));
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
					ARBString errMsg;
					Element tree;
					if (!tree.LoadXMLFile((LPCTSTR)file.GetPathName(), errMsg))
					{
						CString msg;
						msg.LoadString(AFX_IDP_FAILED_TO_OPEN_DOC);
						if (0 < errMsg.length())
						{
							msg += _T("\n\n");
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
							int countRegNumsAdded = 0;
							int countRegNumsUpdated = 0;
							int countExistingPts = 0;
							int countTitlesAdded = 0;
							int countTitlesUpdated = 0;
							int countTrials = 0;
							int countClubs = 0;
							int countJudges = 0;
							int countLocations = 0;
							for (ARBDogList::iterator iterDog = book.GetDogs().begin();
								iterDog != book.GetDogs().end();
								++iterDog)
							{
								ARBDogPtr pDog = *iterDog;
								ARBDogPtr pExisting;
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
											ARBDogRegNumPtr pRegNum = *iter;
											ARBDogRegNumPtr pRegExist;
											if (pExisting->GetRegNums().FindRegNum(pRegNum->GetVenue(), &pRegExist))
											{
												if (*pRegExist != *pRegNum)
												{
													++countRegNumsUpdated;
													*pRegExist = *pRegNum;
												}
											}
											else
											{
												++countRegNumsAdded;
												pExisting->GetRegNums().AddRegNum(pRegNum);
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
											ARBDogTitlePtr pTitle = *iter;
											ARBDogTitlePtr pTitleExist;
											if (pExisting->GetTitles().FindTitle(pTitle->GetVenue(), pTitle->GetRawName(), &pTitleExist))
											{
												if (*pTitle != *pTitleExist)
												{
													++countTitlesUpdated;
													*pTitleExist = *pTitle;
												}
											}
											else
											{
												++countTitlesAdded;
												ARBDogTitlePtr pNewTitle = pTitle->Clone();
												pExisting->GetTitles().AddTitle(pNewTitle);
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
								ARBInfoItemPtr pClub = *iterClub;
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
								ARBInfoItemPtr pJudge = *iterJudge;
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
								ARBInfoItemPtr pLocation = *iterLocation;
								// If this fails, it already exists.
								if (m_pDoc->GetARB().GetInfo().GetInfo(ARBInfo::eLocationInfo).AddItem(pLocation))
								{
									++countLocations;
								}
							}
							if (0 < countDog
							|| 0 < countRegNumsAdded || 0 < countRegNumsUpdated
							|| 0 < countExistingPts
							|| 0 < countTitlesAdded || 0 < countTitlesUpdated
							|| 0 < countTrials)
							{
								m_pDoc->UpdateAllViews(NULL, UPDATE_ALL_VIEW);
								m_pDoc->SetModifiedFlag();
							}
							if (0 < countClubs)
							{
								std::set<ARBString> namesInUse;
								m_pDoc->GetAllClubNames(namesInUse, false);
								m_pDoc->GetARB().GetInfo().GetInfo(ARBInfo::eClubInfo).CondenseContent(namesInUse);
							}
							if (0 < countJudges)
							{
								std::set<ARBString> namesInUse;
								m_pDoc->GetAllJudges(namesInUse, false);
								m_pDoc->GetARB().GetInfo().GetInfo(ARBInfo::eJudgeInfo).CondenseContent(namesInUse);
							}
							if (0 < countLocations)
							{
								std::set<ARBString> namesInUse;
								m_pDoc->GetAllTrialLocations(namesInUse, false);
								m_pDoc->GetARB().GetInfo().GetInfo(ARBInfo::eLocationInfo).CondenseContent(namesInUse);
							}
							//"Added %1!d! new dogs, updated %2!d! dogs and added %3!d! judges."
							CString str(_T("Added "));
							bool bAdded = false;
							if (0 < countDog)
							{
								if (bAdded)
									str += _T(", ");
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_DOGS, countDog);
								str += str2;
							}
							if (0 < countRegNumsAdded)
							{
								if (bAdded)
									str += _T(", ");
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_REGNUMS, countRegNumsAdded);
								str += str2;
							}
							if (0 < countExistingPts)
							{
								if (bAdded)
									str += _T(", ");
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_EXISTINGPTS, countExistingPts);
								str += str2;
							}
							if (0 < countTitlesAdded)
							{
								if (bAdded)
									str += _T(", ");
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_TITLES, countTitlesAdded);
								str += str2;
							}
							if (0 < countTrials)
							{
								if (bAdded)
									str += _T(", ");
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_TRIALS, countTrials);
								str += str2;
							}
							if (0 < countClubs)
							{
								if (bAdded)
									str += _T(", ");
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_CLUBS, countClubs);
								str += str2;
							}
							if (0 < countJudges)
							{
								if (bAdded)
									str += _T(", ");
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_JUDGES, countJudges);
								str += str2;
							}
							if (0 < countLocations)
							{
								if (bAdded)
									str += _T(", ");
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_LOCATIONS, countLocations);
								str += str2;
							}
							bAdded = false;
							if (0 < countRegNumsUpdated)
							{
								if (bAdded)
									str += _T(", ");
								else
									str += _T("\nUpdated ");
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_REGNUMS, countRegNumsUpdated);
								str += str2;
							}
							if (0 < countTitlesUpdated)
							{
								if (bAdded)
									str += _T(", ");
								else
									str += _T("\nUpdated ");
								bAdded = true;
								CString str2;
								str2.FormatMessage(IDS_ADDED_TITLES, countTitlesUpdated);
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
					ARBString errMsg;
					Element tree;
					if (!tree.LoadXMLFile((LPCTSTR)file.GetPathName(), errMsg))
					{
						CString msg;
						msg.LoadString(AFX_IDP_FAILED_TO_OPEN_DOC);
						if (0 < errMsg.length())
						{
							msg += _T("\n\n");
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
								ARBCalendarPtr cal = *iter;
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
					AfxGetMainWnd()->UpdateWindow();
					CWaitCursor wait;
					CVersionNum ver;
					ARBString verstr = (LPCTSTR)ver.GetVersionString();
					Element tree;
					if (m_pDoc->GetARB().Save(tree, verstr, true, false, false, false, false))
					{
						CStringA filename(file.GetFileName());
						std::ofstream output(filename, std::ios::out | std::ios::binary);
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

		case WIZ_EXPORT_CALENDAR_VCAL:
		case WIZ_EXPORT_CALENDAR_ICAL:
			{
				CString def, fname, filter;
				if (WIZ_EXPORT_CALENDAR_VCAL == data)
				{
					def.LoadString(IDS_FILEEXT_DEF_VCS);
					fname.LoadString(IDS_FILEEXT_FNAME_VCS);
					filter.LoadString(IDS_FILEEXT_FILTER_VCS);
				}
				else
				{
					def.LoadString(IDS_FILEEXT_DEF_ICS);
					fname.LoadString(IDS_FILEEXT_FNAME_ICS);
					filter.LoadString(IDS_FILEEXT_FILTER_ICS);
				}
				CFileDialog file(FALSE, def, fname, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST, filter, this);
				if (IDOK == file.DoModal())
				{
					AfxGetMainWnd()->UpdateWindow();
					CWaitCursor wait;
					std::vector<ARBCalendarPtr> allEntries;
					std::vector<ARBCalendarPtr>* entries = m_pSheet->GetCalendarEntries();
					if (!entries)
					{
						allEntries.reserve(m_pDoc->GetCalendar().size());
						for (ARBCalendarList::const_iterator iterCal = m_pDoc->GetCalendar().begin(); iterCal != m_pDoc->GetCalendar().end(); ++iterCal)
						{
							allEntries.push_back(*iterCal);
						}
						entries = &allEntries;
					}
					CStringA filename(file.GetFileName());
					std::ofstream output(filename, std::ios::out | std::ios::binary);
					output.exceptions(std::ios_base::badbit);
					if (output.is_open())
					{
						int nWarning = CAgilityBookOptions::CalendarOpeningNear();
						ICalendar* iCalendar = ICalendar::iCalendarBegin(output, (WIZ_EXPORT_CALENDAR_VCAL == data) ? 1 : 2);
						for (std::vector<ARBCalendarPtr>::const_iterator iterCal = entries->begin(); iterCal != entries->end(); ++iterCal)
						{
							ARBCalendarPtr pCal = *iterCal;
							pCal->iCalendar(iCalendar, nWarning);
						}
						iCalendar->Release();
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
					ARBString errMsg;
					Element tree;
					if (!tree.LoadXMLFile((LPCTSTR)file.GetPathName(), errMsg))
					{
						CString msg;
						msg.LoadString(AFX_IDP_FAILED_TO_OPEN_DOC);
						if (0 < errMsg.length())
						{
							msg += _T("\n\n");
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
								ARBTrainingPtr item = *iter;
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
					AfxGetMainWnd()->UpdateWindow();
					CWaitCursor wait;
					CVersionNum ver;
					ARBString verstr = (LPCTSTR)ver.GetVersionString();
					Element tree;
					if (m_pDoc->GetARB().Save(tree, verstr, false, true, false, false, false))
					{
						CStringA filename(file.GetFileName());
						std::ofstream output(filename, std::ios::out | std::ios::binary);
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
					AfxGetMainWnd()->UpdateWindow();
					CWaitCursor wait;
					CVersionNum ver;
					ARBString verstr = (LPCTSTR)ver.GetVersionString();
					Element tree;
					if (m_pDoc->GetARB().Save(tree, verstr, false, false, true, false, false))
					{
						CStringA filename(file.GetFileName());
						std::ofstream output(filename, std::ios::out | std::ios::binary);
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
				CFileDialog file(FALSE, def, _T("AgilityRecordBook.dtd"), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST, filter, this);
				if (IDOK == file.DoModal())
				{
					AfxGetMainWnd()->UpdateWindow();
					CWaitCursor wait;
					CStdioFile output(file.GetFileName(), CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
					ARBString dtd = ARBConfig::GetDTD(false);
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
					name = _T("AgilityRecordBook.xml");
				else
					name = name.Left(name.ReverseFind('.')) + _T(".xml");
				CString def, fname, filter;
				def.LoadString(IDS_FILEEXT_DEF_XML);
				filter.LoadString(IDS_FILEEXT_FILTER_XML);
				CFileDialog file(FALSE, def, name, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST, filter, this);
				if (IDOK == file.DoModal())
				{
					AfxGetMainWnd()->UpdateWindow();
					CWaitCursor wait;
					CVersionNum ver;
					ARBString verstr = (LPCTSTR)ver.GetVersionString();
					Element tree;
					if (m_pDoc->GetARB().Save(tree, verstr, true, true, true, true, true))
					{
						CStringA filename(file.GetFileName());
						std::ofstream output(filename, std::ios::out | std::ios::binary);
						output.exceptions(std::ios_base::badbit);
						if (output.is_open())
						{
							ARBString dtd = ARBConfig::GetDTD();
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
	AfxGetApp()->WriteProfileInt(LAST_SECTION, LAST_STYLE, TransDlgToWizard(m_Style));
	m_pSheet->ResetData();
	UpdateList();
}

void CWizardStart::OnSelchangeExportList() 
{
	CString msg;
	int index = m_ctrlList.GetCurSel();
	if (-1 != m_Style && LB_ERR != index)
	{
		msg = sc_Items[m_ctrlList.GetItemData(index)].data[m_Style].desc;
	}
	m_ctrlDesc.SetWindowText(msg);
	ARBostringstream str;
	str << LAST_STYLEITEM << m_Style;
	AfxGetApp()->WriteProfileInt(LAST_SECTION, str.str().c_str(), index);
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
