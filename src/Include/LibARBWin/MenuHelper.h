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
 * 2018-10-11 Moved to Win LibARBWin
 * 2013-10-13 Changed to using an art provider.
 * 2012-07-13 Created
 */

#include "LibwxARBWin.h"

#include <vector>
#include <wx/artprov.h>
class wxFrame;

// MENU_SEP/MENU_ITEM/MENU_MRU are mutually exclusive
#define MENU_SEP	0x01	///< Menu separator
#define MENU_ITEM	0x02	///< Menu item
#define MENU_MRU	0x04	///< MRU item, can only be one
#define MENU_HELP	0x08	///< Top-level Help item, can only be one


class ARBWIN_API CMenuHelper
{
	DECLARE_NO_COPY_IMPLEMENTED(CMenuHelper)
public:
	struct ItemData
	{
		int menuId;				///< Menu id, 0 is main menu. all others are program defined
		unsigned int flags;		///< Above defines
		int id;					///< 0: Separator or Menu root
		wxItemKind kind;		///< Check/radio/normal button (or menu)
		size_t menuLevel;		///< Menu indent level, not used in toolbar
		wchar_t const* toolbar;	///< Toolbar text
		wchar_t const* menu;	///< Menu text
		wchar_t const* help;	///< Tooltip help (and status help)
		char const* artId;		///< Toolbar/menu bitmap artid (wxArtId/wxString)
		ItemData()
			: flags(0)
			, id(0)
			, kind(wxITEM_SEPARATOR)
			, menuLevel(0)
			, toolbar(nullptr)
			, menu(nullptr)
			, help(nullptr)
			, artId(nullptr)
		{
		}
		ItemData(int _menuId, unsigned int _flags, int _id, wxItemKind _kind, size_t _menuLevel, wchar_t const* _toolbar, wchar_t const* _menu, wchar_t const* _help, char const* _artId)
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
		int key; // Unique value, must remain stable since it's stored
		int id; // menu id, not stored (values could change)
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
		ItemAccel(ItemAccel const& rhs)
			: key(rhs.key)
			, id(rhs.id)
			, keyCode(rhs.keyCode)
			, bCtrl(rhs.bCtrl)
			, bAlt(rhs.bAlt)
			, bShift(rhs.bShift)
		{
		}
		ItemAccel(int _key, int _id, bool _bCtrl, bool _bAlt, bool _bShift, int _keyCode)
			: key(_key)
			, id(_id)
			, bCtrl(_bCtrl)
			, bAlt(_bAlt)
			, bShift(_bShift)
			, keyCode(_keyCode)
		{
		}
		bool operator==(ItemAccel const& rhs) const
		{
			return key == rhs.key
				&& id == rhs.id
				&& keyCode == rhs.keyCode
				&& bCtrl == rhs.bCtrl
				&& bAlt == rhs.bAlt
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

	CMenuHelper();
	~CMenuHelper();

	// The default accel list must contain all possible accelerators.
	void LoadAccelerators(
			ItemAccel const defAccelItems[],
			size_t numDefAccelItems);

	void SaveAccelerators();

	// If pParent is null, the parent window is the frame
	bool ConfigureAccelerators(
			wxFrame* pFrame,
			ItemData const menuItems[],
			size_t numMenuItems,
			wxWindow* pParent = nullptr);

	void CreateMenu(
			wxFrame* pFrame,
			ItemData const menuItems[],
			size_t numMenuItems,
			bool doTranslation,
			wxMenu* mruMenu = nullptr);

	// Toolbar array is the 'id' in ItemData, use 0 for separator.
	void CreateMenu(
			wxFrame* pFrame,
			ItemData const menuItems[],
			size_t numMenuItems,
			int const toolbarItems[],
			size_t numToolbarItems,
			bool doTranslation,
			wxMenu* mruMenu = nullptr);

	wxMenu* CreatePopupMenu(
			wxWindow* pWindow,
			int menuId,
			ItemData const items[],
			size_t numItems);

	// Note: Before calling this, make sure to call LoadAccelerators
	void UpdateMenu();

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

	long ToBitmask(ItemAccel const& accel);
	void FromBitmask(long mask, ItemAccel& accel);
	wxString GetAccelString(std::vector<ItemAccel> const& accelData, int id);
	int TranslateId(
			int id,
			ItemAccel const defAccelItems[], 
			size_t numDefAccelItems);
	void CreateAccelTable(wxFrame* pFrame);

	void Menu(
			wxWindow* pWindow,
			int menuId,
			MenuHandle& handle,
			size_t& index,
			size_t level,
			wxMenu* mruMenu,
			bool& mruAdded,
			ItemData const items[],
			size_t numItems);

	void DoSubMenu(wxMenu* parent, MenuHandle const& handle);

	wxFrame* m_Frame;
	wxMenuBar* m_MenuBar;
	bool m_doTranslation;
	std::vector<MenuHandle> m_MenuData;
	std::vector<TranslationData> m_ToolbarData;
	bool m_bModified;
	std::vector<ItemAccel> m_accelDataDefaults;
	std::vector<ItemAccel> m_accelData;
};
