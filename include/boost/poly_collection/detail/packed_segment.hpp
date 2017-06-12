/* Copyright 2016-2017 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_PACKED_SEGMENT_HPP
#define BOOST_POLY_COLLECTION_DETAIL_PACKED_SEGMENT_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/poly_collection/detail/segment_backend.hpp>
#include <memory>
#include <new>
#include <vector>
#include <utility>

namespace boost{

namespace poly_collection{

namespace detail{

/* segment_backend implementation where value_type& and Concrete& actually refer
 * to the same stored entity of type Model::element_type<Concrete>.
 *
 * Requires:
 *  - [const_]base_iterator is a stride iterator constructible from
 *    {value_type*,sizeof(element_type<Concrete>)}.
 *  - element_type<Concrete> is copy constructible from a Concrete&, copy
 *    constructible, move assignable, equality comparable and type-erased
 *    emplaceable (see segment_backend.hpp).
 *  - Model provides functions value_ptr for
 *    element_type<Concrete>* -> value_type* conversion and element_ptr for
 *    Concrete* -> element_type<Concrete>* conversion.
 */

template<typename Model,typename Concrete,typename Allocator>
class packed_segment:public segment_backend<Model>
{
  using value_type=typename Model::value_type;
  using store_value_type=typename Model::template element_type<Concrete>;
  template<typename Concrete2>
  using const_iterator=typename Model::template const_iterator<Concrete2>;
  using store=std::vector<
    store_value_type,
    typename std::allocator_traits<Allocator>::
      template rebind_alloc<store_value_type>
  >;
  using store_iterator=typename store::iterator;
  using const_store_iterator=typename store::const_iterator;
  using segment_backend=detail::segment_backend<Model>;
  using typename segment_backend::segment_backend_unique_ptr;
  using typename segment_backend::value_pointer;
  using typename segment_backend::const_value_pointer;
  using typename segment_backend::base_iterator;
  using typename segment_backend::const_base_iterator;
  using typename segment_backend::base_sentinel;
  using typename segment_backend::position_pointer;
  using typename segment_backend::range;
  using segment_allocator_type=typename std::allocator_traits<Allocator>::
    template rebind_alloc<packed_segment>;

public:
  virtual ~packed_segment()=default;

  virtual segment_backend_unique_ptr copy()const
  {
    return new_(s.get_allocator(),store{s});
  }

  virtual segment_backend_unique_ptr empty_copy()const
  {
    return new_(s.get_allocator(),s.get_allocator());
  }

  virtual bool equal(const segment_backend* p)const
  {
    return s==static_cast<const packed_segment*>(p)->s;
  }

  virtual base_iterator begin()const noexcept
  {
    return {value_ptr(s.data()),sizeof(store_value_type)};
  }

  virtual base_iterator end()const noexcept
  {
    return {value_ptr(s.data()+s.size()),sizeof(store_value_type)};
  }

  virtual bool          empty()const noexcept{return s.empty();}
  virtual std::size_t   size()const noexcept{return s.size();}
  virtual std::size_t   max_size()const noexcept{return s.max_size();}
  virtual std::size_t   capacity()const noexcept{return s.capacity();}
  virtual base_sentinel reserve(std::size_t n){s.reserve(n);return sentinel();}
  virtual base_sentinel shrink_to_fit(){s.shrink_to_fit();return sentinel();}

  virtual range emplace(
    const_base_iterator p,void (*emplf)(void*,void*),void* arg)
  {
    return range_from(s.emplace(iterator_from(p),emplf,arg));
  }

  virtual range emplace(
    position_pointer p,void (*emplf)(void*,void*),void* arg)
  {
    return range_from(s.emplace(iterator_from(p),emplf,arg));
  }

  virtual range emplace_back(void (*emplf)(void*,void*),void* arg)
  {
    s.emplace_back(emplf,arg);
    return range_from(s.size()-1);
  }

  virtual range push_back(const_value_pointer x)
  {
    s.push_back(const_concrete_ref(x));
    return range_from(s.size()-1);
  }

  virtual range push_back_move(value_pointer x)
  {
    s.push_back(std::move(concrete_ref(x)));
    return range_from(s.size()-1);
  }

  virtual range insert(const_base_iterator p,const_value_pointer x)
  {
    return range_from(s.insert(iterator_from(p),const_concrete_ref(x)));
  }

