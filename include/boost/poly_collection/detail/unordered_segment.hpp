/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_UNORDERED_SEGMENT_HPP
#define BOOST_POLY_COLLECTION_DETAIL_UNORDERED_SEGMENT_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace boost{

namespace poly_collection{

namespace detail{

/* Equivalent of segment<Model> for unordered collections (those for which
 * Model::segment_backend implements unordered_segment_backend virtual 
 * interface).
 */

template<typename Model>
class unordered_segment
{
public:
  using is_unordered=void;
  using value_type=typename Model::value_type;
  using allocator_type=typename Model::segment_allocator_type;
    /* needed for uses-allocator construction */
  using base_iterator=typename Model::base_iterator;
  using const_base_iterator=typename Model::const_base_iterator;
  using base_sentinel=typename Model::base_sentinel;
  using const_base_sentinel=typename Model::const_base_sentinel;
  template<typename T>
  using iterator=typename Model::template iterator<T>;
  template<typename T>
  using const_iterator=typename Model::template const_iterator<T>;

  template<typename T>
  static unordered_segment make(const allocator_type& al)
  {
    return segment_backend_implementation<T>::make(al);
  }

  /* clones the implementation of x with no elements */

  static unordered_segment make_from_prototype(
    const unordered_segment& x,const allocator_type& al)
  {
    return {from_prototype{},x,al};
  }

  unordered_segment(const unordered_segment& x):
    pimpl{x.impl().copy()}{set_sentinel();}
  unordered_segment(unordered_segment&& x)=default;
  unordered_segment(const unordered_segment& x,const allocator_type& al):
    pimpl{x.impl().copy(al)}{set_sentinel();}

  /* TODO: try ptr-level move before impl().move() */
  unordered_segment(unordered_segment&& x,const allocator_type& al):
    pimpl{x.impl().move(al)}{set_sentinel();}

  unordered_segment& operator=(const unordered_segment& x)
  {
    pimpl=allocator_traits::propagate_on_container_copy_assignment::value?
      x.impl().copy():x.impl().copy(impl().get_allocator());
    set_sentinel();
    return *this;
  }
  
  unordered_segment& operator=(unordered_segment&& x)
  {
    pimpl=x.impl().move(
      allocator_traits::propagate_on_container_move_assignment::value?
      x.impl().get_allocator():impl().get_allocator());
    set_sentinel();
    return *this;
  }

  base_iterator        begin()const noexcept{return impl().begin();}
  template<typename U>
  base_iterator        begin()const noexcept{return impl<U>().nv_begin();}
  base_iterator        end()const noexcept{return impl().end();}
  template<typename U>
  base_iterator        end()const noexcept{return impl<U>().nv_end();}
  base_sentinel        sentinel()const noexcept{return snt;}
  bool                 empty()const noexcept{return impl().empty();}
  template<typename U>
  bool                 empty()const noexcept{return impl<U>().nv_empty();}
  std::size_t          size()const noexcept{return impl().size();}
  template<typename U>
  std::size_t          size()const noexcept{return impl<U>().nv_size();}
  std::size_t          max_size()const noexcept{return impl().max_size();}
  template<typename U>
  std::size_t          max_size()const noexcept
                         {return impl<U>().nv_max_size();}
  void                 reserve(std::size_t n){impl().reserve(n);}
  template<typename U>
  void                 reserve(std::size_t n){impl<U>().nv_reserve(n);}
  std::size_t          capacity()const noexcept{return impl().capacity();}
  template<typename U>
  std::size_t          capacity()const noexcept
                         {return impl<U>().nv_capacity();}
  void                 shrink_to_fit(){impl().shrink_to_fit();}
  template<typename U>
  void                 shrink_to_fit(){impl<U>().nv_shrink_to_fit();}

  template<typename U,typename... Args>
  base_iterator emplace_back(Args&&... args)
  {
    return impl<U>().nv_emplace_back(std::forward<Args>(args)...);
  }

  template<typename T>
  base_iterator push_back(const T& x)
  {
    return impl().push_back(subaddress(x));
  }

  template<
    typename T,
    typename std::enable_if<
      !std::is_lvalue_reference<T>::value&&!std::is_const<T>::value
    >::type* =nullptr
  >
  base_iterator push_back(T&& x)
  {
    return impl().push_back_move(subaddress(x));
  }

  template<typename T>
  base_iterator push_back_hint(const_base_iterator,const T& x) 
  {
    return push_back(x);
  }

  template<typename T>
  base_iterator push_back_hint(base_iterator,const T& x) 
  {
    return push_back(x);
  }

