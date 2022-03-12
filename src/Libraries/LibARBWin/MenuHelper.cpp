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
 * 2016-02-17 Create a disabled image for toolbar. wx3.1 images are too dark.
 * 2015-12-23 Fix a bug when a separator followed submenu.
 * 2013-10-13 Changed to using an art provider.
 * 2012-07-28 Fix About menu insertion.
 * 2012-07-13 Created
 */

#include "stdafx.h"
#include "LibARBWin/MenuHelper.h"

#include "DlgConfigAccel.h"

#include "ARBCommon/ARBUtils.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ImageHelperBase.h"
#include "LibARBWin/RegItemsBase.h"
#include <wx/artprov.h>
#include <wx/config.h>
#include <wx/frame.h>
#include <map>
#include <set>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

namespace
{
// See wxMenuItem::SetItemLabel() for set of special keys
std::unordered_map<int, KeyCodeMapping> const& GetKeyCodes()
{
	static std::unordered_map<int, KeyCodeMapping> s_codeMapping = {
		{WXK_F1, {WXK_F1, L"F1", true}},
		{WXK_F2, {WXK_F2, L"F2", true}},
		{WXK_F3, {WXK_F3, L"F3", true}},
		{WXK_F4, {WXK_F4, L"F4", true}},
		{WXK_F5, {WXK_F5, L"F5", true}},
		{WXK_F6, {WXK_F6, L"F6", true}},
		{WXK_F7, {WXK_F7, L"F7", true}},
		{WXK_F8, {WXK_F8, L"F8", true}},
		{WXK_F9, {WXK_F9, L"F9", true}},
		{WXK_F10, {WXK_F10, L"F10", true}},
		{WXK_F11, {WXK_F11, L"F11", true}},
		{WXK_F12, {WXK_F12, L"F12", true}},
		{WXK_DELETE, {WXK_DELETE, L"Del", true}},
		{WXK_DELETE, {WXK_DELETE, L"Delete", true}},
		{WXK_BACK, {WXK_BACK, L"Back", true}},
		{WXK_INSERT, {WXK_INSERT, L"Ins", true}},
		{WXK_INSERT, {WXK_INSERT, L"Insert", true}},
		{WXK_RETURN, {WXK_RETURN, L"Enter", true}},
		{WXK_RETURN, {WXK_RETURN, L"Return", true}},
		{WXK_PAGEUP, {WXK_PAGEUP, L"PgUp", true}},
		{WXK_PAGEUP, {WXK_PAGEUP, L"PageUp", true}},
		{WXK_PAGEDOWN, {WXK_PAGEDOWN, L"PgDn", true}},
		{WXK_PAGEDOWN, {WXK_PAGEDOWN, L"PageDn", true}},
		{WXK_LEFT, {WXK_LEFT, L"Left", true}},
		{WXK_RIGHT, {WXK_RIGHT, L"Right", true}},
		{WXK_UP, {WXK_UP, L"Up", true}},
		{WXK_DOWN, {WXK_DOWN, L"Down", true}},
		{WXK_HOME, {WXK_HOME, L"Home", true}},
		{WXK_END, {WXK_END, L"End", true}},
		{WXK_SPACE, {WXK_SPACE, L"Space", true}},
		{WXK_TAB, {WXK_TAB, L"Tab", true}},
		{WXK_ESCAPE, {WXK_ESCAPE, L"Esc", true}},
		{WXK_ESCAPE, {WXK_ESCAPE, L"Escape", true}},
		{WXK_CANCEL, {WXK_CANCEL, L"Cancel", true}},
		{WXK_CLEAR, {WXK_CLEAR, L"Clear", true}},
		{WXK_MENU, {WXK_MENU, L"Menu", true}},
		{WXK_PAUSE, {WXK_PAUSE, L"Pause", true}},
		{WXK_CAPITAL, {WXK_CAPITAL, L"Capital", true}},
		{WXK_SELECT, {WXK_SELECT, L"Select", true}},
		{WXK_PRINT, {WXK_PRINT, L"Print", true}},
		{WXK_EXECUTE, {WXK_EXECUTE, L"Execute", true}},
		{WXK_SNAPSHOT, {WXK_SNAPSHOT, L"Snapshot", true}},
		{WXK_HELP, {WXK_HELP, L"Help", true}},
		{WXK_ADD, {WXK_ADD, L"Add", true}},
		{WXK_SEPARATOR, {WXK_SEPARATOR, L"Separator", true}},
		{WXK_SUBTRACT, {WXK_SUBTRACT, L"Subtract", true}},
		{WXK_DECIMAL, {WXK_DECIMAL, L"Decimal", true}},
		{WXK_DIVIDE, {WXK_DIVIDE, L"Divide", true}},
		{WXK_NUMLOCK, {WXK_NUMLOCK, L"Num_lock", true}},
		{WXK_SCROLL, {WXK_SCROLL, L"Scroll_lock", true}},
		{WXK_NUMPAD_SPACE, {WXK_NUMPAD_SPACE, L"KP_Space", true}},
		{WXK_NUMPAD_TAB, {WXK_NUMPAD_TAB, L"KP_Tab", true}},
		{WXK_NUMPAD_ENTER, {WXK_NUMPAD_ENTER, L"KP_Enter", true}},
		{WXK_NUMPAD_HOME, {WXK_NUMPAD_HOME, L"KP_Home", true}},
		{WXK_NUMPAD_LEFT, {WXK_NUMPAD_LEFT, L"KP_Left", true}},
		{WXK_NUMPAD_UP, {WXK_NUMPAD_UP, L"KP_Up", true}},
		{WXK_NUMPAD_RIGHT, {WXK_NUMPAD_RIGHT, L"KP_Right", true}},
		{WXK_NUMPAD_DOWN, {WXK_NUMPAD_DOWN, L"KP_Down", true}},
		{WXK_NUMPAD_PAGEUP, {WXK_NUMPAD_PAGEUP, L"KP_PageUp", true}},
		{WXK_NUMPAD_PAGEDOWN, {WXK_NUMPAD_PAGEDOWN, L"KP_PageDown", true}},
		{WXK_NUMPAD_PAGEUP, {WXK_NUMPAD_PAGEUP, L"KP_Prior", true}},
		{WXK_NUMPAD_PAGEDOWN, {WXK_NUMPAD_PAGEDOWN, L"KP_Next", true}},
		{WXK_NUMPAD_END, {WXK_NUMPAD_END, L"KP_End", true}},
		{WXK_NUMPAD_BEGIN, {WXK_NUMPAD_BEGIN, L"KP_Begin", true}},
		{WXK_NUMPAD_INSERT, {WXK_NUMPAD_INSERT, L"KP_Insert", true}},
		{WXK_NUMPAD_DELETE, {WXK_NUMPAD_DELETE, L"KP_Delete", true}},
		{WXK_NUMPAD_EQUAL, {WXK_NUMPAD_EQUAL, L"KP_Equal", true}},
		{WXK_NUMPAD_MULTIPLY, {WXK_NUMPAD_MULTIPLY, L"KP_Multiply", true}},
		{WXK_NUMPAD_ADD, {WXK_NUMPAD_ADD, L"KP_Add", true}},
		{WXK_NUMPAD_SEPARATOR, {WXK_NUMPAD_SEPARATOR, L"KP_Separator", true}},
		{WXK_NUMPAD_SUBTRACT, {WXK_NUMPAD_SUBTRACT, L"KP_Subtract", true}},
		{WXK_NUMPAD_DECIMAL, {WXK_NUMPAD_DECIMAL, L"KP_Decimal", true}},
		{WXK_NUMPAD_DIVIDE, {WXK_NUMPAD_DIVIDE, L"KP_Divide", true}},
		{WXK_WINDOWS_LEFT, {WXK_WINDOWS_LEFT, L"Windows_Left", true}},
		{WXK_WINDOWS_RIGHT, {WXK_WINDOWS_RIGHT, L"Windows_Right", true}},
		{WXK_WINDOWS_MENU, {WXK_WINDOWS_MENU, L"Windows_Menu", true}},
		{WXK_NUMPAD0, {WXK_NUMPAD0, L"KP_0", true}}, // Note: wx po file has "KP_" only
		{WXK_NUMPAD1, {WXK_NUMPAD1, L"KP_1", true}},
		{WXK_NUMPAD2, {WXK_NUMPAD2, L"KP_2", true}},
		{WXK_NUMPAD3, {WXK_NUMPAD3, L"KP_3", true}},
		{WXK_NUMPAD4, {WXK_NUMPAD4, L"KP_4", true}},
		{WXK_NUMPAD5, {WXK_NUMPAD5, L"KP_5", true}},
		{WXK_NUMPAD6, {WXK_NUMPAD6, L"KP_6", true}},
		{WXK_NUMPAD7, {WXK_NUMPAD7, L"KP_7", true}},
		{WXK_NUMPAD8, {WXK_NUMPAD8, L"KP_8", true}},
		{WXK_NUMPAD9, {WXK_NUMPAD9, L"KP_9", true}},
		{WXK_SPECIAL1, {WXK_SPECIAL1, L"SPECIAL1", false}}, // Note: wx po file has "SPECIAL" only
		{WXK_SPECIAL2, {WXK_SPECIAL2, L"SPECIAL2", false}},
		{WXK_SPECIAL3, {WXK_SPECIAL3, L"SPECIAL3", false}},
		{WXK_SPECIAL4, {WXK_SPECIAL4, L"SPECIAL4", false}},
		{WXK_SPECIAL5, {WXK_SPECIAL5, L"SPECIAL5", false}},
		{WXK_SPECIAL6, {WXK_SPECIAL6, L"SPECIAL6", false}},
		{WXK_SPECIAL7, {WXK_SPECIAL7, L"SPECIAL7", false}},
		{WXK_SPECIAL8, {WXK_SPECIAL8, L"SPECIAL8", false}},
		{WXK_SPECIAL9, {WXK_SPECIAL9, L"SPECIAL9", false}},
		{WXK_SPECIAL10, {WXK_SPECIAL10, L"SPECIAL10", false}},
		{WXK_SPECIAL11, {WXK_SPECIAL11, L"SPECIAL11", false}},
		{WXK_SPECIAL12, {WXK_SPECIAL12, L"SPECIAL12", false}},
		{WXK_SPECIAL13, {WXK_SPECIAL13, L"SPECIAL13", false}},
		{WXK_SPECIAL14, {WXK_SPECIAL14, L"SPECIAL14", false}},
		{WXK_SPECIAL15, {WXK_SPECIAL15, L"SPECIAL15", false}},
		{WXK_SPECIAL16, {WXK_SPECIAL16, L"SPECIAL16", false}},
		{WXK_SPECIAL17, {WXK_SPECIAL17, L"SPECIAL17", false}},
		{WXK_SPECIAL18, {WXK_SPECIAL18, L"SPECIAL18", false}},
		{WXK_SPECIAL19, {WXK_SPECIAL19, L"SPECIAL19", false}},
		{WXK_SPECIAL20, {WXK_SPECIAL20, L"SPECIAL20", false}},
		{WXK_BROWSER_BACK, {WXK_BROWSER_BACK, L"BrowserBack", false}}, // Not in wx po (not a supported menu option)
		{WXK_BROWSER_FORWARD, {WXK_BROWSER_FORWARD, L"BrowserForward", false}},
		{WXK_BROWSER_REFRESH, {WXK_BROWSER_REFRESH, L"BrowserRefresh", false}},
		{WXK_BROWSER_STOP, {WXK_BROWSER_STOP, L"BrowserStop", false}},
		{WXK_BROWSER_SEARCH, {WXK_BROWSER_SEARCH, L"BrowserSearch", false}},
		{WXK_BROWSER_FAVORITES, {WXK_BROWSER_FAVORITES, L"BrowserFav", false}},
		{WXK_BROWSER_HOME, {WXK_BROWSER_HOME, L"BrowserHome", false}},
		{WXK_VOLUME_MUTE, {WXK_VOLUME_MUTE, L"VolumeMute", false}},
		{WXK_VOLUME_DOWN, {WXK_VOLUME_DOWN, L"VolumeDown", false}},
		{WXK_VOLUME_UP, {WXK_VOLUME_UP, L"VolumeUp", false}},
		{WXK_MEDIA_NEXT_TRACK, {WXK_MEDIA_NEXT_TRACK, L"MediaNext", false}},
		{WXK_MEDIA_PREV_TRACK, {WXK_MEDIA_PREV_TRACK, L"MediaPrev", false}},
		{WXK_MEDIA_STOP, {WXK_MEDIA_STOP, L"MediaStop", false}},
		{WXK_MEDIA_PLAY_PAUSE, {WXK_MEDIA_PLAY_PAUSE, L"MediaPlay", false}},
		{WXK_LAUNCH_MAIL, {WXK_LAUNCH_MAIL, L"LaunchMail", false}},
		{WXK_LAUNCH_APP1, {WXK_LAUNCH_APP1, L"LaunchApp1", false}},
		{WXK_LAUNCH_APP2, {WXK_LAUNCH_APP2, L"LaunchApp2", false}},
	};
	return s_codeMapping;
};

int SpecialToCode(wxString const& special)
{
	if (special.empty())
		return 0;

	for (auto iter : GetKeyCodes())
	{
		if (special == iter.second.special)
			return iter.second.wxCode;
	}
	long val = 0;
	if (special.ToCLong(&val))
		return val;
	return 0;
}

wxString CodeToSpecial(int code, bool bNonMenu)
{
	auto codes = GetKeyCodes();
	auto key = codes.find(code);

	if (key != codes.end())
	{
		if (bNonMenu || (!bNonMenu && key->second.bHasMenuSupport))
			return key->second.special;
	}

	// Note: This does not mean it's not a key - just that the menu doesn't
	// support it - such as VolumeUp/etc. (and we don't list it above)
	wxString special;
	if (bNonMenu)
		special = wxString::Format(L"%d", code);
	return special;
}

// On Windows, we can never see the mnemonics in a popup menu (because you have
// to invoke it with the ALT key to start). So rather than have some unknown
// value, remove them. In debug, we assert because we combine some of the main
// menu items, so trying to make them unique is not possible since now they
// would be different in different menus. That's just wrong. When stripped,
// Windows (don't know about other platforms) just uses the first letter as
// the mnemonic shortcut.
wxString StripMnemonics(wxString const& str, bool isPopup)
{
	if (!isPopup)
		return str;

	wxString s(str);
	size_t found = 0;
	while ((size_t)-1 != (found = s.find(L"&", found)))
	{
		// If there's a "&&", skip it.
		if (found + 1 < s.length() && s[found + 1] == '&')
			found += 2;
		else
			s.erase(found, 1);
	}
	return s;
}
} // namespace

