/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgDog class
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2014-12-31 Changed pixels to dialog units.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-03-14 Merged DlgRun* into here.
 * 2009-02-09 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-10-17 Points view didn't update 'View Hidden' after canceling.
 * 2004-02-03 Broke dialog up into pages.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-08-18 Added a deceased date for a dog.
 *
 * DlgDogNumbers
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-02-03 Broke dialog up into pages.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-08-18 Added a deceased date for a dog.
 *
 * DlgDogPoints
 * 2016-04-29 Separate lifetime points from title (run) points.
 * 2006-07-15 Only adjust column widths the first time.
 *            Add a selected-item total.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-02-03 Created.
 *
 * DlgDogProp
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-05-30 DOB/Deceased were not initialized in ctor.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-08-18 Added a deceased date for a dog.
 *
 * DlgDogTitles
 * 2013-05-18 Modifying titles caused loss of view filtering.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-01-10 Allow titles to be optionally entered multiple times.
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-02-03 Broke dialog up into pages.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-08-18 Added a deceased date for a dog.
 */

#include "stdafx.h"
#include "DlgDog.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "DlgExistingPoints.h"
#include "DlgRegNum.h"
#include "DlgTitle.h"
#include "FilterOptions.h"
#include "ImageHelper.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBDog.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ListCtrl.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/Validators.h"
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/notebook.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

struct DogSortInfo : public SortInfo
{
	CDlgDog* pThis;
	CAgilityBookDoc* pDoc;
	CColumnOrder* pCols;

	DogSortInfo(CDlgDog* This, CAgilityBookDoc* doc, CColumnOrder* cols)
		: pThis(This)
		, pDoc(doc)
		, pCols(cols)
	{
	}
};


static struct
{
	int fmt;
	int cx;
	wchar_t const* idText;
} const colTitleInfo[] =
{
	{wxLIST_FORMAT_LEFT, 5, nullptr},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_DATE")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_VENUE")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_TITLE")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_NAME")},
};
static int const nColTitleInfo = sizeof(colTitleInfo) / sizeof(colTitleInfo[0]);


static struct
{
	int fmt;
	int cx;
	wchar_t const* idText;
} const colRegNumInfo[] =
{
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_VENUE")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_NUMBER")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_HEIGHT")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_RECEIVED")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_NOTE")},
};
static int const nColRegNumInfo = sizeof(colRegNumInfo) / sizeof(colRegNumInfo[0]);


static struct
{
	int fmt;
	int cx;
	wchar_t const* idText;
} const colExistingPointsInfo[] =
{
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_DATE")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_TYPE")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_POINTS")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_OTHERPOINTS")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_VENUE")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_MULTIQ")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_DIVISION")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_LEVEL")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_EVENT")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_SUBNAME")},
	{wxLIST_FORMAT_LEFT, 25, arbT("IDS_COL_COMMENTS")},
};
static int const nColExistingPointsInfo = sizeof(colExistingPointsInfo) / sizeof(colExistingPointsInfo[0]);

/////////////////////////////////////////////////////////////////////////////

class CDlgDogDataTitle : public CListData
{
public:
	CDlgDogDataTitle(
			CDlgDog* pDlg,
			ARBDogTitlePtr const& inTitle)
		: m_pDlg(pDlg)
		, m_Title(inTitle)
	{
	}
	virtual std::wstring OnNeedText(long iCol) const;
	virtual void OnNeedListItem(long iCol, wxListItem& info) const;
	ARBDogTitlePtr GetData() const	{return m_Title;}
private:
	CDlgDog* m_pDlg;
	ARBDogTitlePtr m_Title;
};


std::wstring CDlgDogDataTitle::OnNeedText(long iCol) const
{
	std::wstring text;
	switch (iCol)
	{
	case 1:
		if (m_Title->GetDate().IsValid())
			text = m_Title->GetDate().GetString();
		else
			text = _("IDS_UNEARNED");
		break;
	case 2:
		text = m_Title->GetVenue();
		break;
	case 3:
		text = m_Title->GetGenericName();
		break;
	case 4:
		text = m_pDlg->m_pDoc->Book().GetConfig().GetTitleNiceName(m_Title->GetVenue(), m_Title->GetRawName());
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
		info.SetText(StringUtil::stringWX(OnNeedText(iCol)));
		break;
	}
}


