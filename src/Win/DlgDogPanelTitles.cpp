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
 * 2021-07-20 Fix deletion of multiple selection.
 * 2021-01-07 Separated from DlgDog.cpp
 * 2013-05-18 Modifying titles caused loss of view filtering.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-01-10 Allow titles to be optionally entered multiple times.
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-02-03 Broke dialog up into pages.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-08-18 Added a deceased date for a dog.
 */

#include "stdafx.h"
#include "DlgDogPanelTitles.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "DlgTitle.h"
#include "ImageHelper.h"

#include "ARB/ARBDog.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ImageHelperBase.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/ReportListCtrl.h"
#include "LibARBWin/ReportListHeader.h"
#include "LibARBWin/ResourceManager.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

namespace
{
constexpr int k_colReceived = 0;
constexpr int k_colDate = 1;
constexpr int k_colVenue = 2;
constexpr int k_colTitle = 3;
constexpr int k_colName = 4;
static const std::vector<CReportListHeader::ColumnInfo> k_columnsTitleInfo{
	{k_colReceived, wxLIST_FORMAT_LEFT, L""},
	{k_colDate, wxLIST_FORMAT_LEFT, arbT("IDS_COL_DATE")},
	{k_colVenue, wxLIST_FORMAT_LEFT, arbT("IDS_COL_VENUE")},
	{k_colTitle, wxLIST_FORMAT_LEFT, arbT("IDS_COL_TITLE")},
	{k_colName, wxLIST_FORMAT_LEFT, arbT("IDS_COL_NAME")},
};
const std::vector<int> k_columnsWidth{5, 25, 25, 25, 25};
} // namespace

/////////////////////////////////////////////////////////////////////////////

class CDlgDogDataTitle : public CListData
{
public:
	CDlgDogDataTitle(CDlgDogPanelTitles* pDlg, ARBDogTitlePtr const& inTitle)
		: m_pDlg(pDlg)
		, m_Title(inTitle)
	{
	}
	int OnCompare(CListDataPtr const& item, long iCol) const override;
	std::wstring OnNeedText(long iCol) const override;
	void OnNeedListItem(long iCol, wxListItem& info) const override;
	ARBDogTitlePtr GetData() const
	{
		return m_Title;
	}

private:
	CDlgDogPanelTitles* m_pDlg;
	ARBDogTitlePtr m_Title;
};


int CDlgDogDataTitle::OnCompare(CListDataPtr const& item, long iCol) const
{
	CDlgDogDataTitlePtr pData2 = std::dynamic_pointer_cast<CDlgDogDataTitle, CListData>(item);
	if (!pData2)
		return 1;

	ARBDogTitlePtr pTitle1 = GetData();
	ARBDogTitlePtr pTitle2 = pData2->GetData();
	int rc = 0;
	switch (iCol)
	{
	case k_colReceived: // Received and hidden
		if (!pTitle1->GetReceived() && pTitle2->GetReceived())
			rc = -1;
		else if (pTitle1->GetReceived() && !pTitle2->GetReceived())
			rc = 1;
		else if (!pTitle1->IsHidden() && pTitle2->IsHidden())
			rc = -1;
		else if (pTitle1->IsHidden() && !pTitle2->IsHidden())
			rc = 1;
		break;
	case k_colDate:
		if (pTitle1->GetDate() < pTitle2->GetDate())
			rc = -1;
		else if (pTitle1->GetDate() > pTitle2->GetDate())
			rc = 1;
		break;
	case k_colVenue:
		if (pTitle1->GetVenue() < pTitle2->GetVenue())
			rc = -1;
		else if (pTitle1->GetVenue() > pTitle2->GetVenue())
			rc = 1;
		break;
	case k_colTitle:
	{
		std::wstring n1 = pTitle1->GetGenericName();
		std::wstring n2 = pTitle2->GetGenericName();
		if (n1 < n2)
			rc = -1;
		else if (n1 > n2)
			rc = 1;
	}
	break;
	case k_colName:
	{
		std::wstring name1
			= m_pDlg->m_pDoc->Book().GetConfig().GetTitleNiceName(pTitle1->GetVenue(), pTitle1->GetRawName());
		std::wstring name2
			= m_pDlg->m_pDoc->Book().GetConfig().GetTitleNiceName(pTitle2->GetVenue(), pTitle2->GetRawName());
		if (name1 < name2)
			rc = -1;
		else if (name1 > name2)
			rc = 1;
	}
	break;
	default:
		break;
	}
	return rc;
}


