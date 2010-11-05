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

#include "ARBAgilityRecordBook.h"
#include "ARBCalendar.h"
#include "ARBDate.h"
#include "ARBDogTitle.h"
#include "ARBDogTrial.h"
#include "DlgAssignColumns.h"
#include "Element.h"
#include "RegItems.h"
#include "VersionNum.h"

#include <wx/config.h>


static void ExportConfigItem(wxString const& entry, ElementNodePtr tree)
{
	switch (wxConfig::Get()->GetEntryType(entry))
	{
	default:
		break;
	case wxConfigBase::Type_String:
		{
			ElementNodePtr item = tree->AddElementNode(entry);
			item->AddAttrib(wxT("type"), wxT("s"));
			item->SetValue(wxConfig::Get()->Read(entry, wxEmptyString));
		}
		break;
	case wxConfigBase::Type_Boolean:
		{
			ElementNodePtr item = tree->AddElementNode(entry);
			item->AddAttrib(wxT("type"), wxT("b"));
			bool val;
			wxConfig::Get()->Read(entry, &val);
			item->AddAttrib(wxT("val"), val);
		}
		break;
	case wxConfigBase::Type_Integer:
		{
			ElementNodePtr item = tree->AddElementNode(entry);
			item->AddAttrib(wxT("type"), wxT("i"));
			long val;
			wxConfig::Get()->Read(entry, &val);
			item->AddAttrib(wxT("val"), val);
		}
		break;
	case wxConfigBase::Type_Float:
		{
			ElementNodePtr item = tree->AddElementNode(entry);
			item->AddAttrib(wxT("type"), wxT("f"));
			double val;
			wxConfig::Get()->Read(entry, &val);
			item->AddAttrib(wxT("val"), val);
		}
		break;
	}
}


