/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_BASE_UNORDERED_COLLECTION_HPP
#define BOOST_POLY_COLLECTION_BASE_UNORDERED_COLLECTION_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/poly_collection/base_unordered_collection_fwd.hpp>
#include <boost/poly_collection/detail/base_unordered_model.hpp>
#include <boost/poly_collection/detail/poly_collection.hpp>

namespace boost{

namespace poly_collection{

template<typename Base,typename Allocator>
class base_unordered_collection:
  public common_impl::poly_collection<
    detail::base_unordered_model<Base,Allocator>>
{
  using base_type=common_impl::poly_collection<
    detail::base_unordered_model<Base,Allocator>>;

  base_type&       base()noexcept{return *this;}
  const base_type& base()const noexcept{return *this;}

public:
  using base_type::base_type;

  base_unordered_collection()=default;
  base_unordered_collection(const base_unordered_collection& x)=default;
  base_unordered_collection(base_unordered_collection&& x)=default;
  base_unordered_collection& operator=(
    const base_unordered_collection& x)=default;
  base_unordered_collection& operator=(base_unordered_collection&& x)=default;
};

template<typename Base,typename Allocator>
void swap(
  base_unordered_collection<Base,Allocator>& x,
  base_unordered_collection<Base,Allocator>& y)
{
  x.swap(y);
}

} /* namespace poly_collection */

using poly_collection::base_unordered_collection;

} /* namespace boost */

#endif
