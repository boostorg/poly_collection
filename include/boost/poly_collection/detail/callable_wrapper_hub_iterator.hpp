/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_CALLABLE_WRAPPER_HUB_ITERATOR_HPP
#define BOOST_POLY_COLLECTION_DETAIL_CALLABLE_WRAPPER_HUB_ITERATOR_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/config.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/poly_collection/detail/base_offset.hpp>
#include <boost/poly_collection/detail/callable_wrapper_closure.hpp>
#include <boost/poly_collection/detail/hub_access.hpp>
#include <type_traits>

#if defined(BOOST_MSVC)
#pragma warning(push)
#pragma warning(disable:4714) /* marked as __forceinline not inlined */
#endif

namespace boost{

namespace poly_collection{

namespace detail{

/* Iterator over the sequence of Callable subobjects in a
 * boost::container::hub of callable_wrapper_closure<Callable,Wrapper>s.
 */

template<typename Wrapper,typename Callable>
using callable_wrapper_target_hub_iterator_base_value_type=
  typename std::conditional<
    std::is_const<Callable>::value,
    const callable_wrapper_closure<
      typename std::remove_const<Callable>::type,Wrapper>,
    callable_wrapper_closure<Callable,Wrapper>
  >::type;

template<typename Wrapper,typename Callable>
class callable_wrapper_target_hub_iterator:
  public boost::iterator_adaptor<
    callable_wrapper_target_hub_iterator<Wrapper,Callable>,
    hub_iterator<
      callable_wrapper_target_hub_iterator_base_value_type<Wrapper,Callable>*
    >,
    Callable,
    boost::use_default,
    Callable&
  >
{
  using base_type=typename callable_wrapper_target_hub_iterator::base_type;

public:
  callable_wrapper_target_hub_iterator()=default;
  callable_wrapper_target_hub_iterator(const base_type& it)noexcept:
    callable_wrapper_target_hub_iterator::iterator_adaptor_{it}{}
  callable_wrapper_target_hub_iterator(
    const callable_wrapper_target_hub_iterator&)=default;
  callable_wrapper_target_hub_iterator& operator=(
    const callable_wrapper_target_hub_iterator&)=default;

  template<
    typename NonConstCallable,
    typename std::enable_if<
      std::is_same<Callable,const NonConstCallable>::value>::type* =nullptr
  >
  callable_wrapper_target_hub_iterator(
    const callable_wrapper_target_hub_iterator<Wrapper,NonConstCallable>& x)
    noexcept:
    callable_wrapper_target_hub_iterator::iterator_adaptor_{x.base()}{}

  template<
    typename NonConstCallable,
    typename std::enable_if<
      std::is_same<Callable,const NonConstCallable>::value>::type* =nullptr
  >
  callable_wrapper_target_hub_iterator& operator=(
    const callable_wrapper_target_hub_iterator<Wrapper,NonConstCallable>& x)
    noexcept
  {
    this->base_reference()=x.base();
    return *this;
  }

private:
  friend class boost::iterator_core_access;

  Callable& dereference()const noexcept
  {
    using base_value_type=
      callable_wrapper_target_hub_iterator_base_value_type<Wrapper,Callable>;

    return static_cast<base_value_type&>(*this->base()).value;
  }
};

template<typename Wrapper,typename Callable>
hub_iterator_members get_members(
  const callable_wrapper_target_hub_iterator<Wrapper,Callable>& it)noexcept
{
  return get_members(it.base());
}

/* Iterator over the sequence of callable_wrapper base objects in a
 * boost::container::hub of 
 * callable_wrapper_closure<Callable,callable_wrapper<...>>s.
 * As Callable is not part of the iterator definition, the stride between
 * values and the offset between callable_wrapper and callable_wrapper_closure
 * are run-time values.
 */

template<typename Wrapper>
class callable_wrapper_hub_iterator:
  public boost::iterator_facade<
    callable_wrapper_hub_iterator<Wrapper>,
    Wrapper,
    boost::bidirectional_traversal_tag
  >
{
public:
  callable_wrapper_hub_iterator()=default;
  callable_wrapper_hub_iterator(
    hub_block_base* pbb,int n,
    std::size_t stride,std::ptrdiff_t offset)noexcept:
    pbb{pbb},n{n},stride_{stride},offset_{offset}{}
  callable_wrapper_hub_iterator(const callable_wrapper_hub_iterator&)=default;
  callable_wrapper_hub_iterator& operator=(
    const callable_wrapper_hub_iterator&)=default;

  template<
    typename NonConstWrapper,
    typename std::enable_if<
      std::is_same<Wrapper,const NonConstWrapper>::value>::type* =nullptr
  >
  callable_wrapper_hub_iterator(
    const callable_wrapper_hub_iterator<NonConstWrapper>& x)noexcept:
    pbb{x.pbb},n{x.n},stride_{x.stride_},offset_{x.offset_}{}

  template<
    typename NonConstWrapper,
    typename std::enable_if<
      std::is_same<Wrapper,const NonConstWrapper>::value>::type* =nullptr
  >
  callable_wrapper_hub_iterator& operator=(
    const callable_wrapper_hub_iterator<NonConstWrapper>& x)noexcept
  {
    pbb=x.pbb;n=x.n;stride_=x.stride_;offset_=x.offset_;
    return *this;
  }

  /* Interoperability with
   * callable_wrapper_target_hub_iterator<Wrapper,Callable>.
   */

  template<
    typename Callable,
    typename NonConstCallable=typename std::remove_const<Callable>::type,
    typename NonConstWrapper=typename std::remove_const<Wrapper>::type,
    typename std::enable_if<
      std::is_constructible<NonConstWrapper,NonConstCallable&>::value&&
      (std::is_const<Wrapper>::value||!std::is_const<Callable>::value)
    >::type* =nullptr
  >
  explicit callable_wrapper_hub_iterator(
    const callable_wrapper_target_hub_iterator<NonConstWrapper,Callable>& x)
    noexcept:
    callable_wrapper_hub_iterator{
      get_members(x),
      sizeof(callable_wrapper_target_hub_iterator_base_value_type<
        NonConstWrapper,NonConstCallable>),
      base_offset<
        NonConstWrapper,
        callable_wrapper_target_hub_iterator_base_value_type<
          NonConstWrapper,NonConstCallable>>()}
    {}

  template<
    typename Callable,
    typename NonConstCallable=typename std::remove_const<Callable>::type,
    typename NonConstWrapper=typename std::remove_const<Wrapper>::type,
    typename std::enable_if<
      std::is_constructible<NonConstWrapper,NonConstCallable&>::value&&
      (!std::is_const<Wrapper>::value||std::is_const<Callable>::value)
    >::type* =nullptr
  >
  explicit operator
    callable_wrapper_target_hub_iterator<NonConstWrapper,Callable>()
    const noexcept
  {
    using base_value_type=
      callable_wrapper_target_hub_iterator_base_value_type<
        NonConstWrapper,Callable>;
    return make_hub_iterator<hub_iterator<base_value_type*>>({pbb,n});
  }

  /* nullification used by poly_collection global iterators */

  callable_wrapper_hub_iterator& operator=(std::nullptr_t)
  {
    return *this=callable_wrapper_hub_iterator{};
  }

  hub_block_base* block()const noexcept{return pbb;}
  int             slot()const noexcept{return n;}
  std::size_t     stride()const noexcept{return stride_;}
  std::ptrdiff_t  offset()const noexcept{return offset_;}

private:
  template<typename>
  friend class callable_wrapper_hub_iterator;
  friend class boost::iterator_core_access;

  callable_wrapper_hub_iterator(
    hub_iterator_members m,std::size_t stride,std::ptrdiff_t offset)noexcept:
    pbb{m.pbb},n{m.n},stride_{stride},offset_{offset}{}

#include <boost/poly_collection/detail/begin_no_sanitize.hpp>

  BOOST_POLY_COLLECTION_NO_SANITIZE
  BOOST_FORCEINLINE Wrapper& dereference()const noexcept
  {
    auto p=static_cast<char*>(
      static_cast<const hub_type_erased_block*>(pbb)->data_);
    return *reinterpret_cast<Wrapper*>(
      p+static_cast<std::size_t>(n)*stride_+offset_);
  }

#include <boost/poly_collection/detail/end_no_sanitize.hpp>

  bool equal(const callable_wrapper_hub_iterator& x)const noexcept
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
