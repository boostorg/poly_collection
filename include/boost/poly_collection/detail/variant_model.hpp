/* Copyright 2024 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_VARIANT_MODEL_HPP
#define BOOST_POLY_COLLECTION_DETAIL_VARIANT_MODEL_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/core/addressof.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/set.hpp>
#include <boost/poly_collection/detail/is_acceptable.hpp>
#include <boost/poly_collection/detail/reference_variant.hpp>
#include <boost/poly_collection/detail/reference_variant_iterator.hpp>
#include <boost/poly_collection/detail/segment_backend.hpp>
#include <boost/poly_collection/detail/split_segment.hpp>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace boost{

namespace poly_collection{

namespace detail{

/* model for variant_collection */

template<typename... Ts>
struct variant_model;

template<typename V,typename...Ts>
struct variant_model_is_subvariant:std::false_type{};

template<
  typename TL1,typename TL2,
  typename TS1=mp11::mp_set_union<mp11::mp_list<>,TL1>,
  typename TS2=mp11::mp_set_union<mp11::mp_list<>,TL2>
>
struct variant_model_is_subset:std::integral_constant<
  bool,
  mp11::mp_size<mp11::mp_set_union<TS1,TS2>>::value==
  mp11::mp_size<TS2>::value
>{};

template<typename... Us,typename... Ts>
struct variant_model_is_subvariant<
  reference_variant_impl::reference_variant<Us...>,
  Ts...
>:variant_model_is_subset<mp11::mp_list<Us...>,mp11::mp_list<Ts...>>{};

template<typename... Us,typename... Ts>
struct variant_model_is_subvariant<
  variant2::variant<Us...>,
  Ts...
>:variant_model_is_subset<mp11::mp_list<Us...>,mp11::mp_list<Ts...>>{};

template<typename... Ts>
struct variant_model
{
  using value_type=reference_variant_impl::reference_variant<Ts...>;

  using type_index=std::size_t;

  using acceptable_type_list=mp11::mp_list<Ts...>;

  template<typename T>
  struct is_terminal: /* using makes VS2015 choke, hence we derive */
    mp11::mp_contains<acceptable_type_list,T>{};

  template<typename T>
  struct is_implementation:std::integral_constant< /* idem */
    bool,
    is_terminal<T>::value||
    variant_model_is_subvariant<T,Ts...>::value
  >{};

private:
  template<typename T>
  using enable_if_terminal=
    typename std::enable_if<is_terminal<T>::value>::type*;
  template<typename T>
  using enable_if_not_terminal=
    typename std::enable_if<!is_terminal<T>::value>::type*;

public:
  template<typename T> static type_index typeid_()
  {
    return mp11::mp_find<acceptable_type_list,T>::value;
  }

  template<typename T,enable_if_terminal<T> =nullptr>
  static type_index subtypeid(const T&){return typeid_<T>();}

private:
  template<typename... Qs>
  struct subtypeid_lambda
  {
    template<typename Idx>
    std::size_t operator()(Idx)const
    {
      return mp11::mp_find<
        acceptable_type_list,mp11::mp_at<mp11::mp_list<Qs...>,Idx>>::value;
    }
  };

public:
  template<
    template<typename...> class V,typename... Qs,
    enable_if_not_terminal<V<Qs...>> =nullptr>
  static type_index subtypeid(const V<Qs...>& x)
  {
    static constexpr auto not_found=mp11::mp_size<acceptable_type_list>::value;
    auto i=x.index();
    if(i>=sizeof...(Qs))return not_found;
    else return mp11::mp_with_index<sizeof...(Qs)>(
      i,subtypeid_lambda<Qs...>{});
  }

  template<typename T,enable_if_terminal<T> =nullptr>
  static void* subaddress(T& x){return boost::addressof(x);}

  template<typename T,enable_if_terminal<T> =nullptr>
  static const void* subaddress(const T& x){return boost::addressof(x);}

  template<typename T,enable_if_not_terminal<T> =nullptr>
  static void* subaddress(T& x)
  {
    return const_cast<void*>(subaddress(const_cast<const T&>(x)));
  }
  
private:
  struct subaddress_visitor
  {
    template<typename T>
    const void* operator()(const T& x)const
    {
      return static_cast<const void*>(boost::addressof(x));
    }
  };

public:
  template<typename T,enable_if_not_terminal<T> =nullptr>
  static const void* subaddress(const T& x)
  {
    return visit(subaddress_visitor{},x);
  }

  template<typename T,enable_if_terminal<T> =nullptr>
  static const std::type_info& subtype_info(const T& x)
  {
    return typeid(x);
  }

private:
  struct subtype_info_visitor
  {
    template<typename T>
    const std::type_info& operator()(const T&)const{return typeid(T);}
  };

public:
  template<typename T,enable_if_not_terminal<T> =nullptr>
  static const std::type_info& subtype_info(const T& x)
  {
    return visit(subtype_info_visitor{},x);
  }

  using base_iterator=reference_variant_iterator<value_type>;
  using const_base_iterator=reference_variant_iterator<const value_type>;
  using base_sentinel=value_type*;
  using const_base_sentinel=const value_type*;
  template<typename T>
  using iterator=T*;
  template<typename T>
  using const_iterator=const T*;
  template<typename Allocator>
  using segment_backend=detail::segment_backend<variant_model,Allocator>;
  template<typename T,typename Allocator>
  using segment_backend_implementation=
    split_segment<variant_model,T,Allocator>;

  static base_iterator nonconst_iterator(const_base_iterator it)
  {
    return base_iterator{
      const_cast<value_type*>(static_cast<const value_type*>(it))};
  }

  template<typename T>
  static iterator<T> nonconst_iterator(const_iterator<T> it)
  {
    return const_cast<iterator<T>>(it);
  }

private:
  template<typename,typename,typename>
  friend class split_segment;

  template<typename T>
  static value_type make_value_type(T& x){return value_type{x};}
};

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
