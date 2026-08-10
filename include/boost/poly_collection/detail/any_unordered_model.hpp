/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_ANY_UNORDERED_MODEL_HPP
#define BOOST_POLY_COLLECTION_DETAIL_ANY_UNORDERED_MODEL_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/container/hub.hpp>
#include <boost/poly_collection/detail/allocator_adaptor.hpp>
#include <boost/poly_collection/detail/any_closure.hpp>
#include <boost/poly_collection/detail/any_polymorphism.hpp>
#include <boost/poly_collection/detail/base_offset.hpp>
#include <boost/poly_collection/detail/packed_hub_segment.hpp>
#include <boost/poly_collection/detail/stride_hub_iterator.hpp>
#include <boost/poly_collection/detail/subvalue_hub_iterator.hpp>
#include <boost/poly_collection/detail/unordered_segment.hpp>
#include <boost/poly_collection/detail/unordered_segment_backend.hpp>

namespace boost{

namespace poly_collection{

namespace detail{

/* model for any_unordered_collection */

template<typename Any,typename Concrete>
using any_unordered_storage_final_type=
  any_closure<typename std::remove_const<Concrete>::type,Any>;

struct any_unordered_storage_iterator_traits: /* for stride_hub_iterator */
  subvalue_hub_iterator_traits
{
  /* can't compile-time check concept compliance, see any_polymorphism */
  template<typename Any,typename Concrete>
  using is_implementation=std::true_type;
  template<typename Any,typename Concrete>
  using iterator=subvalue_hub_iterator<
    any_unordered_storage_final_type<Any,Concrete>,Concrete>;
};

template<typename Concept,typename Allocator>
struct any_unordered_storage
{
  using value_type=any_polymorphism_value_type<Concept>;
  using allocator_type=Allocator;
  using segment_allocator_type=allocator_adaptor<Allocator>;
  using base_iterator=stride_hub_iterator<
    value_type,any_unordered_storage_iterator_traits>;
  using const_base_iterator=stride_hub_iterator<
    const value_type,any_unordered_storage_iterator_traits>;
  using base_sentinel=base_iterator;
  using const_base_sentinel=const_base_iterator;
  template<typename Concrete>
  using iterator=typename any_unordered_storage_iterator_traits::
    template iterator<value_type,Concrete>;
  template<typename Concrete>
  using const_iterator=typename any_unordered_storage_iterator_traits::
    template iterator<value_type,const Concrete>;
  using segment_backend=
    detail::unordered_segment_backend<any_unordered_storage>;
  template<typename Concrete>
  using segment_backend_implementation=
    packed_hub_segment<any_unordered_storage,Concrete>;

  static base_iterator nonconst_iterator(const_base_iterator it)
  {
    return {it.block(),it.slot(),it.stride(),it.offset()};
  }

  template<typename T>
  static iterator<T> nonconst_iterator(const_iterator<T> it)
  {
    return any_unordered_storage_iterator_traits::
      make_iterator<iterator<T>>(get_members(it));
  }

private:
  template<typename,typename>
  friend class packed_hub_segment;

  template<typename Concrete>
  using final_type=
    any_unordered_storage_final_type<value_type,Concrete>;

  template<typename Concrete>
  static std::ptrdiff_t value_offset()noexcept
  {
    return base_offset<value_type,final_type<Concrete>>();
  }
};

template<typename Concept,typename Allocator>
struct any_unordered_model:
  any_polymorphism<Concept>,any_unordered_storage<Concept,Allocator>
{
  /* disambiguate multiply-inherited, identical typedef */
  using value_type=typename any_polymorphism<Concept>::value_type;

  using segment=detail::unordered_segment<any_unordered_model>;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
