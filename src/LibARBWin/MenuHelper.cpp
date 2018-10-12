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

#include "LibARBWin/ImageHelperBase.h"
#include <set>
#include <wx/artprov.h>
#include <wx/frame.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


static wxString GetAccelString(std::vector<CMenuHelper::ItemAccel> const& accelItems, int id)
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
			str << (*iter).keyCode;
			break;
		}
	}
	if (str.empty())
		return str;
	return wxString(L"\t") + str;
}

/////////////////////////////////////////////////////////////////////////////

CMenuHelper::CMenuHelper()
	: m_Frame(nullptr)
	, m_MenuBar(nullptr)
	, m_doTranslation(false)
	, m_MenuData()
	, m_ToolbarData()
{
}


CMenuHelper::~CMenuHelper()
{
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
	wxMenuItem* item = new wxMenuItem(menu, id, label + GetAccelString(m_accelItems, id), desc, kind, subMenu);
	if (!artId.empty() && kind == wxITEM_NORMAL)
	{
		wxBitmap bmp = ImageHelper::GetBitmap(pWindow, artId, wxART_MENU);
		if (bmp.IsOk())
			item->SetBitmap(bmp);
	}
	menu->Append(item);
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


void CMenuHelper::LoadAccelerators(
		ItemAccel const defAccelItems[],
		size_t numDefAccelItems)
{
	m_accelItems.clear();
	for (size_t index = 0; index < numDefAccelItems; ++index)
		m_accelItems.push_back(defAccelItems[index]);

#pragma PRAGMA_TODO(Load custom accelerators)

#ifdef _DEBUG
	// Sanity checking
	std::set<int> ids;
	for (auto iter = m_accelItems.begin(); iter != m_accelItems.end(); ++iter)
	{
		// Must have a code (assuming valid string for WX)
		assert((*iter).keyCode);
		// Only list id once
		assert(ids.find((*iter).id) == ids.end());
		ids.insert((*iter).id);
	}
#endif
}


void CMenuHelper::SaveAccelerators()
{
#pragma PRAGMA_TODO(Save custom accelerators)
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
		handle.pMenu->SetLabel(data->id, wxGetTranslation(data->name.c_str()) + GetAccelString(m_accelItems, data->id));
		handle.pMenu->SetHelpString(data->id, wxGetTranslation(data->desc.c_str()));
	}
}


void CMenuHelper::UpdateMenu()
{
	if (m_doTranslation)
	{
		for (std::vector<MenuHandle>::iterator i = m_MenuData.begin(); i != m_MenuData.end(); ++i)
		{
			m_MenuBar->SetMenuLabel(i->idx, wxGetTranslation(i->item.c_str()) + GetAccelString(m_accelItems, i->idx));
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
