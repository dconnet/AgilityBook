#pragma once

/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @brief Utility classes.
 * @author David Connet
 *
 * Revision History
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-09-01 DRC Added 'operator+=' and 'operator-=' to ARBDouble.
 */

#include <string>
#include <vector>
class CElement;

/**
 * A version has a major and minor number.
 * Differences between major numbers prevents older versions of the program
 * from reading newer files. Differences in minor numbers allows an older
 * version to read the newer file, but warns that some information may be
 * lost if the file is saved.
 */
class ARBVersion
{
public:
	ARBVersion()
		: m_Version(0)
	{
	}

	ARBVersion(unsigned short major, unsigned short minor)
		: m_Version((major<<8) | minor)
	{
	}

	ARBVersion(const ARBVersion& inVer)
		: m_Version(inVer.m_Version)
	{
	}

	ARBVersion& operator=(const ARBVersion& rhs)
	{
		if (this != &rhs)
		{
			m_Version = rhs.m_Version;
		}
		return *this;
	}
	bool operator==(const ARBVersion& rhs) const
	{
		return m_Version == rhs.m_Version;
	}
	bool operator<(const ARBVersion& rhs) const
	{
		return m_Version < rhs.m_Version;
	}
	bool operator<=(const ARBVersion& rhs) const
	{
		return m_Version <= rhs.m_Version;
	}
	bool operator>(const ARBVersion& rhs) const
	{
		return m_Version > rhs.m_Version;
	}
	bool operator>=(const ARBVersion& rhs) const
	{
		return m_Version >= rhs.m_Version;
	}

	unsigned short Major() const
	{
		return static_cast<unsigned short>(m_Version >> 8);
	}
	unsigned short Minor() const
	{
		return static_cast<unsigned short>(m_Version & 0xff);
	}
	std::string ToString() const;

private:
	unsigned long m_Version;
};

/////////////////////////////////////////////////////////////////////////////
/**
 * Qualifying status of a run.
 * In XML/CElement, this class is only used as an attribute, never an element.
 * This serves as a translation mechanism from the string in the XML to
 * something we can use as a basic type.
 */
class ARB_Q
{
public:
	/**
	 * Types of Qs
	 */
	typedef enum
	{
		eQ_NA,		///< Cannot qualify in this run.
		eQ_Q,		///< Qualified.
		eQ_NQ,		///< Not qualified.
		eQ_E,		///< Eliminated.
		eQ_SuperQ,	///< Super Qualifier (USDAA Snooker top 15%).
	} eQ;

	// These functions map the above enum into strings.

	/**
	 * Get a single string listing all valid types.
	 */
	static std::string GetValidTypes();

	/**
	 * Get a list of all valid types.
	 */
	static void GetValidTypes(std::vector<std::string>& outTypes);

	/**
	 * Get the number of valid types.
	 */
	static int GetNumValidTypes();

	/**
	 * Get the valid type for a given index. Use this with GetNumValidTypes().
	 */
	static ARB_Q GetValidType(int inIndex);

	ARB_Q() : m_Q(eQ_NA)
	{
	}
	ARB_Q(const ARB_Q& rhs) : m_Q(rhs.m_Q)
	{
	}
	~ARB_Q()
	{
	}

	ARB_Q& operator=(const ARB_Q& rhs)
	{
		if (this != &rhs)
			m_Q = rhs.m_Q;
		return *this;
	}
	bool operator==(const ARB_Q& rhs) const
	{
		return m_Q == rhs.m_Q;
	}
	bool operator!=(const ARB_Q& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Is this a qualifying run?
	 */
	bool Qualified() const
	{
		return eQ_Q == m_Q || eQ_SuperQ == m_Q;
	}

	operator eQ() const
	{
		return m_Q;
	}

	/**
	 * Translate the enum value to a string
	 */
	std::string str() const;

	/**
	 * Load a Q
	 * @param inAttrib Name of attribute containing value to parse.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool Load(
		const std::string& inAttrib,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	/**
	 * Save a document.
	 * @param ioTree Element.
	 * @param inAttribName Name of attribute to write Q to.
	 * @return Success
	 * @post The ARB_Q attribute will be added to ioTree.
	 */
	bool Save(
		CElement& ioTree,
		const char* const inAttribName) const;

private:
	eQ m_Q;
};

/////////////////////////////////////////////////////////////////////////////
/**
 * Double class with some formatting.
 * In XML/CElement, this class is only used as an attribute, never an element.
 */
class ARBDouble
{
public:
	ARBDouble()
		: m_Prec(2)
		, m_Val(0.0)
	{
	}
	ARBDouble(double rhs)
		: m_Prec(2)
		, m_Val(rhs)
	{
	}
	ARBDouble(const ARBDouble& rhs)
		: m_Prec(rhs.m_Prec)
		, m_Val(rhs.m_Val)
	{
	}
	~ARBDouble()
	{
	}

	ARBDouble& operator=(const ARBDouble& rhs)
	{
		if (this != &rhs)
		{
			m_Prec = rhs.m_Prec;
			m_Val = rhs.m_Val;
		}
		return *this;
	}
	ARBDouble& operator+=(const ARBDouble& rhs)
	{
		m_Val += rhs.m_Val;
		return *this;
	}
	ARBDouble& operator-=(const ARBDouble& rhs)
	{
		m_Val -= rhs.m_Val;
		return *this;
	}
	bool operator==(const ARBDouble& rhs) const
	{
		return m_Prec == rhs.m_Prec
			&& m_Val == rhs.m_Val;
	}
	bool operator!=(const ARBDouble& rhs) const
	{
		return !operator==(rhs);
	}

	operator double() const
	{
		return m_Val;
	}

	/**
	 * Translate the double to a string with the desired precision.
	 */
	std::string str() const;

	/**
	 * Load a double
	 * @param inAttrib Name of attribute containing value to parse.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool Load(
		const std::string& inAttrib,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	/**
	 * Save a document.
	 * @param ioTree Element.
	 * @param inAttribName Name of attribute to write Q to.
	 * @return Success
	 * @post The double attribute will be added to ioTree.
	 */
	bool Save(
		CElement& ioTree,
		const char* const inAttribName) const;

private:
	int m_Prec;		///< Precision for display.
	double m_Val;	///< Actual value.
};
