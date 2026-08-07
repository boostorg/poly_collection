/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_ANY_CLOSURE_HPP
#define BOOST_POLY_COLLECTION_DETAIL_ANY_CLOSURE_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/poly_collection/detail/make_any.hpp>
#include <type_traits>
#include <utility>

namespace boost{

namespace poly_collection{

namespace detail{

/* any_closure<Concrete,Any> stores a Concrete immediately followed by a
 * type_erasure::any referring to it. Used for packed segments.
 */

template<typename Concrete>
struct any_store
{
  template<typename... Args>
  any_store(Args&&... args):value{std::forward<Args>(args)...}{}

  Concrete value;
};

template<typename Concrete,typename Any>
class any_closure:public any_store<Concrete>,public Any
{
  using store=any_store<Concrete>;

public:
  template<
    typename... Args,
    typename std::enable_if<
      std::is_constructible<Concrete,Args&&...>::value
    >::type* =nullptr
  >
  explicit any_closure(Args&&... args):
    store{std::forward<Args>(args)...},
    Any{make_any<Any>(this->value)}
    {}

  any_closure(const any_closure& x):
    store{x.value},Any{make_any<Any>(this->value)}{}

  any_closure(any_closure&& x):
    store{std::move(x.value)},Any{make_any<Any>(this->value)}{}

  any_closure& operator=(const any_closure& x)
  {
    this->value=x.value;
    return *this;
  }

  any_closure& operator=(any_closure&& x)
  {
    this->value=std::move(x.value);
    return *this;
  }
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