/////////////////////////////////////////////////////////////////////////////

CMenuHelper::CMenuHelper(
	std::vector<CMenuHelper::ItemData> const& menuItems,
	std::vector<CMenuHelper::ItemAccel> const& accels,
	std::vector<int> const& toolbarItems,
	std::unordered_map<int, std::wstring> const& menuIds,
	bool doTranslation,
	bool bAllowDups)
	: m_menuItems(menuItems)
	, m_accelDataDefaults(accels)
	, m_toolbarItems(toolbarItems)
	, m_menuIds(menuIds)
	, m_Frame(nullptr)
	, m_MenuBar(nullptr)
	, m_doTranslation(doTranslation)
	, m_MenuData()
	, m_ToolbarData()
	, m_bModified(false)
	, m_accelData()
	, m_bAllowDups(bAllowDups)
{
}


CMenuHelper::~CMenuHelper()
{
}


long CMenuHelper::ToBitmask(ItemAccel const& accel)
{
	long modifiers = 0;
	if (accel.bAlt)
		modifiers |= 0x1;
	if (accel.bCtrl)
		modifiers |= 0x2;
	if (accel.bShift)
		modifiers |= 0x4;
	return modifiers;
}


void CMenuHelper::FromBitmask(long mask, ItemAccel& accel)
{
	accel.bAlt = (mask & 0x1);
	accel.bCtrl = (mask & 0x2);
	accel.bShift = (mask & 0x4);
}


