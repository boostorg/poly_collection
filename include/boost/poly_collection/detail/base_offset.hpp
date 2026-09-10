/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_BASE_OFFSET_HPP
#define BOOST_POLY_COLLECTION_DETAIL_BASE_OFFSET_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <cstddef>
#include <type_traits>

namespace boost{

namespace poly_collection{

namespace detail{

template<typename Base,typename Derived>
std::ptrdiff_t base_offset()noexcept
{
  static_assert(
    std::is_base_of<Base,Derived>::value,
    "Base must be a base of Derived");
  alignas(Derived) static const unsigned char spc[sizeof(Derived)]={};
  auto p=reinterpret_cast<const Derived*>(&spc);
  return
    reinterpret_cast<const char*>(static_cast<const Base*>(p))-
    reinterpret_cast<const char*>(p);
}

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
