/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CAgilityBookOptions class
 * @author David Connet
 *
 * Revision History
 * @li 2013-06-15 DRC Make default for GetNewestDatesFirst true.
 * @li 2013-05-19 DRC Make last div/level/height/handler context aware.
 * @li 2012-07-04 DRC Add option to use run time or opening time in gamble OPS.
 * @li 2010-11-07 DRC Merge filters on program settings import also.
 * @li 2010-11-04 DRC Change importing program settings to merge columninfo.
 * @li 2010-03-28 DRC Added ability to import/export program settings (v2.2.0).
 * @li 2010-01-21 DRC Fixed font flag parsing.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-08-26 DRC Changed auto update check to false in debug mode.
 * @li 2009-08-08 DRC Changed default setting for proxy and show dog after title
 * @li 2009-07-19 DRC Implement proxy support. 
 * @li 2008-12-24 DRC Ported to wxWidgets.
 * @li 2008-01-14 DRC Make ViewLifetimeEvents default to true.
 * @li 2007-08-03 DRC Added UserNames
 * @li 2006-07-16 DRC Added PointsViewSort
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-08-18 DRC Changed how filter options are done.
 * @li 2005-05-04 DRC Added IncludeCRCDImage
 * @li 2004-12-18 DRC Added Opening/Closing dates to view, plus color.
 * @li 2004-08-31 DRC Added AutoShowSplashScreen
 * @li 2004-06-16 DRC Added options to remember date formats.
 * @li 2004-06-06 DRC Added additional clipboard formats.
 * @li 2004-06-01 DRC Venue filtering could allow filtered runs thru at times.
 * @li 2004-04-08 DRC Added general program options.
 * @li 2004-03-13 DRC Added GetViewHiddenTitles, updated IsTitleVisible.
 * @li 2004-01-04 DRC Added GetImportExportDateFormat.
 * @li 2003-12-30 DRC Implemented customized text in tree.
 * @li 2003-12-29 DRC Created default column orders.
 * @li 2003-12-11 DRC Added options for import/export wizard.
 * @li 2003-10-09 DRC Added *etViewRunsByTrial.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "AgilityBookOptions.h"

#include "DlgAssignColumns.h"
#include "RegItems.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBCalendar.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBDogTitle.h"
#include "ARB/ARBDogTrial.h"
#include "ARBCommon/ARBDate.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "ARBCommon/VersionNum.h"
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

namespace
{
	// Calendar Defaults
	static const bool sc_AutoDeleteCalEntries = false;
	static const bool sc_ViewAllCalEntries = true;
	static const bool sc_ViewAllCalOpening = true;
	static const bool sc_ViewAllCalClosing = true;
	static const long sc_DaysTillEntryIsPast = 5;
	static const bool sc_HideOverlapping = false;
	static const wxColour sc_CalColorPast(128,128,128); // gray
	static const wxColour sc_CalColorNotEntered(0,0,0); // Black
	static const wxColour sc_CalColorPlanning(255,128,0); // Orange
	static const wxColour sc_CalColorPending(128,0,255); // Blue-ish
	static const wxColour sc_CalColorEntered(0,0,255); // Blue
	static const wxColour sc_CalColorOpening(0,128,0); // Dk Green
	static const wxColour sc_CalColorClosing(255,0,0); // Red
	static const long sc_CalOpeningNear = 4;
	static const long sc_CalClosingNear = 10;
	static const wxColour sc_CalColorOpeningNear(0,0,255);
	static const wxColour sc_CalColorClosingNear(255,0,0);

	// Common
	static const ARBDate::DayOfWeek sc_FirstDayOfWeek = ARBDate::eSunday;

	// Runs/points
	static const bool sc_ViewRunsByTrial = true;
	static const bool sc_NewestDatesFirst = true;
	static const CAgilityBookOptions::PointsViewSort sc_PointsViewSort1 = CAgilityBookOptions::ePointsViewSortDivision;
	static const CAgilityBookOptions::PointsViewSort sc_PointsViewSort2 = CAgilityBookOptions::ePointsViewSortLevel;
	static const CAgilityBookOptions::PointsViewSort sc_PointsViewSort3 = CAgilityBookOptions::ePointsViewSortEvent;
	static const bool sc_ViewHiddenTitles = false;
	static const bool sc_ViewLifetimePointsByEvent = true;
	static const bool sc_TableInYPS = false;
	static const bool sc_RunTimeInOPS = false;
	static const bool sc_IncludeCRCDImage = false;

	// Font
	static const wchar_t* const sc_FontPrinterName = L"Times New Roman";
	static const int sc_FontPrinterSize = 8;
	static const bool sc_FontPrinterItalic = false;
	static const bool sc_FontPrinterBold = false;
	static const bool sc_UnitsAsMM = false;
	static const bool sc_MarginAsMM = false;
	static const long sc_MarginLeft = 50;
	static const long sc_MarginRight = 50;
	static const long sc_MarginTop = 50;
	static const long sc_MarginBottom = 50;
	static const bool sc_RunPageAsMM = false;
	static const long sc_RunPageWidth = 0;
	static const long sc_RunPageHeight = 0;
	static const wchar_t* const sc_FontCalName = L"Times New Roman";
	static const int sc_FontCalSize = 8;
	static const bool sc_FontCalItalic = false;
	static const bool sc_FontCalBold = false;

	// Import/export
	static const long sc_ImportStartRow = 1;
	static const long sc_ImportExportDelim = CAgilityBookOptions::eDelimTab;
	static const ARBDate::DateFormat sc_ImportExportFormat = ARBDate::eISO;
	// Note: Default column order is defined in GetColumnOrder()

	// Program
	static const long sc_MRUCount = 4;
	static const bool sc_AutoUpdateCheck = true;
	static const long sc_NumBackup = 3;
	static const bool sc_ShowPropOnNewTitle = false;
	static const bool sc_ShowHtmlPoints = true;
	static const bool sc_UseProxy = false;
};


static void ExportConfigItem(wxString const& entry, ElementNodePtr tree)
{
	switch (wxConfig::Get()->GetEntryType(entry))
	{
	default:
		break;
	case wxConfigBase::Type_String:
		{
			ElementNodePtr item = tree->AddElementNode(StringUtil::stringW(entry));
			item->AddAttrib(L"type", L"s");
			item->SetValue(wxConfig::Get()->Read(entry, wxEmptyString));
		}
		break;
	case wxConfigBase::Type_Boolean:
		{
			ElementNodePtr item = tree->AddElementNode(StringUtil::stringW(entry));
			item->AddAttrib(L"type", L"b");
			bool val;
			wxConfig::Get()->Read(entry, &val);
			item->AddAttrib(L"val", val);
		}
		break;
	case wxConfigBase::Type_Integer:
		{
			ElementNodePtr item = tree->AddElementNode(StringUtil::stringW(entry));
			item->AddAttrib(L"type", L"i");
			long val;
			wxConfig::Get()->Read(entry, &val);
			item->AddAttrib(L"val", val);
		}
		break;
	case wxConfigBase::Type_Float:
		{
			ElementNodePtr item = tree->AddElementNode(StringUtil::stringW(entry));
			item->AddAttrib(L"type", L"f");
			double val;
			wxConfig::Get()->Read(entry, &val);
			item->AddAttrib(L"val", val);
		}
		break;
	}
}


static void ExportConfig(wxString const& key, ElementNodePtr root)
{
	if (!wxConfig::Get()->HasGroup(key))
		return;

	ElementNodePtr tree = root->AddElementNode(StringUtil::stringW(key));
	tree->AddAttrib(L"type", L"g");

	wxString path = wxConfig::Get()->GetPath();
	wxConfig::Get()->SetPath(key);

	wxString entry;
	long index = 0;
	if (wxConfig::Get()->GetFirstEntry(entry, index))
	{
		do
		{
			ExportConfigItem(entry, tree);
		} while (wxConfig::Get()->GetNextEntry(entry, index));
	}
	if (wxConfig::Get()->GetFirstGroup(entry, index))
	{
		do
		{
			ExportConfig(entry, tree);
		} while (wxConfig::Get()->GetNextGroup(entry, index));
	}

	wxConfig::Get()->SetPath(path);
}


static ElementNodePtr FindElementName(
		ElementNodePtr tree,
		long numConfigs,
		std::wstring const& name,
		const wchar_t *const eleItem,
		const wchar_t *const eleName)
{
	for (long i = 0; i < numConfigs; ++i)
	{
		std::wstring configName = StringUtil::stringW(wxString::Format(L"%s%ld", eleItem, i));
		int idxConfig = tree->FindElement(configName);
		if (0 <= idxConfig
		&& Element::Element_Node == tree->GetElement(idxConfig)->GetType())
		{
			ElementNodePtr nodeConfig = tree->GetElementNode(idxConfig);
			int idxName = nodeConfig->FindElement(L"name");
			if (0 <= idxName
			&& name == nodeConfig->GetElementNode(idxName)->GetValue())
			{
				return nodeConfig;
			}
		}
	}
	return ElementNodePtr();
}


static void ImportConfig(ElementNodePtr tree, bool bClobberFilters);


