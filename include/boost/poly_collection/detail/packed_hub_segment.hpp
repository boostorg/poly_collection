/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_PACKED_HUB_SEGMENT_HPP
#define BOOST_POLY_COLLECTION_DETAIL_PACKED_HUB_SEGMENT_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/poly_collection/detail/hub_access.hpp>
#include <boost/poly_collection/detail/unordered_segment_backend.hpp>
#include <boost/poly_collection/detail/value_holder.hpp>
#include <algorithm>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace boost{

namespace poly_collection{

namespace detail{

/* boost::container::hub-based unordered_segment_backend implementation where
 * value_type& and Concrete& actually refer to the same stored entity. 
 *
 * Requires:
 *  - StorageModel provide a template function value_offset<Concrete>
 *    providing the offset between a Concrete address and the address of its
 *    value_type part.
 *  - [const_]base_iterator is a stride iterator constructible from
 *    {pbb,n,sizeof(store_value_type),offset}, where pbb is a pointer
 *    to boost::container::hub_detail::block_base<void*>, n indicates the slot
 *    in the block, and offset has been obtained with value_offset<Concrete>().
 *  - If StorageModel provides a final_type<Concrete> template alias, it is this
 *    type that is stored in the segment rather than Concrete. In this case,
 *    final_type<Concrete> must be constructible from Concrete and
 *    Concrete* must be reinterpret_castable to final_type<Concrete>* and vice
 *    versa.
 */

template<typename StorageModel,typename Concrete>
class packed_hub_segment:public unordered_segment_backend<StorageModel>
{
  template<typename M>
  static typename M::template final_type<Concrete> final_type_helper(M);
  static Concrete final_type_helper(...);

  using value_type=typename StorageModel::value_type;
  using allocator_type=typename StorageModel::segment_allocator_type;
  using final_type=decltype(final_type_helper(std::declval<StorageModel>()));
  using store_value_type=value_holder<final_type,Concrete>;
  using store=boost::container::hub<
    store_value_type,
    typename std::allocator_traits<allocator_type>::
      template rebind_alloc<store_value_type>
  >;
  using store_iterator=typename store::iterator;
  using const_store_iterator=typename store::const_iterator;
  using segment_backend=detail::unordered_segment_backend<StorageModel>;

  static_assert(
    std::is_pointer<
      typename std::allocator_traits<allocator_type>::pointer>::value,
    "packed_hub_segment supports raw-pointer allocators only");

public:
  using typename segment_backend::segment_backend_unique_ptr;
  using typename segment_backend::value_pointer;
  using typename segment_backend::const_value_pointer;
  using typename segment_backend::base_iterator;
  using typename segment_backend::const_base_iterator;
  using const_iterator=
    typename segment_backend::template const_iterator<Concrete>;
  using typename segment_backend::base_sentinel;
  using typename segment_backend::range;
  using segment_allocator_type=typename std::allocator_traits<allocator_type>::
    template rebind_alloc<packed_hub_segment>;

  static segment_backend_unique_ptr make(const segment_allocator_type& al)
  {
    return new_(al,al);
  }

  virtual segment_backend_unique_ptr copy()const
  {
    return new_(s.get_allocator(),store{s});
  }

  virtual segment_backend_unique_ptr copy(const allocator_type& al)const
  {
    return new_(al,store{s,typename store::allocator_type{al}});
  }

  virtual segment_backend_unique_ptr empty_copy(const allocator_type& al)const
  {
    return new_(al,al);
  }

  virtual segment_backend_unique_ptr move(const allocator_type& al)
  {
    return new_(al,store{std::move(s),typename store::allocator_type{al}});
  }

  virtual allocator_type get_allocator()const noexcept
  {
    return allocator_type{s.get_allocator()};
  }

  virtual base_iterator begin()const noexcept{return nv_begin();}

  base_iterator nv_begin()const noexcept
  {
    return make_base_iterator(get_members(s.cbegin()));
  }

  virtual base_iterator end()const noexcept{return nv_end();}

  base_iterator nv_end()const noexcept
  {
    return make_base_iterator(get_members(s.cend()));
  }

  virtual bool        empty()const noexcept{return nv_empty();}
  bool                nv_empty()const noexcept{return s.empty();}
  virtual std::size_t size()const noexcept{return nv_size();}
  std::size_t         nv_size()const noexcept{return s.size();}
  virtual std::size_t max_size()const noexcept{return nv_max_size();}
  std::size_t         nv_max_size()const noexcept{return s.max_size();}
  virtual std::size_t capacity()const noexcept{return nv_capacity();}
  std::size_t         nv_capacity()const noexcept{return s.capacity();}

