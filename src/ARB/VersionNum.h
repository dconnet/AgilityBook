#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the VersionNum class
 * @author David Connet
 *
 * Revision History
 * @li 2012-04-10 DRC Based on wx-group thread, use std::string for internal use
 * @li 2009-01-28 DRC Removed Windows VERSIONNUM support (use VersionNumber.h)
 * @li 2009-01-01 DRC Ported to wxWidgets.
 * @li 2007-10-24 DRC Changed ctor and added <= operators
 * @li 2004-03-04 DRC Created
 */

#include <string>


class CVersionNum
{
public:
	typedef struct
	{
		unsigned short part1;
		unsigned short part2;
		unsigned short part3;
		unsigned short part4;
	} VERSION_NUMBER;

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
