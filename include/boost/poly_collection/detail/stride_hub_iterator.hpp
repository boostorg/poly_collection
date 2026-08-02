/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_STRIDE_HUB_ITERATOR_HPP
#define BOOST_POLY_COLLECTION_DETAIL_STRIDE_HUB_ITERATOR_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/config.hpp>
#include <boost/core/pointer_traits.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/poly_collection/detail/base_offset.hpp>
#include <boost/poly_collection/detail/hub_access.hpp>
#include <cstddef>
#include <memory>
#include <type_traits>

#if defined(BOOST_MSVC)
#pragma warning(push)
#pragma warning(disable:4714) /* marked as __forceinline not inlined */
#endif

namespace boost{

namespace poly_collection{

namespace detail{

/* Iterator over the data structure of a boost::container::hub<Derived> where
 * Value is a base of Derived.
 */

template<typename Value>
class stride_hub_iterator:
  public boost::iterator_facade<
    stride_hub_iterator<Value>,
    Value,
    boost::bidirectional_traversal_tag
  >
{
public:
  stride_hub_iterator()=default;
  stride_hub_iterator(
    hub_block_base* pbb,int n,
    std::size_t stride,std::ptrdiff_t offset)noexcept:
    pbb{pbb},n{n},stride_{stride},offset_{offset}{}
  stride_hub_iterator(const stride_hub_iterator&)=default;
  stride_hub_iterator& operator=(const stride_hub_iterator&)=default;

  template<
    typename NonConstValue,
    typename std::enable_if<
      std::is_same<Value,const NonConstValue>::value>::type* =nullptr
  >
  stride_hub_iterator(const stride_hub_iterator<NonConstValue>& x)noexcept:
    pbb{x.pbb},n{x.n},stride_{x.stride_},offset_{x.offset_}{}

  template<
    typename NonConstValue,
    typename std::enable_if<
      std::is_same<Value,const NonConstValue>::value>::type* =nullptr
  >
  stride_hub_iterator& operator=(
    const stride_hub_iterator<NonConstValue>& x)noexcept
  {
    pbb=x.pbb;n=x.n;stride_=x.stride_;offset_=x.offset_;
    return *this;
  }

  /* interoperability with boost::container::hub iterators */

  template<
    typename ValuePointer,
    typename Element=typename pointer_traits<ValuePointer>::element_type,
    typename std::enable_if<
      std::is_base_of<
        typename std::remove_const<Value>::type,
        typename std::remove_const<Element>::type>::value&&
      (std::is_const<Value>::value||!std::is_const<Element>::value)
    >::type* = nullptr
  >
  explicit stride_hub_iterator(const hub_iterator<ValuePointer>& x)noexcept:
    stride_hub_iterator{
      get_members(x),
      sizeof(Element),
      base_offset<
        typename std::remove_const<Value>::type,
        typename std::remove_const<Element>::type>()}
    {}

  template<
    typename ValuePointer,
    typename Element=typename pointer_traits<ValuePointer>::element_type,
    typename std::enable_if<
      std::is_base_of<
        typename std::remove_const<Value>::type,
        typename std::remove_const<Element>::type>::value&&
      (!std::is_const<Value>::value||std::is_const<Element>::value)
    >::type* = nullptr
  >
  explicit operator hub_iterator<ValuePointer>()const noexcept
  {
    return make_hub_iterator<hub_iterator<ValuePointer>>({pbb,n});
  }

  /* nullification used by poly_collection global iterators */

  stride_hub_iterator& operator=(std::nullptr_t)
  {
    return *this=stride_hub_iterator{};
  }

  hub_block_base* block()const noexcept{return pbb;}
  int             slot()const noexcept{return n;}
  std::size_t     stride()const noexcept{return stride_;}
  std::ptrdiff_t  offset()const noexcept{return offset_;}

private:
  template<typename>
  friend class stride_hub_iterator;
  friend class boost::iterator_core_access;

  stride_hub_iterator(
    hub_iterator_members m,std::size_t stride,std::ptrdiff_t offset)noexcept:
    pbb{m.pbb},n{m.n},stride_{stride},offset_{offset}{}

#include <boost/poly_collection/detail/begin_no_sanitize.hpp>

  BOOST_POLY_COLLECTION_NO_SANITIZE
  BOOST_FORCEINLINE Value& dereference()const noexcept
  {
    auto p=static_cast<char*>(
      static_cast<const hub_type_erased_block*>(pbb)->data_);
    return *reinterpret_cast<Value*>(
      p+static_cast<std::size_t>(n)*stride_+offset_);
  }

#include <boost/poly_collection/detail/end_no_sanitize.hpp>

  bool equal(const stride_hub_iterator& x)const noexcept
    {return pbb==x.pbb&&n==x.n;}

  BOOST_FORCEINLINE void increment()noexcept{detail::increment(pbb,n);}
  BOOST_FORCEINLINE void decrement()noexcept{detail::decrement(pbb,n);}

  hub_block_base* pbb=nullptr;
  int             n=0;
  std::size_t     stride_=0;
  std::ptrdiff_t  offset_=0;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#if defined(BOOST_MSVC)
#pragma warning(pop) /* C4714 */
#endif

#endif
