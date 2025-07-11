/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Dialog for adding notes to ARBInfo items.
 * @author David Connet
 *
 * Remember, when adding an entry, it is only saved if there is a comment.
 *
 * Revision History
 * 2021-09-19 Changed infonote from a combobox to a listctrl.
 * 2014-12-31 Changed pixels to dialog units.
 * 2012-05-22 Change KillFocus handler to text change handler.
 * 2012-02-16 Fix initial focus.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-12 Fix killfocus handling.
 * 2009-02-10 Ported to wxWidgets.
 * 2008-02-01 Add ability to see what was last selected.
 * 2008-01-01 Added visible flag.
 * 2007-12-03 SelectString was still used in OnNew.
 *            Fix a drawing problem in the drop list.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-13 SelectString doesn't work in ownerdraw combo. Changed
 *            ctor interface for more versatility.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-24-05 If there were 0 items in the list, it crashed.
 *            Allow saving an entry with no comment.
 * 2004-12-11 Added indicators if item is added and/or has comments.
 *            Merged in club/location support (was in separate files
 *            that were added 11/18/04.
 * 2003-12-07 Created
 */

#include "stdafx.h"
#include "DlgInfoNote.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "ClipBoard.h"
#include "DlgInfoNoteEdit.h"
#include "ImageHelper.h"

#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ConfigPosition.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/ListData.h"
#include "LibARBWin/ReportListCtrl.h"
#include "LibARBWin/ReportListHeader.h"
#include "LibARBWin/Widgets.h"
#include <numeric>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

namespace
{
constexpr int k_colIcon = 0;
constexpr int k_colName = 1;
constexpr int k_colComment = 2;
const std::vector<CReportListHeader::ColumnInfo> k_columns{
	{k_colIcon, wxLIST_FORMAT_LEFT, L""},
	{k_colName, wxLIST_FORMAT_LEFT, arbT("IDS_COL_NAME")},
	{k_colComment, wxLIST_FORMAT_LEFT, arbT("IDS_COL_COMMENTS")},
};
} // namespace

/////////////////////////////////////////////////////////////////////////////

class CConfigInfoNote : public CConfigPosition
{
public:
	CConfigInfoNote()
		: CConfigPosition(L"InfoNote", CConfigPosition::eConfigSize)
	{
	}
};

/////////////////////////////////////////////////////////////////////////////

class InfoNoteListData : public CListData
{
	DECLARE_NO_COPY_IMPLEMENTED(InfoNoteListData)
public:
	InfoNoteListData(CDlgInfoNote* parent, size_t idxName)
		: m_parent(parent)
		, m_idxName(idxName)
		, m_item()
	{
		m_item = m_parent->FindName(m_parent->GetNames()[m_idxName].m_name);
	}
	// We edited an existing item into a new one.
	void UpdateItem(size_t index, ARBInfoItemPtr item)
	{
		m_idxName = index;
		m_item = item;
	}
	// Force an update
	ARBInfoItemPtr UpdateItem(wxString const& name, ARBInfoItemList& info);

	int OnCompare(CListDataPtr const& item, long iCol) const override;
	wxString OnNeedText(long iCol) const override;
	void OnNeedListItem(long iCol, wxListItem& info) const override;
	bool OnNeedCheck() const override
	{
		return IsVisible();
	}

	bool IsVisible() const
	{
		return !m_item || m_item->IsVisible();
	}
	size_t GetIndex() const
	{
		return m_idxName;
	}
	ARBInfoItemPtr GetItem() const
	{
		return m_item;
	}

protected:
	int GetIcon() const;

	CDlgInfoNote* m_parent;
	size_t m_idxName;
	ARBInfoItemPtr m_item;
};


ARBInfoItemPtr InfoNoteListData::UpdateItem(wxString const& name, ARBInfoItemList& info)
{
	if (!info.FindItem(name, &m_item))
	{
		info.AddItem(name);
		info.FindItem(name, &m_item);
	}
	return m_item;
}


int InfoNoteListData::OnCompare(CListDataPtr const& item, long iCol) const
{
	InfoNoteListDataPtr pData2 = std::dynamic_pointer_cast<InfoNoteListData, CListData>(item);
	if (!pData2)
		return 1;

	int rc = 0;
	switch (iCol)
	{
	case k_colIcon:
	{
		int icon1 = GetIcon();
		int icon2 = pData2->GetIcon();
		if (icon1 < icon2)
			rc = -1;
		else if (icon1 > icon2)
			rc = 1;
	}
	break;
	case k_colName:
	{
		wxString n1 = m_parent->GetNames()[m_idxName].m_name;
		wxString n2 = m_parent->GetNames()[pData2->m_idxName].m_name;
		if (n1 < n2)
			rc = -1;
		else if (n1 > n2)
			rc = 1;
	}
	break;
	case k_colComment:
	{
		wxString s1 = m_item ? m_item->GetComment() : L"";
		wxString s2 = pData2->m_item ? pData2->m_item->GetComment() : L"";
		if (s1 < s2)
			rc = -1;
		else if (s1 > s2)
			rc = 1;
	}
	break;
	default:
		assert(0);
		break;
	}
	return rc;
}


wxString InfoNoteListData::OnNeedText(long iCol) const
{
	wxString str;
	switch (iCol)
	{
	case k_colIcon:
		break;
	case k_colName:
		str = m_parent->GetNames()[m_idxName].m_name;
		break;
	case k_colComment:
	{
		if (m_item)
			str = m_item->GetComment();
	}
	break;
	default:
		assert(0);
		break;
	}
	return str;
}


void InfoNoteListData::OnNeedListItem(long iCol, wxListItem& info) const
{
	switch (iCol)
	{
	case k_colIcon:
		info.SetMask(info.GetMask() | wxLIST_MASK_IMAGE);
		info.SetImage(GetIcon());
		break;
	default:
		info.SetText(OnNeedText(iCol));
	}
}


int InfoNoteListData::GetIcon() const
{
	int image = m_parent->m_imgNone;
	if (0 < m_parent->GetAddedCount() && NameInfo::Usage::NotInUse == m_parent->GetNames()[m_idxName].m_usage)
	{
		if (m_item && m_item->HasData())
			image = m_parent->m_imgNoteAdded;
		else
			image = m_parent->m_imgAdded;
	}
	else
	{
		if (m_item && m_item->HasData())
			image = m_parent->m_imgNote;
	}
	return image;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgInfoNote dialog

// TODO: not getting selected when selecting 2nd item via shift-click (wx bug)
// Listen to focused because of https://github.com/wxWidgets/wxWidgets/issues/4541
// Use EventTable because Binding allows a focused event after destroy on mac
constexpr int LIST_CTRL = 1000;
wxBEGIN_EVENT_TABLE(CDlgInfoNote, wxDialog)
	EVT_LIST_ITEM_FOCUSED(LIST_CTRL, CDlgInfoNote::OnItemFocused)
wxEND_EVENT_TABLE()

CDlgInfoNote::CDlgInfoNote(CAgilityBookDoc* pDoc, ARBInfoType inType, wxString const& inSelect, wxWindow* parent)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_type(inType)
	, m_NamesInUse()
	, m_InfoOrig(m_pDoc->Book().GetInfo().GetInfo(inType))
	, m_Info(m_pDoc->Book().GetInfo().GetInfo(inType).GetItemName())
	, m_Names()
	, m_nAdded(0)
	, m_CurSel()
	, m_viewVis(ViewVis::All)
	, m_viewUse(ViewUse::All)
	, m_ctrlEdit(nullptr)
	, m_ctrlDelete(nullptr)
	, m_ctrlCopy(nullptr)
	, m_ctrlList(nullptr)
	, m_reportColumn()
	, m_imgNone(-1)
	, m_imgNote(-1)
	, m_imgAdded(-1)
	, m_imgNoteAdded(-1)
	, m_textCount(nullptr)
	, m_trace("CDlgInfoNote")
{
	wxString caption = L"?";
	std::set<wxString> allNames;
	switch (m_type)
	{
	case ARBInfoType::Club:
		// Names in use only
		m_pDoc->Book().GetAllClubNames(m_NamesInUse, false, false);
		// Names in use, plus names added
		m_pDoc->Book().GetAllClubNames(allNames, true, false);
		caption = _("IDS_INFO_CLUB");
		break;
	case ARBInfoType::Judge:
		m_pDoc->Book().GetAllJudges(m_NamesInUse, false, false);
		m_pDoc->Book().GetAllJudges(allNames, true, false);
		caption = _("IDS_INFO_JUDGE");
		break;
	case ARBInfoType::Location:
		m_pDoc->Book().GetAllTrialLocations(m_NamesInUse, false, false);
		m_pDoc->Book().GetAllTrialLocations(allNames, true, false);
		caption = _("IDS_INFO_LOCATION");
		break;
	}
	m_InfoOrig.Clone(m_Info);

	// inSelect is from an active dialog edit - make sure it exists - and is 'inuse'.
	// It may not since the user may have just added it.
	bool addedSelection = false;
	if (!inSelect.empty())
	{
		if (allNames.end() == std::find(allNames.begin(), allNames.end(), inSelect))
		{
			addedSelection = true;
			allNames.insert(inSelect);
		}
	}

	m_Names.reserve(allNames.size());
	for (std::set<wxString>::iterator iter = allNames.begin(); iter != allNames.end(); ++iter)
	{
		NameInfo data(*iter);
		ARBInfoItemPtr item;
		if (m_NamesInUse.end() != std::find(m_NamesInUse.begin(), m_NamesInUse.end(), data.m_name))
			data.m_usage = NameInfo::Usage::InUse;
		else if (addedSelection && data.m_name == inSelect)
			data.m_usage = NameInfo::Usage::InUse;
		else
			++m_nAdded;
		m_Names.push_back(data);
	}

	if (!parent)
		parent = wxGetApp().GetTopWindow();
	wxDialog::Create(
		parent,
		wxID_ANY,
		caption,
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	// Controls (these are done first to control tab order)

	wxButton* ctrlNew = new wxButton(this, wxID_ANY, _("IDC_INFONOTE_NEW"), wxDefaultPosition, wxDefaultSize, 0);
	ctrlNew->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [this](wxCommandEvent& evt) { DoEdit(-1); });
	ctrlNew->SetHelpText(_("HIDC_INFONOTE_NEW"));
	ctrlNew->SetToolTip(_("HIDC_INFONOTE_NEW"));

	m_ctrlEdit = new wxButton(this, wxID_ANY, _("IDC_INFONOTE_EDIT"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEdit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, [this](wxCommandEvent& evt) { DoEdit(); });
	m_ctrlEdit->SetHelpText(_("HIDC_INFONOTE_EDIT"));
	m_ctrlEdit->SetToolTip(_("HIDC_INFONOTE_EDIT"));

	m_ctrlDelete = new wxButton(this, wxID_ANY, _("IDC_INFONOTE_DELETE"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlDelete->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgInfoNote::OnDeleteItem, this);
	m_ctrlDelete->SetHelpText(_("HIDC_INFONOTE_DELETE"));
	m_ctrlDelete->SetToolTip(_("HIDC_INFONOTE_DELETE"));

	m_ctrlCopy = new wxButton(this, wxID_ANY, _("IDC_INFONOTE_COPY"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlCopy->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgInfoNote::OnCopyItems, this);
	m_ctrlCopy->SetHelpText(_("HIDC_INFONOTE_COPY"));
	m_ctrlCopy->SetToolTip(_("HIDC_INFONOTE_COPY"));

	m_ctrlList = new CReportListCtrl(this, false, CReportListCtrl::SortHeader::Sort, true, true);
	m_ctrlList->SetId(LIST_CTRL);
	m_imgNone = m_ctrlList->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrBlank));
	m_imgNote = m_ctrlList->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrInfoNote));
	m_imgAdded = m_ctrlList->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrInfoNoteAdded));
	m_imgNoteAdded = m_ctrlList->AddIcon(CResourceManager::Get()->GetIcon(ImageMgrInfoNoteNoteAdded));
	m_ctrlList->SetHelpText(_("HIDC_INFONOTE"));
	m_ctrlList->SetToolTip(_("HIDC_INFONOTE"));
	m_ctrlList->EnableCheckBoxes();
	assert(m_ctrlList->HasCheckBoxes());
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, [this](wxListEvent& evt) { UpdateControls(); });
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_ITEM_DESELECTED, [this](wxListEvent& evt) { UpdateControls(); });
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, [this](wxListEvent& evt) { DoEdit(); });
	m_ctrlList->Bind(wxEVT_KEY_DOWN, [this](wxKeyEvent& evt) {
		switch (evt.GetKeyCode())
		{
		default:
			evt.Skip();
			break;
		case WXK_SPACE:
		case WXK_NUMPAD_SPACE:
			DoEdit();
			break;
		}
	});
	m_ctrlList->Bind(wxEVT_LIST_ITEM_CHECKED, [this](wxListEvent& evt) {
		if (wxNOT_FOUND != evt.GetIndex())
		{
			auto data = GetData(evt.GetIndex());
			if (data)
			{
				ARBInfoItemPtr item = data->UpdateItem(m_Names[data->GetIndex()].m_name, m_Info);
				item->SetIsVisible(true);
				UpdateItem(evt.GetIndex(), data->GetIndex());
			}
		}
	});
	m_ctrlList->Bind(wxEVT_LIST_ITEM_UNCHECKED, [this](wxListEvent& evt) {
		if (wxNOT_FOUND != evt.GetIndex())
		{
			auto data = GetData(evt.GetIndex());
			if (data)
			{
				ARBInfoItemPtr item = data->UpdateItem(m_Names[data->GetIndex()].m_name, m_Info);
				item->SetIsVisible(false);
				UpdateItem(evt.GetIndex(), data->GetIndex());
			}
		}
	});

	m_reportColumn.Initialize(this, m_ctrlList);
	m_reportColumn.CreateColumns(k_columns, k_colName);

	wxStaticBox* boxVis = new wxStaticBox(this, wxID_ANY, _("IDC_INFONOTE_VIS"));

	auto ctrlVisAll
		= new wxRadioButton(this, wxID_ANY, _("IDC_INFONOTE_VIS_ALL"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	ctrlVisAll->SetHelpText(_("HIDC_INFONOTE_VIS_ALL"));
	ctrlVisAll->SetToolTip(_("HIDC_INFONOTE_VIS_ALL"));
	ctrlVisAll->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, [this](wxCommandEvent& evt) {
		if (ViewVis::All != m_viewVis)
		{
			m_viewVis = ViewVis::All;
			LoadItems();
		}
	});

	auto ctrlVisVis = new wxRadioButton(this, wxID_ANY, _("IDC_INFONOTE_VIS_VIS"), wxDefaultPosition, wxDefaultSize, 0);
	ctrlVisVis->SetHelpText(_("HIDC_INFONOTE_VIS_VIS"));
	ctrlVisVis->SetToolTip(_("HIDC_INFONOTE_VIS_VIS"));
	ctrlVisVis->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, [this](wxCommandEvent& evt) {
		if (ViewVis::Visible != m_viewVis)
		{
			m_viewVis = ViewVis::Visible;
			LoadItems();
		}
	});

	auto ctrlVisHidden
		= new wxRadioButton(this, wxID_ANY, _("IDC_INFONOTE_VIS_HIDDEN"), wxDefaultPosition, wxDefaultSize, 0);
	ctrlVisHidden->SetHelpText(_("HIDC_INFONOTE_VIS_HIDDEN"));
	ctrlVisHidden->SetToolTip(_("HIDC_INFONOTE_VIS_HIDDEN"));
	ctrlVisHidden->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, [this](wxCommandEvent& evt) {
		if (ViewVis::Hidden != m_viewVis)
		{
			m_viewVis = ViewVis::Hidden;
			LoadItems();
		}
	});

	wxStaticBox* boxUse = new wxStaticBox(this, wxID_ANY, _("IDC_INFONOTE_USE"));

	auto ctrlUsageAll
		= new wxRadioButton(this, wxID_ANY, _("IDC_INFONOTE_USE_ALL"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	ctrlUsageAll->SetHelpText(_("HIDC_INFONOTE_USE_ALL"));
	ctrlUsageAll->SetToolTip(_("HIDC_INFONOTE_USE_ALL"));
	ctrlUsageAll->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, [this](wxCommandEvent& evt) {
		if (ViewUse::All != m_viewUse)
		{
			m_viewUse = ViewUse::All;
			LoadItems();
		}
	});

	auto ctrlUsageInUse
		= new wxRadioButton(this, wxID_ANY, _("IDC_INFONOTE_USE_INUSE"), wxDefaultPosition, wxDefaultSize, 0);
	ctrlUsageInUse->SetHelpText(_("HIDC_INFONOTE_USE_INUSE"));
	ctrlUsageInUse->SetToolTip(_("HIDC_INFONOTE_USE_INUSE"));
	ctrlUsageInUse->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, [this](wxCommandEvent& evt) {
		if (ViewUse::InUse != m_viewUse)
		{
			m_viewUse = ViewUse::InUse;
			LoadItems();
		}
	});

	auto ctrlUsageNot
		= new wxRadioButton(this, wxID_ANY, _("IDC_INFONOTE_USE_NOTINUSE"), wxDefaultPosition, wxDefaultSize, 0);
	ctrlUsageNot->SetHelpText(_("HIDC_INFONOTE_USE_NOTINUSE"));
	ctrlUsageNot->SetToolTip(_("HIDC_INFONOTE_USE_NOTINUSE"));
	ctrlUsageNot->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, [this](wxCommandEvent& evt) {
		if (ViewUse::NotInUse != m_viewUse)
		{
			m_viewUse = ViewUse::NotInUse;
			LoadItems();
		}
	});

	m_textCount = new wxStaticText(this, wxID_ANY, wxString(), wxDefaultPosition, wxDefaultSize, 0);

	LoadItems(&inSelect);

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sizerVis = new wxStaticBoxSizer(boxVis, wxVERTICAL);
	sizerVis->Add(ctrlVisAll, 0, wxLEFT | wxRIGHT | wxTOP, padding.Inner());
	sizerVis->Add(ctrlVisVis, 0, wxLEFT | wxRIGHT | wxTOP, padding.Inner());
	sizerVis->Add(ctrlVisHidden, 0, wxALL, padding.Inner());

	wxStaticBoxSizer* sizerUse = new wxStaticBoxSizer(boxUse, wxVERTICAL);
	sizerUse->Add(ctrlUsageAll, 0, wxLEFT | wxRIGHT | wxTOP, padding.Inner());
	sizerUse->Add(ctrlUsageInUse, 0, wxLEFT | wxRIGHT | wxTOP, padding.Inner());
	sizerUse->Add(ctrlUsageNot, 0, wxALL, padding.Inner());

	wxBoxSizer* sizerButtons = new wxBoxSizer(wxVERTICAL);
	sizerButtons->Add(ctrlNew, 0, wxBOTTOM, padding.Inner());
	sizerButtons->Add(m_ctrlEdit, 0, wxBOTTOM, padding.Inner());
	sizerButtons->Add(m_ctrlDelete, 0, wxBOTTOM, padding.Inner());
	sizerButtons->Add(m_ctrlCopy);
	sizerButtons->AddStretchSpacer();
	sizerButtons->Add(sizerVis, 0, wxTOP, padding.Controls());
	sizerButtons->Add(sizerUse, 0, wxTOP, padding.Controls());

	wxBoxSizer* sizerControls = new wxBoxSizer(wxHORIZONTAL);
	sizerControls->Add(m_ctrlList, 1, wxEXPAND | wxRIGHT, padding.Controls());
	sizerControls->Add(sizerButtons);

	bSizer->Add(sizerControls, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	bSizer->Add(m_textCount, 0, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());

	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgInfoNote::OnOk, this, wxID_OK);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	CConfigInfoNote pos;
	pos.Set(this, true);

	IMPLEMENT_ON_INIT(m_ctrlList)
}


