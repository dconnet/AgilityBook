#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Data-driven helper to create menus and toolbar items.
 * @author David Connet
 *
 * Revision History
 * @li 2012-07-13 DRC Created
 */

#include <vector>
class wxFrame;


// MENU_SEP/MENU_ITEM/MENU_MRU are mutually exclusive
#define MENU_SEP	0x01	///< Menu separator
#define MENU_ITEM	0x02	///< Menu item
#define MENU_MRU	0x04	///< MRU item, can only be one
#define MENU_HELP	0x08	///< Top-level Help item, can only be one

class CMenuHelper
{
public:
	struct ItemData
	{
		unsigned int flags;		///< Above defines
		int id;					///< 0: Separator or Menu root
		wxItemKind kind;		///< Check/radio/normal button (or menu)
		size_t menuLevel;		///< Menu indent level, not used in toolbar
		wchar_t const* toolbar;	///< Toolbar text
		wchar_t const* menu;	///< Menu text
		wchar_t const* help;	///< Tooltip help (and status help)
		const char** bitmap;	///< Toolbar/menu bitmap
	};

	CMenuHelper();
	~CMenuHelper();

	void CreateMenu(
			wxFrame* pFrame,
			ItemData const menuItems[],
			size_t numMenuItems,
			bool doTranslation,
			wxMenu* mruMenu = NULL);

	// Toolbar array is the 'id' in ItemData, use 0 for separator.
	void CreateMenu(
			wxFrame* pFrame,
			ItemData const menuItems[],
			size_t numMenuItems,
			int const toolbarItems[],
			size_t numToolbarItems,
			bool doTranslation,
			wxMenu* mruMenu = NULL);

	void UpdateMenu();

	static void DoMenuItem(
			wxMenu* menu,
			int id,
			wxString const& label,
			wxString const& desc,
			const char* const* pBitmap = NULL);

	static void DoMenuItem(
			wxMenu* menu,
			int id,
			wxString const& label,
			wxString const& desc,
			wxItemKind kind,
			wxMenu* subMenu,
			const char* const* pBitmap);

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
		{
		}
		MenuHandle(wxMenu* menu, int index)
			: pMenu(menu)
			, idx(index)
		{
		}
	};

	void Menu(
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
};
