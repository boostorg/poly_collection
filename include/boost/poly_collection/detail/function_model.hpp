/* Copyright 2016-2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_FUNCTION_MODEL_HPP
#define BOOST_POLY_COLLECTION_DETAIL_FUNCTION_MODEL_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/poly_collection/detail/allocator_adaptor.hpp>
#include <boost/poly_collection/detail/callable_wrapper.hpp>
#include <boost/poly_collection/detail/callable_wrapper_iterator.hpp>
#include <boost/poly_collection/detail/function_polymorphism.hpp>
#include <boost/poly_collection/detail/segment.hpp>
#include <boost/poly_collection/detail/segment_backend.hpp>
#include <boost/poly_collection/detail/split_segment.hpp>

namespace boost{

namespace poly_collection{

namespace detail{

/* model for function_collection */

template<typename Signature,typename Allocator>
struct function_storage;

template<typename Signature,typename Allocator>
struct function_model;

template<typename R,typename... Args,typename Allocator>
struct function_storage<R(Args...),Allocator>
{
  using value_type=callable_wrapper<R(Args...)>;
  using allocator_type=Allocator;
  using segment_allocator_type=allocator_adaptor<Allocator>;
  using base_iterator=callable_wrapper_iterator<value_type>;
  using const_base_iterator=callable_wrapper_iterator<const value_type>;
  using base_sentinel=value_type*;
  using const_base_sentinel=const value_type*;
  template<typename Callable>
  using iterator=Callable*;
  template<typename Callable>
  using const_iterator=const Callable*;
  using segment_backend=detail::segment_backend<function_storage>;
  template<typename Callable>
  using segment_backend_implementation=
    split_segment<function_storage,Callable>;

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

  template<typename Callable>
  static value_type make_value_type(Callable& x){return value_type{x};}
};

template<typename R,typename... Args,typename Allocator>
struct function_model<R(Args...),Allocator>:
  function_polymorphism<R(Args...)>,
  function_storage<R(Args...),Allocator>
{
  /* disambiguate multiply-inherited, identical typedef */
  using value_type=typename function_polymorphism<R(Args...)>::value_type;

  using segment=detail::segment<function_model>;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
