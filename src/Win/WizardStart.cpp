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
 * @brief Import/Export Wizard
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-10 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "WizardStart.h"
#include <fstream>

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "DlgConfigUpdate.h"
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

IMPLEMENT_DYNAMIC(CWizardStart, CPropertyPage)

CWizardStart::CWizardStart(CWizard* pSheet, CAgilityBookDoc* pDoc)
	: CPropertyPage(CWizardStart::IDD)
	, m_pSheet(pSheet)
	, m_pDoc(pDoc)
{
	//{{AFX_DATA_INIT(CWizardStart)
	//}}AFX_DATA_INIT
}

CWizardStart::~CWizardStart()
{
}

void CWizardStart::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardStart)
	DDX_Control(pDX, IDC_EXPORT_LIST, m_ctrlList);
	DDX_Control(pDX, IDC_EXPORT_DESCRIPTION, m_ctrlDesc);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWizardStart, CPropertyPage)
	//{{AFX_MSG_MAP(CWizardStart)
	ON_LBN_SELCHANGE(IDC_EXPORT_LIST, OnSelchangeExportList)
	ON_LBN_DBLCLK(IDC_EXPORT_LIST, OnDblclkExportList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

// These must agree with the order below
//#define WIZ_IMPORT_RUNS				0
#define WIZ_EXPORT_RUNS				0
#define WIZ_IMPORT_CALENDAR			1
#define WIZ_EXPORT_CALENDAR			2
#define WIZ_IMPORT_CONFIGURATION	3
#define WIZ_EXPORT_CONFIGURATION	4
#define WIZ_EXPORT_DTD				5
#define WIZ_EXPORT_XML				6

static const struct
{
	int index; // For integrity checking
	int nButton; // Sets the next button for the wizard. Also used when dbl-click on item.
	const char* item;
	const char* desc;
} sc_Items[] =
{
	//{WIZ_IMPORT_RUNS, PSWIZB_NEXT, "Import Runs", "Import trial and run information from a spreadsheet. Data must be in a text format."},
	{WIZ_EXPORT_RUNS, PSWIZB_NEXT, "Export Runs", "Export trial and run information so it can be imported into a spreadsheet."},
	{WIZ_IMPORT_CALENDAR, PSWIZB_FINISH, "Import Calendar", "Import a calendar listing that was exported from Agility Record Book."},
	{WIZ_EXPORT_CALENDAR, PSWIZB_FINISH, "Export Calendar", "Export your calendar listing so it can be imported into another file.\n\nNote, to export the calendar to a spreadsheet, use copy/paste."},
	{WIZ_IMPORT_CONFIGURATION, PSWIZB_FINISH, "Import Configuration", "Update your configuration to support new and/or updated venues."},
	{WIZ_EXPORT_CONFIGURATION, PSWIZB_FINISH, "Export Configuration", "Export your configuration so it can be imported into another file."},
	{WIZ_EXPORT_DTD, PSWIZB_FINISH, "Export DTD", "Export the Document Type Definition. This data describes the XML format of the data file."},
	{WIZ_EXPORT_XML, PSWIZB_FINISH, "Export File as XML", "Export your data file as an XML file. The DTD will be contained within this file."},
};
static const int sc_nItems = sizeof(sc_Items) / sizeof(sc_Items[0]);

void CWizardStart::UpdateButtons()
{
	DWORD dwWiz = 0;
	int index = m_ctrlList.GetCurSel();
	switch (index)
	{
	default:
		break;
	case WIZ_IMPORT_CALENDAR:
	case WIZ_EXPORT_CALENDAR:
	case WIZ_IMPORT_CONFIGURATION:
	case WIZ_EXPORT_CONFIGURATION:
	case WIZ_EXPORT_DTD:
	case WIZ_EXPORT_XML:
	//case WIZ_IMPORT_RUNS:
	case WIZ_EXPORT_RUNS:
		dwWiz |= sc_Items[index].nButton;
		break;
	}
	m_pSheet->SetWizardButtons(dwWiz);
}

/////////////////////////////////////////////////////////////////////////////
// CWizardStart message handlers

BOOL CWizardStart::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	for (int i = 0; i < sc_nItems; ++i)
	{
		ASSERT(sc_Items[i].index == i);
		int index = m_ctrlList.AddString(sc_Items[i].item);
		if (LB_ERR != index)
			m_ctrlList.SetItemData(index, i + 1);
	}
	m_ctrlList.SetCurSel(-1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWizardStart::OnSetActive() 
{
	UpdateButtons();
	return CPropertyPage::OnSetActive();
}

LRESULT CWizardStart::OnWizardNext() 
{
	int index = m_ctrlList.GetCurSel();
	// -1: Prevent page from changing
	// 0: Automatic next page
	// other: ID of dialog
	LRESULT nextPage = -1;
	switch (index)
	{
	default: break;
	//case WIZ_IMPORT_RUNS: nextPage = IDD_WIZARD_IMPORT; break;
	case WIZ_EXPORT_RUNS: nextPage = IDD_WIZARD_EXPORT; break;
	}
	return nextPage;
}

BOOL CWizardStart::OnWizardFinish() 
{
	int index = m_ctrlList.GetCurSel();
	bool bOk = false;

	switch (index)
	{
	default:
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
				CElement tree;
				if (!tree.LoadXMLFile(file.GetPathName()))
					AfxMessageBox(AFX_IDP_FAILED_TO_OPEN_DOC);
				else
				{
					ARBAgilityRecordBook book;
					if (book.Load(tree, true, false, false, false, false))
					{
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
					}
					bOk = true;
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
				CElement tree;
				if (m_pDoc->GetARB().Save(tree, true, false, false, false, false))
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
		{
			CDlgConfigUpdate dlg(this);
			if (IDOK == dlg.DoModal())
			{
				ARBConfig& update = dlg.GetConfig();
				CString msg;
				msg = m_pDoc->GetConfig().Update(0, update).c_str();
				if (0 < msg.GetLength())
				{
					CDlgMessage dlg(msg, 0);
					dlg.DoModal();
					m_pDoc->SetModifiedFlag();
					m_pDoc->UpdateAllViews(NULL, UPDATE_CONFIG);
				}
				else
					AfxMessageBox(IDS_CONFIG_NO_UPDATE, MB_ICONINFORMATION);
				bOk = true;
			}
		}
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
				CElement tree;
				if (m_pDoc->GetARB().Save(tree, false, false, true, false, false))
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
				CElement tree;
				if (m_pDoc->GetARB().Save(tree, true, true, true, true, true))
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
	if (bOk)
		return CPropertyPage::OnWizardFinish();
	else
		return FALSE;
}

void CWizardStart::OnSelchangeExportList() 
{
	CString str;
	int index = m_ctrlList.GetCurSel();
	if (LB_ERR != index)
		str = sc_Items[m_ctrlList.GetItemData(index)-1].desc;
	m_ctrlDesc.SetWindowText(str);
	UpdateButtons();
}

void CWizardStart::OnDblclkExportList() 
{
	int index = m_ctrlList.GetCurSel();
	if (LB_ERR != index)
	{
		switch (sc_Items[m_ctrlList.GetItemData(index)-1].nButton)
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