static void ExportConfig(wxString const& key, ElementNodePtr root)
{
	if (!wxConfig::Get()->HasGroup(key))
		return;

	ElementNodePtr tree = root->AddElementNode(key);
	tree->AddAttrib(wxT("type"), wxT("g"));

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


static ElementNodePtr FindConfigName(
		ElementNodePtr tree,
		long numConfigs,
		wxString const& name)
{
	for (long i = 0; i < numConfigs; ++i)
	{
		wxString configName = wxString::Format(wxT("Config%d"), i);
		int idxConfig = tree->FindElement(configName);
		if (0 <= idxConfig
		&& Element::Element_Node == tree->GetElement(idxConfig)->GetType())
		{
			ElementNodePtr nodeConfig = tree->GetElementNode(idxConfig);
			int idxName = nodeConfig->FindElement(wxT("name"));
			if (0 <= idxName
			&& name == nodeConfig->GetElementNode(idxName)->GetValue())
			{
				return nodeConfig;
			}
		}
	}
	return ElementNodePtr();
}


static void ImportConfig(ElementNodePtr tree);


// Return true to invoke clobber code below.
static bool ImportColumnInfo(ElementNodePtr tree)
{
	// Only "Config#" are merged (unless we clobber).
	int idxNum = tree->FindElement(wxT("numConfigs"));
	if (0 > idxNum)
		return false;
	long numConfigs = 0;
	tree->GetElementNode(idxNum)->GetAttrib(wxT("val"), numConfigs);
	if (0 >= numConfigs)
		return false;

	// If there are no existing configs, just bail and clobber.
	ElementNodePtr root(ElementNode::New(wxT("Top")));
	wxString path = wxConfig::Get()->GetPath();
	wxConfig::Get()->SetPath(wxT("/"));
	ExportConfig(wxT("ColumnInfo"), root);
	wxConfig::Get()->SetPath(path);
	if (1 != root->GetElementCount())
		return true;
	ElementNodePtr existing = root->GetElementNode(0);
	if (!existing)
		return true;
	idxNum = existing->FindElement(wxT("numConfigs"));
	if (0 > idxNum)
		return true;
	long numExistingConfigs = 0;
	existing->GetElementNode(idxNum)->GetAttrib(wxT("val"), numExistingConfigs);
	if (0 >= numExistingConfigs)
		return true;

	long added = 0;
	for (long i = 0; i < numConfigs; ++i)
	{
		wxString configName = wxString::Format(wxT("Config%ld"), i);
		int idxConfig = tree->FindElement(configName);
		if (0 <= idxConfig
		&& Element::Element_Node == tree->GetElement(idxConfig)->GetType())
		{
			ElementNodePtr nodeConfig = tree->GetElementNode(idxConfig);
			int idxName = nodeConfig->FindElement(wxT("name"));
			if (0 > idxName)
				continue; // Ignore no-name configs.
			wxString name = nodeConfig->GetElementNode(idxName)->GetValue();
			ElementNodePtr existingConfig = FindConfigName(existing, numExistingConfigs, name);
			if (existingConfig)
			{
				path = wxConfig::Get()->GetPath();
				wxConfig::Get()->SetPath(existingConfig->GetName());
				for (int iCfg = 0; iCfg < nodeConfig->GetElementCount(); ++iCfg)
				{
					ElementNodePtr node = nodeConfig->GetElementNode(iCfg);
					if (!node)
						continue;
					if (node->GetName() != wxT("name"))
					{
						ImportConfig(node);
					}
				}
				wxConfig::Get()->SetPath(path);
			}
			else
			{
				name = nodeConfig->GetName();
				wxString newName = wxString::Format(wxT("Config%ld"), numExistingConfigs + added);
				++added;
				nodeConfig->SetName(newName);
				ImportConfig(nodeConfig);
				nodeConfig->SetName(name);
			}
		}
	}
	return false;
}


static void ImportConfig(ElementNodePtr tree)
{
	wxString type;
	if (ElementNode::eFound != tree->GetAttrib(wxT("type"), type))
		return;
	if (wxT("g") == type)
	{
		wxString path = wxConfig::Get()->GetPath();
		wxConfig::Get()->SetPath(tree->GetName());
		// When importing config info, treat ColumnInfo differently.
		// We don't want to wipe out existing items. We want to add the new
		// names. If we find an existing name, the specified format will
		// be clobbered. But any existing ones won't. This allows us to
		// import only "Import" data and not touch "Export" (depending on
		// what's in the file we're reading, of course!)
		bool bClobber = true;
		if (tree->GetName() == wxT("ColumnInfo"))
		{
			bClobber = ImportColumnInfo(tree);
		}
		if (bClobber)
		{
			for (int i = 0; i < tree->GetElementCount(); ++i)
			{
				if (Element::Element_Node == tree->GetElement(i)->GetType())
				{
					ImportConfig(tree->GetElementNode(i));
				}
			}
		}
		wxConfig::Get()->SetPath(path);
	}
	else if (wxT("s") == type)
	{
		wxConfig::Get()->Write(tree->GetName(), tree->GetValue());
	}
	else if (wxT("b") == type)
	{
		bool val;
		tree->GetAttrib(wxT("val"), val);
		wxConfig::Get()->Write(tree->GetName(), val);
	}
	else if (wxT("i") == type)
	{
		long val;
		tree->GetAttrib(wxT("val"), val);
		wxConfig::Get()->Write(tree->GetName(), val);
	}
	else if (wxT("f") == type)
	{
		double val;
		tree->GetAttrib(wxT("val"), val);
		wxConfig::Get()->Write(tree->GetName(), val);
	}
}


bool CAgilityBookOptions::ImportSettings(ElementNodePtr tree)
{
	if (!tree || tree->GetName() != wxT("AgilityBookSettings"))
		return false;
	// Version numbers aren't needed yet.
	ARBVersion version;
	if (ElementNode::eFound != tree->GetAttrib(ATTRIB_BOOK_VERSION, version))
		return false;
	wxString pgmVersion;
	if (ElementNode::eFound != tree->GetAttrib(ATTRIB_BOOK_PGM_VERSION, pgmVersion))
		return false;
	for (int i = 0; i < tree->GetElementCount(); ++i)
	{
		ElementNodePtr ele = tree->GetElementNode(i);
		if (!ele)
			continue;
		ImportConfig(ele);
	}
	return true;
}


ElementNodePtr CAgilityBookOptions::ExportSettings()
{
	wxBusyCursor wait;
	CVersionNum ver(true);
	wxString verstr = ver.GetVersionString();
	ElementNodePtr tree(ElementNode::New(wxT("AgilityBookSettings")));
	tree->AddAttrib(ATTRIB_BOOK_VERSION, ARBAgilityRecordBook::GetCurrentDocVersion());
	tree->AddAttrib(ATTRIB_BOOK_PGM_VERSION, verstr);

	// These sections are copied complete.
	static wxChar const* const sections[] = {
		wxT("Calendar"),
		wxT("Columns"),
		wxT("ColumnInfo"),
		wxT("Common"),
		wxT("Export"),
		wxT("ExportCal"),
		wxT("ExportCalAppt"),
		wxT("ExportCalTask"),
		wxT("ExportLog"),
		wxT("Import"),
		wxT("ImportCal"),
		wxT("ImportLog"),
		wxT("Sorting"),
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
	ElementNodePtr settings = tree->AddElementNode(wxT("Settings"));
	settings->AddAttrib(wxT("type"), wxT("g"));
	wxString path = wxConfig::Get()->GetPath();
	wxConfig::Get()->SetPath(wxT("Settings"));
	static wxChar const* const items[] = {
		wxT("autoCheck"),
		wxT("autoShowTitle"),
		wxT("printLand"),
		wxT("proxy"),
		wxT("showHtml"),
		wxT("useproxy"),
		wxT("View"),
		wxT("ViewOrient"),
		wxT("ViewType"),
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
	font.SetFaceName(name);
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
	bool val = false;
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
	bool val = true;
	wxConfig::Get()->Read(CFG_CAL_VIEWALL, &val);
	return val;
}


void CAgilityBookOptions::SetViewAllCalendarEntries(bool bView)
{
	wxConfig::Get()->Write(CFG_CAL_VIEWALL, bView);
}


bool CAgilityBookOptions::ViewAllCalendarOpening()
{
	bool val = true;
	wxConfig::Get()->Read(CFG_CAL_VIEWOPEN, &val);
	return val;
}


void CAgilityBookOptions::SetViewAllCalendarOpening(bool bView)
{
	wxConfig::Get()->Write(CFG_CAL_VIEWOPEN, bView);
}


bool CAgilityBookOptions::ViewAllCalendarClosing()
{
	bool val = true;
	wxConfig::Get()->Read(CFG_CAL_VIEWCLOSE, &val);
	return val;
}


void CAgilityBookOptions::SetViewAllCalendarClosing(bool bView)
{
	wxConfig::Get()->Write(CFG_CAL_VIEWCLOSE, bView);
}


long CAgilityBookOptions::DaysTillEntryIsPast()
{
	long val = 5;
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
	bool val = false;
	wxConfig::Get()->Read(CFG_CAL_HIDEOVERLAPPING, &val);
	return val;
}


void CAgilityBookOptions::SetHideOverlappingCalendarEntries(bool bHide)
{
	wxConfig::Get()->Write(CFG_CAL_HIDEOVERLAPPING, bHide);
}


static wxChar const* CalItemName(CAgilityBookOptions::CalendarColorItem inItem)
{
	switch (inItem)
	{
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
	return wxT("");
}


static wxColour CalItemColor(CAgilityBookOptions::CalendarColorItem inItem)
{
	switch (inItem)
	{
	case CAgilityBookOptions::eCalColorNotEntered:
		return wxColour(0,0,0); // Black
	case CAgilityBookOptions::eCalColorPlanning:
		return wxColour(255,128,0); // Orange
	case CAgilityBookOptions::eCalColorPending:
		return wxColour(128,0,255); // Blue-ish
	case CAgilityBookOptions::eCalColorEntered:
		return wxColour(0,0,255); // Blue
	case CAgilityBookOptions::eCalColorOpening:
		return wxColour(0,128,0); // Dk Green
	case CAgilityBookOptions::eCalColorClosing:
		return wxColour(255,0,0); // Red
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
	long val = 4;
	wxConfig::Get()->Read(CFG_CAL_OPENNEAR, &val);
	return val;
}


void CAgilityBookOptions::SetCalendarOpeningNear(long inDays)
{
	wxConfig::Get()->Write(CFG_CAL_OPENNEAR, inDays);
}


long CAgilityBookOptions::CalendarClosingNear()
{
	long val = 10;
	wxConfig::Get()->Read(CFG_CAL_CLOSENEAR, &val);
	return val;
}


void CAgilityBookOptions::SetCalendarClosingNear(long inDays)
{
	wxConfig::Get()->Write(CFG_CAL_CLOSENEAR, inDays);
}


wxColour CAgilityBookOptions::CalendarOpeningNearColor()
{
	return ReadColor(CFG_CAL_OPENNEARCOLOR, wxColour(0,0,255));
}


void CAgilityBookOptions::SetCalendarOpeningNearColor(wxColour inColor)
{
	WriteColor(CFG_CAL_OPENNEARCOLOR, inColor);
}


wxColour CAgilityBookOptions::CalendarClosingNearColor()
{
	return ReadColor(CFG_CAL_CLOSENEARCOLOR, wxColour(255,0,0));
}


void CAgilityBookOptions::SetCalendarClosingNearColor(wxColour inColor)
{
	WriteColor(CFG_CAL_CLOSENEARCOLOR, inColor);
}

/////////////////////////////////////////////////////////////////////////////
// Common options

ARBDate::DayOfWeek CAgilityBookOptions::GetFirstDayOfWeek()
{
	long val = static_cast<long>(ARBDate::eSunday);
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
	bool val = true;
	wxConfig::Get()->Read(CFG_COMMON_VIEWRUNSBYTRIAL, &val);
	return val;
}


void CAgilityBookOptions::SetViewRunsByTrial(bool bView)
{
	wxConfig::Get()->Write(CFG_COMMON_VIEWRUNSBYTRIAL,bView);
}


bool CAgilityBookOptions::GetNewestDatesFirst()
{
	bool val = false;
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
	outPrimary = static_cast<PointsViewSort>(wxConfig::Get()->Read(CFG_COMMON_SORTPTVW1, static_cast<long>(ePointsViewSortDivision)));
	outSecondary = static_cast<PointsViewSort>(wxConfig::Get()->Read(CFG_COMMON_SORTPTVW2, static_cast<long>(ePointsViewSortLevel)));
	outTertiary = static_cast<PointsViewSort>(wxConfig::Get()->Read(CFG_COMMON_SORTPTVW3, static_cast<long>(ePointsViewSortEvent)));
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
	bool val = false;
	wxConfig::Get()->Read(CFG_COMMON_VIEWHIDDENTITLES, &val);
	return val;
}


void CAgilityBookOptions::SetViewHiddenTitles(bool bSet)
{
	wxConfig::Get()->Write(CFG_COMMON_VIEWHIDDENTITLES, bSet);
}


bool CAgilityBookOptions::GetViewLifetimePointsByEvent()
{
	bool val = true;
	wxConfig::Get()->Read(CFG_COMMON_VIEWLIFETIMEEVENTS, &val);
	return val;
}


void CAgilityBookOptions::SetViewLifetimePointsByEvent(bool bSet)
{
	wxConfig::Get()->Write(CFG_COMMON_VIEWLIFETIMEEVENTS, bSet);
}


bool CAgilityBookOptions::GetTableInYPS()
{
	bool val = false;
	wxConfig::Get()->Read(CFG_COMMON_TABLEINYPS, &val);
	return val;
}


void CAgilityBookOptions::SetTableInYPS(bool bSet)
{
	wxConfig::Get()->Write(CFG_COMMON_TABLEINYPS, bSet);
}


bool CAgilityBookOptions::GetIncludeCRCDImage()
{
	bool val = false;
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
	info.name = wxT("Times New Roman");
	info.size = 80;
	info.italic = false;
	info.bold = false;
	info.name = wxConfig::Get()->Read(CFG_COMMON_PRINTFONTLISTNAME, info.name);
	wxConfig::Get()->Read(CFG_COMMON_PRINTFONTLISTSIZE, &info.size, info.size);
	wxConfig::Get()->Read(CFG_COMMON_PRINTFONTLISTITALIC, &info.italic, info.italic);
	wxConfig::Get()->Read(CFG_COMMON_PRINTFONTLISTBOLD, &info.bold, info.bold);
	info.size /= 10;
}


void CAgilityBookOptions::SetPrinterFontInfo(CFontInfo const& info)
{
	wxConfig::Get()->Write(CFG_COMMON_PRINTFONTLISTNAME, info.name);
	wxConfig::Get()->Write(CFG_COMMON_PRINTFONTLISTSIZE, info.size * 10);
	wxConfig::Get()->Write(CFG_COMMON_PRINTFONTLISTITALIC, info.italic);
	wxConfig::Get()->Write(CFG_COMMON_PRINTFONTLISTBOLD, info.bold);
}


bool CAgilityBookOptions::GetUnitsAsMM()
{
	bool bAsMM = false;
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
	bool bInMM = false;
	wxConfig::Get()->Read(CFG_COMMON_MARGINS_MM, &bInMM);
	outLeft = wxConfig::Get()->Read(CFG_COMMON_MARGINS_L, 50L);
	outRight = wxConfig::Get()->Read(CFG_COMMON_MARGINS_R, 50L);
	outTop = wxConfig::Get()->Read(CFG_COMMON_MARGINS_T, 50L);
	outBottom = wxConfig::Get()->Read(CFG_COMMON_MARGINS_B, 50L);
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
		outLeft = pDC->DeviceToLogicalXRel(outLeft / 100.0 * szPPI.x);
		outRight = pDC->DeviceToLogicalXRel(outRight / 100.0 * szPPI.x);
		outTop = pDC->DeviceToLogicalYRel(outTop / 100.0 * szPPI.y);
		outBottom = pDC->DeviceToLogicalYRel(outBottom / 100.0 * szPPI.y);
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
	bool bInMM = false;
	wxConfig::Get()->Read(CFG_COMMON_RUNPAGE_MM, &bInMM);
	outWidth = wxConfig::Get()->Read(CFG_COMMON_RUNPAGE_W, 0L);
	outHeight = wxConfig::Get()->Read(CFG_COMMON_RUNPAGE_H, 0L);
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
		outWidth = pDC->DeviceToLogicalXRel(outWidth / 100.0 * szPPI.x);
		outHeight = pDC->DeviceToLogicalXRel(outHeight / 100.0 * szPPI.x);
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
	info.name = wxT("Times New Roman");
	info.size = 80;
	info.italic = false;
	info.bold = false;
	info.name = wxConfig::Get()->Read(CFG_CAL_FONTTEXTNAME, info.name);
	wxConfig::Get()->Read(CFG_CAL_FONTTEXTSIZE, &info.size, info.size);
	wxConfig::Get()->Read(CFG_CAL_FONTTEXTITALIC, &info.italic, info.italic);
	wxConfig::Get()->Read(CFG_CAL_FONTTEXTBOLD, &info.bold, info.bold);
	info.size /= 10;
}


void CAgilityBookOptions::SetCalendarFontInfo(CFontInfo const& info)
{
	wxConfig::Get()->Write(CFG_CAL_FONTTEXTNAME, info.name);
	wxConfig::Get()->Write(CFG_CAL_FONTTEXTSIZE, info.size * 10);
	wxConfig::Get()->Write(CFG_CAL_FONTTEXTITALIC, info.italic);
	wxConfig::Get()->Write(CFG_CAL_FONTTEXTBOLD, info.bold);
}

/////////////////////////////////////////////////////////////////////////////
// Last entered options

wxString CAgilityBookOptions::GetLastEnteredDivision()
{
	return wxConfig::Get()->Read(CFG_LAST_DIVISION, wxString());
}


void CAgilityBookOptions::SetLastEnteredDivision(wxChar const* inLast)
{
	if (inLast)
		wxConfig::Get()->Write(CFG_LAST_DIVISION, inLast);
	else
		wxConfig::Get()->DeleteEntry(CFG_LAST_DIVISION);
}


wxString CAgilityBookOptions::GetLastEnteredLevel()
{
	return wxConfig::Get()->Read(CFG_LAST_LEVEL, wxString());
}


void CAgilityBookOptions::SetLastEnteredLevel(wxChar const* inLast)
{
	if (inLast)
		wxConfig::Get()->Write(CFG_LAST_LEVEL, inLast);
	else
		wxConfig::Get()->DeleteEntry(CFG_LAST_LEVEL);
}


wxString CAgilityBookOptions::GetLastEnteredHeight()
{
	return wxConfig::Get()->Read(CFG_LAST_HEIGHT, wxString());
}


void CAgilityBookOptions::SetLastEnteredHeight(wxChar const* inLast)
{
	if (inLast)
		wxConfig::Get()->Write(CFG_LAST_HEIGHT, inLast);
	else
		wxConfig::Get()->DeleteEntry(CFG_LAST_HEIGHT);
}


wxString CAgilityBookOptions::GetLastEnteredRefHeight()
{
	return wxConfig::Get()->Read(CFG_LAST_REFHEIGHT, wxString());
}


void CAgilityBookOptions::SetLastEnteredRefHeight(wxChar const* inLast)
{
	if (inLast)
		wxConfig::Get()->Write(CFG_LAST_REFHEIGHT, inLast);
	else
		wxConfig::Get()->DeleteEntry(CFG_LAST_REFHEIGHT);
}


wxString CAgilityBookOptions::GetLastEnteredJudge()
{
	return wxConfig::Get()->Read(CFG_LAST_JUDGE, wxString());
}


void CAgilityBookOptions::SetLastEnteredJudge(wxChar const* inLast)
{
	if (inLast)
		wxConfig::Get()->Write(CFG_LAST_JUDGE, inLast);
	else
		wxConfig::Get()->DeleteEntry(CFG_LAST_JUDGE);
}


wxString CAgilityBookOptions::GetLastEnteredHandler()
{
	return wxConfig::Get()->Read(CFG_LAST_HANDLER, wxString());
}


void CAgilityBookOptions::SetLastEnteredHandler(wxChar const* inLast)
{
	if (inLast)
		wxConfig::Get()->Write(CFG_LAST_HANDLER, inLast);
	else
		wxConfig::Get()->DeleteEntry(CFG_LAST_HANDLER);
}

/////////////////////////////////////////////////////////////////////////////
// Import/export options

long CAgilityBookOptions::GetImportStartRow()
{
	long row = wxConfig::Get()->Read(CFG_IMPORT_ROW, 1L);
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
		wxString& delimiter)
{
	wxString section;
	if (bImport)
		section = CFG_KEY_IMPORT;
	else
		section = CFG_KEY_EXPORT;
	delim = eDelimTab;
	delimiter.Empty();
	delim = wxConfig::Get()->Read(section + CFG_IMPORT_EXPORT_DELIM, delim);
	delimiter = wxConfig::Get()->Read(section + CFG_IMPORT_EXPORT_DELIMITER, delimiter);
	if (1 < delimiter.length())
		delimiter = delimiter.Left(1);
}


void CAgilityBookOptions::SetImportExportDelimiters(
		bool bImport,
		long delim,
		wxString const& delimiter)
{
	wxString section;
	if (bImport)
		section = CFG_KEY_IMPORT;
	else
		section = CFG_KEY_EXPORT;
	wxConfig::Get()->Write(section + CFG_IMPORT_EXPORT_DELIM, delim);
	wxConfig::Get()->Write(section + CFG_IMPORT_EXPORT_DELIMITER, delimiter);
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
	outFormat = static_cast<ARBDate::DateFormat>(wxConfig::Get()->Read(section + CFG_IMPORT_EXPORT_DATEFORMAT, static_cast<long>(ARBDate::eISO)));
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


static wxChar const* const GetColumnName(CAgilityBookOptions::ColumnOrder eOrder)
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
		wxString const& namedColumn,
		std::vector<long>& outValues,
		bool bDefaultValues)
{
	outValues.clear();
	if (!bDefaultValues)
	{
		wxString item;
		if (!namedColumn.empty())
			item << wxT('/') << namedColumn;
		item << GetColumnName(eOrder) << CFG_COL_BASENAME(idxColumn);
		wxString data = wxConfig::Get()->Read(item, wxEmptyString);
		int idx = data.Find(',');
		while (0 <= idx)
		{
			long val;
			tstringUtil::ToCLong(data, val, true);
			outValues.push_back(val);
			data = data.Mid(idx+1);
			idx = data.Find(',');
		}
		if (0 < data.length())
		{
			long val;
			tstringUtil::ToCLong(data, val, true);
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
#ifndef WIN32
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
		wxString const& namedColumn,
		std::vector<long> const& inValues)
{
	wxString data;
	for (size_t i = 0; i < inValues.size(); ++i)
	{
		if (0 < i)
			data << wxT(",");
		data << inValues[i];
	}
	wxString item;
	if (!namedColumn.empty())
		item << wxT('/') << namedColumn;
	item << GetColumnName(eOrder) << CFG_COL_BASENAME(idxColumn);
	wxConfig::Get()->Write(item, data);
}

/////////////////////////////////////////////////////////////////////////////

long CAgilityBookOptions::GetMRUFileCount()
{
	return wxConfig::Get()->Read(wxT("Settings/MRUsize"), 4L);
}


void CAgilityBookOptions::SetMRUFileCount(long nFiles)
{
	wxConfig::Get()->Write(wxT("Settings/MRUsize"), nFiles);
}


bool CAgilityBookOptions::GetAutoUpdateCheck()
{
	bool val = true;
#ifdef _DEBUG
	val = false;
#else
	wxConfig::Get()->Read(CFG_SETTINGS_AUTOCHECK, &val);
#endif
	return val;
}


void CAgilityBookOptions::SetAutoUpdateCheck(bool bSet)
{
	wxConfig::Get()->Write(CFG_SETTINGS_AUTOCHECK, bSet);
}


long CAgilityBookOptions::GetNumBackupFiles()
{
	return wxConfig::Get()->Read(CFG_SETTINGS_BACKUPFILES, 3L);
}


void CAgilityBookOptions::SetNumBackupFiles(long nFiles)
{
	wxConfig::Get()->Write(CFG_SETTINGS_BACKUPFILES, nFiles);
}


bool CAgilityBookOptions::AutoShowPropertiesOnNewTitle()
{
	bool val = false;
	wxConfig::Get()->Read(CFG_SETTINGS_AUTOSHOWTITLE, &val);
	return val;
}


void CAgilityBookOptions::AutoShowPropertiesOnNewTitle(bool bShow)
{
	wxConfig::Get()->Write(CFG_SETTINGS_AUTOSHOWTITLE, bShow);
}


bool CAgilityBookOptions::ShowHtmlPoints()
{
	bool val = true;
	wxConfig::Get()->Read(CFG_SETTINGS_SHOWHTML, &val);
	return val;
}


void CAgilityBookOptions::SetShowHtmlPoints(bool bSet)
{
	wxConfig::Get()->Write(CFG_SETTINGS_SHOWHTML, bSet);
}

/////////////////////////////////////////////////////////////////////////////

wxString CAgilityBookOptions::GetUserName(wxString const& hint)
{
	wxString section(CFG_KEY_USERNAMES);
	section += hint;
	return wxConfig::Get()->Read(section, wxString());
}


void CAgilityBookOptions::SetUserName(
		wxString const& hint,
		wxString const& userName)
{
	wxString section(CFG_KEY_USERNAMES);
	section += hint;
	wxConfig::Get()->Write(section, userName);
}


// Keeping use and proxy strings separate allow us to turn off the proxy
// without losing the settings.

bool CAgilityBookOptions::GetUseProxy()
{
	bool val = false;
	wxConfig::Get()->Read(CFG_SETTINGS_USEPROXY, &val);
	return val;
}


void CAgilityBookOptions::SetUseProxy(bool inUseProxy)
{
	wxConfig::Get()->Write(CFG_SETTINGS_USEPROXY, inUseProxy);
}


wxString CAgilityBookOptions::GetProxyServer()
{
	return wxConfig::Get()->Read(CFG_SETTINGS_PROXY, wxString());
}


void CAgilityBookOptions::SetProxyServer(wxString const& inProxy)
{
	wxConfig::Get()->Write(CFG_SETTINGS_PROXY, inProxy);
}


wxString CAgilityBookOptions::GetProxy()
{
	if (CAgilityBookOptions::GetUseProxy())
		return CAgilityBookOptions::GetProxyServer();
	return wxEmptyString;
}

/////////////////////////////////////////////////////////////////////////////

bool CAgilityBookOptions::IsCalSiteVisible(
		wxString const& filename,
		CVersionNum const& inVer)
{
	assert(inVer.Valid());
	if (filename.IsEmpty())
		return true;
	bool bVisible = true;
	wxString section(CFG_KEY_CALSITES);
	section += filename;
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
		wxString const& filename,
		bool bSuppress)
{
	if (filename.IsEmpty())
		return;
	wxString section(CFG_KEY_CALSITES);
	section += filename;
	wxConfig::Get()->Write(section, !bSuppress);
}


CVersionNum CAgilityBookOptions::GetCalSitePermanentStatus(wxString const& filename)
{
	CVersionNum ver(false);
	if (!filename.IsEmpty())
	{
		wxString section(CFG_KEY_CALSITES2);
		section += filename;
		wxString str = wxConfig::Get()->Read(section, wxString());
		if (!str.IsEmpty())
			ver.Parse(str);
	}
	return ver;
}


void CAgilityBookOptions::SuppressCalSitePermanently(
		wxString const& filename,
		CVersionNum const& inVer,
		bool bSuppress)
{
	if (filename.IsEmpty())
		return;
	wxString section(CFG_KEY_CALSITES2);
	section += filename;
	if (bSuppress)
		wxConfig::Get()->Write(section, inVer.GetVersionString());
	else
	{
		// If we're clearing one, make sure we haven't written a different version
		CVersionNum ver(false);
		wxString str = wxConfig::Get()->Read(section, wxString());
		if (!str.IsEmpty())
			ver.Parse(str);
		if (ver == inVer)
			wxConfig::Get()->DeleteEntry(section);
	}
}
