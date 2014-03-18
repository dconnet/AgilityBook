#ifdef _MSC_VER
#pragma once
#endif

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Precompiled header file for system headers.
 * @author David Connet
 *
 * This must be included after any wx headers (including any system headers
 * before include wx headers causes issues in msvc)
 *
 * Revision History
 * 2013-01-27 Split/moved stdafx.h.
 */

// VC9/VC10 defines _HAS_TR1 if tr1 is present (via yvals.h).
#include <memory>
// If not present, pick up boost's. Now we can use std::tr1::shared_ptr
#if _MSC_VER < 1700 && (!defined(_HAS_TR1) || !_HAS_TR1)
#define __USE_BOOST	1
#else
#define __USE_BOOST	0
#endif

#if __USE_BOOST
// Boost can also be included by tweaking the include path and
// including <memory>:
//  boost-root/boost/tr1/tr1
//  boost-root
//  other standard library replacements (like STLport)
//  your regular standard library
// (see <boost>/doc/html/boost_tr1/usage.html)
// Including memory.hpp is the preferred method over include path order.
#include <boost/version.hpp>
#if BOOST_VERSION < 103800
#error Minimum supported version of Boost: 1.38.0
#endif
#include <boost/tr1/memory.hpp>
#include <boost/tr1/tuple.hpp>
#include <boost/make_shared.hpp>
namespace std
{
	using boost::make_shared;
	using boost::shared_ptr;
	using boost::weak_ptr;
	using boost::dynamic_pointer_cast;
	using tr1::tuple;
	using tr1::get;
};

#else
#include <tuple>

#endif

#include <assert.h>

// For _() usage when used indirectly.
// poedit is configured to also look for these.
#if defined(UNICODE) || defined(_UNICODE)
#define arbT(x)	L##x
#else
#define arbT(x)	x
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1400
#define ARRAY_SIZE(buf)	_countof(buf)
#else
#define ARRAY_SIZE(buf) (sizeof(buf) / sizeof(buf[0]))
#endif