// Return true to invoke clobber code below.
static bool ImportColumnInfo(ElementNodePtr tree)
{
	// Only "Config#" are merged (unless we clobber).
	int idxNum = tree->FindElement(L"numConfigs");
	if (0 > idxNum)
		return false;
	long numConfigs = 0;
	tree->GetElementNode(idxNum)->GetAttrib(L"val", numConfigs);
	if (0 >= numConfigs)
		return false;

	// If there are no existing configs, just bail and clobber.
	ElementNodePtr root(ElementNode::New(L"Top"));
	wxString path = wxConfig::Get()->GetPath();
	wxConfig::Get()->SetPath(L"/");
	ExportConfig(L"ColumnInfo", root);
	wxConfig::Get()->SetPath(path);
	if (1 != root->GetElementCount())
		return true;
	ElementNodePtr existing = root->GetElementNode(0);
	if (!existing)
		return true;
	idxNum = existing->FindElement(L"numConfigs");
	if (0 > idxNum)
		return true;
	long numExistingConfigs = 0;
	existing->GetElementNode(idxNum)->GetAttrib(L"val", numExistingConfigs);
	if (0 >= numExistingConfigs)
		return true;

	long added = 0;
	for (long i = 0; i < numConfigs; ++i)
	{
		std::wstring configName = StringUtil::stringW(wxString::Format(L"Config%ld", i));
		int idxConfig = tree->FindElement(configName);
		if (0 <= idxConfig
		&& Element::Element_Node == tree->GetElement(idxConfig)->GetType())
		{
			ElementNodePtr nodeConfig = tree->GetElementNode(idxConfig);
			int idxName = nodeConfig->FindElement(L"name");
			if (0 > idxName)
				continue; // Ignore no-name configs.
			std::wstring name = nodeConfig->GetElementNode(idxName)->GetValue();
			ElementNodePtr existingConfig = FindElementName(existing, numExistingConfigs, name, L"Config", L"name");
			if (existingConfig)
			{
				path = wxConfig::Get()->GetPath();
				wxConfig::Get()->SetPath(StringUtil::stringWX(existingConfig->GetName()));
				for (int iCfg = 0; iCfg < nodeConfig->GetElementCount(); ++iCfg)
				{
					ElementNodePtr node = nodeConfig->GetElementNode(iCfg);
					if (!node)
						continue;
					if (node->GetName() != L"name")
					{
						ImportConfig(node, false);
					}
				}
				wxConfig::Get()->SetPath(path);
			}
			else
			{
				name = nodeConfig->GetName();
				std::wstring newName = StringUtil::stringW(wxString::Format(L"Config%ld", numExistingConfigs + added));
				++added;
				nodeConfig->SetName(newName);
				ImportConfig(nodeConfig, false);
				nodeConfig->SetName(name);
			}
		}
	}
	if (0 < added)
		wxConfig::Get()->Write(CFG_CI_NUMCONFIGS, numExistingConfigs + added);
	return false;
}


// We will not merge the current named filter selection
static bool MergeFilters(ElementNodePtr tree)
{
	int idx = tree->FindElement(L"Common");
	if (0 > idx)
		return false;
	ElementNodePtr nodeCommon = tree->GetElementNode(idx);
	if (!nodeCommon)
		return false;
	idx = nodeCommon->FindElement(L"numFilters");
	if (0 > idx)
		return false;
	long numFilters = 0;
	nodeCommon->GetElementNode(idx)->GetAttrib(L"val", numFilters);
	if (0 >= numFilters)
		return false;

	// If there are no existing filters, just bail and clobber.
	int numExistingFilters = wxConfig::Get()->Read(CFG_COMMON_NUMFILTERS, 0L);
	if (0 > numExistingFilters)
		return true;
	// If there are no existing configs, just bail and clobber.
	ElementNodePtr root(ElementNode::New(L"Top"));
	wxString path = wxConfig::Get()->GetPath();
	wxConfig::Get()->SetPath(L"/");
	for (int i = 0; i < numExistingFilters; ++i)
	{
		ExportConfig(CFG_KEY_FILTER(i, false), root);
	}
	wxConfig::Get()->SetPath(path);
	if (numExistingFilters != root->GetElementCount())
		return true;

	long added = 0;
	for (long i = 0; i < numFilters; ++i)
	{
		std::wstring configName = StringUtil::stringW(wxString::Format(L"Filter%ld", i));
		int idxFilter = tree->FindElement(configName);
		if (0 <= idxFilter
		&& Element::Element_Node == tree->GetElement(idxFilter)->GetType())
		{
			ElementNodePtr nodeFilter = tree->GetElementNode(idxFilter);
			int idxName = nodeFilter->FindElement(L"Name");
			if (0 > idxName)
				continue; // Ignore no-name filters.
			std::wstring name = nodeFilter->GetElementNode(idxName)->GetValue();
			ElementNodePtr existingFilter = FindElementName(root, numExistingFilters, name, L"Filter", L"Name");
			if (existingFilter)
			{
				path = wxConfig::Get()->GetPath();
				wxConfig::Get()->SetPath(StringUtil::stringWX(existingFilter->GetName()));
				for (int iCfg = 0; iCfg < nodeFilter->GetElementCount(); ++iCfg)
				{
					ElementNodePtr node = nodeFilter->GetElementNode(iCfg);
					if (!node)
						continue;
					if (node->GetName() != L"Name")
					{
						ImportConfig(node, true);
					}
				}
				wxConfig::Get()->SetPath(path);
			}
			else
			{
				name = nodeFilter->GetName();
				std::wstring newName = StringUtil::stringW(wxString::Format(L"Filter%ld", numExistingFilters + added));
				++added;
				nodeFilter->SetName(newName);
				ImportConfig(nodeFilter, true);
				nodeFilter->SetName(name);
			}
		}
	}
	if (0 < added)
		wxConfig::Get()->Write(CFG_COMMON_NUMFILTERS, numExistingFilters + added);
	return false;
}


static void ImportConfig(ElementNodePtr tree, bool bClobberFilters)
{
	std::wstring type;
	if (ElementNode::eFound != tree->GetAttrib(L"type", type))
		return;
	if (L"g" == type)
	{
		wxString path = wxConfig::Get()->GetPath();
		wxConfig::Get()->SetPath(StringUtil::stringWX(tree->GetName()));
		// When importing config info, treat ColumnInfo differently.
		// We don't want to wipe out existing items. We want to add the new
		// names. If we find an existing name, the specified format will
		// be clobbered. But any existing ones won't. This allows us to
		// import only "Import" data and not touch "Export" (depending on
		// what's in the file we're reading, of course!)
		bool bClobber = true;
		if (tree->GetName() == L"ColumnInfo")
		{
			bClobber = ImportColumnInfo(tree);
		}
		else if (!bClobberFilters && tree->GetName().substr(0, 6) == L"Filter")
		{
			bClobber = false;
		}
		if (bClobber)
		{
			bool bCommon = (tree->GetName() == L"Common");
			for (int i = 0; i < tree->GetElementCount(); ++i)
			{
				ElementNodePtr node = tree->GetElementNode(i);
				if (!node)
					continue;
				bool bSkip = false;
				if (!bClobberFilters && bCommon
				&& (node->GetName() == L"numFilters"
				|| node->GetName() == L"CurrentFilter"))
				{
					bSkip = true;
				}
				if (!bSkip)
					ImportConfig(node, bClobberFilters);
			}
		}
		wxConfig::Get()->SetPath(path);
	}
	else if (L"s" == type)
	{
		wxConfig::Get()->Write(StringUtil::stringWX(tree->GetName()), tree->GetValue().c_str());
	}
	else if (L"b" == type)
	{
		bool val;
		tree->GetAttrib(L"val", val);
		wxConfig::Get()->Write(StringUtil::stringWX(tree->GetName()), val);
	}
	else if (L"i" == type)
	{
		long val;
		tree->GetAttrib(L"val", val);
		wxConfig::Get()->Write(StringUtil::stringWX(tree->GetName()), val);
	}
	else if (L"f" == type)
	{
		double val;
		tree->GetAttrib(L"val", val);
		wxConfig::Get()->Write(StringUtil::stringWX(tree->GetName()), val);
	}
}


bool CAgilityBookOptions::ImportSettings(ElementNodePtr tree)
{
	if (!tree || tree->GetName() != L"AgilityBookSettings")
		return false;
	// Version numbers aren't needed yet.
	ARBVersion version;
	if (ElementNode::eFound != version.GetAttrib(tree, ATTRIB_BOOK_VERSION))
		return false;
	std::wstring pgmVersion;
	if (ElementNode::eFound != tree->GetAttrib(ATTRIB_BOOK_PGM_VERSION, pgmVersion))
		return false;
	bool bClobberFilters = MergeFilters(tree);
	for (int i = 0; i < tree->GetElementCount(); ++i)
	{
		ElementNodePtr ele = tree->GetElementNode(i);
		if (!ele)
			continue;
		ImportConfig(ele, bClobberFilters);
	}
	return true;
}


