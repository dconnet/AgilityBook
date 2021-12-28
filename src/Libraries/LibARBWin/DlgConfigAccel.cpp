/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Data-driven helper to create menus and toolbar items.
 * @author David Connet
 *
 * Revision History
 * 2019-03-05 Moved out of MenuHelper.cpp
 */

#include "stdafx.h"
#include "DlgConfigAccel.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/MenuHelper.h"
#include "LibARBWin/ReportListCtrl.h"
#include "LibARBWin/Validators.h"
#include "fmt/xchar.h"
#include <wx/utils.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


static wxString GetKeyCode(std::unordered_map<int, KeyCodeMapping> const& keyMap, int code)
{
	wxString text;
	auto key = keyMap.find(code);

	if (key != keyMap.end())
		text = wxGetTranslation(key->second.special);
	else if (isascii(code))
		text << static_cast<wxChar>(code);

	return text;
}


static std::wstring TrimDots(wxString const& str)
{
	std::wstring s = StringUtil::stringW(str);
	if (s.length() > 3)
	{
		if (s.substr(s.length() - 3) == L"...")
			s = s.substr(0, s.length() - 3);
	}
	return s;
}

/////////////////////////////////////////////////////////////////////////////

class CDlgEditAccel : public wxDialog
{
public:
	CDlgEditAccel(
		std::vector<CMenuHelper::ItemAccel> const& accelData,
		CMenuHelper::ItemAccel const& item,
		std::unordered_map<int, KeyCodeMapping> const& keyMap,
		std::wstring const& action,
		bool bAllowDups,
		wxWindow* pParent);

	bool GetData(CMenuHelper::ItemAccel& item);

private:
	void OnKeyDown(wxKeyEvent& evt);
	void OnClear(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);

	std::vector<CMenuHelper::ItemAccel> const& m_accelData;
	CMenuHelper::ItemAccel m_item;
	std::unordered_map<int, KeyCodeMapping> const& m_keyMap;
	bool m_bAllowDups;
	wxTextCtrl* m_ctrlKey;
};

/////////////////////////////////////////////////////////////////////////////

CDlgEditAccel::CDlgEditAccel(
	std::vector<CMenuHelper::ItemAccel> const& accelData,
	CMenuHelper::ItemAccel const& item,
	std::unordered_map<int, KeyCodeMapping> const& keyMap,
	std::wstring const& action,
	bool bAllowDups,
	wxWindow* pParent)
	: m_accelData(accelData)
	, m_item(item)
	, m_keyMap(keyMap)
	, m_bAllowDups(bAllowDups)
	, m_ctrlKey(nullptr)
{
	wxString caption = wxString::Format(L"%s : %s", _("Assign Key"), action.c_str());
	Create(pParent, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	wxStaticText* textKey = new wxStaticText(this, wxID_ANY, _("Key:"), wxDefaultPosition, wxDefaultSize, 0);

	m_ctrlKey = new wxTextCtrl(
		this,
		wxID_ANY,
		GetKeyCode(m_keyMap, m_item.keyCode),
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(30, -1)),
		0);
	m_ctrlKey->Bind(wxEVT_KEY_DOWN, &CDlgEditAccel::OnKeyDown, this);

	wxStaticText* textMod = new wxStaticText(this, wxID_ANY, _("Modifiers:"), wxDefaultPosition, wxDefaultSize, 0);

	wxCheckBox* ctrlCtrl = new wxCheckBox(
		this,
		wxID_ANY,
		_("Ctrl"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_item.bCtrl));

	wxCheckBox* ctrlAlt = new wxCheckBox(
		this,
		wxID_ANY,
		_("Alt"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_item.bAlt));

	wxCheckBox* ctrlShift = new wxCheckBox(
		this,
		wxID_ANY,
		_("Shift"),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		wxGenericValidator(&m_item.bShift));

	wxButton* ctrlClear = new wxButton(this, wxID_ANY, _("Clear"), wxDefaultPosition, wxDefaultSize, 0);
	ctrlClear->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgEditAccel::OnClear, this);

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerAll = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerModifiers = new wxBoxSizer(wxHORIZONTAL);
	sizerModifiers->Add(ctrlCtrl, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerModifiers->Add(ctrlAlt, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerModifiers->Add(ctrlShift, 0, 0, 0);

	wxFlexGridSizer* sizerCtrls = new wxFlexGridSizer(2, 2, wxDLG_UNIT_X(this, 3), wxDLG_UNIT_X(this, 3));
	sizerCtrls->AddGrowableCol(1);
	sizerCtrls->SetFlexibleDirection(wxBOTH);
	sizerCtrls->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerCtrls->Add(textKey, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 0);
	sizerCtrls->Add(m_ctrlKey, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND, 0);
	sizerCtrls->Add(textMod, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 0);
	sizerCtrls->Add(sizerModifiers, 0, wxALIGN_CENTER_VERTICAL, 0);

	sizerAll->Add(sizerCtrls, 0, wxALL, wxDLG_UNIT_X(this, 3));
	sizerAll->Add(ctrlClear, 0, wxALL, wxDLG_UNIT_X(this, 3));

	bSizer->Add(sizerAll, 0, wxLEFT | wxTOP | wxRIGHT, wxDLG_UNIT_X(this, 3));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL | wxEXPAND, wxDLG_UNIT_X(this, 3));

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgEditAccel::OnOk, this, wxID_OK);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgEditAccel, m_ctrlKey)
}


