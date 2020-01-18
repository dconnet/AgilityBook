#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Type definitions.
 * @author David Connet
 *
 * Revision History
 * 2020-01-17 Moved ShouldComputeScore here.
 * 2015-03-15 Fixed Unknown-Q usage.
 * 2015-02-13 Added Unknown state.
 * 2013-08-14 Moved out of ARBTypes.h
 */

#include "ARBTypes2.h"
#include "LibwxARB.h"


enum class Q
{
	UNK,	///< Unknown / not set. This must be first.
	NA,		///< Cannot qualify in this run.
	DNR,	///< Did Not Run.
	E,		///< Eliminated.
	NQ,		///< Not qualified.
	Q,		///< Qualified.
	SuperQ,	///< Super Qualifier (USDAA Snooker top 15%).
	FEO,    ///< For Exhibition Only.
};


/**
 * Qualifying status of a run.
 * In XML/Element, this class is only used as an attribute, never an element.
 * This serves as a translation mechanism from the string in the XML to
 * something we can use as a basic type.
 */
class ARB_API ARB_Q
{
public:
	/**
	 * Types of Qs (the enum order determines the sort order)
	 */

	// These functions map the above enum into strings.

	/**
	 * Get a single string listing all valid types.
	 */
	static std::wstring GetValidTypes();

	/**
	 * Get a list of all valid types.
	 */
	static void GetValidTypes(std::vector<std::wstring>& outTypes);

	/**
	 * Get the number of valid types.
	 */
	static int GetNumValidTypes();

	/**
	 * Get the valid type for a given index. Use this with GetNumValidTypes().
	 */
	static ARB_Q GetValidType(int inIndex);

	ARB_Q() : m_Q(Q::UNK)
	{
	}
	ARB_Q(Q inQ) : m_Q(inQ)
	{
	}
	ARB_Q(ARB_Q const& rhs) : m_Q(rhs.m_Q)
	{
	}
	ARB_Q(ARB_Q&& rhs) : m_Q(rhs.m_Q)
	{
		rhs.m_Q = Q::UNK;
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
	ARB_Q& operator=(ARB_Q&& rhs)
	{
		if (this != &rhs)
		{
			m_Q = rhs.m_Q;
			rhs.m_Q = Q::UNK;
		}
		return *this;
	}
	bool operator==(ARB_Q const& rhs) const
	{
		return m_Q == rhs.m_Q;
	}
	bool operator==(Q rhs) const
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
		return Q::Q == m_Q || Q::SuperQ == m_Q;
	}

	/**
	 * Should we compute a score for this?
	 */
	bool ShouldComputeScore()
	{
		return Qualified()
			|| Q::NQ == m_Q
			|| Q::FEO == m_Q
			|| Q::NA == m_Q;
	}

	/**
	 * Do we want to tally statistics about this?
	 * (It makes no sense to list faults for either of these)
	 */
	bool AllowTally() const
	{
		return Q::UNK != m_Q
			&& Q::NA != m_Q
			&& Q::DNR != m_Q;
	}

	/**
	 * Is this type allowed for non-titling runs?
	 * We only show these types for non-titling runs
	 */
	bool AllowForNonTitling() const
	{
		return Q::UNK == m_Q
			|| Q::NA == m_Q
			|| Q::DNR == m_Q
			|| Q::E == m_Q
			|| Q::FEO == m_Q;
	}

	operator Q() const
	{
		return m_Q;
	}

	/**
	 * Translate the enum value to a string
	 */
	std::wstring str() const;

	/**
	 * Load a Q
	 * @param inAttrib Name of attribute containing value to parse.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			std::wstring const& inAttrib,
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
			ElementNodePtr const& ioTree,
			wchar_t const* const inAttribName) const;

private:
	Q m_Q;
};
