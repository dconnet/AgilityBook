/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Dialog for adding notes to ARBInfo items.
 * @author David Connet
 *
 * Revision History
 * 2021-09-08 Created
 */

#include "stdafx.h"
#include "DlgInfoNote.h"

#include "ImageHelper.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/ReportListCtrl.h"
#include "LibARBWin/ReportListHeader.h"
#include "LibARBWin/Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

namespace
{
constexpr int k_colIcon = 0;
constexpr int k_colName = 1;
constexpr int k_colComment = 2;
static const std::vector<CReportListHeader::ColumnInfo> k_columns{
	{k_colIcon, wxLIST_FORMAT_LEFT, L""},
	{k_colName, wxLIST_FORMAT_LEFT, arbT("IDS_COL_NAME")},
	{k_colComment, wxLIST_FORMAT_LEFT, arbT("IDS_COL_COMMENTS")},
};

} // namespace

/////////////////////////////////////////////////////////////////////////////

ARB_TYPEDEF(InfoNoteListData)

class InfoNoteAlternate : public InfoNotePanel
{
	friend class InfoNoteListData;

public:
	InfoNoteAlternate(std::set<std::wstring> const& namesInUse, std::wstring const& inSelect, CDlgInfoNote* parent);

	wxWindow* GetInitialFocus() override;
	void LoadData() override;
	bool OnOk() override;

private:
	InfoNoteListDataPtr GetData(long index) const;

	CReportListCtrl* m_ctrlList;
	CReportListHeader m_reportColumn;
	int m_imgNone;
	int m_imgNote;
	int m_imgAdded;
	int m_imgNoteAdded;
};

InfoNotePanel* InfoNotePanel::CreateAlternate(
	std::set<std::wstring> const& namesInUse,
	std::wstring const& inSelect,
	CDlgInfoNote* parent)
{
	return new InfoNoteAlternate(namesInUse, inSelect, parent);
}

/////////////////////////////////////////////////////////////////////////////

class InfoNoteListData : public CListData
{
	DECLARE_NO_COPY_IMPLEMENTED(InfoNoteListData)
public:
	InfoNoteListData(InfoNoteAlternate* parent, size_t idxName)
		: m_parent(parent)
		, m_idxName(idxName)
		, m_item()
	{
		m_item = m_parent->m_parent->FindName(m_parent->m_parent->GetNames()[m_idxName].m_name);
	}
	int OnCompare(CListDataPtr const& item, long iCol) const override;
	std::wstring OnNeedText(long iCol) const override;
	void OnNeedListItem(long iCol, wxListItem& info) const override;

protected:
	int GetIcon() const;

	InfoNoteAlternate* m_parent;
	size_t m_idxName;
	ARBInfoItemPtr m_item;
};


int InfoNoteListData::OnCompare(CListDataPtr const& item, long iCol) const
{
	InfoNoteListDataPtr pData2 = std::dynamic_pointer_cast<InfoNoteListData, CListData>(item);
	if (!pData2)
		return 1;

	int rc = 0;
	switch (iCol)
	{
	case k_colIcon:
	{
		int icon1 = GetIcon();
		int icon2 = pData2->GetIcon();
		if (icon1 < icon2)
			rc = -1;
		else if (icon1 > icon2)
			rc = 1;
	}
	break;
	case k_colName:
	{
		std::wstring n1 = m_parent->m_parent->GetNames()[m_idxName].m_name;
		std::wstring n2 = m_parent->m_parent->GetNames()[pData2->m_idxName].m_name;
		if (n1 < n2)
			rc = -1;
		else if (n1 > n2)
			rc = 1;
	}
	break;
	case k_colComment:
	{
		std::wstring s1 = m_item ? m_item->GetComment() : L"";
		std::wstring s2 = pData2->m_item ? pData2->m_item->GetComment() : L"";
		if (s1 < s2)
			rc = -1;
		else if (s1 > s2)
			rc = 1;
	}
	break;
	}
	return rc;
}


std::wstring InfoNoteListData::OnNeedText(long iCol) const
{
	std::wstring str;
	switch (iCol)
	{
	case k_colName:
		str = m_parent->m_parent->GetNames()[m_idxName].m_name;
		break;
	case k_colComment:
	{
		if (m_item)
			str = m_item->GetComment();
	}
	break;
	}
	return str;
}


void InfoNoteListData::OnNeedListItem(long iCol, wxListItem& info) const
{
	switch (iCol)
	{
	case k_colIcon:
		info.SetMask(info.GetMask() | wxLIST_MASK_IMAGE);
		info.SetImage(GetIcon());

		break;
	default:
		info.SetText(StringUtil::stringWX(OnNeedText(iCol)));
	}
}


