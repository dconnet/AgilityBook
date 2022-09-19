/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgFindLinks class
 * @author David Connet
 *
 * Revision History
 * 2018-12-25 Convert to fmt.
 * 2014-12-31 Changed pixels to dialog units.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2011-08-31 Column headings were wrong.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-08 Fixed data index lookup when editing an item.
 * 2009-02-10 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-06-02 Added 'Open' button.
 * 2004-03-31 Created.
 */

#include "stdafx.h"
#include "DlgFindLinks.h"

#include "AgilityBook.h"
#include "ClipBoard.h"
#include "DlgSelectURL.h"
#include "ImageHelper.h"

#include "ARB/ARBDog.h"
#include "ARB/ARBDogRun.h"
#include "ARB/ARBDogTrial.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/CheckLink.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/ReportListCtrl.h"

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
constexpr wchar_t k_regkeyBase[] = L"Links";
constexpr long k_colLink = 0;
constexpr long k_colDog = 1;
constexpr long k_colTrial = 2;
constexpr long k_colRun = 3;
const std::vector<CReportListHeader::ColumnInfo> k_columns{
	{k_colLink, wxLIST_FORMAT_LEFT, arbT("IDS_COL_NAME")},
	{k_colDog, wxLIST_FORMAT_LEFT, arbT("IDS_COL_DOG")},
	{k_colTrial, wxLIST_FORMAT_LEFT, arbT("IDS_COL_TRIAL")},
	{k_colRun, wxLIST_FORMAT_LEFT, arbT("IDS_COL_EVENT")},
};
} // namespace

class CDlgFindLinksData : public CListData
{
public:
	CDlgFindLinksData(
		ARBDogPtr const& inDog,
		ARBDogTrialPtr const& inTrial,
		ARBDogRunPtr const& inRun,
		std::wstring const& inLink,
		int image)
		: m_pDog(inDog)
		, m_pTrial(inTrial)
		, m_pRun(inRun)
		, m_OldLink(inLink)
		, m_Link(inLink)
		, m_Image(image)
	{
	}
	int OnCompare(CListDataPtr const& item, long iCol) const override;
	std::wstring OnNeedText(long iCol) const override;
	void OnNeedListItem(long iCol, wxListItem& info) const override;

	ARBDogPtr m_pDog;
	ARBDogTrialPtr m_pTrial;
	ARBDogRunPtr m_pRun;
	std::wstring m_OldLink;
	std::wstring m_Link;
	int m_Image;
};


int CDlgFindLinksData::OnCompare(CListDataPtr const& item, long iCol) const
{
	CDlgFindLinksDataPtr pData2 = std::dynamic_pointer_cast<CDlgFindLinksData, CListData>(item);
	if (!pData2)
		return 1;

	std::wstring str1, str2;
	switch (iCol)
	{
	default:
		assert(0);
	case k_colLink:
		str1 = m_Link;
		str2 = pData2->m_Link;
		break;
	case k_colTrial:
		str1 = m_pTrial->GetGenericName();
		str2 = pData2->m_pTrial->GetGenericName();
		break;
	case k_colDog:
		str1 = m_pDog->GetGenericName();
		str2 = pData2->m_pDog->GetGenericName();
		break;
	case k_colRun:
		str1 = m_pRun->GetGenericName();
		str2 = pData2->m_pRun->GetGenericName();
		break;
	}
	if (str1 < str2)
		return -1;
	else if (str1 > str2)
		return 1;
	return 0;
}


std::wstring CDlgFindLinksData::OnNeedText(long iCol) const
{
	switch (iCol)
	{
	default:
		assert(0);
	case k_colLink:
		return m_Link;
	case k_colTrial:
		return m_pTrial->GetGenericName();
	case k_colDog:
		return m_pDog->GetGenericName();
	case k_colRun:
		return m_pRun->GetGenericName();
	}
}


