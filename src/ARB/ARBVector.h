#pragma once

/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
 * @brief Base collection classes for loading/saving data.
 * @author David Connet
 *
 * Revision History
 */

#include <vector>
class ARBConfig;
class ARBConfigDivisionList;
class ARBConfigVenue;
class ARBDogClubList;
class CElement;

#if _MSC_VER < 1300
// VC6 has problems with std::vector::clear() type syntax.
// And I really don't want to declaring using statements in my headers!
using namespace std;
#endif

/**
 * @brief Templates to simplify some vector operations.
 * @author David Connet
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

	ARBVector(const ARBVector& rhs)
	{
		clear();
		for (const_iterator iter = rhs.begin(); iter != rhs.end(); ++iter)
		{
			if ((*iter))
				push_back(new ARBThing(*(*iter)));
		}
	}

	virtual ~ARBVector()
	{
		clear();
	}

	ARBVector& operator=(const ARBVector& rhs)
	{
		if (this != &rhs)
		{
			clear();
			for (const_iterator iter = rhs.begin(); iter != rhs.end(); ++iter)
			{
				if ((*iter))
					push_back(new ARBThing(*(*iter)));
			}
		}
		return *this;
	}

	// Note, op==/op!= has a template instantiation problem.
	// @attention: Implement op==/op!= in each derived class and call this.
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
	 * Save the vector to an output tree (which will become XML). In
	 * general, the vectors themselves are never stored, only the elements.
	 * (It flattens the tree a little.)
	 * @param outTree Parent element in which data is stored. Each class will create the necessary sub-elements.
	 * @retval true Successfully stored.
	 * @retval false Unable to store.
	 */
	bool Save(CElement& outTree) const
	{
		for (const_iterator iter = begin(); iter != end(); ++iter)
		{
			if (!(*iter)->Save(outTree))
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
 * @brief This load method is used by several of the ARBConfig* classes.
 */
template <class ARBThing>
class ARBVectorLoad1 : public ARBVector<ARBThing>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @param inTree Actual data to load (not the parent as it is during Save).
	 * @param inVersion Version of .arb file (for backwards compatibility).
	 * @retval true Successfully loaded.
	 * @retval false Unable to load (an error message is generated).
	 */
	bool Load(const CElement& inTree, int inVersion)
	{
		ARBThing* thing = new ARBThing;
		if (!thing->Load(inTree, inVersion))
		{
			thing->Release();
			return false;
		}
		push_back(thing);
		return true;
	}
};

/**
 * @brief This load method is used by several of the ARBDog* classes.
 */
template <class ARBThing>
class ARBVectorLoad2 : public ARBVector<ARBThing>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @param inConfig Configuration information to verify data to load against.
	 * @param inTree Actual data to load (not the parent as it is during Save).
	 * @param inVersion Version of .arb file (for backwards compatibility).
	 * @retval true Successfully loaded.
	 * @retval false Unable to load (an error message is generated).
	 */
	bool Load(const ARBConfig& inConfig, const CElement& inTree, int inVersion)
	{
		ARBThing* thing = new ARBThing;
		if (!thing->Load(inConfig, inTree, inVersion))
		{
			thing->Release();
			return false;
		}
		push_back(thing);
		return true;
	}
};
