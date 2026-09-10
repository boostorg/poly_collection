/* Copyright 2016-2026 Joaquin M Lopez Munoz.
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

#include <boost/poly_collection/detail/allocator_adaptor.hpp>
#include <boost/poly_collection/detail/any_polymorphism.hpp>
#include <boost/poly_collection/detail/is_storable.hpp>
#include <boost/poly_collection/detail/make_any.hpp>
#include <boost/poly_collection/detail/proxy_iterator.hpp>
#include <boost/poly_collection/detail/segment.hpp>
#include <boost/poly_collection/detail/segment_backend.hpp>
#include <boost/poly_collection/detail/split_segment.hpp>
#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/any_cast.hpp>
#include <boost/type_erasure/builtin.hpp>
#include <boost/type_erasure/is_subconcept.hpp>
#include <boost/type_erasure/relaxed.hpp>
#include <type_traits>

namespace boost{

namespace poly_collection{

namespace detail{

/* model for any_collection */

struct any_storage_iterator_traits /* for proxy_iterator */
{
  /* can't compile-time check concept compliance, see any_polymorphism */
  template<typename Any,typename Concrete>
  using is_implementation=std::true_type;

  template<typename Any>
  static const void* target_address(Any* p)noexcept
  {
    return type_erasure::any_cast<const void*>(p);
  }
};

template<typename Concept,typename Allocator>
struct any_model;

/* Specialize is_storable to deal with type_erasure::any classes whose
 * nominally existing assignment operator won't compile.
 */

template<typename Concept,typename Allocator,typename Concept2,typename T>
struct is_storable<type_erasure::any<Concept2,T>,any_model<Concept,Allocator>>:
  std::integral_constant<
    bool,
    type_erasure::is_relaxed<Concept2>::value||
    type_erasure::is_subconcept<type_erasure::assignable<>,Concept2>::value||
    type_erasure::is_subconcept<
      type_erasure::assignable<type_erasure::_self,type_erasure::_self&&>,
      Concept2>::value
  >
{};

template<typename Concept,typename Allocator>
struct any_storage
{
  using value_type=any_polymorphism_value_type<Concept>;
  using allocator_type=Allocator;
  using segment_allocator_type=allocator_adaptor<Allocator>;
  using base_iterator=
    proxy_iterator<value_type,any_storage_iterator_traits>;
  using const_base_iterator=
    proxy_iterator<const value_type,any_storage_iterator_traits>;
  using base_sentinel=value_type*;
  using const_base_sentinel=const value_type*;
  template<typename Concrete>
  using iterator=Concrete*;
  template<typename Concrete>
  using const_iterator=const Concrete*;
  using segment_backend=detail::segment_backend<any_storage>;
  template<typename Concrete>
  using segment_backend_implementation=
    split_segment<any_storage,Concrete>;

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

private:
  template<typename,typename>
  friend class split_segment;

  template<typename Concrete>
  static value_type make_value_type(Concrete& x)
  {
    return make_any<value_type>(x);
  }
};

template<typename Concept,typename Allocator>
struct any_model:any_polymorphism<Concept>,any_storage<Concept,Allocator>
{
  /* disambiguate multiply-inherited, identical typedef */
  using value_type=typename any_polymorphism<Concept>::value_type;

  using segment=detail::segment<any_model>;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