void CMenuHelper::LoadAccelerators()
{
	m_accelData = m_accelDataDefaults;

	if (wxConfig::Get()->HasGroup(CFG_KEY_ACCELERATORS))
	{
		for (ItemAccel& item : m_accelData)
			item.Clear();

		CConfigPathHelper config(CFG_KEY_ACCELERATORS);
		wxString entry;
		long index = 0;
		if (wxConfig::Get()->GetFirstGroup(entry, index))
		{
			do
			{
				CConfigPathHelper configEntry(entry);
				int key = 0;
				key = wxConfig::Get()->Read(L"id", key);
				if (key)
				{
					for (ItemAccel& item : m_accelData)
					{
						if (item.key == key)
						{
							item.id = TranslateId(key, m_accelDataDefaults);
							item.keyCode = SpecialToCode(wxConfig::Get()->Read(L"KeyCode", wxString()));
							if (item.id && item.keyCode)
							{
								long mask = 0;
								wxConfig::Get()->Read(L"mod", &mask);
								FromBitmask(mask, item);
							}
							break;
						}
					}
				}
			} while (wxConfig::Get()->GetNextGroup(entry, index));
		}
	}

	// Sanity checking
	std::set<int> ids;
	std::set<std::pair<int, long>> accels;
	for (auto iter = m_accelData.begin(); iter != m_accelData.end(); ++iter)
	{
		// Must have a key
		assert(iter->key);
		assert(iter->id);
		// Check that we know how to translate wxKeyCode to string
		// assert(!GetAccelString(m_accelData, iter->id).empty());
		// Actually, don't. Otherwise I can't support things like BrowserBack.

		// Make sure id string is unique
		// Note: do not check id - ctrl+c/ctrl+ins can both be copy
		assert(ids.find(iter->key) == ids.end());
		ids.insert(iter->key);
		// Note: Do not check id/accel unique. While it may not make sense,
		// the user could be using the same accel in different contexts.
		// If both are enabled in the same context, it's up to the system...
		if (!m_bAllowDups && iter->keyCode != 0)
		{
			long mask = ToBitmask(*iter);
			auto accel = std::make_pair(iter->keyCode, mask);
			if (accels.find(accel) != accels.end())
			{
				assert(!"Accelerator in use!");
				iter->Clear(); // Fix bad registry data
			}
			else
				accels.insert(accel);
		}
	}
}


