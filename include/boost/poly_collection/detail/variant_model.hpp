/* Copyright 2024-2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_VARIANT_MODEL_HPP
#define BOOST_POLY_COLLECTION_DETAIL_VARIANT_MODEL_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/poly_collection/detail/allocator_adaptor.hpp>
#include <boost/poly_collection/detail/fixed_variant_iterator.hpp>
#include <boost/poly_collection/detail/packed_segment.hpp>
#include <boost/poly_collection/detail/segment.hpp>
#include <boost/poly_collection/detail/segment_backend.hpp>
#include <boost/poly_collection/detail/variant_polymorphism.hpp>

namespace boost{

namespace poly_collection{

namespace detail{

/* model for variant_collection */

template<typename Allocator,typename... Ts>
struct variant_storage
{
  using value_type=fixed_variant_impl::fixed_variant<Ts...>;
  using allocator_type=Allocator;
  using segment_allocator_type=allocator_adaptor<Allocator>;
  using base_iterator=fixed_variant_iterator<value_type>;
  using const_base_iterator=fixed_variant_iterator<const value_type>;
  using base_sentinel=value_type*;
  using const_base_sentinel=const value_type*;
  template<typename T>
  using iterator=fixed_variant_alternative_iterator<value_type,T>;
  template<typename T>
  using const_iterator=fixed_variant_alternative_iterator<value_type,const T>;
  using segment_backend=detail::segment_backend<variant_storage>;
  template<typename T>
  using segment_backend_implementation=
    packed_segment<variant_storage,T>;

  static base_iterator nonconst_iterator(const_base_iterator it)
  {
    return base_iterator{
      const_cast<value_type*>(static_cast<const value_type*>(it)),it.stride()};
  }

  template<typename T>
  static iterator<T> nonconst_iterator(const_iterator<T> it)
  {
    return {const_cast<T*>(static_cast<const T*>(it))};
  }

private:
  template<typename,typename>
  friend class packed_segment;

  template<typename T>
  using final_type=fixed_variant_impl::fixed_variant_closure<
     T,fixed_variant_impl::fixed_variant<Ts...>>;

  template<typename T>
  static const value_type* value_ptr(const T* p)noexcept
  {
    return reinterpret_cast<const final_type<T>*>(p);
  }
};

template<typename Allocator,typename... Ts>
struct variant_model:
  variant_polymorphism<Ts...>,variant_storage<Allocator,Ts...>
{
  /* disambiguate multiply-inherited, identical typedef */
  using value_type=fixed_variant_impl::fixed_variant<Ts...>;

  using segment=detail::segment<variant_model>;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
