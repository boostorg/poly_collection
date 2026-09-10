/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#include "test_algorithm8.hpp"

#include "variant_types.hpp"
#include "test_algorithm_impl.hpp"
#include "test_total_restitution_algorithm_impl.hpp"

void test_algorithm8()
{
  test_algorithm<
    variant_types::unordered_collection,jammed_auto_increment,
    variant_types::to_int,
    variant_types::t1,variant_types::t2,variant_types::t3,
    variant_types::t4,variant_types::t5>();
  test_total_restitution_algorithm<
    variant_types::unordered_collection,variant_types::t1>();
}