wxString CDlgInfoNote::GetCaption() const
{
	wxString caption;
	switch (m_type)
	{
	case ARBInfoType::Club:
		caption = _("IDS_COL_CLUB");
		break;
	case ARBInfoType::Judge:
		caption = _("IDS_COL_JUDGE");
		break;
	case ARBInfoType::Location:
		caption = _("IDS_COL_LOCATION");
		break;
	}
	return caption;
}


wxString CDlgInfoNote::CurrentSelection() const
{
	return m_CurSel;
}


bool CDlgInfoNote::NameExists(wxString const& name)
{
	auto iter = std::find(m_Names.begin(), m_Names.end(), name);
	if (iter == m_Names.end())
		return false;
	size_t idxName = iter - m_Names.begin();
	return NameInfo::Usage::Deleted != m_Names[idxName].m_usage;
}


size_t CDlgInfoNote::AddName(wxString const& name, UpdateStatus& status)
{
	size_t idxName = 0;
	auto iter = std::find(m_Names.begin(), m_Names.end(), name);
	if (iter != m_Names.end())
	{
		// Ok, it exists, but it may have been deleted.
		idxName = iter - m_Names.begin();
		// Added items cannot be in-use. So we're re-adding.
		if (NameInfo::Usage::Deleted == m_Names[idxName].m_usage)
		{
			m_Names[idxName].m_usage = NameInfo::Usage::NotInUse;
			++m_nAdded;
			status = UpdateStatus::ReAdded;
			m_Info.AddItem(m_Names[idxName].m_name);
		}
		else
			status = UpdateStatus::Exists;
	}
	else
	{
		// Brand new name!
		status = UpdateStatus::Added;
		NameInfo data(name);
		m_Names.push_back(data);
		idxName = m_Names.size() - 1;
		++m_nAdded;
		m_Info.AddItem(m_Names[idxName].m_name);
	}
	return idxName;
}