  virtual range insert(position_pointer p,const_value_pointer x)
  {
    return range_from(s.insert(iterator_from(p),const_concrete_ref(x)));
  }

  virtual range insert_move(const_base_iterator p,value_pointer x)
  {
    return range_from(s.insert(iterator_from(p),std::move(concrete_ref(x))));
  }

  virtual range insert_move(position_pointer p,value_pointer x)
  {
    return range_from(s.insert(iterator_from(p),std::move(concrete_ref(x))));
  }

  virtual range erase(const_base_iterator p)
  {
    return range_from(s.erase(iterator_from(p)));
  }

  virtual range erase(position_pointer p)
  {
    return range_from(s.erase(iterator_from(p)));
  }
    
  virtual range erase(const_base_iterator first,const_base_iterator last)
  {
    return range_from(s.erase(iterator_from(first),iterator_from(last)));
  }

  virtual range erase(position_pointer first,position_pointer last)
  {
    return range_from(s.erase(iterator_from(first),iterator_from(last)));
  }

  virtual range erase_till_end(const_base_iterator first)
  {
    return range_from(s.erase(iterator_from(first),s.end()));
  }

  virtual range erase_till_end(position_pointer first)
  {
    return range_from(s.erase(iterator_from(first),s.end()));
  }

  virtual range erase_from_begin(const_base_iterator last)
  {
    return range_from(s.erase(s.begin(),iterator_from(last)));
  }

  virtual range erase_from_begin(position_pointer last)
  {
    return range_from(s.erase(s.begin(),iterator_from(last)));
  }

  virtual base_sentinel clear()noexcept{s.clear();return sentinel();}

private:
  friend Model;

  template<typename... Args>
  static segment_backend_unique_ptr new_(
    segment_allocator_type al,Args&&... args)
  {
    auto p=std::allocator_traits<segment_allocator_type>::allocate(al,1);
    try{
      ::new ((void*)p) packed_segment{std::forward<Args>(args)...};
    }
    catch(...){
      std::allocator_traits<segment_allocator_type>::deallocate(al,p,1);
      throw;
    }
    return {p,&delete_};
  }

  static void delete_(segment_backend* p)
  {
    auto q=static_cast<packed_segment*>(p);
    auto al=segment_allocator_type{q->s.get_allocator()};
    q->~packed_segment();
    std::allocator_traits<segment_allocator_type>::deallocate(al,q,1);
  }

  packed_segment(const Allocator& al):s{al}{}
  packed_segment(store&& s):s{std::move(s)}{}

  static Concrete& concrete_ref(value_pointer p)noexcept
  {
    return *static_cast<Concrete*>(p);
  }

  static const Concrete& const_concrete_ref(const_value_pointer p)noexcept
  {
    return *static_cast<const Concrete*>(p);
  }

  static const Concrete* const_concrete_ptr(position_pointer p)noexcept
  {
    return static_cast<const Concrete*>(p);
  }

  static value_type* value_ptr(const store_value_type* p)noexcept
  {
    return const_cast<value_type*>(Model::value_ptr(p));
  }

  static const store_value_type* const_store_value_type_ptr(
    position_pointer p)noexcept
  {
    return Model::element_ptr(const_concrete_ptr(p));
  }

  /* It would have sufficed if iterator_from returned const_store_iterator
   * except for the fact that some old versions of libstdc++ claiming to be
   * C++11 compliant do not however provide std::vector modifier ops taking
   * const_iterator's.
   */

  store_iterator iterator_from(const_base_iterator p)
  {
    return iterator_from(
      static_cast<const Concrete*>(
        static_cast<const_iterator<Concrete>>(p)));
  }

  store_iterator iterator_from(position_pointer p)
  {
    return s.begin()+(const_store_value_type_ptr(p)-s.data());
  }

  base_sentinel sentinel()const noexcept
  {
    return base_iterator{
      value_ptr(s.data()+s.size()),
      sizeof(store_value_type)
    };
  }

  range range_from(const_store_iterator it)const
  {
    return {
      {value_ptr(s.data()+(it-s.begin())),sizeof(store_value_type)},
      sentinel()
    };
  }
    
  range range_from(std::size_t n)const
  {
    return {
      {value_ptr(s.data()+n),sizeof(store_value_type)},
      sentinel()
    };
  }

  store s;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
