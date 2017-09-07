// makeswf -v 7 -s 200x150 -r 1 -o movie23.swf movie23.as

createEmptyMovieClip ("a", 0);
trace (a._framesloaded);
trace (a._totalframes);
trace (a.getBytesLoaded());
trace (a.getBytesTotal());

loadMovie ("fscommand:quit", "");
