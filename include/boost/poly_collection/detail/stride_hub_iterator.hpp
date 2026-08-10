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
#include <boost/iterator/iterator_facade.hpp>
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

/* Bidirectional iterator over a sequence of Base subobjects in a
 * boost::container::hub of elements of undisclosed type Final.
 * stride=sizeof(Final) and the offset between Base and Final are passed at
 * run time. Traits provides user-defined machinery for interoperability with
 * associated typed iterators.
 */

template<typename Base,typename Traits>
class stride_hub_iterator:
  public boost::iterator_facade<
    stride_hub_iterator<Base,Traits>,
    Base,
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
    typename NonConstBase,
    typename std::enable_if<
      std::is_same<Base,const NonConstBase>::value>::type* =nullptr
  >
  stride_hub_iterator(
    const stride_hub_iterator<NonConstBase,Traits>& x)noexcept:
    pbb{x.pbb},n{x.n},stride_{x.stride_},offset_{x.offset_}{}

  template<
    typename NonConstBase,
    typename std::enable_if<
      std::is_same<Base,const NonConstBase>::value>::type* =nullptr
  >
  stride_hub_iterator& operator=(
    const stride_hub_iterator<NonConstBase,Traits>& x)noexcept
  {
    pbb=x.pbb;n=x.n;stride_=x.stride_;offset_=x.offset_;
    return *this;
  }

  /* interoperability with associated typed iterators */

  template<
    typename T,
    typename NonConstT=typename std::remove_const<T>::type,
    typename NonConstBase=typename std::remove_const<Base>::type,
    typename std::enable_if<
      Traits::template is_implementation<NonConstBase,NonConstT>::value&&
      (std::is_const<Base>::value||!std::is_const<T>::value)
    >::type* =nullptr
  >
  explicit stride_hub_iterator(
    const typename Traits::template iterator<NonConstBase,T>& x)noexcept:
    stride_hub_iterator{
      get_members(x),
      Traits::stride_from(x),
      Traits::template offset_from<NonConstBase>(x)}
    {}

  template<
    typename T,
    typename NonConstT=typename std::remove_const<T>::type,
    typename NonConstBase=typename std::remove_const<Base>::type,
    typename std::enable_if<
      Traits::template is_implementation<NonConstBase,NonConstT>::value&&
      (!std::is_const<Base>::value||std::is_const<T>::value)
    >::type* =nullptr
  >
  explicit operator 
    typename Traits::template iterator<NonConstBase,T>()const noexcept
  {
    return Traits::template make_iterator<
      typename Traits::template iterator<NonConstBase,T>>({pbb,n});
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
  template<typename,typename>
  friend class stride_hub_iterator;
  friend class boost::iterator_core_access;

  stride_hub_iterator(
    hub_iterator_members m,std::size_t stride,std::ptrdiff_t offset)noexcept:
    pbb{m.pbb},n{m.n},stride_{stride},offset_{offset}{}

#include <boost/poly_collection/detail/begin_no_sanitize.hpp>
  BOOST_POLY_COLLECTION_NO_SANITIZE
  BOOST_FORCEINLINE Base& dereference()const noexcept
  {
    auto p=static_cast<char*>(
      static_cast<const hub_type_erased_block*>(pbb)->data_);
    return *reinterpret_cast<Base*>(
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
