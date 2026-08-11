/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_VARIANT_UNORDERED_COLLECTION_HPP
#define BOOST_POLY_COLLECTION_VARIANT_UNORDERED_COLLECTION_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/mp11/list.hpp>
#include <boost/poly_collection/detail/poly_collection.hpp>
#include <boost/poly_collection/detail/variant_unordered_model.hpp>
#include <boost/poly_collection/variant_unordered_collection_fwd.hpp>

namespace boost{

namespace poly_collection{

template<typename TypeList,typename Allocator>
class variant_unordered_collection:
  public common_impl::poly_collection<
    mp11::mp_rename<
      mp11::mp_push_front<TypeList,Allocator>,detail::variant_unordered_model>>
{
  using base_type=common_impl::poly_collection<
    mp11::mp_rename<
      mp11::mp_push_front<TypeList,Allocator>,
      detail::variant_unordered_model>>;

  base_type&       base()noexcept{return *this;}
  const base_type& base()const noexcept{return *this;}

public:
  using base_type::base_type;

  variant_unordered_collection()=default;
  variant_unordered_collection(const variant_unordered_collection& x)=default;
  variant_unordered_collection(variant_unordered_collection&& x)=default;
  variant_unordered_collection& operator=(
    const variant_unordered_collection& x)=default;
  variant_unordered_collection& operator=(
    variant_unordered_collection&& x)=default;
};

template<typename TypeList,typename Allocator>
void swap(
  variant_unordered_collection<TypeList,Allocator>& x,
  variant_unordered_collection<TypeList,Allocator>& y)
{
  x.swap(y);
}

} /* namespace poly_collection */

using poly_collection::variant_unordered_collection;
using poly_collection::variant_unordered_collection_of;

} /* namespace boost */

#endif
