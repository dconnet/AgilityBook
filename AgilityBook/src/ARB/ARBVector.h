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
 * @brief Templates to simplify some vector operations.
 * @author David Connet
 *
 * Revision History
 * @li 2004-03-16 DRC Made copy ctor and operator= safer.
 * @li 2004-01-25 DRC Added Move().
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <vector>
class ARBConfig;
class ARBConfigDivisionList;
class ARBConfigVenue;
class ARBDogClubList;
class ARBVersion;
class CElement;

#if _MSC_VER < 1300
// VC6 has problems with std::vector::clear() type syntax.
// And I really don't want to declaring using statements in my headers!
using std::set;
using std::string;
using std::vector;
#endif

/**
 * Base collection template for loading/saving data.
 *
 * I wanted to hide as much implementation as possible - so this template hides
 * the fact that we are actually using arrays of pointers. The only place
 * pointers vs. real objects is evident is when we use an iterator. Even if we
 * were to change to a full object, we'd really still want to return a pointer
 * on most functions - so the api itself would not actually change.
 *
 * The base template handles all the copy/cleanup necessary. In addition,
 * the Save functionality is the same for all classes, so that is here too.
 * Load functionality varies slightly from class to class, so that is handled
 * by several ARBVector derivations.
 */
template <class ARBThing>
class ARBVector : public std::vector<ARBThing*>
{
public:
	ARBVector()
	{
	}

	/**
	 * Copy constructor.
	 * @param rhs Object being copied.
	 * @post A deep copy of rhs.
	 */
	ARBVector(const ARBVector& rhs)
	{
		// Make a copy first. Then if we throw an exception during
		// the copy, we won't clobber the existing data.
		ARBVector<ARBThing> tmp;
		tmp.reserve(rhs.size());
		for (const_iterator iter = rhs.begin(); iter != rhs.end(); ++iter)
		{
			ARBThing* pItem = *iter;
			if (pItem)
				tmp.push_back(new ARBThing(*pItem));
		}
		swap(tmp);
	}

	virtual ~ARBVector()
	{
		clear();
	}

	/**
	 * Assignment operator.
	 * @param rhs Object being copied.
	 * @post A deep copy of rhs.
	 */
	ARBVector& operator=(const ARBVector& rhs)
	{
		if (this != &rhs)
		{
			ARBVector<ARBThing> tmp;
			tmp.reserve(rhs.size());
			for (const_iterator iter = rhs.begin(); iter != rhs.end(); ++iter)
			{
				if ((*iter))
					tmp.push_back(new ARBThing(*(*iter)));
			}
			swap(tmp);
		}
		return *this;
	}

	/**
	 * Ahh, the fun of templates...
	 * op==/op!= has a template instantiation problem.
	 * @attention: Implement op==/op!= in each derived class and call this.
	 */
	bool isEqual(const ARBVector& rhs) const
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

	/**
	 * Get all the strings to search in this list.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const
	{
		size_t nItems = 0;
		for (const_iterator iter = begin(); iter != end(); ++iter)
			nItems += (*iter)->GetSearchStrings(ioStrings);
		return nItems;
	}

	/**
	 * Reset the contents of this object and all sub-objects.
	 * @post All content cleared, including configuration.
	 */
	void clear()
	{
		for (iterator iter = begin(); iter != end(); ++iter)
			(*iter)->Release();
#if _MSC_VER < 1300
		vector<ARBThing*>::clear();
#else
		std::vector<ARBThing*>::clear();
#endif
	}

	iterator erase(iterator inIter)
	{
		(*inIter)->Release();
#if _MSC_VER < 1300
		return vector<ARBThing*>::erase(inIter);
#else
		return std::vector<ARBThing*>::erase(inIter);
#endif
	}

	iterator erase(iterator inFirst, iterator inLast)
	{
		for (iterator iter = inFirst; iter <= inLast; ++iter)
			(*iter)->Release();
#if _MSC_VER < 1300
		return vector<ARBThing*>::erase(inFirst, inLast);
#else
		return std::vector<ARBThing*>::erase(inFirst, inLast);
#endif
	}

	/**
	 * Reorder a list.
	 * @param inItem Object to be moved.
	 * @param inMove Number of positions to move object.
	 * @return Whether or not object was moved.
	 */
	bool Move(ARBThing* inItem, int inMove)
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
						ARBThing* tmp = at(n);
						at(n) = at(offset);
						at(offset) = tmp;
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
	 * @post The ARBThing element will be created in ioTree.
	 */
	bool Save(CElement& ioTree) const
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

// Load methods are specific to classes. We could implement templates to
// handle all the different Load styles, but I decided that for those
// classes that are truly unique, just let them derive from ARBVector and
// implement Load themselves. The ones below are used by multiple classes.
// (Theres only a few that derive directly: ARBConfigDivisionList,
// ARBConfigEventList, ARBConfigScoringList, ARBDogRunList)

/**
 * This load method is used by several of the ARBConfig* classes.
 */
template <class ARBThing>
class ARBVectorLoad1 : public ARBVector<ARBThing>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual ARBThing element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool Load(const CElement& inTree, const ARBVersion& inVersion, std::string& ioErrMsg)
	{
		ARBThing* thing = new ARBThing();
		if (!thing->Load(inTree, inVersion, ioErrMsg))
		{
			thing->Release();
			return false;
		}
		push_back(thing);
		return true;
	}
};

/**
 * This load method is used by several of the ARBDog* classes.
 */
template <class ARBThing>
class ARBVectorLoad2 : public ARBVector<ARBThing>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual ARBThing element.
	 * @param inConfig Configuration information to verify data to load against.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool Load(const ARBConfig& inConfig, const CElement& inTree, const ARBVersion& inVersion, std::string& ioErrMsg)
	{
		ARBThing* thing = new ARBThing();
		if (!thing->Load(inConfig, inTree, inVersion, ioErrMsg))
		{
			thing->Release();
			return false;
		}
		push_back(thing);
		return true;
	}
};
