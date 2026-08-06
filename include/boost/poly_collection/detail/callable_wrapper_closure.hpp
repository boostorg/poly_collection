/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_CALLABLE_WRAPPER_CLOSURE_HPP
#define BOOST_POLY_COLLECTION_DETAIL_CALLABLE_WRAPPER_CLOSURE_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <type_traits>
#include <utility>

namespace boost{

namespace poly_collection{

namespace detail{

/* callable_wrapper_closure<Callable,Wrapper> stores a Callable immediately
 * followed by a a callable_wrapper referring to it. Used for packed segments.
 */

template<typename Callable>
struct callable_wrapper_store
{
  template<typename... Args>
  callable_wrapper_store(Args&&... args):value{std::forward<Args>(args)...}{}

  Callable value;
};

template<typename Callable,typename Wrapper>
class callable_wrapper_closure:
  public callable_wrapper_store<Callable>,public Wrapper
{
  using store=callable_wrapper_store<Callable>;

public:
  template<
    typename... Args,
    typename std::enable_if<
      std::is_constructible<Callable,Args&&...>::value
    >::type* =nullptr
  >
  callable_wrapper_closure(Args&&... args)
    noexcept(std::is_nothrow_constructible<Callable,Args&&...>::value):
    store{std::forward<Args>(args)...},
    Wrapper{this->value}
    {}

  callable_wrapper_closure(const callable_wrapper_closure& x)
    noexcept(std::is_nothrow_copy_constructible<Callable>::value):
    store{x.value},Wrapper{this->value}{}

  callable_wrapper_closure(callable_wrapper_closure&& x)
    noexcept(std::is_nothrow_move_constructible<Callable>::value):
    store{std::move(x.value)},Wrapper{this->value}{}

  callable_wrapper_closure& operator=(const callable_wrapper_closure& x)
    noexcept(std::is_nothrow_copy_assignable<Callable>::value)
  {
    this->value=x.value;
    return *this;
  }

  callable_wrapper_closure& operator=(callable_wrapper_closure&& x)
    noexcept(std::is_nothrow_move_assignable<Callable>::value)
  {
    this->value=std::move(x.value);
    return *this;
  }
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
