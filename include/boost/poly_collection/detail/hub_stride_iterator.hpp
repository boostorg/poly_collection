/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_HUB_STRIDE_ITERATOR_HPP
#define BOOST_POLY_COLLECTION_DETAIL_HUB_STRIDE_ITERATOR_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/config.hpp>
#include <boost/core/pointer_traits.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/poly_collection/detail/base_offset.hpp>
#include <boost/poly_collection/detail/hub_access.hpp>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace boost{

namespace poly_collection{

namespace detail{

/* Iterator over the data structure of a boost::container::hub<Derived> where
 * Value is a base of Derived.
 */

template<typename Value>
class hub_stride_iterator:
  public boost::iterator_facade<
    hub_stride_iterator<Value>,
    Value,
    boost::bidirectional_traversal_tag
  >
{
public:
  hub_stride_iterator()=default;
  hub_stride_iterator(
    hub_block_base* pbb,int n,
    std::size_t stride,std::ptrdiff_t offset)noexcept:
    pbb{pbb},n{n},stride_{stride},offset_{offset}{}
  hub_stride_iterator(const hub_stride_iterator&)=default;
  hub_stride_iterator& operator=(const hub_stride_iterator&)=default;

  template<
    typename NonConstValue,
    typename std::enable_if<
      std::is_same<Value,const NonConstValue>::value>::type* =nullptr
  >
  hub_stride_iterator(const hub_stride_iterator<NonConstValue>& x)noexcept:
    pbb{x.pbb},n{x.n},stride_{x.stride_},offset_{x.offset_}{}

  template<
    typename NonConstValue,
    typename std::enable_if<
      std::is_same<Value,const NonConstValue>::value>::type* =nullptr
  >
  hub_stride_iterator& operator=(
    const hub_stride_iterator<NonConstValue>& x)noexcept
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
  explicit hub_stride_iterator(const hub_iterator<ValuePointer>& x)noexcept:
    hub_stride_iterator{
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
  explicit operator
  boost::container::hub_detail::iterator<ValuePointer>()const noexcept
  {
    return make_hub_iterator<hub_iterator<ValuePointer>>({pbb,n});
  }

  /* nullification used by poly_collection global iterators */

  hub_stride_iterator& operator=(std::nullptr_t)
  {
    return *this=hub_stride_iterator{};
  }

  hub_block_base* block()const noexcept{return pbb;}
  int             slot()const noexcept{return n;}
  std::size_t     stride()const noexcept{return stride_;}
  std::ptrdiff_t  offset()const noexcept{return offset_;}

private:
  template<typename>
  friend class hub_stride_iterator;
  friend class boost::iterator_core_access;

  hub_stride_iterator(
    hub_iterator_members m,std::size_t stride,std::ptrdiff_t offset)noexcept:
    pbb{m.pbb},n{m.n},stride_{stride},offset_{offset}{}

#include <boost/poly_collection/detail/begin_no_sanitize.hpp>

  BOOST_POLY_COLLECTION_NO_SANITIZE
  Value& dereference()const noexcept
  {
    auto p=static_cast<char*>(
      static_cast<const hub_type_erased_block*>(pbb)->data_);
    return *reinterpret_cast<Value*>(
      p+static_cast<std::size_t>(n)*stride_+offset_);
  }

#include <boost/poly_collection/detail/end_no_sanitize.hpp>

  bool equal(const hub_stride_iterator& x)const noexcept
    {return pbb==x.pbb&&n==x.n;}

  void increment()noexcept
  {
    constexpr auto full=hub_block_base::full;
    auto mask=pbb->mask&(full<<1<<n);
    if(BOOST_UNLIKELY(mask==0)){
      pbb=pbb->next;
      mask=pbb->mask;
    }
    n=boost::container::dtl::unchecked_countr_zero(mask);
  }

  void decrement()noexcept
  {
    constexpr auto full=hub_block_base::full;
    constexpr int  N=hub_block_base::N;
    auto mask=pbb->mask&(full>>1>>(N-1-n));
    if(BOOST_UNLIKELY(mask==0)){
      pbb=pbb->prev;
      mask=pbb->mask;
    }
    n=N-1-boost::container::dtl::unchecked_countl_zero(mask);
  }

  hub_block_base* pbb=nullptr;
  int             n=0;
  std::size_t     stride_=0;
  std::ptrdiff_t  offset_=0;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