std::wstring CDlgDogDataTitle::OnNeedText(long iCol) const
{
	std::wstring text;
	switch (iCol)
	{
	case k_colDate:
		if (m_Title->GetDate().IsValid())
			text = m_Title->GetDate().GetString();
		else
			text = _("IDS_UNEARNED");
		break;
	case k_colVenue:
		text = m_Title->GetVenue();
		break;
	case k_colTitle:
		text = m_Title->GetGenericName();
		break;
	case k_colName:
		text = m_pDlg->m_pDoc->Book().GetConfig().GetTitleNiceName(m_Title->GetVenue(), m_Title->GetRawName());
		break;
	default:
		break;
	}
	return text;
}


void CDlgDogDataTitle::OnNeedListItem(long iCol, wxListItem& info) const
{
	switch (iCol)
	{
	case k_colReceived:
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

/////////////////////////////////////////////////////////////////////////////

CDlgDogPanelTitles::CDlgDogPanelTitles(CAgilityBookDoc* pDoc, ARBDogPtr const& inDog, wxWindow* parent)
	: CDlgDogPanelReportBase(pDoc, inDog, parent)
	, m_imgTitlesEmpty(-1)
	, m_imgTitlesTitled(-1)
	, m_imgTitlesTitledReceived(-1)
	, m_imgTitlesHidden(-1)
	, m_imgTitlesTitledHidden(-1)
	, m_imgTitlesTitledHiddenReceived(-1)
	, m_Titles()
	, m_ViewHiddenTitles(CAgilityBookOptions::GetViewHiddenTitles())
{
	m_pDog->GetTitles().Clone(m_Titles);

	DoCreate(false, true);
	m_ctrlList->SetHelpText(_("HIDC_DOG_TITLE_TITLES"));
	m_ctrlList->SetToolTip(_("HIDC_DOG_TITLE_TITLES"));

	m_imgTitlesEmpty = m_ctrlList->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrBlank));
	m_imgTitlesTitled = m_ctrlList->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrTitleTitled));
	m_imgTitlesTitledReceived = m_ctrlList->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrTitleTitledHave));
	m_imgTitlesHidden = m_ctrlList->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrTitleHidden));
	m_imgTitlesTitledHidden = m_ctrlList->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrTitleTitledHidden));
	m_imgTitlesTitledHiddenReceived
		= m_ctrlList->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrTitleTitledHiddenHave));

	m_btnNew->SetLabel(_("IDC_DOG_TITLE_NEW"));
	m_btnNew->SetHelpText(_("HIDC_DOG_TITLE_NEW"));
	m_btnNew->SetToolTip(_("HIDC_DOG_TITLE_NEW"));

	m_btnEdit->SetLabel(_("IDC_DOG_TITLE_EDIT"));
	m_btnEdit->SetHelpText(_("HIDC_DOG_TITLE_EDIT"));
	m_btnEdit->SetToolTip(_("HIDC_DOG_TITLE_EDIT"));

	m_btnDelete->SetLabel(_("IDC_DOG_TITLE_DELETE"));
	m_btnDelete->SetHelpText(_("HIDC_DOG_TITLE_DELETE"));
	m_btnDelete->SetToolTip(_("HIDC_DOG_TITLE_DELETE"));

	wxCheckBox* ctrlHidden = new wxCheckBox(
		this,
		wxID_ANY,
		_("IDC_DOG_TITLE_HIDDEN"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_ViewHiddenTitles));
	ctrlHidden->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgDogPanelTitles::OnTitleHidden, this);
	ctrlHidden->SetHelpText(_("HIDC_DOG_TITLE_HIDDEN"));
	ctrlHidden->SetToolTip(_("HIDC_DOG_TITLE_HIDDEN"));

	// Sizers

	m_sizerButtons->Add(0, 0, 1, wxEXPAND, 0);
	m_sizerButtons->Add(ctrlHidden, 0, wxALIGN_CENTER_VERTICAL, 0);

	Layout();
	GetSizer()->Fit(this);

	UpdateControls();
}