bool CDlgEditAccel::GetData(CMenuHelper::ItemAccel& item)
{
	item = m_item;
	return true;
}


void CDlgEditAccel::OnKeyDown(wxKeyEvent& evt)
{
	wxString text = GetKeyCode(m_keyMap, evt.GetKeyCode());
	if (!text.IsEmpty())
	{
		m_item.keyCode = evt.GetKeyCode();
		m_ctrlKey->ChangeValue(text);
	}
}


void CDlgEditAccel::OnClear(wxCommandEvent& evt)
{
	m_item.Clear();
	m_ctrlKey->Clear();
	TransferDataToWindow();
}


void CDlgEditAccel::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;
	if (!m_bAllowDups)
	{
		std::set<std::pair<int, long>> accels;

		long mask = CMenuHelper::ToBitmask(m_item);
		accels.insert(std::make_pair(m_item.keyCode, mask));

		for (auto iter = m_accelData.begin(); iter != m_accelData.end(); ++iter)
		{
			if (iter->keyCode != 0 && *iter != m_item)
			{
				mask = CMenuHelper::ToBitmask(*iter);
				auto accel = std::make_pair(iter->keyCode, mask);
				if (accels.find(accel) != accels.end())
				{
					wxMessageBox(
						_("ERROR: This accelerator is already in use."),
						wxMessageBoxCaptionStr,
						wxOK | wxCENTRE | wxICON_STOP);
					return;
				}
			}
		}
	}

	EndDialog(wxID_OK);
}

/////////////////////////////////////////////////////////////////////////////

ARB_TYPEDEF(CMenuData)
class CMenuData : public CListData
{
public:
	CMenuData(
		std::unordered_map<int, KeyCodeMapping> const& keyMap,
		std::vector<CMenuHelper::ItemAccel> const& accels,
		std::wstring const& path,
		std::wstring const& item,
		std::wstring const& location,
		int order)
		: m_keyMap(keyMap)
		, m_accels(accels)
		, m_path(path)
		, m_item(item)
		, m_location(location)
		, m_order(order)
	{
	}

	int OnCompare(CListDataPtr const& item, long iCol) const override
	{
		CMenuDataPtr pRow2 = std::dynamic_pointer_cast<CMenuData, CListData>(item);
		if (!pRow2)
			return 0;

		int o1 = GetSortOrder(iCol);
		int o2 = pRow2->GetSortOrder(iCol);
		int nRet = 0;
		if (o1 < o2)
			nRet = -1;
		else if (o1 > o2)
			nRet = 1;
		return nRet;
	}

