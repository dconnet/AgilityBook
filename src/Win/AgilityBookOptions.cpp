/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CAgilityBookOptions class
 * @author David Connet
 *
 * Revision History
 * 2021-06-22 Added AutoUpdateCheckInterval.
 * 2020-01-27 Add alternate row color setting.
 * 2018-12-16 Convert to fmt.
 * 2013-06-15 Make default for GetNewestDatesFirst true.
 * 2013-05-19 Make last div/level/height/handler context aware.
 * 2012-12-23 Changed default Runs view columns.
 * 2012-07-04 Add option to use run time or opening time in gamble OPS.
 * 2010-11-07 Merge filters on program settings import also.
 * 2010-11-04 Change importing program settings to merge columninfo.
 * 2010-03-28 Added ability to import/export program settings (v2.2.0).
 * 2010-01-21 Fixed font flag parsing.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-26 Changed auto update check to false in debug mode.
 * 2009-08-08 Changed default setting for proxy and show dog after title
 * 2009-07-19 Implement proxy support.
 * 2008-12-24 Ported to wxWidgets.
 * 2008-01-14 Make ViewLifetimeEvents default to true.
 * 2007-08-03 Added UserNames
 * 2006-07-16 Added ARBPointsViewSort
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-08-18 Changed how filter options are done.
 * 2005-05-04 Added IncludeCRCDImage
 * 2004-12-18 Added Opening/Closing dates to view, plus color.
 * 2004-08-31 Added AutoShowSplashScreen
 * 2004-06-16 Added options to remember date formats.
 * 2004-06-06 Added additional clipboard formats.
 * 2004-06-01 Venue filtering could allow filtered runs thru at times.
 * 2004-04-08 Added general program options.
 * 2004-03-13 Added GetViewHiddenTitles, updated IsTitleVisible.
 * 2004-01-04 Added GetImportExportDateFormat.
 * 2003-12-30 Implemented customized text in tree.
 * 2003-12-29 Created default column orders.
 * 2003-12-11 Added options for import/export wizard.
 * 2003-10-09 Added *etViewRunsByTrial.
 */

#include "stdafx.h"
#include "AgilityBookOptions.h"

#include "AgilityBook.h"
#include "DlgAssignColumns.h"
#include "RegItems.h"
#include "VersionNumber.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBCalendar.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBDogTitle.h"
#include "ARB/ARBDogTrial.h"
#include "ARBCommon/ARBDate.h"
#include "ARBCommon/ARBUtils.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "ARBCommon/VersionNum.h"
#include <wx/config.h>

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
// Calendar Defaults
constexpr bool sc_AutoDeleteCalEntries = false;
constexpr bool sc_ViewAllCalEntries = true;
constexpr bool sc_ViewAllCalOpening = true;
constexpr bool sc_ViewAllCalClosing = true;
constexpr long sc_DaysTillEntryIsPast = 5;
constexpr bool sc_HideOverlapping = false;

// xcode5 dies on any static wx objects
const wxColour sc_CalColorPast()
{
	return wxColour(128, 128, 128);
} // gray
const wxColour sc_CalColorNotEntered()
{
	return wxColour(0, 0, 0);
} // Black
const wxColour sc_CalColorPlanning()
{
	return wxColour(255, 128, 0);
} // Orange
const wxColour sc_CalColorPending()
{
	return wxColour(128, 0, 255);
} // Blue-ish
const wxColour sc_CalColorEntered()
{
	return wxColour(0, 0, 255);
} // Blue
const wxColour sc_CalColorOpening()
{
	return wxColour(0, 128, 0);
} // Dk Green
const wxColour sc_CalColorClosing()
{
	return wxColour(255, 0, 0);
} // Red
constexpr long sc_CalOpeningNear = 4;
constexpr long sc_CalClosingNear = 10;
const wxColour sc_CalColorOpeningNear()
{
	return wxColour(0, 0, 255);
}
const wxColour sc_CalColorClosingNear()
{
	return wxColour(255, 0, 0);
}

// Common
constexpr ARBDayOfWeek sc_FirstDayOfWeek = ARBDayOfWeek::Sunday;

// Runs/points
constexpr ARBViewRuns sc_ViewRunsStyle = ARBViewRuns::RunsByTrial;
constexpr bool sc_NewestDatesFirst = true;
constexpr ARBPointsViewSort sc_PointsViewSort1 = ARBPointsViewSort::Division;
constexpr ARBPointsViewSort sc_PointsViewSort2 = ARBPointsViewSort::Level;
constexpr ARBPointsViewSort sc_PointsViewSort3 = ARBPointsViewSort::Event;
constexpr bool sc_ViewHiddenTitles = false;
constexpr bool sc_ViewLifetimePointsByEvent = true;
constexpr bool sc_TableInYPS = false;
constexpr bool sc_RunTimeInOPS = false;
constexpr bool sc_IncludeCRCDImage = false;
constexpr bool sc_ShowCoSanctioning = false;

