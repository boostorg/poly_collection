/* Copyright 2016 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

#ifndef BOOST_POLY_COLLECTION_DETAIL_POLY_COLLECTION_HPP
#define BOOST_POLY_COLLECTION_DETAIL_POLY_COLLECTION_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <algorithm>
#include <boost/assert.hpp>
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/poly_collection/detail/iterator_impl.hpp>
#include <boost/poly_collection/detail/is_acceptable.hpp>
#include <boost/poly_collection/detail/is_constructible.hpp>
#include <boost/poly_collection/detail/is_final.hpp>
#include <boost/poly_collection/detail/segment.hpp>
#include <boost/poly_collection/exception.hpp>
#include <iterator>
#include <limits>
#include <new>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace boost{

namespace poly_collection{

namespace detail{

/* common implementation for all polymorphic collections */

template<typename Model,typename Allocator>
class poly_collection
{
  template<typename T>
  static const std::type_info& subtypeid(const T& x)
    {return Model::subtypeid(x);}
  template<typename...>
  struct for_all_types{using type=void*;};
  template<typename... T>
  using for_all=typename for_all_types<T...>::type;
  template<typename T>
  struct is_subtype: /* using makes VS2015 choke, hence we derive */
    Model::template is_subtype<typename std::decay<T>::type>{};
  template<typename T>
  using enable_if_subtype=
    typename std::enable_if<is_subtype<T>::value>::type*;
  template<typename T>
  using enable_if_not_subtype=
    typename std::enable_if<!is_subtype<T>::value>::type*;
  template<typename T>
  using is_acceptable=detail::is_acceptable<T,Model>;
  template<typename T>
  using enable_if_acceptable=
    typename std::enable_if<is_acceptable<T>::value>::type*;
  template<typename T>
  using enable_if_not_acceptable=
    typename std::enable_if<!is_acceptable<T>::value>::type*;
  template<typename InputIterator>
  using enable_if_derefs_to_subtype=enable_if_subtype<
    typename std::iterator_traits<InputIterator>::value_type
  >;
  template<typename T>
  using is_terminal=
    typename Model::template is_terminal<typename std::decay<T>::type>;
  template<typename T>
  using enable_if_terminal=
    typename std::enable_if<is_terminal<T>::value>::type*;
  template<typename T>
  using enable_if_not_terminal=
    typename std::enable_if<!is_terminal<T>::value>::type*;
  template<typename InputIterator>
  using derefs_to_terminal=is_terminal< 
    typename std::iterator_traits<InputIterator>::value_type
  >;
  template<typename InputIterator>
  using enable_if_derefs_to_terminal=
    typename std::enable_if<derefs_to_terminal<InputIterator>::value>::type*;
  template<typename InputIterator>
  using enable_if_derefs_to_not_terminal=
    typename std::enable_if<!derefs_to_terminal<InputIterator>::value>::type*;
  template<typename T,typename U>
  using enable_if_not_same=typename std::enable_if<
    !std::is_same<
      typename std::decay<T>::type,typename std::decay<U>::type
    >::value
  >::type*;
  template<typename T,typename U>
  using enable_if_constructible=
    typename std::enable_if<is_constructible<T,U>::value>::type*;
  template<typename T,typename U>
  using enable_if_not_constructible=
    typename std::enable_if<!is_constructible<T,U>::value>::type*;

