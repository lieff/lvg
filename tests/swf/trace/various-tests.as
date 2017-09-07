// makeswf -v 7 -s 200x150 -r 1 -o various-tests.swf various-tests.as

/* check if ASBroadcaster requires an Array in _listeners */
o = {};
AsBroadcaster.initialize (o);
o._listeners = this;
this.length = 1;
this[0] = { foo: function () { trace ("foo"); } };
o.broadcastMessage ("foo");


/* check if Transform works on dead matrices */
createEmptyMovieClip ("a", 0);
no = a;
a.removeMovieClip ();
t = new flash.geom.Transform (no);
trace (t.matrix);
createEmptyMovieClip ("a", 0);
trace (no);
trace (t.matrix);
t = new flash.geom.Transform (no);
trace (t.matrix);
delete t;


/* check if objects are equal that return the same thing from valueOf */
x = { valueOf: function () { return o; } };
y = { valueOf: function () { return o; } };
trace (x == y);


/* check if setInterval works with deleted (and recreated) movies */
createEmptyMovieClip ("a", 1000);
trace (setInterval (no, "quit", 500));
a.removeMovieClip ();
trace (setInterval (no, "quit2", 500));
createEmptyMovieClip ("a", 1000);
a.quit = function () {
  trace ("not quitting, HAHA");
  getURL ("fscommand:quit", "");
};
a.quit2 = function () {
  trace ("quitting");
  getURL ("fscommand:quit", "");
};

createEmptyMovieClip ("b", 1);
b.trace = function () { trace ("hello"); };
trace (setInterval (b, "trace", 500));
b.removeMovieClip ();


/* check if Color.setTransform works with all types of objects */
this.aa = 50;
c = new Color (this);
c.setTransform (this);
trace (_alpha);


/* check what undefined does as argument to Object */
trace (new Object (undefined));
trace (Object (undefined));


/* check when setMember action calls [[ToName]] */
o.toString = function () { trace ("toString"); return "x"; };
asm {
  push "o"
  getvariable
  dup
  push 42
  setmember
};
trace (o.x);
asm {
  push 42, "o"
  getvariable
  push 42
  setmember
};


/* check what deleting the target does */
createEmptyMovieClip ("b", 0);
b.createEmptyMovieClip ("b", 0);
setTarget ("b.b");
trace (_target);
_parent.removeMovieClip ();
trace (_target);


/* check what Number does without args */
trace (new Number ());
trace (Number ());


/* check what object functions are set on upon declaration */
createEmptyMovieClip ("b", 0);
setTarget ("b");
function f () { trace (_target); };
setTarget ("");
f();
a.f();


/* check if native functions get executed when run as constructor */
x = new Array.prototype.push (42);
trace (x[0]);


/* check something with var (what was this supposed to be about?) */
function test () {
  var x = 42;
  trace (x);
}
test ();
trace (x);
