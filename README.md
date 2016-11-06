# (Candidate) Boost PolyCollection library

**Warning:** Currently, Boost.PolyCollection is **not** part of the Boost
libraries: its naming and structure conform to the requirements of Boost as
the intention is to eventually submit it for formal review and acceptance
into the project. 

**Boost.PolyCollection**: fast containers of polymorphic objects.

[Online docs](http://rawgit.com/joaquintides/poly_collection/website/doc/html/index.html)  
[Seminal article at bannalia.blogspot.com](http://bannalia.blogspot.com/2014/05/fast-polymorphic-collections.html)

Typically, polymorphic objects cannot be stored *directly* in regular containers
and need be accessed through an indirection pointer, which introduces performance
problems related to CPU caching and branch prediction. Boost.PolyCollection
implements a
[novel data structure](http://rawgit.com/joaquintides/poly_collection/website/doc/html/poly_collection/an_efficient_polymorphic_data_st.html)
that is able to contiguously store polymorphic objects without such indirection,
thus providing a value-semantics user interface and better performance.
Three *polymorphic collections* are provided:

* [`boost::base_collection`](http://rawgit.com/joaquintides/poly_collection/website/doc/html/poly_collection/tutorial.html#poly_collection.tutorial.basics.boost_base_collection) 
* [`boost::function_collection`](http://rawgit.com/joaquintides/poly_collection/website/doc/html/poly_collection/tutorial.html#poly_collection.tutorial.basics.boost_function_collection)
* [`any::base_collection`](http://rawgit.com/joaquintides/poly_collection/website/doc/html/poly_collection/tutorial.html#poly_collection.tutorial.basics.boost_any_collection)

dealing respectively with classic base/derived or OOP polymorphism, function wrapping
in the spirit of `std::function` and so-called
[*duck typing*](https://en.wikipedia.org/wiki/Duck_typing) as implemented by
[Boost.TypeErasure](http://www.boost.org/libs/type_erasure).

## Requirements

* A compiler with C++11 support. The library has been tested with Visual Studio
2015,  GCC 5.2.1 and Clang 3.7.
* A reasonably recent version of Boost.
