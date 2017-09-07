// makeswf -v 7 -s 200x150 -r 1 -o destroyed-movie-resolve.swf destroyed-movie-resolve.as

x = createEmptyMovieClip ("a", 0);
x.answer = 42;

loader = new MovieClipLoader ();
loader.loadClip ("destroyed-movie-resolve.as", x);

loader.onLoadComplete = function (m) {
  trace (x.answer);
  trace (a.answer);
  getURL ("fscommand:quit", "");
};
