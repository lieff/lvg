// makeswf -v 7 -r 1 -o test-7.swf test.as

function trace_depths () {
  trace ("this: " + this.getDepth ());
  trace ("a: " + a.getDepth ());
  trace ("b: " + b.getDepth ());
  trace ("c: " + c.getDepth ());
}

this.createEmptyMovieClip ("a", 1);
this.createEmptyMovieClip ("b", 2);
this.createEmptyMovieClip ("c", 3);

trace_depths ();

a.swapDepths (3);

trace_depths ();

b.swapDepths (a);

trace_depths ();

c.swapDepths (c);

trace_depths ();

this.swapDepths (2);

trace_depths ();

a.swapDepths ();

trace_depths ();

loadMovie ("FSCommand:quit", "");
