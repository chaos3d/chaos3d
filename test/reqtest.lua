return function()
print "in require";

coroutine.yield(function()
	print "cross c yield";
	return true;
end);

print "resume";
end
