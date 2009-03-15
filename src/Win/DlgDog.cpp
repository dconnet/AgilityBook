/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgDog class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-10-17 DRC Points view didn't update 'View Hidden' after canceling.
 * @li 2004-02-03 DRC Broke dialog up into pages.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-08-18 DRC Added a deceased date for a dog.
 */

#include "stdafx.h"
#include "DlgDog.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBConfig.h"
#include "ARBDog.h"
#include "DlgExistingPoints.h"
#include "DlgRegNum.h"
#include "DlgTitle.h"
#include "FilterOptions.h"
#include "ListCtrl.h"
#include "ListData.h"
#include "Validators.h"
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/notebook.h>

#include "res/CalEmpty.xpm"
#include "res/hidden.xpm"
#include "res/title-hidden-have.xpm"
#include "res/title-hidden.xpm"
#include "res/title-visible-have.xpm"
#include "res/title-visible.xpm"

/////////////////////////////////////////////////////////////////////////////

static struct SORTINFO
{
	CDlgDog* pThis;
	CAgilityBookDoc* pDoc;
	CColumnOrder* pCols;
} s_SortInfo;


static struct
{
	int fmt;
	int cx;
	wxChar const* idText;
} const colTitleInfo[] =
{
	{wxLIST_FORMAT_LEFT, 10, NULL},
	{wxLIST_FORMAT_LEFT, 50, wxT("IDS_COL_DATE")},
	{wxLIST_FORMAT_LEFT, 50, wxT("IDS_COL_VENUE")},
	{wxLIST_FORMAT_LEFT, 50, wxT("IDS_COL_TITLE")},
	{wxLIST_FORMAT_LEFT, 50, wxT("IDS_COL_NAME")},
};
static int const nColTitleInfo = sizeof(colTitleInfo) / sizeof(colTitleInfo[0]);


static struct
{
	int fmt;
	int cx;
	wxChar const* idText;
} const colRegNumInfo[] =
{
	{wxLIST_FORMAT_LEFT, 50, _("IDS_COL_VENUE")},
	{wxLIST_FORMAT_LEFT, 50, _("IDS_COL_NUMBER")},
	{wxLIST_FORMAT_LEFT, 50, _("IDS_COL_HEIGHT")},
	{wxLIST_FORMAT_LEFT, 50, _("IDS_COL_RECEIVED")},
	{wxLIST_FORMAT_LEFT, 50, _("IDS_COL_NOTE")},
};
static int const nColRegNumInfo = sizeof(colRegNumInfo) / sizeof(colRegNumInfo[0]);


static struct
{
	int fmt;
	int cx;
	wxChar const* idText;
} const colExistingPointsInfo[] =
{
	{wxLIST_FORMAT_LEFT, 50, _("IDS_COL_DATE")},
	{wxLIST_FORMAT_LEFT, 50, _("IDS_COL_TYPE")},
	{wxLIST_FORMAT_LEFT, 50, _("IDS_COL_POINTS")},
	{wxLIST_FORMAT_LEFT, 50, _("IDS_OTHERPOINTS")},
	{wxLIST_FORMAT_LEFT, 50, _("IDS_COL_VENUE")},
	{wxLIST_FORMAT_LEFT, 50, _("IDS_COL_MULTIQ")},
	{wxLIST_FORMAT_LEFT, 50, _("IDS_COL_DIVISION")},
	{wxLIST_FORMAT_LEFT, 50, _("IDS_COL_LEVEL")},
	{wxLIST_FORMAT_LEFT, 50, _("IDS_COL_EVENT")},
	{wxLIST_FORMAT_LEFT, 50, _("IDS_COL_SUBNAME")},
	{wxLIST_FORMAT_LEFT, 50, _("IDS_COL_COMMENTS")},
};
static int const nColExistingPointsInfo = sizeof(colExistingPointsInfo) / sizeof(colExistingPointsInfo[0]);

/////////////////////////////////////////////////////////////////////////////

class CDlgDogDataTitle : public CListData
{
public:
	CDlgDogDataTitle(
			CDlgDog* pDlg,
			ARBDogTitlePtr title)
		: m_pDlg(pDlg)
		, m_Title(title)
	{
	}
	virtual wxString OnNeedText(long iCol) const;
	virtual void OnNeedListItem(long iCol, wxListItem& info) const;
	ARBDogTitlePtr GetData() const	{return m_Title;}
private:
	CDlgDog* m_pDlg;
	ARBDogTitlePtr m_Title;
};


wxString CDlgDogDataTitle::OnNeedText(long iCol) const
{
	wxString text;
	switch (iCol)
	{
	case 1:
		if (m_Title->GetDate().IsValid())
			text = m_Title->GetDate().GetString(ARBDate::eDashYYYYMMDD).c_str();
		else
			text = _("IDS_UNEARNED");
		break;
	case 2:
		text = m_Title->GetVenue().c_str();
		break;
	case 3:
		text = m_Title->GetGenericName().c_str();
		break;
	case 4:
		text = m_pDlg->m_pDoc->Book().GetConfig().GetTitleNiceName(m_Title->GetVenue(), m_Title->GetRawName()).c_str();
		break;
	}
	return text;
}


void CDlgDogDataTitle::OnNeedListItem(long iCol, wxListItem& info) const
{
	switch (iCol)
	{
	case 0:
		info.SetMask(info.GetMask() | wxLIST_MASK_IMAGE);
		if (m_Title->IsHidden())
		{
			if (m_Title->GetDate().IsValid())
			{
				if (m_Title->GetReceived())
					info.SetImage(m_pDlg->m_imgTitlesTitledHiddenReceived);
				else
					info.SetImage(m_pDlg->m_imgTitlesTitledHidden);
			}
			else
				info.SetImage(m_pDlg->m_imgTitlesHidden);
		}
		else
		{
			if (m_Title->GetDate().IsValid())
			{
				if (m_Title->GetReceived())
					info.SetImage(m_pDlg->m_imgTitlesTitledReceived);
				else
					info.SetImage(m_pDlg->m_imgTitlesTitled);
			}
			else
				info.SetImage(m_pDlg->m_imgTitlesEmpty);
		}
		break;
	default:
		info.SetText(OnNeedText(iCol));
		break;
	}
}


