/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test Element class
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2017-08-03 Added basic read verification
 * 2012-03-16 Renamed LoadXML functions, added stream version.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-12-27 Replace local LoadTree with LoadXMLData
 * 2008-01-11 Created
 */

#include "stdafx.h"
#include "TestLib.h"

#include "ARBCommon/Element.h"
#include "TestARB.h"

#if defined(__WXWINDOWS__)
#include <wx/filefn.h>
#include <wx/mstream.h>
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBCommon;

TEST_CASE("Element")
{
	SECTION("Save")
	{
		ElementNodePtr tree = LoadXMLData();

		wxString tmpFile(L"data.tmp");
		std::stringstream tmp1;
		REQUIRE(tree->SaveXML(tmpFile));
		REQUIRE(tree->SaveXML(tmp1));

		ElementNodePtr tree2(ElementNode::New());
		wxString errs;
		REQUIRE(tree2->LoadXML(tmpFile, errs));

#if defined(__WXWINDOWS__)
		wxRemoveFile(tmpFile);
#else
#pragma PRAGMA_TODO(remove file)
#endif

		std::stringstream tmp2;
		REQUIRE(tree2->SaveXML(tmp2));

		std::string tmp1data = tmp1.str();
		std::string tmp2data = tmp2.str();
		REQUIRE(tmp1data == tmp2data);
	}
}

} // namespace dconSoft
