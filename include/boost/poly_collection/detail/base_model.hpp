/* Copyright 2016-2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_BASE_MODEL_HPP
#define BOOST_POLY_COLLECTION_DETAIL_BASE_MODEL_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/poly_collection/detail/allocator_adaptor.hpp>
#include <boost/poly_collection/detail/base_polymorphism.hpp>
#include <boost/poly_collection/detail/is_final.hpp>
#include <boost/poly_collection/detail/packed_segment.hpp>
#include <boost/poly_collection/detail/segment.hpp>
#include <boost/poly_collection/detail/segment_backend.hpp>
#include <boost/poly_collection/detail/stride_iterator.hpp>

namespace boost{

namespace poly_collection{

namespace detail{

/* model for base_collection */

struct base_storage_iterator_traits /* for stride_iterator */
{
  template<typename Base,typename Derived>
  using is_implementation=
    typename base_polymorphism<Base>::template is_implementation<Derived>;
  template<typename Base,typename Derived>
  using iterator=Derived*;

#include <boost/poly_collection/detail/begin_no_sanitize.hpp>
  template<typename Iterator,typename Base>
  static BOOST_POLY_COLLECTION_NO_SANITIZE Iterator make_iterator(
    Base* p)noexcept
  {
    return static_cast<Iterator>(p);
  }
#include <boost/poly_collection/detail/end_no_sanitize.hpp>

  template<typename Base,typename Derived>
  static const Base* base_pointer_from(const Derived *p)noexcept{return p;}

  template<typename Derived>
  static std::size_t stride_from(Derived*)noexcept{return sizeof(Derived);}
};

template<typename Base,typename Allocator>
struct base_storage
{
  using value_type=typename base_polymorphism<Base>::value_type;
  using allocator_type=Allocator;
  using segment_allocator_type=allocator_adaptor<Allocator>;
  using base_iterator=stride_iterator<Base,base_storage_iterator_traits>;
  using const_base_iterator=
    stride_iterator<const Base,base_storage_iterator_traits>;
  using base_sentinel=Base*;
  using const_base_sentinel=const Base*;
  template<typename Derived>
  using iterator=typename base_storage_iterator_traits::
    template iterator<value_type,Derived>;
  template<typename Derived>
  using const_iterator=typename base_storage_iterator_traits::
    template iterator<value_type,const Derived>;
  using segment_backend=detail::segment_backend<base_storage>;
  template<typename Derived>
  using segment_backend_implementation=packed_segment<base_storage,Derived>;

  static base_iterator nonconst_iterator(const_base_iterator it)noexcept
  {
    return {
      const_cast<value_type*>(static_cast<const value_type*>(it)),
      it.stride()
    };
  }

  template<typename T>
  static iterator<T> nonconst_iterator(const_iterator<T> it)noexcept
  {
    return const_cast<iterator<T>>(it);
  }

private:
  template<typename,typename>
  friend class packed_segment;

  template<typename Derived>
  static const Base* value_ptr(const Derived* p)noexcept
  {
    return p;
  }
};

template<typename Base,typename Allocator>
struct base_model:base_polymorphism<Base>,base_storage<Base,Allocator>
{
  /* disambiguate multiply-inherited, identical typedef */
  using value_type=typename base_polymorphism<Base>::value_type;

  using segment=detail::segment<base_model>;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