void CMenuHelper::SaveAccelerators()
{
	if (m_bModified)
	{
		// Clear existing
		wxConfig::Get()->DeleteGroup(CFG_KEY_ACCELERATORS);

		if (m_accelDataDefaults != m_accelData)
		{
			CConfigPathHelper config(CFG_KEY_ACCELERATORS);

			// Save
			int nKey = 1;
			for (auto iter = m_accelData.begin(); iter != m_accelData.end(); ++iter)
			{
				if (!iter->keyCode)
					continue;
				wxString key = wxString::Format(L"Item%d", nKey++);
				CConfigPathHelper configKey(key);
				wxString special = CodeToSpecial(iter->keyCode, true);
				assert(!special.empty());
				wxConfig::Get()->Write(L"KeyCode", special);
				wxConfig::Get()->Write(L"id", iter->key);
				wxConfig::Get()->Write(L"mod", ToBitmask(*iter));
			}
			// All were cleared. Need to force an empty one or we default back.
			if (1 == nKey)
			{
				wxString key = wxString::Format(L"Item%d", nKey++);
				CConfigPathHelper configKey(key);
				wxConfig::Get()->Write(L"id", 1);
			}
		}
		m_bModified = false;
	}
}


bool CMenuHelper::ConfigureAccelerators(wxFrame* pFrame, wxWindow* pParent)
{
	assert(pFrame);
	if (!pParent)
		pParent = pFrame;

	bool rc = false;
	CDlgConfigAccel dlg(m_menuIds, m_accelData, m_accelDataDefaults, m_bAllowDups, m_menuItems, GetKeyCodes(), pParent);
	if (wxID_OK == dlg.ShowModal())
	{
		if (dlg.GetAccelData(m_accelData))
		{
			m_bModified = true;
			SaveAccelerators();
			rc = true;
			CreateAccelTable(pFrame);
			UpdateMenu(false);
		}
	}
	return rc;
}