  using segment_type=detail::segment<Model,Allocator>;
  using segment_base_iterator=typename segment_type::base_iterator;
  using const_segment_base_iterator=
    typename segment_type::const_base_iterator;
  using segment_base_sentinel=typename segment_type::base_sentinel;
  using const_segment_base_sentinel=
    typename segment_type::const_base_sentinel;
  template<typename T>
  using segment_iterator=typename segment_type::template iterator<T>;
  template<typename T>
  using const_segment_iterator=
    typename segment_type::template const_iterator<T>;
  using segment_map=std::unordered_map<
    std::type_index,segment_type,
    std::hash<std::type_index>,std::equal_to<std::type_index>,
    typename std::allocator_traits<Allocator>::template
      rebind_alloc<std::pair<const std::type_index,segment_type>
    >
  >;
  using segment_map_allocator_type=typename segment_map::allocator_type;
  using segment_map_iterator=typename segment_map::iterator;
  using const_segment_map_iterator=typename segment_map::const_iterator;

public:
  using value_type=typename segment_type::value_type;
  using allocator_type=Allocator;
  using size_type=std::size_t;
  using difference_type=std::ptrdiff_t;
  using reference=value_type&;
  using const_reference=const value_type&;
  using pointer=typename std::allocator_traits<Allocator>::pointer;
  using const_pointer=typename std::allocator_traits<Allocator>::const_pointer;

private:
  template<typename,bool>
  friend class detail::iterator_impl;
  template<typename,typename>
  friend class detail::local_iterator_impl;
  template<bool Const>
  using iterator_impl=detail::iterator_impl<poly_collection,Const>;
  template<typename BaseIterator>
  using local_iterator_impl=
    detail::local_iterator_impl<poly_collection,BaseIterator>;

public:
  using iterator=iterator_impl<false>;
  using const_iterator=iterator_impl<true>;
  using local_base_iterator=local_iterator_impl<segment_base_iterator>;
  using const_local_base_iterator=
    local_iterator_impl<const_segment_base_iterator>;
  template<typename T>
  using local_iterator=local_iterator_impl<segment_iterator<T>>;
  template<typename T>
  using const_local_iterator=local_iterator_impl<const_segment_iterator<T>>;

  class const_segment_info
  {
  public:
    const_segment_info(const const_segment_info&)=default;
    const_segment_info& operator=(const const_segment_info&)=default;
    
    const_local_base_iterator begin()const noexcept
      {return {it,it->second.begin()};}
    const_local_base_iterator end()const noexcept
      {return {it,it->second.end()};}
    const_local_base_iterator cbegin()const noexcept{return begin();}
    const_local_base_iterator cend()const noexcept{return end();}

    template<typename T>
    const_local_iterator<T> begin()const noexcept
      {return const_local_iterator<T>{begin()};}
    template<typename T>
    const_local_iterator<T> end()const noexcept
      {return const_local_iterator<T>{end()};}
    template<typename T>
    const_local_iterator<T> cbegin()const noexcept{return begin<T>();}
    template<typename T>
    const_local_iterator<T> cend()const noexcept{return end<T>();}

    std::type_index type_index()const{return it->first;}

  protected:
    friend class poly_collection;

    const_segment_info(const_segment_map_iterator it)noexcept:it{it}{}

    const_segment_map_iterator it;
  };

  class segment_info:public const_segment_info
  {
  public:
    segment_info(const segment_info&)=default;
    segment_info& operator=(const segment_info&)=default;

    using const_segment_info::begin;
    using const_segment_info::end;

    local_base_iterator begin()noexcept
      {return {this->it,this->it->second.begin()};}
    local_base_iterator end()noexcept
      {return {this->it,this->it->second.end()};}

    template<typename T>
    local_iterator<T> begin()noexcept{return local_iterator<T>{begin()};}
    template<typename T>
    local_iterator<T> end()noexcept{return local_iterator<T>{end()};}

  private:
    friend class poly_collection;

    using const_segment_info::const_segment_info;
  };

private:
  template<typename SegmentInfo>
  class segment_info_iterator_impl:
    public boost::iterator_adaptor<
      segment_info_iterator_impl<SegmentInfo>,
      const_segment_map_iterator,
      SegmentInfo,
      std::input_iterator_tag,
      SegmentInfo
    >
  {
    segment_info_iterator_impl(const_segment_map_iterator it):
      segment_info_iterator_impl::iterator_adaptor_{it}{}

  public:
    segment_info_iterator_impl()=default;
    segment_info_iterator_impl(const segment_info_iterator_impl&)=default;
    segment_info_iterator_impl& operator=(
      const segment_info_iterator_impl&)=default;

    template<
      typename SegmentInfo2,
      typename std::enable_if<
        std::is_base_of<SegmentInfo,SegmentInfo2>::value
      >::type* =nullptr
    >
    segment_info_iterator_impl(
      const segment_info_iterator_impl<SegmentInfo2>& x):
      segment_info_iterator_impl::iterator_adaptor_{x.base()}{}
      
    template<
      typename SegmentInfo2,
      typename std::enable_if<
        std::is_base_of<SegmentInfo,SegmentInfo2>::value
      >::type* =nullptr
    >
    segment_info_iterator_impl& operator=(
      const segment_info_iterator_impl<SegmentInfo2>& x)
    {
      this->base_reference()=x.base();
      return *this;
    }

  private:
    template<typename>
    friend class segment_info_iterator_impl;
    friend class poly_collection;
    friend class boost::iterator_core_access;
    template<typename>
    friend struct iterator_traits;

    SegmentInfo dereference()const noexcept{return this->base();}
  };

public:
  using segment_info_iterator=
    segment_info_iterator_impl<segment_info>; 
  using const_segment_info_iterator=
    segment_info_iterator_impl<const_segment_info>;
    
private:
  template<typename Iterator>
  static Iterator              nonconst_hlp(Iterator);
  static iterator              nonconst_hlp(const_iterator);
  static local_base_iterator   nonconst_hlp(const_local_base_iterator);
  template<typename T>
  static local_iterator<T>     nonconst_hlp(const_local_iterator<T>);
  static segment_info_iterator nonconst_hlp(const_segment_info_iterator);

