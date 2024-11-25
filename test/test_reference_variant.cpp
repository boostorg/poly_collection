/* Copyright 2024 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#include "test_reference_variant.hpp"

#include <boost/config.hpp>
#include <boost/core/addressof.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/core/lightweight_test_trait.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/poly_collection/detail/reference_variant.hpp>
#include <utility>
#include "test_utilities.hpp"

using namespace test_utilities;

struct non_copyable
{
  constexpr non_copyable(int=0){};
  non_copyable(const non_copyable&)=delete;

  constexpr bool operator==(const non_copyable&)const{return true;}
  constexpr bool operator!=(const non_copyable&)const{return false;}
  constexpr bool operator<(const non_copyable&)const{return false;}
  constexpr bool operator<=(const non_copyable&)const{return true;}
  constexpr bool operator>(const non_copyable&)const{return false;}
  constexpr bool operator>=(const non_copyable&)const{return true;}
};

struct get_addresses
{
  using result_type=std::pair<const void*,const void*>;

  template<typename T,typename Q>
  result_type operator()(const T& x,const Q& y)const{return {&x,&y};}
};

enum cref_qualifier_value
{
  plain_ref=0,
  const_ref,
  rvalue_ref,
  const_rvalue_ref,
};

template<typename T> cref_qualifier_value cref_qualifier(T&)
  {return plain_ref;}
template<typename T> cref_qualifier_value cref_qualifier(const T&)
  {return const_ref;}
template<typename T> cref_qualifier_value cref_qualifier(T&&)
  {return rvalue_ref;}
template<typename T> cref_qualifier_value cref_qualifier(const T&&)
  {return const_rvalue_ref;}

struct get_cref_qualifiers
{
  using result_type=std::pair<cref_qualifier_value,cref_qualifier_value>;

  template<typename T,typename Q>
  result_type operator()(T&& x,Q&& y)const
  {
    return {
      cref_qualifier(std::forward<T>(x)),cref_qualifier(std::forward<Q>(y))
    };
  }
};

struct constexpr_get_sizeof
{
  template<typename T>
  constexpr std::size_t operator()(const T&)const{return sizeof(T);}
};

template<typename T> using add_const=const T;

template<typename V,typename T>
void test_constexpr_reference_variant_for()
{
#ifndef BOOST_CORE_NO_CONSTEXPR_ADDRESSOF /* used on variant construction */
  using namespace boost::poly_collection;

  /* addresses of constexpr objects are const */
  using CV=boost::mp11::mp_transform<add_const,V>; 

  static constexpr std::size_t I=boost::mp11::mp_find<CV,const T>::value;
  static constexpr T           x{};
  static constexpr CV          v{x};
  static constexpr std::size_t res=
    v.index()+
    (v.valueless_by_exception()?1:0)+
#ifdef BOOST_MP11_HAS_CXX14_CONSTEXPR /* mp_with_index requires it */
    (v==v?1:0)+
    (v!=v?1:0)+
    (v< v?1:0)+
    (v<=v?1:0)+
    (v> v?1:0)+
    (v>=v?1:0)+
#endif
    visit(constexpr_get_sizeof{},v)+
    (holds_alternative<const T>(v)?1:0)+
    (get<I>(v)==x?1:0)+
    (get<const T>(v)==x?1:0)+
    (get_if<I>(&v)?1:0)+
    (get_if<const T>(&v)?1:0);

  (void)res;
#endif
}

