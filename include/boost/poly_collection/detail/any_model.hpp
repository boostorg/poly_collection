/* Copyright 2016-2017 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_ANY_MODEL_HPP
#define BOOST_POLY_COLLECTION_DETAIL_ANY_MODEL_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/core/addressof.hpp>
#include <boost/mpl/vector/vector10.hpp>
#include <boost/poly_collection/detail/any_iterator.hpp>
#include <boost/poly_collection/detail/is_acceptable.hpp>
#include <boost/poly_collection/detail/segment_backend.hpp>
#include <boost/poly_collection/detail/split_segment.hpp>
#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/any_cast.hpp>
#include <boost/type_erasure/builtin.hpp>
#include <boost/type_erasure/is_subconcept.hpp>
#include <boost/type_erasure/relaxed.hpp>
#include <boost/type_erasure/typeid_of.hpp>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace boost{

namespace poly_collection{

namespace detail{

/* model for any_collection */

template<typename Concept>
struct any_model;

/* Refine is_acceptable to cover type_erasure::any classes whose assignment
 * operator won't compile.
 */

template<typename Concept,typename Concept2,typename T>
struct is_acceptable<
  type_erasure::any<Concept2,T>,any_model<Concept>,
  typename std::enable_if<
    !type_erasure::is_relaxed<Concept2>::value&&
    !type_erasure::is_subconcept<type_erasure::assignable<>,Concept2>::value
  >::type
>:std::false_type{};

/* is_terminal defined out-class to allow for partial specialization */

template<typename T,typename=void>
struct any_model_is_terminal:std::true_type{};

template<typename Concept,typename T>
struct any_model_is_terminal<
  type_erasure::any<Concept,T>,
  typename std::enable_if<
    type_erasure::is_subconcept<type_erasure::typeid_<>,Concept>::value
  >::type
>:std::false_type{};

template<typename Concept>
struct any_model
{
  using value_type=type_erasure::any<
    typename std::conditional<
      type_erasure::is_subconcept<type_erasure::typeid_<>,Concept>::value,
      Concept,
      mpl::vector2<Concept,type_erasure::typeid_<>>
    >::type,
    type_erasure::_self&
  >;

  template<typename Concrete>
  using is_subtype=std::true_type; /* can't compile-time check concept
                                    * compliance */
  template<typename T>
  using is_terminal=any_model_is_terminal<T>;

  template<typename T>
  static const std::type_info& subtypeid(const T&){return typeid(T);}

  template<
    typename Concept2,typename T,
    typename std::enable_if<
      type_erasure::is_subconcept<type_erasure::typeid_<>,Concept2>::value
    >::type* = nullptr
  >
  static const std::type_info& subtypeid(
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
    typename std::enable_if<
      type_erasure::is_subconcept<type_erasure::typeid_<>,Concept2>::value
    >::type* =nullptr
  >
  static void* subaddress(type_erasure::any<Concept2,T>& a)
  {
    return type_erasure::any_cast<void*>(&a);
  }

  template<
    typename Concept2,typename T,
    typename std::enable_if<
      type_erasure::is_subconcept<type_erasure::typeid_<>,Concept2>::value
    >::type* =nullptr
  >
  static const void* subaddress(const type_erasure::any<Concept2,T>& a)
  {
    return type_erasure::any_cast<const void*>(&a);
  }

  using base_iterator=any_iterator<value_type>;
  using const_base_iterator=any_iterator<const value_type>;
  using base_sentinel=value_type*;
  using const_base_sentinel=const value_type*;
  template<typename Concrete>
  using iterator=Concrete*;
  template<typename Concrete>
  using const_iterator=const Concrete*;
  using segment_backend=detail::segment_backend<any_model>;

  static base_iterator nonconst_iterator(const_base_iterator it)
  {
    return base_iterator{
      const_cast<value_type*>(static_cast<const value_type*>(it))};
  }

  template<typename T>
  static iterator<T> nonconst_iterator(const_iterator<T> it)
  {
    return const_cast<iterator<T>>(it);
  }

  template<typename Concrete,typename Allocator>
  static segment_backend* make(const Allocator& al)
  {
    return new split_segment<
      any_model,
      Concrete,
      typename std::allocator_traits<Allocator>::
        template rebind_alloc<Concrete>
    >{al};
  }
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
