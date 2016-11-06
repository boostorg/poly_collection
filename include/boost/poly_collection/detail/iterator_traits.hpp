/* Copyright 2016 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_ITERATOR_TRAITS_HPP
#define BOOST_POLY_COLLECTION_DETAIL_ITERATOR_TRAITS_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <iterator>
#include <type_traits>

namespace boost{

namespace poly_collection{

namespace detail{

/* (Internal) bunch of traits-grouped functions for const-preserving
 * interoperatibility between iterators and local iterators of a
 * poly_collection.
 */

template<typename Iterator>
struct poly_collection_of /* to be specialized for iterator impls */
{
 using type=void;
};

template<typename Model,typename Allocator>
class poly_collection;

template<typename PolyCollection>
struct model_of;

template<typename Model,typename Allocator>
struct model_of<poly_collection<Model,Allocator>>
{
  using type=Model;
};

template<typename Iterator>
struct iterator_traits
{
  using container_type=typename poly_collection_of<Iterator>::type;
  using is_const_iterator=typename std::is_const<
    typename std::remove_reference<
      typename std::iterator_traits<Iterator>::reference
    >::type
  >::type;
  using iterator=typename std::conditional<
    is_const_iterator::value,
    typename container_type::const_iterator,   
    typename container_type::iterator
  >::type;
  using segment_info_iterator=typename std::conditional<
    is_const_iterator::value,
    typename container_type::const_segment_info_iterator,   
    typename container_type::segment_info_iterator
  >::type;
  using local_base_iterator=typename std::conditional<
    is_const_iterator::value,
    typename container_type::const_local_base_iterator,   
    typename container_type::local_base_iterator
  >::type;
  template<typename T>
  using local_iterator=typename std::conditional<
    is_const_iterator::value,
    typename container_type::template const_local_iterator<T>,   
    typename container_type::template local_iterator<T>
  >::type;

  static segment_info_iterator
  segment_info_iterator_from(iterator it)noexcept{return it.mapit;} 

  static segment_info_iterator
  segment_info_iterator_from(local_base_iterator it)noexcept{return it.mapit;} 

  static segment_info_iterator
  end_segment_info_iterator_from(iterator it)noexcept{return it.mapend;} 

  static local_base_iterator
  local_base_iterator_from(iterator it)noexcept
  {
    return {
      it.mapit,
      model_of<container_type>::type::nonconst_iterator(it.segpos)
    };
  }

  static iterator 
  iterator_from(
    local_base_iterator lbit,segment_info_iterator mapend)noexcept
  {
    return {lbit.mapit,mapend.base(),lbit.base()};
  }
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
