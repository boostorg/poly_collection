/* Copyright 2024 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_REFERENCE_VARIANT_ITERATOR_HPP
#define BOOST_POLY_COLLECTION_DETAIL_REFERENCE_VARIANT_ITERATOR_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/poly_collection/detail/reference_variant.hpp>
#include <memory>
#include <type_traits>

namespace boost{

namespace poly_collection{

namespace detail{

/* reference_variant<Ts...>* adaptor convertible to pointer to wrapped
 * entity.
 */

template<typename RefVariant>
class reference_variant_iterator:
  public boost::iterator_adaptor<
    reference_variant_iterator<RefVariant>,RefVariant*>
{
public:
  reference_variant_iterator()=default;
  explicit reference_variant_iterator(RefVariant* p)noexcept:
    reference_variant_iterator::iterator_adaptor_{p}{}
  reference_variant_iterator(const reference_variant_iterator&)=
    default;
  reference_variant_iterator& operator=(const reference_variant_iterator&)=
    default;

  template<
    typename NonConstRefVariant,
    typename std::enable_if<
      std::is_same<RefVariant,const NonConstRefVariant>::value>::type* =nullptr
  >
  reference_variant_iterator(
    const reference_variant_iterator<NonConstRefVariant>& x)noexcept:
    reference_variant_iterator::iterator_adaptor_{x.base()}{}

  template<
    typename NonConstRefVariant,
    typename std::enable_if<
      std::is_same<RefVariant,const NonConstRefVariant>::value>::type* =nullptr
  >
  reference_variant_iterator& operator=(
    const reference_variant_iterator<NonConstRefVariant>& x)noexcept
  {
    this->base_reference()=x.base();
    return *this;
  }

  /* interoperability with RefVariant* */

  reference_variant_iterator& operator=(RefVariant* p)noexcept
    {this->base_reference()=p;return *this;}
  operator RefVariant*()const noexcept{return this->base();}

  /* interoperability with Ts*... */

  template<
    typename T,
    typename NonConstT=typename std::remove_const<T>::type,
    typename RawRefVariant=typename std::decay<RefVariant>::type,
    std::size_t Idx=mp11::mp_find<RawRefVariant,NonConstT>::value,
    typename std::enable_if<
      (Idx<mp11::mp_size<RawRefVariant>::value)>::type* =nullptr
  >
  explicit operator T*()const noexcept
  {
    return const_cast<T*>(std::addressof(unsafe_get<Idx>(*this->base())));
  }

private:
  template<typename>
  friend class reference_variant_iterator;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