template<typename V,typename T>
void test_reference_variant_for()
{
  using namespace boost::poly_collection;
  static constexpr std::size_t I=boost::mp11::mp_find<V,T>::value;
  static constexpr std::size_t J=I!=1?1:2;
  using Q=boost::mp11::mp_at_c<V,J>;
  
  test_constexpr_reference_variant_for<V,T>();

  T        x{0},y{1};
  Q        z{0};
  V        v{x},w{y},u{z};
  const V  &cv=v,&cw=w,&cz=z;

  BOOST_TEST(v.index()==I);
  BOOST_TEST(!v.valueless_by_exception());

  BOOST_TEST((cv==cw) == (x==y));
  BOOST_TEST((cv!=cw) == (x!=y));
  BOOST_TEST((cv< cw) == (x< y));
  BOOST_TEST((cv<=cw) == (x<=y));
  BOOST_TEST((cv> cw) == (x> y));
  BOOST_TEST((cv>=cw) == (x>=y));
  BOOST_TEST((cv==cz) == (I==J));
  BOOST_TEST((cv!=cz) == (I!=J));
  BOOST_TEST((cv< cz) == (I< J));
  BOOST_TEST((cv<=cz) == (I<=J));
  BOOST_TEST((cv> cz) == (I> J));
  BOOST_TEST((cv>=cz) == (I>=J));

  BOOST_TEST_TRAIT_TRUE((
    std::is_base_of<boost::mp11::mp_size<V>,variant_size<V>>));
  BOOST_TEST_TRAIT_TRUE((
    std::is_base_of<boost::mp11::mp_size<V>,variant_size<const V>>));

#ifndef BOOST_NO_CXX14_VARIABLE_TEMPLATES
  BOOST_TEST_TRAIT_TRUE((std::integral_constant<
    bool,boost::mp11::mp_size<V>::value==variant_size_v<V>>));
  BOOST_TEST_TRAIT_TRUE((std::integral_constant<
    bool,boost::mp11::mp_size<V>::value==variant_size_v<const V>>));
#endif

  BOOST_TEST_TRAIT_TRUE((
    std::is_same<typename variant_alternative<I,V>::type,T>));
  BOOST_TEST_TRAIT_TRUE((
    std::is_same<typename variant_alternative<I,const V>::type,const T>));
  BOOST_TEST_TRAIT_TRUE((
    std::is_same<typename variant_alternative<I,V&>::type,T&>));
  BOOST_TEST_TRAIT_TRUE((
    std::is_same<typename variant_alternative<I,const V&&>::type,const T&&>));

  BOOST_TEST_TRAIT_TRUE((
    std::is_same<variant_alternative_t<I,V>,T>));
  BOOST_TEST_TRAIT_TRUE((
    std::is_same<variant_alternative_t<I,const V>,const T>));
  BOOST_TEST_TRAIT_TRUE((
    std::is_same<variant_alternative_t<I,V&>,T&>));
  BOOST_TEST_TRAIT_TRUE((
    std::is_same<variant_alternative_t<I,const V&&>,const T&&>));

  BOOST_TEST((
    visit(get_addresses{},v,w)==get_addresses::result_type{&x,&y}));
  BOOST_TEST((
    visit(get_cref_qualifiers{},v,cv)==
    get_cref_qualifiers::result_type{plain_ref,const_ref}));
  BOOST_TEST((
    visit(get_cref_qualifiers{},std::move(v),std::move(cv))==
    get_cref_qualifiers::result_type{rvalue_ref,const_rvalue_ref}));

  BOOST_TEST(holds_alternative<T>(cv));
  BOOST_TEST(!holds_alternative<Q>(cv));

  BOOST_TEST(&get<I>(v)==&x);
  BOOST_TEST(&get<I>(cv)==&x);
  BOOST_TEST(get<I>(std::move(v))==x);
  BOOST_TEST(get<I>(std::move(cv))==x);
  check_throw<bad_variant_access>([&]{(void)get<J>(v);});

  BOOST_TEST(&get<T>(v)==&x);
  BOOST_TEST(&get<T>(cv)==&x);
  BOOST_TEST(get<T>(std::move(v))==x);
  BOOST_TEST(get<T>(std::move(cv))==x);
  check_throw<bad_variant_access>([&]{(void)get<Q>(v);});

  BOOST_TEST(get_if<I>((V*)nullptr)==nullptr);
  BOOST_TEST(get_if<I>(&v)==&x);
  BOOST_TEST(get_if<I>(&cv)==&x);
  BOOST_TEST(get_if<J>(&v)==nullptr);

  BOOST_TEST(get_if<T>((V*)nullptr)==nullptr);
  BOOST_TEST(get_if<T>(&v)==&x);
  BOOST_TEST(get_if<T>(&cv)==&x);
  BOOST_TEST(get_if<Q>(&v)==nullptr);

  BOOST_TEST_TRAIT_TRUE((
    std::is_same<decltype(get<I>(v)),T&>));
  BOOST_TEST_TRAIT_TRUE((
    std::is_same<decltype(get<I>(cv)),const T&>));
  BOOST_TEST_TRAIT_TRUE((
    std::is_same<decltype(get<I>(std::move(v))),T&&>));
  BOOST_TEST_TRAIT_TRUE((
    std::is_same<decltype(get<I>(std::move(cv))),const T&&>));

  BOOST_TEST_TRAIT_TRUE((
    std::is_same<decltype(get<T>(v)),T&>));
  BOOST_TEST_TRAIT_TRUE((
    std::is_same<decltype(get<T>(cv)),const T&>));
  BOOST_TEST_TRAIT_TRUE((
    std::is_same<decltype(std::move(get<T>(v))),T&&>));
  BOOST_TEST_TRAIT_TRUE((
    std::is_same<decltype(std::move(get<T>(cv))),const T&&>));

  BOOST_TEST_TRAIT_TRUE((
    std::is_same<decltype(get_if<I>(&v)),T*>));
  BOOST_TEST_TRAIT_TRUE((
    std::is_same<decltype(get_if<I>(&cv)),const T*>));

  BOOST_TEST_TRAIT_TRUE((
    std::is_same<decltype(get_if<T>(&v)),T*>));
  BOOST_TEST_TRAIT_TRUE((
    std::is_same<decltype(get_if<T>(&cv)),const T*>));
}

template<typename... Ts>
void test_reference_variant()
{
  using variant_type=boost::poly_collection::reference_variant_impl::
    reference_variant<Ts...>;

  do_((test_reference_variant_for<variant_type,Ts>(),0)...);
}

void test_reference_variant()
{
  test_reference_variant<int,char,non_copyable>();
}
