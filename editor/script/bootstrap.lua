--require "wx"
local c3d = require "chaos3d"
--c3d.create_fixed_timer(30);

local launcher = c3d.init();
local main_window = launcher:create_game_window("chaos3d", {0,0,launcher:size()});
local rd = launcher:get_render_device(2);

local w,h = launcher:size()
local rwin = rd:new_window(main_window, {0,0,w,h}, 2);
c3d.init_mgr(rd, rd:new_context(rwin):set_current());

local asset = c3d.get_asset_mgr();
local sprite_mgr = c3d.get_sprite_mgr();
local locator = c3d.get_locator():add_locator(0, "@app", "/res"):add_locator(0, "@current", "/res");

local atlas1 = c3d.load_atlas(locator:from "turtle.json", asset)
local spineboy = c3d.load_atlas(locator:from "spineboy_atlas.json", asset)

local c = c3d.go.new(c3d.go.root)
c:set_tag"camera"
c:add_camera2d(rwin)
 :set_viewport_from_target()
 :set_perspective(math.rad(60), rwin:aspect_ratio(), .1, 1000.0)
 :move_for_perfect_pixel(960)

local action = c:add_action()

print("width:", rwin:aspect_ratio()*960)

local build_v = function(vsh)
    return rd:new_shader(c3d.shader.Vertex):compile(locator:from (vsh) :read())
end

local build_f = function(fsh)
    return rd:new_shader(c3d.shader.Fragment):compile(locator:from (fsh) :read())
end

sprite_mgr:add_material("basic", rd:new_program():link(sprite_mgr:vertex_layout(0), {
    build_v"test.vsh", build_f"test.fsh"
}), rd:new_state()
---[[
:set {
    src_blend = c3d.blend.BlendOne,
    src_alpha = c3d.blend.BlendOne,
    dst_blend = c3d.blend.BlendOneMinusSrcAlpha,
    dst_alpha = c3d.blend.BlendOneMinusSrcAlpha
}
--]]
, 
rd:new_uniform():set_texture("c_tex1", asset:load_texture("turtle")))

local go1 = c3d.go.new(c3d.go.root)
go1:add_transform():set_translate(0,180)
--local logo = c3d.go.new(go1)
local logo = c3d.go.new(c3d.go.root)
logo:add_transform():set_translate(0,0):set_skew(0,0):set_rotate(0,0,0)--:mark(true)
--logo:add_quad_sprite(atlas1, "action01.png", "basic")
logo:add_quad_sprite(spineboy, "torso", "basic")

local sb = c3d.go.new(c3d.go.root)
sb:add_transform():set_translate(0,-200);
local sbanim = sb:add_animation(locator:from "spineboy.json", {spineboy}, 20)

action:add_sequence({
--launcher:get_action():add_group({
    --[[
    c3d.action.from(function()
        print "abc"
        coroutine.yield(logo:make_skew_action(2, {
            {0, 0, 0, 0},
            {0.25, 30, 0, 0},
            {0.5, 0, 0, 0},
            {0.75, 30, 0, 0},
            {1, 0, 0, 0},
        }))
    end),
    --]]
    --[[
    logo:make_skew_action(5, {
        {0, 0, 0, 0},
        {0.25, 30, 0, 0},
        {0.5, 0, 0, 0},
        {0.75, 30, 0, 0},
        {1, 0, 0, 0},
    }),
    c3d.action.from(function()
        print "here now"
    end),
    --]]
    --[[
    logo:make_rotate_action(5, {
        {0, 0, 0, 0},
        {0.5, 0, 0, 30},
        {1, 0, 0, 0},
    }),
    --]]
    --[[
    logo:make_atlas_action(-1, {
        {0, atlas1, "action01.png"},
        {0.25, atlas1, "action02.png"},
        {0.5, atlas1, "action01.png"},
        {0.75, atlas1, "action02.png"},
        {1, atlas1, "action01.png"},
    }, 0.4),
    --]]
    --c3d.action.wait_time(1),
    --[[
    logo:make_translate_action(5, {
        {0, 0, 0, 0},
        {1, 100, 180, 0},
    }),
    --]]
    c3d.action.wait_frame(10),
    --[[
    logo:make_translate_action(5, {
        {0, 0, 0, 0},
        {0.5, 0, 1, 0},
        {1, 0, -400, 0},
    }),
    --]]
    sbanim:make_action("run"),
});
--local c1 = turtle:add_collider2d():from_quad_sprite(true, collider.shape())
--turtle:set_tag('turtle')

local h = rwin:register(function(evt, t)
    --print(evt:which(), "heelo", evt:where());
    if t == c3d.evt.touch_moved then print("moved")
    elseif t == c3d.evt.touch_began then print "began" end;
end, c3d.evt.touch_began, c3d.evt.touch_moved, c3d.evt.touch_ended);

--collectgarbage();

while true do
    launcher:poll_event();
end