bool CDlgDogPanelTitles::Save(unsigned int& hint)
{
	CDlgDogPanelReportBase::Save(hint);

	bool bModified = false;
	if (m_pDog->GetTitles() != m_Titles)
	{
		bModified = true;
		hint |= UPDATE_POINTS_VIEW;
		m_pDog->GetTitles() = m_Titles;
	}
	return bModified;
}


void CDlgDogPanelTitles::DoInitList()
{
	m_reportColumn.CreateColumns(k_columnsTitleInfo, -1, L"Titles", &k_columnsWidth);
	ListTitles();
}


void CDlgDogPanelTitles::DoNew()
{
	CDlgTitle dlg(m_pDoc->Book().GetConfig(), m_Titles, ARBDogTitlePtr(), this);
	if (wxID_OK == dlg.ShowModal())
		ListTitles();
}


void CDlgDogPanelTitles::DoEdit()
{
	long i = m_ctrlList->GetFirstSelected();
	if (0 <= i)
	{
		CDlgDogDataTitlePtr pTitle = GetTitleData(i);
		CDlgTitle dlg(m_pDoc->Book().GetConfig(), m_Titles, pTitle->GetData(), this);
		if (wxID_OK == dlg.ShowModal())
			ListTitles();
	}
}


void CDlgDogPanelTitles::DoDelete()
{
	std::vector<long> indices;
	if (0 < m_ctrlList->GetSelection(indices))
	{
		for (std::vector<long>::reverse_iterator i = indices.rbegin(); i != indices.rend(); ++i)
		{
			CDlgDogDataTitlePtr pTitle = GetTitleData(*i);
			ARBConfigVenuePtr venue;
			m_pDoc->Book().GetConfig().GetVenues().FindVenue(pTitle->GetData()->GetVenue(), &venue);
			m_Titles.DeleteTitle(venue, pTitle->GetData());
			DeleteListItem(*i);
		}
		UpdateControls();
	}
}


CDlgDogDataTitlePtr CDlgDogPanelTitles::GetTitleData(long index) const
{
	return std::dynamic_pointer_cast<CDlgDogDataTitle, CListData>(m_ctrlList->GetData(index));
}


void CDlgDogPanelTitles::ListTitles()
{
	wxBusyCursor wait;
	ARBDogTitlePtr pSelected;
	int i = m_ctrlList->GetFirstSelected();
	if (0 <= i)
	{
		CDlgDogDataTitlePtr pData = GetTitleData(i);
		if (pData)
			pSelected = pData->GetData();
	}
	m_ctrlList->DeleteAllItems();
	for (ARBDogTitleList::const_iterator iterTitle = m_Titles.begin(); iterTitle != m_Titles.end(); ++iterTitle)
	{
		ARBDogTitlePtr pTitle = (*iterTitle);
		if (!CAgilityBookOptions::GetViewHiddenTitles() && pTitle->IsHidden())
		{
			continue;
		}
		CDlgDogDataTitlePtr data(std::make_shared<CDlgDogDataTitle>(this, pTitle));
		m_ctrlList->InsertItem(data);
	}
	m_reportColumn.SizeColumns();
	m_reportColumn.Sort();

	if (pSelected)
	{
		for (i = 0; i < m_ctrlList->GetItemCount(); ++i)
		{
			CDlgDogDataTitlePtr pTitle = GetTitleData(i);
			if (pTitle && pTitle->GetData() == pSelected) // compare by ptr is fine.
			{
				m_ctrlList->Select(i, true);
				m_ctrlList->Focus(i);
				break;
			}
		}
	}
	UpdateControls();
}


void CDlgDogPanelTitles::OnTitleHidden(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	CAgilityBookOptions::SetViewHiddenTitles(m_ViewHiddenTitles);
	ListTitles();
}

} // namespace dconSoft
