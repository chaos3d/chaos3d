chaos3d
=======

### Memory Management
* Returning pointers will give up the ownership unless specified otherwise
* Passing pointers will still have the ownership unless specified otherwise
* Objects crossing boundaries should use shared\_ptr if not referenced counting

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
