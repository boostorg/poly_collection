/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_FIXED_VARIANT_HUB_ITERATOR_HPP
#define BOOST_POLY_COLLECTION_DETAIL_FIXED_VARIANT_HUB_ITERATOR_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/config.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/poly_collection/detail/base_offset.hpp>
#include <boost/poly_collection/detail/fixed_variant.hpp>
#include <boost/poly_collection/detail/hub_access.hpp>
#include <type_traits>

#if defined(BOOST_MSVC)
#pragma warning(push)
#pragma warning(disable:4714) /* marked as __forceinline not inlined */
#endif

namespace boost{

namespace poly_collection{

namespace detail{

/* Iterator over the sequence of T subojects in a boost::container::hub of
 * fixed_variant_closure<T,fixed_variant<...>>s.
 */

template<typename Variant,typename T>
using fixed_variant_alternative_hub_iterator_base_value_type=
  typename std::conditional<
    std::is_const<T>::value,
    const fixed_variant_impl::fixed_variant_closure<
      typename std::remove_const<T>::type,Variant>,
    fixed_variant_impl::fixed_variant_closure<T,Variant>
  >::type;

template<typename Variant,typename T>
class fixed_variant_alternative_hub_iterator:
  public boost::iterator_adaptor<
    fixed_variant_alternative_hub_iterator<Variant,T>,
    hub_iterator<
      fixed_variant_alternative_hub_iterator_base_value_type<Variant,T>*
    >,
    T,
    boost::use_default,
    T&
  >
{
  using base_type=typename fixed_variant_alternative_hub_iterator::base_type;

public:
  fixed_variant_alternative_hub_iterator()=default;
  fixed_variant_alternative_hub_iterator(const base_type& it)noexcept:
    fixed_variant_alternative_hub_iterator::iterator_adaptor_{it}{}
  fixed_variant_alternative_hub_iterator(
    const fixed_variant_alternative_hub_iterator&)=default;
  fixed_variant_alternative_hub_iterator& operator=(
    const fixed_variant_alternative_hub_iterator&)=default;

  template<
    typename NonConstT,
    typename std::enable_if<
      std::is_same<T,const NonConstT>::value>::type* =nullptr
  >
  fixed_variant_alternative_hub_iterator(
    const fixed_variant_alternative_hub_iterator<Variant,NonConstT>& x)
    noexcept:
    fixed_variant_alternative_hub_iterator::iterator_adaptor_{x.base()}{}

  template<
    typename NonConstT,
    typename std::enable_if<
      std::is_same<T,const NonConstT>::value>::type* =nullptr
  >
  fixed_variant_alternative_hub_iterator& operator=(
    const fixed_variant_alternative_hub_iterator<Variant,NonConstT>& x)noexcept
  {
    this->base_reference()=x.base();
    return *this;
  }

private:
  friend class boost::iterator_core_access;

  T& dereference()const noexcept
  {
    using base_value_type=
      fixed_variant_alternative_hub_iterator_base_value_type<Variant,T>;

    return static_cast<base_value_type&>(*this->base()).value;
  }
};

template<typename Variant,typename T>
hub_iterator_members get_members(
  const fixed_variant_alternative_hub_iterator<Variant,T>& it)noexcept
{
  return get_members(it.base());
}

/* Iterator over the sequence of fixed_variant base objects in a
 * boost::container::hub of * fixed_variant_closure<T,fixed_variant<...>>s. 
 * As T is not part of the iterator definition, the stride between values and
 * the offset between fixed_variant and fixed_variant_closure are run-time 
 * values.
 */

template<typename Variant>
class fixed_variant_hub_iterator:
  public boost::iterator_facade<
    fixed_variant_hub_iterator<Variant>,
    Variant,
    boost::bidirectional_traversal_tag
  >
{
public:
  fixed_variant_hub_iterator()=default;
  fixed_variant_hub_iterator(
    hub_block_base* pbb,int n,
    std::size_t stride,std::ptrdiff_t offset)noexcept:
    pbb{pbb},n{n},stride_{stride},offset_{offset}{}
  fixed_variant_hub_iterator(const fixed_variant_hub_iterator&)=default;
  fixed_variant_hub_iterator& operator=(
    const fixed_variant_hub_iterator&)=default;

  template<
    typename NonConstVariant,
    typename std::enable_if<
      std::is_same<Variant,const NonConstVariant>::value>::type* =nullptr
  >
  fixed_variant_hub_iterator(
    const fixed_variant_hub_iterator<NonConstVariant>& x)noexcept:
    pbb{x.pbb},n{x.n},stride_{x.stride_},offset_{x.offset_}{}

  template<
    typename NonConstVariant,
    typename std::enable_if<
      std::is_same<Variant,const NonConstVariant>::value>::type* =nullptr
  >
  fixed_variant_hub_iterator& operator=(
    const fixed_variant_hub_iterator<NonConstVariant>& x)noexcept
  {
    pbb=x.pbb;n=x.n;stride_=x.stride_;offset_=x.offset_;
    return *this;
  }

  /* interoperability with fixed_variant_alternative_hub_iterator<Variant,T> */

  template<
    typename T,
    typename NonConstT=typename std::remove_const<T>::type,
    typename NonConstVariant=typename std::remove_const<Variant>::type,
    typename std::enable_if<
      mp11::mp_contains<NonConstVariant,NonConstT>::value&&
      (std::is_const<Variant>::value||!std::is_const<T>::value)
    >::type* =nullptr
  >
  explicit fixed_variant_hub_iterator(
    const fixed_variant_alternative_hub_iterator<NonConstVariant,T>& x)
    noexcept:
    fixed_variant_hub_iterator{
      get_members(x),
      sizeof(fixed_variant_alternative_hub_iterator_base_value_type<
        NonConstVariant,NonConstT>),
      base_offset<
        NonConstVariant,
        fixed_variant_alternative_hub_iterator_base_value_type<
          NonConstVariant,NonConstT>>()}
    {}

  template<
    typename T,
    typename NonConstT=typename std::remove_const<T>::type,
    typename NonConstVariant=typename std::remove_const<Variant>::type,
    typename std::enable_if<
      mp11::mp_contains<NonConstVariant,NonConstT>::value&&
      (!std::is_const<Variant>::value||std::is_const<T>::value)
    >::type* =nullptr
  >
  explicit operator
    fixed_variant_alternative_hub_iterator<NonConstVariant,T>()const noexcept
  {
    using base_value_type=
      fixed_variant_alternative_hub_iterator_base_value_type<
        NonConstVariant,T>;
    return make_hub_iterator<hub_iterator<base_value_type*>>({pbb,n});
  }

  /* nullification used by poly_collection global iterators */

  fixed_variant_hub_iterator& operator=(std::nullptr_t)
  {
    return *this=fixed_variant_hub_iterator{};
  }

  hub_block_base* block()const noexcept{return pbb;}
  int             slot()const noexcept{return n;}
  std::size_t     stride()const noexcept{return stride_;}
  std::ptrdiff_t  offset()const noexcept{return offset_;}

private:
  template<typename>
  friend class fixed_variant_hub_iterator;
  friend class boost::iterator_core_access;

  fixed_variant_hub_iterator(
    hub_iterator_members m,std::size_t stride,std::ptrdiff_t offset)noexcept:
    pbb{m.pbb},n{m.n},stride_{stride},offset_{offset}{}

#include <boost/poly_collection/detail/begin_no_sanitize.hpp>

  BOOST_POLY_COLLECTION_NO_SANITIZE
  BOOST_FORCEINLINE Variant& dereference()const noexcept
  {
    auto p=static_cast<char*>(
      static_cast<const hub_type_erased_block*>(pbb)->data_);
    return *reinterpret_cast<Variant*>(
      p+static_cast<std::size_t>(n)*stride_+offset_);
  }

#include <boost/poly_collection/detail/end_no_sanitize.hpp>

  bool equal(const fixed_variant_hub_iterator& x)const noexcept
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
