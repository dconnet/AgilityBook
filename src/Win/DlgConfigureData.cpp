/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgConfigure class
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2016-06-19 Add support for Lifetime names.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-01-10 Allow titles to be optionally entered multiple times.
 * 2004-04-02 Up the ref count to prevent problems. See comments below.
 * 2004-01-14 Use complete title name instead of nice name. When
 *            renaming a title and that name is in use, prompt to merge.
 * 2003-02-23 Modifying the scoring methods wasn't updated in the tree.
 */

#include "stdafx.h"
#include "DlgConfigureData.h"

#include "DlgConfigEvent.h"
#include "DlgConfigLifetimeName.h"
#include "DlgConfigMultiQ.h"
#include "DlgConfigOtherPoints.h"
#include "DlgConfigTitle.h"
#include "DlgConfigVenue.h"
#include "DlgConfigure.h"
#include "DlgName.h"
#include "DlgReorder.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBConfigDivision.h"
#include "ARB/ARBConfigVenue.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/Widgets.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

CDlgConfigureDataBase::CDlgConfigureDataBase(CDlgConfigVenue* pDlg)
	: m_pDlg(pDlg)
{
}

////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataVenue::CDlgConfigureDataVenue(ARBConfigVenuePtr const& inVenue)
	: CDlgConfigureDataBase(nullptr)
	, m_Venue(inVenue)
{
}


std::wstring CDlgConfigureDataVenue::OnNeedText() const
{
	std::wstring str(OnNeedText(0));
	for (int i = 1; i <= 2; ++i)
	{
		std::wstring s(OnNeedText(i));
		if (!s.empty())
		{
			str += L"  " + s;
		}
	}
	return str;
}


