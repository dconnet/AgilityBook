/*
 * Copyright (c) 2008-2009 David Connet. All Rights Reserved.
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
 * @brief Main access (in non-mfc dll, we'd have the DllMain here)
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-11 DRC Created
 */

#include "stdafx.h"
#include "TestARB.h"
#include "Local.h"

#include "ARBConfig.h"
#include "ARBStructure.h"
#include "ConfigHandler.h"
#include "Element.h"
#include <wx/app.h>
#include <wx/filesys.h>
#include <wx/fs_zip.h>
#include <wx/stdpaths.h>
#include "TestReporterStdout.h"

#ifdef _MSC_VER
#if _MSC_VER >= 1300 && _MSC_VER < 1400
#define UT_NAME			"UnitTest++.VC7"
#elif _MSC_VER >= 1400 && _MSC_VER < 1500
#define UT_NAME			"UnitTest++.VC8"
#elif _MSC_VER >= 1500
#define UT_NAME			"UnitTest++.VC9"
#else
#error Unsupported compiler
#endif
#endif

// Note, if using static runtime, swap comments on following 2 defines
// I don't know if there's a compiler define that tells us which runtime
// we're linking with...
#define UT_STATIC_RTL	"S"
//#define UT_STATIC_RTL	""

#ifdef _UNICODE
#define UT_UNICODE		"U"
#else
#define UT_UNICODE		""
#endif

#ifdef _DEBUG
#define UT_DEBUG		"D"
#else
#define UT_DEBUG		""
#endif

#pragma comment(lib, UT_NAME UT_STATIC_RTL UT_UNICODE UT_DEBUG ".lib")


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


int main(int argc, char** argv)
{
#ifdef _WIN32
	_set_error_mode(_OUT_TO_MSGBOX);
#endif
	bool bVerbose = false;
	if (argc >= 2)
	{
		if (0 == strcmp(argv[1], "verbose"))
			bVerbose = true;
		else
		{
			fprintf(stderr, "Usage: %s [verbose]\n", argv[0]);
		}
	}

	wxInitializer initializer;

	wxString errs;
	if (!Element::Initialize(errs))
	{
		return 1;
	}

	wxFileSystem::AddHandler(new wxZipFSHandler);

	// Some names are 'funny' since it's cut/paste from ../Win/
	static CLocalization m_Localization;
	IARBLocalization::Init(&m_Localization);

#ifdef __WXMAC__
	// Command line programs on Mac are acting like unix. GetResourcesDir
	// returns /usr/local/share. And GetExecutablePath is returning nothing.
	wxString m_dirLang = wxT("./lang");
#else
	wxString m_dirLang = wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + wxT("lang");
#endif

	wxLocale* m_locale = new wxLocale();
	m_locale->Init(wxLANGUAGE_ENGLISH_US, wxLOCALE_CONV_ENCODING);
	m_locale->AddCatalogLookupPathPrefix(m_dirLang);
	m_locale->AddCatalog(wxT("arb"), wxLANGUAGE_USER_DEFINED, wxEmptyString);
	m_Localization.Load();

	CReporterVerbose reporter(bVerbose);
	UnitTest::TestRunner runner(reporter);
	int rc = runner.RunTestsIf(UnitTest::Test::GetTestList(), NULL, UnitTest::True(), 0);

	Element::Terminate();

	return rc;
}


// When adding a new config:
//  - Create ID in TestARB.h
//  - Add case stmt here
//  - Add test case in TestConfig.cpp
// RunARBTests.py will automatically pick up all XML files in ./res/
ElementNodePtr LoadXMLData(int id)
{
	wxString errMsg;
	ARBErrorCallback err(errMsg);
	ElementNodePtr tree(ElementNode::New());
	assert(tree);
#ifdef __WXMAC__
	// Command line programs on Mac are acting like unix. GetResourcesDir
	// returns /usr/local/share. And GetExecutablePath is returning nothing.
	wxString datafile = wxT("./testarb.dat");
#else
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	wxString datafile = wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + fileName.GetName() + wxT(".dat");
#endif
	bool bOk = false;
	std::string data;
	switch (id)
	{
	default:
		assert(0);
		// fallthru
	case IDR_XML_DEFAULT_CONFIG:
		bOk = CConfigHandler::LoadWxFile(datafile, wxT("DefaultConfig.xml"), data);
		break;
	case IDR_XML_CONFIG08_V10_2:
		bOk = CConfigHandler::LoadWxFile(datafile, wxT("Config08_v10_2.xml"), data);
		break;
	case IDR_XML_CONFIG09_V11_0:
		bOk = CConfigHandler::LoadWxFile(datafile, wxT("Config09_v11_0.xml"), data);
		break;
	case IDR_XML_CONFIG12_V12_1:
		bOk = CConfigHandler::LoadWxFile(datafile, wxT("Config12_v12_1.xml"), data);
		break;
	case IDR_XML_CONFIG14_V12_2:
		bOk = CConfigHandler::LoadWxFile(datafile, wxT("Config14_v12_2.xml"), data);
		break;
	case IDR_XML_CONFIG19_V12_5:
		bOk = CConfigHandler::LoadWxFile(datafile, wxT("Config19_v12_5.xml"), data);
		break;
	case IDR_XML_CONFIG20_V12_6:
		bOk = CConfigHandler::LoadWxFile(datafile, wxT("Config20_v12_6.xml"), data);
		break;
	case IDR_XML_CONFIG21_V12_7:
		bOk = CConfigHandler::LoadWxFile(datafile, wxT("Config21_v12_7.xml"), data);
		break;
	case IDR_XML_CONFIG22_V12_7:
		bOk = CConfigHandler::LoadWxFile(datafile, wxT("Config22_v12_7.xml"), data);
		break;
	case IDR_XML_CONFIG23_V12_8:
		bOk = CConfigHandler::LoadWxFile(datafile, wxT("Config23_v12_8.xml"), data);
		break;
	case IDR_XML_CONFIG24_V12_8:
		bOk = CConfigHandler::LoadWxFile(datafile, wxT("Config24_v12_8.xml"), data);
		break;
	case IDR_XML_CONFIG25_V12_9:
		bOk = CConfigHandler::LoadWxFile(datafile, wxT("Config25_v12_9.xml"), data);
		break;
	case IDR_XML_CONFIG26_V12_9:
		bOk = CConfigHandler::LoadWxFile(datafile, wxT("Config26_v12_9.xml"), data);
		break;
	case IDR_XML_CONFIG27_V12_10:
		bOk = CConfigHandler::LoadWxFile(datafile, wxT("Config27_v12_10.xml"), data);
		break;
	}
	assert(bOk);
	if (!bOk || !tree->LoadXMLBuffer(data.c_str(), data.length(), errMsg))
	{
		wxLogError(wxT("%s"), errMsg.c_str());
		tree.reset();
	}
	return tree;
}


bool LoadConfigFromTree(ElementNodePtr tree, ARBConfig& config)
{
	assert(tree);
	if (!tree)
		return false;
	assert(tree->GetName() == wxT("DefaultConfig"));
	if (tree->GetName() != wxT("DefaultConfig"))
		return false;
	ARBVersion version;
	tree->GetAttrib(ATTRIB_BOOK_VERSION, version);
	int idx = tree->FindElement(TREE_CONFIG);
	assert(0 <= idx);
	wxString errMsg;
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
	wxString errmsg;
	bool bParse = actions->LoadXMLBuffer(configData, static_cast<unsigned int>(strlen(configData)), errmsg);
	if (!bParse)
	{
		wxLogError(wxT("%s"), errmsg.c_str());
	}
	assert(bParse);
	return actions;
}
