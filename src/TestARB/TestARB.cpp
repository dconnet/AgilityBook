/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @author David Connet
 *
 * Revision History
 * 2019-08-15 wx3.1.2 (maybe earlier) has fixed GetExecutablePath on Mac cmdline
 * 2018-12-16 Convert to fmt.
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2015-11-27 Added test duration to verbose option.
 * 2013-08-18 Reuse Win/LanguageManager
 * 2012-03-16 Renamed LoadXML functions, added stream version.
 * 2011-11-17 Add ability to switch languages
 * 2011-01-22 Simplified how configs are added (all in TestConfig.cpp).
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-11 Created
 */

#include "stdafx.h"
#define CATCH_CONFIG_RUNNER
#include "TestARB.h"

#include "ConfigHandler.h"
#include "Local.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/ARBUtils.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "fmt/printf.h"
#include <iostream>
#include <stdexcept>

#if defined(__WXWINDOWS__)
#include "LibARBWin/LanguageManager.h"
#include <wx/app.h>
#include <wx/fs_arc.h>
#include <wx/fs_mem.h>
#include <wx/stdpaths.h>
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


bool g_bMicroTest = false;


class CLangManager
#if defined(__WXWINDOWS__)
	: public ILanguageCallback
#endif
{
	DECLARE_NO_COPY_IMPLEMENTED(CLangManager);
public:
	CLangManager(CLocalization& localization)
		: m_Localization(localization)
#if defined(__WXWINDOWS__)
		, m_langMgr(nullptr)
#else
		, m_CurLang(0)
#endif
	{
#if defined(__WXWINDOWS__)
		m_langMgr = std::make_unique<CLanguageManager>(this, true);
		m_langMgr->InitLanguage();
#endif
	}
	virtual ~CLangManager()
	{
	}

	bool SetLang(wxLanguage langId);

private:
	CLocalization& m_Localization;

#if defined(__WXWINDOWS__)
	// ILanguageCallback interface
	wxLanguage OnGetLanguage() const override
	{
		return m_langMgr->GetDefaultLanguage();
	}
	wxString OnGetCatalogName() const override
	{
		return m_langMgr->GetDefaultCatalogName();
	}
	wxString OnGetLangConfigName() const override
	{
		return wxEmptyString;
	}
	wxString OnGetLanguageDir() const override
	{
		return m_langMgr->GetDefaultLanguageDir();
	}
	void OnSetLanguage(wxLanguage langId) override
	{
		if (!m_Localization.Load())
		{
			std::string msg = fmt::format("ERROR: Unable to load '{}.mo'.", StringUtil::stringA(OnGetCatalogName().wx_str()));
			fmt::print(std::cerr, "{}\n", msg);
			throw std::runtime_error(msg);
		}
	}
	void OnErrorMessage(wxString const& msg) const override
	{
		fmt::print(std::wcerr, L"{}\n", msg.wx_str());
	}
	std::unique_ptr<CLanguageManager> m_langMgr;
#else // __WXWINDOWS__
	virtual void OnSetLanguage(int langId)
	{
	}
	int m_CurLang;
#endif
};
static CLangManager* g_LangMgr = nullptr;


bool CLangManager::SetLang(wxLanguage langId)
{
#if defined(__WXWINDOWS__)
	return m_langMgr->SetLang(langId);
#else
	if (langId == m_CurLang)
		return false;
	m_CurLang = langId;
	OnSetLanguage(m_CurLang);
	return true;
#endif
}


void SetLang(wxLanguage langId)
{
	if (g_LangMgr)
		g_LangMgr->SetLang(langId);
}


