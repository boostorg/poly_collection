/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_FUNCTION_UNORDERED_COLLECTION_HPP
#define BOOST_POLY_COLLECTION_FUNCTION_UNORDERED_COLLECTION_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/poly_collection/detail/function_unordered_model.hpp>
#include <boost/poly_collection/detail/poly_collection.hpp>
#include <boost/poly_collection/function_unordered_collection_fwd.hpp>

namespace boost{

namespace poly_collection{

template<typename Signature,typename Allocator>
class function_unordered_collection:
 public common_impl::poly_collection<
   detail::function_unordered_model<Signature,Allocator>>
{
  using base_type=common_impl::poly_collection<
    detail::function_unordered_model<Signature,Allocator>>;

public:
  using base_type::base_type;

  function_unordered_collection()=default;
  function_unordered_collection(
    const function_unordered_collection& x)=default;
  function_unordered_collection(function_unordered_collection&& x)=default;
  function_unordered_collection& operator=(
    const function_unordered_collection& x)=default;
  function_unordered_collection& operator=(
    function_unordered_collection&& x)=default;
};

template<typename Signature,typename Allocator>
void swap(
  function_unordered_collection<Signature,Allocator>& x,
  function_unordered_collection<Signature,Allocator>& y)
{
  x.swap(y);
}

} /* namespace poly_collection */

using poly_collection::function_unordered_collection;

} /* namespace boost */

#endif
