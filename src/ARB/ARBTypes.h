#pragma once

/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @brief Type definitions.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Removed ARBDouble.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-04-06 DRC Added op< to ARB_Q.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-09-01 DRC Added 'operator+=' and 'operator-=' to ARBDouble.
 */

#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

class ARBConfig;
class Element;

#define ARB_TYPEDEF(name) \
	class name;\
	class name##List;\
	typedef boost::shared_ptr<name> name##Ptr;

ARB_TYPEDEF(ARBCalendar)
ARB_TYPEDEF(ARBConfigAction)
ARB_TYPEDEF(ARBConfigDivision)
ARB_TYPEDEF(ARBConfigEvent)
ARB_TYPEDEF(ARBConfigFault)
ARB_TYPEDEF(ARBConfigLevel)
ARB_TYPEDEF(ARBConfigLifetimePoints)
ARB_TYPEDEF(ARBConfigMultiQ)
ARB_TYPEDEF(ARBConfigOtherPoints)
ARB_TYPEDEF(ARBConfigScoring)
ARB_TYPEDEF(ARBConfigSubLevel)
ARB_TYPEDEF(ARBConfigTitle)
ARB_TYPEDEF(ARBConfigTitlePoints)
ARB_TYPEDEF(ARBConfigVenue)
ARB_TYPEDEF(ARBDog)
ARB_TYPEDEF(ARBDogClub)
ARB_TYPEDEF(ARBDogExistingPoints)
ARB_TYPEDEF(ARBDogReferenceRun)
ARB_TYPEDEF(ARBDogRegNum)
ARB_TYPEDEF(ARBDogRun)
ARB_TYPEDEF(ARBDogRunOtherPoints)
ARB_TYPEDEF(ARBDogRunPartner)
ARB_TYPEDEF(ARBDogRunScoring)
ARB_TYPEDEF(ARBDogTitle)
ARB_TYPEDEF(ARBDogTrial)
ARB_TYPEDEF(ARBInfoItem)
ARB_TYPEDEF(ARBTraining)

// Note, for non-Windows systems, see stdafx.h for additional requirements.
#ifdef UNICODE
typedef std::wstring ARBString;
typedef std::wostringstream ARBostringstream;
#else
typedef std::string ARBString;
typedef std::ostringstream ARBostringstream;
#endif

/////////////////////////////////////////////////////////////////////////////
/**
 * Extend some common functionality.
 */
template <typename T>
class ARBVector : public std::vector<T>
{
public:
	ARBVector()
	{
	}

	/**
	 * Equality test.
	 */
	bool operator==(ARBVector<T> const& rhs) const
	{
		if (this == &rhs)
			return true;
		if (size() != rhs.size())
			return false;
		const_iterator iter1, iter2;
		for (iter1 = begin(), iter2 = rhs.begin(); iter1 != end(); ++iter1, ++iter2)
		{
			if (*(*iter1) != *(*iter2))
				return false;
		}
		return true;
	}
	bool operator!=(ARBVector<T> const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Make a copy of everything.
	 * @param outList Object being copied to.
	 */
	size_t Clone(ARBVector<T>& outList) const
	{
		outList.clear();
		outList.reserve(size());
		for (const_iterator iter = begin(); iter != end(); ++iter)
		{
			T pItem = *iter;
			if (pItem)
				outList.push_back(pItem->Clone());
		}
		return outList.size();
	}

	/**
	 * Get all the strings to search in this list.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<ARBString>& ioStrings) const
	{
		size_t nItems = 0;
		for (const_iterator iter = begin(); iter != end(); ++iter)
			nItems += (*iter)->GetSearchStrings(ioStrings);
		return nItems;
	}

	/**
	 * Reorder a list.
	 * @param inItem Object to be moved.
	 * @param inMove Number of positions to move object.
	 * @return Whether or not object was moved.
	 */
	bool Move(
			T inItem,
			int inMove)
	{
		bool bOk = false;
		if (inItem)
		{
			int n = 0;
			for (iterator iter = begin(); iter != end(); ++iter, ++n)
			{
				if (inItem == *iter)
				{
					int offset = n + inMove;
					if (offset < 0)
						offset = 0;
					if (offset >= static_cast<int>(size()))
						offset = static_cast<int>(size()) - 1;
					if (offset != n)
					{
						bOk = true;
						std::swap(at(n), at(offset));
						break;
					}
				}
			}
		}
		return bOk;
	}

	/**
	 * Save a document.
	 * In general, the vectors themselves are never stored, only the elements.
	 * (It flattens the tree a little.)
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The T element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const
	{
		for (const_iterator iter = begin(); iter != end(); ++iter)
		{
			if (!(*iter)->Save(ioTree))
				return false;
		}
		return true;
	}
};

/////////////////////////////////////////////////////////////////////////////
/**
 * Error callback class.
 */
class ARBErrorCallback
{
public:
	ARBErrorCallback(ARBString& ioErrMsg)
		: m_ErrMsg(ioErrMsg)
	{
	}
	virtual ~ARBErrorCallback() {}

	/**
	 * Error message that requires a user response.
	 * @param pMsg Message to display to user.
	 * @return True to continue, false to abort.
	 */
	virtual bool OnError(TCHAR const* const pMsg)
	{
		return false;
	}

	/**
	 * Log an error message.
	 */
	virtual void LogMessage(ARBString const& inMsg)
	{
		m_ErrMsg += inMsg;
	}

protected:
	ARBString& m_ErrMsg;
};

/////////////////////////////////////////////////////////////////////////////
/**
 * A version has a major and minor number.
 * Differences between major numbers prevents older versions of the program
 * from reading newer files. Differences in minor numbers allows an older
 * version to read the newer file, but warns that some information may be
 * lost if the file is saved.
 *
 * This class is assuming that a short==16 bits and long==32 bits.
 */
class ARBVersion
{
public:
	ARBVersion()
		: m_Version(0)
	{
	}

	ARBVersion(
			unsigned short major,
			unsigned short minor)
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
	ARBString str() const;

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
	static ARBString GetValidTypes();

	/**
	 * Get a list of all valid types.
	 */
	static void GetValidTypes(std::vector<ARBString>& outTypes);

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
	ARB_Q(eQ inQ) : m_Q(inQ)
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
	bool operator==(eQ rhs) const
	{
		return m_Q == rhs;
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
	ARBString str() const;

	/**
	 * Load a Q
	 * @param inAttrib Name of attribute containing value to parse.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBString const& inAttrib,
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
			TCHAR const* const inAttribName) const;

private:
	eQ m_Q;
};

/////////////////////////////////////////////////////////////////////////////
/**
 * Helper functions for quickly/easily converting doubles to strings.
 */
class ARBDouble
{
	ARBDouble();
	~ARBDouble();
public:
	static ARBString str(
			double inValue,
			int inPrec = 2);

	/**
	 * Compare two doubles, allowing for DBL_EPSILON error.
	 */
	static bool equal(double const& inVal1, double const& inVal2);
};