  template<typename Iterator>
  using nonconst_version=decltype(nonconst_hlp(std::declval<Iterator>()));

public:
  class const_segment_traversal_info
  {
  public:
    const_segment_traversal_info(const const_segment_traversal_info&)=default;
    const_segment_traversal_info& operator=(
      const const_segment_traversal_info&)=default;
    
    const_segment_info_iterator begin()const noexcept{return pmap->cbegin();}
    const_segment_info_iterator end()const noexcept{return pmap->cend();}
    const_segment_info_iterator cbegin()const noexcept{return begin();}
    const_segment_info_iterator cend()const noexcept{return end();}

  protected:
    friend class poly_collection;

    const_segment_traversal_info(const segment_map& map)noexcept:
      pmap{const_cast<segment_map*>(&map)}{}

    segment_map* pmap;
  };

  class segment_traversal_info:public const_segment_traversal_info
  {
  public:
    segment_traversal_info(const segment_traversal_info&)=default;
    segment_traversal_info& operator=(const segment_traversal_info&)=default;

    using const_segment_traversal_info::begin;
    using const_segment_traversal_info::end;

    segment_info_iterator begin()noexcept{return this->pmap->cbegin();}
    segment_info_iterator end()noexcept{return this->pmap->cend();}

  private:
    friend class poly_collection;

    using const_segment_traversal_info::const_segment_traversal_info;
  };

  /* construct/destroy/copy */

  poly_collection()=default;
  poly_collection(const poly_collection&)=default;
  poly_collection(poly_collection&&)=default;
  explicit poly_collection(const allocator_type& al):map{al}{}
  poly_collection(const poly_collection& x,const allocator_type& al):
    map{x.map,al}{}
  poly_collection(poly_collection&& x,const allocator_type& al):
    map{std::move(x.map),al}{}

  template<typename InputIterator>
  poly_collection(
    InputIterator first,InputIterator last,
    const allocator_type& al=allocator_type{}):
    map{al}
  {
    this->insert(first,last);
  }

  // TODO: what to do with initializer_list?

  poly_collection& operator=(const poly_collection&)=default;
  poly_collection& operator=(poly_collection&&)=default;

  allocator_type get_allocator()const noexcept{return map.get_allocator();}

  template<
    typename... T,
    for_all<enable_if_acceptable<T>...> =nullptr
  >
  void register_types()
  {
    /* http://twitter.com/SeanParent/status/558765089294020609 */

    using seq=int[1+sizeof...(T)];
    (void)seq{
      0,
      (map.insert(
        {typeid(T),segment_type::template make<T>(get_allocator())}),0)...
    };
  }

  bool is_registered(const std::type_index& index)const
  {
    return map.find(index)!=map.end();
  }

  template<typename T,enable_if_acceptable<T> =nullptr>
  bool is_registered()const
  {
    return is_registered(typeid(T));
  }

  iterator       begin()noexcept{return {map.begin(),map.end()};}
  iterator       end()noexcept{return {map.end(),map.end()};}
  const_iterator begin()const noexcept{return {map.begin(),map.end()};}
  const_iterator end()const noexcept{return {map.end(),map.end()};}
  const_iterator cbegin()const noexcept{return begin();}
  const_iterator cend()const noexcept{return end();}

  local_base_iterator begin(const std::type_index& index)
  {
    auto it=get_map_iterator_for(index);
    return {it,segment(it).begin()};
  }

