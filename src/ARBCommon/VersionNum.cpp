/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CVersionNum class
 * @author David Connet
 *
 * Revision History
 * 2012-07-28 Crap, v2.3.6 was hosed. Fix version parsing.
 * 2012-04-10 Based on wx-group thread, use std::string for internal use
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-01-28 Removed Windows VERSIONNUM support (use VersionNumber.h)
 * 2009-01-01 Ported to wxWidgets.
 * 2007-10-24 Changed ctor and added <= operators
 * 2006-12-10 Fixed operator< and operator>.
 * 2004-03-04 Created
 */

#include "stdafx.h"
#include "ARBCommon/VersionNum.h"

#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/BreakLine.h"
#include "ARBCommon/StringUtil.h"
#include "VersionNumber.h"
#include <sstream>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CVersionNum::CVersionNum(bool bAutoLoad)
	: m_Valid(false)
	, m_Version()
{
	if (bAutoLoad)
	{
		m_Valid = true;
		m_Version.part1 = ARB_VER_MAJOR;
		m_Version.part2 = ARB_VER_MINOR;
		m_Version.part3 = ARB_VER_DOT;
		m_Version.part4 = ARB_VER_BUILD;
	}
}


CVersionNum::CVersionNum(CVersionNum const& rhs)
	: m_Valid(rhs.m_Valid)
	, m_Version(rhs.m_Version)
{
}


CVersionNum& CVersionNum::operator=(CVersionNum const& rhs)
{
	if (this != &rhs)
	{
		m_Valid = rhs.m_Valid;
		m_Version = rhs.m_Version;
	}
	return *this;
}


bool CVersionNum::Parse(std::wstring inVer)
{
	clear();
	std::vector<std::wstring> fields;
	if (4 == BreakLine(L'.', inVer, fields))
		m_Valid = true;
	unsigned short* parts[4] = {
		&m_Version.part1,
		&m_Version.part2,
		&m_Version.part3,
		&m_Version.part4
	};
	for (size_t i = 0; i < fields.size(); ++i)
	{
		*(parts[i]) = static_cast<unsigned short>(StringUtil::ToCLong(fields[i]));
	}
	return m_Valid;
}


void CVersionNum::Assign(
		unsigned short inMajor,
		unsigned short inMinor,
		unsigned short inDot,
		unsigned short inBuild)
{
	m_Valid = true;
	m_Version.part1 = inMajor;
	m_Version.part2 = inMinor;
	m_Version.part3 = inDot;
	m_Version.part4 = inBuild;
}


// Equality is based solely on the version number, not any language aspects.
bool CVersionNum::operator==(CVersionNum const& rhs) const
{
	return m_Version.part1 == rhs.m_Version.part1
		&& m_Version.part2 == rhs.m_Version.part2
		&& m_Version.part3 == rhs.m_Version.part3
		&& m_Version.part4 == rhs.m_Version.part4;
}


bool CVersionNum::operator<(CVersionNum const& rhs) const
{
	if (m_Version.part1 < rhs.m_Version.part1
	|| (m_Version.part1 == rhs.m_Version.part1
		&& m_Version.part2 < rhs.m_Version.part2)
	|| (m_Version.part1 == rhs.m_Version.part1
		&& m_Version.part2 == rhs.m_Version.part2
		&& m_Version.part3 < rhs.m_Version.part3)
	|| (m_Version.part1 == rhs.m_Version.part1
		&& m_Version.part2 == rhs.m_Version.part2
		&& m_Version.part3 == rhs.m_Version.part3
		&& m_Version.part4 < rhs.m_Version.part4))
	{
		return true;
	}
	return false;
}


bool CVersionNum::operator<=(CVersionNum const& rhs) const
{
	return operator==(rhs) || rhs.operator>(*this);
}


bool CVersionNum::operator>(CVersionNum const& rhs) const
{
	if (m_Version.part1 > rhs.m_Version.part1
	|| (m_Version.part1 == rhs.m_Version.part1
		&& m_Version.part2 > rhs.m_Version.part2)
	|| (m_Version.part1 == rhs.m_Version.part1
		&& m_Version.part2 == rhs.m_Version.part2
		&& m_Version.part3 > rhs.m_Version.part3)
	|| (m_Version.part1 == rhs.m_Version.part1
		&& m_Version.part2 == rhs.m_Version.part2
		&& m_Version.part3 == rhs.m_Version.part3
		&& m_Version.part4 > rhs.m_Version.part4))
	{
		return true;
	}
	return false;
}


bool CVersionNum::operator>=(CVersionNum const& rhs) const
{
	return operator==(rhs) || rhs.operator<(*this);
}


void CVersionNum::clear()
{
	m_Valid = false;
	m_Version.part1 = m_Version.part2 = m_Version.part3 = m_Version.part4 = 0;
}


std::wstring CVersionNum::GetVersionString() const
{
	std::wostringstream str;
	str << m_Version.part1
		<< L"." << m_Version.part2
		<< L"." << m_Version.part3
		<< L"." << m_Version.part4;
	return str.str();
}


void CVersionNum::GetVersion(VERSION_NUMBER& outVer) const
{
	outVer = m_Version;
}