ElementNodePtr CAgilityBookOptions::ExportSettings()
{
	wxBusyCursor wait;
	CVersionNum ver(true);
	std::wstring verstr = ver.GetVersionString();
	ElementNodePtr tree(ElementNode::New(L"AgilityBookSettings"));
	ARBAgilityRecordBook::GetCurrentDocVersion().AddAttrib(tree, ATTRIB_BOOK_VERSION);
	tree->AddAttrib(ATTRIB_BOOK_PGM_VERSION, verstr);

	// These sections are copied complete.
	static wchar_t const* const sections[] = {
		L"Calendar",
		L"Columns",
		L"ColumnInfo",
		L"Common",
		L"Export",
		L"ExportCal",
		L"ExportCalAppt",
		L"ExportCalTask",
		L"ExportLog",
		L"Import",
		L"ImportCal",
		L"ImportLog",
		L"Sorting",
		NULL
	};
	for (int i = 0; sections[i]; ++i)
	{
		ExportConfig(sections[i], tree);
	}

	// Copy all the filters complete.
	int nFilters = wxConfig::Get()->Read(CFG_COMMON_NUMFILTERS, 0L);
	for (int i = 0; i < nFilters; ++i)
	{
		ExportConfig(CFG_KEY_FILTER(i, false), tree);
	}

	// And pick/choose in Settings.
	ElementNodePtr settings = tree->AddElementNode(L"Settings");
	settings->AddAttrib(L"type", L"g");
	wxString path = wxConfig::Get()->GetPath();
	wxConfig::Get()->SetPath(L"Settings");
	static wchar_t const* const items[] = {
		L"autoCheck",
		L"autoShowTitle",
		L"printLand",
		L"proxy",
		L"showHtml",
		L"useproxy",
		L"View",
		L"ViewOrient",
		L"ViewType",
		NULL
	};
	for (int i = 0; items[i]; ++i)
	{
		ExportConfigItem(items[i], settings);
	}
	wxConfig::Get()->SetPath(path);
	return tree;
}

/////////////////////////////////////////////////////////////////////////////

void CFontInfo::CreateFont(wxFont& font)
{
	font = wxFont();
	font.SetFaceName(name.c_str());
	font.SetPointSize(size);
	int style = wxFONTSTYLE_NORMAL;
	if (italic)
		style = wxFONTSTYLE_ITALIC;
	int weight = wxFONTWEIGHT_NORMAL;
	if (bold)
		weight = wxFONTWEIGHT_BOLD;
	font.SetStyle(style);
	font.SetWeight(weight);
}


void CFontInfo::CreateFont(
		wxFontDialog& dlg,
		wxFont& font)
{
	font = dlg.GetFontData().GetChosenFont();
	size = font.GetPointSize();
	name = font.GetFaceName();
	italic = (font.GetStyle() == wxFONTSTYLE_ITALIC);
	bold = (font.GetWeight() == wxFONTWEIGHT_BOLD);
}

/////////////////////////////////////////////////////////////////////////////
// Calendar options

bool CAgilityBookOptions::AutoDeleteCalendarEntries()
{
	bool val = sc_AutoDeleteCalEntries;
	wxConfig::Get()->Read(CFG_CAL_AUTODELETE, &val);
	return val;
}


void CAgilityBookOptions::SetAutoDeleteCalendarEntries(bool bAuto)
{
	wxConfig::Get()->Write(CFG_CAL_AUTODELETE, bAuto);
}


// View all or hide old entries
bool CAgilityBookOptions::ViewAllCalendarEntries()
{
	bool val = sc_ViewAllCalEntries;
	wxConfig::Get()->Read(CFG_CAL_VIEWALL, &val);
	return val;
}


void CAgilityBookOptions::SetViewAllCalendarEntries(bool bView)
{
	wxConfig::Get()->Write(CFG_CAL_VIEWALL, bView);
}


bool CAgilityBookOptions::ViewAllCalendarOpening()
{
	bool val = sc_ViewAllCalOpening;
	wxConfig::Get()->Read(CFG_CAL_VIEWOPEN, &val);
	return val;
}


void CAgilityBookOptions::SetViewAllCalendarOpening(bool bView)
{
	wxConfig::Get()->Write(CFG_CAL_VIEWOPEN, bView);
}


bool CAgilityBookOptions::ViewAllCalendarClosing()
{
	bool val = sc_ViewAllCalClosing;
	wxConfig::Get()->Read(CFG_CAL_VIEWCLOSE, &val);
	return val;
}


void CAgilityBookOptions::SetViewAllCalendarClosing(bool bView)
{
	wxConfig::Get()->Write(CFG_CAL_VIEWCLOSE, bView);
}


long CAgilityBookOptions::DaysTillEntryIsPast()
{
	long val = sc_DaysTillEntryIsPast;
	wxConfig::Get()->Read(CFG_CAL_PASTENTRY, &val);
	return val;
}


void CAgilityBookOptions::SetDaysTillEntryIsPast(long nDays)
{
	if (0 > nDays)
		nDays = 0;
	wxConfig::Get()->Write(CFG_CAL_PASTENTRY, nDays);
}


bool CAgilityBookOptions::HideOverlappingCalendarEntries()
{
	bool val = sc_HideOverlapping;
	wxConfig::Get()->Read(CFG_CAL_HIDEOVERLAPPING, &val);
	return val;
}


void CAgilityBookOptions::SetHideOverlappingCalendarEntries(bool bHide)
{
	wxConfig::Get()->Write(CFG_CAL_HIDEOVERLAPPING, bHide);
}


static wchar_t const* CalItemName(CAgilityBookOptions::CalendarColorItem inItem)
{
	switch (inItem)
	{
	case CAgilityBookOptions::eCalColorPast:
		return CFG_CAL_ITEM_PASTCOLOR;
	case CAgilityBookOptions::eCalColorNotEntered:
		return CFG_CAL_ITEM_NOTENTEREDCOLOR;
	case CAgilityBookOptions::eCalColorPlanning:
		return CFG_CAL_ITEM_PLANNINGCOLOR;
	case CAgilityBookOptions::eCalColorPending:
		return CFG_CAL_ITEM_PENDINGCOLOR;
	case CAgilityBookOptions::eCalColorEntered:
		return CFG_CAL_ITEM_ENTEREDCOLOR;
	case CAgilityBookOptions::eCalColorOpening:
		return CFG_CAL_ITEM_OPENCOLOR;
	case CAgilityBookOptions::eCalColorClosing:
		return CFG_CAL_ITEM_CLOSECOLOR;
	}
	assert(0);
	return L"";
}


static wxColour CalItemColor(CAgilityBookOptions::CalendarColorItem inItem)
{
	switch (inItem)
	{
	case CAgilityBookOptions::eCalColorPast:
		return sc_CalColorPast;
	case CAgilityBookOptions::eCalColorNotEntered:
		return sc_CalColorNotEntered;
	case CAgilityBookOptions::eCalColorPlanning:
		return sc_CalColorPlanning;
	case CAgilityBookOptions::eCalColorPending:
		return sc_CalColorPending;
	case CAgilityBookOptions::eCalColorEntered:
		return sc_CalColorEntered;
	case CAgilityBookOptions::eCalColorOpening:
		return sc_CalColorOpening;
	case CAgilityBookOptions::eCalColorClosing:
		return sc_CalColorClosing;
	}
	assert(0);
	return wxColour(0,0,0);
}


// Reading/writing is abstracted since a WXCOLORREF is vastly different
// on a Mac (uchar[6] vs ulong). Which means trying to use it directly
// with wxConfig makes for a very unhappy compiler.
static wxColour ReadColor(wxString const& key, wxColour const& defColor)
{
	unsigned long val = defColor.Red();
	val |= (defColor.Green() << 8);
	val |= (defColor.Blue() << 16);
	val = wxConfig::Get()->Read(key, val);
	return wxColour(val);
}


static void WriteColor(wxString const& key, wxColour const& inColor)
{
	unsigned long val = inColor.Red();
	val |= (inColor.Green() << 8);
	val |= (inColor.Blue() << 16);
	wxConfig::Get()->Write(key, static_cast<long>(val));
}


wxColour CAgilityBookOptions::CalendarColor(CalendarColorItem inItem)
{
	wxString key(CFG_KEY_CALENDAR);
	key += CalItemName(inItem);
	return ReadColor(key, CalItemColor(inItem));
}


void CAgilityBookOptions::SetCalendarColor(CalendarColorItem inItem, wxColour inColor)
{
	wxString key(CFG_KEY_CALENDAR);
	key += CalItemName(inItem);
	WriteColor(key, inColor);
}


long CAgilityBookOptions::CalendarOpeningNear()
{
	long val = sc_CalOpeningNear;
	wxConfig::Get()->Read(CFG_CAL_OPENNEAR, &val);
	return val;
}


void CAgilityBookOptions::SetCalendarOpeningNear(long inDays)
{
	wxConfig::Get()->Write(CFG_CAL_OPENNEAR, inDays);
}


long CAgilityBookOptions::CalendarClosingNear()
{
	long val = sc_CalClosingNear;
	wxConfig::Get()->Read(CFG_CAL_CLOSENEAR, &val);
	return val;
}


void CAgilityBookOptions::SetCalendarClosingNear(long inDays)
{
	wxConfig::Get()->Write(CFG_CAL_CLOSENEAR, inDays);
}


