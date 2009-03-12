#pragma once

/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @li 2008-06-29 DRC Moved string stuff to ARBString.
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

// Forward references
class ARBConfig;
#define ARB_TYPEDEF(name) \
	class name;\
	typedef tr1::shared_ptr<name> name##Ptr;
#define ARB_TYPEDEF2(name) \
	class name;\
	typedef tr1::weak_ptr<name> name##WPtr;\
	typedef tr1::shared_ptr<name> name##Ptr;
#define ARB_TYPEDEF_LIST(name) \
	class name;\
	class name##List;\
	typedef tr1::shared_ptr<name> name##Ptr;

ARB_TYPEDEF(ARBConfigActionDeleteDivision)
ARB_TYPEDEF(ARBConfigActionDeleteEvent)
ARB_TYPEDEF(ARBConfigActionDeleteLevel)
ARB_TYPEDEF(ARBConfigActionDeleteMultiQ)
ARB_TYPEDEF(ARBConfigActionDeleteOtherPoints)
ARB_TYPEDEF(ARBConfigActionDeleteTitle)
ARB_TYPEDEF(ARBConfigActionDeleteVenue)
ARB_TYPEDEF(ARBConfigActionRenameDivision)
ARB_TYPEDEF(ARBConfigActionRenameEvent)
ARB_TYPEDEF(ARBConfigActionRenameLevel)
ARB_TYPEDEF(ARBConfigActionRenameMultiQ)
ARB_TYPEDEF(ARBConfigActionRenameOtherPoints)
ARB_TYPEDEF(ARBConfigActionRenameTitle)
ARB_TYPEDEF(ARBConfigActionRenameVenue)
ARB_TYPEDEF(Element)
ARB_TYPEDEF_LIST(ARBCalendar)
ARB_TYPEDEF_LIST(ARBConfigAction)
ARB_TYPEDEF_LIST(ARBConfigCalSite)
ARB_TYPEDEF_LIST(ARBConfigDivision)
ARB_TYPEDEF_LIST(ARBConfigEvent)
ARB_TYPEDEF_LIST(ARBConfigFault)
ARB_TYPEDEF_LIST(ARBConfigLevel)
ARB_TYPEDEF_LIST(ARBConfigLifetimePoints)
ARB_TYPEDEF_LIST(ARBConfigMultiQ)
ARB_TYPEDEF_LIST(ARBConfigOtherPoints)
ARB_TYPEDEF_LIST(ARBConfigPlaceInfo)
ARB_TYPEDEF_LIST(ARBConfigScoring)
ARB_TYPEDEF_LIST(ARBConfigSubLevel)
ARB_TYPEDEF_LIST(ARBConfigTitle)
ARB_TYPEDEF_LIST(ARBConfigTitlePoints)
ARB_TYPEDEF_LIST(ARBConfigVenue)
ARB_TYPEDEF_LIST(ARBDog)
ARB_TYPEDEF_LIST(ARBDogClub)
ARB_TYPEDEF_LIST(ARBDogExistingPoints)
ARB_TYPEDEF_LIST(ARBDogReferenceRun)
ARB_TYPEDEF_LIST(ARBDogRegNum)
ARB_TYPEDEF_LIST(ARBDogRun)
ARB_TYPEDEF_LIST(ARBDogRunOtherPoints)
ARB_TYPEDEF_LIST(ARBDogRunPartner)
ARB_TYPEDEF_LIST(ARBDogRunScoring)
ARB_TYPEDEF_LIST(ARBDogTitle)
ARB_TYPEDEF_LIST(ARBDogTrial)
ARB_TYPEDEF_LIST(ARBInfoItem)
ARB_TYPEDEF_LIST(ARBTraining)
ARB_TYPEDEF2(ElementNode)
ARB_TYPEDEF2(ElementText)

#include "ARBString.h"


/**
 * Dump a string to cerr or TRACE
 * @param inMsg String to dump
 * @param bIncNewLine Add a newline
 */
void DumpErrorMessage(tstring const& inMsg, bool bIncNewLine = true);


/**
 * Make a string safe for HTML viewing.
 * @param inRawData String to sanitize
 * @param bConvertCR Convert \n to html-breaks.
 */
tstring SanitizeStringForHTML(
		tstring const& inRawData,
		bool bConvertCR = true);

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
	 * Equality test (by value, not pointer).
	 */
	bool operator==(ARBVector<T> const& rhs) const
	{
		if (this == &rhs)
			return true;
		if (size() != rhs.size())
			return false;
		typename std::vector<T>::const_iterator iter1, iter2;
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
		for (typename std::vector<T>::const_iterator iter = begin(); iter != end(); ++iter)
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
	size_t GetSearchStrings(std::set<tstring>& ioStrings) const
	{
		size_t nItems = 0;
		for (typename std::vector<T>::const_iterator iter = begin(); iter != end(); ++iter)
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
			for (typename std::vector<T>::iterator iter = begin(); iter != end(); ++iter, ++n)
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
						std::swap(ARBVector<T>::at(n), ARBVector<T>::at(offset));
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
	bool Save(ElementNodePtr ioTree) const
	{
		for (typename std::vector<T>::const_iterator iter = begin(); iter != end(); ++iter)
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
	ARBErrorCallback(tstring& ioErrMsg)
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
	virtual void LogMessage(tstring const& inMsg)
	{
		m_ErrMsg += inMsg;
	}

protected:
	tstring& m_ErrMsg;
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
	tstring str() const;

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
	static tstring GetValidTypes();

	/**
	 * Get a list of all valid types.
	 */
	static void GetValidTypes(std::vector<tstring>& outTypes);

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
	tstring str() const;

	/**
	 * Load a Q
	 * @param inAttrib Name of attribute containing value to parse.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			tstring const& inAttrib,
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
			ElementNodePtr ioTree,
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
	static tstring str(
			double inValue,
			int inPrec = 2);

	/**
	 * Compare two doubles, allowing for 'prec' error.
	 */
	static bool equal(
			double const& inVal1,
			double const& inVal2,
			double inPrec = 1e-9);
};
