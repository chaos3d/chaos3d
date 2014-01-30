chaos3d
=======

### Memory Management
* Use unique\_ptr and shared\_ptr wherever possible  
  unique\_ptr\<referenced\_count\> uses referenced\_count::release\_deleter
* Returning pointers shall be prevented  
  it will however not release the ownership; it might be only available within the caller's scope
* Use move semantics to pass pointers; const pointers (T \*const) can be used, indicating the callee will not be able to take over the onwership, that is, (T \*) is *disallowed*.
* Reference-counted objects should use unique\_ptr instead of shared\_ptr; because of C++'s RAII, it naturally solves auto-releasing issue
* Objects shall define type _ptr_ and const\_ptr

### Layer
* io
* render, sg, asset, go
* application

### Resource Management
* Resources are expensive data loading from IO to memories while keeping lightweight meta-data
* Abstraction:  
actual data type (texture, audio, font, etc.) ==>  
resource wrapper (load async/sync, unload) ==>  
loader (decoding the IO format to data format, i.e. png => RGBA) ==>  
locator (mapping resource id to IO, this could also load all resources in the same batch)
* Manager:  
different strategies (framed-base, zone-based, shared\_ptr)  
purge/unload  
threading
* Locator  
dir/package locator  
texture locator would wrap another locator and produce different results depending on the device
