/* Copyright 2024-2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_IS_STORABLE_HPP
#define BOOST_POLY_COLLECTION_DETAIL_IS_STORABLE_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/poly_collection/detail/is_unordered_collection.hpp>
#include <type_traits>

namespace boost{

namespace poly_collection{

namespace detail{

/* is_storable<T, Model> determines if T can be placed in the type of segments
 * used by Model.
 * Note we're conflating unorderedness with stability (i.e. with not requiring
 * moveability). We may revisit this in the future.
 * The !std::is_abstract check is needed to prevent base_unordered_collection
 * from instantiating segment<C> (and potentially failing to do so) when a 
 * reference to an abstract class C is passed in an insertion operation. Such
 * a segment, of course, would never be created at run time.
 * is_storable can be further specialized by a Model when the std type_traits
 * classes fail to give the right info (as it can happen with class templates 
 * whose nominally existing operators do not compile for certain 
 * instantiations).
 */

template<typename T,typename Model>
struct is_storable:std::integral_constant<
  bool,
  !std::is_const<T>::value && !std::is_volatile<T>::value && 
  !std::is_function<T>::value && !std::is_reference<T>::value && 
  !std::is_void<T>::value&&
  (is_unordered_collection<Model>::value?
    !std::is_abstract<T>::value:
    std::is_move_constructible<T>::value&&
    (std::is_move_assignable<T>::value||
     std::is_nothrow_move_constructible<T>::value))
>{};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
