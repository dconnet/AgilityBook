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
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-04-06 DRC Added op< to ARB_Q.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-09-01 DRC Added 'operator+=' and 'operator-=' to ARBDouble.
 */

#include <string>
#include <vector>
class ARBErrorCallback;
class Element;

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

	ARBVersion(ARBVersion const& inVer)
		: m_Version(inVer.m_Version)
	{
	}

	ARBVersion& operator=(ARBVersion const& rhs)
	{
		if (this != &rhs)
		{
			m_Version = rhs.m_Version;
		}
		return *this;
	}
	bool operator==(ARBVersion const& rhs) const
	{
		return m_Version == rhs.m_Version;
	}
	bool operator<(ARBVersion const& rhs) const
	{
		return m_Version < rhs.m_Version;
	}
	bool operator<=(ARBVersion const& rhs) const
	{
		return m_Version <= rhs.m_Version;
	}
	bool operator>(ARBVersion const& rhs) const
	{
		return m_Version > rhs.m_Version;
	}
	bool operator>=(ARBVersion const& rhs) const
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
 * In XML/Element, this class is only used as an attribute, never an element.
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
		eQ_E,		///< Eliminated.
		eQ_NQ,		///< Not qualified.
		eQ_Q,		///< Qualified.
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
	ARB_Q(ARB_Q const& rhs) : m_Q(rhs.m_Q)
	{
	}
	~ARB_Q()
	{
	}

	ARB_Q& operator=(ARB_Q const& rhs)
	{
		if (this != &rhs)
			m_Q = rhs.m_Q;
		return *this;
	}
	bool operator==(ARB_Q const& rhs) const
	{
		return m_Q == rhs.m_Q;
	}
	bool operator!=(ARB_Q const& rhs) const
	{
		return !operator==(rhs);
	}
	bool operator<(ARB_Q const& rhs) const
	{
		return static_cast<int>(m_Q) < static_cast<int>(rhs.m_Q);
	}
	bool operator<=(ARB_Q const& rhs) const
	{
		return static_cast<int>(m_Q) <= static_cast<int>(rhs.m_Q);
	}
	bool operator>(ARB_Q const& rhs) const
	{
		return static_cast<int>(m_Q) > static_cast<int>(rhs.m_Q);
	}
	bool operator>=(ARB_Q const& rhs) const
	{
		return static_cast<int>(m_Q) >= static_cast<int>(rhs.m_Q);
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
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
		std::string const& inAttrib,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Element.
	 * @param inAttribName Name of attribute to write Q to.
	 * @return Success
	 * @post The ARB_Q attribute will be added to ioTree.
	 */
	bool Save(
		Element& ioTree,
		char const* const inAttribName) const;

private:
	eQ m_Q;
};

/////////////////////////////////////////////////////////////////////////////
/**
 * Double class with some formatting.
 * In XML/Element, this class is only used as an attribute, never an element.
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
	ARBDouble(ARBDouble const& rhs)
		: m_Prec(rhs.m_Prec)
		, m_Val(rhs.m_Val)
	{
	}
	~ARBDouble()
	{
	}

	ARBDouble& operator=(ARBDouble const& rhs)
	{
		if (this != &rhs)
		{
			m_Prec = rhs.m_Prec;
			m_Val = rhs.m_Val;
		}
		return *this;
	}
	ARBDouble& operator+=(ARBDouble const& rhs)
	{
		m_Val += rhs.m_Val;
		return *this;
	}
	ARBDouble& operator-=(ARBDouble const& rhs)
	{
		m_Val -= rhs.m_Val;
		return *this;
	}
	bool operator==(ARBDouble const& rhs) const
	{
		return m_Prec == rhs.m_Prec
			&& m_Val == rhs.m_Val;
	}
	bool operator!=(ARBDouble const& rhs) const
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
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
		std::string const& inAttrib,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Element.
	 * @param inAttribName Name of attribute to write Q to.
	 * @return Success
	 * @post The double attribute will be added to ioTree.
	 */
	bool Save(
		Element& ioTree,
		char const* const inAttribName) const;

private:
	int m_Prec;		///< Precision for display.
	double m_Val;	///< Actual value.
};
