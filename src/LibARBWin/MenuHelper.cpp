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
 * 2016-02-17 Create a disabled image for toolbar. wx3.1 images are too dark.
 * 2015-12-23 Fix a bug when a separator followed submenu.
 * 2013-10-13 Changed to using an art provider.
 * 2012-07-28 Fix About menu insertion.
 * 2012-07-13 Created
 */

#include "stdafx.h"
#include "LibARBWin/MenuHelper.h"

#include "ARBCommon/ARBUtils.h"
#include "LibARBWin/ImageHelperBase.h"
#include "LibARBWin/RegItemsBase.h"
#include <set>
#include <wx/artprov.h>
#include <wx/config.h>
#include <wx/frame.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

// See wxMenuItem::SetItemLabel() for set of special keys
static struct
{
	int wxCode;
	const wchar_t* special;
} s_codeMapping[] = {
	{WXK_F1, L"F1"},
	{WXK_F2, L"F2"},
	{WXK_F3, L"F3"},
	{WXK_F4, L"F4"},
	{WXK_F5, L"F5"},
	{WXK_F6, L"F6"},
	{WXK_F7, L"F7"},
	{WXK_F8, L"F8"},
	{WXK_F9, L"F9"},
	{WXK_F10, L"F10"},
	{WXK_F11, L"F11"},
	{WXK_F12, L"F12"},
	{WXK_DELETE, L"Del"},
	{WXK_DELETE, L"Delete"},
	{WXK_BACK, L"Back"},
	{WXK_INSERT, L"Ins"},
	{WXK_INSERT, L"Insert"},
	{WXK_RETURN, L"Enter"},
	{WXK_RETURN, L"Return"},
	{WXK_PAGEUP, L"PgUp"},
	{WXK_PAGEUP, L"PageUp"},
	{WXK_PAGEDOWN, L"PgDn"},
	{WXK_PAGEDOWN, L"PageDn"},
	{WXK_LEFT, L"Left"},
	{WXK_RIGHT, L"Right"},
	{WXK_UP, L"Up"},
	{WXK_DOWN, L"Down"},
	{WXK_HOME, L"Home"},
	{WXK_END, L"End"},
	{WXK_SPACE, L"Space"},
	{WXK_TAB, L"Tab"},
	{WXK_ESCAPE, L"Esc"},
	{WXK_ESCAPE, L"Escape"},
	{WXK_CANCEL, L"Cancel"},
	{WXK_CLEAR, L"Clear"},
	{WXK_MENU, L"Menu"},
	{WXK_PAUSE, L"Pause"},
	{WXK_CAPITAL, L"Capital"},
	{WXK_SELECT, L"Select"},
	{WXK_PRINT, L"Print"},
	{WXK_EXECUTE, L"Execute"},
	{WXK_SNAPSHOT, L"Snapshot"},
	{WXK_HELP, L"Help"},
	{WXK_ADD, L"Add"},
	{WXK_SEPARATOR, L"Separator"},
	{WXK_SUBTRACT, L"Subtract"},
	{WXK_DECIMAL, L"Decimal"},
	{WXK_DIVIDE, L"Divide"},
	{WXK_NUMLOCK, L"Num_lock"},
	{WXK_SCROLL, L"Scroll_lock"},
	{WXK_NUMPAD_SPACE, L"KP_Space"},
	{WXK_NUMPAD_TAB, L"KP_Tab"},
	{WXK_NUMPAD_ENTER, L"KP_Enter"},
	{WXK_NUMPAD_HOME, L"KP_Home"},
	{WXK_NUMPAD_LEFT, L"KP_Left"},
	{WXK_NUMPAD_UP, L"KP_Up"},
	{WXK_NUMPAD_RIGHT, L"KP_Right"},
	{WXK_NUMPAD_DOWN, L"KP_Down"},
	{WXK_NUMPAD_PAGEUP, L"KP_PageUp"},
	{WXK_NUMPAD_PAGEDOWN, L"KP_PageDown"},
	{WXK_NUMPAD_PAGEUP, L"KP_Prior"},
	{WXK_NUMPAD_PAGEDOWN, L"KP_Next"},
	{WXK_NUMPAD_END, L"KP_End"},
	{WXK_NUMPAD_BEGIN, L"KP_Begin"},
	{WXK_NUMPAD_INSERT, L"KP_Insert"},
	{WXK_NUMPAD_DELETE, L"KP_Delete"},
	{WXK_NUMPAD_EQUAL, L"KP_Equal"},
	{WXK_NUMPAD_MULTIPLY, L"KP_Multiply"},
	{WXK_NUMPAD_ADD, L"KP_Add"},
	{WXK_NUMPAD_SEPARATOR, L"KP_Separator"},
	{WXK_NUMPAD_SUBTRACT, L"KP_Subtract"},
	{WXK_NUMPAD_DECIMAL, L"KP_Decimal"},
	{WXK_NUMPAD_DIVIDE, L"KP_Divide"},
	{WXK_WINDOWS_LEFT, L"Windows_Left"},
	{WXK_WINDOWS_RIGHT, L"Windows_Right"},
	{WXK_WINDOWS_MENU, L"Windows_Menu"},
	{WXK_NUMPAD0, L"KP_0"},
	{WXK_NUMPAD1, L"KP_1"},
	{WXK_NUMPAD2, L"KP_2"},
	{WXK_NUMPAD3, L"KP_3"},
	{WXK_NUMPAD4, L"KP_4"},
	{WXK_NUMPAD5, L"KP_5"},
	{WXK_NUMPAD6, L"KP_6"},
	{WXK_NUMPAD7, L"KP_7"},
	{WXK_NUMPAD8, L"KP_8"},
	{WXK_NUMPAD9, L"KP_9"},
};