bool CDlgInfoNote::DeleteName(size_t idxName)
{
	if (m_NamesInUse.end() == m_NamesInUse.find(m_Names[idxName].m_name))
	{
		ARBInfoItemPtr item;
		if (m_Info.FindItem(m_Names[idxName].m_name, &item))
		{
			m_Info.DeleteItem(item);
		}
		--m_nAdded;
		// Do NOT remove the name from m_Names. It will mess up
		// the indices of all the other items. But mark it
		// so we know if it's being re-added later.
		m_Names[idxName].m_usage = NameInfo::Usage::Deleted;
		return true;
	}
	else
		return false;
}


ARBInfoItemPtr CDlgInfoNote::FindName(wxString const& name) const
{
	ARBInfoItemPtr item;
	m_Info.FindItem(name, &item);
	return item;
}


InfoNoteListDataPtr CDlgInfoNote::GetData(long index) const
{
	return std::dynamic_pointer_cast<InfoNoteListData, CListData>(m_ctrlList->GetData(index));
}


bool CDlgInfoNote::IsItemVisible(size_t idxName) const
{
	if (idxName >= m_Names.size())
		return false;
	ARBInfoItemPtr item;
	m_Info.FindItem(m_Names[idxName].m_name, &item);
	// !item means there's no addition info. Defaults in use.
	// - NotInUse, IsVisible, no Comment

	bool vis = !item || item->IsVisible();
	bool inUse = m_Names[idxName].m_usage == NameInfo::Usage::InUse;

	switch (m_viewVis)
	{
	case ViewVis::All:
		break;
	case ViewVis::Visible:
		if (!vis)
			return false;
		break;
	case ViewVis::Hidden:
		if (vis)
			return false;
		break;
	}

	switch (m_viewUse)
	{
	case ViewUse::All:
		break;
	case ViewUse::InUse:
		if (!inUse)
			return false;
		break;
	case ViewUse::NotInUse:
		if (inUse)
			return false;
		break;
	}

	return true;
}


