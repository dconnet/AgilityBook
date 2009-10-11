/*
 * Copyright (c) 2004-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgConfigVenue class
 * @author David Connet
 *
 * There's one caveat to this dialog - there is no verification done to
 * make sure that a division/level pair isn't multiply defined. If that
 * happens, when we go to check things, the first match in the list will
 * be the only one used.
 * Note: The UI checks when creating div/level, so this shouldn't happen.
 * If the file is hand-edited, it could... But just how paranoid do we get?!
 * (Plus, the paranoia checking should be done when the file is loaded.)
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-12-10 DRC Enable duplication of a title.
 * @li 2004-08-14 DRC Fixed a problem dbl-clicking when nothing was selected.
 * @li 2004-04-29 DRC Changed the way events are displayed (from tree to list).
 * @li 2004-02-09 DRC Fixed some bugs when creating/modifying venues.
 * @li 2004-02-02 DRC Added ExistingPoints.
 * @li 2004-01-24 DRC Created
 */

#include "stdafx.h"
#include "DlgConfigVenue.h"

#include "AgilityBook.h"
#include "ARBAgilityRecordBook.h"
#include "ARBConfigVenue.h"
#include "DlgConfigEvent.h"
#include "DlgConfigMultiQ.h"
#include "DlgConfigTitle.h"
#include "DlgConfigureData.h"
#include "DlgName.h"
#include "Globals.h"
#include "Validators.h"
#include "Widgets.h"


/////////////////////////////////////////////////////////////////////////////

class CDlgConfigVenueDataRoot : public CDlgConfigureDataBase
{
public:
	CDlgConfigVenueDataRoot(
			CDlgConfigVenue* pDlg,
			CDlgConfigVenue::eAction action)
		: CDlgConfigureDataBase(pDlg)
		, m_Action(action)
	{
	}
	virtual bool CanAdd() const		{return true;}
	virtual bool DoAdd();

private:
	CDlgConfigVenue::eAction m_Action;
};


