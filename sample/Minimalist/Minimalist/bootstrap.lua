print "hello world2"
print(tostring(jit and jit.version))
print(tostring(coroutine.coco))
coroutine.yield();
-- this is due to a hack in the engine
lyield.resume(function()
local idx = 10
while idx > 0 do
    coroutine.yield();
    print(idx)
    idx = idx - 1
end
end)