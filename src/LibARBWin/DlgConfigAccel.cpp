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
#include "fmt/format.h"
#include "LibARBWin/ListCtrl.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/MenuHelper.h"
#include "LibARBWin/Validators.h"
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
		text << (wxChar)code;

	return text;
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
			wxWindow* pParent);

	bool GetData(CMenuHelper::ItemAccel& item);

private:
	DECLARE_ON_INIT()
	void OnKeyDown(wxKeyEvent& evt);
	void OnClear(wxCommandEvent& evt);

	std::vector<CMenuHelper::ItemAccel> const& m_accelData;
	CMenuHelper::ItemAccel m_item;
	std::unordered_map<int, KeyCodeMapping> const& m_keyMap;
	wxTextCtrl* m_ctrlKey;
};

/////////////////////////////////////////////////////////////////////////////

CDlgEditAccel::CDlgEditAccel(
		std::vector<CMenuHelper::ItemAccel> const& accelData,
		CMenuHelper::ItemAccel const& item,
		std::unordered_map<int, KeyCodeMapping> const& keyMap,
		std::wstring const& action,
		wxWindow* pParent)
	: m_accelData(accelData)
	, m_item(item)
	, m_keyMap(keyMap)
	, m_ctrlKey(nullptr)
{
	wxString caption = wxString::Format(L"%s : %s", _("Assign Key"), action.c_str());
	Create(pParent, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	wxStaticText* textKey = new wxStaticText(this, wxID_ANY,
		_("Key:"),
		wxDefaultPosition, wxDefaultSize, 0);

	m_ctrlKey = new wxTextCtrl(this, wxID_ANY, GetKeyCode(m_keyMap, m_item.keyCode),
		wxDefaultPosition, wxDLG_UNIT(this, wxSize(30, -1)), 0);
	BIND_OR_CONNECT_CTRL(m_ctrlKey, wxEVT_KEY_DOWN, wxKeyEventHandler, CDlgEditAccel::OnKeyDown);

	wxStaticText* textMod = new wxStaticText(this, wxID_ANY,
		_("Modifiers:"),
		wxDefaultPosition, wxDefaultSize, 0);

	wxCheckBox* ctrlCtrl = new wxCheckBox(this, wxID_ANY,
		_("Ctrl"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_item.bCtrl));

	wxCheckBox* ctrlAlt = new wxCheckBox(this, wxID_ANY,
		_("Alt"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_item.bAlt));

	wxCheckBox* ctrlShift = new wxCheckBox(this, wxID_ANY,
		_("Shift"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_item.bShift));

	wxButton* ctrlClear = new wxButton(this, wxID_ANY,
		_("Clear"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(ctrlClear, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgEditAccel::OnClear);

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

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	IMPLEMENT_ON_INIT(CDlgEditAccel, m_ctrlKey)
}


DEFINE_ON_INIT(CDlgEditAccel)


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
			int order)
		: m_keyMap(keyMap)
		, m_accels(accels)
		, m_path(path)
		, m_item(item)
		, m_order(order)
	{
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
						fmt::format_to(data, L", ");
					if (m_accels[i].bCtrl)
						fmt::format_to(data, _("Ctrl+").wx_str());
					if (m_accels[i].bAlt)
						fmt::format_to(data, _("Alt+").wx_str());
					if (m_accels[i].bShift)
						fmt::format_to(data, _("Shift+").wx_str());
					fmt::format_to(data, L"{}", GetKeyCode(m_keyMap, m_accels[i].keyCode).wx_str());
				}
				return fmt::to_string(data);
			}
			break;
		case 1:
			return m_path;
		default:
			assert(0);
		}
		return std::wstring();
	}

	std::vector<CMenuHelper::ItemAccel> const& Accels() const { return m_accels; }

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

	bool Configure(
			std::vector<CMenuHelper::ItemAccel> const& accelData,
			bool bAdd,
			wxWindow* pParent)
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

		CDlgEditAccel dlg(accelData, item, m_keyMap, m_item, pParent);
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

private:
	std::unordered_map<int, KeyCodeMapping> const& m_keyMap;
	std::vector<CMenuHelper::ItemAccel> m_accels;
	std::wstring m_path;
	std::wstring m_item;
	int m_order;
};

/////////////////////////////////////////////////////////////////////////////