static size_t FindItem(int id, std::vector<CMenuHelper::ItemData> const& menuItems)
{
	for (size_t i = 0; i < menuItems.size(); ++i)
	{
		if (menuItems[i].id == id)
			return i;
	}
	return menuItems.size();
}


void CMenuHelper::CreateMenu(wxFrame* pFrame, wxMenu* mruMenu)
{
	// Load accelerators first. Not all of these are necessarily in the main menu.
	// This ensures all accelerators are registered.
	LoadAccelerators();
	CreateAccelTable(pFrame);

#if defined(_DEBUG) || defined(__WXDEBUG__)
	// Sanity checking
	// Menu id must be >=0
	for (size_t i = 0; i < m_menuItems.size(); ++i)
	{
		assert(m_menuItems[i].menuId >= 0);
	}
	// All the accelerators must be in the main menu (can't get the text otherwise)
	for (auto iter = m_accelData.begin(); iter != m_accelData.end(); ++iter)
	{
		bool bFound = false;
		for (size_t i = 0; !bFound && i < m_menuItems.size(); ++i)
		{
			if ((*iter).id == m_menuItems[i].id)
			{
				bFound = true;
				break;
			}
		}
		assert(bFound);
	}
#endif

	assert(!m_MenuBar);
	m_Frame = pFrame;

	if (0 < m_menuItems.size())
	{
		m_MenuBar = new wxMenuBar();
		bool mruAdded = false;

		for (size_t index = 0; index < m_menuItems.size();)
		{
			if (0 != m_menuItems[index].menuId || !((MENU_ITEM | MENU_HELP) & m_menuItems[index].flags))
			{
				++index;
				continue;
			}
			assert(m_menuItems[index].menu);
			size_t idxMenu = index;
			MenuHandle handle(static_cast<int>(m_MenuBar->GetMenuCount()));
			Menu(pFrame, 0, handle, index, 1, mruMenu, mruAdded, false);
			wxString name;
			if (m_doTranslation)
			{
				name = wxGetTranslation(m_menuItems[idxMenu].menu);
				handle.item = m_menuItems[idxMenu].menu;
			}
			else
			{
				name = m_menuItems[idxMenu].menu;
			}
#ifdef __WXMAC__
			if (MENU_HELP & m_menuItems[index].flags)
			{
				wxApp::s_macHelpMenuTitleName = name;
			}
#endif
			m_MenuBar->Append(handle.pMenu, name);
			m_MenuData.push_back(handle);
			VerifyMenuShortcuts(handle.pMenu);
		}

		if (mruMenu)
		{
			assert(mruAdded);
		}

		m_Frame->SetMenuBar(m_MenuBar);
	}

	if (0 < m_menuItems.size() && 0 < m_toolbarItems.size())
	{
		wxToolBar* toolbar = m_Frame->CreateToolBar(wxTB_FLAT);

		for (size_t index = 0; index < m_toolbarItems.size(); ++index)
		{
			if (0 == m_toolbarItems[index])
			{
				toolbar->AddSeparator();
			}
			else
			{
				size_t idxItem = FindItem(m_toolbarItems[index], m_menuItems);
				if (idxItem >= m_menuItems.size())
					continue;
				assert(m_menuItems[idxItem].toolbar);
				wxString name;
				wxString descShort;
				if (m_doTranslation)
				{
					name = wxGetTranslation(m_menuItems[idxItem].toolbar);
					descShort = wxGetTranslation(m_menuItems[idxItem].help);
					m_ToolbarData.push_back(TranslationData(
						m_menuItems[idxItem].id,
						m_menuItems[idxItem].toolbar,
						m_menuItems[idxItem].help));
				}
				else
				{
					name = m_menuItems[idxItem].toolbar;
					descShort = m_menuItems[idxItem].help;
				}
				wxBitmap bmp = ImageHelper::GetBitmap(pFrame, m_menuItems[idxItem].artId, wxART_TOOLBAR);
				assert(bmp.IsOk());
				wxBitmap bmpDisabled = bmp.ConvertToDisabled();
				assert(bmpDisabled.IsOk());
				toolbar->AddTool(m_menuItems[idxItem].id, name, bmp, bmpDisabled, m_menuItems[idxItem].kind, descShort);
			}
		}
		toolbar->Realize();
	}
}


