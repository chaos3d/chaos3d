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


-- psudo code
--[[
local screen = device.screen
local window = re.createWindow({screen.width, screen.height})
local camera = scene2d.createNode( { type='camera', target=window } )
local node = scene2d.createNode( {} )

while true do
	camera:doRender()
	coroutine.yield()
end
--]]
