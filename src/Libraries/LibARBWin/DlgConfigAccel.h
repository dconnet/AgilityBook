#pragma once

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

#include "LibARBWin/MenuHelper.h"
#include "LibARBWin/ReportListHeader.h"
#include <unordered_map>
#include <vector>
class CReportListCtrl;


struct KeyCodeMapping
{
	int wxCode;             // WXK code
	const wchar_t* special; // wxGetTranslation name
	bool bHasMenuSupport;   // Is special string recognized in menus
};


class CDlgConfigAccel : public wxDialog
{
public:
	CDlgConfigAccel(
		std::unordered_map<int, std::wstring> const& menuIds,
		std::vector<CMenuHelper::ItemAccel> const& accelData,
		std::vector<CMenuHelper::ItemAccel> const& accelDataDefaults,
		bool bAllowDups,
		std::vector<CMenuHelper::ItemData> const& menuItems,
		std::unordered_map<int, KeyCodeMapping> const& keyMap,
		wxWindow* pParent,
		std::wstring caption = std::wstring());

	bool GetAccelData(std::vector<CMenuHelper::ItemAccel>& accelData);

private:
	void LoadData();
	void UpdateControls();
	void DoEdit(bool bAdd);

	void OnItemSelected(wxListEvent& evt);
	void OnItemActivated(wxListEvent& evt);
	void OnKeyDown(wxKeyEvent& evt);
	void OnAddItem(wxCommandEvent& evt);
	void OnEditItem(wxCommandEvent& evt);
	void OnClearItem(wxCommandEvent& evt);
	void OnSetDefaults(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);

	std::unordered_map<int, std::wstring> const& m_menuIds;
	std::vector<CMenuHelper::ItemAccel> const& m_accelDataDefaults;
	bool m_bAllowDups;
	struct MenuData
	{
		CMenuHelper::ItemData const* m_data;
		std::wstring m_path;
		std::wstring m_item;
		MenuData()
			: m_data(nullptr)
			, m_path()
			, m_item()
		{
		}
	};
	std::vector<MenuData> m_menuItems;
	std::vector<CMenuHelper::ItemAccel> m_accelData;
	std::unordered_map<int, KeyCodeMapping> const& m_keyMap;
	CReportListHeader m_reportColumn;
	CReportListCtrl* m_ctrlItems;
	wxButton* m_ctrlNew;
	wxButton* m_ctrlEdit;
	wxButton* m_ctrlClear;
};