wxMenu* CMenuHelper::CreatePopupMenu(wxWindow* pWindow, int menuId)
{
	MenuHandle handle(0);

	// We're not supporting an MRU menu item in the popups.
	bool mruAdded = false;
	bool bAdded = false;
	for (size_t index = 0; index < m_menuItems.size();)
	{
		if (m_menuItems[index].menuId != menuId || !((MENU_ITEM | MENU_HELP) & m_menuItems[index].flags))
		{
			++index;
			continue;
		}
		assert(m_menuItems[index].menu);
		--index; // Because Menu() assumes we're on the parent item and increments index before processing.
		Menu(pWindow, menuId, handle, index, 0, nullptr, mruAdded, true);
		bAdded = true;
	}
	assert(bAdded);
	VerifyMenuShortcuts(handle.pMenu);
	return handle.pMenu;
}


void CMenuHelper::UpdateMenu(bool bLoadAccelerators)
{
	if (bLoadAccelerators)
		LoadAccelerators();
	if (m_doTranslation)
	{
		for (std::vector<MenuHandle>::iterator i = m_MenuData.begin(); i != m_MenuData.end(); ++i)
		{
			m_MenuBar->SetMenuLabel(i->idx, wxGetTranslation(i->item.c_str()) + GetAccelString(m_accelData, i->idx));
			DoSubMenu(nullptr, *i);
			// TODO: m_MenuData should be verified too
			VerifyMenuShortcuts((*i).pMenu);
		}

		wxToolBar* toolbar = m_Frame->GetToolBar();
		if (toolbar)
		{
			for (std::vector<TranslationData>::iterator i = m_ToolbarData.begin(); i != m_ToolbarData.end(); ++i)
			{
				if (0 == i->id)
					continue;
				wxToolBarToolBase* btn = toolbar->FindById(i->id);
				if (btn)
				{
					btn->SetLabel(wxGetTranslation(i->name.c_str()));
					btn->SetShortHelp(wxGetTranslation(i->desc.c_str()));
				}
			}
		}
	}
}


