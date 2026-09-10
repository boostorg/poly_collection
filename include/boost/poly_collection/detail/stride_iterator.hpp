/* Copyright 2016-2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_STRIDE_ITERATOR_HPP
#define BOOST_POLY_COLLECTION_DETAIL_STRIDE_ITERATOR_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/iterator/iterator_facade.hpp>
#include <cstddef>
#include <type_traits>

namespace boost{

namespace poly_collection{

namespace detail{

/* Random-access iterator to Base elements laid out stride *chars* apart.
 * Traits provides user-defined machinery for interoperability with
 * associated typed iterators.
 */

template<typename Base,typename Traits>
class stride_iterator:
  public boost::iterator_facade<
    stride_iterator<Base,Traits>,
    Base,
    boost::random_access_traversal_tag
  >
{
public:
  stride_iterator()=default;
  stride_iterator(Base* p,std::size_t stride)noexcept:p{p},stride_{stride}{}
  stride_iterator(const stride_iterator&)=default;
  stride_iterator& operator=(const stride_iterator&)=default;

  template<
    typename NonConstBase,
    typename std::enable_if<
      std::is_same<Base,const NonConstBase>::value>::type* =nullptr
  >
  stride_iterator(const stride_iterator<NonConstBase,Traits>& x)noexcept:
    p{x.p},stride_{x.stride_}{}

  template<
    typename NonConstBase,
    typename std::enable_if<
      std::is_same<Base,const NonConstBase>::value>::type* =nullptr
  >
  stride_iterator& operator=(
    const stride_iterator<NonConstBase,Traits>& x)noexcept
  {
    p=x.p;stride_=x.stride_;
    return *this;
  }

  /* interoperability with associated typed iterator */

  template<
    typename T,
    typename NonConstT=typename std::remove_const<T>::type,
    typename NonConstBase=typename std::remove_const<Base>::type,
    typename std::enable_if<
      Traits::template is_implementation<NonConstBase,NonConstT>::value&&
      (std::is_const<Base>::value||!std::is_const<T>::value)
    >::type* =nullptr
  >
  explicit stride_iterator(
    const typename Traits::template iterator<NonConstBase,T>& x)noexcept:
    p{const_cast<Base*>(Traits::template base_pointer_from<NonConstBase>(x))},
    stride_{Traits::stride_from(x)}
    {}

  template<
    typename T,
    typename NonConstT=typename std::remove_const<T>::type,
    typename NonConstBase=typename std::remove_const<Base>::type,
    typename std::enable_if<
      Traits::template is_implementation<NonConstBase,NonConstT>::value&&
      (!std::is_const<Base>::value||std::is_const<T>::value)
    >::type* =nullptr
  >
  explicit operator 
    typename Traits::template iterator<NonConstBase,T>()const noexcept
  {
    return Traits::template make_iterator<
      typename Traits::template iterator<NonConstBase,T>>(p);
  }

  /* comparability with Base* sentinel (if used) */

  operator Base*()const noexcept{return p;}

  /* nullification used by poly_collection global iterators */

  stride_iterator& operator=(std::nullptr_t)noexcept
  {
    return *this=stride_iterator{};
  }

  std::size_t stride()const noexcept{return stride_;}

private:
  template<typename,typename>
  friend class stride_iterator;

  using char_pointer=typename std::conditional<
    std::is_const<Base>::value,
    const char*,
    char*
  >::type;

  static char_pointer char_ptr(Base* p)noexcept
    {return reinterpret_cast<char_pointer>(p);}
  static Base*       value_ptr(char_pointer p)noexcept
    {return reinterpret_cast<Base*>(p);}

  friend class boost::iterator_core_access;

  Base& dereference()const noexcept{return *p;}
  bool equal(const stride_iterator& x)const noexcept{return p==x.p;}
  void increment()noexcept{p=value_ptr(char_ptr(p)+stride_);}
  void decrement()noexcept{p=value_ptr(char_ptr(p)-stride_);}
  template<typename Integral>
  void advance(Integral n)noexcept
    {p=value_ptr(char_ptr(p)+n*(std::ptrdiff_t)stride_);}
  std::ptrdiff_t distance_to(const stride_iterator& x)const noexcept
    {return (char_ptr(x.p)-char_ptr(p))/(std::ptrdiff_t)stride_;}          

  Base*       p;
  std::size_t stride_;
};

/* stride iterator with Stride known at compile time */

template<typename T,std::size_t Stride>
class static_stride_iterator:
  public boost::iterator_facade<
    static_stride_iterator<T,Stride>,
    T,
    boost::random_access_traversal_tag
  >
{
public:
  static_stride_iterator()=default;
  static_stride_iterator(T* p)noexcept:p{p}{}
  static_stride_iterator(
    const static_stride_iterator&)=default;
  static_stride_iterator& operator=(
    const static_stride_iterator&)=default;

  template<
    typename NonConstT,
    typename std::enable_if<
      std::is_same<T,const NonConstT>::value>::type* =nullptr
  >
  static_stride_iterator(
    const static_stride_iterator<NonConstT,Stride>& x)noexcept:
    p{x.p}{}

  template<
    typename NonConstT,
    typename std::enable_if<
      std::is_same<T,const NonConstT>::value>::type* =nullptr
  >
  static_stride_iterator& operator=(
    const static_stride_iterator<NonConstT,Stride>& x)noexcept
  {
    p=x.p;
    return *this;
  }

  /* interoperability with T* */

  operator T*()const noexcept{return p;}

private:
  template<typename,std::size_t>
  friend class static_stride_iterator;

  using char_pointer=typename std::conditional<
    std::is_const<T>::value,
    const char*,
    char*
  >::type;

  static char_pointer char_ptr(T* p)noexcept
    {return reinterpret_cast<char_pointer>(p);}
  static T*           value_ptr(char_pointer p)noexcept
    {return reinterpret_cast<T*>(p);}

  friend class boost::iterator_core_access;

  T& dereference()const noexcept{return *p;}
  bool equal(const static_stride_iterator& x)const noexcept
    {return p==x.p;}
  void increment()noexcept{p=value_ptr(char_ptr(p)+Stride);}
  void decrement()noexcept{p=value_ptr(char_ptr(p)-Stride);}
  template<typename Integral>
  void advance(Integral n)noexcept
    {p=value_ptr(char_ptr(p)+n*(std::ptrdiff_t)Stride);}
  std::ptrdiff_t distance_to(
    const static_stride_iterator& x)const noexcept
    {return (char_ptr(x.p)-char_ptr(p))/(std::ptrdiff_t)Stride;}

  T* p;
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
