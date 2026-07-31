/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_HUB_ACCESS_HPP
#define BOOST_POLY_COLLECTION_DETAIL_HUB_ACCESS_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/assert.hpp>
#include <boost/core/bit.hpp>
#include <boost/core/pointer_traits.hpp>
#include <boost/container/hub.hpp>
#include <cstdint>
#include <cstring>
#include <type_traits>

#if !defined(BOOST_POLY_COLLECTION_DISABLE_SSE2)
#if defined(BOOST_POLY_COLLECTION_ENABLE_SSE2)|| \
    defined(__SSE2__) || \
    defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
#define BOOST_POLY_COLLECTION_SSE2
#endif
#endif

#if defined(BOOST_POLY_COLLECTION_SSE2)
#include <emmintrin.h>
#endif

namespace boost{

namespace poly_collection{

namespace detail{

/* Private access to boost::container::hub internals.
 * To be eventually replaced with legit mechanism provided in Boost.Container.
 */

template<typename ValuePointer>
using hub_iterator=boost::container::hub_detail::iterator<ValuePointer>;

using hub_block_base=boost::container::hub_detail::block_base<void*>;

struct hub_type_erased_block:hub_block_base{void* data_;};

static_assert(
  sizeof(hub_type_erased_block)==
  sizeof(boost::container::hub_detail::block<char*>),
  "hub block layout assumption violated");

/* Structural copying of boost::container::hub_detail::iterator, to be replaced
 * eventually by legal access to private members.
 */

struct hub_iterator_members
{
  hub_block_base* pbb;
  int             n;
};

template<typename ValuePointer>
hub_iterator_members get_members(const hub_iterator<ValuePointer>& it)noexcept
{
  static_assert(
    sizeof(hub_iterator<ValuePointer>)==sizeof(hub_iterator_members)&&
    std::is_standard_layout<hub_iterator<ValuePointer>>::value,
    "hub iterator layout assumption violated");
  hub_iterator_members m;
  std::memcpy(&m,static_cast<const void*>(&it),sizeof(m));
  return m;
}

template<typename HubIterator>
HubIterator make_hub_iterator(hub_iterator_members m)noexcept
{
  static_assert(
    sizeof(HubIterator)==sizeof(hub_iterator_members)&&
    std::is_standard_layout<HubIterator>::value,
    "hub iterator layout assumption violated");
  HubIterator it;
  std::memcpy(static_cast<void*>(&it),&m,sizeof(m));
  return it;
}

/* Related low-level utilities, ripped from Boost.Container source code */

#ifdef __has_builtin
#define BOOST_POLY_COLLECTION_HAS_BUILTIN(x) __has_builtin(x)
#else
#define BOOST_POLY_COLLECTION_HAS_BUILTIN(x) 0
#endif

#if !defined(NDEBUG)
#define BOOST_POLY_COLLECTION_ASSUME(cond) BOOST_ASSERT(cond)
#elif BOOST_POLY_COLLECTION_HAS_BUILTIN(__builtin_assume)
#define BOOST_POLY_COLLECTION_ASSUME(cond) __builtin_assume(cond)
#elif defined(__GNUC__)||\
      BOOST_POLY_COLLECTION_HAS_BUILTIN(__builtin_unreachable)
#define BOOST_POLY_COLLECTION_ASSUME(cond) \
  do{                                      \
    if(!(cond))__builtin_unreachable();    \
  } while(0)
#elif defined(_MSC_VER)
#define BOOST_POLY_COLLECTION_ASSUME(cond) __assume(cond)
#else
#define BOOST_POLY_COLLECTION_ASSUME(cond) \
  do{                                      \
    static_cast<void>(false&&(cond));      \
  } while(0)
#endif

#if defined(BOOST_GCC)||defined(BOOST_CLANG)
#define BOOST_POLY_COLLECTION_PREFETCH(p) \
__builtin_prefetch((const char*)boost::to_address(p))
#elif defined(BOOST_POLY_COLLECTION_SSE2)
#define BOOST_POLY_COLLECTION_PREFETCH(p) \
_mm_prefetch((const char*)boost::to_address(p),_MM_HINT_T0)
#else
#define BOOST_POLY_COLLECTION_PREFETCH(p) ((void)(p))
#endif

#define BOOST_POLY_COLLECTION_PREFETCH_HUB_BLOCK(pbb)                \
do{                                                                  \
  auto p0=&static_cast<                                              \
    boost::poly_collection::detail::hub_type_erased_block&>(*(pbb)); \
  BOOST_POLY_COLLECTION_PREFETCH(p0->data_);                         \
} while(0)

inline int unchecked_countr_zero(std::uint64_t x)
{
#if defined(BOOST_MSVC)&&(defined(_M_X64)||defined(_M_ARM64))
  unsigned long r;
  _BitScanForward64(&r,x);
  return (int)r;
#elif defined(BOOST_GCC)||defined(BOOST_CLANG)
  return (int)__builtin_ctzll(x);
#else
  BOOST_POLY_COLLECTION_ASSUME(x!=0);
  return (int)core::countr_zero(x);
#endif
}

inline int unchecked_countl_zero(std::uint64_t x)
{
#if defined(BOOST_MSVC)&&(defined(_M_X64)||defined(_M_ARM64))
  unsigned long r;
  _BitScanReverse64(&r,x);
  return (int)(63-r);
#elif defined(BOOST_GCC)||defined(BOOST_CLANG)
  return (int)__builtin_clzll(x);
#else  
  BOOST_POLY_COLLECTION_ASSUME(x!=0);
  return (int)core::countl_zero(x);
#endif
}

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
