/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_IS_UNORDERED_COLLECTION_HPP
#define BOOST_POLY_COLLECTION_DETAIL_IS_UNORDERED_COLLECTION_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/type_traits/make_void.hpp>
#include <type_traits>

namespace boost{

namespace poly_collection{

namespace detail{

template<typename Model,typename=void>
struct is_unordered_collection:std::false_type{};

template<typename Model>
struct is_unordered_collection<
  Model,void_t<typename Model::segment::is_unordered>
>:std::true_type{};
  
} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
