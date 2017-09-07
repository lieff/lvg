// makeswf -v 7 -s 200x150 -r 1 -o destroyed-movie-resolve2.swf destroyed-movie-resolve2.as

x = createEmptyMovieClip ("a", 0);
x.answer = 42;
createEmptyMovieClip ("a", 0);
trace (x.answer);

getURL ("fscommand:quit", "");
