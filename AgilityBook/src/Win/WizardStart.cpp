/*
 * Copyright © 2003-2007 David Connet. All Rights Reserved.
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
	case WIZARD_RADIO_CALC:
		return IDC_WIZARD_START_CALC - IDC_WIZARD_START_EXCEL;
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
	case IDC_WIZARD_START_CALC:
		return WIZARD_RADIO_CALC;
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
	if (WIZARD_RADIO_CALC == wiz && !m_pSheet->CalcHelper())
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
	ON_BN_CLICKED(IDC_WIZARD_START_CALC, OnWizardStyle)
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
		// Listing (0 denotes no entry)
		UINT item;
		// Description shown when listing is selected.
		UINT desc;
	} data[4]; // Data must agree with radio buttons.
} const sc_Items[] =
{
	{WIZ_IMPORT_RUNS,
	{
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT, IDS_WIZ_IMPORT_RUNS, IDS_WIZ_IMPORT_RUNS_EXCEL},
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT, IDS_WIZ_IMPORT_RUNS, IDS_WIZ_IMPORT_RUNS_CALC},
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT, IDS_WIZ_IMPORT_RUNS, IDS_WIZ_IMPORT_RUNS_SPREAD},
		{PSWIZB_FINISH, -1, IDS_WIZ_IMPORT_RUNS_PLUS, IDS_WIZ_IMPORT_RUNS_ARB}
	} },
	{WIZ_EXPORT_RUNS,
	{
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT, IDS_WIZ_EXPORT_RUNS, IDS_WIZ_EXPORT_RUNS_EXCEL},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT, IDS_WIZ_EXPORT_RUNS, IDS_WIZ_EXPORT_RUNS_CALC},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT, IDS_WIZ_EXPORT_RUNS, IDS_WIZ_EXPORT_RUNS_SPREAD},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0}
	} },
	{WIZ_IMPORT_CALENDAR,
	{
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT, IDS_WIZ_IMPORT_CAL, IDS_WIZ_IMPORT_CAL_EXCEL},
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT, IDS_WIZ_IMPORT_CAL, IDS_WIZ_IMPORT_CAL_CALC},
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT, IDS_WIZ_IMPORT_CAL, IDS_WIZ_IMPORT_CAL_SPREAD},
		{PSWIZB_FINISH, -1, IDS_WIZ_IMPORT_CAL, IDS_WIZ_IMPORT_CAL_ARB},
	} },
	{WIZ_EXPORT_CALENDAR,
	{
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT, IDS_WIZ_EXPORT_CAL, IDS_WIZ_EXPORT_CAL_EXCEL},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT, IDS_WIZ_EXPORT_CAL, IDS_WIZ_EXPORT_CAL_CALC},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT, IDS_WIZ_EXPORT_CAL, IDS_WIZ_EXPORT_CAL_SPREAD},
		{PSWIZB_FINISH, -1, IDS_WIZ_EXPORT_CAL, IDS_WIZ_EXPORT_CAL_ARB}
	} },
	{WIZ_EXPORT_CALENDAR_VCAL,
	{
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_FINISH, -1, IDS_WIZ_EXPORT_CAL_VCAL, IDS_WIZ_EXPORT_CAL_VCAL_SPREAD},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0}
	} },
	{WIZ_EXPORT_CALENDAR_ICAL,
	{
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_FINISH, -1, IDS_WIZ_EXPORT_CAL_ICAL, IDS_WIZ_EXPORT_CAL_ICAL_SPREAD},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0}
	} },
	{WIZ_EXPORT_CALENDAR_APPT,
	{
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT, IDS_WIZ_EXPORT_CAL_APPT, IDS_WIZ_EXPORT_CAL_APPT_EXCEL},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT, IDS_WIZ_EXPORT_CAL_APPT, IDS_WIZ_EXPORT_CAL_APPT_SPREAD},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0}
	} },
	{WIZ_EXPORT_CALENDAR_TASK,
	{
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT, IDS_WIZ_EXPORT_CAL_TASK, IDS_WIZ_EXPORT_CAL_TASK_EXCEL},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT, IDS_WIZ_EXPORT_CAL_TASK, IDS_WIZ_EXPORT_CAL_TASK_SPREAD},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0}
	} },
	{WIZ_IMPORT_LOG,
	{
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT, IDS_WIZ_IMPORT_LOG, IDS_WIZ_IMPORT_LOG_EXCEL},
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT, IDS_WIZ_IMPORT_LOG, IDS_WIZ_IMPORT_LOG_CALC},
		{PSWIZB_NEXT, IDD_WIZARD_IMPORT, IDS_WIZ_IMPORT_LOG, IDS_WIZ_IMPORT_LOG_SPREAD},
		{PSWIZB_FINISH, -1, IDS_WIZ_IMPORT_LOG, IDS_WIZ_IMPORT_LOG_ARB}
	} },
	{WIZ_EXPORT_LOG,
	{
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT, IDS_WIZ_EXPORT_LOG, IDS_WIZ_EXPORT_LOG_EXCEL},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT, IDS_WIZ_EXPORT_LOG, IDS_WIZ_EXPORT_LOG_CALC},
		{PSWIZB_NEXT, IDD_WIZARD_EXPORT, IDS_WIZ_EXPORT_LOG, IDS_WIZ_EXPORT_LOG_SPREAD},
		{PSWIZB_FINISH, -1, IDS_WIZ_EXPORT_LOG, IDS_WIZ_EXPORT_LOG_ARB}
	} },
	{WIZ_IMPORT_CONFIGURATION,
	{
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_FINISH, -1, IDS_WIZ_IMPORT_CONFIG, IDS_WIZ_IMPORT_CONFIG_ARB}
	} },
	{WIZ_EXPORT_CONFIGURATION,
	{
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_FINISH, -1, IDS_WIZ_EXPORT_CONFIG, IDS_WIZ_EXPORT_CONFIG_ARB}
	} },
	{WIZ_EXPORT_DTD,
	{
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_FINISH, -1, IDS_WIZ_EXPORT_DTD, IDS_WIZ_EXPORT_DTD_ARB}
	} },
	{WIZ_EXPORT_XML,
	{
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_DISABLEDFINISH, -1, 0, 0},
		{PSWIZB_FINISH, -1, IDS_WIZ_EXPORT_XML, IDS_WIZ_EXPORT_XML_ARB}
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
		if (bAdd && 0 == sc_Items[i].data[m_Style].item)
			bAdd = false;
		int index = LB_ERR;
		if (bAdd)
		{
			CString str;
			str.LoadString(sc_Items[i].data[m_Style].item);
			index = m_ctrlList.AddString(str);
		}
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
	if (!m_pSheet->CalcHelper())
		GetDlgItem(IDC_WIZARD_START_CALC)->EnableWindow(FALSE);
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
					ElementNodePtr tree(ElementNode::New());
					CStringA fileName(file.GetPathName());
					if (!tree->LoadXMLFile(fileName, errMsg))
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
						bOk = m_pDoc->ImportARBRunData(tree, this);
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
					ElementNodePtr tree(ElementNode::New());
					CStringA fileName(file.GetPathName());
					if (!tree->LoadXMLFile(fileName, errMsg))
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
						bOk = m_pDoc->ImportARBCalData(tree, this);
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
					ElementNodePtr tree(ElementNode::New());
					if (m_pDoc->GetARB().Save(tree, verstr, true, false, false, false, false))
					{
						CStringA filename(file.GetFileName());
						tree->SaveXML(filename);
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
					ARBofstream output(file.GetFileName(), std::ios::out | std::ios::binary);
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
					ElementNodePtr tree(ElementNode::New());
					CStringA fileName(file.GetPathName());
					if (!tree->LoadXMLFile(fileName, errMsg))
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
						bOk = m_pDoc->ImportARBLogData(tree, this);
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
					ElementNodePtr tree(ElementNode::New());
					if (m_pDoc->GetARB().Save(tree, verstr, false, true, false, false, false))
					{
						CStringA filename(file.GetFileName());
						tree->SaveXML(filename);
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
					ElementNodePtr tree(ElementNode::New());
					if (m_pDoc->GetARB().Save(tree, verstr, false, false, true, false, false))
					{
						CStringA filename(file.GetFileName());
						tree->SaveXML(filename);
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
					CStringA filename(file.GetFileName());
					std::ofstream output(filename, std::ios::out | std::ios::binary);
					output.exceptions(std::ios_base::badbit);
					if (output.is_open())
					{
						output << ARBConfig::GetDTD(false);
						output.close();
					}
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
					ElementNodePtr tree(ElementNode::New());
					if (m_pDoc->GetARB().Save(tree, verstr, true, true, true, true, true))
					{
						CStringA filename(file.GetFileName());
						std::string dtd = ARBConfig::GetDTD();
						tree->SaveXML(filename, &dtd);
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
		msg.LoadString(sc_Items[m_ctrlList.GetItemData(index)].data[m_Style].desc);
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