bool CDlgConfigVenueDataRoot::DoAdd()
{
	bool bAdded = false;
	bool done = false;
	wxString name;
	switch (m_Action)
	{
	case CDlgConfigVenue::eDivisions:
		while (!done)
		{
			done = true;
			CDlgName dlg(name, _("IDS_DIVISION_NAME"), m_pDlg);
			if (wxID_OK == dlg.ShowModal())
			{
				name = dlg.GetName();
				if (m_pDlg->m_pVenue->GetDivisions().FindDivision(name))
				{
					done = false;
					wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
					continue;
				}
				ARBConfigDivisionPtr pNewDiv;
				if (m_pDlg->m_pVenue->GetDivisions().AddDivision(name, &pNewDiv))
				{
					CDlgConfigureDataDivision* pData = new CDlgConfigureDataDivision(m_pDlg, pNewDiv);
					wxTreeItemId div = m_pDlg->m_ctrlItems->AppendItem(GetId(), pData->OnNeedText(), -1, -1, pData);
					pData->AddSubItems();
					m_pDlg->m_ctrlItems->SelectItem(div);
					bAdded = true;
				}
			}
		}
		break;

	case CDlgConfigVenue::eEvents:
		{
			// The dialog will ensure uniqueness.
			ARBConfigEventPtr pEvent(ARBConfigEvent::New());
			CDlgConfigEvent dlg(true, m_pDlg->m_pVenue, pEvent, m_pDlg);
			if (wxID_OK == dlg.ShowModal())
			{
				if (m_pDlg->m_pVenue->GetEvents().AddEvent(pEvent))
				{
					CDlgConfigureDataEvent* pData = new CDlgConfigureDataEvent(m_pDlg, pEvent);
					wxTreeItemId evt = m_pDlg->m_ctrlItems->AppendItem(GetId(), pData->OnNeedText(), -1, -1, pData);
					pData->AddSubItems();
					m_pDlg->m_ctrlItems->SelectItem(evt);
					bAdded = true;
				}
			}
		}
		break;

	case CDlgConfigVenue::eMultiQ:
		{
			// The dialog will ensure uniqueness.
			ARBConfigMultiQPtr multiq(ARBConfigMultiQ::New());
			CDlgConfigMultiQ dlg(m_pDlg->m_pVenue, multiq, m_pDlg);
			if (wxID_OK == dlg.ShowModal())
			{
				if (m_pDlg->m_pVenue->GetMultiQs().AddMultiQ(multiq))
				{
					CDlgConfigureDataMultiQ* pData = new CDlgConfigureDataMultiQ(m_pDlg, multiq);
					wxTreeItemId mq = m_pDlg->m_ctrlItems->AppendItem(GetId(), pData->OnNeedText(), -1, -1, pData);
					pData->AddSubItems();
					m_pDlg->m_ctrlItems->SelectItem(mq);
					bAdded = true;
				}
			}
		}
		break;

	case CDlgConfigVenue::eTitles:
		while (!done)
		{
			done = true;
			ARBConfigTitlePtr title(ARBConfigTitle::New());
			title->SetName(name);
			CDlgConfigTitle dlg(title, m_pDlg);
			if (wxID_OK == dlg.ShowModal())
			{
				name = title->GetName();
				if (m_pDlg->m_pVenue->GetTitles().FindTitle(name))
				{
					done = false;
					wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
					continue;
				}
				ARBConfigTitlePtr pTitle;
				if (m_pDlg->m_pVenue->GetTitles().AddTitle(name, &pTitle))
				{
					*pTitle = *title;
					//pTitle->SetMultiple(dlg.GetMultiple());
					//pTitle->SetLongName(dlg.GetLongName());
					//pTitle->SetDescription(dlg.GetDesc());
					CDlgConfigureDataTitle* pData = new CDlgConfigureDataTitle(m_pDlg, pTitle);
					wxTreeItemId titleId = m_pDlg->m_ctrlItems->AppendItem(GetId(), pData->OnNeedText(), -1, -1, pData);
					pData->AddSubItems();
					m_pDlg->m_ctrlItems->SelectItem(titleId);
					bAdded = true;
				}
			}
		}
		break;
	}
	return bAdded;
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CDlgConfigVenue, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgConfigVenue::OnOk)
END_EVENT_TABLE()


