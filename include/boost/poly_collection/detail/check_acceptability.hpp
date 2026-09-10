/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_CHECK_ACCEPTABILITY_HPP
#define BOOST_POLY_COLLECTION_DETAIL_CHECK_ACCEPTABILITY_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/bind.hpp>
#include <boost/mp11/set.hpp>
#include <boost/poly_collection/detail/is_closed_collection.hpp>
#include <boost/poly_collection/detail/is_storable.hpp>
#include <boost/poly_collection/detail/is_unordered_collection.hpp>
#include <type_traits>

namespace boost{

namespace poly_collection{

namespace detail{

/* checks distinctness and storability for the types of a closed collection */

template<typename Model>
constexpr bool check_acceptability(std::false_type /* open */){return true;}

template<typename TypeList>
constexpr bool check_distinct_types()
{
  static_assert(
    mp11::mp_is_set<TypeList>::value,
    "all types in a closed collection must be distinct");
  return true;
}

template<typename ClosedModel>
constexpr bool check_storable_types(std::false_type /* ordered */)
{
  static_assert(
    mp11::mp_all_of_q<
      typename ClosedModel::acceptable_type_list,
      mp11::mp_bind_back<is_storable,ClosedModel>>::value,
    "all types of an ordered closed collection must be cv-unqualified object "
    "types, and be nothrow move constructible or else move constructible and "
    "move assignable");
  return true;
}

template<typename ClosedModel>
constexpr bool check_storable_types(std::true_type /* unordered */)
{
  static_assert(
    mp11::mp_all_of_q<
      typename ClosedModel::acceptable_type_list,
      mp11::mp_bind_back<is_storable,ClosedModel>>::value,
    "all types of an unordered closed collection must be instantiable "
    "cv-unqualified object types");
  return true;
}

template<typename Model>
constexpr bool check_acceptability(std::true_type /* closed */)
{
  return 
    check_distinct_types<Model>()&&
    check_storable_types<Model>(is_unordered_collection<Model>{});
}
  
template<typename Model>
constexpr bool check_acceptability()
{
  return check_acceptability<Model>(is_closed_collection<Model>{});
}

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
