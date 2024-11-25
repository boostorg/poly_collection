/* Copyright 2024 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_TEST_VARIANT_TYPES_HPP
#define BOOST_POLY_COLLECTION_TEST_VARIANT_TYPES_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/poly_collection/variant_collection.hpp>
#include <string>

namespace variant_types{

using alternative1=int;

using alternative2=long int;

struct alternative3
{
  alternative3(int n):n{n}{}
  alternative3(alternative3&&)=default;
  alternative3& operator=(alternative3&&)=delete;
  int operator()(int x)const{return x*n;}
  bool operator==(const alternative3& x)const{return n==x.n;}
  int n;
};

struct alternative4:alternative3
{
  using alternative3::alternative3;
};

struct alternative5
{
  alternative5(int n):str{std::to_string(n)}{}
  alternative5(alternative5&&)=default;
  alternative5& operator=(alternative5&&)=delete;
  int operator()(int x)const{return x*std::stoi(str);}
  std::string str;
};

using collection=boost::variant_collection<
  boost::mp11::mp_list<
    alternative1,alternative2,alternative3,alternative4,alternative5
  >
>;

using value_type=collection::value_type;

using t1=alternative1;
using t2=alternative2;
using t3=alternative3;
using t4=alternative4;
using t5=alternative5;

/* In order to make algorithm tests work, make collection::value_type (whose
 * default operator== is ill-formed) equality comparable and enable inter-type
 * equality comparison for all the types involved.
 */

struct operator_eq_visitor
{
  template<typename T,typename Q>
  bool operator()(const T&,const Q&)const{return false;}

  bool operator()(const t1& x,const t1& y)const{return x==y;}
  bool operator()(const t2& x,const t2& y)const{return x==y;}
  bool operator()(const t3& x,const t3& y)const{return x==y;}
  bool operator()(const t4& x,const t4& y)const{return x==y;}
};

inline bool operator==(const value_type& x,const value_type& y)
{
  return boost::poly_collection::visit(operator_eq_visitor{},x,y);
}

inline bool operator==(const value_type& x,const t1& y)
{
  auto p=boost::poly_collection::get_if<t1>(&x);
  return p?*p==y:false;
}

inline bool operator==(const t1& x,const value_type& y){return y==x;}

inline bool operator==(const value_type& x,const t2& y)
{
  auto p=boost::poly_collection::get_if<t2>(&x);
  return p?*p==y:false;
}

inline bool operator==(const t2& x,const value_type& y){return y==x;}

inline bool operator==(const value_type& x,const t3& y)
{
  auto p=boost::poly_collection::get_if<t3>(&x);
  return p?*p==y:false;
}

inline bool operator==(const t3& x,const value_type& y){return y==x;}

inline bool operator==(const value_type& x,const t4& y)
{
  auto p=boost::poly_collection::get_if<t4>(&x);
  return p?*p==y:false;
}

inline bool operator==(const t4& x,const value_type& y){return y==x;}

inline bool operator==(const value_type& x,const t5& y){return false;}
inline bool operator==(const t5& x,const value_type& y){return false;}

inline bool operator==(const t1&,const t3&){return false;}
inline bool operator==(const t1&,const t4&){return false;}
inline bool operator==(const t2&,const t3&){return false;}
inline bool operator==(const t2&,const t4&){return false;}
inline bool operator==(const t3&,const t1&){return false;}
inline bool operator==(const t3&,const t2&){return false;}
inline bool operator==(const t3&,const t4&){return false;}
inline bool operator==(const t4&,const t1&){return false;}
inline bool operator==(const t4&,const t2&){return false;}
inline bool operator==(const t4&,const t3&){return false;}

struct to_int
{
  int operator()(const value_type& x)const
  {
    return boost::poly_collection::visit(*this,x);
  }

  int operator()(const t1& x)const{return x;}
  int operator()(const t2& x)const{return (int)x;}
  int operator()(const t3& x)const{return x.n;}
  int operator()(const t4& x)const{return x.n;}
  int operator()(const t5& x)const{return std::stoi(x.str);}
};

} /* namespace base_types */

#endif