int wxCALLBACK CompareTitles(CListDataPtr const& item1, CListDataPtr const& item2, SortInfo const* pSortInfo)
{
	DogSortInfo const* pInfo = dynamic_cast<DogSortInfo const*>(pSortInfo);
	assert(pInfo);

	CDlgDogDataTitlePtr pData1 = std::dynamic_pointer_cast<CDlgDogDataTitle, CListData>(item1);
	CDlgDogDataTitlePtr pData2 = std::dynamic_pointer_cast<CDlgDogDataTitle, CListData>(item2);
	ARBDogTitlePtr pTitle1 = pData1->GetData();
	ARBDogTitlePtr pTitle2 = pData2->GetData();
	int rc = 0;
	for (int i = 0; i < pInfo->pCols->GetSize(); ++i)
	{
		int col = pInfo->pCols->GetColumnAt(i);
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
				std::wstring n1 = pTitle1->GetGenericName();
				std::wstring n2 = pTitle2->GetGenericName();
				if (n1 < n2)
					rc = -1;
				else if (n1 > n2)
					rc = 1;
			}
			break;
		case 4: // nice name
			{
				std::wstring name1 = pInfo->pDoc->Book().GetConfig().GetTitleNiceName(pTitle1->GetVenue(), pTitle1->GetRawName());
				std::wstring name2 = pInfo->pDoc->Book().GetConfig().GetTitleNiceName(pTitle2->GetVenue(), pTitle2->GetRawName());
				if (name1 < name2)
					rc = -1;
				else if (name1 > name2)
					rc = 1;
			}
			break;
		}
		if (rc)
		{
			if (pInfo->pCols->IsDescending(col))
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
			ARBDogRegNumPtr const& inRegnum)
		: m_pDlg(pDlg)
		, m_RegNum(inRegnum)
	{
	}
	virtual std::wstring OnNeedText(long iCol) const;
	ARBDogRegNumPtr GetData() const	{return m_RegNum;}
private:
	CDlgDog* m_pDlg;
	ARBDogRegNumPtr m_RegNum;
};


std::wstring CDlgDogDataRegNum::OnNeedText(long iCol) const
{
	std::wstring text;
	switch (iCol)
	{
	case 0:
		text = m_RegNum->GetVenue();
		break;
	case 1:
		text = m_RegNum->GetNumber();
		break;
	case 2:
		text = m_RegNum->GetHeight();
		break;
	case 3:
		text = m_RegNum->GetReceived() ? L"x" : L"";
		break;
	case 4:
		text = StringUtil::Replace(m_RegNum->GetNote(), L"\n", L" ");
		break;
	}
	return text;
}


int wxCALLBACK CompareRegNums(CListDataPtr const& item1, CListDataPtr const& item2, SortInfo const* pSortInfo)
{
	DogSortInfo const* pInfo = dynamic_cast<DogSortInfo const*>(pSortInfo);
	assert(pInfo);

	CDlgDogDataRegNumPtr pData1 = std::dynamic_pointer_cast<CDlgDogDataRegNum, CListData>(item1);
	CDlgDogDataRegNumPtr pData2 = std::dynamic_pointer_cast<CDlgDogDataRegNum, CListData>(item2);
	ARBDogRegNumPtr pRegNum1 = pData1->GetData();
	ARBDogRegNumPtr pRegNum2 = pData2->GetData();
	int rc = 0;
	for (int i = 0; i < pInfo->pCols->GetSize(); ++i)
	{
		int col = pInfo->pCols->GetColumnAt(i);
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
			if (pInfo->pCols->IsDescending(col))
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
			ARBDogExistingPointsPtr const& pts)
		: m_pDlg(pDlg)
		, m_Pts(pts)
	{
	}
	virtual std::wstring OnNeedText(long iCol) const;
	ARBDogExistingPointsPtr GetData() const	{return m_Pts;}
private:
	CDlgDog* m_pDlg;
	ARBDogExistingPointsPtr m_Pts;
};


std::wstring CDlgDogDataPoint::OnNeedText(long iCol) const
{
	std::wstring text;
	switch (iCol)
	{
	case 0:
		text = m_Pts->GetDate().GetString();
		break;
	case 1: // Type
		text = ARBDogExistingPoints::GetPointTypeName(m_Pts->GetType());
		break;
	case 2: // Points
		text = fmt::format(L"{}", m_Pts->GetPoints());
		break;
	case 3: // Other Points
		text = m_Pts->GetTypeName();
		break;
	case 4: // Venue
		text = m_Pts->GetVenue();
		break;
	case 5: // MultiQ
		text = m_Pts->GetMultiQ();
		break;
	case 6: // Division
		text = m_Pts->GetDivision();
		break;
	case 7: // Level
		text = m_Pts->GetLevel();
		break;
	case 8: // Event
		text = m_Pts->GetEvent();
		break;
	case 9: // SubName
		text = m_Pts->GetSubName();
		break;
	case 10: // Comment
		text = StringUtil::Replace(m_Pts->GetComment(), L"\n", L" ");
		break;
	}
	return text;
}


