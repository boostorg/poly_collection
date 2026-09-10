/* Copyright 2026 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/poly_collection for library home page.
 */

/* Demostration of unorderedness and stability for unordered collections
 * (exemplified with boost::base_collection). 
 */

#include <algorithm>
#include <boost/poly_collection/base_unordered_collection.hpp>
#include "rolegame.hpp"

int main()
{
//[unordered_1
//=  #include <boost/poly_collection/base_unordered_collection.hpp>
//=  ...
//=
  boost::base_unordered_collection<sprite> c; // note the "unordered"

  c.insert(warrior{0});
  c.insert(warrior{1});
  auto it=c.insert(warrior{2}); // keep an iterator to warrior #2
  c.erase(c.begin<warrior>());  // erase the first warrior
  c.insert(warrior{3});
//]

  auto render=[&](){
    const char* comma="";
    for(const sprite& s:c){
      std::cout<<comma;
      s.render(std::cout);
      comma=",";
    }
    std::cout<<"\n";
  };
  render();

//[unordered_2
  // the iterator pointed to warrior #2 *before* the erase operation,
  // but this works and correctly prints "warrior 2"!
  it->render(std::cout);
//]
  std::cout<<"\n";
}
