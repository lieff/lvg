// makeswf -v 7 -s 200x150 -r 1 -o delete-object.swf delete-object.as

trace ("Check that deleting from an object does not delete from the prototype");

o = { x: 1 };
o.__proto__ = { x: 2 };
trace (o.x);
delete o.x;
trace (o.x);
delete o.x;
trace (o.x);

loadMovie ("FSCommand:quit", "");
