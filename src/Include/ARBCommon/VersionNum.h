#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the VersionNum class
 * @author David Connet
 *
 * Revision History
 * 2018-08-15 Changed VERSION_NUMBER to std::array
 * 2012-04-10 Based on wx-group thread, use std::string for internal use
 * 2009-01-28 Removed Windows VERSIONNUM support (use VersionNumber.h)
 * 2009-01-01 Ported to wxWidgets.
 * 2007-10-24 Changed ctor and added <= operators
 * 2004-03-04 Created
 */

#include <array>
#include <string>


class CVersionNum
{
public:
	typedef std::array<unsigned short, 4> VERSION_NUMBER;

	CVersionNum(bool bAutoLoad);
	CVersionNum(CVersionNum const& rhs);
	CVersionNum& operator=(CVersionNum const& rhs);

	/**
	 * Parse a version number, must be in form "n.n.n.n".
	 */
	bool Parse(std::wstring inVer);

	void Assign(
			unsigned short inMajor,
			unsigned short inMinor,
			unsigned short inDot,
			unsigned short inBuild);

	/**
	 * Equality is based solely on the version number, not any language aspects.
	 */
	bool operator==(CVersionNum const& rhs) const;
	bool operator<(CVersionNum const& rhs) const;
	bool operator<=(CVersionNum const& rhs) const;
	bool operator>(CVersionNum const& rhs) const;
	bool operator>=(CVersionNum const& rhs) const;

	void clear();
	bool Valid() const						{return m_Valid;}
	std::wstring GetVersionString() const;
	void GetVersion(VERSION_NUMBER& outVer) const;

private:
	bool m_Valid;
	VERSION_NUMBER m_Version;
};
