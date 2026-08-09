/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_PROXY_ITERATOR_HPP
#define BOOST_POLY_COLLECTION_DETAIL_PROXY_ITERATOR_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/iterator/iterator_adaptor.hpp>
#include <type_traits>

namespace boost{

namespace poly_collection{

namespace detail{

/* Proxy* adaptor convertible to pointer to the entity T* the proxy stands for.
 * Traits provides user-defined machinery for interoperability with T*.
 */

template<typename Proxy,typename Traits>
class proxy_iterator:
  public boost::iterator_adaptor<proxy_iterator<Proxy,Traits>,Proxy*>
{
public:
  proxy_iterator()=default;
  explicit proxy_iterator(Proxy* p)noexcept:
    proxy_iterator::iterator_adaptor_{p}{}
  proxy_iterator(const proxy_iterator&)=default;
  proxy_iterator& operator=(const proxy_iterator&)=default;

  template<
    typename NonConstProxy,
    typename std::enable_if<
      std::is_same<Proxy,const NonConstProxy>::value>::type* =nullptr
  >
  proxy_iterator(const proxy_iterator<NonConstProxy,Traits>& x)noexcept:
    proxy_iterator::iterator_adaptor_{x.base()}{}

  template<
    typename NonConstProxy,
    typename std::enable_if<
      std::is_same<Proxy,const NonConstProxy>::value>::type* =nullptr
  >
  proxy_iterator& operator=(
    const proxy_iterator<NonConstProxy,Traits>& x)noexcept
  {
    this->base_reference()=x.base();
    return *this;
  }

  /* interoperability with T* */

  template<
    typename T,
    typename NonConstProxy=typename std::remove_const<Proxy>::type,
    typename std::enable_if<
      Traits::template is_implementation<NonConstProxy,T>::value&&
      (!std::is_const<Proxy>::value||std::is_const<T>::value)
    >::type* =nullptr
  >
  explicit operator T*()const noexcept
  {
    return const_cast<T*>(
      static_cast<const T*>(Traits::target_address(this->base())));
  }

  /* comparability with Proxy* sentinel (if used) */

  operator Proxy*()const noexcept{return this->base();}

  /* nullification used by poly_collection global iterators */

  proxy_iterator& operator=(std::nullptr_t)noexcept
  {
    this->base_reference()=nullptr;
    return *this;
  }

private:
  template<typename,typename>
  friend class proxy_iterator;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
