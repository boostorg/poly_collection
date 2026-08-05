/* Copyright 2016-2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_IS_ACCEPTABLE_HPP
#define BOOST_POLY_COLLECTION_DETAIL_IS_ACCEPTABLE_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/mp11/algorithm.hpp>
#include <boost/poly_collection/detail/is_closed_collection.hpp>
#include <boost/poly_collection/detail/is_storable.hpp>
#include <type_traits>

namespace boost{

namespace poly_collection{

namespace detail{

/* is_acceptable does not check type storability in the case of closed
 * collections: this is done once at poly_collection instantiation time with
 * check_acceptability.
 */

template<typename T,typename Model>
struct is_acceptable_open_collection:std::integral_constant<
  bool,
  Model::template is_implementation<T>::value&&is_storable<T,Model>::value
>{};

template<typename T,typename Model>
struct is_acceptable_closed_collection:
  mp11::mp_contains<typename Model::acceptable_type_list,T>{};

template<typename T,typename Model>
using is_acceptable=typename std::conditional<
  is_closed_collection<Model>::value,
  is_acceptable_closed_collection<T,Model>,
  is_acceptable_open_collection<T,Model>
>::type;

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
