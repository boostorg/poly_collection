/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_FUNCTION_UNORDERED_MODEL_HPP
#define BOOST_POLY_COLLECTION_DETAIL_FUNCTION_UNORDERED_MODEL_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/mp11/algorithm.hpp>
#include <boost/poly_collection/detail/allocator_adaptor.hpp>
#include <boost/poly_collection/detail/base_offset.hpp>
#include <boost/poly_collection/detail/callable_wrapper.hpp>
#include <boost/poly_collection/detail/callable_wrapper_closure.hpp>
#include <boost/poly_collection/detail/function_polymorphism.hpp>
#include <boost/poly_collection/detail/packed_hub_segment.hpp>
#include <boost/poly_collection/detail/stride_hub_iterator.hpp>
#include <boost/poly_collection/detail/subvalue_hub_iterator.hpp>
#include <boost/poly_collection/detail/unordered_segment.hpp>
#include <boost/poly_collection/detail/unordered_segment_backend.hpp>
#include <cstddef>
#include <type_traits>

namespace boost{

namespace poly_collection{

namespace detail{

/* model for function_unordered_collection */

template<typename CWrapper,typename Callable>
using function_unordered_storage_final_type=callable_wrapper_closure<
  typename std::remove_const<Callable>::type,CWrapper>;

/* for stride_hub_iterator */
struct function_unordered_storage_iterator_traits:
  subvalue_hub_iterator_traits
{
  template<typename CWrapper,typename Callable>
  using is_implementation=
    typename mp11::mp_rename<CWrapper,function_polymorphism>::
      template is_implementation<Callable>;
  template<typename CWrapper,typename Callable>
  using iterator=subvalue_hub_iterator<
    function_unordered_storage_final_type<CWrapper,Callable>,Callable>;
};

template<typename Signature,typename Allocator>
struct function_unordered_storage;

template<typename R,typename... Args,typename Allocator>
struct function_unordered_storage<R(Args...),Allocator>
{
  using value_type=callable_wrapper<R(Args...)>;
  using allocator_type=Allocator;
  using segment_allocator_type=allocator_adaptor<Allocator>;
  using base_iterator=stride_hub_iterator<
    value_type,function_unordered_storage_iterator_traits>;
  using const_base_iterator=stride_hub_iterator<
    const value_type,function_unordered_storage_iterator_traits>;
  using base_sentinel=base_iterator;
  using const_base_sentinel=const_base_iterator;
  template<typename Callable>
  using iterator=typename function_unordered_storage_iterator_traits::
    template iterator<value_type,Callable>;
  template<typename Callable>
  using const_iterator=typename function_unordered_storage_iterator_traits::
    template iterator<value_type,const Callable>;
  using segment_backend=
    detail::unordered_segment_backend<function_unordered_storage>;
  template<typename Callable>
  using segment_backend_implementation=
    packed_hub_segment<function_unordered_storage,Callable>;

  static base_iterator nonconst_iterator(const_base_iterator it)
  {
    return {it.block(),it.slot(),it.stride(),it.offset()};
  }

  template<typename T>
  static iterator<T> nonconst_iterator(const_iterator<T> it)
  {
    return function_unordered_storage_iterator_traits::
      make_iterator<iterator<T>>(get_members(it));
  }

private:
  template<typename,typename>
  friend class packed_hub_segment;

  template<typename Callable>
  using final_type=
    function_unordered_storage_final_type<value_type,Callable>;

  template<typename Callable>
  static std::ptrdiff_t value_offset()noexcept
  {
    return base_offset<value_type,final_type<Callable>>();
  }
};

template<typename Signature,typename Allocator>
struct function_unordered_model;

template<typename R,typename... Args,typename Allocator>
struct function_unordered_model<R(Args...),Allocator>:
  function_polymorphism<R(Args...)>,
  function_unordered_storage<R(Args...),Allocator>
{
  /* disambiguate multiply-inherited, identical typedef */
  using value_type=typename function_polymorphism<R(Args...)>::value_type;

  using segment=detail::unordered_segment<function_unordered_model>;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
