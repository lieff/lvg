// makeswf -v 7 -s 200x150 -r 1 -o duplicate-names.swf duplicate-names.as

createEmptyMovieClip ("foo", 10);
trace (foo.getDepth ());
createEmptyMovieClip ("foo", 0);
trace (foo.getDepth ());
createEmptyMovieClip ("foo", 20);
trace (foo.getDepth ());
foo.removeMovieClip ();
trace (foo.getDepth ());
foo.removeMovieClip ();
trace (foo.getDepth ());

loadMovie ("FSCommand:quit", "");
