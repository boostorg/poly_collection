/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_SIZE_OF_HPP
#define BOOST_POLY_COLLECTION_DETAIL_SIZE_OF_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <cstddef>

namespace boost{

namespace poly_collection{

namespace detail{

/* VS chokes if sizeof(T) is used directly without this indirection on 
 * certain alias templates.
 */

template<typename T>
struct size_of
{
  static constexpr std::size_t value=sizeof(T);
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
