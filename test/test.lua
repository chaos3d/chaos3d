local io = require("io")
local http = require("socket.http")
local ltn12 = require("ltn12")

-- connect to server "www.cs.princeton.edu" and retrieves this manual
-- file from "~diego/professional/luasocket/http.html" and print it to stdout
http.request{ 
	url = "http://www.cs.princeton.edu/~diego/professional/luasocket/http.html", 
	sink = ltn12.sink.file(io.stdout)
}

local rt = require "reqtest"
print "test";

rt();
	local time = 1;
local co = coroutine.create(function()
	print "in coroutine";
	coroutine.yield(function()
		time = time + 1;
		print("timer: ", time);
		return time > 30;
	end);
end);

local i = 1;
coroutine.schedule(coroutine.create(function()
	while i < 60 do
		print("intervined: ", i);
		i = i +1;
		coroutine.yield(true);
	end
end));

coroutine.yield(coroutine.schedule(co));
print("time:", time);

repeat

	print "yield test";
	local kick = 1;
	coroutine.yield(true, function() kick = kick +1; print("kicking:", kick) return kick > 10 end);
	--[[
	coroutine.yield(co, function() 
		if(coroutine.status(co) ~= "dead") then coroutine.resume(co); end;
		kick = kick +1; print("kicking:", kick) return kick > 10 
	end);
	--]]
until true;
	
