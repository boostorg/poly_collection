/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_UNORDERED_SEGMENT_BACKEND_HPP
#define BOOST_POLY_COLLECTION_DETAIL_UNORDERED_SEGMENT_BACKEND_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <cstddef>
#include <memory>

namespace boost{

namespace poly_collection{

namespace detail{

/* Internal *virtual* interface of unordered_segment<StorageModel>.
 * Modeled after (ordered) segment_backend<StorageModel>, with some 
 * observations:
 *  - Positional insertion is not provided. 
 *  - Although insertion is not guaranteed to happen at the end, *_back
 *    naming has been retained so that poly_collection can handle both ordered
 *    and unordered variants generically.
 *  - Unlike segment_backend, here we don't piggyback sentinels as part of the
 *    return value of modifier functions because boost::container::hub::end()
 *    is always stable.
 */

template<typename StorageModel>
struct unordered_segment_backend
{
  using allocator_type=typename StorageModel::segment_allocator_type;
  using segment_backend_unique_ptr=
    std::unique_ptr<
      unordered_segment_backend,void(*)(unordered_segment_backend*)>;
  using value_pointer=void*;
  using const_value_pointer=const void*;
  using base_iterator=typename StorageModel::base_iterator;
  using const_base_iterator=typename StorageModel::const_base_iterator;
  template<typename T>
  using const_iterator=typename StorageModel::template const_iterator<T>;
  using base_sentinel=typename StorageModel::base_sentinel;

  unordered_segment_backend()=default;
  unordered_segment_backend(const unordered_segment_backend&)=delete;
  unordered_segment_backend& operator=(
    const unordered_segment_backend&)=delete;

  virtual                            ~unordered_segment_backend()=default;
  virtual segment_backend_unique_ptr copy()const=0;
  virtual segment_backend_unique_ptr copy(const allocator_type&)const=0;
  virtual segment_backend_unique_ptr empty_copy(const allocator_type&)const=0;
  virtual segment_backend_unique_ptr move(const allocator_type&)=0;

  virtual allocator_type get_allocator()const noexcept=0;
  virtual base_iterator  begin()const noexcept=0;
  virtual base_iterator  end()const noexcept=0;
  virtual bool           empty()const noexcept=0;
  virtual std::size_t    size()const noexcept=0;
  virtual std::size_t    max_size()const noexcept=0;
  virtual std::size_t    capacity()const noexcept=0;
  virtual void           reserve(std::size_t)=0;
  virtual void           shrink_to_fit()=0;
  virtual base_iterator  push_back(const_value_pointer)=0;
  virtual base_iterator  push_back_move(value_pointer)=0;
  virtual base_iterator  erase(const_base_iterator)=0;
  virtual base_iterator  erase(const_base_iterator,const_base_iterator)=0;
  virtual base_iterator  erase_till_end(const_base_iterator)=0;
  virtual base_iterator  erase_from_begin(const_base_iterator)=0;
  virtual void           clear()noexcept=0;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