static int wxCALLBACK CompareItems(CListDataPtr const& item1, CListDataPtr const& item2, SortInfo const* pSortInfo)
{
	assert(pSortInfo);

	CMenuDataPtr pRow1 = std::dynamic_pointer_cast<CMenuData, CListData>(item1);
	CMenuDataPtr pRow2 = std::dynamic_pointer_cast<CMenuData, CListData>(item2);

	if (!pRow1 || !pRow2)
		return 0;
	int iCol = std::abs(pSortInfo->nCol);

	int o1 = pRow1->GetSortOrder(iCol - 1);
	int o2 = pRow2->GetSortOrder(iCol - 1);
	int nRet = 0;
	if (o1 < o2)
		nRet = -1;
	else if (o1 > o2)
		nRet = 1;

	if (0 > pSortInfo->nCol)
		nRet *= -1;
	return nRet;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigAccel::CDlgConfigAccel(
		std::vector<CMenuHelper::ItemAccel> const& accelData,
		std::vector<CMenuHelper::ItemAccel> const& accelDataDefaults,
		CMenuHelper::ItemData const* menuItems,
		size_t numMenuItems,
		std::unordered_map<int, KeyCodeMapping> const& keyMap,
		wxWindow* pParent,
		std::wstring caption)
	: m_accelDataDefaults(accelDataDefaults)
	, m_menuItems()
	, m_accelData(accelData)
	, m_keyMap(keyMap)
	, m_SortColumn(2)
	, m_ctrlItems(nullptr)
	, m_ctrlNew(nullptr)
	, m_ctrlEdit(nullptr)
	, m_ctrlClear(nullptr)
{
	{
		std::vector<std::wstring> path;
		path.push_back(StringUtil::stringW(wxStripMenuCodes(wxGetTranslation(menuItems[0].menu))));

		for (size_t i = 1; i < numMenuItems; ++i)
		{
			while (path.size() > menuItems[i].menuLevel)
				path.pop_back();

			std::wstring menuItem = StringUtil::stringW(wxStripMenuCodes(wxGetTranslation(menuItems[i].menu)));
			path.push_back(menuItem);

			fmt::wmemory_buffer data;
			for (size_t n = 0; n < path.size(); ++n)
			{
				if (0 == n)
					fmt::format_to(data, L"{}", path[n]);
				else
					fmt::format_to(data, L" | {}", path[n]);
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

	m_ctrlItems = new CReportListCtrl(this,
		wxDefaultPosition, wxDLG_UNIT(this, wxSize(200, 150)),
		true, CReportListCtrl::eSortHeader, true, false);
	BIND_OR_CONNECT_CTRL(m_ctrlItems, wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler, CDlgConfigAccel::OnColumnClick);
	BIND_OR_CONNECT_CTRL(m_ctrlItems, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler, CDlgConfigAccel::OnItemSelected);
	BIND_OR_CONNECT_CTRL(m_ctrlItems, wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler, CDlgConfigAccel::OnItemActivated);
	BIND_OR_CONNECT_CTRL(m_ctrlItems, wxEVT_KEY_DOWN, wxKeyEventHandler, CDlgConfigAccel::OnKeyDown);
	m_ctrlItems->InsertColumn(0, _("Keyboard Shortcut"), wxLIST_FORMAT_LEFT);
	m_ctrlItems->InsertColumn(1, _("Description"), wxLIST_FORMAT_LEFT);
	LoadData();
	for (int i = 0; i < m_ctrlItems->GetColumnCount(); ++i)
		m_ctrlItems->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);

	m_ctrlNew = new wxButton(this, wxID_ANY,
		_("New..."),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlNew, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgConfigAccel::OnAddItem);

	m_ctrlEdit = new wxButton(this, wxID_ANY,
		_("Edit..."),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlEdit, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgConfigAccel::OnEditItem);

	m_ctrlClear = new wxButton(this, wxID_ANY,
		_("Clear"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlClear, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgConfigAccel::OnClearItem);

	wxButton* ctrlDefaults = new wxButton(this, wxID_ANY,
		_("Set Defaults"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(ctrlDefaults, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgConfigAccel::OnSetDefaults);

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

	BIND_OR_CONNECT_ID(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgConfigAccel::OnOk);

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
		std::sort(v1.begin(), v1.end(),
				  [](CMenuHelper::ItemAccel const& one, CMenuHelper::ItemAccel const& two)
				  {
					  return one.key < two.key;
				  }
		);
		std::sort(v2.begin(), v2.end(),
				  [](CMenuHelper::ItemAccel const& one, CMenuHelper::ItemAccel const& two)
				  {
					  return one.key < two.key;
				  }
		);
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
	m_ctrlItems->DeleteAllItems();
#ifdef _DEBUG
	std::set<int> inUse;
#endif
	int count = 0;
	for (size_t i = 0; i < m_menuItems.size(); ++i)
	{
		if (m_menuItems[i].m_data->id && !(m_menuItems[i].m_data->flags & MENU_MRU))
		{
#ifdef _DEBUG
			assert(inUse.find(m_menuItems[i].m_data->id) == inUse.end());
			inUse.insert(m_menuItems[i].m_data->id);
#endif

			// Support for multiple accelerators for an id (like copy: Ctrl+C/Ctrl+Ins)
			std::vector<CMenuHelper::ItemAccel> accels;
			for (size_t n = 0; n < m_accelData.size(); ++n)
			{
				if (m_accelData[n].id == m_menuItems[i].m_data->id)
				{
					accels.push_back(m_accelData[n]);
				}
			}
			assert(accels.size() > 0);
				
			m_ctrlItems->InsertItem(std::make_shared<CMenuData>(m_keyMap, accels, m_menuItems[i].m_path, m_menuItems[i].m_item, count++));
		}
	}
#ifdef _DEBUG
	// Sanity checking
	// All the accelerators must be in the main menu (can't get the text otherwise)
	for (CMenuHelper::ItemAccel const& item : m_accelData)
	{
		if (inUse.find(item.id) == inUse.end())
		{
			assert(0); // For now, we don't support independent accel items
			inUse.insert(item.id);
		}
	}
#endif
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
	if (pData->Configure(m_accelData, bAdd, this))
	{
		m_ctrlItems->RefreshItem(idx);
		UpdateControls();
	}
}


void CDlgConfigAccel::OnColumnClick(wxListEvent& evt)
{
	m_ctrlItems->SetColumnSort(std::abs(m_SortColumn) - 1, 0);
	int nBackwards = 1;
	if (m_SortColumn == evt.GetColumn() + 1)
		nBackwards = -1;
	m_SortColumn = (evt.GetColumn() + 1) * nBackwards;
	SortInfo sortInfo(m_SortColumn);
	m_ctrlItems->SortItems(CompareItems, &sortInfo);
	m_ctrlItems->SetColumnSort(std::abs(m_SortColumn) - 1, m_SortColumn);
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
			m_accelData.push_back(item);
		}
	}
	EndDialog(wxID_OK);
}
