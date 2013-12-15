chaos3d
=======

### Memory Management
* Objects that're derived from ReferencedCount are subject to retain/release/autorelease mode. Methods of Creation/Copy should mention its liabilities if it's not common
* Pointers should be wrapped by shared\_ptr
* General objects should be copyable
* Objects passed to/returned from Lua will have policies to determine how the life cycle will come across the bordary.

### Layer
* io
* render, sg, asset, go
* application