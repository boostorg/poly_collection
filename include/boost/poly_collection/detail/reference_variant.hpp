/* Copyright 2024 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_REFERENCE_VARIANT_HPP
#define BOOST_POLY_COLLECTION_DETAIL_REFERENCE_VARIANT_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/config.hpp>
#include <boost/core/addressof.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/function.hpp>
#include <boost/mp11/list.hpp>
#include <boost/mp11/set.hpp>
#include <boost/type_traits/make_void.hpp> 
#include <boost/variant2/variant.hpp>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace boost{

namespace poly_collection{

namespace reference_variant_impl{

/* variant-like holder of external references from a given set of types */

template<typename... Ts>
class reference_variant
{
  static_assert(
    mp11::mp_is_set<reference_variant>::value,
    "all types in the variant must be distinct");

public:
  template<
    typename T,
    std::size_t Idx=mp11::mp_find<reference_variant,T>::value,
    typename std::enable_if<
      (Idx<mp11::mp_size<reference_variant>::value)>::type* =nullptr
  >
  constexpr reference_variant(T& x):v{boost::addressof(x)}{}

  reference_variant(const reference_variant&)=default;
  reference_variant(reference_variant&&)=default;
  reference_variant& operator=(const reference_variant&)=default;

  constexpr std::size_t index()const noexcept{return v.index();}
  constexpr bool valueless_by_exception()const noexcept{return false;}

private:
  friend struct access;

  variant2::variant<Ts*...> v;
};

struct access
{
  template<typename... Ts>
  static constexpr auto impl(reference_variant<Ts...>& x)->decltype((x.v))
  {return x.v;}

  template<typename... Ts>
  static constexpr auto
  impl(const reference_variant<Ts...>& x)->decltype((x.v))
  {return x.v;}

  template<typename... Ts>
  static constexpr
  auto impl(reference_variant<Ts...>&& x)->decltype(std::move(x.v))
  {return std::move(x.v);}

  template<typename... Ts>
  static constexpr auto
  impl(const reference_variant<Ts...>&& x)->decltype(std::move(x.v))
  {return std::move(x.v);}
};

using bad_variant_access=variant2::bad_variant_access;

template<typename T> struct variant_size;
template<typename... Ts>
struct variant_size<reference_variant<Ts...>>:
  std::integral_constant<std::size_t, sizeof...(Ts)>{};
template<typename T> struct variant_size<const T>:variant_size<T>{};

#ifndef BOOST_NO_CXX14_VARIABLE_TEMPLATES
template<typename T>
constexpr std::size_t variant_size_v=variant_size<T>::value;
#endif

template<std::size_t Idx,typename T,typename=void> struct variant_alternative;

template<typename T> struct is_reference_variant:std::false_type{};
template<typename... Ts>
struct is_reference_variant<reference_variant<Ts...>>:std::true_type{};

template<typename T,typename Q> 
struct transfer_cref{using type=Q;};
template<typename T,typename Q>
struct transfer_cref<const T,Q>{using type=const Q;};
template<typename T,typename Q>
struct transfer_cref<T&,Q>
{using type=typename transfer_cref<T,Q>::type&;};
template<typename T,typename Q>
struct transfer_cref<T&&,Q>
{using type=typename transfer_cref<T,Q>::type&&;};

template<std::size_t Idx,typename V>
struct variant_alternative<
  Idx,V,
  typename std::enable_if<
    is_reference_variant<typename std::decay<V>::type>::value
  >::type
>
{
  using type=typename transfer_cref<
    V,mp11::mp_at_c<typename std::decay<V>::type,Idx>>::type;
};

template<std::size_t Idx,typename V>
using variant_alternative_t=typename variant_alternative<Idx,V>::type;

template<typename T>
constexpr T& deref_as_pointer(T*& p){return *p;}
template<typename T>
constexpr const T& deref_as_pointer(T* const & p){return *p;}
template<typename T>
constexpr T&& deref_as_pointer(T*&& p){return std::move(*p);}
template<typename T>
constexpr const T&& deref_as_pointer(T* const && p){return std::move(*p);}

template<typename F>
struct deref_and_call
{
  F f;

  template<typename... Ts>
  constexpr auto operator()(Ts&&... xs)->
    decltype(f(deref_as_pointer(std::forward<Ts>(xs))...))
  {
    return f(deref_as_pointer(std::forward<Ts>(xs))...);
  }
};

template<
  typename R=variant2::detail::deduced,typename F,typename... Vs,
  typename std::enable_if<
    mp11::mp_and<is_reference_variant<typename std::decay<Vs>::type>...>::value
  >::type* =nullptr
>
constexpr auto visit(F&& f,Vs&&... xs)->decltype(
  variant2::visit(
    deref_and_call<F&&>{std::forward<F>(f)},
    access::impl(std::forward<Vs>(xs))...))
{
  return variant2::visit(
    deref_and_call<F&&>{std::forward<F>(f)},
    access::impl(std::forward<Vs>(xs))...);
}

template<typename T,typename... Ts>
constexpr bool holds_alternative(const reference_variant<Ts...>& x)noexcept
{
  return variant2::holds_alternative<T*>(access::impl(x));
}

template<std::size_t Idx,typename V>
constexpr variant_alternative_t<Idx,V&&> get(V&& x)
{
  return deref_as_pointer(
    variant2::get<Idx>(access::impl(std::forward<V>(x))));
}

template<typename T,typename V>
constexpr typename transfer_cref<V&&,T>::type get(V&& x)
{
  return deref_as_pointer(variant2::get<T*>(access::impl(std::forward<V>(x))));
}

