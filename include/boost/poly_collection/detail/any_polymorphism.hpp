/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_ANY_POLYMORPHISM_HPP
#define BOOST_POLY_COLLECTION_DETAIL_ANY_POLYMORPHISM_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/core/addressof.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/any_cast.hpp>
#include <boost/type_erasure/builtin.hpp>
#include <boost/type_erasure/is_subconcept.hpp>
#include <boost/type_erasure/typeid_of.hpp>
#include <type_traits>
#include <typeinfo>

namespace boost{

namespace poly_collection{

namespace detail{

/* any polymorphism model */

template<typename Concept>
struct any_polymorphism;

/* is_terminal defined out-class to allow for partial specialization */

template<typename Concept,typename T>
using any_polymorphism_enable_if_has_typeid_=typename std::enable_if<
  type_erasure::is_subconcept<
    type_erasure::typeid_<typename std::decay<T>::type>,
    Concept
  >::value
>::type*;

template<typename T,typename=void*>
struct any_polymorphism_is_terminal:std::true_type{};

template<typename Concept,typename T>
struct any_polymorphism_is_terminal<
  type_erasure::any<Concept,T>,
  any_polymorphism_enable_if_has_typeid_<Concept,T>
>:std::false_type{};

template<typename Concept>
using any_polymorphism_value_type=type_erasure::any<
  typename std::conditional<
    type_erasure::is_subconcept<type_erasure::typeid_<>,Concept>::value,
    Concept,
    mpl::vector2<Concept,type_erasure::typeid_<>>
  >::type,
  type_erasure::_self&
>;

template<typename Concept>
struct any_polymorphism
{
  using value_type=any_polymorphism_value_type<Concept>;

  using type_index=std::type_info;

  template<typename Concrete>
  using is_implementation=std::true_type; /* can't compile-time check concept
                                           * compliance */
  template<typename T>
  using is_terminal=any_polymorphism_is_terminal<T>;

  template<typename T>
  static const std::type_info& index(){return typeid(T);}

  template<typename T>
  static const std::type_info& subindex(const T&){return typeid(T);}

  template<
    typename Concept2,typename T,
    any_polymorphism_enable_if_has_typeid_<Concept2,T> =nullptr
  >
  static const std::type_info& subindex(
    const type_erasure::any<Concept2,T>& a)
  {
    return type_erasure::typeid_of(a);
  }

  template<typename T>
  static void* subaddress(T& x){return boost::addressof(x);}

  template<typename T>
  static const void* subaddress(const T& x){return boost::addressof(x);}

  template<
    typename Concept2,typename T,
    any_polymorphism_enable_if_has_typeid_<Concept2,T> =nullptr
  >
  static void* subaddress(type_erasure::any<Concept2,T>& a)
  {
    return type_erasure::any_cast<void*>(&a);
  }

  template<
    typename Concept2,typename T,
    any_polymorphism_enable_if_has_typeid_<Concept2,T> =nullptr
  >
  static const void* subaddress(const type_erasure::any<Concept2,T>& a)
  {
    return type_erasure::any_cast<const void*>(&a);
  }
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