	std::wstring OnNeedText(long iCol) const override
	{
		switch (iCol)
		{
		case 0:
			if (0 == m_accels.size())
			{
				return std::wstring();
			}
			else
			{
				fmt::wmemory_buffer data;
				for (size_t i = 0; i < m_accels.size(); ++i)
				{
					if (0 < i)
						fmt::format_to(std::back_inserter(data), L"{}", L", ");
					if (m_accels[i].bCtrl)
						fmt::format_to(std::back_inserter(data), L"{}", _("Ctrl+").wx_str());
					if (m_accels[i].bAlt)
						fmt::format_to(std::back_inserter(data), L"{}", _("Alt+").wx_str());
					if (m_accels[i].bShift)
						fmt::format_to(std::back_inserter(data), L"{}", _("Shift+").wx_str());
					fmt::format_to(std::back_inserter(data), L"{}", GetKeyCode(m_keyMap, m_accels[i].keyCode).wx_str());
				}
				return fmt::to_string(data);
			}
			break;
		case 1:
			return m_path;
		case 2:
			return m_location;
		default:
			assert(0);
		}
		return std::wstring();
	}

	std::vector<CMenuHelper::ItemAccel> const& Accels() const
	{
		return m_accels;
	}

	void ClearAccels()
	{
		for (CMenuHelper::ItemAccel& item : m_accels)
			item.Clear();
	}

	int GetSortOrder(int inCol) const
	{
		switch (inCol)
		{
		case 0:
			if (m_accels.size() == 0)
				return 0;
			return m_accels[0].keyCode;
		case 1:
			return m_order;
		}
		return 0;
	}

	bool Configure(std::vector<CMenuHelper::ItemAccel> const& accelData, bool bAdd, bool bAllowDups, wxWindow* pParent)
	{
		assert(m_accels.size() > 0);

		bool bUpdated = false;
		CMenuHelper::ItemAccel item;
		if (bAdd)
		{
			bool bFoundEmpty = false;
			for (CMenuHelper::ItemAccel const& i : m_accels)
			{
				if (!i.keyCode)
				{
					item = i;
					bFoundEmpty = true;
					break;
				}
			}
			if (!bFoundEmpty)
				return bUpdated;
		}
		else
		{
#pragma PRAGMA_TODO(handle selecting from multiple items)
			item = m_accels[0];
		}

		CDlgEditAccel dlg(accelData, item, m_keyMap, m_item, bAllowDups, pParent);
		if (wxID_OK == dlg.ShowModal())
		{
			dlg.GetData(item);
			for (CMenuHelper::ItemAccel& i : m_accels)
			{
				if (i.key == item.key)
					i = item;
			}
			bUpdated = true;
		}
		return bUpdated;
	}

	int Id() const
	{
		if (m_accels.size() == 0)
			return 0;
		return m_accels[0].id;
	}

	void CopyFrom(CMenuDataPtr pItem)
	{
		m_accels = pItem->m_accels;
	}

private:
	std::unordered_map<int, KeyCodeMapping> const& m_keyMap;
	std::vector<CMenuHelper::ItemAccel> m_accels;
	std::wstring m_path;
	std::wstring m_item;
	std::wstring m_location;
	int m_order;
};

/////////////////////////////////////////////////////////////////////////////

