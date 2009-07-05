/*
 * Copyright � 2002-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 *
 * @brief implementation of the CDlgConfigure class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-10 DRC Ported to wxWidgets.
 * @li 2007-08-19 DRC Simplified UI layout into a single tree.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2006-02-08 DRC Added 'RenameEvent' action.
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-03-26 DRC Added code to migrate runs to the new table-in-run form.
 * @li 2004-02-18 DRC Added 'DeleteTitle' configuration action.
 * @li 2004-01-21 DRC Implemented Action items in configuration update.
 * @li 2004-01-14 DRC Use complete title name instead of nice name.
 * @li 2003-12-27 DRC Changed the scoring method to show the valid date range.
 * @li 2003-02-04 DRC Changed dbl-click to edit an item.
 */

#include "stdafx.h"
#include "DlgConfigure.h"

#include "AgilityBookDoc.h"
#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "ARBConfigVenue.h"
#include "DlgConfigOtherPoints.h"
#include "DlgConfigUpdate.h"
#include "DlgConfigVenue.h"
#include "DlgConfigureData.h"
#include "DlgListViewer.h"
#include "DlgMessage.h"
#include "DlgMessageBox.h"
#include "DlgName.h"
#include "Globals.h"
#include <wx/artprov.h>

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataRoot : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataRoot(CDlgConfigure::eAction action)
		: CDlgConfigureDataBase(NULL)
		, m_Action(action)
	{
	}
	virtual wxString OnNeedText() const	{return wxEmptyString;}
	CDlgConfigure::eAction GetAction() const
	{
		return m_Action;
	}
private:
	CDlgConfigure::eAction m_Action;
};

/////////////////////////////////////////////////////////////////////////////

