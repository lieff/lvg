// makeswf -v 7 -s 200x150 -r 1 -o remove-child-onUnload.swf remove-child-onUnload.as

createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("a", 0);
a.createEmptyMovieClip ("b", 1);
a.b.onUnload = function () { trace (this); trace (this._parent); };
a.removeMovieClip ();
trace (a + ": " + a.getDepth ());
trace (a.a + ": " + a.a.getDepth ());
trace (a.b + ": " + a.b.getDepth ());
trace ("===");

loadMovie ("fscommand:quit", "");