  local_base_iterator end(const std::type_index& index)
  {
    auto it=get_map_iterator_for(index);
    return {it,segment(it).end()};
  }

  const_local_base_iterator begin(const std::type_index& index)const
  {
    auto it=get_map_iterator_for(index);
    return {it,segment(it).begin()};
  }

  const_local_base_iterator end(const std::type_index& index)const
  {
    auto it=get_map_iterator_for(index);
    return {it,segment(it).end()};
  }

  const_local_base_iterator cbegin(const std::type_index& index)const
    {return begin(index);}
  const_local_base_iterator cend(const std::type_index& index)const
    {return end(index);}

  template<typename T,enable_if_acceptable<T> =nullptr>
  local_iterator<T> begin()
  {
    auto it=get_map_iterator_for(typeid(T));
    return {it,segment(it).begin()};
  }

  template<typename T,enable_if_acceptable<T> =nullptr>
  local_iterator<T> end()
  {
    auto it=get_map_iterator_for(typeid(T));
    return {it,segment(it).end()};
  }

  template<typename T,enable_if_acceptable<T> =nullptr>
  const_local_iterator<T> begin()const
  {
    auto it=get_map_iterator_for(typeid(T));
    return {it,segment(it).begin()};
  }

  template<typename T,enable_if_acceptable<T> =nullptr>
  const_local_iterator<T> end()const
  {
    auto it=get_map_iterator_for(typeid(T));
    return {it,segment(it).end()};
  }

  template<typename T,enable_if_acceptable<T> =nullptr>
  const_local_iterator<T> cbegin()const{return begin<T>();}

  template<typename T,enable_if_acceptable<T> =nullptr>
  const_local_iterator<T> cend()const{return end<T>();}

  segment_traversal_info       segment_traversal()noexcept{return map;}
  const_segment_traversal_info segment_traversal()const noexcept{return map;}

  bool empty()const noexcept
  {
    for(const auto& x:map)if(!x.second.empty())return false;
    return true;
  }

  bool empty(const std::type_index& index)const
  {
    return segment(get_map_iterator_for(index)).empty();
  }

  template<typename T,enable_if_acceptable<T> =nullptr>
  bool empty()const
  {
    return empty(typeid(T));
  }

  size_type size()const noexcept
  {
    size_type res=0;
    for(const auto& x:map)res+=x.second.size();
    return res;
  }

  size_type size(const std::type_index& index)const
  {
    return segment(get_map_iterator_for(index)).size();
  }

  template<typename T,enable_if_acceptable<T> =nullptr>
  size_type size()const
  {
    return size(typeid(T));
  }

  size_type max_size()const noexcept
  {
    size_type res=(std::numeric_limits<size_type>::max)();
    for(const auto& x:map)res=(std::min)(res,x.second.max_size());
    return res;
  }

  size_type max_size(const std::type_index& index)const
  {
    return segment(get_map_iterator_for(index)).max_size();
  }

  template<typename T,enable_if_acceptable<T> =nullptr>
  size_type max_size()const
  {
    return max_size(typeid(T));
  }

  size_type capacity()const noexcept
  {
    size_type res=(std::numeric_limits<size_type>::max)();
    for(const auto& x:map)res=(std::min)(res,x.second.capacity());
    return res;
  }

  size_type capacity(const std::type_index& index)const
  {
    return segment(get_map_iterator_for(index)).capacity();
  }

  template<typename T,enable_if_acceptable<T> =nullptr>
  size_type capacity()const
  {
    return capacity(typeid(T));
  }

  void reserve(size_type n)
  {
    for(auto& x:map)x.second.reserve(n);
  }

  void reserve(const std::type_index& index,size_type n)
  {
    segment(get_map_iterator_for(index)).reserve(n);
  }

  template<typename T,enable_if_acceptable<T> =nullptr>
  void reserve(size_type n)
  {
    /* note this creates the segment if it didn't previously exist */

    segment(get_map_iterator_for<T>()).reserve(n);
  }

  void shrink_to_fit()
  {
    for(auto& x:map)x.second.shrink_to_fit();
  }

  void shrink_to_fit(const std::type_index& index)
  {
    segment(get_map_iterator_for(index)).shrink_to_fit();
  }

  template<typename T,enable_if_acceptable<T> =nullptr>
  void shrink_to_fit()
  {
    shrink_to_fit(typeid(T));
  }

