// makeswf -v 7 -s 200x150 -r 15 -o settarget-remove.swf settarget-remove.as

_global.test = "global";
test = "this";

createEmptyMovieClip ("a", 0);

setTarget("a");
trace (test);

_root.a.test = "a";
trace (test);

removeMovieClip (_root.a);
trace (test);

setTarget("");
trace (test);

getURL ("fscommand:quit", "");