bool CMenuHelper::VerifyMenuShortcuts(wxMenu* menu, int level)
{
	bool hasDups = false;
#if defined(_DEBUG) || defined(__WXDEBUG__)
	if (!menu)
		return hasDups;
	// wxLogDebug(L"Processing %s", menu->GetTitle());
	std::map<wchar_t, wxString> shortcuts;
	for (auto const& item : menu->GetMenuItems())
	{
		if (item->IsSeparator())
			continue;
		wxString name = item->GetItemLabel();
		// wxLogDebug(L"%*s %s", level + 1, L" ", name);
		// Strip accelerator
		auto idx = name.Find('\t');
		if (wxNOT_FOUND != idx)
			name = name.substr(0, idx);
		wxString fullname = name;
		// Strip '&&', we should only have a single '&' now.
		name.Replace(L"&&", L"", true);
		// Sanity: Ending with '&' isn't right.
		if (name.length() > 0 && name[name.length() - 1] == '&')
		{
			wxLogDebug(L"WARNING: '%s' ends with '&'", fullname);
			assert(!"Menu ends with '&'");
			name = name.substr(0, name.length() - 1);
		}
		idx = name.Find('&');
		if (wxNOT_FOUND != idx)
		{
			if (shortcuts.find(name[idx + 1]) != shortcuts.end())
			{
				wxLogDebug(
					L"WARNING: Shortcut '%c' used by '%s' is already in use by '%s'",
					name[idx + 1],
					fullname,
					shortcuts[name[idx + 1]]);
				assert(!"Duplicate menu shortcut");
				hasDups = true;
			}
			else
				shortcuts[name[idx + 1]] = fullname;
		}
		if (item->GetSubMenu())
			VerifyMenuShortcuts(item->GetSubMenu(), level + 1);
	}
#endif
	return hasDups;
}


void CMenuHelper::DoMenuItem(
	wxWindow* pWindow,
	wxMenu* menu,
	int id,
	wxString const& label,
	wxString const& desc,
	wxArtID const& artId)
{
	DoMenuItem(pWindow, menu, id, label, desc, wxITEM_NORMAL, nullptr, artId);
}


void CMenuHelper::DoMenuItem(
	wxWindow* pWindow,
	wxMenu* menu,
	int id,
	wxString const& label,
	wxString const& desc,
	wxItemKind kind,
	wxMenu* subMenu,
	wxArtID const& artId)
{
	wxMenuItem* item = new wxMenuItem(menu, id, label + GetAccelString(m_accelData, id), desc, kind, subMenu);
	if (!artId.empty() && kind == wxITEM_NORMAL)
	{
		wxBitmap bmp = ImageHelper::GetBitmap(pWindow, artId, wxART_MENU);
		if (bmp.IsOk())
			item->SetBitmap(bmp);
	}
	menu->Append(item);
}


wxString CMenuHelper::GetAccelString(std::vector<ItemAccel> const& accelItems, int id)
{
	wxString str;
	for (auto iter = accelItems.begin(); iter != accelItems.end(); ++iter)
	{
		if ((*iter).id == id)
		{
			if ((*iter).bCtrl)
				str << _("Ctrl+");
			if ((*iter).bAlt)
				str << _("Alt+");
			if ((*iter).bShift)
				str << _("Shift+");
			if (32 <= (*iter).keyCode && (*iter).keyCode < 127)
				str << wxString::Format(L"%c", (*iter).keyCode);
			else
			{
				wxString special = CodeToSpecial((*iter).keyCode, false);
				if (!special.empty())
					str << wxGetTranslation(special);
				else
					str.clear(); // Key not supported in menu
			}
			break;
		}
	}
	if (str.empty())
		return str;
	return wxString(L"\t") + str;
}


int CMenuHelper::TranslateId(int id, std::vector<ItemAccel> const& defAccelItems)
{
	if (0 != id)
	{
		for (size_t n = 0; n < defAccelItems.size(); ++n)
		{
			if (id == defAccelItems[n].key)
			{
				return defAccelItems[n].id;
			}
		}
	}
	return 0;
}