  template<typename T,typename... Args,enable_if_acceptable<T> =nullptr>
  iterator emplace(Args&&... args)
  {
    auto it=get_map_iterator_for<T>();
    return {
      it,map.end(),
      emplace_back_impl<T>(segment(it),std::forward<Args>(args)...)
    };
  }

  template<typename T,typename... Args,enable_if_acceptable<T> =nullptr>
  iterator emplace_hint(const_iterator hint,Args&&... args)
  {
    auto  it=get_map_iterator_for<T>();
    return {
      it,map.end(),
      hint.mapit==it? /* hint in segment */
        emplace_impl<T>(
          segment(it),hint.segpos,std::forward<Args>(args)...):
        emplace_back_impl<T>(segment(it),std::forward<Args>(args)...)
    };
  }

  template<typename T,typename... Args,enable_if_acceptable<T> =nullptr>
  local_base_iterator
  emplace_pos(local_base_iterator pos,Args&&... args)
  {
    return emplace_pos<T>(
      const_local_base_iterator{pos},std::forward<Args>(args)...);
  }

  template<typename T,typename... Args,enable_if_acceptable<T> =nullptr>
  local_base_iterator
  emplace_pos(const_local_base_iterator pos,Args&&... args)
  {
    BOOST_ASSERT(pos.type_index()==typeid(T));
    return {
      pos.mapit,
      emplace_impl<T>(pos.segment(),pos.base(),std::forward<Args>(args)...)
    };
  }

  template<typename T,typename... Args>
  local_iterator<T>
  emplace_pos(const_local_iterator<T> pos,Args&&... args)
  {
    return {
      pos.mapit,
      emplace_impl<T>(pos.segment(),pos.base(),std::forward<Args>(args)...)
    };
  }

  template<typename T,typename... Args>
  local_iterator<T>
  emplace_pos(local_iterator<T> pos,Args&&... args)
  {
    return emplace_pos(
      const_local_iterator<T>{pos},std::forward<Args>(args)...);
  }

  template<typename T,enable_if_subtype<T> =nullptr>
  iterator insert(T&& x)
  {
    auto it=get_map_iterator_for(x);
    return {it,map.end(),segment(it).push_back(std::forward<T>(x))};
  }

  template<
    typename T,
    enable_if_not_same<const_iterator,T> =nullptr,
    enable_if_subtype<T> =nullptr
  >
  iterator insert(const_iterator hint,T&& x)
  {
    auto it=get_map_iterator_for(x);
    return {
      it,map.end(),
      hint.mapit==it? /* hint in segment */
        segment(it).insert(hint.segpos,std::forward<T>(x)):
        segment(it).push_back(std::forward<T>(x))
    };
  }

  template<
    typename BaseIterator,typename T,
    enable_if_not_same<local_iterator_impl<BaseIterator>,T> =nullptr,
    enable_if_subtype<T> =nullptr
  >
  nonconst_version<local_iterator_impl<BaseIterator>>
  insert(local_iterator_impl<BaseIterator> pos,T&& x)
  {
    BOOST_ASSERT(pos.type_index()==subtypeid(x));
    return {
      pos.mapit,
      pos.segment().insert(pos.base(),std::forward<T>(x))
    };
  }

  template<
    typename InputIterator,
    enable_if_derefs_to_subtype<InputIterator> =nullptr,
    enable_if_derefs_to_not_terminal<InputIterator> =nullptr
  >
  void insert(InputIterator first,InputIterator last)
  {
    for(;first!=last;++first)insert(*first);
  }

  template<
    typename InputIterator,
    enable_if_derefs_to_subtype<InputIterator> =nullptr,
    enable_if_derefs_to_terminal<InputIterator> =nullptr
  >
  void insert(InputIterator first,InputIterator last) 
  {
    if(first==last)return;

    /* same segment for all (type is terminal) */

    auto& seg=segment(get_map_iterator_for(*first)); 
    do seg.push_back(*first); while(++first!=last);
  }

  template<bool Const>
  void insert(iterator_impl<Const> first,iterator_impl<Const> last)
  {
    for(;first!=last;++first){
      auto& seg=segment(get_map_iterator_for(*first,first.segment()));
      seg.push_back(*first);
    }
  }