class CDetails : public IMessageBoxCallback
{
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
	wxString str = _("IDS_AFFECTED_RUNS");
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
	, m_ctrlItems(NULL)
	, m_ctrlNew(NULL)
	, m_ctrlEdit(NULL)
	, m_ctrlDelete(NULL)
	, m_ctrlCopy(NULL)
	, m_hItemVenues()
	, m_hItemFaults()
	, m_hItemOtherPts()
{
	m_idxFaults = m_ImageList.Add(wxArtProvider::GetIcon(wxART_WARNING));
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	Create(pParent, wxID_ANY, _("IDD_CONFIGURE"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	assert(m_pDoc);

	// Controls (these are done first to control tab order)

	m_ctrlItems = new wxTreeCtrl(this, wxID_ANY,
		wxDefaultPosition, wxSize(375, 400),
		wxTR_FULL_ROW_HIGHLIGHT|wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|wxTR_SINGLE);
	m_ctrlItems->SetImageList(&m_ImageList);
	m_ctrlItems->Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(CDlgConfigure::OnSelectionChanged), NULL, this);
	m_ctrlItems->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(CDlgConfigure::OnDoubleClick), NULL, this);
	m_ctrlItems->SetHelpText(_("HIDC_CONFIG_ITEMS"));
	m_ctrlItems->SetToolTip(_("HIDC_CONFIG_ITEMS"));

	m_ctrlNew = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlNew->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigure::OnNew), NULL, this);
	m_ctrlNew->SetHelpText(_("HIDC_CONFIG_NEW"));
	m_ctrlNew->SetToolTip(_("HIDC_CONFIG_NEW"));

	m_ctrlEdit = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigure::OnEdit), NULL, this);
	m_ctrlEdit->SetHelpText(_("HIDC_CONFIG_EDIT"));
	m_ctrlEdit->SetToolTip(_("HIDC_CONFIG_EDIT"));

	m_ctrlDelete = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigure::OnDelete), NULL, this);
	m_ctrlDelete->SetHelpText(_("HIDC_CONFIG_DELETE"));
	m_ctrlDelete->SetToolTip(_("HIDC_CONFIG_DELETE"));

	m_ctrlCopy = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_COPY"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlCopy->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigure::OnCopy), NULL, this);
	m_ctrlCopy->SetHelpText(_("HIDC_CONFIG_COPY"));
	m_ctrlCopy->SetToolTip(_("HIDC_CONFIG_COPY"));

	wxButton* btnUpdate = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_UPDATE"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnUpdate->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigure::OnUpdate), NULL, this);
	btnUpdate->SetHelpText(_("HIDC_CONFIG_UPDATE"));
	btnUpdate->SetToolTip(_("HIDC_CONFIG_UPDATE"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerConfig = new wxBoxSizer(wxHORIZONTAL);
	sizerConfig->Add(m_ctrlItems, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxVERTICAL);
	sizerBtns->Add(m_ctrlNew, 0, wxALL, 5);
	sizerBtns->Add(m_ctrlEdit, 0, wxALL, 5);
	sizerBtns->Add(m_ctrlDelete, 0, wxALL, 5);
	sizerBtns->Add(m_ctrlCopy, 0, wxALL, 5);

	sizerConfig->Add(sizerBtns, 0, wxEXPAND, 5);

	bSizer->Add(sizerConfig, 1, wxEXPAND, 5);

	wxBoxSizer* sizerUpdate = new wxBoxSizer(wxHORIZONTAL);
	sizerUpdate->Add(0, 0, 1, wxEXPAND, 5);
	sizerUpdate->Add(btnUpdate, 0, wxALL, 5);

	bSizer->Add(sizerUpdate, 0, wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	wxTreeItemId root = m_ctrlItems->AddRoot(wxT("Root"));

	m_hItemVenues = m_ctrlItems->AppendItem(root, _("IDS_COL_VENUES"), m_ImageList.ARB(), m_ImageList.ARB(), new CDlgConfigureDataRoot(eVenues));
	LoadData(eVenues);
	m_ctrlItems->Expand(m_hItemVenues);

	m_hItemFaults = m_ctrlItems->AppendItem(root, _("IDS_COL_FAULTS"), m_idxFaults, m_idxFaults, new CDlgConfigureDataRoot(eFaults));
	LoadData(eFaults);

	m_hItemOtherPts = m_ctrlItems->AppendItem(root, _("IDS_OTHERPOINTS"), m_ImageList.Run(), m_ImageList.Run(), new CDlgConfigureDataRoot(eOtherPoints));
	LoadData(eOtherPoints);

	m_ctrlItems->SelectItem(m_hItemVenues);
	m_ctrlItems->EnsureVisible(m_hItemVenues);
	UpdateButtons();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();
}


CDlgConfigure::~CDlgConfigure()
{
	// Unhook the selection event. DeleteAllItems triggers this and wxWidgets
	// apparently didn't clean something up - the virtual root is trying
	// to access GetParam() [via our GetData] whichs dies horribly.
	m_ctrlItems->Disconnect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(CDlgConfigure::OnSelectionChanged), NULL, this);
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
	return NULL;
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
	case eVenues:
		{ // Scoped because of VC6's scoping of for-variables.
			for (ARBConfigVenueList::iterator iterVenue = m_Config.GetVenues().begin(); iterVenue != m_Config.GetVenues().end(); ++iterVenue)
			{
				int idx = (*iterVenue)->GetIcon();
				if (0 > idx || idx >= m_ImageList.GetImageCount())
					idx = -1;
				CDlgConfigureDataVenue* pData = new CDlgConfigureDataVenue(*iterVenue);
				m_ctrlItems->AppendItem(
					hParent,
					pData->OnNeedText(),
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
					pData->OnNeedText(),
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
					pData->OnNeedText(),
					m_ImageList.Run(), m_ImageList.Run(),
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
			tstring oldName = pFaultData->GetFault()->GetName();
			tstring name(oldName);
			while (!done)
			{
				done = true;
				CDlgName dlg(name.c_str(), _("IDS_FAULT_TYPE_NAME"), this);
				if (wxID_OK == dlg.ShowModal())
				{
					name = dlg.GetName();
					if (oldName != name)
					{
						if (m_Config.GetFaults().FindFault(name))
						{
							done = false;
							wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
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
			tstring oldName = pOtherData->GetOtherPoints()->GetName();
			CDlgConfigOtherPoints dlg(m_Config, pOtherData->GetOtherPoints(), this);
			if (wxID_OK == dlg.ShowModal())
			{
				if (pOtherData->GetOtherPoints()->GetName() != oldName)
				{
					// Other things may have changed, but we only care about the name for fixup.
					m_Config.GetActions().push_back(ARBConfigActionRenameOtherPoints::New(oldName, pOtherData->GetOtherPoints()->GetName()));
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
		item = m_ctrlItems->GetNextChild(item, cookie))
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
		item = m_ctrlItems->GetNextChild(item, cookie))
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
		item = m_ctrlItems->GetNextChild(item, cookie))
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


void CDlgConfigure::OnDoubleClick(wxMouseEvent& evt)
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
						pData->OnNeedText(),
						m_ImageList.ARB(), m_ImageList.ARB(),
						pData);
					m_ctrlItems->SortChildren(m_hItemVenues);
					FindCurrentVenue(pVenue, true);
				}
			}
		}
		break;

	case eFaults:
		{
			CDlgName dlg(wxEmptyString, _("IDS_FAULT_TYPE_NAME"), this);
			if (wxID_OK == dlg.ShowModal())
			{
				wxString name = dlg.GetName();
				// We could check for uniqueness, but if the user wants 2
				// strings the same, why argue! Afterall, these strings
				// are only "helper" items to fill in other data.
				if (0 < name.length())
				{
					ARBConfigFaultPtr pNewFault;
					if (m_Config.GetFaults().AddFault(name.c_str(), &pNewFault))
					{
						CDlgConfigureDataFault* pData = new CDlgConfigureDataFault(pNewFault);
						m_ctrlItems->AppendItem(
							m_hItemFaults,
							pData->OnNeedText(),
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
						pData->OnNeedText(),
						m_ImageList.Run(), m_ImageList.Run(),
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
	case eVenues:
		{
			CDlgConfigureDataVenue* pVenueData = dynamic_cast<CDlgConfigureDataVenue*>(pData);
			tstring venue = pVenueData->GetVenue()->GetName();
			// If we were able to delete it...
			if (m_Config.GetVenues().DeleteVenue(venue))
			{
				m_Config.GetActions().push_back(ARBConfigActionDeleteVenue::New(venue));
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
			tstring otherPoints = pOtherData->GetOtherPoints()->GetName();
			if (m_Config.GetOtherPoints().DeleteOtherPoints(otherPoints))
			{
				m_Config.GetActions().push_back(ARBConfigActionDeleteOtherPoints::New(otherPoints));
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
	case eVenues:
		{
			CDlgConfigureDataVenue* pVenueData = dynamic_cast<CDlgConfigureDataVenue*>(pData);
			tstring name(pVenueData->GetVenue()->GetName());
			while (m_Config.GetVenues().FindVenue(name))
			{
				wxString copyOf = wxString::Format(_("IDS_COPYOF"), name.c_str());
				name = copyOf.c_str();
			}
			ARBConfigVenuePtr pNewVenue;
			if (m_Config.GetVenues().AddVenue(name, &pNewVenue))
			{
				*pNewVenue = *pVenueData->GetVenue();
				pNewVenue->SetName(name); // Put the name back.
				CDlgConfigureDataBase* pNewData = new CDlgConfigureDataVenue(pNewVenue);
				m_ctrlItems->AppendItem(
					m_hItemVenues,
					pData->OnNeedText(),
					m_ImageList.ARB(), m_ImageList.ARB(),
					pNewData);
				m_ctrlItems->SortChildren(m_hItemVenues);
				FindCurrentVenue(pNewVenue, true);
			}
		}
		break;

	case eFaults:
		{
			CDlgConfigureDataFault* pFaultData = dynamic_cast<CDlgConfigureDataFault*>(pData);
			tstring name(pFaultData->GetFault()->GetName());
			ARBConfigFaultPtr pNewFault;
			if (m_Config.GetFaults().AddFault(name, &pNewFault))
			{
				CDlgConfigureDataBase* pNewData = new CDlgConfigureDataFault(pNewFault);
				m_ctrlItems->AppendItem(
					m_hItemFaults,
					pData->OnNeedText(),
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
			tstring name(pOtherData->GetOtherPoints()->GetName());
			while (m_Config.GetOtherPoints().FindOtherPoints(name))
			{
				wxString copyOf = wxString::Format(_("IDS_COPYOF"), name.c_str());
				name = copyOf.c_str();
			}
			ARBConfigOtherPointsPtr pOther = pOtherData->GetOtherPoints()->Clone();
			pOther->SetName(name);
			if (m_Config.GetOtherPoints().AddOtherPoints(pOther))
			{
				CDlgConfigureDataBase* pNewData = new CDlgConfigureDataOtherPoints(pOther);
				m_ctrlItems->AppendItem(
					m_hItemOtherPts,
					pData->OnNeedText(),
					m_ImageList.Run(), m_ImageList.Run(),
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
	virtual void PreDelete(tstring const& inMsg) {}
	void PostDelete(tstring const& inMsg) const {}
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
		otstringstream info;
		CDlgConfigCallback callback;
		if (0 < update.GetActions().Apply(m_Config, NULL, info, callback))
		{
			bUpdated = true;
			// Now move the actions into our config so we can fully apply them.
			m_Config.GetActions().insert(m_Config.GetActions().end(), update.GetActions().begin(), update.GetActions().end());
			update.GetActions().clear();
		}

		// Update the config.
		if (m_Config.Update(0, update, info) || bUpdated)
		{
			CDlgMessage dlgMsg(info.str().c_str(), this);
			dlgMsg.ShowModal();
			LoadData(eVenues);
			LoadData(eFaults);
			LoadData(eOtherPoints);
		}
		else
			wxMessageBox(_("IDS_CONFIG_NO_UPDATE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_INFORMATION);
	}
}


void CDlgConfigure::OnOk(wxCommandEvent& evt)
{
	wxBusyCursor wait;
	if (m_Book.GetConfig() != m_Config)
	{
		m_pDoc->ImportConfiguration(m_Config);
	}
	EndDialog(wxID_OK);
}