void CDlgInfoNote::LoadItems(wxString const* inSelect)
{
	std::vector<long> items;
	m_ctrlList->GetSelection(items);

	std::vector<size_t> idxSel;
	for (auto item : items)
	{
		auto data = GetData(item);
		if (data)
			idxSel.push_back(data->GetIndex());
	}

	m_ctrlList->DeleteAllItems();

	InfoNoteListDataPtr pSelected;
	for (size_t idxName = 0; idxName < m_Names.size(); ++idxName)
	{
		if (!IsItemVisible(idxName))
			continue;
		InfoNoteListDataPtr data(std::make_shared<InfoNoteListData>(this, idxName));
		m_ctrlList->InsertItem(data);
		if (inSelect && !pSelected && 0 < inSelect->length())
		{
			auto name = m_Names[idxName].m_name;
			if (0 == name.find(*inSelect))
			{
				pSelected = data;
			}
		}
	}
	if (inSelect)
		m_reportColumn.SizeColumns();
	m_reportColumn.Sort();

	UpdateText();

	if (pSelected || 0 < idxSel.size())
	{
		items.clear();
		for (long i = 0; i < m_ctrlList->GetItemCount(); ++i)
		{
			InfoNoteListDataPtr data = GetData(i);
			if (pSelected)
			{
				if (data->OnNeedText(1) == pSelected->OnNeedText(1))
				{
					m_ctrlList->Select(i, true);
					m_ctrlList->Focus(i);
					break;
				}
			}
			else
			{
				if (idxSel.end() != std::find(idxSel.begin(), idxSel.end(), data->GetIndex()))
					items.push_back(i);
			}
		}
		if (0 < items.size())
		{
			m_ctrlList->SetSelection(items);
			m_ctrlList->EnsureVisible(items[0]);
			m_ctrlList->Focus(items[0]);
		}
	}

	UpdateControls();
}


