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
 * 2016-06-19 Add a blank icon to fix issue on Mac.
 * 2015-01-01 Changed pixels to dialog units.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-10 Ported to wxWidgets.
 * 2007-08-19 Simplified UI layout into a single tree.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2006-02-08 Added 'RenameEvent' action.
 * 2005-12-14 Moved 'Titles' to 'Venue'.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-03-26 Added code to migrate runs to the new table-in-run form.
 * 2004-02-18 Added 'DeleteTitle' configuration action.
 * 2004-01-21 Implemented Action items in configuration update.
 * 2004-01-14 Use complete title name instead of nice name.
 * 2003-12-27 Changed the scoring method to show the valid date range.
 * 2003-02-04 Changed dbl-click to edit an item.
 */

#include "stdafx.h"
#include "DlgConfigure.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "DlgConfigOtherPoints.h"
#include "DlgConfigUpdate.h"
#include "DlgConfigureData.h"
#include "DlgConfigVenue.h"
#include "DlgListViewer.h"
#include "DlgMessage.h"
#include "DlgMessageBox.h"
#include "DlgName.h"
#include "Globals.h"
#include "Widgets.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBConfigVenue.h"
#include "ARBCommon/StringUtil.h"
#include <wx/artprov.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataRoot : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataRoot(CDlgConfigure::eAction action)
		: CDlgConfigureDataBase(nullptr)
		, m_Action(action)
	{
	}
	virtual std::wstring OnNeedText() const	{return std::wstring();}
	CDlgConfigure::eAction GetAction() const
	{
		return m_Action;
	}
private:
	CDlgConfigure::eAction m_Action;
};

/////////////////////////////////////////////////////////////////////////////

// TODO: This doesn't seem to be used. Can it be deleted?
// Also affects DlgMessageBox.*
class CDetails : public IMessageBoxCallback
{
	DECLARE_NO_COPY_IMPLEMENTED(CDetails)
public:
	CDetails(
			CAgilityBookDoc* inDoc,
			std::list<ScoringRunInfo> const& inScoringRuns)
		: m_pDoc(inDoc)
		, m_ScoringRuns(inScoringRuns)
	{
	}
	virtual void OnDetails(wxWindow* pParent);
protected:
	CAgilityBookDoc* m_pDoc;
	std::list<ScoringRunInfo> const& m_ScoringRuns;
};


void CDetails::OnDetails(wxWindow* pParent)
{
	std::wstring str = StringUtil::stringW(_("IDS_AFFECTED_RUNS"));
	CDlgListViewer dlg(m_pDoc, str, m_ScoringRuns, pParent);
	dlg.ShowModal();
}

/////////////////////////////////////////////////////////////////////////////
// CDlgConfigure dialog

BEGIN_EVENT_TABLE(CDlgConfigure, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgConfigure::OnOk)
END_EVENT_TABLE()


