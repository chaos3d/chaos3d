--require "wx"
local c3d = require "chaos3d"
local launcher = c3d.init();

local win = launcher:create_game_window("chaos3d", {0,0,1024,768});
local rd = launcher:get_render_device(2);
local rwin = rd:new_window(win, {0,0,1024,768}, 2);
local context = rd:new_context(rwin);
context:set_current();

c3d.init_mgr(rd, context);

local c = c3d.go.new(c3d.go.root)
c:set_tag"camera"
c = c:add_camera2d()
c:set_target(rwin):set_viewport_from_target()

--collectgarbage();
while true do
    launcher:poll_event();
end
--[[
require "calculator"
require "chaos3d.wx"

print "hello"
print(c3d, c3d.get_sprite_mgr(), c3d.go.root)
print(c3d.blend, blend)
-- Call wx.wxGetApp():MainLoop() last to start the wxWidgets event loop,
-- otherwise the wxLua program will exit immediately.
-- Does nothing if running from wxLua, wxLuaFreeze, or wxLuaEdit since the
-- MainLoop is already running or will be started by the C++ program.
wx.wxGetApp():MainLoop()

os.exit()
--]]