CDlgConfigAccel::CDlgConfigAccel(
	std::unordered_map<int, std::wstring> const& menuIds,
	std::vector<CMenuHelper::ItemAccel> const& accelData,
	std::vector<CMenuHelper::ItemAccel> const& accelDataDefaults,
	bool bAllowDups,
	std::vector<CMenuHelper::ItemData> const& menuItems,
	std::unordered_map<int, KeyCodeMapping> const& keyMap,
	wxWindow* pParent,
	std::wstring caption)
	: m_menuIds(menuIds)
	, m_accelDataDefaults(accelDataDefaults)
	, m_bAllowDups(bAllowDups)
	, m_menuItems()
	, m_accelData(accelData)
	, m_keyMap(keyMap)
	, m_reportColumn()
	, m_ctrlItems(nullptr)
	, m_ctrlNew(nullptr)
	, m_ctrlEdit(nullptr)
	, m_ctrlClear(nullptr)
{
	{
		std::vector<std::wstring> path;
		path.push_back(TrimDots(wxStripMenuCodes(wxGetTranslation(menuItems[0].menu))));

		for (size_t i = 1; i < menuItems.size(); ++i)
		{
			while (path.size() > menuItems[i].menuLevel)
				path.pop_back();

			std::wstring menuItem = TrimDots(wxStripMenuCodes(wxGetTranslation(menuItems[i].menu)));
			path.push_back(menuItem);

			fmt::wmemory_buffer data;
			for (size_t n = 0; n < path.size(); ++n)
			{
				if (0 == n)
					fmt::format_to(std::back_inserter(data), L"{}", path[n]);
				else
					fmt::format_to(std::back_inserter(data), L" | {}", path[n]);
			}

			MenuData menudata;
			menudata.m_data = &menuItems[i];
			menudata.m_path = fmt::to_string(data);
			menudata.m_item = menuItem;
			m_menuItems.push_back(menudata);
		}
	}

	if (caption.empty())
		caption = StringUtil::stringW(_("Configure Keyboard Shortcuts"));
	Create(pParent, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	m_ctrlItems = new CReportListCtrl(
		this,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(250, 150)),
		true,
		CReportListCtrl::SortHeader::Sort,
		true,
		false);
	m_ctrlItems->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, &CDlgConfigAccel::OnItemSelected, this);
	m_ctrlItems->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &CDlgConfigAccel::OnItemActivated, this);
	m_ctrlItems->Bind(wxEVT_KEY_DOWN, &CDlgConfigAccel::OnKeyDown, this);

	std::vector<CReportListHeader::ColumnInfo> columns;
	columns.push_back(CReportListHeader::ColumnInfo{0, wxLIST_FORMAT_LEFT, arbT("Keyboard Shortcut")});
	columns.push_back(CReportListHeader::ColumnInfo{1, wxLIST_FORMAT_LEFT, arbT("Description")});
	if (!menuIds.empty())
		columns.push_back(CReportListHeader::ColumnInfo{2, wxLIST_FORMAT_LEFT, arbT("Location")});
	m_reportColumn.Initialize(this, m_ctrlItems);
	m_reportColumn.CreateColumns(columns);
	LoadData();

	m_ctrlNew = new wxButton(this, wxID_ANY, _("New..."), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigAccel::OnAddItem, this);

	m_ctrlEdit = new wxButton(this, wxID_ANY, _("Edit..."), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigAccel::OnEditItem, this);

	m_ctrlClear = new wxButton(this, wxID_ANY, _("Clear"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlClear->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigAccel::OnClearItem, this);

	wxButton* ctrlDefaults = new wxButton(this, wxID_ANY, _("Set Defaults"), wxDefaultPosition, wxDefaultSize, 0);
	ctrlDefaults->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigAccel::OnSetDefaults, this);

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerItems = new wxBoxSizer(wxHORIZONTAL);
	sizerItems->Add(m_ctrlItems, 1, wxALL | wxEXPAND, wxDLG_UNIT_X(this, 3));

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxVERTICAL);
	sizerBtns->Add(m_ctrlNew, 0, wxALL | wxEXPAND, wxDLG_UNIT_X(this, 3));
	sizerBtns->Add(m_ctrlEdit, 0, wxALL | wxEXPAND, wxDLG_UNIT_X(this, 3));
	sizerBtns->Add(m_ctrlClear, 0, wxALL | wxEXPAND, wxDLG_UNIT_X(this, 3));
	sizerBtns->Add(ctrlDefaults, 0, wxALL | wxEXPAND, wxDLG_UNIT_X(this, 3));

	sizerItems->Add(sizerBtns, 0, wxEXPAND, 0);

	bSizer->Add(sizerItems, 1, wxEXPAND, 0);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL | wxEXPAND, wxDLG_UNIT_X(this, 3));

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgConfigAccel::OnOk, this, wxID_OK);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	UpdateControls();
}