  template<typename BaseIterator>
  void insert(
    local_iterator_impl<BaseIterator> first,
    local_iterator_impl<BaseIterator> last)
  {
    if(first==last)return;

    /* same segment for all (iterator is local) */

    auto& seg=segment(get_map_iterator_for(*first,first.segment()));
    do seg.push_back(*first); while(++first!=last);
  }

  template<
    typename InputIterator,
    enable_if_derefs_to_subtype<InputIterator> =nullptr,
    enable_if_derefs_to_not_terminal<InputIterator> =nullptr
  >
  void insert(const_iterator hint,InputIterator first,InputIterator last)
  {
    for(;first!=last;++first){
      auto it=get_map_iterator_for(*first);
      if(hint.mapit==it){ /* hint in segment */
        hint={it,map.end(),segment(it).insert(hint.segpos,*first)};
        ++hint;
      }
      else segment(it).push_back(*first);
    }
  }

  template<
    typename InputIterator,
    enable_if_derefs_to_subtype<InputIterator> =nullptr,
    enable_if_derefs_to_terminal<InputIterator> =nullptr
  >
  void insert(const_iterator hint,InputIterator first,InputIterator last) 
  {
    if(first==last)return;

    /* same segment for all (type is terminal) */

    auto it=get_map_iterator_for(*first);
    auto& seg=segment(it); 
    if(hint.mapit==it){ /* hint in segment */
      do{
        hint={it,map.end(),seg.insert(hint.segpos,*first)};
        ++hint;
      }while(++first!=last);
    }
    else{
      do seg.push_back(*first); while(++first!=last);
    }
  }

  template<bool Const>
  void insert(
    const_iterator hint,iterator_impl<Const> first,iterator_impl<Const> last)
  {
    for(;first!=last;++first){
      auto it=get_map_iterator_for(*first,first.segment());
      if(hint.mapit==it){ /* hint in segment */
        hint={it,map.end(),segment(it).insert(hint.segpos,*first)};
        ++hint;
      }
      else segment(it).push_back(*first);
    }
  }

  template<typename BaseIterator>
  void insert(
    const_iterator hint,
    local_iterator_impl<BaseIterator> first,
    local_iterator_impl<BaseIterator> last)
  {
    if(first==last)return;

    /* same segment for all (iterator is local) */

    auto it=get_map_iterator_for(*first,first.segment());
    auto& seg=segment(it); 
    if(hint.mapit==it){ /* hint in segment */
      do{
        hint={it,map.end(),seg.insert(hint.segpos,*first)};
        ++hint;
      }while(++first!=last);
    }
    else{
      do seg.push_back(*first); while(++first!=last);
    }
  }

  template<
    typename InputIterator,
    enable_if_derefs_to_subtype<InputIterator> =nullptr
  >
  local_base_iterator insert(
    const_local_base_iterator pos,InputIterator first,InputIterator last)
  {
    auto&     seg=pos.segment();
    auto      it=Model::nonconst_iterator(pos.base());
    size_type n=0;

    for(;first!=last;++first){
      BOOST_ASSERT(pos.type_index()==subtypeid(*first));
      it=std::next(seg.insert(it,*first));
      ++n;
    }
    return {pos.mapit,it-n};
  }

  template<typename T,typename InputIterator>
  local_iterator<T> insert(
    const_local_iterator<T> pos,InputIterator first,InputIterator last)
  {
    auto&               seg=pos.segment();
    segment_iterator<T> it=Model::nonconst_iterator(pos.base());
    size_type           n=0;

    for(;first!=last;++first){
      it=std::next(
        static_cast<segment_iterator<T>>(local_insert<T>(seg,it,*first)));
      ++n;
    }
    return {pos.mapit,it-n};
  }

  template<typename T,typename InputIterator>
  local_iterator<T> insert(
    local_iterator<T> pos,InputIterator first,InputIterator last)
  {
    return insert(const_local_iterator<T>{pos},first,last);
  }

  iterator erase(const_iterator pos)
  {
    return {pos.mapit,pos.mapend,pos.segment().erase(pos.segpos)};
  }

  template<typename BaseIterator>
  nonconst_version<local_iterator_impl<BaseIterator>>
  erase(local_iterator_impl<BaseIterator> pos)
  {
    return {pos.mapit,pos.segment().erase(pos.base())};
  }

