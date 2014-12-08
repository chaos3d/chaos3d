chaos3d
=======
[![Build Statue](https://travis-ci.org/chaos3d/chaos3d.svg?branch=master)](https://travis-ci.org/chaos3d/chaos3d)

### Animation
* action, an execution unit that can be a sequence, or a group, of others.
  it has basic endpoints to do the work, i.e. start, end and update. it should maintain the internal state of the execution, only to support forward playing. it should not contain much info and be easily created, stopped, or ditched.
* animator, the holder of actions, the conceptual entity that can exist in various forms.
  it is the controller to start the action, and control and transist the actions. right now, there is no common animators because the use cases vary. only action component is one kind of animator.
  there could be skeleton animator, ui animator, etc. and maybe the common functions will be moved up

### Memory Management
* Use unique\_ptr and shared\_ptr, deprecating referenced\_count
  unique\_ptr: used for only one instance to exist at any time
  shared\_ptr: used for shared scoping instance
* Returning raw pointers is OK but it's not for releasing ownership
* Use move semantics to pass pointers; const pointers (T \*const) can be used, indicating the callee will not be able to take over the onwership, that is, (T \*) is *disallowed*.
* Objects shall define type _ptr_ and const\_ptr
* Class inheriting from enable\_shared\_ptr\_from\_this should hide the contructors but provide alternative helper functions to create objects, at least when exposing API's, for consistency

### Object 
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
