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
 * @li 2012-03-16 DRC Renamed LoadXML functions, added stream version.
 * @li 2011-11-17 DRC Add ability to switch languages
 * @li 2011-01-22 DRC Simplified how configs are added (all in TestConfig.cpp).
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-11 DRC Created
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
#include <wx/app.h>
#include <wx/filesys.h>
#include <wx/fs_zip.h>
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


class CLanguageManager
{
	DECLARE_NO_COPY_IMPLEMENTED(CLanguageManager)
public:
	CLanguageManager(CLocalization& localization)
		: m_Localization(localization)
		, m_dirLang()
#if defined(__WXWINDOWS__)
		, m_CurLang(wxLANGUAGE_DEFAULT)
		, m_locale(NULL)
#else
		, m_CurLang(0)
#endif
	{
#if defined(__WXWINDOWS__)
#ifdef __WXMAC__
		// Command line programs on Mac are acting like unix. GetResourcesDir
		// returns /usr/local/share. And GetExecutablePath is returning nothing.
		m_dirLang = L"./lang";
#else
		m_dirLang = wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + L"lang";
#endif
#endif
	}
	~CLanguageManager()
	{
#if defined(__WXWINDOWS__)
		delete m_locale;
#endif
	}

	bool SetLang(int langId);

	CLocalization& m_Localization;
	std::wstring m_dirLang;
	int m_CurLang;
#if defined(__WXWINDOWS__)
	wxLocale* m_locale;
#endif
};
static CLanguageManager* g_LangMgr = NULL;


bool CLanguageManager::SetLang(int langId)
{
	if (langId == m_CurLang)
		return false;

#if defined(__WXWINDOWS__)
	m_CurLang = langId;
	if (m_locale)
		delete m_locale;
	m_locale = new wxLocale();
	m_locale->AddCatalogLookupPathPrefix(StringUtil::stringWX(m_dirLang));
#if wxCHECK_VERSION(2, 9, 5)
	if (!m_locale->Init(m_CurLang, wxLOCALE_DONT_LOAD_DEFAULT))
#else

	if (!m_locale->Init(m_CurLang, wxLOCALE_CONV_ENCODING))
#endif
	{
		//return false;
	}
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	m_locale->AddCatalog(fileName.GetName(), wxLANGUAGE_USER_DEFINED, wxEmptyString);

	if (!m_Localization.Load())
	{
		wxString str = wxString::Format(wxT("ERROR: Unable to load '%s.mo'."), fileName.GetName().c_str());
		std::string msg(str.ToAscii());
		std::cerr << msg << "\n";
		throw std::runtime_error(msg);
	}

	return true;

#else
	return false;
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
		}
	}

#if defined(__WXWINDOWS__)
	wxInitializer initializer(argc, argv);
#if defined(__WXMSW__) && wxCHECK_VERSION(2, 9, 5)
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
	wxFileSystem::AddHandler(new wxZipFSHandler);
#endif

	// Some names are 'funny' since it's cut/paste from ../Win/
	static CLocalization m_Localization;
	IARBLocalization::Init(&m_Localization);

	bool bRunTests = true;
#if defined(__WXWINDOWS__)
	g_LangMgr = new CLanguageManager(m_Localization);
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
		rc = runner.RunTestsIf(UnitTest::Test::GetTestList(), NULL, UnitTest::True(), 0);
	}

	Element::Terminate();
	delete g_LangMgr;
	g_LangMgr = NULL;

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
	std::wstring datafile = L"./testarb.dat";
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
