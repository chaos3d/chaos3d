print "hello world"

-- the following is beyond minimum
require "lyield"

-- this is due to a hack in the engine
lyield.resume(function()
local idx = 10
while idx > 0 do
    coroutine.yield();
    print(idx)
    idx = idx - 1
end
end)