std::wstring CDlgConfigureDataVenue::OnNeedText(int iColumn) const
{
	std::wstring str;
	switch (iColumn)
	{
	case 0:
		str = m_Venue->GetName();
		break;
	case 1:
		str = m_Venue->GetURL();
		break;
	case 2:
		str = StringUtil::Replace(m_Venue->GetDesc(), L"\n", L" ");
		break;
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataFault::CDlgConfigureDataFault(ARBConfigFaultPtr const& inFault)
	: CDlgConfigureDataBase(nullptr)
	, m_Fault(inFault)
{
}


std::wstring CDlgConfigureDataFault::OnNeedText() const
{
	return OnNeedText(0);
}


std::wstring CDlgConfigureDataFault::OnNeedText(int iColumn) const
{
	return m_Fault->GetName();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataOtherPoints::CDlgConfigureDataOtherPoints(ARBConfigOtherPointsPtr const& inOtherPoints)
	: CDlgConfigureDataBase(nullptr)
	, m_OtherPoints(inOtherPoints)
{
}


std::wstring CDlgConfigureDataOtherPoints::OnNeedText() const
{
	return OnNeedText(0);
}


std::wstring CDlgConfigureDataOtherPoints::OnNeedText(int iColumn) const
{
	return m_OtherPoints->GetName();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataDivision::CDlgConfigureDataDivision(CDlgConfigVenue* pDlg, ARBConfigDivisionPtr const& inDiv)
	: CDlgConfigureDataBase(pDlg)
	, m_Div(inDiv)
{
}


std::wstring CDlgConfigureDataDivision::OnNeedText() const
{
	return OnNeedText(0);
}


std::wstring CDlgConfigureDataDivision::OnNeedText(int iColumn) const
{
	return m_Div->GetName();
}


void CDlgConfigureDataDivision::AddSubItems()
{
	for (ARBConfigLevelList::iterator iterLevel = m_Div->GetLevels().begin(); iterLevel != m_Div->GetLevels().end();
		 ++iterLevel)
	{
		CDlgConfigureDataLevel* pLevData = new CDlgConfigureDataLevel(m_pDlg, m_Div, *iterLevel);
		m_pDlg->m_ctrlItems->AppendItem(GetId(), StringUtil::stringWX(pLevData->OnNeedText()), -1, -1, pLevData);
		pLevData->AddSubItems();
	}
}


bool CDlgConfigureDataDivision::DoAdd()
{
	bool added = false;
	bool done = false;
	std::wstring name;
	while (!done)
	{
		done = true;
		CDlgName dlg(name, _("IDS_LEVEL_NAME"), m_pDlg);
		if (wxID_OK == dlg.ShowModal())
		{
			name = dlg.Name();
			if (m_Div->GetLevels().FindSubLevel(name))
			{
				done = false;
				wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
				continue;
			}
			ARBConfigLevelPtr pNewLevel;
			if (m_Div->GetLevels().AddLevel(name, &pNewLevel))
			{
				CDlgConfigureDataLevel* pData = new CDlgConfigureDataLevel(m_pDlg, m_Div, pNewLevel);
				wxTreeItemId level
					= m_pDlg->m_ctrlItems
						  ->AppendItem(GetId(), StringUtil::stringWX(pData->OnNeedText()), -1, -1, pData);
				m_pDlg->m_ctrlItems->SelectItem(level);
				added = true;
			}
		}
	}
	return added;
}

bool CDlgConfigureDataDivision::DoEdit()
{
	std::wstring oldName = m_Div->GetName();
	std::wstring name(oldName);
	if (0 < m_pDlg->m_Book.GetDogs()
				.NumMultiHostedTrialsInDivision(m_pDlg->m_Book.GetConfig(), m_pDlg->m_pVenue->GetName(), name))
	{
		if (wxYES
			!= wxMessageBox(
				_("IDS_CHANGEDIV_ISSUES"),
				wxMessageBoxCaptionStr,
				wxYES_NO | wxCENTRE | wxICON_EXCLAMATION))
			return false;
	}
	bool bEdited = false;
	bool done = false;
	while (!done)
	{
		done = true;
		CDlgName dlg(name, _("IDS_DIVISION_NAME"), m_pDlg);
		if (wxID_OK == dlg.ShowModal())
		{
			name = dlg.Name();
			if (oldName != name)
			{
				if (m_pDlg->m_pVenue->GetDivisions().FindDivision(name))
				{
					done = false;
					wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
					continue;
				}
				m_Div->SetName(name);
				m_pDlg->m_pVenue->GetEvents().RenameDivision(oldName, name);
				m_pDlg->m_pVenue->GetMultiQs().RenameDivision(oldName, name);
				m_pDlg->m_DlgFixup.push_back(
					ARBConfigActionRenameDivision::New(0, m_pDlg->m_pVenue->GetName(), oldName, name));
				RefreshTreeItem(m_pDlg->m_ctrlItems, GetId());
				bEdited = true;
			}
		}
	}
	return bEdited;
}


bool CDlgConfigureDataDivision::DoDelete()
{
	std::wstring div = m_Div->GetName();
	// If we were able to delete it...
	if (m_pDlg->m_pVenue->GetDivisions().DeleteDivision(div, m_pDlg->m_pVenue->GetEvents()))
	{
		m_pDlg->m_pVenue->GetMultiQs().DeleteDivision(div);
		m_pDlg->m_DlgFixup.push_back(ARBConfigActionDeleteDivision::New(0, m_pDlg->m_pVenue->GetName(), div));
		m_pDlg->m_ctrlItems->Delete(GetId());
		return true;
	}
	return false;
}


CDlgConfigureDataBase* CDlgConfigureDataDivision::DoMove(bool bUp)
{
	if (m_pDlg->m_pVenue->GetDivisions().Move(m_Div, bUp ? -1 : 1))
		return new CDlgConfigureDataDivision(m_pDlg, m_Div);
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataLevel::CDlgConfigureDataLevel(
	CDlgConfigVenue* pDlg,
	ARBConfigDivisionPtr const& inDiv,
	ARBConfigLevelPtr const& inLevel)
	: CDlgConfigureDataBase(pDlg)
	, m_Division(inDiv)
	, m_Level(inLevel)
{
}


std::wstring CDlgConfigureDataLevel::OnNeedText() const
{
	return OnNeedText(0);
}


std::wstring CDlgConfigureDataLevel::OnNeedText(int iColumn) const
{
	return m_Level->GetName();
}


void CDlgConfigureDataLevel::AddSubItems()
{
	if (0 < m_Level->GetSubLevels().size())
	{
		for (ARBConfigSubLevelList::iterator iterSubLevel = m_Level->GetSubLevels().begin();
			 iterSubLevel != m_Level->GetSubLevels().end();
			 ++iterSubLevel)
		{
			CDlgConfigureDataSubLevel* pSubLevelData
				= new CDlgConfigureDataSubLevel(m_pDlg, m_Division, m_Level, *iterSubLevel);
			m_pDlg->m_ctrlItems
				->AppendItem(GetId(), StringUtil::stringWX(pSubLevelData->OnNeedText()), -1, -1, pSubLevelData);
			pSubLevelData->AddSubItems();
		}
	}
}


bool CDlgConfigureDataLevel::DoAdd()
{
	bool added = false;
	bool done = false;
	std::wstring name;
	while (!done)
	{
		done = true;
		CDlgName dlg(name, _("IDS_SUBLEVEL_NAME"), m_pDlg);
		if (wxID_OK == dlg.ShowModal())
		{
			name = dlg.Name();
			if (m_Division->GetLevels().FindSubLevel(name))
			{
				done = false;
				wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
				continue;
			}
			ARBConfigSubLevelPtr pNewSubLevel;
			if (m_Level->GetSubLevels().AddSubLevel(name, &pNewSubLevel))
			{
				CDlgConfigureDataSubLevel* pData
					= new CDlgConfigureDataSubLevel(m_pDlg, m_Division, m_Level, pNewSubLevel);
				wxTreeItemId level
					= m_pDlg->m_ctrlItems
						  ->AppendItem(GetId(), StringUtil::stringWX(pData->OnNeedText()), -1, -1, pData);
				pData->AddSubItems();
				m_pDlg->m_ctrlItems->SelectItem(level);
				added = true;
			}
		}
	}
	return added;
}


bool CDlgConfigureDataLevel::DoEdit()
{
	bool bEdited = false;
	bool done = false;
	std::wstring oldName = m_Level->GetName();
	std::wstring name(oldName);
	// If there are sublevels, don't ask the following question. If a level has
	// sublevels, the level name isn't allowed to be used for an event.
	if (0 == m_Level->GetSubLevels().size())
	{
		if (0 < m_pDlg->m_Book.GetDogs().NumMultiHostedTrialsInDivision(
				m_pDlg->m_Book.GetConfig(),
				m_pDlg->m_pVenue->GetName(),
				m_Division->GetName()))
		{
			if (wxYES
				!= wxMessageBox(
					_("IDS_CHANGELEVEL_ISSUES"),
					wxMessageBoxCaptionStr,
					wxYES_NO | wxCENTRE | wxICON_EXCLAMATION))
				return false;
		}
	}
	while (!done)
	{
		done = true;
		CDlgName dlg(name, _("IDS_LEVEL_NAME"), m_pDlg);
		if (wxID_OK == dlg.ShowModal())
		{
			name = dlg.Name();
			// If the name hasn't changed, don't do the inuse verification.
			if (oldName != name)
			{
				bool bInUse = false;
				if (0 == m_Level->GetSubLevels().size())
				{
					if (m_Division->GetLevels().FindSubLevel(name))
						bInUse = true;
				}
				else
				{
					if (m_Division->GetLevels().VerifyLevel(name, false))
						bInUse = true;
				}
				if (bInUse)
				{
					done = false;
					wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
					continue;
				}
				m_Level->SetName(name);
				if (0 == m_Level->GetSubLevels().size())
				{
					m_pDlg->m_pVenue->GetEvents().RenameLevel(m_Division->GetName(), oldName, name);
					m_pDlg->m_pVenue->GetMultiQs().RenameLevel(m_Division->GetName(), oldName, name);
				}
				m_pDlg->m_DlgFixup.push_back(ARBConfigActionRenameLevel::NewLevel(
					0,
					m_pDlg->m_pVenue->GetName(),
					m_Division->GetName(),
					oldName,
					m_Level->GetName()));
				RefreshTreeItem(m_pDlg->m_ctrlItems, GetId());
				bEdited = true;
			}
		}
	}
	return bEdited;
}


bool CDlgConfigureDataLevel::DoDelete()
{
	std::wstring level = m_Level->GetName();
	if (m_Division->GetLevels().DeleteLevel(m_Division->GetName(), level, m_pDlg->m_pVenue->GetEvents()))
	{
		m_pDlg->m_pVenue->GetMultiQs().DeleteLevel(level);
		m_pDlg->m_DlgFixup.push_back(
			ARBConfigActionDeleteLevel::NewLevel(0, m_pDlg->m_pVenue->GetName(), m_Division->GetName(), level));
		m_pDlg->m_ctrlItems->Delete(GetId());
		return true;
	}
	return false;
}


CDlgConfigureDataBase* CDlgConfigureDataLevel::DoMove(bool bUp)
{
	if (m_Division->GetLevels().Move(m_Level, bUp ? -1 : 1))
		return new CDlgConfigureDataLevel(m_pDlg, m_Division, m_Level);
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataSubLevel::CDlgConfigureDataSubLevel(
	CDlgConfigVenue* pDlg,
	ARBConfigDivisionPtr const& inDiv,
	ARBConfigLevelPtr const& inLevel,
	ARBConfigSubLevelPtr const& inSubLevel)
	: CDlgConfigureDataBase(pDlg)
	, m_Division(inDiv)
	, m_Level(inLevel)
	, m_SubLevel(inSubLevel)
{
}


std::wstring CDlgConfigureDataSubLevel::OnNeedText() const
{
	return OnNeedText(0);
}


std::wstring CDlgConfigureDataSubLevel::OnNeedText(int iColumn) const
{
	return m_SubLevel->GetName();
}


bool CDlgConfigureDataSubLevel::DoEdit()
{
	bool bEdited = false;
	bool done = false;
	std::wstring oldName = m_SubLevel->GetName();
	std::wstring name(oldName);
	if (0 < m_pDlg->m_Book.GetDogs().NumMultiHostedTrialsInDivision(
			m_pDlg->m_Book.GetConfig(),
			m_pDlg->m_pVenue->GetName(),
			m_Division->GetName()))
	{
		if (wxYES
			!= wxMessageBox(
				_("IDS_CHANGESUBLEVEL_ISSUES"),
				wxMessageBoxCaptionStr,
				wxYES_NO | wxCENTRE | wxICON_EXCLAMATION))
			return false;
	}
	while (!done)
	{
		done = true;
		CDlgName dlg(name, _("IDS_SUBLEVEL_NAME"), m_pDlg);
		if (wxID_OK == dlg.ShowModal())
		{
			name = dlg.Name();
			if (oldName != name)
			{
				if (m_Division->GetLevels().FindSubLevel(name))
				{
					done = false;
					wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
					continue;
				}
				m_SubLevel->SetName(name);
				// No need to fix ARBConfigEventList cause we don't do sublevel names in events.
				m_pDlg->m_pVenue->GetMultiQs().RenameLevel(m_Division->GetName(), oldName, m_SubLevel->GetName());
				m_pDlg->m_DlgFixup.push_back(ARBConfigActionRenameLevel::NewSubLevel(
					0,
					m_pDlg->m_pVenue->GetName(),
					m_Division->GetName(),
					m_Level->GetName(),
					oldName,
					m_SubLevel->GetName()));
				RefreshTreeItem(m_pDlg->m_ctrlItems, GetId());
				bEdited = true;
			}
		}
	}
	return bEdited;
}


bool CDlgConfigureDataSubLevel::DoDelete()
{
	std::wstring level = m_Level->GetName();
	std::wstring subLevel = m_SubLevel->GetName();
	bool bLevelModified = false;
	if (m_Division->GetLevels().DeleteSubLevel(subLevel, bLevelModified))
	{
		m_pDlg->m_pVenue->GetMultiQs().DeleteLevel(subLevel);
		// Note, if deleting the sublevel caused the level's name
		// to change, just leave it. It causes more trouble to
		// try modifing the name to the old sublevel name.
		m_pDlg->m_DlgFixup.push_back(ARBConfigActionDeleteLevel::NewSubLevel(
			0,
			m_pDlg->m_pVenue->GetName(),
			m_Division->GetName(),
			level,
			subLevel));
		m_pDlg->m_ctrlItems->Delete(GetId());
		return true;
	}
	return false;
}


CDlgConfigureDataBase* CDlgConfigureDataSubLevel::DoMove(bool bUp)
{
	if (m_Level->GetSubLevels().Move(m_SubLevel, bUp ? -1 : 1))
		return new CDlgConfigureDataSubLevel(m_pDlg, m_Division, m_Level, m_SubLevel);
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataTitle::CDlgConfigureDataTitle(CDlgConfigVenue* pDlg, ARBConfigTitlePtr const& inTitle)
	: CDlgConfigureDataBase(pDlg)
	, m_Title(inTitle)
{
}


std::wstring CDlgConfigureDataTitle::OnNeedText() const
{
	return OnNeedText(0);
}


std::wstring CDlgConfigureDataTitle::OnNeedText(int iColumn) const
{
	return m_Title->GetCompleteName(-1, true, true);
}


bool CDlgConfigureDataTitle::DoEdit()
{
	bool bEdited = false;
	bool done = false;
	std::wstring oldName = m_Title->GetName();
	std::wstring oldLongName = m_Title->GetLongName();
	std::wstring name(oldName);
	std::wstring longname(oldLongName);
	while (!done)
	{
		done = true;
		ARBConfigTitlePtr title = m_Title->Clone();
		CDlgConfigTitle dlg(title, m_pDlg);
		if (wxID_OK == dlg.ShowModal())
		{
			name = title->GetName();
			longname = title->GetLongName();
			if (oldName != name)
			{
				if (m_pDlg->m_pVenue->GetTitles().FindTitle(name))
				{
					int nTitles = m_pDlg->m_Book.GetDogs().NumTitlesInUse(m_pDlg->m_pVenue->GetName(), oldName);
					bool bInUse = true;
					if (0 < nTitles)
					{
						if (wxYES
							== wxMessageBox(
								_("IDS_NAME_IN_USE_MERGE"),
								wxMessageBoxCaptionStr,
								wxYES_NO | wxCENTRE | wxICON_EXCLAMATION))
						{
							bInUse = false;
							m_pDlg->m_DlgFixup.push_back(
								ARBConfigActionRenameTitle::New(0, m_pDlg->m_pVenue->GetName(), oldName, name));
							if (m_pDlg->m_pVenue->GetTitles().DeleteTitle(oldName))
							{
								m_pDlg->m_ctrlItems->Delete(GetId());
								// This will break out of the loop on the 'continue';
							}
						}
					}
					if (bInUse)
					{
						done = false;
						wxMessageBox(
							_("IDS_NAME_IN_USE"),
							wxMessageBoxCaptionStr,
							wxOK | wxCENTRE | wxICON_EXCLAMATION);
					}
					continue;
				}
				title->SetName(name);
			}
			*m_Title = *title;
			RefreshTreeItem(m_pDlg->m_ctrlItems, GetId());
			if (name != oldName || longname != oldLongName)
			{
				if (name != oldName)
					m_pDlg->m_DlgFixup.push_back(
						ARBConfigActionRenameTitle::New(0, m_pDlg->m_pVenue->GetName(), oldName, name));
				RefreshTreeItem(m_pDlg->m_ctrlItems, GetId());
			}
			bEdited = true;
		}
	}
	return bEdited;
}


bool CDlgConfigureDataTitle::DoDelete()
{
	std::wstring title = m_Title->GetName();
	if (m_pDlg->m_pVenue->GetTitles().DeleteTitle(title))
	{
		m_pDlg->m_DlgFixup.push_back(
			ARBConfigActionDeleteTitle::New(0, m_pDlg->m_pVenue->GetName(), std::wstring(), title, std::wstring()));
		m_pDlg->m_ctrlItems->Delete(GetId());
		return true;
	}
	return false;
}


bool CDlgConfigureDataTitle::DoCopy()
{
	bool bAdded = false;
	std::wstring name(m_Title->GetName());
	std::wstring longname(m_Title->GetLongName());
	while (m_pDlg->m_pVenue->GetTitles().FindTitle(name))
	{
		name = fmt::format(_("IDS_COPYOF").wx_str(), name);
		longname = fmt::format(_("IDS_COPYOF").wx_str(), longname);
	}
	ARBConfigTitlePtr title = m_Title->Clone();
	title->SetName(name);
	title->SetLongName(longname);
	if (m_pDlg->m_pVenue->GetTitles().AddTitle(title))
	{
		CDlgConfigureDataTitle* pData = new CDlgConfigureDataTitle(m_pDlg, title);
		wxTreeItemId id = m_pDlg->m_ctrlItems->AppendItem(
			m_pDlg->m_ctrlItems->GetItemParent(GetId()),
			StringUtil::stringWX(pData->OnNeedText()),
			-1,
			-1,
			pData);
		pData->AddSubItems();
		m_pDlg->m_ctrlItems->SelectItem(id);
		bAdded = true;
	}
	return bAdded;
}


CDlgConfigureDataBase* CDlgConfigureDataTitle::DoMove(bool bUp)
{
	if (m_pDlg->m_pVenue->GetTitles().Move(m_Title, bUp ? -1 : 1))
		return new CDlgConfigureDataTitle(m_pDlg, m_Title);
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataEvent::CDlgConfigureDataEvent(CDlgConfigVenue* pDlg, ARBConfigEventPtr const& inEvent)
	: CDlgConfigureDataBase(pDlg)
	, m_Event(inEvent)
{
}


std::wstring CDlgConfigureDataEvent::OnNeedText() const
{
	return OnNeedText(0);
}


std::wstring CDlgConfigureDataEvent::OnNeedText(int iColumn) const
{
	return m_Event->GetName();
}


bool CDlgConfigureDataEvent::DoEdit()
{
	bool bEdited = false;
	std::wstring oldName = m_Event->GetName();
	CDlgConfigEvent dlg(false, m_pDlg->m_pVenue, m_Event, m_pDlg);
	if (wxID_OK == dlg.ShowModal())
	{
		m_pDlg->m_pVenue->GetMultiQs().RenameEvent(oldName, m_Event->GetName());
		dlg.GetFixups(m_pDlg->m_DlgFixup);
		RefreshTreeItem(m_pDlg->m_ctrlItems, GetId());
		bEdited = true;
	}
	return bEdited;
}


bool CDlgConfigureDataEvent::DoDelete()
{
	std::wstring evt = m_Event->GetName();
	if (m_pDlg->m_pVenue->GetEvents().DeleteEvent(evt))
	{
		m_pDlg->m_pVenue->GetMultiQs().DeleteEvent(evt);
		m_pDlg->m_DlgFixup.push_back(ARBConfigActionDeleteEvent::New(0, m_pDlg->m_pVenue->GetName(), evt));
		m_pDlg->m_ctrlItems->Delete(GetId());
		return true;
	}
	return false;
}


bool CDlgConfigureDataEvent::DoCopy()
{
	bool bAdded = false;
	std::wstring name(m_Event->GetName());
	while (m_pDlg->m_pVenue->GetEvents().FindEvent(name))
	{
		name = fmt::format(_("IDS_COPYOF").wx_str(), name);
	}
	ARBConfigEventPtr pEvent = m_Event->Clone();
	pEvent->SetName(name);
	if (m_pDlg->m_pVenue->GetEvents().AddEvent(pEvent))
	{
		CDlgConfigureDataEvent* pData = new CDlgConfigureDataEvent(m_pDlg, pEvent);
		wxTreeItemId id = m_pDlg->m_ctrlItems->AppendItem(
			m_pDlg->m_ctrlItems->GetItemParent(GetId()),
			StringUtil::stringWX(pData->OnNeedText()),
			-1,
			-1,
			pData);
		pData->AddSubItems();
		m_pDlg->m_ctrlItems->SelectItem(id);
		bAdded = true;
	}
	return bAdded;
}


CDlgConfigureDataBase* CDlgConfigureDataEvent::DoMove(bool bUp)
{
	if (m_pDlg->m_pVenue->GetEvents().Move(m_Event, bUp ? -1 : 1))
		return new CDlgConfigureDataEvent(m_pDlg, m_Event);
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataLifetimeName::CDlgConfigureDataLifetimeName(
	CDlgConfigVenue* pDlg,
	ARBConfigLifetimeNamePtr const& inName)
	: CDlgConfigureDataBase(pDlg)
	, m_pName(inName)
{
}


std::wstring CDlgConfigureDataLifetimeName::OnNeedText() const
{
	return OnNeedText(0);
}


std::wstring CDlgConfigureDataLifetimeName::OnNeedText(int iColumn) const
{
	std::wstring str = m_pName->GetName();
	if (str.empty())
		str = StringUtil::stringW(_("IDS_TITLEPOINT_LIFETIME_NAME"));
	return str;
}


bool CDlgConfigureDataLifetimeName::DoEdit()
{
	bool bEdited = false;
	std::wstring oldName = m_pName->GetName();
	std::wstring name(oldName);
	bool done = false;
	while (!done)
	{
		done = true;
		CDlgConfigLifetimeName dlg(m_pDlg->m_pVenue, name);
		if (wxID_OK == dlg.ShowModal())
		{
			m_pName->SetName(name);
			m_pDlg->m_pVenue->GetEvents().RenameLifetimeName(oldName, name);
			m_pDlg->m_DlgFixup.push_back(
				ARBConfigActionRenameLifetimeName::New(0, m_pDlg->m_pVenue->GetName(), oldName, name));
			RefreshTreeItem(m_pDlg->m_ctrlItems, GetId());
			bEdited = true;
		}
	}
	return bEdited;
}


bool CDlgConfigureDataLifetimeName::DoDelete()
{
	std::wstring name = m_pName->GetName();
	if (m_pDlg->m_pVenue->GetLifetimeNames().DeleteLifetimeName(name))
	{
		m_pDlg->m_pVenue->GetEvents().DeleteLifetimeName(name);
		m_pDlg->m_DlgFixup.push_back(ARBConfigActionDeleteLifetimeName::New(0, m_pDlg->m_pVenue->GetName(), name));
		m_pDlg->m_ctrlItems->Delete(GetId());
		return true;
	}
	return false;
}


bool CDlgConfigureDataLifetimeName::DoCopy()
{
	bool bAdded = false;
	std::wstring name(m_pName->GetName());
	while (m_pDlg->m_pVenue->GetLifetimeNames().FindLifetimeName(name))
	{
		name = fmt::format(_("IDS_COPYOF").wx_str(), name);
	}
	ARBConfigLifetimeNamePtr lifetimename;
	if (m_pDlg->m_pVenue->GetLifetimeNames().AddLifetimeName(name, &lifetimename))
	{
		CDlgConfigureDataLifetimeName* pData = new CDlgConfigureDataLifetimeName(m_pDlg, lifetimename);
		wxTreeItemId id = m_pDlg->m_ctrlItems->AppendItem(
			m_pDlg->m_ctrlItems->GetItemParent(GetId()),
			StringUtil::stringWX(pData->OnNeedText()),
			-1,
			-1,
			pData);
		pData->AddSubItems();
		m_pDlg->m_ctrlItems->SelectItem(id);
		bAdded = true;
	}
	return bAdded;
}


CDlgConfigureDataBase* CDlgConfigureDataLifetimeName::DoMove(bool bUp)
{
	if (m_pDlg->m_pVenue->GetLifetimeNames().Move(m_pName, bUp ? -1 : 1))
		return new CDlgConfigureDataLifetimeName(m_pDlg, m_pName);
	return nullptr;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataMultiQ::CDlgConfigureDataMultiQ(CDlgConfigVenue* pDlg, ARBConfigMultiQPtr const& inMultiq)
	: CDlgConfigureDataBase(pDlg)
	, m_MultiQ(inMultiq)
{
}


std::wstring CDlgConfigureDataMultiQ::OnNeedText() const
{
	return OnNeedText(0);
}


std::wstring CDlgConfigureDataMultiQ::OnNeedText(int iColumn) const
{
	return m_MultiQ->GetName();
}


bool CDlgConfigureDataMultiQ::DoEdit()
{
	bool bEdited = false;
	std::wstring oldName = m_MultiQ->GetName();
	bool done = false;
	while (!done)
	{
		done = true;
		CDlgConfigMultiQ dlg(m_pDlg->m_pVenue, m_MultiQ, m_pDlg);
		if (wxID_OK == dlg.ShowModal())
		{
			std::wstring name = m_MultiQ->GetName();
			if (name != oldName)
				m_pDlg->m_DlgFixup.push_back(
					ARBConfigActionRenameMultiQ::New(0, m_pDlg->m_pVenue->GetName(), oldName, name));
			RefreshTreeItem(m_pDlg->m_ctrlItems, GetId());
			bEdited = true;
		}
	}
	return bEdited;
}


bool CDlgConfigureDataMultiQ::DoDelete()
{
	std::wstring multiQ = m_MultiQ->GetName();
	if (m_pDlg->m_pVenue->GetMultiQs().DeleteMultiQ(m_MultiQ))
	{
		m_pDlg->m_DlgFixup.push_back(ARBConfigActionDeleteMultiQ::New(0, m_pDlg->m_pVenue->GetName(), multiQ));
		m_pDlg->m_ctrlItems->Delete(GetId());
		return true;
	}
	return false;
}


bool CDlgConfigureDataMultiQ::DoCopy()
{
	bool bAdded = false;
	std::wstring name(m_MultiQ->GetName());
	while (m_pDlg->m_pVenue->GetMultiQs().FindMultiQ(name))
	{
		name = fmt::format(_("IDS_COPYOF").wx_str(), name);
	}
	ARBConfigMultiQPtr multiq = m_MultiQ->Clone();
	multiq->SetName(name);
	if (m_pDlg->m_pVenue->GetMultiQs().AddMultiQ(multiq))
	{
		CDlgConfigureDataMultiQ* pData = new CDlgConfigureDataMultiQ(m_pDlg, multiq);
		wxTreeItemId id = m_pDlg->m_ctrlItems->AppendItem(
			m_pDlg->m_ctrlItems->GetItemParent(GetId()),
			StringUtil::stringWX(pData->OnNeedText()),
			-1,
			-1,
			pData);
		pData->AddSubItems();
		m_pDlg->m_ctrlItems->SelectItem(id);
		bAdded = true;
	}
	return bAdded;
}


CDlgConfigureDataBase* CDlgConfigureDataMultiQ::DoMove(bool bUp)
{
	if (m_pDlg->m_pVenue->GetMultiQs().Move(m_MultiQ, bUp ? -1 : 1))
		return new CDlgConfigureDataMultiQ(m_pDlg, m_MultiQ);
	return nullptr;
}

} // namespace dconSoft