int wxCALLBACK ComparePoints(CListDataPtr const& item1, CListDataPtr const& item2, SortInfo const* pSortInfo)
{
	DogSortInfo const* pInfo = dynamic_cast<DogSortInfo const*>(pSortInfo);
	assert(pInfo);

	CDlgDogDataPointPtr pData1 = std::dynamic_pointer_cast<CDlgDogDataPoint, CListData>(item1);
	CDlgDogDataPointPtr pData2 = std::dynamic_pointer_cast<CDlgDogDataPoint, CListData>(item2);
	ARBDogExistingPointsPtr pExistingPoints1 = pData1->GetData();
	ARBDogExistingPointsPtr pExistingPoints2 = pData2->GetData();
	int rc = 0;
	for (int i = 0; i < pInfo->pCols->GetSize(); ++i)
	{
		int col = pInfo->pCols->GetColumnAt(i);
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
			if (pExistingPoints1->GetTypeName() < pExistingPoints2->GetTypeName())
				rc = -1;
			else if (pExistingPoints1->GetTypeName() > pExistingPoints2->GetTypeName())
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
			if (pInfo->pCols->IsDescending(col))
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
		ARBDogPtr const& inDog,
		wxWindow* pParent,
		int iSelectPage)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_pDog(inDog)
	, m_viewHidden(CAgilityBookOptions::GetViewHiddenTitles())
	, m_imgTitlesEmpty(-1)
	, m_imgTitlesTitled(-1)
	, m_imgTitlesTitledReceived(-1)
	, m_imgTitlesHidden(-1)
	, m_imgTitlesTitledHidden(-1)
	, m_imgTitlesTitledHiddenReceived(-1)
	, m_CallName(StringUtil::stringWX(inDog->GetCallName()))
	, m_Breed(StringUtil::stringWX(inDog->GetBreed()))
	, m_RegName(StringUtil::stringWX(inDog->GetRegisteredName()))
	, m_DOB(inDog->GetDOB())
	, m_IsDeceased(inDog->GetDeceased().IsValid())
	, m_Deceased(inDog->GetDeceased())
	, m_Notes(StringUtil::stringWX(inDog->GetNote()))
	, m_ctrlAge(nullptr)
	, m_ctrlDDay(nullptr)
	, m_ctrlTitles(nullptr)
	, m_ctrlTitleEdit(nullptr)
	, m_ctrlTitleDelete(nullptr)
	, m_ctrlRegNums(nullptr)
	, m_ctrlRegEdit(nullptr)
	, m_ctrlRegDelete(nullptr)
	, m_ctrlPoints(nullptr)
	, m_ctrlPointsEdit(nullptr)
	, m_ctrlPointsDelete(nullptr)
	, m_ctrlSelectedPts(nullptr)
	, m_Titles()
	, m_sortTitles(L"Titles")
	, m_ViewHiddenTitles(CAgilityBookOptions::GetViewHiddenTitles())
	, m_sortPoints(L"ExistingPoints")
	, m_RegNums()
	, m_sortRegNums(L"RegNums")
	, m_ExistingPoints()
{
	SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY | GetExtraStyle());
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDS_COL_DOG"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	if (!m_Deceased.IsValid())
		m_Deceased.SetToday();
	m_pDog->GetTitles().Clone(m_Titles);
	m_sortTitles.Initialize(nColTitleInfo);
	inDog->GetRegNums().Clone(m_RegNums);
	m_sortRegNums.Initialize(nColRegNumInfo);
	inDog->GetExistingPoints().Clone(m_ExistingPoints);
	m_sortPoints.Initialize(nColExistingPointsInfo);

	// Controls (these are done first to control tab order)

	wxNotebook* notebook = new wxNotebook(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(notebook, wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, wxNotebookEventHandler, CDlgDog::OnPageChanging);

	// Properties

	wxPanel* panelProp = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText* textCallName = new wxStaticText(panelProp, wxID_ANY,
		_("IDC_DOG_CALLNAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textCallName->Wrap(-1);

	CTextCtrl* ctrlName = new CTextCtrl(panelProp, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_CallName, TRIMVALIDATOR_DEFAULT, _("IDS_BLANK_CALLNAME")));
	ctrlName->SetHelpText(_("HIDC_DOG_CALLNAME"));
	ctrlName->SetToolTip(_("HIDC_DOG_CALLNAME"));

	wxStaticText* textBreed = new wxStaticText(panelProp, wxID_ANY,
		_("IDC_DOG_BREED"),
		wxDefaultPosition, wxDefaultSize, 0);
	textBreed->Wrap(-1);

	CTextCtrl* ctrlBreed = new CTextCtrl(panelProp, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_Breed, TRIMVALIDATOR_TRIM_BOTH));
	ctrlBreed->SetHelpText(_("HIDC_DOG_BREED"));
	ctrlBreed->SetToolTip(_("HIDC_DOG_BREED"));

	wxStaticText* textRegName = new wxStaticText(panelProp, wxID_ANY,
		_("IDC_DOG_REG_NAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textRegName->Wrap(-1);

	CTextCtrl* ctrlRegName = new CTextCtrl(panelProp, wxID_ANY, wxEmptyString,
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
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 20), -1), wxALIGN_CENTRE | wxST_NO_AUTORESIZE | wxSUNKEN_BORDER);
	m_ctrlAge->Wrap(-1);

	wxStaticText* textBDay = new wxStaticText(panelProp, wxID_ANY,
		_("IDC_DOG_DOB"),
		wxDefaultPosition, wxDefaultSize, 0);
	textBDay->Wrap(-1);

	wxDatePickerCtrl* ctrlBDay = new wxDatePickerCtrl(panelProp, wxID_ANY,
		wxDefaultDateTime, wxDefaultPosition, wxDefaultSize,
		wxDP_DROPDOWN|wxDP_SHOWCENTURY,
		CGenericValidator(&m_DOB));
	BIND_OR_CONNECT_CTRL(ctrlBDay, wxEVT_DATE_CHANGED, wxDateEventHandler, CDlgDog::OnDateChanged);
	ctrlBDay->SetHelpText(_("HIDC_DOG_DOB"));
	ctrlBDay->SetToolTip(_("HIDC_DOG_DOB"));

	wxCheckBox* ctrlDeceased = new wxCheckBox(panelProp, wxID_ANY,
		_("IDC_DOG_IS_DECEASED"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_IsDeceased));
	BIND_OR_CONNECT_CTRL(ctrlDeceased, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler, CDlgDog::OnDeceased);
	ctrlDeceased->SetHelpText(_("HIDC_DOG_IS_DECEASED"));
	ctrlDeceased->SetToolTip(_("HIDC_DOG_IS_DECEASED"));

	m_ctrlDDay = new wxDatePickerCtrl(panelProp, wxID_ANY, wxDefaultDateTime,
		wxDefaultPosition, wxDefaultSize,
		wxDP_DROPDOWN|wxDP_SHOWCENTURY,
		CGenericValidator(&m_Deceased));
	BIND_OR_CONNECT_CTRL(m_ctrlDDay, wxEVT_DATE_CHANGED, wxDateEventHandler, CDlgDog::OnDateChanged);
	m_ctrlDDay->SetHelpText(_("HIDC_DOG_DECEASED"));
	m_ctrlDDay->SetToolTip(_("HIDC_DOG_DECEASED"));

	wxStaticText* textNotes = new wxStaticText(panelProp, wxID_ANY,
		_("IDC_DOG_NOTES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textNotes->Wrap(-1);

	CSpellCheckCtrl* ctrlNotes = new CSpellCheckCtrl(panelProp, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(-1, wxDLG_UNIT_Y(this, 80)), wxTE_MULTILINE | wxTE_WORDWRAP,
		CTrimValidator(&m_Notes, TRIMVALIDATOR_TRIM_BOTH));
	ctrlNotes->SetHelpText(_("HIDC_DOG_NOTES"));
	ctrlNotes->SetToolTip(_("HIDC_DOG_NOTES"));

	// Titles

	wxPanel* panelTitles = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	m_ctrlTitles = new CReportListCtrl(panelTitles, wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 280), -1), true, CReportListCtrl::eSortHeader, true);
	BIND_OR_CONNECT_CTRL(m_ctrlTitles, wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler, CDlgDog::OnTitleColumnClick);
	BIND_OR_CONNECT_CTRL(m_ctrlTitles, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler, CDlgDog::OnTitleItemSelected);
	BIND_OR_CONNECT_CTRL(m_ctrlTitles, wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler, CDlgDog::OnTitleItemActivated);
	BIND_OR_CONNECT_CTRL(m_ctrlTitles, wxEVT_KEY_DOWN, wxKeyEventHandler, CDlgDog::OnTitleKeyDown);
	m_ctrlTitles->SetHelpText(_("HIDC_DOG_TITLE_TITLES"));
	m_ctrlTitles->SetToolTip(_("HIDC_DOG_TITLE_TITLES"));
	m_imgTitlesEmpty = m_ctrlTitles->AddIcon(CImageManager::Get()->GetIcon(ImageMgrBlank));
	m_imgTitlesTitled = m_ctrlTitles->AddIcon(CImageManager::Get()->GetIcon(ImageMgrTitleTitled));
	m_imgTitlesTitledReceived = m_ctrlTitles->AddIcon(CImageManager::Get()->GetIcon(ImageMgrTitleTitledHave));
	m_imgTitlesHidden = m_ctrlTitles->AddIcon(CImageManager::Get()->GetIcon(ImageMgrTitleHidden));
	m_imgTitlesTitledHidden = m_ctrlTitles->AddIcon(CImageManager::Get()->GetIcon(ImageMgrTitleTitledHidden));
	m_imgTitlesTitledHiddenReceived = m_ctrlTitles->AddIcon(CImageManager::Get()->GetIcon(ImageMgrTitleTitledHiddenHave));
	int i;
	for (i = 0; i < nColTitleInfo; ++i)
	{
		m_ctrlTitles->InsertColumn(i, wxEmptyString, colTitleInfo[i].fmt, wxDLG_UNIT_X(this, colTitleInfo[i].cx));
	}
	SetColumnTitleHeaders();

	wxButton* btnTitleNew = new wxButton(panelTitles, wxID_ANY,
		_("IDC_DOG_TITLE_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(btnTitleNew, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgDog::OnTitleNew);
	btnTitleNew->SetHelpText(_("HIDC_DOG_TITLE_NEW"));
	btnTitleNew->SetToolTip(_("HIDC_DOG_TITLE_NEW"));

	m_ctrlTitleEdit = new wxButton(panelTitles, wxID_ANY,
		_("IDC_DOG_TITLE_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlTitleEdit, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgDog::OnTitleEdit);
	m_ctrlTitleEdit->SetHelpText(_("HIDC_DOG_TITLE_EDIT"));
	m_ctrlTitleEdit->SetToolTip(_("HIDC_DOG_TITLE_EDIT"));

	m_ctrlTitleDelete = new wxButton(panelTitles, wxID_ANY,
		_("IDC_DOG_TITLE_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlTitleDelete, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgDog::OnTitleDelete);
	m_ctrlTitleDelete->SetHelpText(_("HIDC_DOG_TITLE_DELETE"));
	m_ctrlTitleDelete->SetToolTip(_("HIDC_DOG_TITLE_DELETE"));

	wxCheckBox* ctrlHidden = new wxCheckBox(panelTitles, wxID_ANY,
		_("IDC_DOG_TITLE_HIDDEN"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_ViewHiddenTitles));
	BIND_OR_CONNECT_CTRL(ctrlHidden, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler, CDlgDog::OnTitleHidden);
	ctrlHidden->SetHelpText(_("HIDC_DOG_TITLE_HIDDEN"));
	ctrlHidden->SetToolTip(_("HIDC_DOG_TITLE_HIDDEN"));

	// RegNums

	wxPanel* panelRegNums = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	m_ctrlRegNums = new CReportListCtrl(panelRegNums, wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 280), -1), true, CReportListCtrl::eSortHeader, true);
	BIND_OR_CONNECT_CTRL(m_ctrlRegNums, wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler, CDlgDog::OnRegNumColumnClick);
	BIND_OR_CONNECT_CTRL(m_ctrlRegNums, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler, CDlgDog::OnRegNumItemSelected);
	BIND_OR_CONNECT_CTRL(m_ctrlRegNums, wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler, CDlgDog::OnRegNumItemActivated);
	BIND_OR_CONNECT_CTRL(m_ctrlRegNums, wxEVT_KEY_DOWN, wxKeyEventHandler, CDlgDog::OnRegNumKeyDown);
	m_ctrlRegNums->SetHelpText(_("HIDC_DOGNUM_REG_NUMS"));
	m_ctrlRegNums->SetToolTip(_("HIDC_DOGNUM_REG_NUMS"));
	for (i = 0; i < nColRegNumInfo; ++i)
	{
		m_ctrlRegNums->InsertColumn(i, wxEmptyString, colRegNumInfo[i].fmt, wxDLG_UNIT_X(this, colRegNumInfo[i].cx));
	}
	SetColumnRegNumHeaders();

	wxButton* btnRegNew = new wxButton(panelRegNums, wxID_ANY,
		_("IDC_DOGNUM_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(btnRegNew, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgDog::OnRegNumNew);
	btnRegNew->SetHelpText(_("HIDC_DOGNUM_NEW"));
	btnRegNew->SetToolTip(_("HIDC_DOGNUM_NEW"));

	m_ctrlRegEdit = new wxButton(panelRegNums, wxID_ANY,
		_("IDC_DOGNUM_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlRegEdit, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgDog::OnRegNumEdit);
	m_ctrlRegEdit->SetHelpText(_("HIDC_DOGNUM_EDIT"));
	m_ctrlRegEdit->SetToolTip(_("HIDC_DOGNUM_EDIT"));

	m_ctrlRegDelete = new wxButton(panelRegNums, wxID_ANY,
		_("IDC_DOGNUM_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlRegDelete, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgDog::OnRegNumDelete);
	m_ctrlRegDelete->SetHelpText(_("HIDC_DOGNUM_DELETE"));
	m_ctrlRegDelete->SetToolTip(_("HIDC_DOGNUM_DELETE"));

	// Points

	wxPanel* panelPoints = new wxPanel(notebook, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	m_ctrlPoints = new CReportListCtrl(panelPoints, wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 280), -1), false, CReportListCtrl::eSortHeader, true);
	BIND_OR_CONNECT_CTRL(m_ctrlPoints, wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler, CDlgDog::OnPointsColumnClick);
	BIND_OR_CONNECT_CTRL(m_ctrlPoints, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler, CDlgDog::OnPointsItemSelected);
	BIND_OR_CONNECT_CTRL(m_ctrlPoints, wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler, CDlgDog::OnPointsItemActivated);
	BIND_OR_CONNECT_CTRL(m_ctrlPoints, wxEVT_KEY_DOWN, wxKeyEventHandler, CDlgDog::OnPointsKeyDown);
	m_ctrlPoints->SetHelpText(_("HIDC_DOGPTS_POINTS"));
	m_ctrlPoints->SetToolTip(_("HIDC_DOGPTS_POINTS"));
	for (i = 0; i < nColExistingPointsInfo; ++i)
	{
		m_ctrlPoints->InsertColumn(i, wxEmptyString, colExistingPointsInfo[i].fmt, wxDLG_UNIT_X(this, colExistingPointsInfo[i].cx));
	}
	SetColumnPointsHeaders();

	wxButton* btnPtsNew = new wxButton(panelPoints, wxID_ANY,
		_("IDC_DOGPTS_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(btnPtsNew, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgDog::OnPointsNew);
	btnPtsNew->SetHelpText(_("HIDC_DOGPTS_NEW"));
	btnPtsNew->SetToolTip(_("HIDC_DOGPTS_NEW"));

	m_ctrlPointsEdit = new wxButton(panelPoints, wxID_ANY,
		_("IDC_DOGPTS_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlPointsEdit, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgDog::OnPointsEdit);
	m_ctrlPointsEdit->SetHelpText(_("HIDC_DOGPTS_EDIT"));
	m_ctrlPointsEdit->SetToolTip(_("HIDC_DOGPTS_EDIT"));

	m_ctrlPointsDelete = new wxButton(panelPoints, wxID_ANY,
		_("IDC_DOGPTS_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlPointsDelete, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgDog::OnPointsDelete);
	m_ctrlPointsDelete->SetHelpText(_("HIDC_DOGPTS_DELETE"));
	m_ctrlPointsDelete->SetToolTip(_("HIDC_DOGPTS_DELETE"));

	wxStaticText* textPts = new wxStaticText(panelPoints, wxID_ANY,
		_("IDC_DOGPTS_SELECTED_PTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPts->Wrap(-1);

	m_ctrlSelectedPts = new wxStaticText(panelPoints, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(wxDLG_UNIT_X(this, 30), -1), wxALIGN_CENTRE | wxST_NO_AUTORESIZE | wxSUNKEN_BORDER);
	m_ctrlSelectedPts->Wrap(-1);

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerProp = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textCallName, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerName->Add(ctrlName, 2, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerName->Add(textBreed, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerName->Add(ctrlBreed, 3, wxALIGN_CENTER_VERTICAL, 0);

	sizerProp->Add(sizerName, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerRegName = new wxBoxSizer(wxHORIZONTAL);
	sizerRegName->Add(textRegName, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerRegName->Add(ctrlRegName, 1, wxALIGN_CENTER_VERTICAL, 0);

	sizerProp->Add(sizerRegName, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerAge = new wxBoxSizer(wxHORIZONTAL);
	sizerAge->Add(textAge, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerAge->Add(m_ctrlAge, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerAge->Add(textBDay, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerAge->Add(ctrlBDay, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerAge->Add(ctrlDeceased, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerAge->Add(m_ctrlDDay, 0, wxALIGN_CENTER_VERTICAL, 0);

	sizerProp->Add(sizerAge, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerProp->Add(textNotes, 0, wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));
	sizerProp->Add(ctrlNotes, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, wxDLG_UNIT_X(this, 5));

	panelProp->SetSizer(sizerProp);
	panelProp->Layout();
	sizerProp->Fit(panelProp);
	notebook->AddPage(panelProp, _("IDD_DOG_PROP"), true);

	wxBoxSizer* sizerTitles = new wxBoxSizer(wxVERTICAL);
	sizerTitles->Add(m_ctrlTitles, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerTitleBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerTitleBtns->Add(btnTitleNew, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerTitleBtns->Add(m_ctrlTitleEdit, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerTitleBtns->Add(m_ctrlTitleDelete, 0, wxRIGHT, wxDLG_UNIT_X(this, 3)); 
	sizerTitleBtns->Add(0, 0, 1, wxEXPAND, 0);
	sizerTitleBtns->Add(ctrlHidden, 0, wxALIGN_CENTER_VERTICAL, 0);

	sizerTitles->Add(sizerTitleBtns, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	panelTitles->SetSizer(sizerTitles);
	panelTitles->Layout();
	sizerTitles->Fit(panelTitles);
	notebook->AddPage(panelTitles, _("IDD_DOG_TITLES"), false);

	wxBoxSizer* sizerReg = new wxBoxSizer(wxVERTICAL);
	sizerReg->Add(m_ctrlRegNums, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerRegBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerRegBtns->Add(btnRegNew, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerRegBtns->Add(m_ctrlRegEdit, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerRegBtns->Add(m_ctrlRegDelete, 0, 0, 0);

	sizerReg->Add(sizerRegBtns, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	panelRegNums->SetSizer(sizerReg);
	panelRegNums->Layout();
	sizerReg->Fit(panelRegNums);
	notebook->AddPage(panelRegNums, _("IDD_DOG_NUMBERS"), false);

	wxBoxSizer* sizerPts = new wxBoxSizer(wxVERTICAL);
	sizerPts->Add(m_ctrlPoints, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerPtsBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerPtsBtns->Add(btnPtsNew, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerPtsBtns->Add(m_ctrlPointsEdit, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerPtsBtns->Add(m_ctrlPointsDelete, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerPtsBtns->Add(0, 0, 1, wxEXPAND, 0);
	sizerPtsBtns->Add(textPts, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerPtsBtns->Add(m_ctrlSelectedPts, 0, wxALIGN_CENTER_VERTICAL, 0);

	sizerPts->Add(sizerPtsBtns, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	panelPoints->SetSizer(sizerPts);
	panelPoints->Layout();
	sizerPts->Fit(panelPoints);
	notebook->AddPage(panelPoints, _("IDD_DOG_POINTS"), false);

	bSizer->Add(notebook, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	UpdateDeceased();
	ListTitles();
	ListRegNums();
	ListExistingPoints();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	std::vector<wxWindow*> ctrls;
	ctrls.push_back(ctrlName);
	ctrls.push_back(m_ctrlTitles);
	ctrls.push_back(m_ctrlRegNums);
	ctrls.push_back(m_ctrlPoints);
	IMPLEMENT_ON_INIT(CDlgDog, ctrls[iSelectPage])

	notebook->ChangeSelection(iSelectPage);
}


DEFINE_ON_INIT(CDlgDog)


CDlgDog::~CDlgDog()
{
}


void CDlgDog::OnPageChanging(wxBookCtrlEvent& evt)
{
	if (wxNOT_FOUND != evt.GetOldSelection())
	{
		if (!Validate() || !TransferDataFromWindow())
		{
			evt.Veto();
			return;
		}
	}
	evt.Skip();
}


CDlgDogDataTitlePtr CDlgDog::GetTitleData(long index) const
{
	return std::dynamic_pointer_cast<CDlgDogDataTitle, CListData>(m_ctrlTitles->GetData(index));
}


CDlgDogDataRegNumPtr CDlgDog::GetRegNumData(long index) const
{
	return std::dynamic_pointer_cast<CDlgDogDataRegNum, CListData>(m_ctrlRegNums->GetData(index));
}


CDlgDogDataPointPtr CDlgDog::GetPointData(long index) const
{
	return std::dynamic_pointer_cast<CDlgDogDataPoint, CListData>(m_ctrlPoints->GetData(index));
}


void CDlgDog::UpdateAge()
{
	long ageDays = 0;
	if (m_DOB.IsValid())
	{
		ARBDate current(ARBDate::Today());
		if (m_IsDeceased)
			current = m_Deceased;
		ageDays = current - m_DOB;
	}
	m_ctrlAge->SetLabel(StringUtil::stringWX(ARBDouble::ToString(ageDays/365.0, 1)));
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
		std::wstring tmp;
		if (colTitleInfo[i].idText)
			tmp = fmt::format(L"{} ({})", wxGetTranslation(colTitleInfo[i].idText).wx_str(),  m_sortTitles.FindColumnOrder(i) + 1);
		else
			tmp = fmt::format(L"({})", m_sortTitles.FindColumnOrder(i) + 1);
		wxListItem col;
		col.SetMask(wxLIST_MASK_TEXT);
		col.SetColumn(i);
		col.SetText(tmp);
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
	DogSortInfo sortInfo(this, m_pDoc, &m_sortTitles);
	m_ctrlTitles->SortItems(CompareTitles, &sortInfo);
	if (pSelected)
	{
		for (i = 0; i < m_ctrlTitles->GetItemCount(); ++i)
		{
			CDlgDogDataTitlePtr pTitle = GetTitleData(i);
			if (pTitle && pTitle->GetData() == pSelected) // compare by ptr is fine.
			{
				m_ctrlTitles->Select(i, true);
				m_ctrlTitles->Focus(i);
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
		std::wstring tmp;
		if (colRegNumInfo[i].idText)
			tmp = fmt::format(L"{} ({})", wxGetTranslation(colRegNumInfo[i].idText).wx_str(), m_sortRegNums.FindColumnOrder(i) + 1);
		else
			tmp = fmt::format(L"({})", m_sortRegNums.FindColumnOrder(i) + 1);
		wxListItem col;
		col.SetMask(wxLIST_MASK_TEXT);
		col.SetColumn(i);
		col.SetText(tmp);
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
	DogSortInfo sortInfo(this, m_pDoc, &m_sortRegNums);
	m_ctrlRegNums->SortItems(CompareRegNums, &sortInfo);
	if (pSelected)
	{
		for (i = 0; i < m_ctrlRegNums->GetItemCount(); ++i)
		{
			CDlgDogDataRegNumPtr pData = GetRegNumData(i);
			if (pData && pData->GetData() == pSelected) // compare by ptr is fine.
			{
				m_ctrlRegNums->Select(i, true);
				m_ctrlRegNums->Focus(i);
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
		std::wstring tmp;
		if (colExistingPointsInfo[i].idText)
			tmp = fmt::format(L"{} ({})", wxGetTranslation(colExistingPointsInfo[i].idText).wx_str(), m_sortPoints.FindColumnOrder(i) + 1);
		else
			tmp = fmt::format(L"({})", m_sortPoints.FindColumnOrder(i) + 1);
		wxListItem col;
		col.SetMask(wxLIST_MASK_TEXT);
		col.SetColumn(i);
		col.SetText(tmp);
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
	DogSortInfo sortInfo(this, m_pDoc, &m_sortPoints);
	m_ctrlPoints->SortItems(ComparePoints, &sortInfo);
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
				m_ctrlPoints->Focus(i);
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
	std::wstring str;
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
		str = fmt::format(L"{}", total);
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
	DogSortInfo sortInfo(this, m_pDoc, &m_sortTitles);
	m_ctrlTitles->SortItems(CompareTitles, &sortInfo);
	m_sortTitles.Save();
}


void CDlgDog::OnTitleItemSelected(wxListEvent& evt)
{
	UpdateTitleButtons();
}


void CDlgDog::OnTitleItemActivated(wxListEvent& evt)
{
	EditTitle();
}


void CDlgDog::OnTitleKeyDown(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		evt.Skip();
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		EditTitle();
		break;
	}
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
		ARBConfigVenuePtr venue;
		m_pDoc->Book().GetConfig().GetVenues().FindVenue(pTitle->GetData()->GetVenue(), &venue);
		m_Titles.DeleteTitle(venue, pTitle->GetData());
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
	DogSortInfo sortInfo(this, m_pDoc, &m_sortRegNums);
	m_ctrlRegNums->SortItems(CompareRegNums, &sortInfo);
	m_sortRegNums.Save();
}


void CDlgDog::OnRegNumItemSelected(wxListEvent& evt)
{
	UpdateRegNumButtons();
}


void CDlgDog::OnRegNumItemActivated(wxListEvent& evt)
{
	EditRegNum();
}


void CDlgDog::OnRegNumKeyDown(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		evt.Skip();
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		EditRegNum();
		break;
	}
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
	DogSortInfo sortInfo(this, m_pDoc, &m_sortPoints);
	m_ctrlPoints->SortItems(ComparePoints, &sortInfo);
	m_sortPoints.Save();
}


void CDlgDog::OnPointsItemSelected(wxListEvent& evt)
{
	UpdatePointsButtons();
}


void CDlgDog::OnPointsItemActivated(wxListEvent& evt)
{
	EditPoints();
}


void CDlgDog::OnPointsKeyDown(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		evt.Skip();
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		EditPoints();
		break;
	}
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
	if (!m_IsDeceased)
		m_Deceased.clear();

	bool refreshTitles = false;
	bool bModified = false;
	unsigned int hint = 0;

	if (m_pDog->GetCallName() != StringUtil::stringW(m_CallName))
	{
		std::wstring newCallName = StringUtil::stringW(m_CallName);
		CAgilityBookOptions::CleanLastItems(m_pDog->GetCallName(), newCallName);
		hint |= UPDATE_TREE_VIEW | UPDATE_RUNS_VIEW | UPDATE_POINTS_VIEW;
		m_pDog->SetCallName(newCallName);
	}
	if (m_pDog->GetDOB() != m_DOB)
	{
		// All we really need to do here is to refresh the tree item. But
		// that may not be the current one right now, so use the big hammer.
		hint |= UPDATE_TREE_VIEW;
		m_pDog->SetDOB(m_DOB);
	}
	if (m_pDog->GetDeceased() != m_Deceased)
	{
		// big hammer
		hint |= UPDATE_TREE_VIEW;
		m_pDog->SetDeceased(m_Deceased);
	}
	if (m_pDog->GetBreed() != StringUtil::stringW(m_Breed))
	{
		bModified = true;
		m_pDog->SetBreed(StringUtil::stringW(m_Breed));
	}
	if (m_pDog->GetRegisteredName() != StringUtil::stringW(m_RegName))
	{
		hint |= UPDATE_POINTS_VIEW;
		m_pDog->SetRegisteredName(StringUtil::stringW(m_RegName));
	}
	if (m_pDog->GetNote() != StringUtil::stringW(m_Notes))
	{
		bModified = true;
		m_pDog->SetNote(StringUtil::stringW(m_Notes));
	}
	if (m_pDog->GetTitles() != m_Titles)
	{
		hint |= UPDATE_POINTS_VIEW;
		refreshTitles = true;
		m_pDog->GetTitles() = m_Titles;
	}
	if (m_pDog->GetRegNums() != m_RegNums)
	{
		hint |= UPDATE_POINTS_VIEW;
		m_pDog->GetRegNums() = m_RegNums;
	}
	if (m_pDog->GetExistingPoints() != m_ExistingPoints)
	{
		hint |= UPDATE_POINTS_VIEW;
		m_pDog->GetExistingPoints() = m_ExistingPoints;
	}
	if (hint)
		bModified = true;

	if (refreshTitles || m_viewHidden != CAgilityBookOptions::GetViewHiddenTitles())
	{
		std::vector<CVenueFilter> venues;
		CFilterOptions::Options().GetFilterVenue(venues);
		for (ARBDogList::iterator iterDogs = m_pDoc->Book().GetDogs().begin(); iterDogs != m_pDoc->Book().GetDogs().end(); ++iterDogs)
			for (ARBDogTitleList::iterator iterTitle = (*iterDogs)->GetTitles().begin(); iterTitle != (*iterDogs)->GetTitles().end(); ++iterTitle)
			{
				if (m_pDoc->ResetVisibility(venues, *iterTitle))
					hint |= UPDATE_POINTS_VIEW;
			}
	}

	if (bModified)
		m_pDoc->Modify(true);

	if (hint)
	{
		CUpdateHint updateHint(hint);
		m_pDoc->UpdateAllViews(nullptr, &updateHint);
	}

	EndDialog(wxID_OK);
}
