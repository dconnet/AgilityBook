/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgRun class
 * @author David Connet
 *
 * Revision History
 * 2022-04-15 Use wx DPI support.
 * 2021-01-07 Separated into multiple files.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-06-02 Moved ShellExecute code to AgilityBook.cpp, added icons.
 * 2004-03-30 Created
 */

#include "stdafx.h"
#include "DlgRunPanelLinks.h"

#include "DlgSelectURL.h"
#include "ImageHelper.h"

#include "ARB/ARBDogRun.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/CheckLink.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/ResourceManager.h"
#include "LibARBWin/Widgets.h"
#include <wx/dnd.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

#if wxUSE_DRAG_AND_DROP

class CLinkDropTarget : public wxFileDropTarget
{
public:
	CLinkDropTarget(CDlgRunPanelLinks* dlg)
		: m_dlg(dlg)
	{
	}
	bool OnDropFiles(wxCoord x, wxCoord y, wxArrayString const& filenames) override;

private:
	CDlgRunPanelLinks* m_dlg;
};


bool CLinkDropTarget::OnDropFiles(wxCoord x, wxCoord y, wxArrayString const& filenames)
{
	for (size_t n = 0; n < filenames.size(); ++n)
	{
		m_dlg->m_Run->AddLink(StringUtil::stringW(filenames[n]));
		m_dlg->ListLinkFiles(StringUtil::stringW(filenames[n]).c_str());
	}
	return true;
}

#endif

/////////////////////////////////////////////////////////////////////////////

CDlgRunPanelLinks::CDlgRunPanelLinks(
	CDlgRun* pDlg,
	CAgilityBookDoc* pDoc,
	ARBDogPtr const& inDog,
	ARBDogTrialPtr const& inTrial,
	ARBDogRunPtr const& inRun,
	wxWindow* parent)
	: CDlgRunPanelBase(pDlg, pDoc, inDog, inTrial, inRun, parent)
	, m_ctrlLinks(nullptr)
	, m_ImageList()
	, m_imgEmpty(-1)
	, m_imgOk(-1)
	, m_imgMissing(-1)
	, m_ctrlLinkEdit(nullptr)
	, m_ctrlLinkDelete(nullptr)
	, m_ctrlLinkOpen(nullptr)
{
	m_ImageList.Create(FromDIP(16), FromDIP(16));

	m_ctrlLinks = new CListCtrl(
		this,
		wxID_ANY,
		wxDefaultPosition,
		wxDefaultSize,
		wxLC_NO_HEADER | wxLC_REPORT | wxLC_SINGLE_SEL);
	m_ctrlLinks->SetImageList(&m_ImageList, wxIMAGE_LIST_SMALL);
	m_imgEmpty = m_ImageList.Add(CResourceManager::Get()->GetIcon(ImageMgrBlank));
	m_imgOk = m_ImageList.Add(CResourceManager::Get()->GetIcon(ImageMgrCheck));
	m_imgMissing = m_ImageList.Add(CResourceManager::Get()->GetIcon(ImageMgrQuestion));
	m_ctrlLinks->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, &CDlgRunPanelLinks::OnLinksItemSelected, this);
	m_ctrlLinks->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &CDlgRunPanelLinks::OnLinksItemActivated, this);
	m_ctrlLinks->Bind(wxEVT_KEY_DOWN, &CDlgRunPanelLinks::OnLinksKeyDown, this);
	m_ctrlLinks->SetHelpText(_("HIDC_RUNLINK_LIST"));
	m_ctrlLinks->SetToolTip(_("HIDC_RUNLINK_LIST"));
	m_ctrlLinks->InsertColumn(0, L"");
#if wxUSE_DRAG_AND_DROP
	m_ctrlLinks->SetDropTarget(new CLinkDropTarget(this));
