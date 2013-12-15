chaos3d
=======

### Memory Management
* Returning pointers will give up the owership unless specified otherwise
* Objects crossing boundaries should use shared\_ptr if not referenced counting

### Layer
* io
* render, sg, asset, go
* application