void CDlgInfoNote::UpdateText()
{
	if (static_cast<int>(m_Names.size()) == m_ctrlList->GetItemCount())
		m_textCount->SetLabel(wxString::Format(_("IDS_INFONOTE_FMT_COUNT"), m_ctrlList->GetItemCount()));
	else
		m_textCount->SetLabel(
			wxString::Format(_("IDS_INFONOTE_FMT_COUNT_OF"), m_ctrlList->GetItemCount(), m_Names.size()));
}


bool CDlgInfoNote::UpdateItem(long index, size_t idxName)
{
	bool vis = IsItemVisible(idxName);
	if (vis)
	{
		m_ctrlList->RefreshItem(index);
	}
	else
	{
		wxMessageBox(_("IDS_INFONOTE_FILTERED"), GetCaption());
		m_ctrlList->DeleteItem(index);
		UpdateText();
	}
	return vis;
}


void CDlgInfoNote::UpdateControls()
{
	std::vector<long> items;
	m_ctrlList->GetSelection(items);

	bool bEnableEdit = false;
	bool bEnableDelete = false;

	if (1 == items.size())
	{
		auto data = GetData(items[0]);
		if (data)
		{
			bEnableEdit = true;
			bEnableDelete = NameInfo::Usage::NotInUse == m_Names[data->GetIndex()].m_usage;
		}
	}
	else if (1 < items.size())
	{
		size_t deletable = 0;
		for (auto item : items)
		{
			auto data = GetData(item);
			if (data && NameInfo::Usage::NotInUse == m_Names[data->GetIndex()].m_usage)
			{
				++deletable;
			}
		}
		bEnableDelete = deletable == items.size();
	}
	m_ctrlEdit->Enable(bEnableEdit);
	m_ctrlDelete->Enable(bEnableDelete);
	m_ctrlCopy->Enable(m_Names.size() > 0);
}