int main(int argc, char** argv)
{
#ifdef _WIN32
	_set_error_mode(_OUT_TO_MSGBOX);
#endif

	int ac = argc;
	bool bVerbose = false;
	if (argc >= 2)
	{
		for (int i = 1; i < argc; ++i)
		{
			if (0 == strcmp(argv[i], "verbose"))
			{
				bVerbose = true;
				continue;
			}
			if (0 == strcmp(argv[i], "micro"))
			{
				--ac;
				g_bMicroTest = true;
				continue;
			}
			//fprintf(stderr, "Usage: %s [micro] [verbose]\n", argv[0]);
			//return -1;
		}
	}

	bool bClean = false;
	char** av = argv;
	char verbose[3] = "-s";

	if (bVerbose || g_bMicroTest)
	{
		bClean = true;
		av = new char*[ac];
		av[0] = argv[0];
		int iNew = 1;
		for (int i = 1; i < argc; ++i)
		{
			if (0 == strcmp(argv[i], "verbose"))
				av[iNew++] = verbose;
			else if (0 == strcmp(argv[i], "micro"))
				continue;
			else
				av[iNew++] = argv[i];
		}
	}

#if defined(__WXWINDOWS__)
	wxInitializer initializer(ac, av);
#if defined(__WXMSW__)
	// By default, the path directories are tweaked to remove debug/release.
	// I assume my files are in the same location as the binary.
	// Now I don't need to tweak the wx source!
	wxStandardPaths::Get().DontIgnoreAppSubDir();
#endif
#endif

	std::wstring errs;
	if (!Element::Initialize(errs))
	{
		return 1;
	}

#if defined(__WXWINDOWS__)
	wxFileSystem::AddHandler(new wxArchiveFSHandler);
	wxFileSystem::AddHandler(new wxMemoryFSHandler);
#endif

	// Some names are 'funny' since it's cut/paste from ../Win/
	static CLocalization m_Localization;
	IARBLocalization::Init(&m_Localization);

	bool bRunTests = true;
#if defined(__WXWINDOWS__)
	g_LangMgr = new CLangManager(m_Localization);
	try
	{
		SetLang(wxLANGUAGE_ENGLISH_US);
	}
	catch (std::runtime_error const&)
	{
		bRunTests = false;
	}
#endif

	int rc = -1;
	if (bRunTests)
	{
		rc = Catch::Session().run(ac, av);
	}

	Element::Terminate();
	delete g_LangMgr;
	g_LangMgr = nullptr;

	if (bClean)
		delete[] av;

	return rc;
}


ElementNodePtr LoadXMLData(size_t id)
{
	fmt::wmemory_buffer errMsg;
	ARBErrorCallback err(errMsg);
	ElementNodePtr tree(ElementNode::New());
	assert(tree);
#if defined(__WXWINDOWS__)
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	std::wstring datafile = wxString(GetARBResourceDir() + wxFileName::GetPathSeparator() + fileName.GetName() + L".dat").wx_str();
#else
#pragma PRAGMA_TODO(write LoadXMLData)
#ifdef WIN32
	wchar_t fileName[MAX_PATH];
	GetModuleFileNameW(nullptr, fileName, _countof(fileName));
	std::wstring datafile(fileName);
	size_t n = datafile.find_last_of('.');
	datafile = datafile.substr(0, n);
	datafile += L".dat";
#else
	std::wstring datafile = L"./testarb.dat";
#endif
#endif
	assert(id < gc_NumConfigs);
	std::stringstream data;
	bool bOk = CConfigHandler::LoadWxFile(datafile, gc_Configs[id], data);
	assert(bOk);
	if (!bOk || !tree->LoadXML(data, errMsg))
	{
		fmt::print(std::wcout, L"{}\n", fmt::to_string(errMsg));
		tree.reset();
	}
	return tree;
}


bool LoadConfigFromTree(ElementNodePtr tree, ARBConfig& config)
{
	assert(tree);
	if (!tree)
		return false;
	assert(tree->GetName() == L"DefaultConfig");
	if (tree->GetName() != L"DefaultConfig")
		return false;
	ARBVersion version;
	tree->GetAttrib(ATTRIB_BOOK_VERSION, version);
	int idx = tree->FindElement(TREE_CONFIG);
	assert(0 <= idx);
	fmt::wmemory_buffer errMsg;
	ARBErrorCallback err(errMsg);
	return config.Load(tree->GetElementNode(idx), version, err);
}


