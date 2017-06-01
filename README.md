# Boost PolyCollection library

develop branch &nbsp;&nbsp; [![Build Status](https://travis-ci.org/boostorg/poly_collection.svg?branch=develop)](https://travis-ci.org/boostorg/poly_collection) | master branch &nbsp;&nbsp; [![Build Status](https://travis-ci.org/boostorg/poly_collection.svg?branch=master)](https://travis-ci.org/boostorg/poly_collection)
-|-

**Warning:** Boost.PolyCollection has been accepted into Boost but
it is yet to be released as part of an official Boost distribution.

**Boost.PolyCollection**: fast containers of polymorphic objects.

[Online docs](http://rawgit.com/boostorg/poly_collection/website/doc/html/index.html)  
[Seminal article at bannalia.blogspot.com](http://bannalia.blogspot.com/2014/05/fast-polymorphic-collections.html)

Typically, polymorphic objects cannot be stored *directly* in regular containers
and need be accessed through an indirection pointer, which introduces performance
problems related to CPU caching and branch prediction. Boost.PolyCollection
implements a
[novel data structure](http://rawgit.com/boostorg/poly_collection/website/doc/html/poly_collection/an_efficient_polymorphic_data_st.html)
that is able to contiguously store polymorphic objects without such indirection,
thus providing a value-semantics user interface and better performance.
Three *polymorphic collections* are provided:

* [`boost::base_collection`](http://rawgit.com/boostorg/poly_collection/website/doc/html/poly_collection/tutorial.html#poly_collection.tutorial.basics.boost_base_collection) 
* [`boost::function_collection`](http://rawgit.com/boostorg/poly_collection/website/doc/html/poly_collection/tutorial.html#poly_collection.tutorial.basics.boost_function_collection)
* [`boost::any_collection`](http://rawgit.com/boostorg/poly_collection/website/doc/html/poly_collection/tutorial.html#poly_collection.tutorial.basics.boost_any_collection)

dealing respectively with classic base/derived or OOP polymorphism, function wrapping
in the spirit of `std::function` and so-called
[*duck typing*](https://en.wikipedia.org/wiki/Duck_typing) as implemented by
[Boost.TypeErasure](http://www.boost.org/libs/type_erasure).

## Requirements

* A compiler with C++11 support. The library has been tested with Visual Studio
2015,  GCC 5.2.1 and Clang 3.7.
* A reasonably recent version of Boost.
