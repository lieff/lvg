// makeswf -v 7 -s 200x150 -r 1 -o settarget2-tostring.swf settarget2-tostring.as

trace ("test that setTarget2 action just converts its argument to a string");

_global.a = { toString: function () { return ""; } };
createEmptyMovieClip ("undefined", 0);
setTarget (undefined);
trace (_target);
setTarget (a);
trace (_target);

loadMovie ("FSCommand:quit", "");
