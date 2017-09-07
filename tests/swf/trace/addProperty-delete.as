// makeswf -v 7 -s 200x150 -r 1 -o addProperty-delete.swf addProperty-delete.as

trace ("check if deleting a property added with addProperty works");

o = {};
o.addProperty ("x", function () { return 42; }, null);
trace (o.x);
delete o.x;
trace (o.x);

loadMovie ("FSCommand:quit", "");
