/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_BASE_UNORDERED_MODEL_HPP
#define BOOST_POLY_COLLECTION_DETAIL_BASE_UNORDERED_MODEL_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/container/hub.hpp>
#include <boost/core/pointer_traits.hpp>
#include <boost/poly_collection/detail/allocator_adaptor.hpp>
#include <boost/poly_collection/detail/base_polymorphism.hpp>
#include <boost/poly_collection/detail/hub_stride_iterator.hpp>
#include <boost/poly_collection/detail/packed_hub_segment.hpp>
#include <boost/poly_collection/detail/unordered_segment.hpp>
#include <boost/poly_collection/detail/unordered_segment_backend.hpp>
#include <memory>

namespace boost{

namespace poly_collection{

namespace detail{

/* model for base_unordered_collection */

template<typename Base,typename Allocator>
struct base_unordered_storage
{
private:
  template<typename Derived>
  using hub_iterator=detail::hub_iterator<
    typename pointer_traits<
      typename std::allocator_traits<Allocator>::pointer
    >::template rebind<Derived>>;

public:
  using value_type=Base;
  using allocator_type=Allocator;
  using segment_allocator_type=allocator_adaptor<Allocator>;
  using base_iterator=hub_stride_iterator<Base>;
  using const_base_iterator=hub_stride_iterator<const Base>;
  using base_sentinel=base_iterator;
  using const_base_sentinel=const_base_iterator;
  template<typename Derived>
  using iterator=hub_iterator<Derived>;
  template<typename Derived>
  using const_iterator=hub_iterator<const Derived>;
  using segment_backend=
    detail::unordered_segment_backend<base_unordered_storage>;
  template<typename Derived>
  using segment_backend_implementation=
    packed_hub_segment<base_unordered_storage,Derived>;

  static base_iterator nonconst_iterator(const_base_iterator it)
  {
    return {it.block(),it.slot(),it.stride(),it.offset()};
  }

  template<typename T>
  static iterator<T> nonconst_iterator(const_iterator<T> it)
  {
    return make_hub_iterator<iterator<T>>(get_members(it));
  }

private:
  template<typename,typename>
  friend class packed_hub_segment;

  template<typename Derived>
  static std::ptrdiff_t value_offset()noexcept
  {
    return base_offset<Base,Derived>();
  }
};

template<typename Base,typename Allocator>
struct base_unordered_model:
  base_polymorphism<Base>,base_unordered_storage<Base,Allocator>
{
  /* disambiguate multiply-inherited, identical typedef */
  using value_type=typename base_polymorphism<Base>::value_type;

  using segment=detail::unordered_segment<base_unordered_model>;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
