/* Copyright 2016-2017 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_SEGMENT_BACKEND_HPP
#define BOOST_POLY_COLLECTION_DETAIL_SEGMENT_BACKEND_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <cstddef>
#include <memory>
#include <utility>

namespace boost{

namespace poly_collection{

namespace detail{

/* Internal virtual interface of segment<Model,Allocator>. Member functions
 * have been defined to minimize virtual function calls according to usage
 * patterns by poly_collection. For instance, ranges are returned rather than
 * iterators to allow for caching of and end sentinel at a higher level.
 * Information associated to concrete types handled by the segment, such as
 * inserted elements and local_iterator's, are type-erased. Similarly, emplace
 * is implemented through type-independent callback functions with signature
 *
 *   void emplace(void* p,void* arg)
 *
 *  where p is the address of the object to be constructed and arg is a
 *  type-erased pointer to the constructor arguments.
 */

template<typename Model>
struct segment_backend
{
  using segment_backend_unique_ptr=
    std::unique_ptr<segment_backend,void(*)(segment_backend*)>;
  using value_pointer=void*;
  using const_value_pointer=const void*;
  using base_iterator=typename Model::base_iterator;
  using const_base_iterator=typename Model::const_base_iterator;
  using base_sentinel=typename Model::base_sentinel;
  using const_base_sentinel=typename Model::const_base_sentinel;
  using position_pointer=const void*;
  using range=std::pair<base_iterator,base_sentinel>;

  segment_backend()=default;
  segment_backend(const segment_backend&)=delete;
  segment_backend& operator=(const segment_backend&)=delete;

  virtual                            ~segment_backend()=default;
  virtual segment_backend_unique_ptr copy()const=0;
  virtual segment_backend_unique_ptr empty_copy()const=0;
  virtual bool                       equal(const segment_backend*)const=0;

  virtual base_iterator begin()const noexcept=0;
  virtual base_iterator end()const noexcept=0;
  virtual bool          empty()const noexcept=0;
  virtual std::size_t   size()const noexcept=0;
  virtual std::size_t   max_size()const noexcept=0;
  virtual std::size_t   capacity()const noexcept=0;
  virtual base_sentinel reserve(std::size_t)=0;
  virtual base_sentinel shrink_to_fit()=0;
  virtual range         emplace(
                          const_base_iterator,void (*)(void*,void*),void*)=0;
  virtual range         emplace(
                          position_pointer,void (*)(void*,void*),void*)=0;
  virtual range         emplace_back(void (*)(void*,void*),void*)=0;
  virtual range         push_back(const_value_pointer)=0;
  virtual range         push_back_move(value_pointer)=0;
  virtual range         insert(const_base_iterator,const_value_pointer)=0;
  virtual range         insert(position_pointer,const_value_pointer)=0;
  virtual range         insert_move(const_base_iterator,value_pointer)=0;
  virtual range         insert_move(position_pointer,value_pointer)=0;
  virtual range         erase(const_base_iterator)=0;
  virtual range         erase(position_pointer)=0;
  virtual range         erase(const_base_iterator,const_base_iterator)=0;
  virtual range         erase(position_pointer,position_pointer)=0;
  virtual range         erase_till_end(const_base_iterator)=0;
  virtual range         erase_till_end(position_pointer)=0;
  virtual range         erase_from_begin(const_base_iterator)=0;
  virtual range         erase_from_begin(position_pointer)=0;
  virtual base_sentinel clear()noexcept=0;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