CDlgConfigVenue::CDlgConfigVenue(
		ARBAgilityRecordBook const& book,
		ARBConfig const& config,
		ARBConfigVenuePtr pVenue,
		wxWindow* pParent)
	: wxDialog()
	, m_Book(book)
	, m_Config(config)
	, m_pVenueOrig(pVenue)
	, m_pVenue(pVenue->Clone())
	, m_DlgFixup()
	, m_Name(m_pVenue->GetName())
	, m_LongName(m_pVenue->GetLongName())
	, m_URL(m_pVenue->GetURL())
	, m_LifetimeName(m_pVenue->GetLifetimeName())
	, m_Desc(m_pVenue->GetDesc())
	, m_ctrlItems(NULL)
	, m_ctrlNew(NULL)
	, m_ctrlEdit(NULL)
	, m_ctrlDelete(NULL)
	, m_ctrlCopy(NULL)
	, m_ctrlMoveUp(NULL)
	, m_ctrlMoveDown(NULL)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_CONFIG_VENUE"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	assert(m_pVenueOrig);
	assert(m_pVenue);

	// Controls (these are done first to control tab order)

	wxStaticText* textName = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_VENUE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	CTextCtrl* ctrlName = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_Name, _("IDS_INVALID_NAME")));
	ctrlName->SetHelpText(_("HIDC_CONFIG_VENUE"));
	ctrlName->SetToolTip(_("HIDC_CONFIG_VENUE"));

	wxStaticText* textURL = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_VENUE_URL"),
		wxDefaultPosition, wxDefaultSize, 0);
	textURL->Wrap(-1);

	CTextCtrl* ctrlURL = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_URL, TRIMVALIDATOR_TRIM_BOTH));
	ctrlURL->SetHelpText(_("HIDC_CONFIG_VENUE_URL"));
	ctrlURL->SetToolTip(_("HIDC_CONFIG_VENUE_URL"));

	wxStaticText* textLongName = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_VENUE_LONGNAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textLongName->Wrap(-1);

	CTextCtrl* ctrlLongName = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_LongName, TRIMVALIDATOR_TRIM_BOTH));
	ctrlLongName->SetHelpText(_("HIDC_CONFIG_VENUE_LONGNAME"));
	ctrlLongName->SetToolTip(_("HIDC_CONFIG_VENUE_LONGNAME"));

	wxStaticText* textLifetime = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_VENUE_LIFETIME_NAME"),
		wxDefaultPosition, wxDefaultSize, 0);
	textLifetime->Wrap(-1);

	CTextCtrl* ctrlLifetime = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_LifetimeName, TRIMVALIDATOR_TRIM_RIGHT));
	ctrlLifetime->SetHelpText(_("HIDC_CONFIG_VENUE_LIFETIME_NAME"));
	ctrlLifetime->SetToolTip(_("HIDC_CONFIG_VENUE_LIFETIME_NAME"));

	wxStaticText* textDesc = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_VENUE_DESC"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDesc->Wrap(-1);

	CTextCtrl* ctrlDesc = new CTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(-1, 70), wxTE_MULTILINE|wxTE_WORDWRAP,
		CTrimValidator(&m_Desc, TRIMVALIDATOR_TRIM_RIGHT));
	ctrlDesc->SetHelpText(_("HIDC_CONFIG_VENUE_DESC"));
	ctrlDesc->SetToolTip(_("HIDC_CONFIG_VENUE_DESC"));

	m_ctrlItems = new CTreeCtrl(this, wxID_ANY,
		wxDefaultPosition, wxSize(300, 250),
		wxTR_FULL_ROW_HIGHLIGHT|wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|wxTR_SINGLE);
	m_ctrlItems->Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(CDlgConfigVenue::OnSelectionChanged), NULL, this);
	m_ctrlItems->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(CDlgConfigVenue::OnItemActivated), NULL, this);
	m_ctrlItems->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(CDlgConfigVenue::OnKeydown), NULL, this);
	//m_ctrlItems->SetHelpText(_(""));
	//m_ctrlItems->SetToolTip(_(""));

	m_ctrlNew = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_VENUE_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlNew->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigVenue::OnNew), NULL, this);
	m_ctrlNew->SetHelpText(_("HIDC_CONFIG_VENUE_NEW"));
	m_ctrlNew->SetToolTip(_("HIDC_CONFIG_VENUE_NEW"));

	m_ctrlEdit = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_VENUE_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigVenue::OnEdit), NULL, this);
	m_ctrlEdit->SetHelpText(_("HIDC_CONFIG_VENUE_EDIT"));
	m_ctrlEdit->SetToolTip(_("HIDC_CONFIG_VENUE_EDIT"));

	m_ctrlDelete = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_VENUE_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigVenue::OnDelete), NULL, this);
	m_ctrlDelete->SetHelpText(_("HIDC_CONFIG_VENUE_DELETE"));
	m_ctrlDelete->SetToolTip(_("HIDC_CONFIG_VENUE_DELETE"));

	m_ctrlCopy = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_VENUE_COPY"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlCopy->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigVenue::OnCopy), NULL, this);
	m_ctrlCopy->SetHelpText(_("HIDC_CONFIG_VENUE_COPY"));
	m_ctrlCopy->SetToolTip(_("HIDC_CONFIG_VENUE_COPY"));

	m_ctrlMoveUp = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_VENUE_MOVE_UP"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlMoveUp->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigVenue::OnMoveUp), NULL, this);
	m_ctrlMoveUp->SetHelpText(_("HIDC_CONFIG_VENUE_MOVE_UP"));
	m_ctrlMoveUp->SetToolTip(_("HIDC_CONFIG_VENUE_MOVE_UP"));

	m_ctrlMoveDown = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_VENUE_MOVE_DOWN"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlMoveDown->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigVenue::OnMoveDown), NULL, this);
	m_ctrlMoveDown->SetHelpText(_("HIDC_CONFIG_VENUE_MOVE_DOWN"));
	m_ctrlMoveDown->SetToolTip(_("HIDC_CONFIG_VENUE_MOVE_DOWN"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerName->Add(ctrlName, 0, wxALL, 5);
	sizerName->Add(textURL, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerName->Add(ctrlURL, 1, wxALL, 5);

	bSizer->Add(sizerName, 0, wxEXPAND, 5);

	wxBoxSizer* sizerLongName = new wxBoxSizer(wxHORIZONTAL);
	sizerLongName->Add(textLongName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerLongName->Add(ctrlLongName, 1, wxALL, 5);

	bSizer->Add(sizerLongName, 0, wxEXPAND, 5);
	bSizer->Add(textLifetime, 0, wxLEFT|wxRIGHT|wxTOP, 5);
	bSizer->Add(ctrlLifetime, 0, wxALL|wxEXPAND, 5);
	bSizer->Add(textDesc, 0, wxLEFT|wxRIGHT|wxTOP, 5);
	bSizer->Add(ctrlDesc, 0, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerTree = new wxBoxSizer(wxHORIZONTAL);
	sizerTree->Add(m_ctrlItems, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxVERTICAL);
	sizerBtns->Add(m_ctrlNew, 0, wxALL, 5);
	sizerBtns->Add(m_ctrlEdit, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5);
	sizerBtns->Add(m_ctrlDelete, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5);
	sizerBtns->Add(m_ctrlCopy, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5);
	sizerBtns->Add(m_ctrlMoveUp, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5);
	sizerBtns->Add(m_ctrlMoveDown, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerTree->Add(sizerBtns, 0, wxEXPAND, 5);

	bSizer->Add(sizerTree, 1, wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	wxTreeItemId root = m_ctrlItems->AddRoot(wxT("Root"));

	wxTreeItemId divs = m_ctrlItems->AppendItem(root, _("IDC_CONFIG_VENUE_DIVISION"), -1, -1, new CDlgConfigVenueDataRoot(this, eDivisions));
	for (ARBConfigDivisionList::iterator iterDiv = m_pVenue->GetDivisions().begin(); iterDiv != m_pVenue->GetDivisions().end(); ++iterDiv)
	{
		CDlgConfigureDataDivision* pData = new CDlgConfigureDataDivision(this, *iterDiv);
		wxTreeItemId div = m_ctrlItems->AppendItem(divs, pData->OnNeedText(), -1, -1, pData);
		pData->AddSubItems();
	}
	m_ctrlItems->Expand(divs);

	wxTreeItemId events = m_ctrlItems->AppendItem(root, _("IDC_CONFIG_VENUE_EVENT"), -1, -1, new CDlgConfigVenueDataRoot(this, eEvents));
	for (ARBConfigEventList::iterator iterEvent = m_pVenue->GetEvents().begin(); iterEvent != m_pVenue->GetEvents().end(); ++iterEvent)
	{
		CDlgConfigureDataEvent* pData = new CDlgConfigureDataEvent(this, *iterEvent);
		m_ctrlItems->AppendItem(events, pData->OnNeedText(), -1, -1, pData);
		pData->AddSubItems();
	}
	m_ctrlItems->Expand(events);

	wxTreeItemId multiQs = m_ctrlItems->AppendItem(root, _("IDC_CONFIG_VENUE_MULTIQ"), -1, -1, new CDlgConfigVenueDataRoot(this, eMultiQ));
	for (ARBConfigMultiQList::iterator iter = m_pVenue->GetMultiQs().begin();
		iter != m_pVenue->GetMultiQs().end();
		++iter)
	{
		CDlgConfigureDataMultiQ* pData = new CDlgConfigureDataMultiQ(this, *iter);
		m_ctrlItems->AppendItem(multiQs, pData->OnNeedText(), -1, -1, pData);
		pData->AddSubItems();
	}

	wxTreeItemId titles = m_ctrlItems->AppendItem(root, _("IDC_CONFIG_VENUE_TITLES"), -1, -1, new CDlgConfigVenueDataRoot(this, eTitles));
	for (ARBConfigTitleList::iterator iterTitle = m_pVenue->GetTitles().begin();
		iterTitle != m_pVenue->GetTitles().end();
		++iterTitle)
	{
		CDlgConfigureDataTitle* pData = new CDlgConfigureDataTitle(this, *iterTitle);
		m_ctrlItems->AppendItem(titles, pData->OnNeedText(), -1, -1, pData);
		pData->AddSubItems();
	}

	m_ctrlItems->SelectItem(events);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();
}


CDlgConfigVenue::~CDlgConfigVenue()
{
	m_DlgFixup.clear();
}


void CDlgConfigVenue::GetFixups(ARBConfigActionList& ioFixups)
{
	ioFixups.insert(ioFixups.end(), m_DlgFixup.begin(), m_DlgFixup.end());
	m_DlgFixup.clear();
}


CDlgConfigureDataBase* CDlgConfigVenue::GetData(wxTreeItemId item) const
{
	if (item.IsOk())
		return dynamic_cast<CDlgConfigureDataBase*>(m_ctrlItems->GetItemData(item));
	return NULL;
}


CDlgConfigureDataBase* CDlgConfigVenue::GetCurrentData(wxTreeItemId* pItem) const
{
	wxTreeItemId item = m_ctrlItems->GetSelection();
	if (pItem)
		*pItem = item;
	return GetData(item);
}


void CDlgConfigVenue::UpdateButtons()
{
	bool bNew = false;
	bool bEdit = false;
	bool bDelete = false;
	bool bCopy = false;
	bool bMoveUp = false;
	bool bMoveDown = false;

	wxTreeItemId item;
	CDlgConfigureDataBase* pBase = GetCurrentData(&item);
	if (pBase)
	{
		bNew = pBase->CanAdd();
		bEdit = pBase->CanEdit();
		bDelete = pBase->CanDelete();
		bCopy = pBase->CanCopy();
		bMoveUp = pBase->CanMove() && m_ctrlItems->GetPrevSibling(item).IsOk();
		bMoveDown = pBase->CanMove() && m_ctrlItems->GetNextSibling(item).IsOk();
	}

	m_ctrlNew->Enable(bNew);
	m_ctrlEdit->Enable(bEdit);
	m_ctrlDelete->Enable(bDelete);
	m_ctrlCopy->Enable(bCopy);
	m_ctrlMoveUp->Enable(bMoveUp);
	m_ctrlMoveDown->Enable(bMoveDown);
}


void CDlgConfigVenue::OnItemActivated(wxTreeEvent& evt)
{
	CDlgConfigureDataBase* pBase = GetCurrentData();
	if (pBase && pBase->CanEdit())
		pBase->DoEdit();
}


void CDlgConfigVenue::OnSelectionChanged(wxTreeEvent& evt)
{
	UpdateButtons();
}


void CDlgConfigVenue::OnKeydown(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode())
	{
	default:
		evt.Skip();
		break;
	case WXK_SPACE:
	case WXK_NUMPAD_SPACE:
		{
			CDlgConfigureDataBase* pBase = GetCurrentData();
			if (pBase && pBase->CanEdit())
				pBase->DoEdit();
		}
		break;
	}
}


void CDlgConfigVenue::OnNew(wxCommandEvent& evt)
{
	CDlgConfigureDataBase* pBase = GetCurrentData();
	if (pBase && pBase->CanAdd())
		pBase->DoAdd();
}


void CDlgConfigVenue::OnEdit(wxCommandEvent& evt)
{
	CDlgConfigureDataBase* pBase = GetCurrentData();
	if (pBase && pBase->CanEdit())
		pBase->DoEdit();
}


void CDlgConfigVenue::OnDelete(wxCommandEvent& evt)
{
	CDlgConfigureDataBase* pBase = GetCurrentData();
	if (pBase && pBase->CanDelete())
		pBase->DoDelete();
}


void CDlgConfigVenue::OnCopy(wxCommandEvent& evt)
{
	CDlgConfigureDataBase* pBase = GetCurrentData();
	if (pBase && pBase->CanCopy())
		pBase->DoCopy();
}


void CDlgConfigVenue::OnMoveUp(wxCommandEvent& evt)
{
	wxTreeItemId item;
	CDlgConfigureDataBase* pBase = GetCurrentData(&item);
	if (pBase && pBase->CanMove())
	{
		wxTreeItemId itemPrev = m_ctrlItems->GetPrevSibling(item);
		if (itemPrev.IsOk())
		{
			CDlgConfigureDataBase* pDup = pBase->DoMove(true);
			if (pDup)
			{
				wxTreeItemId parent = m_ctrlItems->GetItemParent(item);
				wxTreeItemId itemPrev2 = m_ctrlItems->GetPrevSibling(itemPrev);
				wxTreeItemId itemNew;
				if (itemPrev2.IsOk())
					itemNew = m_ctrlItems->InsertItem(parent, itemPrev2, pDup->OnNeedText(), -1, -1, pDup);
				else
					itemNew = m_ctrlItems->InsertItem(parent, 0, pDup->OnNeedText(), -1, -1, pDup);
				m_ctrlItems->Delete(item);
				pDup->AddSubItems();
				m_ctrlItems->SelectItem(itemNew);
				UpdateButtons();
			}
		}
	}
}


void CDlgConfigVenue::OnMoveDown(wxCommandEvent& evt)
{
	wxTreeItemId item;
	CDlgConfigureDataBase* pBase = GetCurrentData(&item);
	if (pBase && pBase->CanMove())
	{
		wxTreeItemId itemNext = m_ctrlItems->GetNextSibling(item);
		if (itemNext.IsOk())
		{
			CDlgConfigureDataBase* pDup = pBase->DoMove(false);
			if (pDup)
			{
				wxTreeItemId parent = m_ctrlItems->GetItemParent(item);
				wxTreeItemId itemNew = m_ctrlItems->InsertItem(parent, itemNext, pDup->OnNeedText(), -1, -1, pDup);
				m_ctrlItems->Delete(item);
				pDup->AddSubItems();
				m_ctrlItems->SelectItem(itemNew);
				UpdateButtons();
			}
		}
	}
}


void CDlgConfigVenue::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;
	m_URL.Replace(wxT("\""), wxT(""));

	wxString name(m_Name);
	wxString oldName = m_pVenue->GetName();
	if (oldName != name)
	{
		if (m_Config.GetVenues().FindVenue(name))
		{
			wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
			return;
		}
		m_pVenue->SetName(name);
	}

	m_pVenue->SetLongName(m_LongName);
	m_pVenue->SetURL(m_URL);
	m_pVenue->SetDesc(m_Desc);
	m_pVenue->SetLifetimeName(m_LifetimeName);

	if (oldName != name)
		m_DlgFixup.push_back(ARBConfigActionRenameVenue::New(oldName, name));

	// Push the copy back.
	*m_pVenueOrig = *m_pVenue;

	// The rest is already taken care of.

	EndDialog(wxID_OK);
}