void CMenuHelper::CreateAccelTable(wxFrame* pFrame)
{
	if (m_accelData.size() > 0)
	{
		auto entries = std::vector<wxAcceleratorEntry>(m_accelData.size());
		for (size_t n = 0; n < m_accelData.size(); ++n)
		{
			int flags = wxACCEL_NORMAL;
			if (m_accelData[n].bAlt)
				flags |= wxACCEL_ALT;
			if (m_accelData[n].bCtrl)
				flags |= wxACCEL_CTRL;
			if (m_accelData[n].bShift)
				flags |= wxACCEL_SHIFT;
			entries[n].Set(flags, m_accelData[n].keyCode, m_accelData[n].id);
		}
		wxAcceleratorTable accel(static_cast<int>(m_accelData.size()), entries.data());
		pFrame->SetAcceleratorTable(accel);
	}
	else
	{
		pFrame->SetAcceleratorTable(wxNullAcceleratorTable);
	}
}


void CMenuHelper::Menu(
	wxWindow* pWindow,
	int menuId,
	MenuHandle& handle,
	size_t& index,
	size_t level,
	wxMenu* mruMenu,
	bool& mruAdded,
	bool isPopup)
{
	for (++index; index < m_menuItems.size() && level == m_menuItems[index].menuLevel; ++index)
	{
		if (m_menuItems[index].menuId != menuId)
			continue;
		if (MENU_SEP & m_menuItems[index].flags)
			handle.pMenu->AppendSeparator();
		else if ((MENU_ITEM | MENU_MRU | MENU_HELP) & m_menuItems[index].flags)
		{
			assert(m_menuItems[index].menu);
			if (MENU_MRU & m_menuItems[index].flags)
			{
				assert(mruMenu);
				assert(!mruAdded);
				mruAdded = true;
				wxString name;
				MenuHandle subhandle(mruMenu, static_cast<int>(handle.pMenu->GetMenuItemCount()));
				if (m_doTranslation)
				{
					name = StripMnemonics(wxGetTranslation(m_menuItems[index].menu), isPopup);
					subhandle.item = m_menuItems[index].menu;
				}
				else
				{
					name = m_menuItems[index].menu;
				}
				DoMenuItem(
					pWindow,
					handle.pMenu,
					wxID_ANY,
					name,
					wxString(),
					wxITEM_NORMAL,
					mruMenu,
					m_menuItems[index].artId);
				handle.subMenus.push_back(subhandle);
			}
			else if (0 == m_menuItems[index].id)
			{
				size_t idxMenu = index;
				MenuHandle subhandle(static_cast<int>(handle.pMenu->GetMenuItemCount()));
				Menu(pWindow, menuId, subhandle, index, level + 1, mruMenu, mruAdded, isPopup);
				wxString name;
				if (m_doTranslation)
				{
					name = StripMnemonics(wxGetTranslation(m_menuItems[idxMenu].menu), isPopup);
					subhandle.item = m_menuItems[idxMenu].menu;
				}
				else
				{
					name = m_menuItems[idxMenu].menu;
				}
				DoMenuItem(
					pWindow,
					handle.pMenu,
					wxID_ANY,
					name,
					wxString(),
					wxITEM_NORMAL,
					subhandle.pMenu,
					m_menuItems[idxMenu].artId);
				--index;
				handle.subMenus.push_back(subhandle);
			}
			else
			{
				wxString name;
				wxString help;
				if (m_doTranslation)
				{
					name = StripMnemonics(wxGetTranslation(m_menuItems[index].menu), isPopup);
					help = wxGetTranslation(m_menuItems[index].help);
					handle.items.push_back(
						TranslationData(m_menuItems[index].id, m_menuItems[index].menu, m_menuItems[index].help));
				}
				else
				{
					name = m_menuItems[index].menu;
					help = m_menuItems[index].help;
				}
				DoMenuItem(
					pWindow,
					handle.pMenu,
					m_menuItems[index].id,
					name,
					help,
					m_menuItems[index].kind,
					nullptr,
					m_menuItems[index].artId);
			}
		}
	}
}


void CMenuHelper::DoSubMenu(wxMenu const* parent, MenuHandle const& handle)
{
	if (parent)
	{
		parent->FindItemByPosition(handle.idx)->SetItemLabel(wxGetTranslation(handle.item.c_str()));
	}
	for (std::vector<MenuHandle>::const_iterator data = handle.subMenus.begin(); data != handle.subMenus.end(); ++data)
	{
		DoSubMenu(handle.pMenu, *data);
	}
	for (std::vector<TranslationData>::const_iterator data = handle.items.begin(); data != handle.items.end(); ++data)
	{
		handle.pMenu->SetLabel(data->id, wxGetTranslation(data->name.c_str()) + GetAccelString(m_accelData, data->id));
		handle.pMenu->SetHelpString(data->id, wxGetTranslation(data->desc.c_str()));
	}
}
