#pragma once

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
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-10 Ported to wxWidgets.
 * 2008-02-01 Add ability to see what was last selected.
 * 2008-01-01 Added visible flag.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2003-12-07 Created
 */

#include "ARB/ARBInfo.h"
#include <set>
#include <vector>
class CAgilityBookDoc;
class CDlgInfoNote;
class CSpellCheckCtrl;
class CTextCtrl;
class wxBitmapComboBox;


class NameInfo
{
public:
	enum class Usage
	{
		NotInUse,
		InUse,
		Deleted
	};

	NameInfo()
		: m_name()
		, m_usage(Usage::NotInUse)
		, m_hasData(false)
	{
	}
	NameInfo(std::wstring const& inName)
		: m_name(inName)
		, m_usage(Usage::NotInUse)
		, m_hasData(false)
	{
	}
	NameInfo(NameInfo const& rhs)
		: m_name(rhs.m_name)
		, m_usage(rhs.m_usage)
		, m_hasData(rhs.m_hasData)
	{
	}
	NameInfo(NameInfo&& rhs)
		: m_name(std::move(rhs.m_name))
		, m_usage(std::move(rhs.m_usage))
		, m_hasData(std::move(rhs.m_hasData))
	{
	}
	~NameInfo()
	{
	}

	NameInfo& operator=(NameInfo const& rhs)
	{
		if (this != &rhs)
		{
			m_name = rhs.m_name;
			m_usage = rhs.m_usage;
			m_hasData = rhs.m_hasData;
		}
		return *this;
	}
	NameInfo& operator=(NameInfo&& rhs)
	{
		if (this != &rhs)
		{
			m_name = std::move(rhs.m_name);
			m_usage = std::move(rhs.m_usage);
			m_hasData = std::move(rhs.m_hasData);
		}
		return *this;
	}
	bool operator==(NameInfo const& rhs) const
	{
		return m_name == rhs.m_name;
	}
	bool operator!=(NameInfo const& rhs) const
	{
		return !operator==(rhs);
	}

	std::wstring m_name;
	Usage m_usage;
	bool m_hasData;
};

/////////////////////////////////////////////////////////////////////////////

class InfoNotePanel : public wxPanel
{
public:
	static InfoNotePanel* CreateBasic(
		std::set<std::wstring> const& namesInUse,
		std::wstring const& inSelect,
		CDlgInfoNote* parent);
	static InfoNotePanel* CreateAlternate(
		std::set<std::wstring> const& namesInUse,
		std::wstring const& inSelect,
		CDlgInfoNote* parent);

	InfoNotePanel(std::set<std::wstring> const& namesInUse, std::wstring const& inSelect, CDlgInfoNote* parent);

	virtual wxWindow* GetInitialFocus() = 0; // Focus on display
	virtual void LoadData() = 0;             // Load/reload data on display
	virtual bool OnOk() = 0;                 // Update selection before main OnOk processes

	wxString GetSelection() const
	{
		return m_Select;
	}
	void SetSelection(wxString const& select)
	{
		m_Select = select;
	}

protected:
	CDlgInfoNote* m_parent;
	std::set<std::wstring> const& m_NamesInUse;
	wxString m_Select;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgInfoNote : public wxDialog
{
public:
	CDlgInfoNote(CAgilityBookDoc* pDoc, ARBInfoType inType, std::wstring const& inSelect, wxWindow* pParent = nullptr);

	wxString GetCaption() const;
	std::wstring CurrentSelection() const;
	void SetCurrentSelection(size_t current);

	enum class UpdateStatus
	{
		Exists,
		Added,
		ReAdded,
	};
	size_t AddName(std::wstring const& name, UpdateStatus& status);
	bool DeleteName(size_t idxName);
	void SetNameVisible(size_t idxName, bool visible);
	void SetNameComment(size_t idxName, std::wstring const& data);

	size_t GetAddedCount() const
	{
		return m_nAdded;
	}
	ARBInfoItemPtr FindName(std::wstring const& name) const;

	std::vector<NameInfo> const& GetNames() const
	{
		return m_Names;
	}

private:
	CAgilityBookDoc* m_pDoc;
	ARBInfoType m_type;
	std::set<std::wstring> m_NamesInUse;
	ARBInfoItemList const& m_InfoOrig;
	ARBInfoItemList m_Info;
	std::vector<NameInfo> m_Names;
	size_t m_nAdded;
	std::wstring m_CurSel;

	InfoNotePanel* m_panelBasic;
	InfoNotePanel* m_panelAlternate;

	DECLARE_ON_INIT()

	void OnOk(wxCommandEvent& evt);
};
