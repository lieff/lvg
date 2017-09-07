// makeswf -v 7 -s 200x150 -r 1 -o settarget-tostring.swf settarget-tostring.as

a = createEmptyMovieClip ("name", 0);
b = createEmptyMovieClip ("name", 1);
setTarget (b);
trace (getDepth ());

loadMovie ("fscommand:QUIT", "");
