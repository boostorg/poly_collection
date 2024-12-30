/* Copyright 2024 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#include "test_null_variant_collection.hpp"

#include <boost/core/lightweight_test.hpp>
#include <boost/poly_collection/variant_collection.hpp>
#include <boost/poly_collection/algorithm.hpp>
#include <utility>
#include "test_utilities.hpp"

using namespace test_utilities;

void test_null_variant_collection()
{
  using boost::poly_collection::unregistered_type;
  using collection_type=boost::variant_collection_of<>;
  using value_type=collection_type::value_type;

  collection_type        c,c2{c},c3{std::move(c2)};
  const collection_type& cc=c;

  c2=c;
  c=std::move(c2);

  (void)c.get_allocator();

  (void)c.begin();
  (void)c.end();
  (void)c.cbegin();
  (void)c.cend();
  check_throw<unregistered_type>(
    [&]{(void)c.begin(0);},
    [&]{(void)c.end(0);},
    [&]{(void)c.cbegin(0);},
    [&]{(void)c.cend(0);});

  check_throw<unregistered_type>(
    [&]{c.segment(0);},
    [&]{cc.segment(0);});

  for(auto seg:c.segment_traversal()){(void)seg;}
  for(auto seg:cc.segment_traversal()){(void)seg;}

  BOOST_TEST(cc.empty());
  check_throw<unregistered_type>([&]{(void)cc.empty(0);});

  BOOST_TEST_EQ(cc.size(),0);
  check_throw<unregistered_type>([&]{(void)cc.size(0);});

  check_throw<unregistered_type>([&]{(void)cc.capacity(0);});

  c.shrink_to_fit();
  check_throw<unregistered_type>([&]{(void)c.shrink_to_fit(0);});

  c.clear();
  check_throw<unregistered_type>([&]{(void)c.clear(0);});

  c.swap(c);

  BOOST_TEST(cc==cc);
  BOOST_TEST(!(cc!=cc));

  auto f=[](value_type&){};

  boost::poly_collection::for_each(c.begin(),c.end(),f);
  boost::poly_collection::for_each_n(c.begin(),0,f);
}
