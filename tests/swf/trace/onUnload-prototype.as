// makeswf -v 7 -s 200x150 -r 1 -o onUnload-prototype.swf onUnload-prototype.as

trace ("check onUnload even gets called when only defined on the prototype.");

MovieClip.prototype.onUnload = function () { trace ("unload " + this); };
createEmptyMovieClip ("a", 0);
a.removeMovieClip ();
trace ("done with " + a);
delete MovieClip.prototype.onUnload;
createEmptyMovieClip ("b", 0);
b.removeMovieClip ();
trace ("done with " + b);
MovieClip.prototype.onUnload = 42;
createEmptyMovieClip ("c", 0);
c.removeMovieClip ();
trace ("done with " + c);
loadMovie ("fscommand:QUIT", "");
