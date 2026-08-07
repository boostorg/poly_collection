/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_ANY_HUB_ITERATOR_HPP
#define BOOST_POLY_COLLECTION_DETAIL_ANY_HUB_ITERATOR_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/config.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/poly_collection/detail/any_closure.hpp>
#include <boost/poly_collection/detail/base_offset.hpp>
#include <boost/poly_collection/detail/hub_access.hpp>
#include <type_traits>

#if defined(BOOST_MSVC)
#pragma warning(push)
#pragma warning(disable:4714) /* marked as __forceinline not inlined */
#endif

namespace boost{

namespace poly_collection{

namespace detail{

/* Iterator over the sequence of Concrete subobjects in a
 * boost::container::hub of any_closure<Concrete,type_erasure::any<...>>s.
 */

template<typename Any,typename Concrete>
using any_concrete_hub_iterator_base_value_type=
  typename std::conditional<
    std::is_const<Concrete>::value,
    const any_closure<
      typename std::remove_const<Concrete>::type,Any>,
    any_closure<Concrete,Any>
  >::type;

template<typename Any,typename Concrete>
class any_concrete_hub_iterator:
  public boost::iterator_adaptor<
    any_concrete_hub_iterator<Any,Concrete>,
    hub_iterator<
      any_concrete_hub_iterator_base_value_type<Any,Concrete>*
    >,
    Concrete,
    boost::use_default,
    Concrete&
  >
{
  using base_type=typename any_concrete_hub_iterator::base_type;

public:
  any_concrete_hub_iterator()=default;
  any_concrete_hub_iterator(const base_type& it)noexcept:
    any_concrete_hub_iterator::iterator_adaptor_{it}{}
  any_concrete_hub_iterator(const any_concrete_hub_iterator&)=default;
  any_concrete_hub_iterator& operator=(
    const any_concrete_hub_iterator&)=default;

  template<
    typename NonConstConcrete,
    typename std::enable_if<
      std::is_same<Concrete,const NonConstConcrete>::value>::type* =nullptr
  >
  any_concrete_hub_iterator(
    const any_concrete_hub_iterator<Any,NonConstConcrete>& x)noexcept:
    any_concrete_hub_iterator::iterator_adaptor_{x.base()}{}

  template<
    typename NonConstConcrete,
    typename std::enable_if<
      std::is_same<Concrete,const NonConstConcrete>::value>::type* =nullptr
  >
  any_concrete_hub_iterator& operator=(
    const any_concrete_hub_iterator<Any,NonConstConcrete>& x)noexcept
  {
    this->base_reference()=x.base();
    return *this;
  }

private:
  friend class boost::iterator_core_access;

  Concrete& dereference()const noexcept
  {
    using base_value_type=
      any_concrete_hub_iterator_base_value_type<Any,Concrete>;

    return static_cast<base_value_type&>(*this->base()).value;
  }
};

template<typename Any,typename Concrete>
hub_iterator_members get_members(
  const any_concrete_hub_iterator<Any,Concrete>& it)noexcept
{
  return get_members(it.base());
}

/* Iterator over the sequence of type_erasure::any base objects in a
 * boost::container::hub of any_closure<Concrete,type_erasure::any<...>>s.
 * As Concrete is not part of the iterator definition, the stride between
 * values and the offset between the any and any_closure are run-time values.
 */

template<typename Any>
class any_hub_iterator:
  public boost::iterator_facade<
    any_hub_iterator<Any>,
    Any,
    boost::bidirectional_traversal_tag
  >
{
public:
  any_hub_iterator()=default;
  any_hub_iterator(
    hub_block_base* pbb,int n,
    std::size_t stride,std::ptrdiff_t offset)noexcept:
    pbb{pbb},n{n},stride_{stride},offset_{offset}{}
  any_hub_iterator(const any_hub_iterator&)=default;
  any_hub_iterator& operator=(const any_hub_iterator&)=default;

  template<
    typename NonConstAny,
    typename std::enable_if<
      std::is_same<Any,const NonConstAny>::value>::type* =nullptr
  >
  any_hub_iterator(const any_hub_iterator<NonConstAny>& x)noexcept:
    pbb{x.pbb},n{x.n},stride_{x.stride_},offset_{x.offset_}{}

  template<
    typename NonConstAny,
    typename std::enable_if<
      std::is_same<Any,const NonConstAny>::value>::type* =nullptr
  >
  any_hub_iterator& operator=(
    const any_hub_iterator<NonConstAny>& x)noexcept
  {
    pbb=x.pbb;n=x.n;stride_=x.stride_;offset_=x.offset_;
    return *this;
  }

  /* Interoperability with any_concrete_hub_iterator<Any,Concrete>. */

  template<
    typename Concrete,
    typename NonConstConcrete=typename std::remove_const<Concrete>::type,
    typename NonConstAny=typename std::remove_const<Any>::type,
    typename std::enable_if<
      /* can't compile-time check concept compliance */
      std::is_const<Any>::value||!std::is_const<Concrete>::value
    >::type* =nullptr
  >
  explicit any_hub_iterator(
    const any_concrete_hub_iterator<NonConstAny,Concrete>& x)noexcept:
    any_hub_iterator{
      get_members(x),
      sizeof(any_concrete_hub_iterator_base_value_type<
        NonConstAny,NonConstConcrete>),
      base_offset<
        NonConstAny,
        any_concrete_hub_iterator_base_value_type<
          NonConstAny,NonConstConcrete>>()}
    {}

  template<
    typename Concrete,
    typename NonConstAny=typename std::remove_const<Any>::type,
    typename std::enable_if<
      /* can't compile-time check concept compliance */
      !std::is_const<Any>::value||std::is_const<Concrete>::value
    >::type* =nullptr
  >
  explicit operator
    any_concrete_hub_iterator<NonConstAny,Concrete>()const noexcept
  {
    using base_value_type=
      any_concrete_hub_iterator_base_value_type<NonConstAny,Concrete>;
    return make_hub_iterator<hub_iterator<base_value_type*>>({pbb,n});
  }

  /* nullification used by poly_collection global iterators */

  any_hub_iterator& operator=(std::nullptr_t)
  {
    return *this=any_hub_iterator{};
  }

  hub_block_base* block()const noexcept{return pbb;}
  int             slot()const noexcept{return n;}
  std::size_t     stride()const noexcept{return stride_;}
  std::ptrdiff_t  offset()const noexcept{return offset_;}

private:
  template<typename>
  friend class any_hub_iterator;
  friend class boost::iterator_core_access;

  any_hub_iterator(
    hub_iterator_members m,std::size_t stride,std::ptrdiff_t offset)noexcept:
    pbb{m.pbb},n{m.n},stride_{stride},offset_{offset}{}

#include <boost/poly_collection/detail/begin_no_sanitize.hpp>

  BOOST_POLY_COLLECTION_NO_SANITIZE
  BOOST_FORCEINLINE Any& dereference()const noexcept
  {
    auto p=static_cast<char*>(
      static_cast<const hub_type_erased_block*>(pbb)->data_);
    return *reinterpret_cast<Any*>(
      p+static_cast<std::size_t>(n)*stride_+offset_);
  }

#include <boost/poly_collection/detail/end_no_sanitize.hpp>

  bool equal(const any_hub_iterator& x)const noexcept
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