// Font
constexpr wchar_t sc_FontPrinterName[] = L"Times New Roman";
constexpr int sc_FontPrinterSize = 8;
constexpr bool sc_FontPrinterItalic = false;
constexpr bool sc_FontPrinterBold = false;
constexpr bool sc_UnitsAsMM = false;
constexpr bool sc_MarginAsMM = false;
constexpr long sc_MarginLeft = 50;
constexpr long sc_MarginRight = 50;
constexpr long sc_MarginTop = 50;
constexpr long sc_MarginBottom = 50;
constexpr bool sc_RunPageAsMM = false;
constexpr long sc_RunPageWidth = 0;
constexpr long sc_RunPageHeight = 0;
constexpr wchar_t sc_FontCalName[] = L"Times New Roman";
constexpr int sc_FontCalSize = 8;
constexpr bool sc_FontCalItalic = false;
constexpr bool sc_FontCalBold = false;

// Import/export
constexpr long sc_ImportStartRow = 1;
constexpr ARBImportExportDelim sc_ImportExportDelim = ARBImportExportDelim::Tab;
constexpr ARBDateFormat sc_ImportExportFormat = ARBDateFormat::ISO;
// Note: Default column order is defined in GetColumnOrder()

// Program
constexpr long sc_MRUCount = 4;
#if !defined(_DEBUG) && !defined(__WXDEBUG__)
constexpr bool sc_AutoUpdateCheck = true;
#endif
constexpr long sc_AutoUpdateCheckInterval = 30;
constexpr long sc_NumBackup = 3;
constexpr bool sc_ShowPropOnNewTitle = false;
constexpr bool sc_UseProxy = false;
constexpr bool sc_UseAltRowColor = true;


void ExportConfigItem(wxString const& entry, ElementNodePtr const& inTree)
{
	switch (wxConfig::Get()->GetEntryType(entry))
	{
	case wxConfigBase::Type_Unknown:
	default:
		break;
	case wxConfigBase::Type_String:
	{
		ElementNodePtr item = inTree->AddElementNode(StringUtil::stringW(entry));
		item->AddAttrib(L"type", L"s");
		item->SetValue(wxConfig::Get()->Read(entry, wxEmptyString).wx_str());
	}
	break;
	case wxConfigBase::Type_Boolean:
	{
		ElementNodePtr item = inTree->AddElementNode(StringUtil::stringW(entry));
		item->AddAttrib(L"type", L"b");
		bool val;
		wxConfig::Get()->Read(entry, &val);
		item->AddAttrib(L"val", val);
	}
	break;
	case wxConfigBase::Type_Integer:
	{
		ElementNodePtr item = inTree->AddElementNode(StringUtil::stringW(entry));
		item->AddAttrib(L"type", L"i");
		long val;
		wxConfig::Get()->Read(entry, &val);
		item->AddAttrib(L"val", val);
	}
	break;
	case wxConfigBase::Type_Float:
	{
		ElementNodePtr item = inTree->AddElementNode(StringUtil::stringW(entry));
		item->AddAttrib(L"type", L"f");
		double val;
		wxConfig::Get()->Read(entry, &val);
		item->AddAttrib(L"val", val);
	}
	break;
	}
}


void ExportConfig(wxString const& key, ElementNodePtr const& inRoot)
{
	if (!wxConfig::Get()->HasGroup(key))
		return;

	ElementNodePtr tree = inRoot->AddElementNode(StringUtil::stringW(key));
	tree->AddAttrib(L"type", L"g");

	CConfigPathHelper config(key);

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
}


ElementNodePtr FindElementName(
	ElementNodePtr const& inTree,
	long numConfigs,
	std::wstring const& name,
	const wchar_t* const eleItem,
	const wchar_t* const eleName)
{
	for (long i = 0; i < numConfigs; ++i)
	{
		std::wstring configName = fmt::format(L"{}{}", eleItem, i);
		int idxConfig = inTree->FindElement(configName);
		if (0 <= idxConfig && ARBElementType::Node == inTree->GetElement(idxConfig)->GetType())
		{
			ElementNodePtr nodeConfig = inTree->GetElementNode(idxConfig);
			int idxName = nodeConfig->FindElement(L"name");
			if (0 <= idxName && name == nodeConfig->GetElementNode(idxName)->GetValue())
			{
				return nodeConfig;
			}
		}
	}
	return ElementNodePtr();
}


void ImportConfig(ElementNodePtr const& inTree, bool bClobberFilters);


