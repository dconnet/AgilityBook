/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Test ARBDogNotes class.
 * @author David Connet
 *
 * Revision History
 * 2017-11-09 Convert from UnitTest++ to Catch
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-01-18 Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARB/ARBStructure.h"
#include "ARB/ARBDogNotes.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


TEST_CASE("DogNotes")
{
	SECTION("MetaData")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBMetaDataPtr MetaData();
		//	size_t length() const				{return m_Bytes;}
		//	unsigned char const* data() const	{return m_Data;}
		}
	}


	SECTION("ctor")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogNotes();
		//	ARBDogNotes(ARBDogNotes const& rhs);
		}
	}


	SECTION("OpEqual")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogNotes& operator=(ARBDogNotes const& rhs);
		}
	}


	SECTION("Compare")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBDogNotes const& rhs) const;
		//	bool operator!=(ARBDogNotes const& rhs) const
		}
	}


	SECTION("Load")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Load(
		//			ARBConfig const& inConfig,
		//			ElementNodePtr inTree,
		//			ARBVersion const& inVersion,
		//			ARBErrorCallback& ioCallback);
		}
	}


	SECTION("Save")
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Save(ElementNodePtr ioTree) const;
		}
	}
}
