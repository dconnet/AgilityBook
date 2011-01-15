/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CVersionNum class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-01-28 DRC Removed Windows VERSIONNUM support (use VersionNumber.h)
 * @li 2009-01-01 DRC Ported to wxWidgets.
 * @li 2007-10-24 DRC Changed ctor and added <= operators
 * @li 2006-12-10 DRC Fixed operator< and operator>.
 * @li 2004-03-04 DRC Created
 */

#include "stdafx.h"
#include "VersionNum.h"

#include "ARBTypes.h"
#include "VersionNumber.h"

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


bool CVersionNum::Parse(wxString inVer)
{
	clear();
	int pos = inVer.Find('.');
	if (0 <= pos)
	{
		m_Version.part1 = static_cast<unsigned short>(tstringUtil::ToCLong(inVer));
		inVer = inVer.Mid(pos+1);
		pos = inVer.Find('.');
		if (0 <= pos)
		{
			m_Version.part2 = static_cast<unsigned short>(tstringUtil::ToCLong(inVer));
			inVer = inVer.Mid(pos+1);
			pos = inVer.Find('.');
			if (0 <= pos)
			{
				m_Version.part3 = static_cast<unsigned short>(tstringUtil::ToCLong(inVer));
				inVer = inVer.Mid(pos+1);
				m_Version.part4 = static_cast<unsigned short>(tstringUtil::ToCLong(inVer));
				m_Valid = true;
			}
		}
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


wxString CVersionNum::GetVersionString() const
{
	wxString str;
	str << m_Version.part1
		<< wxT(".") << m_Version.part2
		<< wxT(".") << m_Version.part3
		<< wxT(".") << m_Version.part4;
	return str;
}


void CVersionNum::GetVersion(VERSION_NUMBER& outVer) const
{
	outVer = m_Version;
}