// Return true to invoke clobber code below.
bool ImportColumnInfo(ElementNodePtr const& inTree)
{
	// Only "Config#" are merged (unless we clobber).
	int idxNum = inTree->FindElement(L"numConfigs");
	if (0 > idxNum)
		return false;
	long numConfigs = 0;
	inTree->GetElementNode(idxNum)->GetAttrib(L"val", numConfigs);
	if (0 >= numConfigs)
		return false;

	// If there are no existing configs, just bail and clobber.
	ElementNodePtr root(ElementNode::New(L"Top"));
	{
		CConfigPathHelper config(L"/");
		ExportConfig(CFG_KEY_COLUMNINFO, root);
	}
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
		std::wstring configName = fmt::format(CFG_KEY_CONFIG L"{}", i);
		int idxConfig = inTree->FindElement(configName);
		if (0 <= idxConfig && ARBElementType::Node == inTree->GetElement(idxConfig)->GetType())
		{
			ElementNodePtr nodeConfig = inTree->GetElementNode(idxConfig);
			int idxName = nodeConfig->FindElement(L"name");
			if (0 > idxName)
				continue; // Ignore no-name configs.
			std::wstring name = nodeConfig->GetElementNode(idxName)->GetValue();
			ElementNodePtr existingConfig
				= FindElementName(existing, numExistingConfigs, name, CFG_KEY_CONFIG, L"name");
			if (existingConfig)
			{
				CConfigPathHelper config(StringUtil::stringWX(existingConfig->GetName()));
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
			}
			else
			{
				name = nodeConfig->GetName();
				std::wstring newName = fmt::format(CFG_KEY_CONFIG L"{}", numExistingConfigs + added);
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
bool MergeFilters(ElementNodePtr const& inTree)
{
	int idx = inTree->FindElement(CFG_KEY_COMMON);
	if (0 > idx)
		return false;
	ElementNodePtr nodeCommon = inTree->GetElementNode(idx);
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
	{
		CConfigPathHelper config(L"/");
		for (int i = 0; i < numExistingFilters; ++i)
		{
			ExportConfig(CFG_KEY_FILTER_N(i, false), root);
		}
	}
	if (numExistingFilters != root->GetElementCount())
		return true;

	long added = 0;
	for (long i = 0; i < numFilters; ++i)
	{
		std::wstring configName = fmt::format(CFG_KEY_FILTER L"{}", i);
		int idxFilter = inTree->FindElement(configName);
		if (0 <= idxFilter && ARBElementType::Node == inTree->GetElement(idxFilter)->GetType())
		{
			ElementNodePtr nodeFilter = inTree->GetElementNode(idxFilter);
			int idxName = nodeFilter->FindElement(L"Name");
			if (0 > idxName)
				continue; // Ignore no-name filters.
			std::wstring name = nodeFilter->GetElementNode(idxName)->GetValue();
			ElementNodePtr existingFilter = FindElementName(root, numExistingFilters, name, CFG_KEY_FILTER, L"Name");
			if (existingFilter)
			{
				CConfigPathHelper config(StringUtil::stringWX(existingFilter->GetName()));
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
			}
			else
			{
				name = nodeFilter->GetName();
				std::wstring newName = fmt::format(CFG_KEY_FILTER L"{}", numExistingFilters + added);
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


void ImportConfig(ElementNodePtr const& inTree, bool bClobberFilters)
{
	std::wstring type;
	if (ARBAttribLookup::Found != inTree->GetAttrib(L"type", type))
		return;
	if (L"g" == type)
	{
		CConfigPathHelper config(StringUtil::stringWX(inTree->GetName()));
		// When importing config info, treat ColumnInfo differently.
		// We don't want to wipe out existing items. We want to add the new
		// names. If we find an existing name, the specified format will
		// be clobbered. But any existing ones won't. This allows us to
		// import only "Import" data and not touch "Export" (depending on
		// what's in the file we're reading, of course!)
		bool bClobber = true;
		if (inTree->GetName() == CFG_KEY_COLUMNINFO)
		{
			bClobber = ImportColumnInfo(inTree);
		}
		else if (!bClobberFilters && inTree->GetName().substr(0, 6) == CFG_KEY_FILTER)
		{
			bClobber = false;
		}
		if (bClobber)
		{
			bool bCommon = (inTree->GetName() == CFG_KEY_COMMON);
			for (int i = 0; i < inTree->GetElementCount(); ++i)
			{
				ElementNodePtr node = inTree->GetElementNode(i);
				if (!node)
					continue;
				bool bSkip = false;
				if (!bClobberFilters && bCommon
					&& (node->GetName() == L"numFilters" || node->GetName() == L"CurrentFilter"))
				{
					bSkip = true;
				}
				if (!bSkip)
					ImportConfig(node, bClobberFilters);
			}
		}
	}
	else if (L"s" == type)
	{
		wxConfig::Get()->Write(StringUtil::stringWX(inTree->GetName()), inTree->GetValue().c_str());
	}
	else if (L"b" == type)
	{
		bool val;
		inTree->GetAttrib(L"val", val);
		wxConfig::Get()->Write(StringUtil::stringWX(inTree->GetName()), val);
	}
	else if (L"i" == type)
	{
		long val;
		inTree->GetAttrib(L"val", val);
		wxConfig::Get()->Write(StringUtil::stringWX(inTree->GetName()), val);
	}
	else if (L"f" == type)
	{
		double val;
		inTree->GetAttrib(L"val", val);
		wxConfig::Get()->Write(StringUtil::stringWX(inTree->GetName()), val);
	}
}
}; // namespace


bool CAgilityBookOptions::ImportSettings(ElementNodePtr const& inree)
{
	if (!inree || inree->GetName() != L"AgilityBookSettings")
		return false;
	// Version numbers aren't needed yet.
	ARBVersion version;
	if (ARBAttribLookup::Found != inree->GetAttrib(ATTRIB_BOOK_VERSION, version))
		return false;
	std::wstring pgmVersion;
	if (ARBAttribLookup::Found != inree->GetAttrib(ATTRIB_BOOK_PGM_VERSION, pgmVersion))
		return false;
	bool bClobberFilters = MergeFilters(inree);
	for (int i = 0; i < inree->GetElementCount(); ++i)
	{
		ElementNodePtr ele = inree->GetElementNode(i);
		if (!ele)
			continue;
		ImportConfig(ele, bClobberFilters);
	}
	return true;
}


ElementNodePtr CAgilityBookOptions::ExportSettings()
{
	wxBusyCursor wait;
	CVersionNum ver(ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD);
	std::wstring verstr = ver.GetVersionString();
	ElementNodePtr tree(ElementNode::New(L"AgilityBookSettings"));
	tree->AddAttrib(ATTRIB_BOOK_VERSION, ARBAgilityRecordBook::GetCurrentDocVersion());
	tree->AddAttrib(ATTRIB_BOOK_PGM_VERSION, verstr);

	// These sections are copied complete.
	constexpr wchar_t const* const sections[] = {
		CFG_KEY_CALENDAR,
		CFG_KEY_COLUMNS,
		CFG_KEY_COLUMNINFO,
		CFG_KEY_COMMON,
		CFG_KEY_EXPORT,
		CFG_KEY_EXPORTCAL,
		CFG_KEY_EXPORTCALAPPT,
		CFG_KEY_EXPORTCALTASK,
		CFG_KEY_EXPORTLOG,
		CFG_KEY_IMPORT,
		CFG_KEY_IMPORTCAL,
		CFG_KEY_IMPORTLOG,
		CFG_KEY_SORTING,
		CFG_KEY_ACCELERATORS,
		nullptr,
	};
	for (int i = 0; sections[i]; ++i)
	{
		ExportConfig(sections[i], tree);
	}

	// Copy all the filters complete.
	int nFilters = wxConfig::Get()->Read(CFG_COMMON_NUMFILTERS, 0L);
	for (int i = 0; i < nFilters; ++i)
	{
		ExportConfig(CFG_KEY_FILTER_N(i, false), tree);
	}

	// And pick/choose in Settings.
	ElementNodePtr settings = tree->AddElementNode(CFG_KEY_SETTINGS);
	settings->AddAttrib(L"type", L"g");

	CConfigPathHelper config(CFG_KEY_SETTINGS);
	constexpr wchar_t const* const items[] = {
		L"autoCheck",
		L"autoShowTitle",
		L"printLand",
		L"proxy",
		L"showHtml",
		L"useproxy",
		L"View",
		L"ViewOrient",
		L"ViewType",
		nullptr,
	};
	for (int i = 0; items[i]; ++i)
	{
		ExportConfigItem(items[i], settings);
	}
	return tree;
}

/////////////////////////////////////////////////////////////////////////////

void CFontInfo::CreateFont(wxFont& font)
{
	font = wxFont();
	font.SetFaceName(name.c_str());
	font.SetPointSize(size);
	wxFontStyle style = wxFONTSTYLE_NORMAL;
	if (italic)
		style = wxFONTSTYLE_ITALIC;
	wxFontWeight weight = wxFONTWEIGHT_NORMAL;
	if (bold)
		weight = wxFONTWEIGHT_BOLD;
	font.SetStyle(style);
	font.SetWeight(weight);
}


void CFontInfo::CreateFont(wxFontDialog& dlg, wxFont& font)
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


namespace
{
wchar_t const* CalItemName(ARBCalColorItem inItem)
{
	switch (inItem)
	{
	case ARBCalColorItem::Past:
		return CFG_CAL_ITEM_PASTCOLOR;
	case ARBCalColorItem::NotEntered:
		return CFG_CAL_ITEM_NOTENTEREDCOLOR;
	case ARBCalColorItem::Planning:
		return CFG_CAL_ITEM_PLANNINGCOLOR;
	case ARBCalColorItem::Pending:
		return CFG_CAL_ITEM_PENDINGCOLOR;
	case ARBCalColorItem::Entered:
		return CFG_CAL_ITEM_ENTEREDCOLOR;
	case ARBCalColorItem::Opening:
		return CFG_CAL_ITEM_OPENCOLOR;
	case ARBCalColorItem::Closing:
		return CFG_CAL_ITEM_CLOSECOLOR;
	}
	assert(0);
	return L"";
}


wxColour CalItemColor(ARBCalColorItem inItem)
{
	switch (inItem)
	{
	case ARBCalColorItem::Past:
		return sc_CalColorPast();
	case ARBCalColorItem::NotEntered:
		return sc_CalColorNotEntered();
	case ARBCalColorItem::Planning:
		return sc_CalColorPlanning();
	case ARBCalColorItem::Pending:
		return sc_CalColorPending();
	case ARBCalColorItem::Entered:
		return sc_CalColorEntered();
	case ARBCalColorItem::Opening:
		return sc_CalColorOpening();
	case ARBCalColorItem::Closing:
		return sc_CalColorClosing();
	}
	assert(0);
	return wxColour(0, 0, 0);
}


// Reading/writing is abstracted since a WXCOLORREF is vastly different
// on a Mac (uchar[6] vs ulong). Which means trying to use it directly
// with wxConfig makes for a very unhappy compiler.
wxColour ReadColor(wxString const& key, wxColour const& defColor)
{
	unsigned long val = defColor.Red();
	val |= (defColor.Green() << 8);
	val |= (defColor.Blue() << 16);
	val = wxConfig::Get()->Read(key, val);
	return wxColour(val);
}


void WriteColor(wxString const& key, wxColour const& inColor)
{
	unsigned long val = inColor.Red();
	val |= (inColor.Green() << 8);
	val |= (inColor.Blue() << 16);
	wxConfig::Get()->Write(key, static_cast<long>(val));
}
} // namespace


wxColour CAgilityBookOptions::CalendarColor(ARBCalColorItem inItem)
{
	std::wstring key = fmt::format(L"{}/{}", CFG_KEY_CALENDAR, CalItemName(inItem));
	return ReadColor(key, CalItemColor(inItem));
}


void CAgilityBookOptions::SetCalendarColor(ARBCalColorItem inItem, wxColour inColor)
{
	std::wstring key = fmt::format(L"{}/{}", CFG_KEY_CALENDAR, CalItemName(inItem));
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
	return ReadColor(CFG_CAL_OPENNEARCOLOR, sc_CalColorOpeningNear());
}


void CAgilityBookOptions::SetCalendarOpeningNearColor(wxColour inColor)
{
	WriteColor(CFG_CAL_OPENNEARCOLOR, inColor);
}


wxColour CAgilityBookOptions::CalendarClosingNearColor()
{
	return ReadColor(CFG_CAL_CLOSENEARCOLOR, sc_CalColorClosingNear());
}


void CAgilityBookOptions::SetCalendarClosingNearColor(wxColour inColor)
{
	WriteColor(CFG_CAL_CLOSENEARCOLOR, inColor);
}

/////////////////////////////////////////////////////////////////////////////
// Common options

ARBDayOfWeek CAgilityBookOptions::GetFirstDayOfWeek()
{
	long val = static_cast<long>(sc_FirstDayOfWeek);
	wxConfig::Get()->Read(CFG_COMMON_FIRSTDAYOFWEEK, &val);
	if (val < 0 || val > 6)
		val = static_cast<long>(ARBDayOfWeek::Sunday);
	return static_cast<ARBDayOfWeek>(val);
}


void CAgilityBookOptions::SetFirstDayOfWeek(ARBDayOfWeek day)
{
	wxConfig::Get()->Write(CFG_COMMON_FIRSTDAYOFWEEK, static_cast<long>(day));
}

/////////////////////////////////////////////////////////////////////////////
// Runs/points options

ARBViewRuns CAgilityBookOptions::GetViewRunsStyle()
{
	return static_cast<ARBViewRuns>(
		wxConfig::Get()->Read(CFG_COMMON_VIEWRUNSBYTRIAL, static_cast<long>(sc_ViewRunsStyle)));
}


void CAgilityBookOptions::SetViewRunsStyle(ARBViewRuns style)
{
	wxConfig::Get()->Write(CFG_COMMON_VIEWRUNSBYTRIAL, static_cast<long>(style));
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
	ARBPointsViewSort& outPrimary,
	ARBPointsViewSort& outSecondary,
	ARBPointsViewSort& outTertiary)
{
	outPrimary = static_cast<ARBPointsViewSort>(
		wxConfig::Get()->Read(CFG_COMMON_SORTPTVW1, static_cast<long>(sc_PointsViewSort1)));
	outSecondary = static_cast<ARBPointsViewSort>(
		wxConfig::Get()->Read(CFG_COMMON_SORTPTVW2, static_cast<long>(sc_PointsViewSort2)));
	outTertiary = static_cast<ARBPointsViewSort>(
		wxConfig::Get()->Read(CFG_COMMON_SORTPTVW3, static_cast<long>(sc_PointsViewSort3)));
}


void CAgilityBookOptions::SetPointsViewSort(
	ARBPointsViewSort inPrimary,
	ARBPointsViewSort inSecondary,
	ARBPointsViewSort inTertiary)
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


bool CAgilityBookOptions::ShowCoSanctioning()
{
	bool val = sc_ShowCoSanctioning;
	wxConfig::Get()->Read(CFG_COMMON_SHOWCOSANCTION, &val);
	return val;
}


void CAgilityBookOptions::SetShowCoSanctioning(bool bSet)
{
	wxConfig::Get()->Write(CFG_COMMON_SHOWCOSANCTION, bSet);
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


namespace
{
long ConvertMetric(long val, bool bToMM)
{
	if (bToMM)
		return static_cast<long>(static_cast<double>(val) * 0.254);
	else
		return static_cast<long>(static_cast<double>(val) / 0.254);
}
} // namespace


void CAgilityBookOptions::GetPrinterMargins(
	bool bAsMM, // In .01in or millimeters, ignored if DC is set
	long& outLeft,
	long& outRight,
	long& outTop,
	long& outBottom,
#if wxCHECK_VERSION(3, 3, 0)
	wxReadOnlyDC const* pDC
#else
	wxDC const* pDC
#endif
)
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
#if wxCHECK_VERSION(3, 3, 0)
	wxReadOnlyDC const* pDC
#else
	wxDC const* pDC
#endif
)
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


void CAgilityBookOptions::SetRunPageSize(bool bAsMM, long inWidth, long inHeight)
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

namespace
{
std::wstring GetLastKey(
	ARBDogPtr const& inDog,
	ARBConfigVenuePtr const& inVenue,
	wchar_t const* keyGroup,
	wchar_t const* keyDefault,
	bool bFallback)
{
	assert(keyGroup && keyDefault);
	std::wstring key(keyDefault);
	if (inDog && !inDog->GetCallName().empty())
	{
		// The 'Clean' routines below have "knowledge" of
		//  <key>/<dog>/<venue> order.
		fmt::wmemory_buffer buf;
		fmt::format_to(std::back_inserter(buf), L"{}/{}", keyGroup, inDog->GetCallName());
		if (inVenue && !inVenue->GetName().empty())
		{
			fmt::format_to(std::back_inserter(buf), L"/{}", inVenue->GetName());
		}
		key = fmt::to_string(buf);
		// Fall back
		if (bFallback && !wxConfig::Get()->Exists(key))
			key = keyDefault;
	}
	return key;
}


std::wstring GetLastValue(
	ARBDogPtr const& inDog,
	ARBConfigVenuePtr const& inVenue,
	wchar_t const* keyDog,
	wchar_t const* keyDefault)
{
	std::wstring key = GetLastKey(inDog, inVenue, keyDog, keyDefault, true);
	return StringUtil::stringW(wxConfig::Get()->Read(key, wxString()));
}


void WriteLastValue(
	ARBDogPtr const& inDog,
	ARBConfigVenuePtr const& inVenue,
	wchar_t const* keyDog,
	wchar_t const* keyDefault,
	wchar_t const* inLast)
{
	std::wstring key = GetLastKey(inDog, inVenue, keyDog, keyDefault, false);
	if (inLast)
	{
		wxConfig::Get()->Write(key, inLast);
		if (key != keyDefault)
			wxConfig::Get()->Write(keyDefault, inLast);
	}
	else
		wxConfig::Get()->DeleteEntry(key);
}
} // namespace


std::wstring CAgilityBookOptions::GetLastEnteredDivision(ARBDogPtr const& inDog, ARBConfigVenuePtr const& inVenue)
{
	return GetLastValue(inDog, inVenue, CFG_LAST_DIVISION_KEY, CFG_LAST_DIVISION);
}


void CAgilityBookOptions::SetLastEnteredDivision(
	ARBDogPtr const& inDog,
	ARBConfigVenuePtr const& inVenue,
	wchar_t const* inLast)
{
	WriteLastValue(inDog, inVenue, CFG_LAST_DIVISION_KEY, CFG_LAST_DIVISION, inLast);
}


std::wstring CAgilityBookOptions::GetLastEnteredLevel(ARBDogPtr const& inDog, ARBConfigVenuePtr const& inVenue)
{
	return GetLastValue(inDog, inVenue, CFG_LAST_LEVEL_KEY, CFG_LAST_LEVEL);
}


void CAgilityBookOptions::SetLastEnteredLevel(
	ARBDogPtr const& inDog,
	ARBConfigVenuePtr const& inVenue,
	wchar_t const* inLast)
{
	WriteLastValue(inDog, inVenue, CFG_LAST_LEVEL_KEY, CFG_LAST_LEVEL, inLast);
}


std::wstring CAgilityBookOptions::GetLastEnteredHeight(ARBDogPtr const& inDog, ARBConfigVenuePtr const& inVenue)
{
	return GetLastValue(inDog, inVenue, CFG_LAST_HEIGHT_KEY, CFG_LAST_HEIGHT);
}


void CAgilityBookOptions::SetLastEnteredHeight(
	ARBDogPtr const& inDog,
	ARBConfigVenuePtr const& inVenue,
	wchar_t const* inLast)
{
	WriteLastValue(inDog, inVenue, CFG_LAST_HEIGHT_KEY, CFG_LAST_HEIGHT, inLast);
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


std::wstring CAgilityBookOptions::GetLastEnteredHandler(ARBDogPtr const& inDog)
{
	return GetLastValue(inDog, ARBConfigVenuePtr(), CFG_LAST_HANDLER_KEY, CFG_LAST_HANDLER);
}


void CAgilityBookOptions::SetLastEnteredHandler(ARBDogPtr const& inDog, wchar_t const* inLast)
{
	WriteLastValue(inDog, ARBConfigVenuePtr(), CFG_LAST_HANDLER_KEY, CFG_LAST_HANDLER, inLast);
}


// Note: If name changes, I don't care. Just cleanse.
// Also, make sure this is updated if any of the 'Last' formats change.

namespace
{
constexpr struct
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
constexpr size_t sc_KeysCount = _countof(sc_Keys);
} // namespace


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
		CConfigPathHelper config(sc_Keys[i].key);

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
			CConfigPathHelper config(key);

			wxString strDog;
			long idxDog;
			if (wxConfig::Get()->GetFirstGroup(strDog, idxDog))
			{
				do
				{
					CConfigPathHelper configDog(strDog);

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
					for (std::vector<wxString>::iterator iter = deleteVenues.begin(); iter != deleteVenues.end();
						 ++iter)
					{
						wxConfig::Get()->DeleteEntry(*iter);
					}
				} while (wxConfig::Get()->GetNextGroup(strDog, idxDog));
			}
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


void CAgilityBookOptions::GetImportExportDelimiters(bool bImport, ARBImportExportDelim& delim, std::wstring& delimiter)
{
	std::wstring section = fmt::format(L"{}/", bImport ? CFG_KEY_IMPORT : CFG_KEY_EXPORT);
	delim = sc_ImportExportDelim;
	delimiter.clear();
	delim = static_cast<ARBImportExportDelim>(
		wxConfig::Get()->Read(section + CFG_IMPORT_EXPORT_DELIM, static_cast<long>(delim)));
	delimiter = StringUtil::stringW(wxConfig::Get()->Read(section + CFG_IMPORT_EXPORT_DELIMITER, delimiter.c_str()));
	if (1 < delimiter.length())
		delimiter = delimiter.substr(0, 1);
}


void CAgilityBookOptions::SetImportExportDelimiters(
	bool bImport,
	ARBImportExportDelim delim,
	std::wstring const& delimiter)
{
	std::wstring section = fmt::format(L"{}/", bImport ? CFG_KEY_IMPORT : CFG_KEY_EXPORT);
	wxConfig::Get()->Write(section + CFG_IMPORT_EXPORT_DELIM, static_cast<long>(delim));
	wxConfig::Get()->Write(section + CFG_IMPORT_EXPORT_DELIMITER, delimiter.c_str());
}


void CAgilityBookOptions::GetImportExportDateFormat(bool bImport, ARBDateFormat& outFormat)
{
	std::wstring section = fmt::format(L"{}/", bImport ? CFG_KEY_IMPORT : CFG_KEY_EXPORT);
	outFormat = static_cast<ARBDateFormat>(
		wxConfig::Get()->Read(section + CFG_IMPORT_EXPORT_DATEFORMAT, static_cast<long>(sc_ImportExportFormat)));
	if (ARBDateFormat::Reserved14 == outFormat)
		outFormat = ARBDateFormat::Locale;
}


void CAgilityBookOptions::SetImportExportDateFormat(bool bImport, ARBDateFormat inFormat)
{
	std::wstring section = fmt::format(L"{}/", bImport ? CFG_KEY_IMPORT : CFG_KEY_EXPORT);
	wxConfig::Get()->Write(section + CFG_IMPORT_EXPORT_DATEFORMAT, static_cast<long>(inFormat));
}


namespace
{
wchar_t const* GetColumnName(CAgilityBookOptions::ColumnOrder eOrder)
{
	switch (eOrder)
	{
	case CAgilityBookOptions::eUnknown:
	case CAgilityBookOptions::eAllColumns:
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
	// 'enum class' handles all cases via the switch above
	return CFG_KEY_UNKNOWN;
}
} // namespace


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
		std::wstring item;
		if (!namedColumn.empty())
			item = fmt::format(L"/{}{}/{}", namedColumn, GetColumnName(eOrder), CFG_COL_BASENAME(idxColumn));
		else
			item = fmt::format(L"{}/{}", GetColumnName(eOrder), CFG_COL_BASENAME(idxColumn));
		std::wstring data = StringUtil::stringW(wxConfig::Get()->Read(item, wxEmptyString));
		std::wstring::size_type idx = data.find(',');
		while (std::wstring::npos != idx)
		{
			long val;
			StringUtil::ToCLong(data, val, true);
			outValues.push_back(val);
			data = data.substr(idx + 1);
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
		case eUnknown:
		case eAllColumns:
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

			default:
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
			default:
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
	fmt::wmemory_buffer data;
	for (size_t i = 0; i < inValues.size(); ++i)
	{
		if (0 < i)
			fmt::format_to(std::back_inserter(data), L"{}", L",");
		fmt::format_to(std::back_inserter(data), L"{}", inValues[i]);
	}
	std::wstring item;
	if (!namedColumn.empty())
		item = fmt::format(L"/{}{}/{}", namedColumn, GetColumnName(eOrder), CFG_COL_BASENAME(idxColumn));
	else
		item = fmt::format(L"{}/{}", GetColumnName(eOrder), CFG_COL_BASENAME(idxColumn));
	wxConfig::Get()->Write(item, fmt::to_string(data).c_str());
}

/////////////////////////////////////////////////////////////////////////////

long CAgilityBookOptions::GetMRUFileCount()
{
	return wxConfig::Get()->Read(CFG_SETTINGS_MRU, sc_MRUCount);
}


void CAgilityBookOptions::SetMRUFileCount(long nFiles)
{
	wxConfig::Get()->Write(CFG_SETTINGS_MRU, nFiles);
}


bool CAgilityBookOptions::GetAutoUpdateCheck()
{
#if defined(_DEBUG) || defined(__WXDEBUG__)
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


long CAgilityBookOptions::GetAutoUpdateCheckInterval()
{
	long val = wxConfig::Get()->Read(CFG_SETTINGS_AUTOCHECK_INTERVAL, sc_AutoUpdateCheckInterval);
	if (val < 1)
	{
		val = sc_AutoUpdateCheckInterval;
		SetAutoUpdateCheckInterval(val);
	}
	return val;
}


void CAgilityBookOptions::SetAutoUpdateCheckInterval(long days)
{
	wxConfig::Get()->Write(CFG_SETTINGS_AUTOCHECK_INTERVAL, days);
}


long CAgilityBookOptions::GetNumBackupFiles()
{
	return wxConfig::Get()->Read(CFG_SETTINGS_BACKUPFILES, sc_NumBackup);
}


void CAgilityBookOptions::SetNumBackupFiles(long nFiles)
{
	wxConfig::Get()->Write(CFG_SETTINGS_BACKUPFILES, nFiles);
}


wxString CAgilityBookOptions::GetBackupDirectory()
{
	return wxConfig::Get()->Read(CFG_SETTINGS_BACKUPDIR, wxString());
}


void CAgilityBookOptions::SetBackupDirectory(wxString const& dir)
{
	wxConfig::Get()->Write(CFG_SETTINGS_BACKUPDIR, dir);
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


bool CAgilityBookOptions::UseAlternateRowColor()
{
	bool val = sc_UseAltRowColor;
	wxConfig::Get()->Read(CFG_SETTINGS_USEALTROWCOLOR, &val);
	return val;
}


void CAgilityBookOptions::SetUseAlternateRowColor(bool bUse)
{
	wxConfig::Get()->Write(CFG_SETTINGS_USEALTROWCOLOR, bUse);
}

/////////////////////////////////////////////////////////////////////////////

std::wstring CAgilityBookOptions::GetUserName(std::wstring const& hint)
{
	std::wstring section = fmt::format(L"{}/{}", CFG_KEY_USERNAMES, hint);
	return StringUtil::stringW(wxConfig::Get()->Read(section, wxString()));
}


void CAgilityBookOptions::SetUserName(std::wstring const& hint, std::wstring const& userName)
{
	std::wstring section = fmt::format(L"{}/{}", CFG_KEY_USERNAMES, hint);
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

} // namespace dconSoft