int wxCALLBACK CompareTitles(long item1, long item2, long sortData)
{
	CDlgDogDataTitlePtr pData1 = s_SortInfo.pThis->GetTitleDataByData(item1);
	CDlgDogDataTitlePtr pData2 = s_SortInfo.pThis->GetTitleDataByData(item2);
	ARBDogTitlePtr pTitle1 = pData1->GetData();
	ARBDogTitlePtr pTitle2 = pData2->GetData();
	int rc = 0;
	for (int i = 0; i < s_SortInfo.pCols->GetSize(); ++i)
	{
		int col = s_SortInfo.pCols->GetColumnAt(i);
		switch (col)
		{
		case 0: // Received and hidden
			if (!pTitle1->GetReceived() && pTitle2->GetReceived())
				rc = -1;
			else if (pTitle1->GetReceived() && !pTitle2->GetReceived())
				rc = 1;
			else if (!pTitle1->IsHidden() && pTitle2->IsHidden())
				rc = -1;
			else if (pTitle1->IsHidden() && !pTitle2->IsHidden())
				rc = 1;
			break;
		case 2: // venue
			if (pTitle1->GetVenue() < pTitle2->GetVenue())
				rc = -1;
			else if (pTitle1->GetVenue() > pTitle2->GetVenue())
				rc = 1;
			break;
		case 1: // date
			if (pTitle1->GetDate() < pTitle2->GetDate())
				rc = -1;
			else if (pTitle1->GetDate() > pTitle2->GetDate())
				rc = 1;
			break;
		case 3: // name
			{
				tstring n1 = pTitle1->GetGenericName();
				tstring n2 = pTitle2->GetGenericName();
				if (n1 < n2)
					rc = -1;
				else if (n1 > n2)
					rc = 1;
			}
			break;
		case 4: // nice name
			{
				tstring name1 = s_SortInfo.pDoc->Book().GetConfig().GetTitleNiceName(pTitle1->GetVenue(), pTitle1->GetRawName());
				tstring name2 = s_SortInfo.pDoc->Book().GetConfig().GetTitleNiceName(pTitle2->GetVenue(), pTitle2->GetRawName());
				if (name1 < name2)
					rc = -1;
				else if (name1 > name2)
					rc = 1;
			}
			break;
		}
		if (rc)
		{
			if (s_SortInfo.pCols->IsDescending(col))
				rc *= -1;
			break;
		}
	}
	return rc;
}

/////////////////////////////////////////////////////////////////////////////

class CDlgDogDataRegNum : public CListData
{
public:
	CDlgDogDataRegNum(
			CDlgDog* pDlg,
			ARBDogRegNumPtr regnum)
		: m_pDlg(pDlg)
		, m_RegNum(regnum)
	{
	}
	virtual wxString OnNeedText(long iCol) const;
	ARBDogRegNumPtr GetData() const	{return m_RegNum;}
private:
	CDlgDog* m_pDlg;
	ARBDogRegNumPtr m_RegNum;
};


wxString CDlgDogDataRegNum::OnNeedText(long iCol) const
{
	wxString text;
	switch (iCol)
	{
	case 0:
		text = m_RegNum->GetVenue().c_str();
		break;
	case 1:
		text = m_RegNum->GetNumber().c_str();
		break;
	case 2:
		text = m_RegNum->GetHeight().c_str();
		break;
	case 3:
		text = m_RegNum->GetReceived() ? wxT("x") : wxT("");
		break;
	case 4:
		text = m_RegNum->GetNote().c_str();
		text.Replace(wxT("\n"), wxT(" "));
		break;
	}
	return text;
}


int wxCALLBACK CompareRegNums(long item1, long item2, long sortData)
{
	CDlgDogDataRegNumPtr pData1 = s_SortInfo.pThis->GetRegNumDataByData(item1);
	CDlgDogDataRegNumPtr pData2 = s_SortInfo.pThis->GetRegNumDataByData(item2);
	ARBDogRegNumPtr pRegNum1 = pData1->GetData();
	ARBDogRegNumPtr pRegNum2 = pData2->GetData();
	int rc = 0;
	for (int i = 0; i < s_SortInfo.pCols->GetSize(); ++i)
	{
		int col = s_SortInfo.pCols->GetColumnAt(i);
		switch (col)
		{
		default:
		case 0: // venue
			if (pRegNum1->GetVenue() < pRegNum2->GetVenue())
				rc = -1;
			else if (pRegNum1->GetVenue() > pRegNum2->GetVenue())
				rc = 1;
			break;
		case 1: // number
			if (pRegNum1->GetNumber() < pRegNum2->GetNumber())
				rc = -1;
			else if (pRegNum1->GetNumber() > pRegNum2->GetNumber())
				rc = 1;
			break;
		case 2: // height
			if (pRegNum1->GetNumber() < pRegNum2->GetNumber())
				rc = -1;
			else if (pRegNum1->GetNumber() > pRegNum2->GetNumber())
				rc = 1;
			break;
		case 3: // received
			if (!pRegNum1->GetReceived() && pRegNum2->GetReceived())
				rc = -1;
			else if (pRegNum1->GetReceived() && !pRegNum2->GetReceived())
				rc = 1;
			break;
		case 4: // note
			if (pRegNum1->GetNote() < pRegNum2->GetNote())
				rc = -1;
			else if (pRegNum1->GetNote() > pRegNum2->GetNote())
				rc = 1;
			break;
		}
		if (rc)
		{
			if (s_SortInfo.pCols->IsDescending(col))
				rc *= -1;
			break;
		}
	}
	return rc;
}

/////////////////////////////////////////////////////////////////////////////

class CDlgDogDataPoint : public CListData
{
public:
	CDlgDogDataPoint(
			CDlgDog* pDlg,
			ARBDogExistingPointsPtr pts)
		: m_pDlg(pDlg)
		, m_Pts(pts)
	{
	}
	virtual wxString OnNeedText(long iCol) const;
	ARBDogExistingPointsPtr GetData() const	{return m_Pts;}
private:
	CDlgDog* m_pDlg;
	ARBDogExistingPointsPtr m_Pts;
};


wxString CDlgDogDataPoint::OnNeedText(long iCol) const
{
	wxString text;
	switch (iCol)
	{
	case 0:
		text = m_Pts->GetDate().GetString(ARBDate::eDashYMD).c_str();
		break;
	case 1: // Type
		text = ARBDogExistingPoints::GetPointTypeName(m_Pts->GetType()).c_str();
		break;
	case 2: // Points
		{
			otstringstream tmp;
			tmp << m_Pts->GetPoints();
			text = tmp.str().c_str();
		}
		break;
	case 3: // Other Points
		text = m_Pts->GetOtherPoints().c_str();
		break;
	case 4: // Venue
		text = m_Pts->GetVenue().c_str();
		break;
	case 5: // MultiQ
		text = m_Pts->GetMultiQ().c_str();
		break;
	case 6: // Division
		text = m_Pts->GetDivision().c_str();
		break;
	case 7: // Level
		text = m_Pts->GetLevel().c_str();
		break;
	case 8: // Event
		text = m_Pts->GetEvent().c_str();
		break;
	case 9: // SubName
		text = m_Pts->GetSubName().c_str();
		break;
	case 10: // Comment
		text = m_Pts->GetComment().c_str();
		text.Replace(wxT("\n"), wxT(" "));
		break;
	}
	return text;
}


