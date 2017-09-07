// makeswf -v 7 -s 200x150 -r 1 -o remove-with-onUnload.swf remove-with-onUnload.as

function foo () {
  trace (">> " + this);
  trace (_root.a + ": " + _root.a.getDepth ());
  trace (_root.a.a + ": " + _root.a.a.getDepth ());
  trace (_root.a.b + ": " + _root.a.b.getDepth ());
  trace (_root.a.c + ": " + _root.a.c.getDepth ());
  trace (_root.a.d + ": " + _root.a.d.getDepth ());
};
createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("b", 1);
a.createEmptyMovieClip ("c", 2);
a.createEmptyMovieClip ("d", 3);
a.onUnload = foo;
a.a.onUnload = foo;
a.b.onUnload = foo;
a.c.removeMovieClip ();
a.removeMovieClip ();
a.removeMovieClip ();
a.d.removeMovieClip ();
foo ();
trace ("===");

loadMovie ("fscommand:quit", "");