#endif

	wxButton* btnLinkNew = new wxButton(this, wxID_ANY, _("IDC_RUNLINK_NEW"), wxDefaultPosition, wxDefaultSize, 0);
	btnLinkNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRunPanelLinks::OnLinksNew, this);
	btnLinkNew->SetHelpText(_("HIDC_RUNLINK_NEW"));
	btnLinkNew->SetToolTip(_("HIDC_RUNLINK_NEW"));

	m_ctrlLinkEdit = new wxButton(this, wxID_ANY, _("IDC_RUNLINK_EDIT"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlLinkEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRunPanelLinks::OnLinksEdit, this);
	m_ctrlLinkEdit->SetHelpText(_("HIDC_RUNLINK_EDIT"));
	m_ctrlLinkEdit->SetToolTip(_("HIDC_RUNLINK_EDIT"));

	m_ctrlLinkDelete = new wxButton(this, wxID_ANY, _("IDC_RUNLINK_DELETE"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlLinkDelete->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRunPanelLinks::OnLinksDelete, this);
	m_ctrlLinkDelete->SetHelpText(_("HIDC_RUNLINK_DELETE"));
	m_ctrlLinkDelete->SetToolTip(_("HIDC_RUNLINK_DELETE"));

	m_ctrlLinkOpen = new wxButton(this, wxID_ANY, _("IDC_RUNLINK_OPEN"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlLinkOpen->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRunPanelLinks::OnLinksOpen, this);
	m_ctrlLinkOpen->SetHelpText(_("HIDC_RUNLINK_OPEN"));
	m_ctrlLinkOpen->SetToolTip(_("HIDC_RUNLINK_OPEN"));

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* sizerLinks = new wxBoxSizer(wxVERTICAL);
	sizerLinks->Add(m_ctrlLinks, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerLinkBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerLinkBtns->Add(btnLinkNew, 0, wxRIGHT, padding.Inner());
	sizerLinkBtns->Add(m_ctrlLinkEdit, 0, wxRIGHT, padding.Inner());
	sizerLinkBtns->Add(m_ctrlLinkDelete, 0, wxRIGHT, padding.Inner());
	sizerLinkBtns->Add(0, 0, 1, wxEXPAND, 0);
	sizerLinkBtns->Add(m_ctrlLinkOpen, 0, 0, 0);

	sizerLinks->Add(sizerLinkBtns, 0, wxEXPAND | wxALL, padding.Controls());

	SetSizer(sizerLinks);
	Layout();
	sizerLinks->Fit(this);

	ListLinkFiles(nullptr);
}


wxWindow* CDlgRunPanelLinks::GetInitialControl()
{
	return m_ctrlLinks;
}


bool CDlgRunPanelLinks::Validate()
{
	return true;
}


bool CDlgRunPanelLinks::Save()
{
	return true;
}


void CDlgRunPanelLinks::UpdateLinksButtons()
{
	bool bEnable = (0 <= m_ctrlLinks->GetFirstSelected());
	m_ctrlLinkEdit->Enable(bEnable);
	m_ctrlLinkDelete->Enable(bEnable);
	m_ctrlLinkOpen->Enable(bEnable);
}


void CDlgRunPanelLinks::ListLinkFiles(wchar_t const* pItem)
{
	wxBusyCursor wait;
	m_ctrlLinks->DeleteAllItems();
	std::set<std::wstring> links;
	m_Run->GetLinks(links);
	long i = 0;
	for (std::set<std::wstring>::iterator iter = links.begin(); iter != links.end(); ++iter)
	{
		int idx = m_ctrlLinks->InsertItem(i++, StringUtil::stringWX(*iter), GetImageIndex(*iter));
		if (pItem && *iter == pItem)
			m_ctrlLinks->Select(idx);
	}
	m_ctrlLinks->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
	UpdateLinksButtons();
}


int CDlgRunPanelLinks::GetImageIndex(std::wstring const& inLink)
{
	wxBusyCursor wait;
	int img = m_imgEmpty;
	if (0 < inLink.length())
	{
		if (CheckLink(inLink, this))
			img = m_imgOk;
		else
			img = m_imgMissing;
	}
	return img;
}


void CDlgRunPanelLinks::EditLink()
{
	int nItem = m_ctrlLinks->GetFirstSelected();
	if (0 <= nItem)
	{
		std::wstring name = GetListColumnText(m_ctrlLinks, nItem, 0);
		CDlgSelectURL dlg(name, this);
		if (wxID_OK == dlg.ShowModal())
		{
			std::wstring newName = dlg.Name();
			if (name != newName)
			{
				m_Run->RemoveLink(name);
				if (0 < newName.length())
					m_Run->AddLink(newName);
				ListLinkFiles(newName.c_str());
			}
		}
	}
}


void CDlgRunPanelLinks::OnLinksItemSelected(wxListEvent& evt)
{
	UpdateLinksButtons();
}


void CDlgRunPanelLinks::OnLinksItemActivated(wxListEvent& evt)
{
	EditLink();
}


void CDlgRunPanelLinks::OnLinksKeyDown(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		evt.Skip();
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		EditLink();
		break;
	}
}


void CDlgRunPanelLinks::OnLinksNew(wxCommandEvent& evt)
{
	CDlgSelectURL dlg(L"", this);
	if (wxID_OK == dlg.ShowModal())
	{
		std::wstring newName = dlg.Name();
		if (0 < newName.length())
		{
			m_Run->AddLink(newName);
			ListLinkFiles(newName.c_str());
		}
	}
}


void CDlgRunPanelLinks::OnLinksEdit(wxCommandEvent& evt)
{
	EditLink();
}


void CDlgRunPanelLinks::OnLinksDelete(wxCommandEvent& evt)
{
	long nItem = m_ctrlLinks->GetFirstSelected();
	if (0 <= nItem)
	{
		std::wstring name = GetListColumnText(m_ctrlLinks, nItem, 0);
		m_Run->RemoveLink(name);
		m_ctrlLinks->DeleteItem(nItem);
		if (nItem == m_ctrlLinks->GetItemCount())
			--nItem;
		if (0 <= nItem)
			m_ctrlLinks->Select(nItem);
	}
}


void CDlgRunPanelLinks::OnLinksOpen(wxCommandEvent& evt)
{
	int nItem = m_ctrlLinks->GetFirstSelected();
	if (0 <= nItem)
	{
		std::wstring name = GetListColumnText(m_ctrlLinks, nItem, 0);
		wxLaunchDefaultBrowser(StringUtil::stringWX(name));
	}
}

} // namespace dconSoft
