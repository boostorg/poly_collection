/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#include "test_algorithm7.hpp"

#include "function_types.hpp"
#include "test_algorithm_impl.hpp"

void test_algorithm7()
{
  test_algorithm<
    function_types::unordered_collection,jammed_auto_increment,
    function_types::to_int,
    function_types::t1,function_types::t2,function_types::t3,
    function_types::t4,function_types::t5>();
}
