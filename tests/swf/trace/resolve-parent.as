// makeswf -v 7 -s 200x150 -r 1 -o resolve-parent.swf resolve-parent.as

createEmptyMovieClip ("a", 0);
x = a.createEmptyMovieClip ("a", 0);

a.removeMovieClip ();
createEmptyMovieClip ("a", 0);
trace (x);
a.createEmptyMovieClip ("a", 0);
trace (x);

getURL ("fscommand:quit", "");