void CDlgFindLinksData::OnNeedListItem(long iCol, wxListItem& info) const
{
	info.SetMask(info.GetMask() | wxLIST_MASK_TEXT);
	info.SetText(StringUtil::stringWX(OnNeedText(iCol)));
	if (0 == iCol && 0 <= m_Image)
	{
		info.SetMask(info.GetMask() | wxLIST_MASK_IMAGE);
		info.SetImage(m_Image);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDlgFindLinks dialog

wxBEGIN_EVENT_TABLE(CDlgFindLinks, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgFindLinks::OnOk)
wxEND_EVENT_TABLE()


CDlgFindLinks::CDlgFindLinks(ARBDogList& inDogs, wxWindow* pParent)
	: wxDialog()
	, m_ctrlLinks(nullptr)
	, m_reportColumn()
	, m_ctrlEdit(nullptr)
	, m_ctrlOpen(nullptr)
	, m_imgOk(-1)
	, m_imgMissing(-1)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(
		pParent,
		wxID_ANY,
		_("IDD_FIND_LINKS"),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	m_ctrlLinks = new CReportListCtrl(
		this,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(250, 85)),
		true,
		CReportListCtrl::SortHeader::Sort,
		true);
	m_imgOk = m_ctrlLinks->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrCheck));
	m_imgMissing = m_ctrlLinks->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrQuestion));
	m_ctrlLinks->SetHelpText(_("HIDC_FINDLINKS_LIST"));
	m_ctrlLinks->SetToolTip(_("HIDC_FINDLINKS_LIST"));
	m_ctrlLinks->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, [this](wxListEvent& evt) { UpdateButtons(); });
	m_ctrlLinks->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, [this](wxListEvent& evt) { Edit(); });
	m_ctrlLinks->Bind(wxEVT_KEY_DOWN, [this](wxKeyEvent& evt) {
		switch (evt.GetKeyCode())
		{
		default:
			evt.Skip();
			break;
		case WXK_SPACE:
		case WXK_NUMPAD_SPACE:
			Edit();
			break;
		}
	});
	m_reportColumn.Initialize(this, m_ctrlLinks);
	m_reportColumn.CreateColumns(k_columns, -1, k_regkeyBase);

	wxButton* btnOk = new wxButton(this, wxID_OK);

	wxButton* btnCancel = new wxButton(this, wxID_CANCEL);

	wxButton* btnCopy = new wxButton(this, wxID_ANY, _("IDC_FINDLINKS_COPY"), wxDefaultPosition, wxDefaultSize, 0);
	btnCopy->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgFindLinks::OnCopy, this);
	btnCopy->SetHelpText(_("HIDC_FINDLINKS_COPY"));
	btnCopy->SetToolTip(_("HIDC_FINDLINKS_COPY"));

	m_ctrlEdit = new wxButton(this, wxID_ANY, _("IDC_FINDLINKS_EDIT"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgFindLinks::OnEdit, this);
	m_ctrlEdit->SetHelpText(_("HIDC_FINDLINKS_EDIT"));
	m_ctrlEdit->SetToolTip(_("HIDC_FINDLINKS_EDIT"));

	m_ctrlOpen = new wxButton(this, wxID_ANY, _("IDC_FINDLINKS_OPEN"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlOpen->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgFindLinks::OnOpen, this);
	m_ctrlOpen->SetHelpText(_("HIDC_FINDLINKS_OPEN"));
	m_ctrlOpen->SetToolTip(_("HIDC_FINDLINKS_OPEN"));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizerList = new wxBoxSizer(wxHORIZONTAL);
	bSizerList->Add(m_ctrlLinks, 1, wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* bSizerSide = new wxBoxSizer(wxVERTICAL);
	bSizerSide->Add(btnOk, 0, wxEXPAND | wxBOTTOM, wxDLG_UNIT_X(this, 3));
	bSizerSide->Add(btnCancel, 0, wxEXPAND);
	bSizerSide->AddStretchSpacer();
	bSizerSide->Add(btnCopy, 0, wxEXPAND | wxBOTTOM, wxDLG_UNIT_X(this, 3));
	bSizerSide->Add(m_ctrlEdit, 0, wxEXPAND | wxBOTTOM, wxDLG_UNIT_X(this, 3));
	bSizerSide->Add(m_ctrlOpen, 0, wxEXPAND);

	bSizerList->Add(bSizerSide, 0, wxEXPAND);

	bSizer->Add(bSizerList, 1, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	m_ctrlLinks->SetFocus();

	for (ARBDogList::iterator iterDog = inDogs.begin(); iterDog != inDogs.end(); ++iterDog)
	{
		ARBDogPtr pDog = *iterDog;
		for (ARBDogTrialList::iterator iterTrial = pDog->GetTrials().begin(); iterTrial != pDog->GetTrials().end();
			 ++iterTrial)
		{
			ARBDogTrialPtr pTrial = *iterTrial;
			for (ARBDogRunList::iterator iterRun = pTrial->GetRuns().begin(); iterRun != pTrial->GetRuns().end();
				 ++iterRun)
			{
				ARBDogRunPtr pRun = *iterRun;
				std::set<std::wstring> links;
				pRun->GetLinks(links);
				for (std::set<std::wstring>::iterator iter = links.begin(); iter != links.end(); ++iter)
				{
					int image = GetImageIndex(*iter);
					m_Data.push_back(std::make_shared<CDlgFindLinksData>(pDog, pTrial, pRun, *iter, image));
				}
			}
		}
	}

	for (size_t iData = 0; iData < m_Data.size(); ++iData)
	{
		m_ctrlLinks->InsertItem(m_Data[iData]);
	}
	m_reportColumn.SizeColumns();
	m_reportColumn.Sort();

	if (0 < m_Data.size())
		m_ctrlLinks->Select(0);
	m_ctrlLinks->SetFocus();

	UpdateButtons();
}


CDlgFindLinksDataPtr CDlgFindLinks::GetItemLinkData(long item)
{
	return std::dynamic_pointer_cast<CDlgFindLinksData, CListData>(m_ctrlLinks->GetData(item));
}


int CDlgFindLinks::GetImageIndex(std::wstring const& inLink)
{
	wxBusyCursor wait;
	int img = m_ctrlLinks->ImageEmpty();
	if (0 < inLink.length())
	{
		if (CheckLink(inLink, this))
			img = m_imgOk;
		else
			img = m_imgMissing;
	}
	return img;
}


void CDlgFindLinks::UpdateButtons()
{
	bool bEnable = false;
	if (0 <= m_ctrlLinks->GetFirstSelected())
		bEnable = true;
	m_ctrlEdit->Enable(bEnable);
}


void CDlgFindLinks::Edit()
{
	int nItem = m_ctrlLinks->GetFirstSelected();
	if (0 <= nItem)
	{
		CDlgFindLinksDataPtr data = GetItemLinkData(nItem);
		CDlgSelectURL dlg(data->m_Link, true, this);
		if (wxID_OK == dlg.ShowModal())
		{
			std::wstring newName = dlg.Name();
			if (data->m_Link != newName)
			{
				data->m_Link = newName;
				data->m_Image = GetImageIndex(newName);
				m_ctrlLinks->RefreshItem(nItem);
			}
		}
	}
}


void CDlgFindLinks::OnCopy(wxCommandEvent& evt)
{
	if (0 < m_Data.size())
	{
		CClipboardDataWriter clpData;
		if (!clpData.isOkay())
			return;

		std::wstring data;
		for (size_t i = 0; i < m_Data.size(); ++i)
		{
			data += m_Data[i]->m_OldLink;
			data += L"\r\n";
		}

		clpData.AddData(data);
		clpData.CommitData();
	}
}


void CDlgFindLinks::OnEdit(wxCommandEvent& evt)
{
	Edit();
}


void CDlgFindLinks::OnOpen(wxCommandEvent& evt)
{
	int nItem = m_ctrlLinks->GetFirstSelected();
	if (0 <= nItem)
	{
		CDlgFindLinksDataPtr data = GetItemLinkData(nItem);
		if (data)
			wxLaunchDefaultBrowser(StringUtil::stringWX(data->m_Link));
	}
}


void CDlgFindLinks::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;
	for (size_t idx = 0; idx < m_Data.size(); ++idx)
	{
		if (m_Data[idx]->m_Link != m_Data[idx]->m_OldLink)
		{
			m_Data[idx]->m_pRun->RemoveLink(m_Data[idx]->m_OldLink);
			if (0 < m_Data[idx]->m_Link.length())
				m_Data[idx]->m_pRun->AddLink(m_Data[idx]->m_Link);
		}
	}
	EndDialog(wxID_OK);
}

} // namespace dconSoft