void CDlgInfoNote::DoEdit()
{
	if (!m_ctrlList)
		return;
	auto index = m_ctrlList->GetSelection(true);
	if (wxNOT_FOUND != index)
		DoEdit(index);
}


void CDlgInfoNote::DoEdit(long index)
{
	InfoNoteListDataPtr data;
	wxString name, comment;
	bool isVisible = true;
	if (wxNOT_FOUND != index)
	{
		data = GetData(index);
		assert(data);
		name = data->OnNeedText(k_colName);
		comment = data->OnNeedText(k_colComment);
		isVisible = data->IsVisible();
	}

	CDlgInfoNoteEdit dlg(name, comment, isVisible, this);
	if (wxID_OK == dlg.ShowModal())
	{
		bool doSort = false;
		name = dlg.Name();
		comment = dlg.Comment();
		// Create new name entry. It's either a new one or we're modifying an existing entry.
		if (!data || data->OnNeedText(k_colName) != name)
		{
			// Find existing (deleted) entry, or create a new one.
			std::vector<NameInfo>::iterator iter = std::find(m_Names.begin(), m_Names.end(), name);
			if (iter != m_Names.end())
			{
				size_t idxName = iter - m_Names.begin();
				if (NameInfo::Usage::Deleted != m_Names[idxName].m_usage)
					iter = m_Names.end();
			}
			if (iter == m_Names.end())
			{
				NameInfo info(name);
				m_Names.push_back(info);
				iter = m_Names.end() - 1;
			}
			assert(iter != m_Names.end());
			if (iter != m_Names.end())
			{
				doSort = true;

				size_t idxName = iter - m_Names.begin();
				m_Info.AddItem(m_Names[idxName].m_name);

				ARBInfoItemPtr item;
				if (m_Info.FindItem(m_Names[idxName].m_name, &item))
				{
					item->SetIsVisible(dlg.IsVisible());
					item->SetComment(comment);
					if (data)
					{
						if (NameInfo::Usage::NotInUse == m_Names[data->GetIndex()].m_usage)
							data->UpdateItem(idxName, item);
						else
							data.reset();
					}
				}
				else
				{
					// We just did an AddItem - we BETTER find it!
					assert(0);
				}

				++m_nAdded;
				if (!data || item->HasData())
				{
					if (data)
					{
						if (!UpdateItem(index, idxName))
							doSort = false;
					}
					else
					{
						if (IsItemVisible(idxName))
						{
							data = std::make_shared<InfoNoteListData>(this, idxName);
							index = m_ctrlList->InsertItem(data);
							m_ctrlList->SetSelection(index, true);
							UpdateText();
						}
						else
						{
							// filter message
							doSort = false;
						}
					}
				}
			}
		}
		// Updating existing (no name change)
		else if (data)
		{
			ARBInfoItemPtr item = data->UpdateItem(m_Names[data->GetIndex()].m_name, m_Info);
			assert(item);
			if (item->IsVisible() != dlg.IsVisible())
			{
				doSort = true;
				item->SetIsVisible(dlg.IsVisible());
			}
			if (item->GetComment() != comment)
			{
				doSort = true;
				item->SetComment(comment);
			}
			if (doSort)
				UpdateItem(index, data->GetIndex());
		}
		if (doSort)
			m_reportColumn.Sort();
	}
}