wxColour CAgilityBookOptions::CalendarOpeningNearColor()
{
	return ReadColor(CFG_CAL_OPENNEARCOLOR, sc_CalColorOpeningNear);
}


void CAgilityBookOptions::SetCalendarOpeningNearColor(wxColour inColor)
{
	WriteColor(CFG_CAL_OPENNEARCOLOR, inColor);
}


wxColour CAgilityBookOptions::CalendarClosingNearColor()
{
	return ReadColor(CFG_CAL_CLOSENEARCOLOR, sc_CalColorClosingNear);
}


void CAgilityBookOptions::SetCalendarClosingNearColor(wxColour inColor)
{
	WriteColor(CFG_CAL_CLOSENEARCOLOR, inColor);
}

/////////////////////////////////////////////////////////////////////////////
// Common options

ARBDate::DayOfWeek CAgilityBookOptions::GetFirstDayOfWeek()
{
	long val = static_cast<long>(sc_FirstDayOfWeek);
	wxConfig::Get()->Read(CFG_COMMON_FIRSTDAYOFWEEK, &val);
	if (val < 0 || val > 6)
		val = static_cast<long>(ARBDate::eSunday);
	return static_cast<ARBDate::DayOfWeek>(val);
}


void CAgilityBookOptions::SetFirstDayOfWeek(ARBDate::DayOfWeek day)
{
	wxConfig::Get()->Write(CFG_COMMON_FIRSTDAYOFWEEK, static_cast<long>(day));
}

/////////////////////////////////////////////////////////////////////////////
// Runs/points options

bool CAgilityBookOptions::GetViewRunsByTrial()
{
	bool val = sc_ViewRunsByTrial;
	wxConfig::Get()->Read(CFG_COMMON_VIEWRUNSBYTRIAL, &val);
	return val;
}


void CAgilityBookOptions::SetViewRunsByTrial(bool bView)
{
	wxConfig::Get()->Write(CFG_COMMON_VIEWRUNSBYTRIAL,bView);
}


bool CAgilityBookOptions::GetNewestDatesFirst()
{
	bool val = sc_NewestDatesFirst;
	wxConfig::Get()->Read(CFG_COMMON_VIEWNEWESTFIRST, &val);
	return val;
}


void CAgilityBookOptions::SetNewestDatesFirst(bool bNewest)
{
	wxConfig::Get()->Write(CFG_COMMON_VIEWNEWESTFIRST, bNewest);
}


void CAgilityBookOptions::GetPointsViewSort(
		PointsViewSort& outPrimary,
		PointsViewSort& outSecondary,
		PointsViewSort& outTertiary)
{
	outPrimary = static_cast<PointsViewSort>(wxConfig::Get()->Read(CFG_COMMON_SORTPTVW1, static_cast<long>(sc_PointsViewSort1)));
	outSecondary = static_cast<PointsViewSort>(wxConfig::Get()->Read(CFG_COMMON_SORTPTVW2, static_cast<long>(sc_PointsViewSort2)));
	outTertiary = static_cast<PointsViewSort>(wxConfig::Get()->Read(CFG_COMMON_SORTPTVW3, static_cast<long>(sc_PointsViewSort3)));
}


void CAgilityBookOptions::SetPointsViewSort(
		PointsViewSort inPrimary,
		PointsViewSort inSecondary,
		PointsViewSort inTertiary)
{
	wxConfig::Get()->Write(CFG_COMMON_SORTPTVW1, static_cast<long>(inPrimary));
	wxConfig::Get()->Write(CFG_COMMON_SORTPTVW2, static_cast<long>(inSecondary));
	wxConfig::Get()->Write(CFG_COMMON_SORTPTVW3, static_cast<long>(inTertiary));
}


bool CAgilityBookOptions::GetViewHiddenTitles()
{
	bool val = sc_ViewHiddenTitles;
	wxConfig::Get()->Read(CFG_COMMON_VIEWHIDDENTITLES, &val);
	return val;
}


void CAgilityBookOptions::SetViewHiddenTitles(bool bSet)
{
	wxConfig::Get()->Write(CFG_COMMON_VIEWHIDDENTITLES, bSet);
}


bool CAgilityBookOptions::GetViewLifetimePointsByEvent()
{
	bool val = sc_ViewLifetimePointsByEvent;
	wxConfig::Get()->Read(CFG_COMMON_VIEWLIFETIMEEVENTS, &val);
	return val;
}


void CAgilityBookOptions::SetViewLifetimePointsByEvent(bool bSet)
{
	wxConfig::Get()->Write(CFG_COMMON_VIEWLIFETIMEEVENTS, bSet);
}


bool CAgilityBookOptions::GetTableInYPS()
{
	bool val = sc_TableInYPS;
	wxConfig::Get()->Read(CFG_COMMON_TABLEINYPS, &val);
	return val;
}


void CAgilityBookOptions::SetTableInYPS(bool bSet)
{
	wxConfig::Get()->Write(CFG_COMMON_TABLEINYPS, bSet);
}


bool CAgilityBookOptions::GetRunTimeInOPS()
{
	bool val = sc_RunTimeInOPS;
	wxConfig::Get()->Read(CFG_COMMON_RUNTIMEINOPS, &val);
	return val;
}


void CAgilityBookOptions::SetRunTimeInOPS(bool bSet)
{
	wxConfig::Get()->Write(CFG_COMMON_RUNTIMEINOPS, bSet);
}


bool CAgilityBookOptions::GetIncludeCRCDImage()
{
	bool val = sc_IncludeCRCDImage;
	wxConfig::Get()->Read(CFG_COMMON_CRCDIMAGE, &val);
	return val;
}


void CAgilityBookOptions::SetIncludeCRCDImage(bool bSet)
{
	wxConfig::Get()->Write(CFG_COMMON_CRCDIMAGE, bSet);
}

/////////////////////////////////////////////////////////////////////////////
// Font options

// Note: Fonts are factored by 10 for backwards compatibility with ARBv1

void CAgilityBookOptions::GetPrinterFontInfo(CFontInfo& info)
{
	info.name = sc_FontPrinterName;
	info.size = sc_FontPrinterSize * 10;
	info.italic = sc_FontPrinterItalic;
	info.bold = sc_FontPrinterBold;
	info.name = StringUtil::stringW(wxConfig::Get()->Read(CFG_COMMON_PRINTFONTLISTNAME, info.name.c_str()));
	wxConfig::Get()->Read(CFG_COMMON_PRINTFONTLISTSIZE, &info.size, info.size);
	wxConfig::Get()->Read(CFG_COMMON_PRINTFONTLISTITALIC, &info.italic, info.italic);
	wxConfig::Get()->Read(CFG_COMMON_PRINTFONTLISTBOLD, &info.bold, info.bold);
	info.size /= 10;
}


void CAgilityBookOptions::SetPrinterFontInfo(CFontInfo const& info)
{
	wxConfig::Get()->Write(CFG_COMMON_PRINTFONTLISTNAME, info.name.c_str());
	wxConfig::Get()->Write(CFG_COMMON_PRINTFONTLISTSIZE, info.size * 10);
	wxConfig::Get()->Write(CFG_COMMON_PRINTFONTLISTITALIC, info.italic);
	wxConfig::Get()->Write(CFG_COMMON_PRINTFONTLISTBOLD, info.bold);
}


bool CAgilityBookOptions::GetUnitsAsMM()
{
	bool bAsMM = sc_UnitsAsMM;
	wxConfig::Get()->Read(CFG_COMMON_UNITSASMM, &bAsMM);
	return bAsMM;
}


void CAgilityBookOptions::SetUnitsAsMM(bool bAsMM)
{
	wxConfig::Get()->Write(CFG_COMMON_UNITSASMM, bAsMM);
}


static long ConvertMetric(long val, bool bToMM)
{
	if (bToMM)
		return static_cast<long>(static_cast<double>(val) * 0.254);
	else
		return static_cast<long>(static_cast<double>(val) / 0.254);
}


void CAgilityBookOptions::GetPrinterMargins(
		bool bAsMM, // In .01in or millimeters, ignored it DC is set
		long& outLeft,
		long& outRight,
		long& outTop,
		long& outBottom,
		wxDC* pDC)
{
	bool bInMM = sc_MarginAsMM;
	wxConfig::Get()->Read(CFG_COMMON_MARGINS_MM, &bInMM);
	outLeft = wxConfig::Get()->Read(CFG_COMMON_MARGINS_L, sc_MarginLeft);
	outRight = wxConfig::Get()->Read(CFG_COMMON_MARGINS_R, sc_MarginRight);
	outTop = wxConfig::Get()->Read(CFG_COMMON_MARGINS_T, sc_MarginTop);
	outBottom = wxConfig::Get()->Read(CFG_COMMON_MARGINS_B, sc_MarginBottom);
	// Need to convert
	if (bInMM != bAsMM || (pDC && bInMM))
	{
		// Convert to inches
		if (bInMM)
		{
			outLeft = ConvertMetric(outLeft, false);
			outRight = ConvertMetric(outRight, false);
			outTop = ConvertMetric(outTop, false);
			outBottom = ConvertMetric(outBottom, false);
		}
		// Convert to MM
		else
		{
			outLeft = ConvertMetric(outLeft, true);
			outRight = ConvertMetric(outRight, true);
			outTop = ConvertMetric(outTop, true);
			outBottom = ConvertMetric(outBottom, true);
		}
	}
	if (pDC)
	{
		// Convert to logical
		wxSize szPPI = pDC->GetPPI();
		outLeft = pDC->DeviceToLogicalXRel(static_cast<wxCoord>(outLeft / 100.0 * szPPI.x));
		outRight = pDC->DeviceToLogicalXRel(static_cast<wxCoord>(outRight / 100.0 * szPPI.x));
		outTop = pDC->DeviceToLogicalYRel(static_cast<wxCoord>(outTop / 100.0 * szPPI.y));
		outBottom = pDC->DeviceToLogicalYRel(static_cast<wxCoord>(outBottom / 100.0 * szPPI.y));
	}
}