int InfoNoteListData::GetIcon() const
{
	int image = m_parent->m_imgNone;
	if (0 < m_parent->m_parent->GetAddedCount()
		&& NameInfo::Usage::NotInUse == m_parent->m_parent->GetNames()[m_idxName].m_usage)
	{
		if (m_parent->m_parent->GetNames()[m_idxName].m_hasData)
			image = m_parent->m_imgNoteAdded;
		else
			image = m_parent->m_imgAdded;
	}
	else
	{
		if (m_parent->m_parent->GetNames()[m_idxName].m_hasData)
			image = m_parent->m_imgNote;
	}
	return image;
}

/////////////////////////////////////////////////////////////////////////////

InfoNoteAlternate::InfoNoteAlternate(
	std::set<std::wstring> const& namesInUse,
	std::wstring const& inSelect,
	CDlgInfoNote* parent)
	: InfoNotePanel(namesInUse, inSelect, parent)
	, m_ctrlList(nullptr)
	, m_imgNone(-1)
	, m_imgNote(-1)
	, m_imgAdded(-1)
	, m_imgNoteAdded(-1)
{
	m_ctrlList = new CReportListCtrl(this, true, CReportListCtrl::SortHeader::Sort, true, true);
	// m_ctrlList->SetHelpText(_("HIDC_DOG_TITLE_TITLES"));
	// m_ctrlList->SetToolTip(_("HIDC_DOG_TITLE_TITLES"));
#if 0
	// TODO: not getting selected when selecting 2nd item via shift-click (wx bug)
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, [this](wxListEvent& evt) { UpdateControls(); });
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_ITEM_DESELECTED, [this](wxListEvent& evt) { UpdateControls(); });
	// Listen to focused because of https://trac.wxwidgets.org/ticket/4541
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_ITEM_FOCUSED, [this](wxListEvent& evt) { UpdateControls(); });
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &CDlgDogPanelReportBase::OnItemActivated, this);
	m_ctrlList->Bind(wxEVT_KEY_DOWN, &CDlgDogPanelReportBase::OnKeyDown, this);
#endif
	m_reportColumn.Initialize(this, m_ctrlList);
	m_reportColumn.CreateColumns(k_columns, k_colName);

	m_imgNone = m_ctrlList->AddIcon(CImageManager::Get()->GetIcon(ImageMgrBlank));
	m_imgNote = m_ctrlList->AddIcon(CImageManager::Get()->GetIcon(ImageMgrInfoNote));
	m_imgAdded = m_ctrlList->AddIcon(CImageManager::Get()->GetIcon(ImageMgrInfoNoteAdded));
	m_imgNoteAdded = m_ctrlList->AddIcon(CImageManager::Get()->GetIcon(ImageMgrInfoNoteNoteAdded));

	LoadData();

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	bSizer->Add(m_ctrlList, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(parent, 5));

	SetSizer(bSizer);
}


wxWindow* InfoNoteAlternate::GetInitialFocus()
{
	return m_ctrlList;
}


void InfoNoteAlternate::LoadData()
{
	m_ctrlList->DeleteAllItems();

	InfoNoteListDataPtr pSelected;
	for (size_t idxName = 0; idxName < m_parent->GetNames().size(); ++idxName)
	{
		InfoNoteListDataPtr data(std::make_shared<InfoNoteListData>(this, idxName));
		m_ctrlList->InsertItem(data);
		if (!pSelected && 0 < m_Select.length())
		{
			auto name = m_parent->GetNames()[idxName].m_name;
			if (0 == name.find(m_Select))
			{
				pSelected = data;
			}
		}
	}
	m_reportColumn.SizeColumns();
	m_reportColumn.Sort();

	if (pSelected)
	{
		for (long i = 0; i < m_ctrlList->GetItemCount(); ++i)
		{
			InfoNoteListDataPtr data = GetData(i);
			if (data->OnNeedText(1) == pSelected->OnNeedText(1))
			{
				m_ctrlList->Select(i, true);
				m_ctrlList->Focus(i);
				break;
			}
		}
	}
}


bool InfoNoteAlternate::OnOk()
{
	return true;
}


InfoNoteListDataPtr InfoNoteAlternate::GetData(long index) const
{
	return std::dynamic_pointer_cast<InfoNoteListData, CListData>(m_ctrlList->GetData(index));
}
