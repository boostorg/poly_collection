/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_BASE_POLYMORPHISM_HPP
#define BOOST_POLY_COLLECTION_DETAIL_BASE_POLYMORPHISM_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/core/addressof.hpp>
#include <boost/poly_collection/detail/is_final.hpp>
#include <type_traits>
#include <typeinfo>

namespace boost{

namespace poly_collection{

namespace detail{

/* base polymorphism model */

template<typename Base>
struct base_polymorphism
{
  using value_type=Base;
  using type_index=std::type_info;
  template<typename Derived>
  using is_implementation=std::is_base_of<Base,Derived>;
  template<typename T>
  using is_terminal=is_final<T>; //TODO: should we say !is_polymorphic||is_final?

private:
  template<typename T>
  using enable_if_not_terminal=
    typename std::enable_if<!is_terminal<T>::value>::type*;
  template<typename T>
  using enable_if_terminal=
    typename std::enable_if<is_terminal<T>::value>::type*;

public:
  template<typename T> 
  static const std::type_info& index(){return typeid(T);}

  template<typename T,enable_if_not_terminal<T> =nullptr>
  static const std::type_info& subindex(const T& x){return typeid(x);}

  template<typename T,enable_if_terminal<T> =nullptr>
  static const std::type_info& subindex(const T&){return typeid(T);}

  template<typename T,enable_if_not_terminal<T> =nullptr>
  static void* subaddress(T& x)
  {
    return dynamic_cast<void*>(boost::addressof(x));
  }

  template<typename T,enable_if_not_terminal<T> =nullptr>
  static const void* subaddress(const T& x)
  {
    return dynamic_cast<const void*>(boost::addressof(x));
  }

  template<typename T,enable_if_terminal<T> =nullptr>
  static void* subaddress(T& x){return boost::addressof(x);}

  template<typename T,enable_if_terminal<T> =nullptr>
  static const void* subaddress(const T& x){return boost::addressof(x);}
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