CDlgConfigure::CDlgConfigure(
		CAgilityBookDoc* pDoc,
		ARBAgilityRecordBook& book,
		wxWindow* pParent)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_Book(book)
	, m_Config(m_Book.GetConfig())
	, m_ImageList()
	, m_idxFaults(-1)
	, m_ctrlItems(nullptr)
	, m_ctrlNew(nullptr)
	, m_ctrlEdit(nullptr)
	, m_ctrlDelete(nullptr)
	, m_ctrlCopy(nullptr)
	, m_hItemVenues()
	, m_hItemFaults()
	, m_hItemOtherPts()
{
	m_idxFaults = m_ImageList.Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, m_ImageList.GetSize()));
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_CONFIGURE"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	assert(m_pDoc);

	// Controls (these are done first to control tab order)

	m_ctrlItems = new CTreeCtrl(this, wxID_ANY,
		wxDefaultPosition, wxDLG_UNIT(this, wxSize(215, 215)),
		wxTR_FULL_ROW_HIGHLIGHT|wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|wxTR_SINGLE);
	m_ctrlItems->SetImageList(&m_ImageList);
	BIND_OR_CONNECT_CTRL(m_ctrlItems, wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler, CDlgConfigure::OnSelectionChanged);
	BIND_OR_CONNECT_CTRL(m_ctrlItems, wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler, CDlgConfigure::OnItemActivated);
	m_ctrlItems->SetHelpText(_("HIDC_CONFIG_ITEMS"));
	m_ctrlItems->SetToolTip(_("HIDC_CONFIG_ITEMS"));

	m_ctrlNew = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlNew, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgConfigure::OnNew);
	m_ctrlNew->SetHelpText(_("HIDC_CONFIG_NEW"));
	m_ctrlNew->SetToolTip(_("HIDC_CONFIG_NEW"));

	m_ctrlEdit = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlEdit, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgConfigure::OnEdit);
	m_ctrlEdit->SetHelpText(_("HIDC_CONFIG_EDIT"));
	m_ctrlEdit->SetToolTip(_("HIDC_CONFIG_EDIT"));

	m_ctrlDelete = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlDelete, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgConfigure::OnDelete);
	m_ctrlDelete->SetHelpText(_("HIDC_CONFIG_DELETE"));
	m_ctrlDelete->SetToolTip(_("HIDC_CONFIG_DELETE"));

	m_ctrlCopy = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_COPY"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlCopy, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgConfigure::OnCopy);
	m_ctrlCopy->SetHelpText(_("HIDC_CONFIG_COPY"));
	m_ctrlCopy->SetToolTip(_("HIDC_CONFIG_COPY"));

	wxButton* btnUpdate = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_UPDATE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(btnUpdate, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgConfigure::OnUpdate);
	btnUpdate->SetHelpText(_("HIDC_CONFIG_UPDATE"));
	btnUpdate->SetToolTip(_("HIDC_CONFIG_UPDATE"));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerConfig = new wxBoxSizer(wxHORIZONTAL);
	sizerConfig->Add(m_ctrlItems, 1, wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxVERTICAL);
	sizerBtns->Add(m_ctrlNew, 0, 0, 0);
	sizerBtns->Add(m_ctrlEdit, 0, wxTOP, wxDLG_UNIT_X(this, 3));
	sizerBtns->Add(m_ctrlDelete, 0, wxTOP, wxDLG_UNIT_X(this, 3));
	sizerBtns->Add(m_ctrlCopy, 0, wxTOP, wxDLG_UNIT_X(this, 3));

	sizerConfig->Add(sizerBtns, 0, wxEXPAND, 0);

	bSizer->Add(sizerConfig, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerUpdate = new wxBoxSizer(wxHORIZONTAL);
	sizerUpdate->Add(0, 0, 1, wxEXPAND, 0);
	sizerUpdate->Add(btnUpdate, 0, 0, 0);

	bSizer->Add(sizerUpdate, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	wxTreeItemId root = m_ctrlItems->AddRoot(L"Root");

	m_hItemVenues = m_ctrlItems->AppendItem(root, _("IDS_COL_VENUES"), m_ImageList.IndexARB(), m_ImageList.IndexARB(), new CDlgConfigureDataRoot(eVenues));
	LoadData(eVenues);
	m_ctrlItems->Expand(m_hItemVenues);

	m_hItemFaults = m_ctrlItems->AppendItem(root, _("IDS_COL_FAULTS"), m_idxFaults, m_idxFaults, new CDlgConfigureDataRoot(eFaults));
	LoadData(eFaults);

	m_hItemOtherPts = m_ctrlItems->AppendItem(root, _("IDS_OTHERPOINTS"), m_ImageList.IndexRun(), m_ImageList.IndexRun(), new CDlgConfigureDataRoot(eOtherPoints));
	LoadData(eOtherPoints);

	m_ctrlItems->SelectItem(m_hItemVenues);
	m_ctrlItems->EnsureVisible(m_hItemVenues);
	UpdateButtons();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	m_ctrlItems->SetFocus();
}


CDlgConfigure::~CDlgConfigure()
{
	// Unhook the selection event. DeleteAllItems triggers this and wxWidgets
	// apparently didn't clean something up - the virtual root is trying
	// to access GetParam() [via our GetData] whichs dies horribly.
	UNBIND_OR_DISCONNECT_CTRL(m_ctrlItems, wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler, CDlgConfigure::OnSelectionChanged);
}



CDlgConfigure::eAction CDlgConfigure::GetAction() const
{
	wxTreeItemId hItem = m_ctrlItems->GetSelection();
	if (hItem.IsOk())
	{
		wxTreeItemId hParent = m_ctrlItems->GetItemParent(hItem);
		if (!hParent.IsOk() || hParent == m_ctrlItems->GetRootItem())
			hParent = hItem;
		CDlgConfigureDataRoot* pData = dynamic_cast<CDlgConfigureDataRoot*>(GetData(hParent));
		if (pData)
			return pData->GetAction();
	}
	return eNone;
}


CDlgConfigureDataBase* CDlgConfigure::GetData(wxTreeItemId hItem) const
{
	if (hItem.IsOk())
		return dynamic_cast<CDlgConfigureDataBase*>(m_ctrlItems->GetItemData(hItem));
	return nullptr;
}


void CDlgConfigure::UpdateButtons()
{
	bool bNew = false;
	bool bEdit = false;
	bool bDelete = false;
	bool bCopy = false;
	if (eNone != GetAction())
		bNew = true;
	CDlgConfigureDataBase* pData = GetData(m_ctrlItems->GetSelection());
	if (pData)
	{
		bEdit = pData->CanEdit();
		bDelete = pData->CanDelete();
		bCopy = pData->CanCopy();
	}
	m_ctrlNew->Enable(bNew);
	m_ctrlEdit->Enable(bEdit);
	m_ctrlDelete->Enable(bDelete);
	m_ctrlCopy->Enable(bCopy);
}


void CDlgConfigure::LoadData(eAction dataToLoad)
{
	wxTreeItemId hParent;
	switch (dataToLoad)
	{
	default:
		break;
	case eVenues:
		hParent = m_hItemVenues;
		break;
	case eFaults:
		hParent = m_hItemFaults;
		break;
	case eOtherPoints:
		hParent = m_hItemOtherPts;
		break;
	}
	if (!hParent.IsOk())
		return;
	wxTreeItemIdValue cookie;
	while (m_ctrlItems->ItemHasChildren(hParent))
		m_ctrlItems->Delete(m_ctrlItems->GetFirstChild(hParent, cookie));

	switch (dataToLoad)
	{
	default:
		break;
	case eVenues:
		{ // Scoped because of VC6's scoping of for-variables.
			for (ARBConfigVenueList::iterator iterVenue = m_Config.GetVenues().begin(); iterVenue != m_Config.GetVenues().end(); ++iterVenue)
			{
				int idx = (*iterVenue)->GetIcon();
				if (0 > idx || idx >= m_ImageList.GetImageCount())
					idx = m_ImageList.IndexEmpty();
				assert(0 <= idx && idx < m_ImageList.GetImageCount());
					CDlgConfigureDataVenue* pData = new CDlgConfigureDataVenue(*iterVenue);
				m_ctrlItems->AppendItem(
					hParent,
					StringUtil::stringWX(pData->OnNeedText()),
					idx, idx,
					pData);
			}
		}
		break;
	case eFaults:
		{
			for (ARBConfigFaultList::iterator iterFault = m_Config.GetFaults().begin(); iterFault != m_Config.GetFaults().end(); ++iterFault)
			{
				CDlgConfigureDataFault* pData = new CDlgConfigureDataFault(*iterFault);
				m_ctrlItems->AppendItem(
					hParent,
					StringUtil::stringWX(pData->OnNeedText()),
					m_idxFaults, m_idxFaults,
					pData);
			}
		}
		break;
	case eOtherPoints:
		{
			for (ARBConfigOtherPointsList::iterator iterOther = m_Config.GetOtherPoints().begin(); iterOther != m_Config.GetOtherPoints().end(); ++iterOther)
			{
				CDlgConfigureDataOtherPoints* pData = new CDlgConfigureDataOtherPoints(*iterOther);
				m_ctrlItems->AppendItem(
					hParent,
					StringUtil::stringWX(pData->OnNeedText()),
					m_ImageList.IndexRun(), m_ImageList.IndexRun(),
					pData);
			}
		}
		break;
	}
	m_ctrlItems->SortChildren(hParent);
}


void CDlgConfigure::DoEdit()
{
	wxTreeItemId current = m_ctrlItems->GetSelection();
	CDlgConfigureDataBase* pData = GetData(current);
	if (!pData || !pData->CanEdit())
		return;

	switch (GetAction())
	{
	default:
		break;

	case eVenues:
		{
			CDlgConfigureDataVenue* pVenueData = dynamic_cast<CDlgConfigureDataVenue*>(pData);
			CDlgConfigVenue dlg(m_Book, m_Config, pVenueData->GetVenue(), this);
			if (wxID_OK == dlg.ShowModal())
			{
				dlg.GetFixups(m_Config.GetActions());
				RefreshTreeItem(m_ctrlItems, current);
				m_ctrlItems->SortChildren(m_hItemVenues);
			}
		}
		break;

	case eFaults:
		{
			CDlgConfigureDataFault* pFaultData = dynamic_cast<CDlgConfigureDataFault*>(pData);
			bool done = false;
			std::wstring oldName = pFaultData->GetFault()->GetName();
			std::wstring name(oldName);
			while (!done)
			{
				done = true;
				CDlgName dlg(name, _("IDS_FAULT_TYPE_NAME"), this);
				if (wxID_OK == dlg.ShowModal())
				{
					name = dlg.Name();
					if (oldName != name)
					{
						if (m_Config.GetFaults().FindFault(name))
						{
							done = false;
							wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
							continue;
						}
						pFaultData->GetFault()->SetName(name);
						// No fixup necessary for faults.
						RefreshTreeItem(m_ctrlItems, current);
						m_ctrlItems->SortChildren(m_hItemFaults);
					}
				}
			}
		}
		break;

	case eOtherPoints:
		{
			CDlgConfigureDataOtherPoints* pOtherData = dynamic_cast<CDlgConfigureDataOtherPoints*>(pData);
			std::wstring oldName = pOtherData->GetOtherPoints()->GetName();
			CDlgConfigOtherPoints dlg(m_Config, pOtherData->GetOtherPoints(), this);
			if (wxID_OK == dlg.ShowModal())
			{
				if (pOtherData->GetOtherPoints()->GetName() != oldName)
				{
					// Other things may have changed, but we only care about the name for fixup.
					m_Config.GetActions().push_back(ARBConfigActionRenameOtherPoints::New(0, oldName, pOtherData->GetOtherPoints()->GetName()));
					RefreshTreeItem(m_ctrlItems, current);
					m_ctrlItems->SortChildren(m_hItemOtherPts);
				}
			}
		}
		break;
	}
}


wxTreeItemId CDlgConfigure::FindCurrentVenue(
		ARBConfigVenuePtr pVenue,
		bool bSet)
{
	wxTreeItemId hCurrent;
	wxTreeItemIdValue cookie;
	for (wxTreeItemId item = m_ctrlItems->GetFirstChild(m_hItemVenues, cookie);
		item.IsOk();
		item = m_ctrlItems->GetNextChild(m_hItemVenues, cookie))
	{
		CDlgConfigureDataVenue* pData = dynamic_cast<CDlgConfigureDataVenue*>(GetData(item));
		if (pData && pData->GetVenue() == pVenue)
		{
			hCurrent = item;
			break;
		}
	}
	if (bSet)
	{
		m_ctrlItems->SelectItem(hCurrent);
		if (hCurrent)
			m_ctrlItems->EnsureVisible(hCurrent);
	}
	return hCurrent;
}


wxTreeItemId CDlgConfigure::FindCurrentFault(
		ARBConfigFaultPtr pFault,
		bool bSet)
{
	wxTreeItemId hCurrent;
	wxTreeItemIdValue cookie;
	for (wxTreeItemId item = m_ctrlItems->GetFirstChild(m_hItemFaults, cookie);
		item.IsOk();
		item = m_ctrlItems->GetNextChild(m_hItemFaults, cookie))
	{
		CDlgConfigureDataFault* pData = dynamic_cast<CDlgConfigureDataFault*>(GetData(item));
		if (pData && pData->GetFault() == pFault)
		{
			hCurrent = item;
			break;
		}
	}
	if (bSet)
	{
		m_ctrlItems->SelectItem(hCurrent);
		if (hCurrent)
			m_ctrlItems->EnsureVisible(hCurrent);
	}
	return hCurrent;
}


wxTreeItemId CDlgConfigure::FindCurrentOtherPoints(
		ARBConfigOtherPointsPtr pOther,
		bool bSet)
{
	wxTreeItemId hCurrent;
	wxTreeItemIdValue cookie;
	for (wxTreeItemId item = m_ctrlItems->GetFirstChild(m_hItemOtherPts, cookie);
		item.IsOk();
		item = m_ctrlItems->GetNextChild(m_hItemOtherPts, cookie))
	{
		CDlgConfigureDataOtherPoints* pData = dynamic_cast<CDlgConfigureDataOtherPoints*>(GetData(item));
		if (pData && pData->GetOtherPoints() == pOther)
		{
			hCurrent = item;
			break;
		}
	}
	if (bSet)
	{
		m_ctrlItems->SelectItem(hCurrent);
		if (hCurrent)
			m_ctrlItems->EnsureVisible(hCurrent);
	}
	return hCurrent;
}


void CDlgConfigure::OnItemActivated(wxTreeEvent& evt)
{
	DoEdit();
}


void CDlgConfigure::OnSelectionChanged(wxTreeEvent& evt)
{
	UpdateButtons();
}


void CDlgConfigure::OnNew(wxCommandEvent& evt)
{
	switch (GetAction())
	{
	default:
		break;

	case eVenues:
		{
			ARBConfigVenuePtr pVenue(ARBConfigVenue::New());
			CDlgConfigVenue dlg(m_Book, m_Config, pVenue, this);
			if (wxID_OK == dlg.ShowModal())
			{
				if (m_Config.GetVenues().AddVenue(pVenue))
				{
					dlg.GetFixups(m_Config.GetActions());
					CDlgConfigureDataVenue* pData = new CDlgConfigureDataVenue(pVenue);
					m_ctrlItems->AppendItem(
						m_hItemVenues,
						StringUtil::stringWX(pData->OnNeedText()),
						m_ImageList.IndexARB(), m_ImageList.IndexARB(),
						pData);
					m_ctrlItems->SortChildren(m_hItemVenues);
					FindCurrentVenue(pVenue, true);
				}
			}
		}
		break;

	case eFaults:
		{
			CDlgName dlg(std::wstring(), _("IDS_FAULT_TYPE_NAME"), this);
			if (wxID_OK == dlg.ShowModal())
			{
				std::wstring name = dlg.Name();
				// We could check for uniqueness, but if the user wants 2
				// strings the same, why argue! Afterall, these strings
				// are only "helper" items to fill in other data.
				if (0 < name.length())
				{
					ARBConfigFaultPtr pNewFault;
					if (m_Config.GetFaults().AddFault(name, &pNewFault))
					{
						CDlgConfigureDataFault* pData = new CDlgConfigureDataFault(pNewFault);
						m_ctrlItems->AppendItem(
							m_hItemFaults,
							StringUtil::stringWX(pData->OnNeedText()),
							m_idxFaults, m_idxFaults,
							pData);
						m_ctrlItems->SortChildren(m_hItemFaults);
						FindCurrentFault(pNewFault, true);
					}
				}
			}
		}
		break;

	case eOtherPoints:
		{
			ARBConfigOtherPointsPtr pOther(ARBConfigOtherPoints::New());
			// The dialog will ensure uniqueness.
			CDlgConfigOtherPoints dlg(m_Config, pOther, this);
			if (wxID_OK == dlg.ShowModal())
			{
				if (m_Config.GetOtherPoints().AddOtherPoints(pOther))
				{
					CDlgConfigureDataOtherPoints* pData = new CDlgConfigureDataOtherPoints(pOther);
					m_ctrlItems->AppendItem(
						m_hItemOtherPts,
						StringUtil::stringWX(pData->OnNeedText()),
						m_ImageList.IndexRun(), m_ImageList.IndexRun(),
						pData);
					m_ctrlItems->SortChildren(m_hItemOtherPts);
					FindCurrentOtherPoints(pOther, true);
				}
			}
		}
		break;
	}
}


void CDlgConfigure::OnDelete(wxCommandEvent& evt)
{
	wxTreeItemId current = m_ctrlItems->GetSelection();
	CDlgConfigureDataBase* pData = GetData(current);
	if (!pData || !pData->CanDelete())
		return;

	switch (GetAction())
	{
	default:
		break;

	case eVenues:
		{
			CDlgConfigureDataVenue* pVenueData = dynamic_cast<CDlgConfigureDataVenue*>(pData);
			std::wstring venue = pVenueData->GetVenue()->GetName();
			// If we were able to delete it...
			if (m_Config.GetVenues().DeleteVenue(venue))
			{
				m_Config.GetActions().push_back(ARBConfigActionDeleteVenue::New(0, venue));
				m_ctrlItems->Delete(current);
			}
		}
		break;

	case eFaults:
		{
			CDlgConfigureDataFault* pFaultData = dynamic_cast<CDlgConfigureDataFault*>(pData);
			if (m_Config.GetFaults().DeleteFault(pFaultData->GetFault()->GetName()))
			{
				m_ctrlItems->Delete(current);
				// No fixup necessary for faults.
			}
		}
		break;

	case eOtherPoints:
		{
			CDlgConfigureDataOtherPoints* pOtherData = dynamic_cast<CDlgConfigureDataOtherPoints*>(pData);
			std::wstring otherPoints = pOtherData->GetOtherPoints()->GetName();
			if (m_Config.GetOtherPoints().DeleteOtherPoints(otherPoints))
			{
				m_Config.GetActions().push_back(ARBConfigActionDeleteOtherPoints::New(0, otherPoints));
				m_ctrlItems->Delete(current);
			}
		}
		break;
	}
}


void CDlgConfigure::OnEdit(wxCommandEvent& evt)
{
	DoEdit();
}


void CDlgConfigure::OnCopy(wxCommandEvent& evt)
{
	wxTreeItemId current = m_ctrlItems->GetSelection();
	CDlgConfigureDataBase* pData = GetData(current);
	if (!pData || !pData->CanCopy())
		return;

	switch (GetAction())
	{
	default:
		break;

	case eVenues:
		{
			CDlgConfigureDataVenue* pVenueData = dynamic_cast<CDlgConfigureDataVenue*>(pData);
			std::wstring name(pVenueData->GetVenue()->GetName());
			while (m_Config.GetVenues().FindVenue(name))
			{
				name = StringUtil::stringW(wxString::Format(_("IDS_COPYOF"), name.c_str()));
			}
			ARBConfigVenuePtr pNewVenue;
			if (m_Config.GetVenues().AddVenue(name, &pNewVenue))
			{
				*pNewVenue = *pVenueData->GetVenue();
				pNewVenue->SetName(name); // Put the name back.
				CDlgConfigureDataBase* pNewData = new CDlgConfigureDataVenue(pNewVenue);
				m_ctrlItems->AppendItem(
					m_hItemVenues,
					StringUtil::stringWX(pData->OnNeedText()),
					m_ImageList.IndexARB(), m_ImageList.IndexARB(),
					pNewData);
				m_ctrlItems->SortChildren(m_hItemVenues);
				FindCurrentVenue(pNewVenue, true);
			}
		}
		break;

	case eFaults:
		{
			CDlgConfigureDataFault* pFaultData = dynamic_cast<CDlgConfigureDataFault*>(pData);
			std::wstring name(pFaultData->GetFault()->GetName());
			ARBConfigFaultPtr pNewFault;
			if (m_Config.GetFaults().AddFault(name, &pNewFault))
			{
				CDlgConfigureDataBase* pNewData = new CDlgConfigureDataFault(pNewFault);
				m_ctrlItems->AppendItem(
					m_hItemFaults,
					StringUtil::stringWX(pData->OnNeedText()),
					m_idxFaults, m_idxFaults,
					pNewData);
				m_ctrlItems->SortChildren(m_hItemFaults);
				FindCurrentFault(pNewFault, true);
			}
		}
		break;

	case eOtherPoints:
		{
			CDlgConfigureDataOtherPoints* pOtherData = dynamic_cast<CDlgConfigureDataOtherPoints*>(pData);
			std::wstring name(pOtherData->GetOtherPoints()->GetName());
			while (m_Config.GetOtherPoints().FindOtherPoints(name))
			{
				name = StringUtil::stringW(wxString::Format(_("IDS_COPYOF"), name.c_str()));
			}
			ARBConfigOtherPointsPtr pOther = pOtherData->GetOtherPoints()->Clone();
			pOther->SetName(name);
			if (m_Config.GetOtherPoints().AddOtherPoints(pOther))
			{
				CDlgConfigureDataBase* pNewData = new CDlgConfigureDataOtherPoints(pOther);
				m_ctrlItems->AppendItem(
					m_hItemOtherPts,
					StringUtil::stringWX(pData->OnNeedText()),
					m_ImageList.IndexRun(), m_ImageList.IndexRun(),
					pNewData);
				m_ctrlItems->SortChildren(m_hItemOtherPts);
				FindCurrentOtherPoints(pOther, true);
			}
		}
		break;
	}
}


// During import here, we do nothing.
class CDlgConfigCallback : public IConfigActionCallback
{
public:
	CDlgConfigCallback() {}
	virtual void PreDelete(std::wstring const& inMsg) {}
	void PostDelete(std::wstring const& inMsg) const {}
};


// Updating a configuration is mainly an additive procedure.
void CDlgConfigure::OnUpdate(wxCommandEvent& evt)
{
	CDlgConfigUpdate dlg(this);
	if (wxID_OK == dlg.ShowModal())
	{
		ARBConfig& update = dlg.GetConfig();
		// Update our current config (not runs, later)
		bool bUpdated = false;
		std::wostringstream info;
		CDlgConfigCallback callback;
		if (0 < update.GetActions().Apply(m_Config, nullptr, info, callback))
		{
			bUpdated = true;
			// Now move the actions into our config so we can fully apply them.
			m_Config.GetActions().insert(m_Config.GetActions().end(), update.GetActions().begin(), update.GetActions().end());
			update.GetActions().clear();
		}

		// Update the config.
		if (m_Config.Update(0, update, info) || bUpdated)
		{
			CDlgMessage dlgMsg(info.str(), this);
			dlgMsg.ShowModal();
			LoadData(eVenues);
			LoadData(eFaults);
			LoadData(eOtherPoints);
		}
		else
			wxMessageBox(_("IDS_CONFIG_NO_UPDATE"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
	}
}


void CDlgConfigure::OnOk(wxCommandEvent& evt)
{
	if (m_Book.GetConfig() != m_Config)
	{
		m_pDoc->ImportConfiguration(m_Config);
	}
	EndDialog(wxID_OK);
}