ElementNodePtr CreateActionList()
{
	static char const* const configData =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<RootNode>\n\
<Action NewName=\"SSA\" OldName=\"Sweepstakes\" Verb=\"RenameVenue\" />\n\
<Action NewName=\"MXF\" OldName=\"MF\" Venue=\"AKC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"ATCH\" OldName=\"V-ATCH\" Venue=\"ASCA\" Verb=\"DeleteTitle\" />\n\
<Action NewName=\"ATCH-OP\" OldName=\"V-ATCH-OP\" Venue=\"ASCA\" Verb=\"DeleteTitle\" />\n\
<Action NewName=\"ATCH-SP\" OldName=\"V-ATCH-SP\" Venue=\"ASCA\" Verb=\"DeleteTitle\" />\n\
<Action NewName=\"Regular\" OldName=\"Standard\" Venue=\"CPE\" Verb=\"RenameDivision\" />\n\
<Action NewName=\"Regular\" OldName=\"Junior\" Venue=\"CPE\" Verb=\"RenameDivision\" />\n\
<Action NewName=\"UAGI\" OldName=\"UAgI\" Venue=\"UKC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"UAGII\" OldName=\"UAgII\" Venue=\"UKC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"UAGIII\" OldName=\"UAgIII\" Venue=\"UKC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"ADCH\" OldName=\"ADCh\" Venue=\"USDAA\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"PD1\" OldName=\"PDI\" Venue=\"USDAA\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"PD2\" OldName=\"PDII\" Venue=\"USDAA\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"PD3\" OldName=\"PDIII\" Venue=\"USDAA\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"PJ3\" OldName=\"PJ\" Venue=\"USDAA\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"PG3\" OldName=\"PG\" Venue=\"USDAA\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"PK3\" OldName=\"PS\" Venue=\"USDAA\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"PJ3\" OldName=\"PJ\" Venue=\"USDAA\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Novice Triple Superior Performance Dog\" OldName=\"Triple Superior Performance Dog\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Triple Superior-Novice\" OldName=\"Novice Triple Superior Performance Dog\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Triple Superior-Open\" OldName=\"Open Triple Superior Performance Dog\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Triple Superior-Elite\" OldName=\"Elite Triple Superior Performance Dog\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Triple Triple\" OldName=\"Triple Triple Superior Performance\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Vers-NATCH\" OldName=\"Versatility NATCH\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"O-Vers-NATCH\" OldName=\"Outstanding Versatility NATCH\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Vers-NATCH\" OldName=\"O-Vers-NATCH\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"S-Vers-NATCH\" OldName=\"Superior Versatility NATCH\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Vers-NATCH\" OldName=\"S-Vers-NATCH\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"NADAC Medal\" OldName=\"Bronze Medal\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"NADAC Medal\" OldName=\"Silver Medal\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"NADAC Medal\" OldName=\"Gold Medal\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"NATCH\" OldName=\"O-NATCH\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"NATCH\" OldName=\"S-NATCH\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Platinum Speed Star\" OldName=\"Platinum Medal\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Platinum Versatility Speed Star\" OldName=\"Platinum Versatility Medal\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Purple Achievement Cup\" OldName=\"Purple Medal\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action NewName=\"Purple Versatility Achievement Cup\" OldName=\"Purple Versatility Medal\" Venue=\"NADAC\" Verb=\"RenameTitle\" />\n\
<Action OldName=\"NAS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"NJS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"NGS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"TNS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"TGS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"WVS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-NAS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-NJS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-NGS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-TNS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-TGS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-WVS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-NAS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-NJS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-NGS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-TNS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-TGS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-WVS-N\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"OAS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"OJS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"OGS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"TNS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"TGS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"WVS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-OAS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-OJS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-OGS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-TNS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-TGS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"O-WVS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-OAS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-OJS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-OGS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-TNS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-TGS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"S-WVS-O\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"CHC\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"CHS\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"Outstanding Novice Versatility\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"Outstanding Open Versatility\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action OldName=\"Outstanding Elite Versatility\" Venue=\"NADAC\" Verb=\"DeleteTitle\" />\n\
<Action NewName=\"Chances\" OldName=\"Wild Card\" Venue=\"NADAC\" Verb=\"RenameEvent\" />\n\
<Action OldName=\"Championship\" Venue=\"NADAC\" Verb=\"DeleteEvent\" />\n\
</RootNode>";

	ElementNodePtr actions = ElementNode::New();
	fmt::wmemory_buffer errmsg;
	bool bParse = actions->LoadXML(configData, static_cast<unsigned int>(strlen(configData)), errmsg);
	if (!bParse)
	{
		fmt::print(std::wcout, L"{}\n", fmt::to_string(errmsg));
	}
	assert(bParse);
	return actions;
}