int wxCALLBACK ComparePoints(long item1, long item2, long sortData)
{
	CDlgDogDataPointPtr pData1 = s_SortInfo.pThis->GetPointDataByData(item1);
	CDlgDogDataPointPtr pData2 = s_SortInfo.pThis->GetPointDataByData(item2);
	ARBDogExistingPointsPtr pExistingPoints1 = pData1->GetData();
	ARBDogExistingPointsPtr pExistingPoints2 = pData2->GetData();
	int rc = 0;
	for (int i = 0; i < s_SortInfo.pCols->GetSize(); ++i)
	{
		int col = s_SortInfo.pCols->GetColumnAt(i);
		switch (col)
		{

		default:
		case 0: // date
			if (pExistingPoints1->GetDate() < pExistingPoints2->GetDate())
				rc = -1;
			else if (pExistingPoints1->GetDate() > pExistingPoints2->GetDate())
				rc = 1;
			break;
		case 1: // Type
			if (pExistingPoints1->GetType() < pExistingPoints2->GetType())
				rc = -1;
			else if (pExistingPoints1->GetType() > pExistingPoints2->GetType())
				rc = 1;
			break;
		case 2: // Points
			if (pExistingPoints1->GetPoints() < pExistingPoints2->GetPoints())
				rc = -1;
			else if (pExistingPoints1->GetPoints() > pExistingPoints2->GetPoints())
				rc = 1;
			break;
		case 3: // Other Points
			if (pExistingPoints1->GetOtherPoints() < pExistingPoints2->GetOtherPoints())
				rc = -1;
			else if (pExistingPoints1->GetOtherPoints() > pExistingPoints2->GetOtherPoints())
				rc = 1;
			break;
		case 4: // Venue
			if (pExistingPoints1->GetVenue() < pExistingPoints2->GetVenue())
				rc = -1;
			else if (pExistingPoints1->GetVenue() > pExistingPoints2->GetVenue())
				rc = 1;
			break;
		case 5: // MultiQ
			if (pExistingPoints1->GetMultiQ() < pExistingPoints2->GetMultiQ())
				rc = -1;
			else if (pExistingPoints1->GetMultiQ() > pExistingPoints2->GetMultiQ())
				rc = 1;
			break;
		case 6: // Division
			if (pExistingPoints1->GetDivision() < pExistingPoints2->GetDivision())
				rc = -1;
			else if (pExistingPoints1->GetDivision() > pExistingPoints2->GetDivision())
				rc = 1;
			break;
		case 7: // Level
			if (pExistingPoints1->GetLevel() < pExistingPoints2->GetLevel())
				rc = -1;
			else if (pExistingPoints1->GetLevel() > pExistingPoints2->GetLevel())
				rc = 1;
			break;
		case 8: // Event
			if (pExistingPoints1->GetEvent() < pExistingPoints2->GetEvent())
				rc = -1;
			else if (pExistingPoints1->GetEvent() > pExistingPoints2->GetEvent())
				rc = 1;
			break;
		case 9: // Subname
			if (pExistingPoints1->GetSubName() < pExistingPoints2->GetSubName())
				rc = -1;
			else if (pExistingPoints1->GetSubName() > pExistingPoints2->GetSubName())
				rc = 1;
			break;
		case 10: // Comment
			if (pExistingPoints1->GetComment() < pExistingPoints2->GetComment())
				rc = -1;
			else if (pExistingPoints1->GetComment() > pExistingPoints2->GetComment())
				rc = 1;
			break;
		}
		if (rc)
		{
			if (s_SortInfo.pCols->IsDescending(col))
				rc *= -1;
			break;
		}
	}
	return rc;
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CDlgDog, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgDog::OnOk)
END_EVENT_TABLE()


