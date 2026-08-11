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

#include <boost/poly_collection/detail/allocator_adaptor.hpp>
#include <boost/poly_collection/detail/base_offset.hpp>
#include <boost/poly_collection/detail/base_polymorphism.hpp>
#include <boost/poly_collection/detail/packed_hub_segment.hpp>
#include <boost/poly_collection/detail/stride_hub_iterator.hpp>
#include <boost/poly_collection/detail/unordered_segment.hpp>
#include <boost/poly_collection/detail/unordered_segment_backend.hpp>
#include <cstddef>
#include <type_traits>

namespace boost{

namespace poly_collection{

namespace detail{

/* model for base_unordered_collection */

struct base_unordered_storage_iterator_traits /* for stride_hub_iterator */
{
  template<typename Base,typename Derived>
  using is_implementation=
    typename base_polymorphism<Base>::template is_implementation<Derived>;
  template<typename Base,typename Derived>
  using iterator=hub_iterator<Derived*>;

  template<typename Iterator>
  static Iterator make_iterator(hub_iterator_members m)noexcept
  {
    return make_hub_iterator<Iterator>(m);
  }

  template<typename Derived>
  static std::size_t stride_from(const hub_iterator<Derived*>&)noexcept
  {
    return sizeof(Derived);
  }

  template<typename Base,typename Derived>
  static ptrdiff_t offset_from(const hub_iterator<Derived*>&)noexcept
  {
    return base_offset<Base,Derived>();
  }
};

template<typename Base,typename Allocator>
struct base_unordered_storage
{
private:
  template<typename Derived>
  using hub_iterator=detail::hub_iterator<Derived*>;

public:
  using value_type=Base;
  using allocator_type=Allocator;
  using segment_allocator_type=allocator_adaptor<Allocator>;
  using base_iterator=
    stride_hub_iterator<Base,base_unordered_storage_iterator_traits>;
  using const_base_iterator=stride_hub_iterator<
    const Base,base_unordered_storage_iterator_traits>;
  using base_sentinel=base_iterator;
  using const_base_sentinel=const_base_iterator;
  template<typename Derived>
  using iterator=typename base_unordered_storage_iterator_traits::
    template iterator<value_type,Derived>;
  template<typename Derived>
  using const_iterator=typename base_unordered_storage_iterator_traits::
    template iterator<value_type,const Derived>;
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
