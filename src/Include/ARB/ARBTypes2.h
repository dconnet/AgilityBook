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
 * 2018-12-16 Convert to fmt.
 * 2016-01-06 Added ARBConfigLifetimeName.
 * 2013-04-15 Moved ARB specific things out of ARBTypes.h
 * 2012-09-09 Added ARBVectorNoSave.
 */

#include "LibwxARB.h"

#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/Element.h"
#include "fmt/format.h"
class ARBConfig;


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
ARB_TYPEDEF(ICalendar)
ARB_TYPEDEF_LIST(ARBCalendar)
ARB_TYPEDEF_LIST(ARBConfigAction)
ARB_TYPEDEF_LIST(ARBConfigDivision)
ARB_TYPEDEF_LIST(ARBConfigEvent)
ARB_TYPEDEF_LIST(ARBConfigFault)
ARB_TYPEDEF_LIST(ARBConfigLevel)
ARB_TYPEDEF_LIST(ARBConfigLifetimeName)
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
ARB_TYPEDEF2_LIST(ARBDogClub)
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


/**
 * Extend some common functionality.
 */
template <typename T> class ARBVectorNoSave : public std::vector<T>
{
public:
	ARBVectorNoSave()
	{
	}

	/**
	 * Equality test (by value, not pointer).
	 */
	bool operator==(ARBVectorNoSave<T> const& rhs) const
	{
		if (this == &rhs)
			return true;
		if (ARBVectorNoSave<T>::size() != rhs.size())
			return false;
		typename ARBVectorNoSave<T>::const_iterator iter1, iter2;
		for (iter1 = ARBVectorNoSave<T>::begin(), iter2 = rhs.begin(); iter1 != ARBVectorNoSave<T>::end();
			 ++iter1, ++iter2)
		{
			if (*(*iter1) != *(*iter2))
				return false;
		}
		return true;
	}
	bool operator!=(ARBVectorNoSave<T> const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Make a copy of everything.
	 * @param outList Object being copied to.
	 */
	size_t Clone(ARBVectorNoSave<T>& outList) const
	{
		outList.clear();
		outList.reserve(ARBVectorNoSave<T>::size());
		for (typename ARBVectorNoSave<T>::const_iterator iter = ARBVectorNoSave<T>::begin();
			 iter != ARBVectorNoSave<T>::end();
			 ++iter)
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
	size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const
	{
		size_t nItems = 0;
		for (typename ARBVectorNoSave<T>::const_iterator iter = ARBVectorNoSave<T>::begin();
			 iter != ARBVectorNoSave<T>::end();
			 ++iter)
		{
			nItems += (*iter)->GetSearchStrings(ioStrings);
		}
		return nItems;
	}

	/**
	 * Reorder a list.
	 * @param inItem Object to be moved.
	 * @param inMove Number of positions to move object.
	 * @return Whether or not object was moved.
	 */
	bool Move(T inItem, int inMove)
	{
		bool bOk = false;
		if (inItem)
		{
			int n = 0;
			for (typename ARBVectorNoSave<T>::iterator iter = ARBVectorNoSave<T>::begin();
				 iter != ARBVectorNoSave<T>::end();
				 ++iter, ++n)
			{
				if (inItem == *iter)
				{
					int offset = n + inMove;
					if (offset < 0)
						offset = 0;
					if (offset >= static_cast<int>(ARBVectorNoSave<T>::size()))
						offset = static_cast<int>(ARBVectorNoSave<T>::size()) - 1;
					if (offset != n)
					{
						bOk = true;
						std::swap(ARBVectorNoSave<T>::at(n), ARBVectorNoSave<T>::at(offset));
						break;
					}
				}
			}
		}
		return bOk;
	}
};


template <typename T> class ARBVector : public ARBVectorNoSave<T>
{
public:
	ARBVector()
	{
	}

	/**
	 * Save a document.
	 * In general, the vectors themselves are never stored, only the elements.
	 * (It flattens the tree a little.)
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The T element will be created in ioTree.
	 */
	bool Save(ElementNodePtr const& ioTree) const
	{
		for (typename ARBVector<T>::const_iterator iter = ARBVector<T>::begin(); iter != ARBVector<T>::end(); ++iter)
		{
			if (!(*iter)->Save(ioTree))
				return false;
		}
		return true;
	}
};


template <typename T> class ARBVectorSaveConfig : public ARBVectorNoSave<T>
{
public:
	ARBVectorSaveConfig()
	{
	}

	/**
	 * Save a document.
	 * In general, the vectors themselves are never stored, only the elements.
	 * (It flattens the tree a little.)
	 * @param ioTree Parent element.
	 * @param inConfig Configuration.
	 * @return Success
	 * @post The T element will be created in ioTree.
	 */
	bool Save(ElementNodePtr const& ioTree, ARBConfig const& inConfig) const
	{
		for (typename ARBVectorSaveConfig<T>::const_iterator iter = ARBVectorSaveConfig<T>::begin();
			 iter != ARBVectorSaveConfig<T>::end();
			 ++iter)
		{
			if (!(*iter)->Save(ioTree, inConfig))
				return false;
		}
		return true;
	}
};

/////////////////////////////////////////////////////////////////////////////
/**
 * Error callback class.
 */
class ARB_API ARBErrorCallback
{
public:
	explicit ARBErrorCallback(fmt::wmemory_buffer& ioErrMsg)
		: m_ErrMsg(ioErrMsg)
	{
	}
	virtual ~ARBErrorCallback()
	{
	}

	/**
	 * Error message that requires a user response.
	 * @param pMsg Message to display to user.
	 * @return True to continue, false to abort.
	 */
	virtual bool OnError(wchar_t const* const pMsg)
	{
		return false;
	}
	virtual bool OnError(std::wstring const& msg)
	{
		return OnError(msg.c_str());
	}

	/**
	 * Log an error message.
	 */
	virtual void LogMessage(std::wstring const& inMsg)
	{
		fmt::format_to(m_ErrMsg, L"{}", inMsg);
	}

protected:
	fmt::wmemory_buffer& m_ErrMsg;

	DECLARE_NO_COPY_IMPLEMENTED(ARBErrorCallback);
};