CDlgDog::CDlgDog(
		CAgilityBookDoc* pDoc,
		ARBDogPtr pDog,
		wxWindow* pParent,
		int iSelectPage)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_pDog(pDog)
	, m_viewHidden(CAgilityBookOptions::GetViewHiddenTitles())
	, m_imgTitlesEmpty(-1)
	, m_imgTitlesTitled(-1)
	, m_imgTitlesTitledReceived(-1)
	, m_imgTitlesHidden(-1)
	, m_imgTitlesTitledHidden(-1)
	, m_imgTitlesTitledHiddenReceived(-1)
	, m_CallName(pDog->GetCallName().c_str())
	, m_Breed(pDog->GetBreed().c_str())
	, m_RegName(pDog->GetRegisteredName().c_str())
	, m_DOB(pDog->GetDOB())
	, m_IsDeceased(pDog->GetDeceased().IsValid())
	, m_Deceased(pDog->GetDeceased())
	, m_Notes(pDog->GetNote().c_str())
	, m_ctrlAge(NULL)
	, m_ctrlDDay(NULL)
	, m_ctrlTitles(NULL)
	, m_ctrlTitleEdit(NULL)
	, m_ctrlTitleDelete(NULL)
	, m_ctrlRegNums(NULL)
	, m_ctrlRegEdit(NULL)
	, m_ctrlRegDelete(NULL)
	, m_ctrlPoints(NULL)
	, m_ctrlPointsEdit(NULL)
	, m_ctrlPointsDelete(NULL)
	, m_ctrlSelectedPts(NULL)
	, m_Titles()
	, m_sortTitles(wxT("Titles"))
	, m_ViewHiddenTitles(CAgilityBookOptions::GetViewHiddenTitles())
	, m_sortPoints(wxT("ExistingPoints"))
	, m_RegNums()
	, m_sortRegNums(wxT("RegNums"))
	, m_ExistingPoints()
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);
	Create(pParent, wxID_ANY, _("IDS_COL_DOG"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	if (!m_Deceased.IsValid())
		m_Deceased.SetToday();
	m_pDog->GetTitles().Clone(m_Titles);
	m_sortTitles.Initialize(nColTitleInfo);
	pDog->GetRegNums().Clone(m_RegNums);
	m_sortRegNums.Initialize(nColRegNumInfo);
	pDog->GetExistingPoints().Clone(m_ExistingPoints);
	m_sortPoints.Initialize(nColExistingPointsInfo);

	// Controls (these are done first to control tab order)

	// Properties

	wxNotebook* notebook = new wxNotebook(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, 0);

	wxPanel* panelProp = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText* textCallName = new wxStaticText(panelProp, wxID_ANY,
		_("IDC_DOG_CALLNAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textCallName->Wrap(-1);

	wxTextCtrl* ctrlName = new wxTextCtrl(panelProp, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_CallName, _("IDS_BLANK_CALLNAME")));
	ctrlName->SetHelpText(_("HIDC_DOG_CALLNAME"));
	ctrlName->SetToolTip(_("HIDC_DOG_CALLNAME"));

	wxStaticText* textBreed = new wxStaticText(panelProp, wxID_ANY,
		_("IDC_DOG_BREED"),
		wxDefaultPosition, wxDefaultSize, 0);
	textBreed->Wrap(-1);

	wxTextCtrl* ctrlBreed = new wxTextCtrl(panelProp, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_Breed, TRIMVALIDATOR_TRIM_BOTH));
	ctrlBreed->SetHelpText(_("HIDC_DOG_BREED"));
	ctrlBreed->SetToolTip(_("HIDC_DOG_BREED"));

	wxStaticText* textRegName = new wxStaticText(panelProp, wxID_ANY,
		_("IDC_DOG_REG_NAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textRegName->Wrap(-1);

	wxTextCtrl* ctrlRegName = new wxTextCtrl(panelProp, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_RegName, TRIMVALIDATOR_TRIM_BOTH));
	ctrlRegName->SetHelpText(_("HIDC_DOG_REG_NAME"));
	ctrlRegName->SetToolTip(_("HIDC_DOG_REG_NAME"));

	wxStaticText* textAge = new wxStaticText(panelProp, wxID_ANY,
		_("IDC_DOG_AGE_TEXT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textAge->Wrap(-1);

	m_ctrlAge = new wxStaticText(panelProp, wxID_ANY,
		wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	m_ctrlAge->Wrap(-1);

	wxStaticText* textBDay = new wxStaticText(panelProp, wxID_ANY,
		_("IDC_DOG_DOB"),
		wxDefaultPosition, wxDefaultSize, 0);
	textBDay->Wrap(-1);

	wxDatePickerCtrl* ctrlBDay = new wxDatePickerCtrl(panelProp, wxID_ANY,
		wxDefaultDateTime, wxDefaultPosition, wxDefaultSize,
		wxDP_DROPDOWN|wxDP_SHOWCENTURY,
		CGenericValidator(&m_DOB));
	ctrlBDay->Connect(wxEVT_DATE_CHANGED, wxDateEventHandler(CDlgDog::OnDateChanged), NULL, this);
	ctrlBDay->SetHelpText(_("HIDC_DOG_DOB"));
	ctrlBDay->SetToolTip(_("HIDC_DOG_DOB"));

	wxCheckBox* ctrlDeceased = new wxCheckBox(panelProp, wxID_ANY,
		_("IDC_DOG_IS_DECEASED"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_IsDeceased));
	ctrlDeceased->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CDlgDog::OnDeceased), NULL, this);
	ctrlDeceased->SetHelpText(_("HIDC_DOG_IS_DECEASED"));
	ctrlDeceased->SetToolTip(_("HIDC_DOG_IS_DECEASED"));

	m_ctrlDDay = new wxDatePickerCtrl(panelProp, wxID_ANY, wxDefaultDateTime,
		wxDefaultPosition, wxDefaultSize,
		wxDP_DROPDOWN|wxDP_SHOWCENTURY,
		CGenericValidator(&m_Deceased));
	m_ctrlDDay->Connect(wxEVT_DATE_CHANGED, wxDateEventHandler(CDlgDog::OnDateChanged), NULL, this);
	m_ctrlDDay->SetHelpText(_("HIDC_DOG_DECEASED"));
	m_ctrlDDay->SetToolTip(_("HIDC_DOG_DECEASED"));

	wxStaticText* textNotes = new wxStaticText(panelProp, wxID_ANY,
		_("IDC_DOG_NOTES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textNotes->Wrap(-1);

	wxTextCtrl* ctrlNotes = new wxTextCtrl(panelProp, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(-1, 180), wxTE_MULTILINE|wxTE_WORDWRAP,
		CTrimValidator(&m_Notes, TRIMVALIDATOR_TRIM_BOTH));
	ctrlNotes->SetHelpText(_("HIDC_DOG_NOTES"));
	ctrlNotes->SetToolTip(_("HIDC_DOG_NOTES"));

	// Titles

	wxPanel* panelTitles = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	m_ctrlTitles = new CReportListCtrl(panelTitles, wxDefaultPosition, wxSize(500, -1), true, CReportListCtrl::eSortHeader, true);
	m_ctrlTitles->Connect(wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler(CDlgDog::OnTitleColumnClick), NULL, this);
	m_ctrlTitles->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CDlgDog::OnTitleItemSelected), NULL, this);
	m_ctrlTitles->Connect(wxEVT_COMMAND_LEFT_DCLICK, wxMouseEventHandler(CDlgDog::OnTitleDoubleClick), NULL, this);
	m_ctrlTitles->Connect(wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler(CDlgDog::OnTitleKeyDown), NULL, this);
	m_ctrlTitles->SetHelpText(_("HIDC_DOG_TITLE_TITLES"));
	m_ctrlTitles->SetToolTip(_("HIDC_DOG_TITLE_TITLES"));
	m_imgTitlesEmpty = m_ctrlTitles->AddIcon(wxIcon(CalEmpty_xpm));
	m_imgTitlesTitled = m_ctrlTitles->AddIcon(wxIcon(title_visible_xpm));
	m_imgTitlesTitledReceived = m_ctrlTitles->AddIcon(wxIcon(title_visible_have_xpm));
	m_imgTitlesHidden = m_ctrlTitles->AddIcon(wxIcon(hidden_xpm));
	m_imgTitlesTitledHidden = m_ctrlTitles->AddIcon(wxIcon(title_hidden_xpm));
	m_imgTitlesTitledHiddenReceived = m_ctrlTitles->AddIcon(wxIcon(title_hidden_have_xpm));
	int i;
	for (i = 0; i < nColTitleInfo; ++i)
	{
		m_ctrlTitles->InsertColumn(i, wxEmptyString, colTitleInfo[i].fmt, colTitleInfo[i].cx);
	}
	SetColumnTitleHeaders();

	wxButton* btnTitleNew = new wxButton(panelTitles, wxID_ANY,
		_("IDC_DOG_TITLE_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnTitleNew->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgDog::OnTitleNew), NULL, this);
	btnTitleNew->SetHelpText(_("HIDC_DOG_TITLE_NEW"));
	btnTitleNew->SetToolTip(_("HIDC_DOG_TITLE_NEW"));

	m_ctrlTitleEdit = new wxButton(panelTitles, wxID_ANY,
		_("IDC_DOG_TITLE_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlTitleEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgDog::OnTitleEdit), NULL, this);
	m_ctrlTitleEdit->SetHelpText(_("HIDC_DOG_TITLE_EDIT"));
	m_ctrlTitleEdit->SetToolTip(_("HIDC_DOG_TITLE_EDIT"));

	m_ctrlTitleDelete = new wxButton(panelTitles, wxID_ANY,
		_("IDC_DOG_TITLE_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlTitleDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgDog::OnTitleDelete), NULL, this);
	m_ctrlTitleDelete->SetHelpText(_("HIDC_DOG_TITLE_DELETE"));
	m_ctrlTitleDelete->SetToolTip(_("HIDC_DOG_TITLE_DELETE"));

	wxCheckBox* ctrlHidden = new wxCheckBox(panelTitles, wxID_ANY,
		_("IDC_DOG_TITLE_HIDDEN"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_ViewHiddenTitles));
	ctrlHidden->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CDlgDog::OnTitleHidden), NULL, this);
	ctrlHidden->SetHelpText(_("HIDC_DOG_TITLE_HIDDEN"));
	ctrlHidden->SetToolTip(_("HIDC_DOG_TITLE_HIDDEN"));

	// RegNums

	wxPanel* panelRegNums = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	m_ctrlRegNums = new CReportListCtrl(panelRegNums, wxDefaultPosition, wxSize(500, -1), true, CReportListCtrl::eSortHeader, true);
	m_ctrlRegNums->Connect(wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler(CDlgDog::OnRegNumColumnClick), NULL, this);
	m_ctrlRegNums->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CDlgDog::OnRegNumItemSelected), NULL, this);
	m_ctrlRegNums->Connect(wxEVT_COMMAND_LEFT_DCLICK, wxMouseEventHandler(CDlgDog::OnRegNumDoubleClick), NULL, this);
	m_ctrlRegNums->Connect(wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler(CDlgDog::OnRegNumKeyDown), NULL, this);
	m_ctrlRegNums->SetHelpText(_("HIDC_DOGNUM_REG_NUMS"));
	m_ctrlRegNums->SetToolTip(_("HIDC_DOGNUM_REG_NUMS"));
	for (i = 0; i < nColRegNumInfo; ++i)
	{
		m_ctrlRegNums->InsertColumn(i, wxEmptyString, colRegNumInfo[i].fmt, colRegNumInfo[i].cx);
	}
	SetColumnRegNumHeaders();

	wxButton* btnRegNew = new wxButton(panelRegNums, wxID_ANY,
		_("IDC_DOGNUM_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnRegNew->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgDog::OnRegNumNew), NULL, this);
	btnRegNew->SetHelpText(_("HIDC_DOGNUM_NEW"));
	btnRegNew->SetToolTip(_("HIDC_DOGNUM_NEW"));

	m_ctrlRegEdit = new wxButton(panelRegNums, wxID_ANY,
		_("IDC_DOGNUM_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRegEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgDog::OnRegNumEdit), NULL, this);
	m_ctrlRegEdit->SetHelpText(_("HIDC_DOGNUM_EDIT"));
	m_ctrlRegEdit->SetToolTip(_("HIDC_DOGNUM_EDIT"));

	m_ctrlRegDelete = new wxButton(panelRegNums, wxID_ANY,
		_("IDC_DOGNUM_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRegDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgDog::OnRegNumDelete), NULL, this);
	m_ctrlRegDelete->SetHelpText(_("HIDC_DOGNUM_DELETE"));
	m_ctrlRegDelete->SetToolTip(_("HIDC_DOGNUM_DELETE"));

	// Points

	wxPanel* panelPoints = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	m_ctrlPoints = new CReportListCtrl(panelPoints, wxDefaultPosition, wxSize(500, -1), false, CReportListCtrl::eSortHeader, true);
	m_ctrlPoints->Connect(wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler(CDlgDog::OnPointsColumnClick), NULL, this);
	m_ctrlPoints->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CDlgDog::OnPointsItemSelected), NULL, this);
	m_ctrlPoints->Connect(wxEVT_COMMAND_LEFT_DCLICK, wxMouseEventHandler(CDlgDog::OnPointsDoubleClick), NULL, this);
	m_ctrlPoints->Connect(wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler(CDlgDog::OnPointsKeyDown), NULL, this);
	m_ctrlPoints->SetHelpText(_("HIDC_DOGPTS_POINTS"));
	m_ctrlPoints->SetToolTip(_("HIDC_DOGPTS_POINTS"));
	for (i = 0; i < nColExistingPointsInfo; ++i)
	{
		m_ctrlPoints->InsertColumn(i, wxEmptyString, colExistingPointsInfo[i].fmt, colExistingPointsInfo[i].cx);
	}
	SetColumnPointsHeaders();

	wxButton* btnPtsNew = new wxButton(panelPoints, wxID_ANY,
		_("IDC_DOGPTS_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRegDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgDog::OnPointsNew), NULL, this);
	btnPtsNew->SetHelpText(_("HIDC_DOGPTS_NEW"));
	btnPtsNew->SetToolTip(_("HIDC_DOGPTS_NEW"));

	m_ctrlPointsEdit = new wxButton(panelPoints, wxID_ANY,
		_("IDC_DOGPTS_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRegDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgDog::OnPointsEdit), NULL, this);
	m_ctrlPointsEdit->SetHelpText(_("HIDC_DOGPTS_EDIT"));
	m_ctrlPointsEdit->SetToolTip(_("HIDC_DOGPTS_EDIT"));

	m_ctrlPointsDelete = new wxButton(panelPoints, wxID_ANY,
		_("IDC_DOGPTS_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlRegDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgDog::OnPointsDelete), NULL, this);
	m_ctrlPointsDelete->SetHelpText(_("HIDC_DOGPTS_DELETE"));
	m_ctrlPointsDelete->SetToolTip(_("HIDC_DOGPTS_DELETE"));

	wxStaticText* textPts = new wxStaticText(panelPoints, wxID_ANY,
		_("IDC_DOGPTS_SELECTED_PTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPts->Wrap(-1);

	m_ctrlSelectedPts = new wxStaticText(panelPoints, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(50, -1), wxALIGN_CENTRE|wxST_NO_AUTORESIZE|wxSUNKEN_BORDER);
	m_ctrlSelectedPts->Wrap(-1);

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerProp = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textCallName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerName->Add(ctrlName, 2, wxALL, 5);
	sizerName->Add(textBreed, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerName->Add(ctrlBreed, 3, wxALL, 5);

	sizerProp->Add(sizerName, 0, wxEXPAND, 5);

	wxBoxSizer* sizerRegName = new wxBoxSizer(wxHORIZONTAL);
	sizerRegName->Add(textRegName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerRegName->Add(ctrlRegName, 1, wxALL, 5);

	sizerProp->Add(sizerRegName, 0, wxEXPAND, 5);

	wxBoxSizer* sizerAge = new wxBoxSizer(wxHORIZONTAL);
	sizerAge->Add(textAge, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerAge->Add(m_ctrlAge, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerAge->Add(textBDay, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerAge->Add(ctrlBDay, 0, wxALL, 5);
	sizerAge->Add(ctrlDeceased, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerAge->Add(m_ctrlDDay, 0, wxALL, 5);

	sizerProp->Add(sizerAge, 0, wxEXPAND, 5);
	sizerProp->Add(textNotes, 0, wxLEFT|wxRIGHT|wxTOP, 5);
	sizerProp->Add(ctrlNotes, 1, wxALL|wxEXPAND, 5);

	panelProp->SetSizer(sizerProp);
	panelProp->Layout();
	sizerProp->Fit(panelProp);
	notebook->AddPage(panelProp, _("IDD_DOG_PROP"), true);

	wxBoxSizer* sizerTitles = new wxBoxSizer(wxVERTICAL);
	sizerTitles->Add(m_ctrlTitles, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerTitleBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerTitleBtns->Add(btnTitleNew, 0, wxALL, 5);
	sizerTitleBtns->Add(m_ctrlTitleEdit, 0, wxALL, 5);
	sizerTitleBtns->Add(m_ctrlTitleDelete, 0, wxALL, 5);
	sizerTitleBtns->Add(0, 0, 1, wxEXPAND, 5);
	sizerTitleBtns->Add(ctrlHidden, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerTitles->Add(sizerTitleBtns, 0, wxEXPAND, 5);

	panelTitles->SetSizer(sizerTitles);
	panelTitles->Layout();
	sizerTitles->Fit(panelTitles);
	notebook->AddPage(panelTitles, _("IDD_DOG_TITLES"), false);

	wxBoxSizer* sizerReg = new wxBoxSizer(wxVERTICAL);
	sizerReg->Add(m_ctrlRegNums, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerRegBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerRegBtns->Add(btnRegNew, 0, wxALL, 5);
	sizerRegBtns->Add(m_ctrlRegEdit, 0, wxALL, 5);
	sizerRegBtns->Add(m_ctrlRegDelete, 0, wxALL, 5);

	sizerReg->Add(sizerRegBtns, 0, wxEXPAND, 5);

	panelRegNums->SetSizer(sizerReg);
	panelRegNums->Layout();
	sizerReg->Fit(panelRegNums);
	notebook->AddPage(panelRegNums, _("IDD_DOG_NUMBERS"), false);

	wxBoxSizer* sizerPts = new wxBoxSizer(wxVERTICAL);
	sizerPts->Add(m_ctrlPoints, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerPtsBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerPtsBtns->Add(btnPtsNew, 0, wxALL, 5);
	sizerPtsBtns->Add(m_ctrlPointsEdit, 0, wxALL, 5);
	sizerPtsBtns->Add(m_ctrlPointsDelete, 0, wxALL, 5);
	sizerPtsBtns->Add(0, 0, 1, wxEXPAND, 5);
	sizerPtsBtns->Add(textPts, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerPtsBtns->Add(m_ctrlSelectedPts, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerPts->Add(sizerPtsBtns, 0, wxEXPAND, 5);

	panelPoints->SetSizer(sizerPts);
	panelPoints->Layout();
	sizerPts->Fit(panelPoints);
	notebook->AddPage(panelPoints, _("IDD_DOG_POINTS"), false);

	bSizer->Add(notebook, 1, wxEXPAND | wxALL, 5);

	wxSizer* sdbSizer = CreateButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	UpdateDeceased();
	ListTitles();
	ListRegNums();
	ListExistingPoints();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	notebook->SetSelection(iSelectPage);
}


CDlgDog::~CDlgDog()
{
	if (m_viewHidden != CAgilityBookOptions::GetViewHiddenTitles())
	{
		std::vector<CVenueFilter> venues;
		CFilterOptions::Options().GetFilterVenue(venues);
		for (ARBDogList::iterator iterDogs = m_pDoc->Book().GetDogs().begin(); iterDogs != m_pDoc->Book().GetDogs().end(); ++iterDogs)
			for (ARBDogTitleList::iterator iterTitle = (*iterDogs)->GetTitles().begin(); iterTitle != (*iterDogs)->GetTitles().end(); ++iterTitle)
				m_pDoc->ResetVisibility(venues, *iterTitle);
		CUpdateHint hint(UPDATE_POINTS_VIEW);
		m_pDoc->UpdateAllViews(NULL, &hint);
	}
}


CDlgDogDataTitlePtr CDlgDog::GetTitleData(long index) const
{
	return tr1::dynamic_pointer_cast<CDlgDogDataTitle, CListData>(m_ctrlTitles->GetData(index));
}


CDlgDogDataTitlePtr CDlgDog::GetTitleDataByData(long index) const
{
	return tr1::dynamic_pointer_cast<CDlgDogDataTitle, CListData>(m_ctrlTitles->GetDataByData(index));
}


CDlgDogDataRegNumPtr CDlgDog::GetRegNumData(long index) const
{
	return tr1::dynamic_pointer_cast<CDlgDogDataRegNum, CListData>(m_ctrlRegNums->GetData(index));
}


CDlgDogDataRegNumPtr CDlgDog::GetRegNumDataByData(long index) const
{
	return tr1::dynamic_pointer_cast<CDlgDogDataRegNum, CListData>(m_ctrlRegNums->GetDataByData(index));
}


CDlgDogDataPointPtr CDlgDog::GetPointData(long index) const
{
	return tr1::dynamic_pointer_cast<CDlgDogDataPoint, CListData>(m_ctrlPoints->GetData(index));
}


CDlgDogDataPointPtr CDlgDog::GetPointDataByData(long index) const
{
	return tr1::dynamic_pointer_cast<CDlgDogDataPoint, CListData>(m_ctrlPoints->GetDataByData(index));
}


void CDlgDog::UpdateAge()
{
	ARBDate current(ARBDate::Today());
	if (m_IsDeceased)
		current = m_Deceased;
	long ageDays = current - m_DOB;
	m_ctrlAge->SetLabel(ARBDouble::str(ageDays/365.0, 1).c_str());
}


void CDlgDog::UpdateDeceased()
{
	m_ctrlDDay->Show(m_IsDeceased);
	UpdateAge();
}


void CDlgDog::SetColumnTitleHeaders()
{
	for (int i = 0; i < nColTitleInfo; ++i)
	{
		otstringstream tmp;
		if (colTitleInfo[i].idText)
			tmp << wxGetTranslation(colTitleInfo[i].idText) << ' ';
		tmp << '(' << m_sortTitles.FindColumnOrder(i) + 1 << ')';
		wxListItem col;
		col.SetMask(wxLIST_MASK_TEXT);
		col.SetColumn(i);
		col.SetText(tmp.str().c_str());
		m_ctrlTitles->SetColumn(i, col);
		m_ctrlTitles->SetColumnSort(i, m_sortTitles.IsDescending(i) ? -1 : 1);
	}
}


void CDlgDog::ListTitles()
{
	wxBusyCursor wait;
	ARBDogTitlePtr pSelected;
	int i = m_ctrlTitles->GetFirstSelected();
	if (0 <= i)
	{
		CDlgDogDataTitlePtr pData = GetTitleData(i);
		if (pData)
			pSelected = pData->GetData();
	}
	m_ctrlTitles->DeleteAllItems();
	for (ARBDogTitleList::const_iterator iterTitle = m_Titles.begin();
		iterTitle != m_Titles.end();
		++iterTitle)
	{
		ARBDogTitlePtr pTitle = (*iterTitle);
		if (!CAgilityBookOptions::GetViewHiddenTitles()
		&& pTitle->IsHidden())
		{
			continue;
		}
		CDlgDogDataTitlePtr data(new CDlgDogDataTitle(this, pTitle));
		m_ctrlTitles->InsertItem(data);
	}
	for (i = 0; i < nColTitleInfo; ++i)
		m_ctrlTitles->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
	s_SortInfo.pThis = this;
	s_SortInfo.pDoc = m_pDoc;
	s_SortInfo.pCols = &m_sortTitles;
	m_ctrlTitles->SortItems(CompareTitles, 0);
	if (pSelected)
	{
		for (i = 0; i < m_ctrlTitles->GetItemCount(); ++i)
		{
			CDlgDogDataTitlePtr pTitle = GetTitleData(i);
			if (pTitle && pTitle->GetData() == pSelected) // compare by ptr is fine.
			{
				m_ctrlTitles->Select(i, true);
				m_ctrlTitles->EnsureVisible(i);
				break;
			}
		}
	}
	UpdateTitleButtons();
}


void CDlgDog::UpdateTitleButtons()
{
	if (0 == m_ctrlTitles->GetSelectedItemCount())
	{
		m_ctrlTitleEdit->Enable(false);
		m_ctrlTitleDelete->Enable(false);
	}
	else
	{
		m_ctrlTitleEdit->Enable(true);
		m_ctrlTitleDelete->Enable(true);
	}
}


void CDlgDog::EditTitle()
{
	long i = m_ctrlTitles->GetFirstSelected();
	if (0 <= i)
	{
		CDlgDogDataTitlePtr pTitle = GetTitleData(i);
		CDlgTitle dlg(m_pDoc->Book().GetConfig(), m_Titles, pTitle->GetData(), this);
		if (wxID_OK == dlg.ShowModal())
			ListTitles();
	}
}


void CDlgDog::SetColumnRegNumHeaders()
{
	for (int i = 0; i < nColRegNumInfo; ++i)
	{
		otstringstream tmp;
		if (colRegNumInfo[i].idText)
			tmp << wxGetTranslation(colRegNumInfo[i].idText) << ' ';
		tmp << '(' << m_sortRegNums.FindColumnOrder(i) + 1 << ')';
		wxListItem col;
		col.SetMask(wxLIST_MASK_TEXT);
		col.SetColumn(i);
		col.SetText(tmp.str().c_str());
		m_ctrlRegNums->SetColumn(i, col);
		m_ctrlRegNums->SetColumnSort(i, m_sortRegNums.IsDescending(i) ? -1 : 1);
	}
}


void CDlgDog::ListRegNums()
{
	ARBDogRegNumPtr pSelected;
	long i = m_ctrlRegNums->GetFirstSelected();
	if (0 <= i)
	{
		CDlgDogDataRegNumPtr pData = GetRegNumData(i);
		if (pData)
			pSelected = pData->GetData();
	}
	m_ctrlRegNums->DeleteAllItems();
	for (ARBDogRegNumList::const_iterator iterRegNum = m_RegNums.begin();
		iterRegNum != m_RegNums.end();
		++iterRegNum)
	{
		CDlgDogDataRegNumPtr pData(new CDlgDogDataRegNum(this, *iterRegNum));
		m_ctrlRegNums->InsertItem(pData);
	}
	for (i = 0; i < nColRegNumInfo; ++i)
		m_ctrlRegNums->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
	s_SortInfo.pThis = this;
	s_SortInfo.pDoc = m_pDoc;
	s_SortInfo.pCols = &m_sortRegNums;
	m_ctrlRegNums->SortItems(CompareRegNums, 0);
	if (pSelected)
	{
		for (i = 0; i < m_ctrlRegNums->GetItemCount(); ++i)
		{
			CDlgDogDataRegNumPtr pData = GetRegNumData(i);
			if (pData && pData->GetData() == pSelected) // compare by ptr is fine.
			{
				m_ctrlRegNums->Select(i, true);
				m_ctrlRegNums->EnsureVisible(i);
				break;
			}
		}
	}
	UpdateRegNumButtons();
}


void CDlgDog::UpdateRegNumButtons()
{
	if (0 == m_ctrlRegNums->GetSelectedItemCount())
	{
		m_ctrlRegEdit->Enable(false);
		m_ctrlRegDelete->Enable(false);
	}
	else
	{
		m_ctrlRegEdit->Enable(true);
		m_ctrlRegDelete->Enable(true);
	}
}


void CDlgDog::EditRegNum()
{
	long i = m_ctrlRegNums->GetFirstSelected();
	if (0 <= i)
	{
		CDlgDogDataRegNumPtr pData = GetRegNumData(i);
		ARBDogRegNumPtr pRegNum;
		if (pData)
			pRegNum = pData->GetData();
		CDlgRegNum dlg(m_pDoc->Book().GetConfig(), m_RegNums, pRegNum, this);
		if (wxID_OK == dlg.ShowModal())
			ListRegNums();
	}
}


void CDlgDog::SetColumnPointsHeaders()
{
	for (int i = 0; i < nColExistingPointsInfo; ++i)
	{
		otstringstream tmp;
		if (colExistingPointsInfo[i].idText)
			tmp << wxGetTranslation(colExistingPointsInfo[i].idText) << ' ';
		tmp << '(' << m_sortPoints.FindColumnOrder(i) + 1 << ')';
		wxListItem col;
		col.SetMask(wxLIST_MASK_TEXT);
		col.SetColumn(i);
		col.SetText(tmp.str().c_str());
		m_ctrlPoints->SetColumn(i, col);
		m_ctrlPoints->SetColumnSort(i, m_sortPoints.IsDescending(i) ? -1 : 1);
	}
}


void CDlgDog::ListExistingPoints()
{
	bool bSetWidth = m_ctrlPoints->GetItemCount() == 0;
	ARBDogExistingPointsPtr pSelected;
	int i = m_ctrlPoints->GetFirstSelected();
	if (0 <= i)
	{
		CDlgDogDataPointPtr pData = GetPointData(i);
		if (pData)
			pSelected = pData->GetData();
	}
	m_ctrlPoints->DeleteAllItems();
	i = 0;
	for (ARBDogExistingPointsList::const_iterator iterExistingPoints = m_ExistingPoints.begin();
		iterExistingPoints != m_ExistingPoints.end();
		++i, ++iterExistingPoints)
	{
		CDlgDogDataPointPtr data(new CDlgDogDataPoint(this, *iterExistingPoints));
		m_ctrlPoints->InsertItem(data);
	}
	if (bSetWidth)
		for (i = 0; i < nColExistingPointsInfo; ++i)
			m_ctrlPoints->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
	s_SortInfo.pThis = this;
	s_SortInfo.pDoc = m_pDoc;
	s_SortInfo.pCols = &m_sortPoints;
	m_ctrlPoints->SortItems(ComparePoints, 0);
	if (pSelected)
	{
		for (i = 0; i < m_ctrlPoints->GetItemCount(); ++i)
		{
			CDlgDogDataPointPtr pData = GetPointData(i);
			ARBDogExistingPointsPtr pExistingPoints;
			if (pData)
				pExistingPoints = pData->GetData();
			if (pExistingPoints == pSelected) // compare by ptr is fine.
			{
				m_ctrlPoints->Select(i, true);
				m_ctrlPoints->EnsureVisible(i);
				break;
			}
		}
	}
	UpdatePointsButtons();
}


void CDlgDog::UpdatePointsButtons()
{
	bool bEnableEdit = false;
	bool bEnableDelete = false;
	int selected = m_ctrlPoints->GetSelectedItemCount();
	if (1 == selected)
	{
		bEnableEdit = true;
		bEnableDelete = true;
	}
	else if (1 < selected)
	{
		bEnableDelete = true;
	}
	m_ctrlPointsEdit->Enable(bEnableEdit);
	m_ctrlPointsDelete->Enable(bEnableDelete);
	wxString str;
	if (0 < selected)
	{
		double total = 0;
		std::vector<long> indices;
		m_ctrlPoints->GetSelection(indices);
		for (std::vector<long>::iterator i = indices.begin(); i != indices.end(); ++i)
		{
			CDlgDogDataPointPtr pData = GetPointData(*i);
			if (pData)
				total += pData->GetData()->GetPoints();
		}
		otstringstream tmp;
		tmp << total;
		str = tmp.str().c_str();
	}
	m_ctrlSelectedPts->SetLabel(str);
}


void CDlgDog::EditPoints()
{
	long i = m_ctrlPoints->GetSelection(true);
	if (0 <= i)
	{
		CDlgDogDataPointPtr pData = GetPointData(i);
		ARBDogExistingPointsPtr pExistingPoints;
		if (pData)
			pExistingPoints = pData->GetData();
		CDlgExistingPoints dlg(m_pDoc, m_ExistingPoints, pExistingPoints, this);
		if (wxID_OK == dlg.ShowModal())
			ListExistingPoints();
	}
}


void CDlgDog::OnDateChanged(wxDateEvent& evt)
{
	TransferDataFromWindow();
	UpdateAge();
}


void CDlgDog::OnDeceased(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	UpdateDeceased();
}


void CDlgDog::OnTitleColumnClick(wxListEvent& evt)
{
	m_sortTitles.SetColumnOrder(evt.GetColumn());
	SetColumnTitleHeaders();
	s_SortInfo.pThis = this;
	s_SortInfo.pDoc = m_pDoc;
	s_SortInfo.pCols = &m_sortTitles;
	m_ctrlTitles->SortItems(CompareTitles, 0);
	m_sortTitles.Save();
}


void CDlgDog::OnTitleItemSelected(wxListEvent& evt)
{
	UpdateTitleButtons();
}


void CDlgDog::OnTitleDoubleClick(wxMouseEvent& evt)
{
	EditTitle();
	evt.Skip();
}


void CDlgDog::OnTitleKeyDown(wxListEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		EditTitle();
		break;
	}
	evt.Skip();
}


void CDlgDog::OnTitleNew(wxCommandEvent& evt)
{
	CDlgTitle dlg(m_pDoc->Book().GetConfig(), m_Titles, ARBDogTitlePtr(), this);
	if (wxID_OK == dlg.ShowModal())
		ListTitles();
}


void CDlgDog::OnTitleEdit(wxCommandEvent& evt)
{
	EditTitle();
}


void CDlgDog::OnTitleDelete(wxCommandEvent& evt)
{
	long i = m_ctrlTitles->GetFirstSelected();
	if (0 <= i)
	{
		CDlgDogDataTitlePtr pTitle = GetTitleData(i);
		m_Titles.DeleteTitle(pTitle->GetData());
		m_ctrlTitles->DeleteItem(i);
		UpdateTitleButtons();
	}
}


void CDlgDog::OnTitleHidden(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	CAgilityBookOptions::SetViewHiddenTitles(m_ViewHiddenTitles);
	ListTitles();
}


void CDlgDog::OnRegNumColumnClick(wxListEvent& evt)
{
	m_sortRegNums.SetColumnOrder(evt.GetColumn());
	SetColumnRegNumHeaders();
	s_SortInfo.pThis = this;
	s_SortInfo.pDoc = m_pDoc;
	s_SortInfo.pCols = &m_sortRegNums;
	m_ctrlRegNums->SortItems(CompareRegNums, 0);
	m_sortRegNums.Save();
}


void CDlgDog::OnRegNumItemSelected(wxListEvent& evt)
{
	UpdateRegNumButtons();
}


void CDlgDog::OnRegNumDoubleClick(wxMouseEvent& evt)
{
	EditRegNum();
}


void CDlgDog::OnRegNumKeyDown(wxListEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		EditRegNum();
		break;
	}
	evt.Skip();
}


void CDlgDog::OnRegNumNew(wxCommandEvent& evt)
{
	CDlgRegNum dlg(m_pDoc->Book().GetConfig(), m_RegNums, ARBDogRegNumPtr(), this);
	if (wxID_OK == dlg.ShowModal())
		ListRegNums();
}


void CDlgDog::OnRegNumEdit(wxCommandEvent& evt)
{
	EditRegNum();
}


void CDlgDog::OnRegNumDelete(wxCommandEvent& evt)
{
	long i = m_ctrlRegNums->GetFirstSelected();
	if (0 <= i)
	{
		CDlgDogDataRegNumPtr pData = GetRegNumData(i);
		ARBDogRegNumPtr pRegNum;
		if (pData)
			pRegNum = pData->GetData();
		m_RegNums.DeleteRegNum(pRegNum->GetVenue(), pRegNum->GetNumber());
		m_ctrlRegNums->DeleteItem(i);
		UpdateRegNumButtons();
	}
}


void CDlgDog::OnPointsColumnClick(wxListEvent& evt)
{
	m_sortPoints.SetColumnOrder(evt.GetColumn());
	SetColumnPointsHeaders();
	s_SortInfo.pThis = this;
	s_SortInfo.pDoc = m_pDoc;
	s_SortInfo.pCols = &m_sortPoints;
	m_ctrlPoints->SortItems(ComparePoints, 0);
	m_sortPoints.Save();
}


void CDlgDog::OnPointsItemSelected(wxListEvent& evt)
{
	UpdatePointsButtons();
}


void CDlgDog::OnPointsDoubleClick(wxMouseEvent& evt)
{
	EditPoints();
}


void CDlgDog::OnPointsKeyDown(wxListEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		EditPoints();
		break;
	}
	evt.Skip();
}


void CDlgDog::OnPointsNew(wxCommandEvent& evt)
{
	CDlgExistingPoints dlg(m_pDoc, m_ExistingPoints, ARBDogExistingPointsPtr(), this);
	if (wxID_OK == dlg.ShowModal())
		ListExistingPoints();
}


void CDlgDog::OnPointsEdit(wxCommandEvent& evt)
{
	EditPoints();
}


void CDlgDog::OnPointsDelete(wxCommandEvent& evt)
{
	std::vector<long> indices;
	if (0 < m_ctrlPoints->GetSelection(indices))
	{
		for (std::vector<long>::reverse_iterator i = indices.rbegin(); i != indices.rend(); ++i)
		{
			CDlgDogDataPointPtr pData = GetPointData(*i);
			ARBDogExistingPointsPtr pExistingPoints;
			if (pData)
				pExistingPoints = pData->GetData();
			m_ExistingPoints.DeleteExistingPoints(pExistingPoints);
			m_ctrlPoints->DeleteItem(*i);
		}
		UpdatePointsButtons();
	}
}


void CDlgDog::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;
	if (m_IsDeceased)
		m_Deceased.clear();

	m_pDog->SetCallName(m_CallName.c_str());
	m_pDog->SetDOB(m_DOB);
	m_pDog->SetDeceased(m_Deceased);
	m_pDog->SetBreed(m_Breed.c_str());
	m_pDog->SetRegisteredName(m_RegName.c_str());
	m_pDog->SetNote(m_Notes.c_str());
	m_pDog->GetTitles() = m_Titles;
	m_pDog->GetRegNums() = m_RegNums;
	m_pDog->GetExistingPoints() = m_ExistingPoints;
	m_pDoc->ResetVisibility();

	EndDialog(wxID_OK);
}
