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
 * 2013-08-18 Reuse Win/LanguageManager
 * 2012-03-16 Renamed LoadXML functions, added stream version.
 * 2011-11-17 Add ability to switch languages
 * 2011-01-22 Simplified how configs are added (all in TestConfig.cpp).
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-11 Created
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ConfigHandler.h"
#include "Local.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "TestReporterStdout.h"
#include <iostream>
#include <stdexcept>

#if defined(__WXWINDOWS__)
#include "../Win/LanguageManager.h"
#include <wx/app.h>
#include <wx/fs_arc.h>
#include <wx/stdpaths.h>
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


class CReporterVerbose : public UnitTest::TestReporterStdout
{
public:
	CReporterVerbose(bool bVerbose)
		: m_bVerbose(bVerbose)
	{}
	virtual void ReportTestStart(UnitTest::TestDetails const& test);
	virtual void ReportTestFinish(UnitTest::TestDetails const& test, float secondsElapsed);
private:
	bool m_bVerbose;
};


void CReporterVerbose::ReportTestStart(UnitTest::TestDetails const& test)
{
	if (m_bVerbose)
		printf("%s:%d: %s:%s started\n", test.filename, test.lineNumber, test.suiteName, test.testName);
}


void CReporterVerbose::ReportTestFinish(UnitTest::TestDetails const& test, float /*secondsElapsed*/)
{
	if (m_bVerbose)
		printf("%s:%d: %s:%s finished\n", test.filename, test.lineNumber, test.suiteName, test.testName);
}


bool g_bMicroTest = false;


class CLangManager
#if defined(__WXWINDOWS__)
	: public ILanguageCallback
#endif
{
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
		m_langMgr = new CLanguageManager(this);
		m_langMgr->InitLocale();
#endif
	}
	~CLangManager()
	{
#if defined(__WXWINDOWS__)
		delete m_langMgr;
#endif
	}

	bool SetLang(int langId);

private:
	CLocalization& m_Localization;

#if defined(__WXWINDOWS__)
	// ILanguageCallback interface
	virtual int OnGetLanguage() const
	{
		return m_langMgr->GetDefaultLanguage();
	}
	virtual wxString OnGetCatalogName() const
	{
		return m_langMgr->GetDefaultCatalogName();
	}
	virtual wxString OnGetLangConfigName() const
	{
		return wxEmptyString;
	}
	virtual wxString OnGetLanguageDir() const
	{
#ifdef __WXMAC__
		// Command line programs on Mac are acting like unix. GetResourcesDir
		// returns /usr/local/share. And GetExecutablePath is returning nothing.
		return wxT("./lang");
#else
		return m_langMgr->GetDefaultLanguageDir();
#endif
	}
	virtual void OnSetLanguage(int langId)
	{
		if (!m_Localization.Load())
		{
#if defined(__WXWINDOWS__)
			wxString str = wxString::Format(wxT("ERROR: Unable to load '%s.mo'."), OnGetCatalogName().c_str());
			std::string msg(str.ToAscii());
#else
			std::string msg("ERROR: Unable to load localization");
#endif
			std::cerr << msg << "\n";
			throw std::runtime_error(msg);
		}
	}
	virtual void OnErrorMessage(wxString const& msg) const
	{
		std::wcerr << msg.wx_str() << std::endl;
	}
	CLanguageManager* m_langMgr;
#else // __WXWINDOWS__
	virtual void OnSetLanguage(int langId)
	{
	}
	int m_CurLang;
#endif
private:
	CLangManager(CLangManager const&);
	CLangManager& operator=(CLangManager const&);
};
static CLangManager* g_LangMgr = nullptr;


bool CLangManager::SetLang(int langId)
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


void SetLang(int langId)
{
	if (g_LangMgr)
		g_LangMgr->SetLang(langId);
}


int main(int argc, char** argv)
{
#ifdef _WIN32
	_set_error_mode(_OUT_TO_MSGBOX);
#endif
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
				g_bMicroTest = true;
				continue;
			}
			fprintf(stderr, "Usage: %s [micro] [verbose]\n", argv[0]);
			return -1;
		}
	}

#if defined(__WXWINDOWS__)
	wxInitializer initializer(argc, argv);
#if defined(__WXMSW__) && wxCHECK_VERSION(3, 0, 0)
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
		CReporterVerbose reporter(bVerbose);
		UnitTest::TestRunner runner(reporter);
		rc = runner.RunTestsIf(UnitTest::Test::GetTestList(), nullptr, UnitTest::True(), 0);
	}

	Element::Terminate();
	delete g_LangMgr;
	g_LangMgr = nullptr;

	return rc;
}


ElementNodePtr LoadXMLData(size_t id)
{
	std::wostringstream errMsg;
	ARBErrorCallback err(errMsg);
	ElementNodePtr tree(ElementNode::New());
	assert(tree);
#if defined(__WXWINDOWS__)
#ifdef __WXMAC__
	// Command line programs on Mac are acting like unix. GetResourcesDir
	// returns /usr/local/share. And GetExecutablePath is returning nothing.
	std::wstring datafile = L"./testarb.dat";
#else
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	std::wstring datafile = wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + fileName.GetName() + L".dat";
#endif
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
		std::wcout << errMsg.str() << std::endl;
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
	std::wostringstream errMsg;
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
	std::wostringstream errmsg;
	bool bParse = actions->LoadXML(configData, static_cast<unsigned int>(strlen(configData)), errmsg);
	if (!bParse)
	{
		std::wcout << errmsg.str() << std::endl;
	}
	assert(bParse);
	return actions;
}