  template<typename U,typename T>
  base_iterator push_back_hint(const_iterator<U>,const T& x)
  {
    return impl<U>().nv_push_back(*static_cast<const U*>(subaddress(x)));
  }

  template<typename U,typename T>
  base_iterator push_back_hint(iterator<U> it,const T& x)
  {
    return push_back_hint(const_iterator<U>{it},x);
  }

  template<
    typename T,
    typename std::enable_if<
      !std::is_lvalue_reference<T>::value&&!std::is_const<T>::value
    >::type* =nullptr
  >
  base_iterator push_back_hint(const_base_iterator,T&& x)
  {
    return push_back(std::forward<T>(x));
  }

  template<
    typename T,
    typename std::enable_if<
      !std::is_lvalue_reference<T>::value&&!std::is_const<T>::value
    >::type* =nullptr
  >
  base_iterator push_back_hint(base_iterator,T&& x)
  {
    return push_back(std::forward<T>(x));
  }

  template<
    typename U,typename T,
    typename std::enable_if<
      !std::is_lvalue_reference<T>::value&&!std::is_const<T>::value
    >::type* =nullptr
  >
  base_iterator push_back_hint(const_iterator<U>,T&& x)
  {
    return impl<U>().nv_push_back(std::move(*static_cast<U*>(subaddress(x))));
  }

  template<
    typename U,typename T,
    typename std::enable_if<
      !std::is_lvalue_reference<T>::value&&!std::is_const<T>::value
    >::type* =nullptr
  >
  base_iterator push_back_hint(iterator<U> it,T&& x)
  {
    return push_back_hint(const_iterator<U>{it},std::forward<T>(x));
  }

  template<typename U>
  base_iterator push_back_terminal(U&& x)
  {
    return impl<typename std::decay<U>::type>().
      nv_push_back(std::forward<U>(x));
  }

  template<typename InputIterator>
  void insert(InputIterator first,InputIterator last)
  {
    impl<typename std::iterator_traits<InputIterator>::value_type>().
      nv_insert(first,last);
  }

  base_iterator erase(const_base_iterator it)
  {
    return impl().erase(it);
  }

  template<typename U>
  base_iterator erase(const_iterator<U> it)
  {
    return impl<U>().nv_erase(it);
  }

  template<typename U>
  base_iterator erase(iterator<U> it)
  {
    return erase(const_iterator<U>{it});
  }

  base_iterator erase(const_base_iterator f,const_base_iterator l)
  {
    return impl().erase(f,l);
  }

  template<typename U>
  base_iterator erase(const_iterator<U> f,const_iterator<U> l)
  {
    return impl<U>().nv_erase(f,l);
  }

  template<typename U>
  base_iterator erase(iterator<U> f,iterator<U> l)
  {
    return erase(const_iterator<U>{f},const_iterator<U>{l});
  }

  template<typename Iterator>
  base_iterator erase_till_end(Iterator f)
  {
    return impl().erase_till_end(f);
  }

  template<typename Iterator>
  base_iterator erase_from_begin(Iterator l)
  {
    return impl().erase_from_begin(l);
  }
  
  void                 clear()noexcept{impl().clear();}
  template<typename U>
  void                 clear()noexcept{impl<U>().nv_clear();}

private:
  using allocator_traits=std::allocator_traits<allocator_type>;
  using segment_backend=typename Model::segment_backend;
  template<typename Concrete>
  using segment_backend_implementation=typename Model::
    template segment_backend_implementation<Concrete>;
  using segment_backend_unique_ptr=
    typename segment_backend::segment_backend_unique_ptr;

  struct from_prototype{};

  unordered_segment(segment_backend_unique_ptr&& pimpl):
    pimpl{std::move(pimpl)}{set_sentinel();}
  unordered_segment(
    from_prototype,const unordered_segment& x,const allocator_type& al):
    pimpl{x.impl().empty_copy(al)}{set_sentinel();}

  segment_backend&       impl()noexcept{return *pimpl;}
  const segment_backend& impl()const noexcept{return *pimpl;}

  template<typename Concrete>
  segment_backend_implementation<Concrete>& impl()noexcept
  {
    return static_cast<segment_backend_implementation<Concrete>&>(impl());
  }

  template<typename Concrete>
  const segment_backend_implementation<Concrete>& impl()const noexcept
  {
    return
      static_cast<const segment_backend_implementation<Concrete>&>(impl());
  }

  template<typename T>
  static void*         subaddress(T& x){return Model::subaddress(x);}
  template<typename T>
  static const void*   subaddress(const T& x){return Model::subaddress(x);}

  void                 set_sentinel(){snt=impl().end();}

  segment_backend_unique_ptr pimpl;
  base_sentinel              snt;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
