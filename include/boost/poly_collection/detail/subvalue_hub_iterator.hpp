/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_SUBVALUE_HUB_ITERATOR_HPP
#define BOOST_POLY_COLLECTION_DETAIL_SUBVALUE_HUB_ITERATOR_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/poly_collection/detail/base_offset.hpp>
#include <boost/poly_collection/detail/hub_access.hpp>
#include <cstddef>
#include <type_traits>

namespace boost{

namespace poly_collection{

namespace detail{

/* Adaptor of a boost::container::hub iterator over a Final class with
 * a "value" member of type T. Final mustn't be const-qualified, it is T that
 * carries constness.
 */

template<typename Final,typename T>
class subvalue_hub_iterator:
  public boost::iterator_adaptor<
    subvalue_hub_iterator<Final,T>,
    hub_iterator<
      typename std::conditional<
        std::is_const<T>::value,const Final,Final
      >::type*
    >,
    T,
    boost::use_default,
    T&
  >
{
  using base_type=typename subvalue_hub_iterator::base_type;

public:
  using final_type=typename std::conditional<
    std::is_const<T>::value,const Final,Final>::type;

  subvalue_hub_iterator()=default;
  subvalue_hub_iterator(const base_type& it)noexcept:
    subvalue_hub_iterator::iterator_adaptor_{it}{}
  subvalue_hub_iterator(const subvalue_hub_iterator&)=default;
  subvalue_hub_iterator& operator=(
    const subvalue_hub_iterator&)=default;

  template<
    typename NonConstT,
    typename std::enable_if<
      std::is_same<T,const NonConstT>::value>::type* =nullptr
  >
  subvalue_hub_iterator(
    const subvalue_hub_iterator<Final,NonConstT>& x)noexcept:
    subvalue_hub_iterator::iterator_adaptor_{x.base()}{}

  template<
    typename NonConstT,
    typename std::enable_if<
      std::is_same<T,const NonConstT>::value>::type* =nullptr
  >
  subvalue_hub_iterator& operator=(
    const subvalue_hub_iterator<Final,NonConstT>& x)noexcept
  {
    this->base_reference()=x.base();
    return *this;
  }

private:
  friend class boost::iterator_core_access;

  T& dereference()const noexcept{return this->base()->value;}
};

template<typename Final,typename T>
hub_iterator_members get_members(
  const subvalue_hub_iterator<Final,T>& it)noexcept
{
  return get_members(it.base());
}

/* stride_hub_iterator interoperability */

struct subvalue_hub_iterator_traits 
{
  template<typename Iterator>
  static Iterator make_iterator(hub_iterator_members m)noexcept
  {
    return
      {make_hub_iterator<hub_iterator<typename Iterator::final_type*>>(m)};
  }

  template<typename Final,typename T>
  static std::size_t stride_from(const subvalue_hub_iterator<Final,T>&)noexcept
  {
    return sizeof(Final);
  }

  template<typename Base,typename Final,typename T>
  static std::ptrdiff_t offset_from(
    const subvalue_hub_iterator<Final,T>&)noexcept
  {
    return base_offset<Base,Final>();
  }
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
