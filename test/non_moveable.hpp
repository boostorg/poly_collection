/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_TEST_NON_MOVEABLE_HPP
#define BOOST_POLY_COLLECTION_TEST_NON_MOVEABLE_HPP

#if defined(_MSC_VER)
#pragma once
#endif

template<typename Class>
struct non_moveable:Class
{
  using Class::Class;
  non_moveable(const non_moveable&)=delete;
  non_moveable(non_moveable&&)=delete;
  non_moveable& operator=(const non_moveable&)=delete;
  non_moveable& operator=(non_moveable&&)=delete;
};

#endif