  virtual base_sentinel reserve(std::size_t n){return nv_reserve(n);}
  base_sentinel         nv_reserve(std::size_t n)
    {s.reserve(n);return sentinel();}

  virtual base_sentinel shrink_to_fit(){return nv_shrink_to_fit();}
  base_sentinel         nv_shrink_to_fit()
    {s.shrink_to_fit();return sentinel();}

  template<typename... Args>
  range nv_emplace_back(Args&&... args)
  {
    return range_from(
      s.emplace(value_holder_emplacing_ctor,std::forward<Args>(args)...));
  }

  virtual range push_back(const_value_pointer x)
  {
    return nv_push_back(const_concrete_ref(x));
  }

  range nv_push_back(const Concrete& x)
  {
    return range_from(s.emplace(x));
  }

  virtual range push_back_move(value_pointer x)
  {
    return nv_push_back(std::move(concrete_ref(x)));
  }

  range nv_push_back(Concrete&& x)
  {
    return range_from(s.emplace(std::move(x)));
  }

  template<typename InputIterator>
  base_sentinel nv_insert(InputIterator first,InputIterator last)
  {
    s.insert(first,last);
    return sentinel();
  }

  virtual range erase(const_base_iterator p)
  {
    return range_from(s.erase(iterator_from(p)));
  }

  range nv_erase(const_iterator p)
  {
    return range_from(s.erase(iterator_from(p)));
  }

  virtual range erase(const_base_iterator first,const_base_iterator last)
  {
    return range_from(s.erase(iterator_from(first),iterator_from(last)));
  }

  range nv_erase(const_iterator first,const_iterator last)
  {
    return range_from(s.erase(iterator_from(first),iterator_from(last)));
  }

  virtual range erase_till_end(const_base_iterator first)
  {
    return range_from(s.erase(iterator_from(first),s.cend()));
  }

  virtual range erase_from_begin(const_base_iterator last)
  {
    return range_from(s.erase(s.cbegin(),iterator_from(last)));
  }

  virtual base_sentinel clear()noexcept{return nv_clear();}
  base_sentinel         nv_clear()noexcept{s.clear();return sentinel();}

private:
  template<typename... Args>
  static segment_backend_unique_ptr new_(
    segment_allocator_type al,Args&&... args)
  {
    auto p=std::allocator_traits<segment_allocator_type>::allocate(al,1);
    try{
      ::new ((void*)p) packed_hub_segment{std::forward<Args>(args)...};
    }
    catch(...){
      std::allocator_traits<segment_allocator_type>::deallocate(al,p,1);
      throw;
    }
    return {p,&delete_};
  }

  static void delete_(segment_backend* p)
  {
    auto q=static_cast<packed_hub_segment*>(p);
    auto al=segment_allocator_type{q->s.get_allocator()};
    q->~packed_hub_segment();
    std::allocator_traits<segment_allocator_type>::deallocate(al,q,1);
  }

  packed_hub_segment(const allocator_type& al):
    s{typename store::allocator_type{al}}{}
  packed_hub_segment(store&& s):s{std::move(s)}{}

  static Concrete& concrete_ref(value_pointer p)noexcept
  {
    return *static_cast<Concrete*>(p);
  }

  static const Concrete& const_concrete_ref(const_value_pointer p)noexcept
  {
    return *static_cast<const Concrete*>(p);
  }

  static const_store_iterator iterator_from(const_base_iterator p)noexcept
  {
    return iterator_from(static_cast<const_iterator>(p));
  }

  static const_store_iterator iterator_from(const_iterator p)noexcept
  {
    return make_hub_iterator<const_store_iterator>(get_members(p));
  }

  static base_iterator make_base_iterator(hub_iterator_members m)noexcept
  {
    return {
      m.pbb,m.n,sizeof(store_value_type),
      StorageModel::template value_offset<Concrete>()};
  }

  base_sentinel sentinel()const noexcept
  {
    return {make_base_iterator(get_members(s.cend()))};
  }

  range range_from(store_iterator it)const noexcept
  {
    return {make_base_iterator(get_members(it)),sentinel()};
  }

  store s;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
