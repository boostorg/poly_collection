/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_FUNCTION_POLYMORPHISM_HPP
#define BOOST_POLY_COLLECTION_DETAIL_FUNCTION_POLYMORPHISM_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/core/addressof.hpp>
#include <boost/poly_collection/detail/callable_wrapper.hpp>
#include <boost/poly_collection/detail/is_invocable.hpp>
#include <type_traits>
#include <typeinfo>

namespace boost{

namespace poly_collection{

namespace detail{

/* function polymorphism model */

template<typename Signature>
struct function_polymorphism;

/* is_terminal defined out-class to allow for partial specialization */

template<typename T>
struct function_polymorphism_is_terminal:std::true_type{};

template<typename Signature>
struct function_polymorphism_is_terminal<callable_wrapper<Signature>>:
  std::false_type{};

template<typename R,typename... Args>
struct function_polymorphism<R(Args...)>
{
  using value_type=callable_wrapper<R(Args...)>;

  using type_index=std::type_info;

  template<typename Callable>
  using is_implementation=is_invocable_r<R,Callable&,Args...>;

  template<typename T>
  using is_terminal=function_polymorphism_is_terminal<T>;

  template<typename T>
  static const std::type_info& index(){return typeid(T);}

  template<typename T>
  static const std::type_info& subindex(const T&){return typeid(T);}

  template<typename Signature>
  static const std::type_info& subindex(
    const callable_wrapper<Signature>& f)
  {
    return f.target_type();
  }

  template<typename T>
  static void* subaddress(T& x){return boost::addressof(x);}

  template<typename T>
  static const void* subaddress(const T& x){return boost::addressof(x);}

  template<typename Signature>
  static void* subaddress(callable_wrapper<Signature>& f)
  {
    return f.data();
  }

  template<typename Signature>
  static const void* subaddress(const callable_wrapper<Signature>& f)
  {
    return f.data();
  }
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