  iterator erase(const_iterator first, const_iterator last)
  {
    const_segment_map_iterator fseg=first.mapit,
                               lseg=last.mapit,
                               end=first.mapend;

    if(fseg!=lseg){ /* [first,last] spans over more than one segment */
      /* from 1st elem to end of 1st segment */

      segment(fseg).erase_till_end(first.segpos);

      /* entire segments till last one */

      while(++fseg!=lseg)segment(fseg).clear(); 

      /* remaining elements of last segment */

      if(fseg==end){                /* except if at end of container */
        return {end,end};
      }
      else{
        return {fseg,end,segment(fseg).erase_from_begin(last.segpos)};
      }
    }
    else{                   /* range is included in one segment only */
      if(first==last){      /* to avoid segment(fseg) when fseg==end */
        return {fseg,end,first.segpos};
      }
      else{
        return {fseg,end,segment(fseg).erase(first.segpos,last.segpos)};
      }
    }
  }

  template<typename BaseIterator>
  nonconst_version<local_iterator_impl<BaseIterator>>
  erase(
    local_iterator_impl<BaseIterator> first,
    local_iterator_impl<BaseIterator> last)
  {
    BOOST_ASSERT(first.mapit==last.mapit);
    return{
      first.mapit,
      first.segment().erase(first.base(),last.base())
    };
  }

  void clear()noexcept
  {
    for(auto& x:map)x.second.clear();
  }

  void clear(const std::type_index& index)
  {
    segment(get_map_iterator_for(index)).clear();
  }

  template<typename T,enable_if_acceptable<T> =nullptr>
  void clear()
  {
    clear(typeid(T));
  }

  void swap(poly_collection& x){map.swap(x.map);}

private:
  template<typename M,typename A>
  friend bool operator==(
    const poly_collection<M,A>&,const poly_collection<M,A>&);

  template<
    typename T,
    enable_if_acceptable<T> =nullptr,
    enable_if_not_terminal<T> =nullptr
  >
  const_segment_map_iterator get_map_iterator_for(const T& x)
  {
    auto it=map.find(subtypeid(x));
    if(it!=map.end())return it;
    else if(subtypeid(x)!=typeid(T))throw unregistered_type{subtypeid(x)};
    else return map.insert(
      {typeid(T),segment_type::template make<T>(get_allocator())}).first;
  }

  template<
    typename T,
    enable_if_acceptable<T> =nullptr,
    enable_if_terminal<T> =nullptr
  >
  const_segment_map_iterator get_map_iterator_for(const T&)
  {
    auto it=map.find(typeid(T));
    if(it!=map.end())return it;
    else return map.insert(
      {typeid(T),segment_type::template make<T>(get_allocator())}).first;
  }

  template<
    typename T,
    enable_if_not_acceptable<T> =nullptr,
    enable_if_not_terminal<T> =nullptr
  >
  const_segment_map_iterator get_map_iterator_for(const T& x)const
  {
    BOOST_ASSERT(subtypeid(x)!=typeid(T));
    auto it=map.find(subtypeid(x));
    if(it!=map.end())return it;
    else throw unregistered_type{subtypeid(x)};
  }

  template<
    typename T,
    enable_if_not_acceptable<T> =nullptr,
    enable_if_terminal<T> =nullptr
  >
  const_segment_map_iterator get_map_iterator_for(const T&)const
  {
    static_assert(
      is_acceptable<T>::value,
      "type must be move constructible and move assignable");
    return {}; /* never executed */
  }

  template<typename T>
  const_segment_map_iterator get_map_iterator_for(
    const T& x,const segment_type& seg)
  {
    auto it=map.find(subtypeid(x));
    if(it!=map.end())return it;
    else return map.insert(
      {subtypeid(x),segment_type::make_from_prototype(seg)}).first;
  }

  template<typename T>
  const_segment_map_iterator get_map_iterator_for()
  {
    auto it=map.find(typeid(T));
    if(it!=map.end())return it;
    else return map.insert(
      {typeid(T),segment_type::template make<T>(get_allocator())}).first;
  }

  const_segment_map_iterator get_map_iterator_for(const std::type_info& info)
  {
    return const_cast<const poly_collection*>(this)->
      get_map_iterator_for(info);
  }

  const_segment_map_iterator get_map_iterator_for(
    const std::type_info& info)const
  {
    return get_map_iterator_for(std::type_index{info});
  }

