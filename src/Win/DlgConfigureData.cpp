/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CDlgConfigure class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-01-10 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-04-02 DRC Up the ref count to prevent problems. See comments below.
 * @li 2004-01-14 DRC Use complete title name instead of nice name. When
 *                    renaming a title and that name is in use, prompt to merge.
 * @li 2003-02-23 DRC Modifying the scoring methods wasn't updated in the tree.
 */

#include "stdafx.h"
#include "DlgConfigureData.h"

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "ARBConfigDivision.h"
#include "ARBConfigVenue.h"
#include "DlgConfigEvent.h"
#include "DlgConfigMultiQ.h"
#include "DlgConfigOtherPoints.h"
#include "DlgConfigTitle.h"
#include "DlgConfigure.h"
#include "DlgConfigVenue.h"
#include "DlgName.h"
#include "DlgReorder.h"
#include "Globals.h"
#include "Widgets.h"


////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataBase::CDlgConfigureDataBase(CDlgConfigVenue* pDlg)
	: m_pDlg(pDlg)
{
}

////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataVenue::CDlgConfigureDataVenue(ARBConfigVenuePtr venue)
	: CDlgConfigureDataBase(NULL)
	, m_Venue(venue)
{
}


wxString CDlgConfigureDataVenue::OnNeedText() const
{
	wxString str(OnNeedText(0));
	for (int i = 1; i <= 2; ++i)
	{
		wxString s(OnNeedText(i));
		if (!s.empty())
		{
			str += wxT("  ") + s;
		}
	}
	return str;
}


