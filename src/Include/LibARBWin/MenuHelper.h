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
 * 2021-01-02 Added VerifyMenuShortcuts
 * 2018-10-11 Moved to Win LibARBWin
 * 2013-10-13 Changed to using an art provider.
 * 2012-07-13 Created
 */

#include "LibwxARBWin.h"

#include <wx/artprov.h>
#include <unordered_map>
#include <vector>
class wxFrame;

// MENU_SEP/MENU_ITEM/MENU_MRU are mutually exclusive
#define MENU_SEP  0x01 ///< Menu separator
#define MENU_ITEM 0x02 ///< Menu item
#define MENU_MRU  0x04 ///< MRU item, can only be one
#define MENU_HELP 0x08 ///< Top-level Help item, can only be one


class ARBWIN_API CMenuHelper
{
	DECLARE_NO_COPY_IMPLEMENTED(CMenuHelper)
public:
	struct ItemData
	{
		// When reading the array of ItemData, it is assumed that all items of
		// a given menuId are grouped together.
		int menuId;             ///< Menu id, 0 is main menu. all others are program defined
		unsigned int flags;     ///< Above defines
		int id;                 ///< 0: Separator or Menu root
		wxItemKind kind;        ///< Check/radio/normal button (or menu)
		size_t menuLevel;       ///< Menu indent level, not used in toolbar
		wchar_t const* toolbar; ///< Toolbar text
		wchar_t const* menu;    ///< Menu text
		wchar_t const* help;    ///< Tooltip help (and status help)
		wxArtID const artId;    ///< Toolbar/menu bitmap artid
		ItemData()
			: menuId(0)
			, flags(0)
			, id(0)
			, kind(wxITEM_SEPARATOR)
			, menuLevel(0)
			, toolbar(nullptr)
			, menu(nullptr)
			, help(nullptr)
			, artId()
		{
		}
		ItemData(
			int _menuId,
			unsigned int _flags,
			int _id,
			wxItemKind _kind,
			size_t _menuLevel,
			wchar_t const* _toolbar,
			wchar_t const* _menu,
			wchar_t const* _help,
			char const* _artId)
			: menuId(_menuId)
			, flags(_flags)
			, id(_id)
			, kind(_kind)
			, menuLevel(_menuLevel)
			, toolbar(_toolbar)
			, menu(_menu)
			, help(_help)
			, artId(_artId)
		{
		}
		ItemData(
			int _menuId,
			unsigned int _flags,
			int _id,
			wxItemKind _kind,
			size_t _menuLevel,
			wchar_t const* _toolbar,
			wchar_t const* _menu,
			wchar_t const* _help,
			wxString const& _artId)
			: menuId(_menuId)
			, flags(_flags)
			, id(_id)
			, kind(_kind)
			, menuLevel(_menuLevel)
			, toolbar(_toolbar)
			, menu(_menu)
			, help(_help)
			, artId(_artId)
		{
		}
	};

	struct ItemAccel
	{
		int key;     // Unique value, must remain stable since it's stored
		int id;      // menu id, not stored (values could change)
		int keyCode; // wxKeyCode: if 0, not set
		bool bCtrl;
		bool bAlt;
		bool bShift;
		ItemAccel()
			: key(0)
			, id(0)
			, keyCode(0)
			, bCtrl(false)
			, bAlt(false)
			, bShift(false)
		{
		}
		ItemAccel(ItemAccel const& rhs) = default;
		ItemAccel(ItemAccel&& rhs) = default;
		ItemAccel(int _key, int _id, bool _bCtrl, bool _bAlt, bool _bShift, int _keyCode)
			: key(_key)
			, id(_id)
			, keyCode(_keyCode)
			, bCtrl(_bCtrl)
			, bAlt(_bAlt)
			, bShift(_bShift)
		{
		}
		ItemAccel& operator=(ItemAccel const& rhs) = default;
		ItemAccel& operator=(ItemAccel&& rhs) = default;
		bool operator==(ItemAccel const& rhs) const
		{
			return key == rhs.key && id == rhs.id && keyCode == rhs.keyCode && bCtrl == rhs.bCtrl && bAlt == rhs.bAlt
				   && bShift == rhs.bShift;
		}
		bool operator!=(ItemAccel const& rhs) const
		{
			return !operator==(rhs);
		}
		void Clear()
		{
			keyCode = 0;
			bCtrl = false;
			bAlt = false;
			bShift = false;
		}
	};

