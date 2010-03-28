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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-01-18 DRC Created empty file
 */

#include "stdafx.h"
#include "TestARB.h"

#include "ARBStructure.h"
#include "ARBDogNotes.h"
#include "Element.h"


SUITE(TestDogNotes)
{
	TEST(MetaData)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	static ARBMetaDataPtr MetaData();
		//	size_t length() const				{return m_Bytes;}
		//	unsigned char const* data() const	{return m_Data;}
		}
	}


	TEST(ctor)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogNotes();
		//	ARBDogNotes(ARBDogNotes const& rhs);
		}
	}


	TEST(OpEqual)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	ARBDogNotes& operator=(ARBDogNotes const& rhs);
		}
	}


	TEST(Compare)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool operator==(ARBDogNotes const& rhs) const;
		//	bool operator!=(ARBDogNotes const& rhs) const
		}
	}


	TEST(Load)
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


	TEST(Save)
	{
		if (!g_bMicroTest)
		{
			TODO_TEST
		//	bool Save(ElementNodePtr ioTree) const;
		}
	}
}
