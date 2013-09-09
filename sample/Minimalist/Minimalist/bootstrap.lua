print "hello world2"
print(tostring(jit and jit.version))
coroutine.yield();

-- this is due to a hack in the engine
local co = lyield.resume(function()
local idx = 10
while idx > 0 do
    coroutine.yield();
    print(idx)
    idx = idx - 1
end
end);

coroutine.yield() -- yield a frame to let the co to run
coroutine.yield(co)

print "end";

print "start simple&dirty test"

-- working code

local node = scene2d.create({
    "tag1",
    transform={translate={1,1}},
})

local camera = scene2d.create({
    "camera",
    type="camera",
})

node:addChildren(
    {camera, others}
)

while true do
    --print "render camera";
    camera:doRender();
	coroutine.yield()
end

-- psudo code
--[[
local children = node.children
print(type(children);
for _, child in ipairs(children) do
    print(child)
end
local screen = platform.screen -- full screen? width? height?
local re = platform.renderEngine
local window = re.createWindow(screen.width, screen.height)
local camera = scene2d.createNode( { type='camera', target=window } )
local node = scene2d.createNode( {} )

while true do
	camera:doRender()
	coroutine.yield()
end
--]]
