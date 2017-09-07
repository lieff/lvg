// makeswf -v 7 -r 1 -o settarget-7.swf settarget.as

this.test = "this";
var o = new Object ();
o.test = "o";
var a = this.createEmptyMovieClip ("a", this.getNextHighestDepth ());
a.test = "a";
var b = a.createEmptyMovieClip ("b", a.getNextHighestDepth ());
b.test = "b";

// SetTarget2 action
trace (test);
setTarget (a);
trace (test);
setTarget (o);
trace (test);
setTarget (b);
trace (test);
setTarget (this);
trace (test);

// SetTarget action
trace (test);
setTarget ("/a");
trace (test);
setTarget ("/a/b");
trace (test);
setTarget ("o");
trace (test);
setTarget ("_level0.a");
trace (test);
setTarget ("");
trace (test);

loadMovie ("FSCommand:quit", "");
