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
 * 2019-10-13 Separated ARB specific things from TestLib.
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
#include "TestLib.h"

#include "Local.h"
#include "TestARB.h"

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
			std::string msg = fmt::format("ERROR: Unable to load '{}.mo'.", OnGetCatalogName().ToStdString());
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