void CAgilityBookOptions::SetPrinterMargins(
		bool bAsMM, // In .01 inches or millimeters
		long inLeft,
		long inRight,
		long inTop,
		long inBottom)
{
	wxConfig::Get()->Write(CFG_COMMON_MARGINS_MM, bAsMM);
	wxConfig::Get()->Write(CFG_COMMON_MARGINS_L, inLeft);
	wxConfig::Get()->Write(CFG_COMMON_MARGINS_R, inRight);
	wxConfig::Get()->Write(CFG_COMMON_MARGINS_T, inTop);
	wxConfig::Get()->Write(CFG_COMMON_MARGINS_B, inBottom);
}


void CAgilityBookOptions::GetRunPageSize(
		bool bAsMM,
		long& outWidth,
		long& outHeight,
		wxDC* pDC)
{
	bool bInMM = sc_RunPageAsMM;
	wxConfig::Get()->Read(CFG_COMMON_RUNPAGE_MM, &bInMM);
	outWidth = wxConfig::Get()->Read(CFG_COMMON_RUNPAGE_W, sc_RunPageWidth);
	outHeight = wxConfig::Get()->Read(CFG_COMMON_RUNPAGE_H, sc_RunPageHeight);
	if (bInMM != bAsMM || (pDC && bInMM))
	{
		if (bInMM)
		{
			outWidth = ConvertMetric(outWidth, false);
			outHeight = ConvertMetric(outHeight, false);
		}
		else
		{
			outWidth = ConvertMetric(outWidth, true);
			outHeight = ConvertMetric(outHeight, true);
		}
	}
	if (pDC)
	{
		// Convert to logical
		wxSize szPPI = pDC->GetPPI();
		outWidth = pDC->DeviceToLogicalXRel(static_cast<wxCoord>(outWidth / 100.0 * szPPI.x));
		outHeight = pDC->DeviceToLogicalXRel(static_cast<wxCoord>(outHeight / 100.0 * szPPI.x));
	}
}


void CAgilityBookOptions::SetRunPageSize(
		bool bAsMM,
		long inWidth,
		long inHeight)
{
	wxConfig::Get()->Write(CFG_COMMON_RUNPAGE_MM, bAsMM);
	wxConfig::Get()->Write(CFG_COMMON_RUNPAGE_W, inWidth);
	wxConfig::Get()->Write(CFG_COMMON_RUNPAGE_H, inHeight);
}


void CAgilityBookOptions::GetCalendarFontInfo(CFontInfo& info)
{
	info.name = sc_FontCalName;
	info.size = sc_FontCalSize * 10;
	info.italic = sc_FontCalItalic;
	info.bold = sc_FontCalBold;
	info.name = StringUtil::stringW(wxConfig::Get()->Read(CFG_CAL_FONTTEXTNAME, info.name.c_str()));
	wxConfig::Get()->Read(CFG_CAL_FONTTEXTSIZE, &info.size, info.size);
	wxConfig::Get()->Read(CFG_CAL_FONTTEXTITALIC, &info.italic, info.italic);
	wxConfig::Get()->Read(CFG_CAL_FONTTEXTBOLD, &info.bold, info.bold);
	info.size /= 10;
}


void CAgilityBookOptions::SetCalendarFontInfo(CFontInfo const& info)
{
	wxConfig::Get()->Write(CFG_CAL_FONTTEXTNAME, info.name.c_str());
	wxConfig::Get()->Write(CFG_CAL_FONTTEXTSIZE, info.size * 10);
	wxConfig::Get()->Write(CFG_CAL_FONTTEXTITALIC, info.italic);
	wxConfig::Get()->Write(CFG_CAL_FONTTEXTBOLD, info.bold);
}

/////////////////////////////////////////////////////////////////////////////
// Last entered options

static wxString GetLastKey(
		ARBDogPtr pDog,
		ARBConfigVenuePtr pVenue,
		wchar_t const* keyGroup,
		wchar_t const* keyDefault,
		bool bFallback)
{
	assert(keyGroup && keyDefault);
	wxString key(keyDefault);
	if (pDog && !pDog->GetCallName().empty())
	{
		// The 'Clean' routines below have "knowledge" of
		//  <key>/<dog>/<venue> order.
		key = keyGroup;
		key << L"/" << pDog->GetCallName().c_str();
		if (pVenue && !pVenue->GetName().empty())
		{
			key << L"/" << pVenue->GetName().c_str();
		}
		// Fall back
		if (bFallback && !wxConfig::Get()->Exists(key))
			key = keyDefault;
	}
	return key;
}


static std::wstring GetLastValue(
		ARBDogPtr pDog,
		ARBConfigVenuePtr pVenue,
		wchar_t const* keyDog,
		wchar_t const* keyDefault)
{
	wxString key = GetLastKey(pDog, pVenue, keyDog, keyDefault, true);
	return StringUtil::stringW(wxConfig::Get()->Read(key, wxString()));
}


static void WriteLastValue(
		ARBDogPtr pDog,
		ARBConfigVenuePtr pVenue,
		wchar_t const* keyDog,
		wchar_t const* keyDefault,
		wchar_t const* inLast)
{
	wxString key = GetLastKey(pDog, pVenue, keyDog, keyDefault, false);
	if (inLast)
	{
		wxConfig::Get()->Write(key, inLast);
		if (key != keyDefault)
			wxConfig::Get()->Write(keyDefault, inLast);
	}
	else
		wxConfig::Get()->DeleteEntry(key);
}


std::wstring CAgilityBookOptions::GetLastEnteredDivision(
		ARBDogPtr pDog,
		ARBConfigVenuePtr pVenue)
{
	return GetLastValue(pDog, pVenue, CFG_LAST_DIVISION_KEY, CFG_LAST_DIVISION);
}


void CAgilityBookOptions::SetLastEnteredDivision(
		ARBDogPtr pDog,
		ARBConfigVenuePtr pVenue,
		wchar_t const* inLast)
{
	WriteLastValue(pDog, pVenue, CFG_LAST_DIVISION_KEY, CFG_LAST_DIVISION, inLast);
}


std::wstring CAgilityBookOptions::GetLastEnteredLevel(
		ARBDogPtr pDog,
		ARBConfigVenuePtr pVenue)
{
	return GetLastValue(pDog, pVenue, CFG_LAST_LEVEL_KEY, CFG_LAST_LEVEL);
}


void CAgilityBookOptions::SetLastEnteredLevel(
		ARBDogPtr pDog,
		ARBConfigVenuePtr pVenue,
		wchar_t const* inLast)
{
	WriteLastValue(pDog, pVenue, CFG_LAST_LEVEL_KEY, CFG_LAST_LEVEL, inLast);
}


std::wstring CAgilityBookOptions::GetLastEnteredHeight(
		ARBDogPtr pDog,
		ARBConfigVenuePtr pVenue)
{
	return GetLastValue(pDog, pVenue, CFG_LAST_HEIGHT_KEY, CFG_LAST_HEIGHT);
}


void CAgilityBookOptions::SetLastEnteredHeight(
		ARBDogPtr pDog,
		ARBConfigVenuePtr pVenue,
		wchar_t const* inLast)
{
	WriteLastValue(pDog, pVenue, CFG_LAST_HEIGHT_KEY, CFG_LAST_HEIGHT, inLast);
}


std::wstring CAgilityBookOptions::GetLastEnteredRefHeight()
{
	return StringUtil::stringW(wxConfig::Get()->Read(CFG_LAST_REFHEIGHT, wxString()));
}


void CAgilityBookOptions::SetLastEnteredRefHeight(wchar_t const* inLast)
{
	if (inLast)
		wxConfig::Get()->Write(CFG_LAST_REFHEIGHT, inLast);
	else
		wxConfig::Get()->DeleteEntry(CFG_LAST_REFHEIGHT);
}


std::wstring CAgilityBookOptions::GetLastEnteredJudge()
{
	return StringUtil::stringW(wxConfig::Get()->Read(CFG_LAST_JUDGE, wxString()));
}


void CAgilityBookOptions::SetLastEnteredJudge(wchar_t const* inLast)
{
	if (inLast)
		wxConfig::Get()->Write(CFG_LAST_JUDGE, inLast);
	else
		wxConfig::Get()->DeleteEntry(CFG_LAST_JUDGE);
}


