/*
 * Copyright (c) 2004-2009 David Connet. All Rights Reserved.
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
		m_Version.part1 = static_cast<unsigned short>(tstringUtil::atol(inVer));
		inVer = inVer.Mid(pos+1);
		pos = inVer.Find('.');
		if (0 <= pos)
		{
			m_Version.part2 = static_cast<unsigned short>(tstringUtil::atol(inVer));
			inVer = inVer.Mid(pos+1);
			pos = inVer.Find('.');
			if (0 <= pos)
			{
				m_Version.part3 = static_cast<unsigned short>(tstringUtil::atol(inVer));
				inVer = inVer.Mid(pos+1);
				m_Version.part4 = static_cast<unsigned short>(tstringUtil::atol(inVer));
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
