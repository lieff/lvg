// makeswf -v 7 -s 200x150 -r 1 -o dangling-print.swf dangling-print.as

x = createEmptyMovieClip ("b", 1);
b.removeMovieClip ();
trace (x);
loadMovie ("fscommand:quit", "");
