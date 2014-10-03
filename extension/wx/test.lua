local c3d = require "chaos3d"
local wxc3d = require "chaos3d.wx"
local device = wxc3d.create_device
local context = device:create_context"gles2"
device:create_wxwindow(parent,size);

wxc3d.set_loop(function()
end)