std::wstring CAgilityBookOptions::GetLastEnteredHandler(ARBDogPtr pDog)
{
	return GetLastValue(pDog, ARBConfigVenuePtr(), CFG_LAST_HANDLER_KEY, CFG_LAST_HANDLER);
}


void CAgilityBookOptions::SetLastEnteredHandler(
		ARBDogPtr pDog,
		wchar_t const* inLast)
{
	WriteLastValue(pDog, ARBConfigVenuePtr(), CFG_LAST_HANDLER_KEY, CFG_LAST_HANDLER, inLast);
}


// Note: If name changes, I don't care. Just cleanse.
// Also, make sure this is updated if any of the 'Last' formats change.

static const struct
{
	wchar_t const* key;
	wchar_t const* def;
	bool hasVenueKey;
} sc_Keys[] = {
	{CFG_LAST_DIVISION_KEY, CFG_LAST_DIVISION, true},
	{CFG_LAST_LEVEL_KEY, CFG_LAST_LEVEL, true},
	{CFG_LAST_HEIGHT_KEY, CFG_LAST_HEIGHT, true},
	{CFG_LAST_HANDLER_KEY, CFG_LAST_HANDLER, false},
};
static const size_t sc_KeysCount = ARRAY_SIZE(sc_Keys);


void CAgilityBookOptions::CleanLastItems(std::wstring const& callName)
{
	CleanLastItems(callName, std::wstring());
}


void CAgilityBookOptions::CleanLastItems(std::wstring const& oldCallName, std::wstring const& newCallName)
{
	if (oldCallName.empty())
		return;

	for (size_t i = 0; i < sc_KeysCount; ++i)
	{
		wxString path = wxConfig::Get()->GetPath();
		wxConfig::Get()->SetPath(sc_Keys[i].key);

		if (sc_Keys[i].hasVenueKey)
		{
			if (wxConfig::Get()->HasGroup(oldCallName.c_str()))
			{
				if (newCallName.empty())
					wxConfig::Get()->DeleteGroup(oldCallName.c_str());
				else
					wxConfig::Get()->RenameGroup(oldCallName.c_str(), newCallName.c_str());
			}
		}
		else
		{
			if (wxConfig::Get()->HasEntry(oldCallName.c_str()))
			{
				if (newCallName.empty())
					wxConfig::Get()->DeleteEntry(oldCallName.c_str());
				else
					wxConfig::Get()->RenameEntry(oldCallName.c_str(), newCallName.c_str());
			}
		}

		wxConfig::Get()->SetPath(path);
	}
}


