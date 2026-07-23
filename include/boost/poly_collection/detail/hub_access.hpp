/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_HUB_ACCESS_HPP
#define BOOST_POLY_COLLECTION_DETAIL_HUB_ACCESS_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/container/hub.hpp>
#include <cstring>
#include <type_traits>

namespace boost{

namespace poly_collection{

namespace detail{

/* Private access to boost::container::hub internals.
 * To be eventually replaced with legit mechanism provided in Boost.Container.
 */

template<typename ValuePointer>
using hub_iterator=boost::container::hub_detail::iterator<ValuePointer>;

using hub_block_base=boost::container::hub_detail::block_base<void*>;

struct hub_type_erased_block:hub_block_base{void* data_;};

static_assert(
  sizeof(hub_type_erased_block)==
  sizeof(boost::container::hub_detail::block<char*>),
  "hub block layout assumption violated");

/* Structural copying of boost::container::hub_detail::iterator, to be replaced
 * eventually by legal access to private members.
 */

struct hub_iterator_members
{
  hub_block_base* pbb;
  int             n;
};

template<typename ValuePointer>
hub_iterator_members get_members(const hub_iterator<ValuePointer>& it)noexcept
{
  static_assert(
    sizeof(hub_iterator<ValuePointer>)==sizeof(hub_iterator_members)&&
    std::is_standard_layout<hub_iterator<ValuePointer>>::value,
    "hub iterator layout assumption violated");
  hub_iterator_members m;
  std::memcpy(&m,static_cast<const void*>(&it),sizeof(m));
  return m;
}

template<typename HubIterator>
HubIterator make_hub_iterator(hub_iterator_members m)noexcept
{
  static_assert(
    sizeof(HubIterator)==sizeof(hub_iterator_members)&&
    std::is_standard_layout<HubIterator>::value,
    "hub iterator layout assumption violated");
  HubIterator it;
  std::memcpy(static_cast<void*>(&it),&m,sizeof(m));
  return it;
}

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