bool CDlgConfigAccel::GetAccelData(std::vector<CMenuHelper::ItemAccel>& accelData)
{
	if (accelData.size() == m_accelData.size())
	{
		std::vector<CMenuHelper::ItemAccel> v1(m_accelData);
		std::vector<CMenuHelper::ItemAccel> v2(accelData);
		std::sort(v1.begin(), v1.end(), [](CMenuHelper::ItemAccel const& one, CMenuHelper::ItemAccel const& two) {
			return one.key < two.key;
		});
		std::sort(v2.begin(), v2.end(), [](CMenuHelper::ItemAccel const& one, CMenuHelper::ItemAccel const& two) {
			return one.key < two.key;
		});
		bool bModified = false;
		for (size_t i = 0; !bModified && i < v1.size(); ++i)
		{
			if (v1[i] != v2[i])
				bModified = true;
		}
		if (!bModified)
			return false;
	}
	accelData = m_accelData;
	return true;
}


void CDlgConfigAccel::LoadData()
{
	m_ctrlItems->Freeze();

	m_ctrlItems->DeleteAllItems();
	int lastMenuId = -1;
#if defined(_DEBUG) || defined(__WXDEBUG__)
	std::set<int> inUseId;
	std::set<int> inUseMenuId;
#endif
	int count = 0;
	for (size_t i = 0; i < m_menuItems.size(); ++i)
	{
		if (m_menuItems[i].m_data->id && !(m_menuItems[i].m_data->flags & MENU_MRU))
		{
			std::vector<CMenuHelper::ItemAccel> accels;

			// Add a blank line between menus.
			if (lastMenuId != m_menuItems[i].m_data->menuId)
			{
				lastMenuId = m_menuItems[i].m_data->menuId;
				if (m_ctrlItems->GetItemCount() > 0)
					m_ctrlItems->InsertItem(std::make_shared<CMenuData>(
						m_keyMap,
						accels,
						std::wstring(),
						std::wstring(),
						std::wstring(),
						-1));
			}

#if defined(_DEBUG) || defined(__WXDEBUG__)
			inUseId.insert(m_menuItems[i].m_data->id);
			auto iterMenuId = inUseMenuId.find(m_menuItems[i].m_data->menuId);
			// Make sure we have a description for each menu.
			if (iterMenuId == inUseMenuId.end())
			{
				inUseMenuId.insert(m_menuItems[i].m_data->menuId);
				if (m_menuIds.size() == 0)
					assert(inUseMenuId.size() == 1);
				else
					assert(m_menuIds.find(m_menuItems[i].m_data->menuId) != m_menuIds.end());
			}
#endif

			// Support for multiple accelerators for an id (like copy: Ctrl+C/Ctrl+Ins)
			for (size_t n = 0; n < m_accelData.size(); ++n)
			{
				if (m_accelData[n].id == m_menuItems[i].m_data->id)
				{
					accels.push_back(m_accelData[n]);
				}
			}
			// This means you forgot to add the ItemData to the ItemAccel array.
			// (all menu items must be represented in the accel array)
			assert(accels.size() > 0);

			auto menuStr = m_menuIds.find(m_menuItems[i].m_data->menuId);
			std::wstring location;
			if (menuStr != m_menuIds.end())
				location = menuStr->second;
			m_ctrlItems->InsertItem(std::make_shared<CMenuData>(
				m_keyMap,
				accels,
				m_menuItems[i].m_path,
				m_menuItems[i].m_item,
				location,
				count++));
		}
	}
#if defined(_DEBUG) || defined(__WXDEBUG__)
	// Sanity checking
	// All the accelerators must be in a menu (can't get the text otherwise)
	for (CMenuHelper::ItemAccel const& item : m_accelData)
	{
		if (inUseId.find(item.id) == inUseId.end())
		{
			assert(0); // For now, we don't support independent accel items
			inUseId.insert(item.id);
		}
	}
#endif

	m_reportColumn.SizeColumns();
	m_reportColumn.Sort();

	m_ctrlItems->Thaw();
}


