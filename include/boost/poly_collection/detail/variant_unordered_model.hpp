/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_VARIANT_UNORDERED_MODEL_HPP
#define BOOST_POLY_COLLECTION_DETAIL_VARIANT_UNORDERED_MODEL_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/mp11/list.hpp>
#include <boost/poly_collection/detail/allocator_adaptor.hpp>
#include <boost/poly_collection/detail/base_offset.hpp>
#include <boost/poly_collection/detail/fixed_variant.hpp>
#include <boost/poly_collection/detail/packed_hub_segment.hpp>
#include <boost/poly_collection/detail/stride_hub_iterator.hpp>
#include <boost/poly_collection/detail/subvalue_hub_iterator.hpp>
#include <boost/poly_collection/detail/unordered_segment.hpp>
#include <boost/poly_collection/detail/unordered_segment_backend.hpp>
#include <boost/poly_collection/detail/variant_polymorphism.hpp>
#include <cstddef>
#include <type_traits>

namespace boost{

namespace poly_collection{

namespace detail{

/* model for variant_unordered_collection */

template<typename Variant,typename T>
using variant_unordered_storage_final_type=
  fixed_variant_impl::fixed_variant_closure<
    typename std::remove_const<T>::type,Variant>;

struct variant_unordered_storage_iterator_traits: /* for stride_hub_iterator */
  subvalue_hub_iterator_traits
{
  template<typename Variant,typename T>
  using is_implementation=
    typename mp11::mp_rename<Variant,variant_polymorphism>::
      template is_implementation<T>;
  template<typename Variant,typename T>
  using iterator=subvalue_hub_iterator<
    variant_unordered_storage_final_type<Variant,T>,T>;
};

template<typename Allocator,typename... Ts>
struct variant_unordered_storage
{
  using value_type=fixed_variant_impl::fixed_variant<Ts...>;
  using allocator_type=Allocator;
  using segment_allocator_type=allocator_adaptor<Allocator>;
  using base_iterator=stride_hub_iterator<
    value_type,variant_unordered_storage_iterator_traits>;
  using const_base_iterator=stride_hub_iterator<
    const value_type,variant_unordered_storage_iterator_traits>;
  using base_sentinel=base_iterator;
  using const_base_sentinel=const_base_iterator;
  template<typename T>
  using iterator=typename variant_unordered_storage_iterator_traits::
    template iterator<value_type,T>;
  template<typename T>
  using const_iterator=typename variant_unordered_storage_iterator_traits::
    template iterator<value_type,const T>;
  using segment_backend=
    detail::unordered_segment_backend<variant_unordered_storage>;
  template<typename T>
  using segment_backend_implementation=
    packed_hub_segment<variant_unordered_storage,T>;

  static base_iterator nonconst_iterator(const_base_iterator it)
  {
    return {it.block(),it.slot(),it.stride(),it.offset()};
  }

  template<typename T>
  static iterator<T> nonconst_iterator(const_iterator<T> it)
  {
    return variant_unordered_storage_iterator_traits::
      make_iterator<iterator<T>>(get_members(it));
  }

private:
  template<typename,typename>
  friend class packed_hub_segment;

  template<typename T>
  using final_type=variant_unordered_storage_final_type<value_type,T>;

  template<typename T>
  static std::ptrdiff_t value_offset()noexcept
  {
    return base_offset<value_type,final_type<T>>();
  }
};

template<typename Allocator,typename... Ts>
struct variant_unordered_model:
  variant_polymorphism<Ts...>,variant_unordered_storage<Allocator,Ts...>
{
  /* disambiguate multiply-inherited, identical typedef */
  using value_type=fixed_variant_impl::fixed_variant<Ts...>;

  using segment=detail::unordered_segment<variant_unordered_model>;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
