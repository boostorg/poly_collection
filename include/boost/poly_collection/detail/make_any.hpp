/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_MAKE_ANY_HPP
#define BOOST_POLY_COLLECTION_DETAIL_MAKE_ANY_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/mpl/map/map10.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/binding.hpp>
#include <boost/type_erasure/concept_of.hpp>
#include <boost/type_erasure/static_binding.hpp>

namespace boost{

namespace poly_collection{

namespace detail{

/* make_any<Any>(x) creates an Any referring to x, where Any is a
 * type_erasure::any<...>. If x is in its turn a type_erasure::any, its
 * referee is extracted and passed to the constructed Any.
 */

template<typename T,typename Q>
struct make_any_reference
{
  static T& apply(Q& x){return x;}
};

template<typename Any,typename Concrete>
Any make_any(Concrete& x){return Any{x};}

template<typename Any,typename Concept,typename T>
Any make_any(type_erasure::any<Concept,T>& x)
{
  /* I don't pretend to understand what's going on here, see
   * https://lists.boost.org/boost-users/2017/05/87556.php
   */

  using ref_type=type_erasure::any<Concept,T>;
  using make_ref=make_any_reference<type_erasure::_self,ref_type>;
  using concept_=typename type_erasure::concept_of<Any>::type;

  auto b=type_erasure::make_binding<
    mpl::map1<mpl::pair<type_erasure::_self,ref_type>>>();

  return {
    type_erasure::call(type_erasure::binding<make_ref>{b},make_ref{},x),
    type_erasure::binding<concept_>{b}
  };
}

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