void CAgilityBookOptions::CleanLastItems(ARBConfig const& inConfig)
{
	for (size_t i = 0; i < sc_KeysCount; ++i)
	{
		if (!sc_Keys[i].hasVenueKey)
			continue;

		wxString key(sc_Keys[i].key);
		if (wxConfig::Get()->HasGroup(key))
		{
			wxString path = wxConfig::Get()->GetPath();
			wxConfig::Get()->SetPath(key);

			wxString strDog;
			long idxDog;
			if (wxConfig::Get()->GetFirstGroup(strDog, idxDog))
			{
				do
				{
					wxConfig::Get()->SetPath(strDog);

					std::vector<wxString> deleteVenues;
					wxString strVenue;
					long idxVenue;
					if (wxConfig::Get()->GetFirstEntry(strVenue, idxVenue))
					{
						do
						{
							if (!inConfig.GetVenues().FindVenue(StringUtil::stringW(strVenue)))
							{
								// Not sure if deleting during iteration might
								// be an issue. So avoiding!
								deleteVenues.push_back(strVenue);
							}
						} while (wxConfig::Get()->GetNextEntry(strVenue, idxVenue));
					}
					for (std::vector<wxString>::iterator iter = deleteVenues.begin();
						iter != deleteVenues.end();
						++iter)
					{
						wxConfig::Get()->DeleteEntry(*iter);
					}

					wxConfig::Get()->SetPath(L"..");
				} while (wxConfig::Get()->GetNextGroup(strDog, idxDog));
			}

			wxConfig::Get()->SetPath(path);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Import/export options

long CAgilityBookOptions::GetImportStartRow()
{
	long row = wxConfig::Get()->Read(CFG_IMPORT_ROW, sc_ImportStartRow);
	if (0 > row)
		row = 1;
	return row;
}


void CAgilityBookOptions::SetImportStartRow(long row)
{
	wxConfig::Get()->Write(CFG_IMPORT_ROW, row);
}


void CAgilityBookOptions::GetImportExportDelimiters(
		bool bImport,
		long& delim,
		std::wstring& delimiter)
{
	wxString section;
	if (bImport)
		section = CFG_KEY_IMPORT;
	else
		section = CFG_KEY_EXPORT;
	delim = sc_ImportExportDelim;
	delimiter.clear();
	delim = wxConfig::Get()->Read(section + CFG_IMPORT_EXPORT_DELIM, delim);
	delimiter = StringUtil::stringW(wxConfig::Get()->Read(section + CFG_IMPORT_EXPORT_DELIMITER, delimiter.c_str()));
	if (1 < delimiter.length())
		delimiter = delimiter.substr(0, 1);
}


void CAgilityBookOptions::SetImportExportDelimiters(
		bool bImport,
		long delim,
		std::wstring const& delimiter)
{
	wxString section;
	if (bImport)
		section = CFG_KEY_IMPORT;
	else
		section = CFG_KEY_EXPORT;
	wxConfig::Get()->Write(section + CFG_IMPORT_EXPORT_DELIM, delim);
	wxConfig::Get()->Write(section + CFG_IMPORT_EXPORT_DELIMITER, delimiter.c_str());
}


void CAgilityBookOptions::GetImportExportDateFormat(
		bool bImport,
		ARBDate::DateFormat& outFormat)
{
	wxString section;
	if (bImport)
		section = CFG_KEY_IMPORT;
	else
		section = CFG_KEY_EXPORT;
	outFormat = static_cast<ARBDate::DateFormat>(wxConfig::Get()->Read(section + CFG_IMPORT_EXPORT_DATEFORMAT, static_cast<long>(sc_ImportExportFormat)));
}


void CAgilityBookOptions::SetImportExportDateFormat(
		bool bImport,
		ARBDate::DateFormat inFormat)
{
	wxString section;
	if (bImport)
		section = CFG_KEY_IMPORT;
	else
		section = CFG_KEY_EXPORT;
	wxConfig::Get()->Write(section + CFG_IMPORT_EXPORT_DATEFORMAT, static_cast<long>(inFormat));
}


static wchar_t const* const GetColumnName(CAgilityBookOptions::ColumnOrder eOrder)
{
	switch (eOrder)
	{
	default:
		return CFG_KEY_UNKNOWN;
	case CAgilityBookOptions::eRunsImport:
		return CFG_KEY_IMPORT;
	case CAgilityBookOptions::eRunsExport:
		return CFG_KEY_EXPORT;
	case CAgilityBookOptions::eCalImport:
		return CFG_KEY_IMPORTCAL;
	case CAgilityBookOptions::eCalExport:
		return CFG_KEY_EXPORTCAL;
	case CAgilityBookOptions::eCalExportAppt:
		return CFG_KEY_EXPORTCALAPPT;
	case CAgilityBookOptions::eCalExportTask:
		return CFG_KEY_EXPORTCALTASK;
	case CAgilityBookOptions::eLogImport:
		return CFG_KEY_IMPORTLOG;
	case CAgilityBookOptions::eLogExport:
		return CFG_KEY_EXPORTLOG;
	case CAgilityBookOptions::eView:
		return CFG_KEY_COLUMNS;
	}
}


void CAgilityBookOptions::GetColumnOrder(
		ColumnOrder eOrder,
		size_t idxColumn,
		std::wstring const& namedColumn,
		std::vector<long>& outValues,
		bool bDefaultValues)
{
	outValues.clear();
	if (!bDefaultValues)
	{
		wxString item;
		if (!namedColumn.empty())
			item << L'/' << namedColumn.c_str();
		item << GetColumnName(eOrder) << CFG_COL_BASENAME(idxColumn);
		std::wstring data = StringUtil::stringW(wxConfig::Get()->Read(item, wxEmptyString));
		std::wstring::size_type idx = data.find(',');
		while (std::wstring::npos != idx)
		{
			long val;
			StringUtil::ToCLong(data, val, true);
			outValues.push_back(val);
			data = data.substr(idx+1);
			idx = data.find(',');
		}
		if (0 < data.length())
		{
			long val;
			StringUtil::ToCLong(data, val, true);
			outValues.push_back(val);
		}
	}
	if (0 == outValues.size())
	{
		switch (eOrder)
		{
		default:
			break;
		case eRunsImport:
		case eRunsExport:
			switch (idxColumn)
			{
			case IO_TYPE_RUNS_FAULTS_TIME:
				outValues.push_back(IO_RUNS_DATE);
				outValues.push_back(IO_RUNS_LOCATION);
				outValues.push_back(IO_RUNS_VENUE);
				outValues.push_back(IO_RUNS_CLUB);
				outValues.push_back(IO_RUNS_DIVISION);
				outValues.push_back(IO_RUNS_LEVEL);
				outValues.push_back(IO_RUNS_EVENT);
				outValues.push_back(IO_RUNS_JUDGE);
				outValues.push_back(IO_RUNS_COURSE_FAULTS);
				outValues.push_back(IO_RUNS_TIME);
				outValues.push_back(IO_RUNS_YARDS);
				outValues.push_back(IO_RUNS_YPS);
				outValues.push_back(IO_RUNS_SCT);
				outValues.push_back(IO_RUNS_TOTAL_FAULTS);
				outValues.push_back(IO_RUNS_SCORE);
				outValues.push_back(IO_RUNS_TITLE_POINTS);
				outValues.push_back(IO_RUNS_PLACE);
				outValues.push_back(IO_RUNS_IN_CLASS);
				outValues.push_back(IO_RUNS_DOGSQD);
				outValues.push_back(IO_RUNS_Q);
				break;

			case IO_TYPE_RUNS_TIME_FAULTS:
				outValues.push_back(IO_RUNS_DATE);
				outValues.push_back(IO_RUNS_LOCATION);
				outValues.push_back(IO_RUNS_VENUE);
				outValues.push_back(IO_RUNS_CLUB);
				outValues.push_back(IO_RUNS_DIVISION);
				outValues.push_back(IO_RUNS_LEVEL);
				outValues.push_back(IO_RUNS_EVENT);
				outValues.push_back(IO_RUNS_JUDGE);
				outValues.push_back(IO_RUNS_COURSE_FAULTS);
				outValues.push_back(IO_RUNS_TIME);
				outValues.push_back(IO_RUNS_YARDS);
				outValues.push_back(IO_RUNS_YPS);
				outValues.push_back(IO_RUNS_SCT);
				outValues.push_back(IO_RUNS_TOTAL_FAULTS);
				outValues.push_back(IO_RUNS_SCORE);
				outValues.push_back(IO_RUNS_TITLE_POINTS);
				outValues.push_back(IO_RUNS_PLACE);
				outValues.push_back(IO_RUNS_IN_CLASS);
				outValues.push_back(IO_RUNS_DOGSQD);
				outValues.push_back(IO_RUNS_Q);
				break;

			case IO_TYPE_RUNS_OPEN_CLOSE:
				outValues.push_back(IO_RUNS_DATE);
				outValues.push_back(IO_RUNS_LOCATION);
				outValues.push_back(IO_RUNS_VENUE);
				outValues.push_back(IO_RUNS_CLUB);
				outValues.push_back(IO_RUNS_DIVISION);
				outValues.push_back(IO_RUNS_LEVEL);
				outValues.push_back(IO_RUNS_EVENT);
				outValues.push_back(IO_RUNS_JUDGE);
				outValues.push_back(IO_RUNS_COURSE_FAULTS);
				outValues.push_back(IO_RUNS_TIME);
				outValues.push_back(IO_RUNS_REQ_OPENING);
				outValues.push_back(IO_RUNS_REQ_CLOSING);
				outValues.push_back(IO_RUNS_OPENING);
				outValues.push_back(IO_RUNS_CLOSING);
				outValues.push_back(IO_RUNS_SCORE);
				outValues.push_back(IO_RUNS_TITLE_POINTS);
				outValues.push_back(IO_RUNS_PLACE);
				outValues.push_back(IO_RUNS_IN_CLASS);
				outValues.push_back(IO_RUNS_DOGSQD);
				outValues.push_back(IO_RUNS_Q);
				break;

			case IO_TYPE_RUNS_POINTS:
				outValues.push_back(IO_RUNS_DATE);
				outValues.push_back(IO_RUNS_LOCATION);
				outValues.push_back(IO_RUNS_VENUE);
				outValues.push_back(IO_RUNS_CLUB);
				outValues.push_back(IO_RUNS_DIVISION);
				outValues.push_back(IO_RUNS_LEVEL);
				outValues.push_back(IO_RUNS_EVENT);
				outValues.push_back(IO_RUNS_JUDGE);
				outValues.push_back(IO_RUNS_COURSE_FAULTS);
				outValues.push_back(IO_RUNS_TIME);
				outValues.push_back(IO_RUNS_REQ_POINTS);
				outValues.push_back(-1);
				outValues.push_back(IO_RUNS_POINTS);
				outValues.push_back(-1);
				outValues.push_back(IO_RUNS_SCORE);
				outValues.push_back(IO_RUNS_TITLE_POINTS);
				outValues.push_back(IO_RUNS_PLACE);
				outValues.push_back(IO_RUNS_IN_CLASS);
				outValues.push_back(IO_RUNS_DOGSQD);
				outValues.push_back(IO_RUNS_Q);
				break;
			}
			break;
		case eCalImport:
		case eCalExport:
			if (IO_TYPE_CALENDAR == idxColumn)
			{
				outValues.push_back(IO_CAL_START_DATE);
				outValues.push_back(IO_CAL_END_DATE);
				outValues.push_back(IO_CAL_TENTATIVE);
				outValues.push_back(IO_CAL_ENTERED);
				outValues.push_back(IO_CAL_VENUE);
				outValues.push_back(IO_CAL_LOCATION);
				outValues.push_back(IO_CAL_CLUB);
				outValues.push_back(IO_CAL_OPENS);
				outValues.push_back(IO_CAL_CLOSES);
				outValues.push_back(IO_CAL_NOTES);
			}
			break;
		case eCalExportAppt:
			if (IO_TYPE_CALENDAR_APPT == idxColumn)
			{
				outValues.push_back(IO_CAL_APPT_SUBJECT);
				outValues.push_back(IO_CAL_APPT_START_DATE);
				outValues.push_back(IO_CAL_APPT_START_TIME);
				outValues.push_back(IO_CAL_APPT_END_DATE);
				outValues.push_back(IO_CAL_APPT_END_TIME);
				outValues.push_back(IO_CAL_APPT_ALLDAY);
				outValues.push_back(IO_CAL_APPT_REMINDER);
				outValues.push_back(IO_CAL_APPT_REMINDER_DATE);
				outValues.push_back(IO_CAL_APPT_REMINDER_TIME);
				outValues.push_back(IO_CAL_APPT_ORGANIZER);
				outValues.push_back(IO_CAL_APPT_REQ_ATTENDEES);
				outValues.push_back(IO_CAL_APPT_OPT_ATTENDEES);
				outValues.push_back(IO_CAL_APPT_RESOURCES);
				outValues.push_back(IO_CAL_APPT_BILLING);
				outValues.push_back(IO_CAL_APPT_CATEGORIES);
				outValues.push_back(IO_CAL_APPT_DESCRIPTION);
				outValues.push_back(IO_CAL_APPT_LOCATION);
				outValues.push_back(IO_CAL_APPT_MILEAGE);
				outValues.push_back(IO_CAL_APPT_PRIORITY);
				outValues.push_back(IO_CAL_APPT_PRIVATE);
				outValues.push_back(IO_CAL_APPT_SENSITIVITY);
				outValues.push_back(IO_CAL_APPT_SHOW_TIME_AS);
			}
			break;
		case eCalExportTask:
			if (IO_TYPE_CALENDAR_TASK == idxColumn)
			{
				outValues.push_back(IO_CAL_TASK_SUBJECT);
				outValues.push_back(IO_CAL_TASK_START_DATE);
				outValues.push_back(IO_CAL_TASK_DUE_DATE);
				outValues.push_back(IO_CAL_TASK_REMINDER);
				outValues.push_back(IO_CAL_TASK_REMINDER_DATE);
				outValues.push_back(IO_CAL_TASK_REMINDER_TIME);
				outValues.push_back(IO_CAL_TASK_COMPLETED_DATE);
				outValues.push_back(IO_CAL_TASK_COMPLETE);
				outValues.push_back(IO_CAL_TASK_TOTAL_WORK);
				outValues.push_back(IO_CAL_TASK_ACTUAL_WORK);
				outValues.push_back(IO_CAL_TASK_BILLING);
				outValues.push_back(IO_CAL_TASK_CATEGORIES);
				outValues.push_back(IO_CAL_TASK_COMPANIES);
				outValues.push_back(IO_CAL_TASK_CONTACTS);
				outValues.push_back(IO_CAL_TASK_MILEAGE);
				outValues.push_back(IO_CAL_TASK_NOTES);
				outValues.push_back(IO_CAL_TASK_PRIORITY);
				outValues.push_back(IO_CAL_TASK_PRIVATE);
				outValues.push_back(IO_CAL_TASK_ROLE);
				outValues.push_back(IO_CAL_TASK_SCH_PRIORITY);
				outValues.push_back(IO_CAL_TASK_SENSITIVITY);
				outValues.push_back(IO_CAL_TASK_STATUS);
			}
			break;
		case eLogImport:
		case eLogExport:
			if (IO_TYPE_TRAINING == idxColumn)
			{
				outValues.push_back(IO_LOG_DATE);
				outValues.push_back(IO_LOG_NAME);
				outValues.push_back(IO_LOG_SUBNAME);
				outValues.push_back(IO_LOG_NOTES);
			}
			break;
		case eView:
			switch (idxColumn)
			{
			case IO_TYPE_VIEW_TREE_DOG:
				outValues.push_back(IO_TREE_DOG_CALLNAME);
				outValues.push_back(IO_TREE_DOG_AGE);
				break;
			case IO_TYPE_VIEW_TREE_TRIAL:
				outValues.push_back(IO_TREE_TRIAL_START);
#ifndef __WXMSW__
				outValues.push_back(IO_TREE_TRIAL_VERIFIED);
#endif
				outValues.push_back(IO_TREE_TRIAL_VENUE);
				outValues.push_back(IO_TREE_TRIAL_LOCATION);
				break;
			case IO_TYPE_VIEW_TREE_RUN:
				outValues.push_back(IO_TREE_RUN_DATE);
				outValues.push_back(IO_TREE_RUN_EVENT);
				break;
			case IO_TYPE_VIEW_RUNS_LIST:
				outValues.push_back(IO_RUNS_Q);
				outValues.push_back(IO_RUNS_TITLE_POINTS);
				outValues.push_back(IO_RUNS_SCORE);
				outValues.push_back(IO_RUNS_DATE);
				outValues.push_back(IO_RUNS_VENUE);
				outValues.push_back(IO_RUNS_EVENT);
				outValues.push_back(IO_RUNS_DIVISION);
				outValues.push_back(IO_RUNS_LEVEL);
				outValues.push_back(IO_RUNS_HEIGHT);
				outValues.push_back(IO_RUNS_JUDGE);
				outValues.push_back(IO_RUNS_TIME);
				outValues.push_back(IO_RUNS_YPS);
				outValues.push_back(IO_RUNS_PLACE);
				outValues.push_back(IO_RUNS_IN_CLASS);
				outValues.push_back(IO_RUNS_DOGSQD);
				outValues.push_back(IO_RUNS_COMMENTS);
				break;
			case IO_TYPE_VIEW_CALENDAR_LIST:
				outValues.push_back(IO_CAL_START_DATE);
				outValues.push_back(IO_CAL_VENUE);
				outValues.push_back(IO_CAL_LOCATION);
				outValues.push_back(IO_CAL_CLOSES);
				outValues.push_back(IO_CAL_CLUB);
				outValues.push_back(IO_CAL_OPENS);
				outValues.push_back(IO_CAL_DRAWS);
				outValues.push_back(IO_CAL_NOTES);
				break;
			case IO_TYPE_VIEW_TRAINING_LIST:
				outValues.push_back(IO_LOG_DATE);
				outValues.push_back(IO_LOG_NAME);
				outValues.push_back(IO_LOG_SUBNAME);
				outValues.push_back(IO_LOG_NOTES);
				break;
			case IO_TYPE_VIEW_CALENDAR:
				outValues.push_back(IO_CAL_VENUE);
				outValues.push_back(IO_CAL_LOCATION);
				break;
			}
			break;
		}
	}
}


void CAgilityBookOptions::SetColumnOrder(
		ColumnOrder eOrder,
		size_t idxColumn,
		std::wstring const& namedColumn,
		std::vector<long> const& inValues)
{
	wxString data;
	for (size_t i = 0; i < inValues.size(); ++i)
	{
		if (0 < i)
			data << L",";
		data << inValues[i];
	}
	wxString item;
	if (!namedColumn.empty())
		item << L'/' << namedColumn.c_str();
	item << GetColumnName(eOrder) << CFG_COL_BASENAME(idxColumn);
	wxConfig::Get()->Write(item, data);
}

/////////////////////////////////////////////////////////////////////////////

long CAgilityBookOptions::GetMRUFileCount()
{
	return wxConfig::Get()->Read(L"Settings/MRUsize", sc_MRUCount);
}


void CAgilityBookOptions::SetMRUFileCount(long nFiles)
{
	wxConfig::Get()->Write(L"Settings/MRUsize", nFiles);
}


bool CAgilityBookOptions::GetAutoUpdateCheck()
{
#ifdef _DEBUG
	return false;
#else
	bool val = sc_AutoUpdateCheck;
	wxConfig::Get()->Read(CFG_SETTINGS_AUTOCHECK, &val);
	return val;
#endif
}


void CAgilityBookOptions::SetAutoUpdateCheck(bool bSet)
{
	wxConfig::Get()->Write(CFG_SETTINGS_AUTOCHECK, bSet);
}


long CAgilityBookOptions::GetNumBackupFiles()
{
	return wxConfig::Get()->Read(CFG_SETTINGS_BACKUPFILES, sc_NumBackup);
}


void CAgilityBookOptions::SetNumBackupFiles(long nFiles)
{
	wxConfig::Get()->Write(CFG_SETTINGS_BACKUPFILES, nFiles);
}


bool CAgilityBookOptions::AutoShowPropertiesOnNewTitle()
{
	bool val = sc_ShowPropOnNewTitle;
	wxConfig::Get()->Read(CFG_SETTINGS_AUTOSHOWTITLE, &val);
	return val;
}


void CAgilityBookOptions::AutoShowPropertiesOnNewTitle(bool bShow)
{
	wxConfig::Get()->Write(CFG_SETTINGS_AUTOSHOWTITLE, bShow);
}


bool CAgilityBookOptions::ShowHtmlPoints()
{
	bool val = sc_ShowHtmlPoints;
	wxConfig::Get()->Read(CFG_SETTINGS_SHOWHTML, &val);
	return val;
}


void CAgilityBookOptions::SetShowHtmlPoints(bool bSet)
{
	wxConfig::Get()->Write(CFG_SETTINGS_SHOWHTML, bSet);
}

/////////////////////////////////////////////////////////////////////////////

std::wstring CAgilityBookOptions::GetUserName(std::wstring const& hint)
{
	wxString section(CFG_KEY_USERNAMES);
	section += hint.c_str();
	return StringUtil::stringW(wxConfig::Get()->Read(section, wxString()));
}


void CAgilityBookOptions::SetUserName(
		std::wstring const& hint,
		std::wstring const& userName)
{
	wxString section(CFG_KEY_USERNAMES);
	section += hint.c_str();
	wxConfig::Get()->Write(section, userName.c_str());
}


// Keeping use and proxy strings separate allow us to turn off the proxy
// without losing the settings.

bool CAgilityBookOptions::GetUseProxy()
{
	bool val = sc_UseProxy;
	wxConfig::Get()->Read(CFG_SETTINGS_USEPROXY, &val);
	return val;
}


void CAgilityBookOptions::SetUseProxy(bool inUseProxy)
{
	wxConfig::Get()->Write(CFG_SETTINGS_USEPROXY, inUseProxy);
}


std::wstring CAgilityBookOptions::GetProxyServer()
{
	return StringUtil::stringW(wxConfig::Get()->Read(CFG_SETTINGS_PROXY, wxString()));
}


void CAgilityBookOptions::SetProxyServer(std::wstring const& inProxy)
{
	wxConfig::Get()->Write(CFG_SETTINGS_PROXY, inProxy.c_str());
}


std::wstring CAgilityBookOptions::GetProxy()
{
	if (CAgilityBookOptions::GetUseProxy())
		return CAgilityBookOptions::GetProxyServer();
	return std::wstring();
}

/////////////////////////////////////////////////////////////////////////////

bool CAgilityBookOptions::IsCalSiteVisible(
		std::wstring const& filename,
		CVersionNum const& inVer)
{
	assert(inVer.Valid());
	if (filename.empty())
		return true;
	bool bVisible = true;
	wxString section(CFG_KEY_CALSITES);
	section += filename.c_str();
	bool bCheckStatus = true;
	wxConfig::Get()->Read(section, &bCheckStatus);
	if (bCheckStatus)
	{
		CVersionNum ver = GetCalSitePermanentStatus(filename);
		if (ver.Valid() && inVer <= ver)
			bVisible = false;
	}
	else
		bVisible = false;
	return bVisible;
}


void CAgilityBookOptions::SuppressCalSite(
		std::wstring const& filename,
		bool bSuppress)
{
	if (filename.empty())
		return;
	wxString section(CFG_KEY_CALSITES);
	section += filename.c_str();
	wxConfig::Get()->Write(section, !bSuppress);
}


CVersionNum CAgilityBookOptions::GetCalSitePermanentStatus(std::wstring const& filename)
{
	CVersionNum ver(false);
	if (!filename.empty())
	{
		wxString section(CFG_KEY_CALSITES2);
		section += filename.c_str();
		std::wstring str = StringUtil::stringW(wxConfig::Get()->Read(section, wxString()));
		if (!str.empty())
			ver.Parse(str);
	}
	return ver;
}


void CAgilityBookOptions::SuppressCalSitePermanently(
		std::wstring const& filename,
		CVersionNum const& inVer,
		bool bSuppress)
{
	if (filename.empty())
		return;
	wxString section(CFG_KEY_CALSITES2);
	section += filename.c_str();
	if (bSuppress)
		wxConfig::Get()->Write(section, inVer.GetVersionString().c_str());
	else
	{
		// If we're clearing one, make sure we haven't written a different version
		CVersionNum ver(false);
		std::wstring str = StringUtil::stringW(wxConfig::Get()->Read(section, wxString()));
		if (!str.empty())
			ver.Parse(str);
		if (ver == inVer)
			wxConfig::Get()->DeleteEntry(section);
	}
}