static int SpecialToCode(wxString const& special)
{
	for (auto item : s_codeMapping)
	{
		if (special == item.special)
			return item.wxCode;
	}
	return 0;
}

static const wchar_t* CodeToSpecial(int code)
{
	for (auto item : s_codeMapping)
	{
		if (code == item.wxCode)
			return item.special;
	}
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////

CMenuHelper::CMenuHelper()
	: m_Frame(nullptr)
	, m_MenuBar(nullptr)
	, m_doTranslation(false)
	, m_MenuData()
	, m_ToolbarData()
	, m_bModified(false)
	, m_accelData()
{
}


CMenuHelper::~CMenuHelper()
{
}


void CMenuHelper::LoadAccelerators(
		ItemAccel const defAccelItems[],
		size_t numDefAccelItems)
{
	m_accelData.clear();

	if (wxConfig::Get()->HasGroup(CFG_KEY_ACCELERATORS))
	{
		CConfigPathHelper config(CFG_KEY_ACCELERATORS);
		wxString entry;
		long index = 0;
		if (wxConfig::Get()->GetFirstGroup(entry, index))
		{
			do
			{
				CConfigPathHelper configEntry(entry);
				AccelData accel;
				accel.idStr = wxConfig::Get()->Read(L"id", wxString());
				accel.id = TranslateId(accel.idStr, defAccelItems, numDefAccelItems);
				accel.keyCode = SpecialToCode(wxConfig::Get()->Read(L"KeyCode", wxString()));
				if (0 != accel.id && accel.keyCode)
				{
					long mask = 0;
					wxConfig::Get()->Read(L"mod", &mask);
					FromBitmask(mask, accel);
					if (std::find_if(m_accelData.begin(), m_accelData.end(), [&accel](AccelData const& arg) {return arg.id == accel.id; }) == m_accelData.end())
						m_accelData.push_back(accel);
				}
			} while (wxConfig::Get()->GetNextGroup(entry, index));
		}
	}

	if (m_accelData.empty())
	{
		for (size_t index = 0; index < numDefAccelItems; ++index)
			if (defAccelItems[index].keyCode)
				m_accelData.push_back(AccelData(defAccelItems[index]));
	}

#ifdef _DEBUG
	// Sanity checking
	std::set<int> ids;
	for (auto iter = m_accelData.begin(); iter != m_accelData.end(); ++iter)
	{
		// Must have a code (assuming valid string for WX)
		assert((*iter).keyCode);
		// Check that we know how to translate wxKeyCode to string
		assert(!GetAccelString(m_accelData, (*iter).id).empty());
		// Only list id once
		assert(ids.find((*iter).id) == ids.end());
		ids.insert((*iter).id);
	}
#endif
}


void CMenuHelper::SaveAccelerators()
{
	if (m_bModified)
	{
		// Clear existing
		wxConfig::Get()->DeleteGroup(CFG_KEY_ACCELERATORS);

		CConfigPathHelper config(CFG_KEY_ACCELERATORS);

		// Save
		int nKey = 1;
		for (auto iter = m_accelData.begin(); iter != m_accelData.end(); ++iter)
		{
			wxString key = wxString::Format(L"Item%d", nKey++);
			CConfigPathHelper configKey(key);
			assert(CodeToSpecial(iter->keyCode));
			wxConfig::Get()->Write(L"KeyCode", CodeToSpecial(iter->keyCode));
			wxConfig::Get()->Write(L"id", iter->idStr);
			wxConfig::Get()->Write(L"mod", ToBitmask(*iter));
		}

		m_bModified = false;
	}
}


void CMenuHelper::CreateMenu(
		wxFrame* pFrame,
		ItemData const items[],
		size_t numItems,
		bool doTranslation,
		wxMenu* mruMenu)
{
	assert(!m_MenuBar);
	m_Frame = pFrame;
	m_doTranslation = doTranslation;

	if (0 < numItems)
	{
		m_MenuBar = new wxMenuBar();
		bool mruAdded = false;

		for (size_t index = 0; index < numItems; )
		{
			if (!((MENU_ITEM | MENU_HELP) & items[index].flags))
				continue;
			assert(items[index].menu);
			size_t idxMenu = index;
			MenuHandle handle(static_cast<int>(m_MenuBar->GetMenuCount()));
			Menu(pFrame, handle, index, 1, mruMenu, mruAdded, items, numItems);
			wxString name;
			if (m_doTranslation)
			{
				name = wxGetTranslation(items[idxMenu].menu);
				handle.item = items[idxMenu].menu;
			}
			else
			{
				name = items[idxMenu].menu;
			}
#ifdef __WXMAC__
			if (MENU_HELP & items[index].flags)
			{
				wxApp::s_macHelpMenuTitleName = name;
			}
#endif
			m_MenuBar->Append(handle.pMenu, name);
			m_MenuData.push_back(handle);
		}

		if (mruMenu)
		{
			assert(mruAdded);
		}

		m_Frame->SetMenuBar(m_MenuBar);
	}
}


static size_t FindItem(int id, CMenuHelper::ItemData const menuItems[], size_t numMenuItems)
{
	for (size_t i = 0; i < numMenuItems; ++i)
	{
		if (menuItems[i].id == id)
			return i;
	}
	return numMenuItems;
}


void CMenuHelper::CreateMenu(
		wxFrame* pFrame,
		ItemData const menuItems[],
		size_t numMenuItems,
		int const toolbarItems[],
		size_t numToolbarItems,
		bool doTranslation,
		wxMenu* mruMenu)
{
	// Load accelerators first. Not all of these are necessarily in the main menu.
	// This ensures all accelerators are registered.
	if (m_accelData.size() > 0)
	{
		wxAcceleratorEntry* entries = new wxAcceleratorEntry[m_accelData.size()];
		for (size_t n = 0; n < m_accelData.size(); ++n)
		{
			int flags = wxACCEL_NORMAL;
			if (m_accelData[n].bAlt)
				flags |= wxACCEL_ALT;
			if (m_accelData[n].bCtrl)
				flags |= wxACCEL_CTRL;
			if (m_accelData[n].bShift)
				flags |= wxACCEL_SHIFT;
			entries[n].Set(flags, m_accelData[n].keyCode , m_accelData[n].id);
		}
		wxAcceleratorTable* accel = new wxAcceleratorTable(int(m_accelData.size()), entries);
		pFrame->SetAcceleratorTable(*accel);
		delete accel;
		delete[] entries;
	}

	CreateMenu(pFrame, menuItems, numMenuItems, doTranslation, mruMenu);

	if (0 < numMenuItems && 0 < numToolbarItems)
	{
		wxToolBar* toolbar = m_Frame->CreateToolBar(wxTB_FLAT);

		for (size_t index = 0; index < numToolbarItems; ++index)
		{
			if (0 == toolbarItems[index])
			{
				toolbar->AddSeparator();
			}
			else
			{
				size_t idxItem = FindItem(toolbarItems[index], menuItems, numMenuItems);
				if (idxItem >= numMenuItems)
					continue;
				assert(menuItems[idxItem].toolbar);
				wxString name;
				wxString descShort;
				if (doTranslation)
				{
					name = wxGetTranslation(menuItems[idxItem].toolbar);
					descShort = wxGetTranslation(menuItems[idxItem].help);
					m_ToolbarData.push_back(TranslationData(menuItems[idxItem].id, menuItems[idxItem].toolbar, menuItems[idxItem].help));
				}
				else
				{
					name = menuItems[idxItem].toolbar;
					descShort = menuItems[idxItem].help;
				}
				wxBitmap bmp = ImageHelper::GetBitmap(pFrame, menuItems[idxItem].artId, wxART_TOOLBAR);
				assert(bmp.IsOk());
				wxBitmap bmpDisabled = bmp.ConvertToDisabled();
				assert(bmpDisabled.IsOk());
				toolbar->AddTool(menuItems[idxItem].id,
					name,
					bmp,
					bmpDisabled,
					menuItems[idxItem].kind,
					descShort);
			}
		}
		toolbar->Realize();
	}
}


void CMenuHelper::UpdateMenu()
{
	if (m_doTranslation)
	{
		for (std::vector<MenuHandle>::iterator i = m_MenuData.begin(); i != m_MenuData.end(); ++i)
		{
			m_MenuBar->SetMenuLabel(i->idx, wxGetTranslation(i->item.c_str()) + GetAccelString(m_accelData, i->idx));
			DoSubMenu(nullptr, *i);
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


long CMenuHelper::ToBitmask(AccelData const& accel)
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


void CMenuHelper::FromBitmask(long mask, AccelData& accel)
{
	accel.bAlt = (mask & 0x1);
	accel.bCtrl = (mask & 0x2);
	accel.bShift = (mask & 0x4);
}


wxString CMenuHelper::GetAccelString(std::vector<AccelData> const& accelItems, int id)
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
				const wchar_t* special = CodeToSpecial((*iter).keyCode);
				assert(special);
				if (special)
					str << special;
				else
					str << L"?";
			}
			break;
		}
	}
	if (str.empty())
		return str;
	return wxString(L"\t") + str;
}