wxString CDlgConfigureDataVenue::OnNeedText(int iColumn) const
{
	wxString str;
	switch (iColumn)
	{
	case 0:
		str = m_Venue->GetName();
		break;
	case 1:
		str = m_Venue->GetURL();
		break;
	case 2:
		str = m_Venue->GetDesc();
		str.Replace(wxT("\n"), wxT(" "));
		break;
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataFault::CDlgConfigureDataFault(ARBConfigFaultPtr fault)
	: CDlgConfigureDataBase(NULL)
	, m_Fault(fault)
{
}


wxString CDlgConfigureDataFault::OnNeedText() const
{
	return OnNeedText(0);
}


wxString CDlgConfigureDataFault::OnNeedText(int iColumn) const
{
	return m_Fault->GetName();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataOtherPoints::CDlgConfigureDataOtherPoints(ARBConfigOtherPointsPtr otherPoints)
	: CDlgConfigureDataBase(NULL)
	, m_OtherPoints(otherPoints)
{
}


wxString CDlgConfigureDataOtherPoints::OnNeedText() const
{
	return OnNeedText(0);
}


wxString CDlgConfigureDataOtherPoints::OnNeedText(int iColumn) const
{
	return m_OtherPoints->GetName();
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataDivision::CDlgConfigureDataDivision(
		CDlgConfigVenue* pDlg,
		ARBConfigDivisionPtr div)
	: CDlgConfigureDataBase(pDlg)
	, m_Div(div)
{
}


wxString CDlgConfigureDataDivision::OnNeedText() const
{
	return OnNeedText(0);
}


wxString CDlgConfigureDataDivision::OnNeedText(int iColumn) const
{
	return m_Div->GetName();
}


void CDlgConfigureDataDivision::AddSubItems()
{
	for (ARBConfigLevelList::iterator iterLevel = m_Div->GetLevels().begin();
		iterLevel != m_Div->GetLevels().end();
		++iterLevel)
	{
		CDlgConfigureDataLevel* pLevData = new CDlgConfigureDataLevel(m_pDlg, m_Div, *iterLevel);
		m_pDlg->m_ctrlItems->AppendItem(GetId(), pLevData->OnNeedText(), -1, -1, pLevData);
		pLevData->AddSubItems();
	}
}


bool CDlgConfigureDataDivision::DoAdd()
{
	bool added = false;
	bool done = false;
	wxString name;
	while (!done)
	{
		done = true;
		CDlgName dlg(name, _("IDS_LEVEL_NAME"), m_pDlg);
		if (wxID_OK == dlg.ShowModal())
		{
			name = dlg.GetName();
			if (m_Div->GetLevels().FindSubLevel(name))
			{
				done = false;
				wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
				continue;
			}
			ARBConfigLevelPtr pNewLevel;
			if (m_Div->GetLevels().AddLevel(name, &pNewLevel))
			{
				CDlgConfigureDataLevel* pData = new CDlgConfigureDataLevel(m_pDlg, m_Div, pNewLevel);
				wxTreeItemId level = m_pDlg->m_ctrlItems->AppendItem(GetId(), pData->OnNeedText(), -1, -1, pData);
				m_pDlg->m_ctrlItems->SelectItem(level);
				added = true;
			}
		}
	}
	return added;
}

bool CDlgConfigureDataDivision::DoEdit()
{
	wxString oldName = m_Div->GetName();
	wxString name(oldName);
	if (0 < m_pDlg->m_Book.GetDogs().NumMultiHostedTrialsInDivision(m_pDlg->m_Book.GetConfig(), m_pDlg->m_pVenue->GetName(), name))
	{
		if (wxYES != wxMessageBox(_("IDS_CHANGEDIV_ISSUES"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_QUESTION | wxYES_NO))
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
			name = dlg.GetName();
			if (oldName != name)
			{
				if (m_pDlg->m_pVenue->GetDivisions().FindDivision(name))
				{
					done = false;
					wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
					continue;
				}
				m_Div->SetName(name);
				m_pDlg->m_pVenue->GetEvents().RenameDivision(oldName, name);
				m_pDlg->m_pVenue->GetMultiQs().RenameDivision(oldName, name);
				m_pDlg->m_DlgFixup.push_back(ARBConfigActionRenameDivision::New(m_pDlg->m_pVenue->GetName(), oldName, name));
				RefreshTreeItem(m_pDlg->m_ctrlItems, GetId());
				bEdited = true;
			}
		}
	}
	return bEdited;
}


bool CDlgConfigureDataDivision::DoDelete()
{
	wxString div = m_Div->GetName();
	// If we were able to delete it...
	if (m_pDlg->m_pVenue->GetDivisions().DeleteDivision(div, m_pDlg->m_pVenue->GetEvents()))
	{
		m_pDlg->m_pVenue->GetMultiQs().DeleteDivision(div);
		m_pDlg->m_DlgFixup.push_back(ARBConfigActionDeleteDivision::New(m_pDlg->m_pVenue->GetName(), div));
		m_pDlg->m_ctrlItems->Delete(GetId());
		return true;
	}
	return false;
}


CDlgConfigureDataBase* CDlgConfigureDataDivision::DoMove(bool bUp)
{
	if (m_pDlg->m_pVenue->GetDivisions().Move(m_Div, bUp ? -1 : 1))
		return new CDlgConfigureDataDivision(m_pDlg, m_Div);
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataLevel::CDlgConfigureDataLevel(
		CDlgConfigVenue* pDlg,
		ARBConfigDivisionPtr div,
		ARBConfigLevelPtr level)
	: CDlgConfigureDataBase(pDlg)
	, m_Division(div)
	, m_Level(level)
{
}


wxString CDlgConfigureDataLevel::OnNeedText() const
{
	return OnNeedText(0);
}


wxString CDlgConfigureDataLevel::OnNeedText(int iColumn) const
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
			CDlgConfigureDataSubLevel* pSubLevelData = new CDlgConfigureDataSubLevel(m_pDlg, m_Division, m_Level, *iterSubLevel);
			m_pDlg->m_ctrlItems->AppendItem(GetId(), pSubLevelData->OnNeedText(), -1, -1, pSubLevelData);
			pSubLevelData->AddSubItems();
		}
	}
}


bool CDlgConfigureDataLevel::DoAdd()
{
	bool added = false;
	bool done = false;
	wxString name;
	while (!done)
	{
		done = true;
		CDlgName dlg(name, _("IDS_SUBLEVEL_NAME"), m_pDlg);
		if (wxID_OK == dlg.ShowModal())
		{
			name = dlg.GetName();
			if (m_Division->GetLevels().FindSubLevel(name))
			{
				done = false;
				wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
				continue;
			}
			ARBConfigSubLevelPtr pNewSubLevel;
			if (m_Level->GetSubLevels().AddSubLevel(name, &pNewSubLevel))
			{
				CDlgConfigureDataSubLevel* pData = new CDlgConfigureDataSubLevel(m_pDlg, m_Division, m_Level, pNewSubLevel);
				wxTreeItemId level = m_pDlg->m_ctrlItems->AppendItem(GetId(), pData->OnNeedText(), -1, -1, pData);
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
	wxString oldName = m_Level->GetName();
	wxString name(oldName);
	// If there are sublevels, don't ask the following question. If a level has
	// sublevels, the level name isn't allowed to be used for an event.
	if (0 == m_Level->GetSubLevels().size())
	{
		if (0 < m_pDlg->m_Book.GetDogs().NumMultiHostedTrialsInDivision(m_pDlg->m_Book.GetConfig(), m_pDlg->m_pVenue->GetName(), m_Division->GetName()))
		{
			if (wxYES != wxMessageBox(_("IDS_CHANGELEVEL_ISSUES"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_QUESTION | wxYES_NO))
				return false;
		}
	}
	while (!done)
	{
		done = true;
		CDlgName dlg(name, _("IDS_LEVEL_NAME"), m_pDlg);
		if (wxID_OK == dlg.ShowModal())
		{
			name = dlg.GetName();
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
					wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
					continue;
				}
				m_Level->SetName(name);
				if (0 == m_Level->GetSubLevels().size())
				{
					m_pDlg->m_pVenue->GetEvents().RenameLevel(
						m_Division->GetName(),
						oldName, name);
					m_pDlg->m_pVenue->GetMultiQs().RenameLevel(
						m_Division->GetName(),
						oldName, name);
				}
				m_pDlg->m_DlgFixup.push_back(ARBConfigActionRenameLevel::NewLevel(
					m_pDlg->m_pVenue->GetName(),
					m_Division->GetName(),
					oldName, m_Level->GetName()));
				RefreshTreeItem(m_pDlg->m_ctrlItems, GetId());
				bEdited = true;
			}
		}
	}
	return bEdited;
}


bool CDlgConfigureDataLevel::DoDelete()
{
	wxString level = m_Level->GetName();
	if (m_Division->GetLevels().DeleteLevel(m_Division->GetName(), level, m_pDlg->m_pVenue->GetEvents()))
	{
		m_pDlg->m_pVenue->GetMultiQs().DeleteLevel(level);
		m_pDlg->m_DlgFixup.push_back(ARBConfigActionDeleteLevel::NewLevel(
			m_pDlg->m_pVenue->GetName(),
			m_Division->GetName(),
			level));
		m_pDlg->m_ctrlItems->Delete(GetId());
		return true;
	}
	return false;
}


CDlgConfigureDataBase* CDlgConfigureDataLevel::DoMove(bool bUp)
{
	if (m_Division->GetLevels().Move(m_Level, bUp ? -1 : 1))
		return new CDlgConfigureDataLevel(m_pDlg, m_Division, m_Level);
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataSubLevel::CDlgConfigureDataSubLevel(
		CDlgConfigVenue* pDlg,
		ARBConfigDivisionPtr div,
		ARBConfigLevelPtr level,
		ARBConfigSubLevelPtr subLevel)
	: CDlgConfigureDataBase(pDlg)
	, m_Division(div)
	, m_Level(level)
	, m_SubLevel(subLevel)
{
}


wxString CDlgConfigureDataSubLevel::OnNeedText() const
{
	return OnNeedText(0);
}


wxString CDlgConfigureDataSubLevel::OnNeedText(int iColumn) const
{
	return m_SubLevel->GetName();
}


bool CDlgConfigureDataSubLevel::DoEdit()
{
	bool bEdited = false;
	bool done = false;
	wxString oldName = m_SubLevel->GetName();
	wxString name(oldName);
	if (0 < m_pDlg->m_Book.GetDogs().NumMultiHostedTrialsInDivision(m_pDlg->m_Book.GetConfig(), m_pDlg->m_pVenue->GetName(), m_Division->GetName()))
	{
		if (wxYES != wxMessageBox(_("IDS_CHANGESUBLEVEL_ISSUES"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_QUESTION | wxYES_NO))
			return false;
	}
	while (!done)
	{
		done = true;
		CDlgName dlg(name, _("IDS_SUBLEVEL_NAME"), m_pDlg);
		if (wxID_OK == dlg.ShowModal())
		{
			name = dlg.GetName();
			if (oldName != name)
			{
				if (m_Division->GetLevels().FindSubLevel(name))
				{
					done = false;
					wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
					continue;
				}
				m_SubLevel->SetName(name);
				// No need to fix ARBConfigEventList cause we don't do sublevel names in events.
				m_pDlg->m_pVenue->GetMultiQs().RenameLevel(
					m_Division->GetName(),
					oldName, m_SubLevel->GetName());
				m_pDlg->m_DlgFixup.push_back(ARBConfigActionRenameLevel::NewSubLevel(
					m_pDlg->m_pVenue->GetName(),
					m_Division->GetName(),
					m_Level->GetName(),
					oldName, m_SubLevel->GetName()));
				RefreshTreeItem(m_pDlg->m_ctrlItems, GetId());
				bEdited = true;
			}
		}
	}
	return bEdited;
}


bool CDlgConfigureDataSubLevel::DoDelete()
{
	wxString level = m_Level->GetName();
	wxString subLevel = m_SubLevel->GetName();
	bool bLevelModified = false;
	if (m_Division->GetLevels().DeleteSubLevel(subLevel, bLevelModified))
	{
		m_pDlg->m_pVenue->GetMultiQs().DeleteLevel(subLevel);
		// Note, if deleting the sublevel caused the level's name
		// to change, just leave it. It causes more trouble to
		// try modifing the name to the old sublevel name.
		m_pDlg->m_DlgFixup.push_back(ARBConfigActionDeleteLevel::NewSubLevel(
			m_pDlg->m_pVenue->GetName(),
			m_Division->GetName(),
			level, subLevel));
		m_pDlg->m_ctrlItems->Delete(GetId());
		return true;
	}
	return false;
}


CDlgConfigureDataBase* CDlgConfigureDataSubLevel::DoMove(bool bUp)
{
	if (m_Level->GetSubLevels().Move(m_SubLevel, bUp ? -1 : 1))
		return new CDlgConfigureDataSubLevel(m_pDlg, m_Division, m_Level, m_SubLevel);
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataTitle::CDlgConfigureDataTitle(
		CDlgConfigVenue* pDlg,
		ARBConfigTitlePtr title)
	: CDlgConfigureDataBase(pDlg)
	, m_Title(title)
{
}


wxString CDlgConfigureDataTitle::OnNeedText() const
{
	return OnNeedText(0);
}


wxString CDlgConfigureDataTitle::OnNeedText(int iColumn) const
{
	return m_Title->GetCompleteName(-1, false, true, true);
}


bool CDlgConfigureDataTitle::DoEdit()
{
	bool bEdited = false;
	bool done = false;
	wxString oldName = m_Title->GetName();
	wxString oldLongName = m_Title->GetLongName();
	wxString name(oldName);
	wxString longname(oldLongName);
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
						if (wxYES == wxMessageBox(_("IDS_NAME_IN_USE_MERGE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_QUESTION | wxYES_NO))
						{
							bInUse = false;
							m_pDlg->m_DlgFixup.push_back(ARBConfigActionRenameTitle::New(m_pDlg->m_pVenue->GetName(), oldName, name));
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
						wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
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
					m_pDlg->m_DlgFixup.push_back(ARBConfigActionRenameTitle::New(m_pDlg->m_pVenue->GetName(), oldName, name));
				RefreshTreeItem(m_pDlg->m_ctrlItems, GetId());
			}
			bEdited = true;
		}
	}
	return bEdited;
}


bool CDlgConfigureDataTitle::DoDelete()
{
	wxString title = m_Title->GetName();
	if (m_pDlg->m_pVenue->GetTitles().DeleteTitle(title))
	{
		m_pDlg->m_DlgFixup.push_back(ARBConfigActionDeleteTitle::New(m_pDlg->m_pVenue->GetName(), wxString(), title, wxString()));
		m_pDlg->m_ctrlItems->Delete(GetId());
		return true;
	}
	return false;
}


bool CDlgConfigureDataTitle::DoCopy()
{
	bool bAdded = false;
	wxString name(m_Title->GetName());
	wxString longname(m_Title->GetLongName());
	while (m_pDlg->m_pVenue->GetTitles().FindTitle(name))
	{
		name = wxString::Format(_("IDS_COPYOF"), name.c_str());
		longname = wxString::Format(_("IDS_COPYOF"), longname.c_str());
	}
	ARBConfigTitlePtr title = m_Title->Clone();
	title->SetName(name);
	title->SetLongName(longname);
	if (m_pDlg->m_pVenue->GetTitles().AddTitle(title))
	{
		CDlgConfigureDataTitle* pData = new CDlgConfigureDataTitle(m_pDlg, title);
		wxTreeItemId id = m_pDlg->m_ctrlItems->AppendItem(m_pDlg->m_ctrlItems->GetItemParent(GetId()), pData->OnNeedText(), -1, -1, pData);
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
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataEvent::CDlgConfigureDataEvent(
		CDlgConfigVenue* pDlg,
		ARBConfigEventPtr inEvent)
	: CDlgConfigureDataBase(pDlg)
	, m_Event(inEvent)
{
}


wxString CDlgConfigureDataEvent::OnNeedText() const
{
	return OnNeedText(0);
}


wxString CDlgConfigureDataEvent::OnNeedText(int iColumn) const
{
	return m_Event->GetName();
}


bool CDlgConfigureDataEvent::DoEdit()
{
	bool bEdited = false;
	wxString oldName = m_Event->GetName();
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
	wxString evt = m_Event->GetName();
	if (m_pDlg->m_pVenue->GetEvents().DeleteEvent(evt))
	{
		m_pDlg->m_pVenue->GetMultiQs().DeleteEvent(evt);
		m_pDlg->m_DlgFixup.push_back(ARBConfigActionDeleteEvent::New(m_pDlg->m_pVenue->GetName(), evt));
		m_pDlg->m_ctrlItems->Delete(GetId());
		return true;
	}
	return false;
}


bool CDlgConfigureDataEvent::DoCopy()
{
	bool bAdded = false;
	wxString name(m_Event->GetName());
	while (m_pDlg->m_pVenue->GetEvents().FindEvent(name))
	{
		name = wxString::Format(_("IDS_COPYOF"), name.c_str());
	}
	ARBConfigEventPtr pEvent = m_Event->Clone();
	pEvent->SetName(name);
	if (m_pDlg->m_pVenue->GetEvents().AddEvent(pEvent))
	{
		CDlgConfigureDataEvent* pData = new CDlgConfigureDataEvent(m_pDlg, pEvent);
		wxTreeItemId id = m_pDlg->m_ctrlItems->AppendItem(m_pDlg->m_ctrlItems->GetItemParent(GetId()), pData->OnNeedText(), -1, -1, pData);
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
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////

CDlgConfigureDataMultiQ::CDlgConfigureDataMultiQ(
		CDlgConfigVenue* pDlg,
		ARBConfigMultiQPtr multiq)
	: CDlgConfigureDataBase(pDlg)
	, m_MultiQ(multiq)
{
}


wxString CDlgConfigureDataMultiQ::OnNeedText() const
{
	return OnNeedText(0);
}


wxString CDlgConfigureDataMultiQ::OnNeedText(int iColumn) const
{
	return m_MultiQ->GetName();
}


bool CDlgConfigureDataMultiQ::DoEdit()
{
	bool bEdited = false;
	wxString oldName = m_MultiQ->GetName();
	bool done = false;
	while (!done)
	{
		done = true;
		CDlgConfigMultiQ dlg(m_pDlg->m_pVenue, m_MultiQ, m_pDlg);
		if (wxID_OK == dlg.ShowModal())
		{
			wxString name = m_MultiQ->GetName();
			if (name != oldName)
				m_pDlg->m_DlgFixup.push_back(ARBConfigActionRenameMultiQ::New(m_pDlg->m_pVenue->GetName(), oldName, name));
			RefreshTreeItem(m_pDlg->m_ctrlItems, GetId());
			bEdited = true;
		}
	}
	return bEdited;
}


bool CDlgConfigureDataMultiQ::DoDelete()
{
	wxString multiQ = m_MultiQ->GetName();
	if (m_pDlg->m_pVenue->GetMultiQs().DeleteMultiQ(m_MultiQ))
	{
		m_pDlg->m_DlgFixup.push_back(ARBConfigActionDeleteMultiQ::New(m_pDlg->m_pVenue->GetName(), multiQ));
		m_pDlg->m_ctrlItems->Delete(GetId());
		return true;
	}
	return false;
}


bool CDlgConfigureDataMultiQ::DoCopy()
{
	bool bAdded = false;
	wxString name(m_MultiQ->GetName());
	while (m_pDlg->m_pVenue->GetMultiQs().FindMultiQ(name))
	{
		name = wxString::Format(_("IDS_COPYOF"), name.c_str());
	}
	ARBConfigMultiQPtr multiq = m_MultiQ->Clone();
	multiq->SetName(name);
	if (m_pDlg->m_pVenue->GetMultiQs().AddMultiQ(multiq))
	{
		CDlgConfigureDataMultiQ* pData = new CDlgConfigureDataMultiQ(m_pDlg, multiq);
		wxTreeItemId id = m_pDlg->m_ctrlItems->AppendItem(m_pDlg->m_ctrlItems->GetItemParent(GetId()), pData->OnNeedText(), -1, -1, pData);
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
	return NULL;
}
