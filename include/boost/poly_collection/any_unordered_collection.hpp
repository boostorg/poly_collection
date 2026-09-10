/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_ANY_UNORDERED_COLLECTION_HPP
#define BOOST_POLY_COLLECTION_ANY_UNORDERED_COLLECTION_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/poly_collection/any_unordered_collection_fwd.hpp>
#include <boost/poly_collection/detail/any_unordered_model.hpp>
#include <boost/poly_collection/detail/poly_collection.hpp>

namespace boost{

namespace poly_collection{

template<typename Concept,typename Allocator>
class any_unordered_collection:
 public common_impl::poly_collection<
   detail::any_unordered_model<Concept,Allocator>>
{
  using base_type=common_impl::poly_collection<
    detail::any_unordered_model<Concept,Allocator>>;

public:
  using base_type::base_type;

  any_unordered_collection()=default;
  any_unordered_collection(const any_unordered_collection& x)=default;
  any_unordered_collection(any_unordered_collection&& x)=default;
  any_unordered_collection& operator=(
    const any_unordered_collection& x)=default;
  any_unordered_collection& operator=(any_unordered_collection&& x)=default;
};

template<typename Concept,typename Allocator>
void swap(
  any_unordered_collection<Concept,Allocator>& x,
  any_unordered_collection<Concept,Allocator>& y)
{
  x.swap(y);
}

} /* namespace poly_collection */

using poly_collection::any_unordered_collection;

} /* namespace boost */

#endif