  const_segment_map_iterator get_map_iterator_for(
    const std::type_index& index)
  {
    return const_cast<const poly_collection*>(this)->
      get_map_iterator_for(index);
  }

  const_segment_map_iterator get_map_iterator_for(
    const std::type_index& index)const
  {
    auto it=map.find(index);
    if(it!=map.end())return it;
    else throw unregistered_type{index};
  }

  static segment_type& segment(const_segment_map_iterator pos)
  {
    return const_cast<segment_type&>(pos->second);
  }

  template<typename T,typename... Args>
  static segment_base_iterator emplace_back_impl(
    segment_type& seg,Args&&... args)
  {
    /* emplace_function/emplace_thunk technique described at
     * http://bannalia.blogspot.com/2016/07/
     *   passing-capturing-c-lambda-functions-as.html
     */

    auto  emplace_function=[&](void* p)
    {
      ::new (p) T(std::forward<Args>(args)...);
    };
    using emplace_function_t=decltype(emplace_function);
    auto  emplace_thunk=[](void* p,void* arg)
    {
      (*static_cast<emplace_function_t*>(arg))(p);
    };
    return seg.emplace_back(emplace_thunk,&emplace_function);
  }

  template<typename T,typename BaseIterator,typename... Args>
  static segment_base_iterator emplace_impl(
    segment_type& seg,BaseIterator pos,Args&&... args)
  {
    auto  emplace_function=[&](void* p)
    {
      ::new (p) T(std::forward<Args>(args)...);
    };
    using emplace_function_t=decltype(emplace_function);
    auto  emplace_thunk=[](void* p,void* arg)
    {
      (*static_cast<emplace_function_t*>(arg))(p);
    };
    return seg.emplace(pos,emplace_thunk,&emplace_function);
  }

  template<
    typename T,typename BaseIterator,typename U,
    enable_if_subtype<U> =nullptr,
    enable_if_not_constructible<T,U&&> =nullptr
  >
  static segment_base_iterator local_insert(
    segment_type& seg,BaseIterator pos,U&& x)
  {
    BOOST_ASSERT(subtypeid(x)==typeid(T));
    return seg.insert(pos,std::forward<U>(x));
  }

  template<
    typename T,typename BaseIterator,typename U,
    enable_if_subtype<U> =nullptr,
    enable_if_constructible<T,U&&> =nullptr
  >
  static segment_base_iterator local_insert(
    segment_type& seg,BaseIterator pos,U&& x)
  {
    if(subtypeid(x)==typeid(T))return seg.insert(pos,std::forward<U>(x));
    else return emplace_impl<T>(seg,pos,std::forward<U>(x));
  }

  template<
    typename T,typename BaseIterator,typename U,
    enable_if_not_subtype<U> =nullptr,
    enable_if_constructible<T,U&&> =nullptr
  >
  static segment_base_iterator local_insert(
    segment_type& seg,BaseIterator pos,U&& x)
  {
    return emplace_impl<T>(seg,pos,std::forward<U>(x));
  }

  template<
    typename T,typename BaseIterator,typename U,
    enable_if_not_subtype<U> =nullptr,
    enable_if_not_constructible<T,U&&> =nullptr
  >
  static segment_base_iterator local_insert(
    segment_type&,BaseIterator,U&&)
  {
    static_assert(
      is_constructible<T,U&&>::value,
      "element must be constructible from type");
    return {}; /* never executed */
  }

  segment_map map;
};

template<typename Model,typename Allocator>
bool operator==(
  const poly_collection<Model,Allocator>& x,
  const poly_collection<Model,Allocator>& y)
{
  // TODO: can we do better than two passes?
  if(x.size()!=y.size())return false;
  const auto &mapx=x.map,&mapy=y.map;
  for(const auto& p:mapx){
    auto it=mapy.find(p.first);
    if(it==mapy.end()?!p.second.empty():p.second!=it->second)return false;
  }
  return true;
}

template<typename Model,typename Allocator>
bool operator!=(
  const poly_collection<Model,Allocator>& x,
  const poly_collection<Model,Allocator>& y)
{
  return !(x==y);
}

template<typename Model,typename Allocator>
void swap(
  poly_collection<Model,Allocator>& x,poly_collection<Model,Allocator>& y)
{
  x.swap(y);
}

} /* namespace poly_collection::detail */

} /* namespace poly_collection */

} /* namespace boost */

#endif