void CDlgInfoNote::OnItemFocused(wxListEvent& evt)
{
	UpdateControls();
}


void CDlgInfoNote::OnDeleteItem(wxCommandEvent& evt)
{
	std::vector<long> items;
	if (0 < m_ctrlList->GetSelection(items))
	{
		for (size_t idx = items.size(); idx > 0; --idx)
		{
			long nItem = items[idx - 1];
			auto data = GetData(nItem);
			assert(data);
			if (data->GetItem())
				m_Info.DeleteItem(data->GetItem());
			m_ctrlList->DeleteItem(nItem);
			if (nItem == m_ctrlList->GetItemCount())
				--nItem;
			if (0 <= nItem)
				m_ctrlList->Select(nItem);
			UpdateText();
		}
	}
}


void CDlgInfoNote::OnCopyItems(wxCommandEvent& evt)
{
	std::vector<long> items;
	// Only get selection if SHIFT is down.
	if (::wxGetKeyState(WXK_SHIFT))
	{
		m_ctrlList->GetSelection(items);
	}
	// Even if SHIFT is down, if we didn't get anything, get all.
	if (items.empty())
	{
		items.resize(m_ctrlList->GetItemCount());
		std::iota(items.begin(), items.end(), 0);
	}
	if (items.empty())
		return;

	CClipboardDataWriter clpData;
	if (!clpData.isOkay())
		return;

	wxString tableData;
	wxString tableHtml;
	CClipboardDataTable table(tableData, tableHtml);

	table.StartLine();
	table.Cell(0, _("IDC_INFONOTE_VIS_VIS"));
	table.Cell(1, _("IDC_INFONOTE_USE_INUSE"));
	table.Cell(2, _("IDS_COL_NAME"));
	table.Cell(3, _("IDS_COL_COMMENTS"));
	table.EndLine();

	for (size_t idx = 0; idx < items.size(); ++idx)
	{
		auto data = GetData(items[idx]);
		assert(data);
		table.StartLine();
		table.Cell(0, data->IsVisible() ? _("IDS_INFONOTE_YES") : _("IDS_INFONOTE_NO"));
		table.Cell(
			1,
			m_Names[data->GetIndex()].m_usage == NameInfo::Usage::NotInUse ? _("IDS_INFONOTE_NO")
																		   : _("IDS_INFONOTE_YES"));
		table.Cell(2, data->OnNeedText(k_colName));
		table.Cell(3, data->OnNeedText(k_colComment));
		table.EndLine();
	}

	clpData.AddData(table);
	clpData.CommitData();
}


void CDlgInfoNote::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	auto sel = m_ctrlList->GetSelection(true);
	if (wxNOT_FOUND != sel)
	{
		auto data = GetData(sel);
		assert(data);
		m_CurSel = m_Names[data->GetIndex()].m_name;
	}

	m_Info.CondenseContent(m_NamesInUse);
	if (m_Info != m_InfoOrig)
	{
		m_pDoc->Book().GetInfo().GetInfo(m_type) = m_Info;
		m_pDoc->Modify(true);
	}

	CConfigInfoNote pos;
	pos.SaveWindow(this);

	EndDialog(wxID_OK);
}

} // namespace dconSoft
