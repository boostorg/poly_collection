/* Copyright 2016 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_IS_CALLABLE_HPP
#define BOOST_POLY_COLLECTION_DETAIL_IS_CALLABLE_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <functional>
#include <type_traits>

/* technique explained at
 * http://bannalia.blogspot.com/2016/09/compile-time-checking-existence-of.html 
 */

namespace boost{
namespace poly_collection{
namespace detail{
namespace is_callable_fallback{

template <typename,typename> struct is_callable;
template <typename F,typename... Args,typename R>
struct is_callable<F(Args...), R>:
  std::is_constructible<
    std::function<R(Args...)>,
    std::reference_wrapper<typename std::remove_reference<F>::type>
  >
{};

struct hook{};

}}}}

namespace std{

template<>
struct is_void<::boost::poly_collection::detail::is_callable_fallback::hook>:
  std::false_type
{      
  template<typename T,typename R>
  static constexpr bool is_callable_f()
  {
    using namespace ::boost::poly_collection::detail::is_callable_fallback;
    return is_callable<T,R>::value;
  }
};

} /* namespace std */

namespace boost{

namespace poly_collection{

namespace detail{

template<typename T,typename R=void>
struct is_callable:std::integral_constant<
  bool,
  std::is_void<is_callable_fallback::hook>::template is_callable_f<T,R>()
>{};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