int CMenuHelper::TranslateId(
		wxString const& idStr,
		ItemAccel const defAccelItems[],
		size_t numDefAccelItems)
{
	int id = 0;
	if (!idStr.empty())
	{
		for (size_t n = 0; n < numDefAccelItems; ++n)
		{
			if (idStr == defAccelItems[n].idStr)
			{
				id = defAccelItems[n].id;
				break;
			}
		}
	}
	return id;
}


void CMenuHelper::Menu(
		wxWindow* pWindow,
		MenuHandle& handle,
		size_t& index,
		size_t level,
		wxMenu* mruMenu,
		bool& mruAdded,
		ItemData const items[],
		size_t numItems)
{
	for (++index; index < numItems && level == items[index].menuLevel; ++index)
	{
		if (MENU_SEP & items[index].flags)
			handle.pMenu->AppendSeparator();
		else if ((MENU_ITEM | MENU_MRU | MENU_HELP) & items[index].flags)
		{
			assert(items[index].menu);
			if (MENU_MRU & items[index].flags)
			{
				assert(mruMenu);
				assert(!mruAdded);
				mruAdded = true;
				wxString name;
				MenuHandle subhandle(mruMenu, static_cast<int>(handle.pMenu->GetMenuItemCount()));
				if (m_doTranslation)
				{
					name = wxGetTranslation(items[index].menu);
					subhandle.item = items[index].menu;
				}
				else
				{
					name = items[index].menu;
				}
				DoMenuItem(pWindow, handle.pMenu, wxID_ANY,
					name,
					wxString(),
					wxITEM_NORMAL,
					mruMenu,
					items[index].artId);
				handle.subMenus.push_back(subhandle);
			}
			else if (0 == items[index].id)
			{
				size_t idxMenu = index;
				MenuHandle subhandle(static_cast<int>(handle.pMenu->GetMenuItemCount()));
				Menu(pWindow, subhandle, index, level + 1, mruMenu, mruAdded, items, numItems);
				wxString name;
				if (m_doTranslation)
				{
					name = wxGetTranslation(items[idxMenu].menu);
					subhandle.item = items[idxMenu].menu;
				}
				else
				{
					name = items[idxMenu].menu;
				}
				DoMenuItem(pWindow, handle.pMenu, wxID_ANY,
					name,
					wxString(),
					wxITEM_NORMAL,
					subhandle.pMenu,
					items[idxMenu].artId);
				--index;
				handle.subMenus.push_back(subhandle);
			}
			else
			{
				wxString name;
				wxString help;
				if (m_doTranslation)
				{
					name = wxGetTranslation(items[index].menu);
					help = wxGetTranslation(items[index].help);
					handle.items.push_back(TranslationData(items[index].id, items[index].menu, items[index].help));
				}
				else
				{
					name = items[index].menu;
					help = items[index].help;
				}
				DoMenuItem(pWindow, handle.pMenu, items[index].id,
					name,
					help,
					items[index].kind,
					nullptr,
					items[index].artId);
			}
		}
	}
}


void CMenuHelper::DoSubMenu(wxMenu* parent, MenuHandle const& handle)
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
