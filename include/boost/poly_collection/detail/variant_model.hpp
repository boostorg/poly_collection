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

#include <boost/mp11/list.hpp>
#include <boost/poly_collection/detail/allocator_adaptor.hpp>
#include <boost/poly_collection/detail/fixed_variant.hpp>
#include <boost/poly_collection/detail/packed_segment.hpp>
#include <boost/poly_collection/detail/segment.hpp>
#include <boost/poly_collection/detail/segment_backend.hpp>
#include <boost/poly_collection/detail/size_of.hpp>
#include <boost/poly_collection/detail/stride_iterator.hpp>
#include <boost/poly_collection/detail/variant_polymorphism.hpp>
#include <cstddef>
#include <memory>

namespace boost{

namespace poly_collection{

namespace detail{

/* model for variant_collection */

template<typename Variant,typename T>
using variant_storage_final_type=
  fixed_variant_impl::fixed_variant_closure<T,Variant>;

struct variant_storage_iterator_traits /* for stride_iterator */
{
  template<typename Variant,typename T>
  using is_implementation=
    typename mp11::mp_rename<Variant,variant_polymorphism>::
      template is_implementation<T>;
  template<typename Variant,typename T>
  using iterator=static_stride_iterator<
    T,size_of<variant_storage_final_type<Variant,T>>::value>;

#include <boost/poly_collection/detail/begin_no_sanitize.hpp>
  template<typename Iterator,typename Variant>
  static BOOST_POLY_COLLECTION_NO_SANITIZE Iterator make_iterator(
    Variant* p)noexcept
  {
    return p?
      std::addressof(unsafe_get<typename Iterator::value_type>(*p)):
      nullptr;
  }
#include <boost/poly_collection/detail/end_no_sanitize.hpp>

  template<typename Variant,typename Iterator>
  static const Variant* base_pointer_from(const Iterator& it)noexcept
  {
    return reinterpret_cast<
      const variant_storage_final_type<
        Variant,typename Iterator::value_type>*>(it.operator->());
  }

  template<typename T,std::size_t Stride>
  static std::size_t stride_from(
    const static_stride_iterator<T,Stride>&)noexcept
  {
    return Stride;
  }
};

template<typename Allocator,typename... Ts>
struct variant_storage
{
  using value_type=typename variant_polymorphism<Ts...>::value_type;
  using allocator_type=Allocator;
  using segment_allocator_type=allocator_adaptor<Allocator>;
  using base_iterator=
    stride_iterator<value_type,variant_storage_iterator_traits>;
  using const_base_iterator=
    stride_iterator<const value_type,variant_storage_iterator_traits>;
  using base_sentinel=value_type*;
  using const_base_sentinel=const value_type*;
  template<typename T>
  using iterator=typename variant_storage_iterator_traits::
    template iterator<value_type,T>;
  template<typename T>
  using const_iterator=typename variant_storage_iterator_traits::
    template iterator<value_type,const T>;
  using segment_backend=detail::segment_backend<variant_storage>;
  template<typename T>
  using segment_backend_implementation=
    packed_segment<variant_storage,T>;

  static base_iterator nonconst_iterator(const_base_iterator it)noexcept
  {
    return base_iterator{
      const_cast<value_type*>(static_cast<const value_type*>(it)),it.stride()};
  }

  template<typename T>
  static iterator<T> nonconst_iterator(const_iterator<T> it)noexcept
  {
    return {const_cast<T*>(static_cast<const T*>(it))};
  }

private:
  template<typename,typename>
  friend class packed_segment;

  template<typename T>
  using final_type=variant_storage_final_type<value_type,T>;

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
  using value_type=typename variant_polymorphism<Ts...>::value_type;

  using segment=detail::segment<variant_model>;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