template<std::size_t Idx,typename V>
constexpr variant_alternative_t<Idx,V&&> unsafe_get(V&& x)
{
  return deref_as_pointer(
    variant2::unsafe_get<Idx>(access::impl(std::forward<V>(x))));
}

template<typename T>
constexpr T* get_if_helper(T* const * p)
{
  return p?*p:nullptr;
}

template<std::size_t Idx,typename... Ts>
constexpr variant_alternative_t<Idx,reference_variant<Ts...>>*
get_if(reference_variant<Ts...>* px)noexcept
{
  return px?
    get_if_helper(variant2::get_if<Idx>(&access::impl(*px))):nullptr;
}

template<std::size_t Idx,typename... Ts>
constexpr const variant_alternative_t<Idx,reference_variant<Ts...>>*
get_if(const reference_variant<Ts...>* px)noexcept
{
  return px?
    get_if_helper(variant2::get_if<Idx>(&access::impl(*px))):nullptr;
}

template<typename T,typename... Ts>
constexpr T* get_if(reference_variant<Ts...>* px)noexcept
{
  return px?
    get_if_helper(variant2::get_if<T*>(&access::impl(*px))):nullptr;
}

template<typename T,typename... Ts>
constexpr const T* get_if(const reference_variant<Ts...>* px)noexcept
{
  return px?
    get_if_helper(variant2::get_if<T*>(&access::impl(*px))):nullptr;
}

template<typename RelOp,typename ... Ts>
struct relop_helper
{
  const reference_variant<Ts...> &x,&y;

  template<typename Idx> constexpr bool operator()(Idx)const
  {
    return RelOp{}(unsafe_get<Idx::value>(x),unsafe_get<Idx::value>(y));
  }
};

struct eq_
{
  template<typename T>
  constexpr bool operator()(const T& x,const T& y){return x==y;}
};

template<typename... Ts>
constexpr bool operator==(
  const reference_variant<Ts...>& x,const reference_variant<Ts...>& y)
{
  return
    x.index()==y.index()&&
    mp11::mp_with_index<sizeof...(Ts)>(x.index(),relop_helper<eq_,Ts...>{x,y});
}

struct neq_
{
  template<typename T>
  constexpr bool operator()(const T& x,const T& y){return x!=y;}
};

template<typename... Ts>
constexpr bool operator!=(
  const reference_variant<Ts...>& x,const reference_variant<Ts...>& y)
{
  return
    x.index()!=y.index()||
    mp11::mp_with_index<sizeof...(Ts)>(
      x.index(),relop_helper<neq_,Ts...>{x,y});
}

struct lt_
{
  template<typename T>
  constexpr bool operator()(const T& x,const T& y){return x<y;}
};

template<typename... Ts>
constexpr bool operator<(
  const reference_variant<Ts...>& x,const reference_variant<Ts...>& y)
{
  return
    x.index()<y.index()||
    (x.index()==y.index()&&mp11::mp_with_index<sizeof...(Ts)>(
      x.index(),relop_helper<lt_,Ts...>{x,y}));
}

struct lte_
{
  template<typename T>
  constexpr bool operator()(const T& x,const T& y){return x<=y;}
};

template<typename... Ts>
constexpr bool operator<=(
  const reference_variant<Ts...>& x,const reference_variant<Ts...>& y)
{
  return
    x.index()<y.index()||
    (x.index()==y.index()&&mp11::mp_with_index<sizeof...(Ts)>(
      x.index(),relop_helper<lte_,Ts...>{x,y}));
}

struct gt_
{
  template<typename T>
  constexpr bool operator()(const T& x,const T& y){return x>y;}
};

template<typename... Ts>
constexpr bool operator>(
  const reference_variant<Ts...>& x,const reference_variant<Ts...>& y)
{
  return
    x.index()>y.index()||
    (x.index()==y.index()&&mp11::mp_with_index<sizeof...(Ts)>(
      x.index(),relop_helper<gt_,Ts...>{x,y}));
}

struct gte_
{
  template<typename T>
  constexpr bool operator()(const T& x,const T& y){return x>=y;}
};

template<typename... Ts>
constexpr bool operator>=(
  const reference_variant<Ts...>& x,const reference_variant<Ts...>& y)
{
  return
    x.index()>y.index()||
    (x.index()==y.index()&&mp11::mp_with_index<sizeof...(Ts)>(
      x.index(),relop_helper<gte_,Ts...>{x,y}));
}

} /* namespace poly_collection::reference_variant_impl */

namespace detail{

using reference_variant_access=
  boost::poly_collection::reference_variant_impl::access;

} /* namespace poly_collection::detail */

using boost::poly_collection::reference_variant_impl::variant_size;

#ifndef BOOST_NO_CXX14_VARIABLE_TEMPLATES
using boost::poly_collection::reference_variant_impl::variant_size_v;
#endif

using boost::poly_collection::reference_variant_impl::bad_variant_access;
using boost::poly_collection::reference_variant_impl::variant_alternative;
using boost::poly_collection::reference_variant_impl::variant_alternative_t;
using boost::poly_collection::reference_variant_impl::visit;
using boost::poly_collection::reference_variant_impl::holds_alternative;
using boost::poly_collection::reference_variant_impl::get;
using boost::poly_collection::reference_variant_impl::unsafe_get;
using boost::poly_collection::reference_variant_impl::get_if;

} /* namespace poly_collection */

} /* namespace boost */

#endif
