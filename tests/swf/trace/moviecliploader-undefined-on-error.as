// makeswf -v 7 -s 200x150 -r 1 -o moviecliploader-undefined-on-error.swf moviecliploader-undefined-on-error.as

l = new MovieClipLoader ();
l.onLoadError = function (m) {
  trace ("I lose");
  trace (m);
  trace (m.b);
  getURL ("fscommand:quit", "");
};
createEmptyMovieClip ("a", 0);
a.b = 42;
trace (l.loadClip ("http://example.com/maynotaccess.swf", "a"));
removeMovieClip (a);
