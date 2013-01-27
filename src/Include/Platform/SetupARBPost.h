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
 *
 * @brief Precompiled header file for system headers.
 * @author David Connet
 *
 * This must be included after any wx headers (including any system headers
 * before include wx headers causes issues in msvc)
 *
 * Revision History
 * @li 2013-01-27 DRC Split/moved stdafx.h.
 */

// VC9/VC10 defines _HAS_TR1 if tr1 is present (via yvals.h).
#include <memory>
// If not present, pick up boost's. Now we can use std::tr1::shared_ptr
#if _MSC_VER < 1700 && (!defined(_HAS_TR1) || !_HAS_TR1)
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
#include <boost/make_shared.hpp>
namespace std
{
	using boost::make_shared;
};
#endif

#include <assert.h>
