/* Copyright 2016 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#include "test_capacity.hpp"

#include <algorithm>
#include <boost/core/lightweight_test.hpp>
#include <limits>
#include "any_types.hpp"
#include "base_types.hpp"
#include "function_types.hpp"
#include "test_utilities.hpp"

using namespace test_utilities;

template<typename PolyCollection,typename ValueFactory,typename... Types>
void test_capacity()
{
  using size_type=typename PolyCollection::size_type;

  PolyCollection        p;
  const PolyCollection& cp=p;
  ValueFactory          v;

  BOOST_TEST(cp.empty());
  BOOST_TEST(cp.size()==0);
  BOOST_TEST(cp.max_size()==(std::numeric_limits<size_type>::max)());
  BOOST_TEST(cp.capacity()==(std::numeric_limits<size_type>::max)());

  p.template register_types<Types...>();
  BOOST_TEST(cp.empty());
  do_((BOOST_TEST(cp.empty(typeid(Types))),0)...);
  do_((BOOST_TEST(cp.template empty<Types>()),0)...);
  BOOST_TEST(cp.size()==0);
  do_((BOOST_TEST(cp.size(typeid(Types))==0),0)...);
  do_((BOOST_TEST(cp.template size<Types>()==0),0)...);
  BOOST_TEST(cp.max_size()==(std::min)({cp.max_size(typeid(Types))...}));
  BOOST_TEST(cp.max_size()==(std::min)({cp.template max_size<Types>()...}));
  BOOST_TEST(cp.capacity()==(std::min)({cp.capacity(typeid(Types))...}));
  BOOST_TEST(cp.capacity()==(std::min)({cp.template capacity<Types>()...}));

  p.reserve(10);
  do_((BOOST_TEST(cp.capacity(typeid(Types))>=10),0)...);
  do_((BOOST_TEST(
    cp.template capacity<Types>()==cp.capacity(typeid(Types))),0)...);
  BOOST_TEST(cp.capacity()==(std::min)({cp.capacity(typeid(Types))...}));

  do_((p.reserve(typeid(Types),20),0)...);
  do_((BOOST_TEST(cp.capacity(typeid(Types))>=20),0)...);

  do_((p.template reserve<Types>(30),0)...);
  do_((BOOST_TEST(cp.template capacity<Types>()>=30),0)...);

  auto c=cp.capacity();
  BOOST_TEST(c==(std::min)({cp.capacity(typeid(Types))...}));

  fill<constraints<>,Types...>(p,v,30);
  BOOST_TEST(c==cp.capacity());
  BOOST_TEST(cp.size()==30*sizeof...(Types));
  do_((BOOST_TEST(cp.size(typeid(Types))==30),0)...);
  do_((BOOST_TEST(cp.template size<Types>()==cp.size(typeid(Types))),0)...);

  p.reserve(cp.capacity()+1);
  BOOST_TEST(cp.size()==30*sizeof...(Types));
  c=cp.capacity();

  p.shrink_to_fit();
  BOOST_TEST(c>=cp.capacity());
  c=cp.capacity();

  do_((p.erase(cp.template begin<Types>()),0)...);
  BOOST_TEST(c==cp.capacity());

  do_((p.shrink_to_fit(typeid(Types)),0)...);
  BOOST_TEST(c>=cp.capacity());
  c=cp.capacity();

  p.clear();
  do_((p.template shrink_to_fit<Types>(),0)...);
  BOOST_TEST(c>=cp.capacity());
}

void test_capacity()
{
  test_capacity<
    any_types::collection,auto_increment,
    any_types::t1,any_types::t2,any_types::t3,
    any_types::t4,any_types::t5>();
  test_capacity<
    base_types::collection,auto_increment,
    base_types::t1,base_types::t2,base_types::t3,
    base_types::t4,base_types::t5>();
  test_capacity<
    function_types::collection,auto_increment,
    function_types::t1,function_types::t2,function_types::t3,
    function_types::t4,function_types::t5>();
}