	// The default accel list must contain all possible accelerators.
	// Toolbar array is the 'id' in ItemData, use 0 for separator.
	// bAllowDups: Allow an accelerator to be duped to different IDs.
	// It's up to the user context to determine what to do...
	CMenuHelper(
		std::vector<CMenuHelper::ItemData> const& menuItems,
		std::vector<CMenuHelper::ItemAccel> const& accels,
		std::vector<int> const& toolbarItems,
		std::unordered_map<int, std::wstring> const& menuIds,
		bool doTranslation = false,
		bool bAllowDups = false); // Accelerator dups
	~CMenuHelper();

	static long ToBitmask(ItemAccel const& accel);
	static void FromBitmask(long mask, ItemAccel& accel);

	void LoadAccelerators();
	void SaveAccelerators();

	// If pParent is null, the parent window is the frame
	bool ConfigureAccelerators(wxFrame* pFrame, wxWindow* pParent = nullptr);

	void CreateMenu(wxFrame* pFrame, wxMenu* mruMenu = nullptr);

	wxMenu* CreatePopupMenu(wxWindow* pWindow, int menuId);

	void UpdateMenu(bool bLoadAccelerators = true);

protected:
	// Verify menu shortcuts (&-prefixed letters) are unique within a menu.
	// Returns true if dups are found.
	// This will only work in DEBUG builds and will print via wxLogDebug.
	bool VerifyMenuShortcuts(wxMenu* menu, int level = 0);

	void DoMenuItem(
		wxWindow* pWindow,
		wxMenu* menu,
		int id,
		wxString const& label,
		wxString const& desc,
		wxArtID const& artId = wxArtID());

	void DoMenuItem(
		wxWindow* pWindow,
		wxMenu* menu,
		int id,
		wxString const& label,
		wxString const& desc,
		wxItemKind kind,
		wxMenu* subMenu,
		wxArtID const& artId = wxArtID());

private:
	struct TranslationData
	{
		int id;
		std::wstring name;
		std::wstring desc;
		TranslationData()
			: id(wxID_ANY)
			, name()
			, desc()
		{
		}
		TranslationData(wchar_t const* menu)
			: id(wxID_ANY)
			, name(menu)
			, desc()
		{
		}
		TranslationData(int i, wchar_t const* item, wchar_t const* help)
			: id(i)
			, name(item)
			, desc(help)
		{
		}
	};

	struct MenuHandle
	{
		wxMenu* pMenu;
		int idx; // Index in parent
		std::wstring item;
		std::vector<TranslationData> items;
		std::vector<MenuHandle> subMenus;
		MenuHandle(int index)
			: pMenu(new wxMenu())
			, idx(index)
			, item()
			, items()
			, subMenus()
		{
		}
		MenuHandle(wxMenu* menu, int index)
			: pMenu(menu)
			, idx(index)
			, item()
			, items()
			, subMenus()
		{
		}
	};

	wxString GetAccelString(std::vector<ItemAccel> const& accelData, int id);
	int TranslateId(int id, std::vector<ItemAccel> const& defAccelItems);
	void CreateAccelTable(wxFrame* pFrame);

	void Menu(
		wxWindow* pWindow,
		int menuId,
		MenuHandle& handle,
		size_t& index,
		size_t level,
		wxMenu* mruMenu,
		bool& mruAdded,
		bool isPopup);

	void DoSubMenu(wxMenu const* parent, MenuHandle const& handle);

	std::vector<CMenuHelper::ItemData> const& m_menuItems;
	std::vector<ItemAccel> const& m_accelDataDefaults;
	std::vector<int> const& m_toolbarItems;
	std::unordered_map<int, std::wstring> m_menuIds;

	wxFrame* m_Frame;
	wxMenuBar* m_MenuBar;
	bool m_doTranslation;
	std::vector<MenuHandle> m_MenuData;
	std::vector<TranslationData> m_ToolbarData;
	bool m_bModified;
	std::vector<ItemAccel> m_accelData;
	bool m_bAllowDups;
};