void CDlgConfigAccel::UpdateControls()
{
	long idx = m_ctrlItems->GetSelection(true);
	bool bEnableNew = false;
	bool bEnableEdit = (0 <= idx);
	bool bHasAccel = false;
	if (0 <= idx)
	{
		CMenuDataPtr pData = std::dynamic_pointer_cast<CMenuData, CListData>(m_ctrlItems->GetData(idx));
		size_t nConfigured = 0;
		for (CMenuHelper::ItemAccel const& item : pData->Accels())
		{
			if (item.keyCode)
			{
				++nConfigured;
			}
		}
		bEnableNew = (nConfigured < pData->Accels().size());
		bHasAccel = (nConfigured > 0);
	}

	m_ctrlNew->Enable(bEnableNew);
	m_ctrlEdit->Enable(bEnableEdit);
	m_ctrlClear->Enable(bHasAccel);
}


void CDlgConfigAccel::DoEdit(bool bAdd)
{
	long idx = m_ctrlItems->GetSelection(true);
	if (0 > idx)
		return;
	CMenuDataPtr pData = std::dynamic_pointer_cast<CMenuData, CListData>(m_ctrlItems->GetData(idx));
	if (pData->Configure(m_accelData, bAdd, m_bAllowDups, this))
	{
		std::vector<long> sameItemsIdx;
		for (long item = 0; item < m_ctrlItems->GetItemCount(); ++item)
		{
			CMenuDataPtr pItem = std::dynamic_pointer_cast<CMenuData, CListData>(m_ctrlItems->GetData(item));
			if (pItem->Id() == pData->Id())
			{
				if (item != idx)
					pItem->CopyFrom(pData);
				sameItemsIdx.push_back(item);
			}
		}

		for (long item : sameItemsIdx)
			m_ctrlItems->RefreshItem(item);
		UpdateControls();
	}
}


void CDlgConfigAccel::OnItemSelected(wxListEvent& evt)
{
	UpdateControls();
}


void CDlgConfigAccel::OnItemActivated(wxListEvent& evt)
{
	DoEdit(false);
}


void CDlgConfigAccel::OnKeyDown(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		evt.Skip();
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		DoEdit(false);
		break;
	}
}


void CDlgConfigAccel::OnAddItem(wxCommandEvent& evt)
{
	DoEdit(true);
}


void CDlgConfigAccel::OnEditItem(wxCommandEvent& evt)
{
	DoEdit(false);
}


void CDlgConfigAccel::OnClearItem(wxCommandEvent& evt)
{
	long idx = m_ctrlItems->GetSelection(true);
	if (0 > idx)
		return;
	CMenuDataPtr pData = std::dynamic_pointer_cast<CMenuData, CListData>(m_ctrlItems->GetData(idx));

	pData->ClearAccels();
	m_ctrlItems->RefreshItem(idx);
	UpdateControls();
	m_ctrlItems->SetFocus();
}


void CDlgConfigAccel::OnSetDefaults(wxCommandEvent& evt)
{
	m_accelData = m_accelDataDefaults;
	LoadData();
	UpdateControls();
}


void CDlgConfigAccel::OnOk(wxCommandEvent& evt)
{
	m_accelData.clear();
	for (long idx = 0; idx < m_ctrlItems->GetItemCount(); ++idx)
	{
		CMenuDataPtr pData = std::dynamic_pointer_cast<CMenuData, CListData>(m_ctrlItems->GetData(idx));
		for (CMenuHelper::ItemAccel const& item : pData->Accels())
		{
			if (std::find(m_accelData.begin(), m_accelData.end(), item) == m_accelData.end())
				m_accelData.push_back(item);
		}
	}
	EndDialog(wxID_OK);
}
